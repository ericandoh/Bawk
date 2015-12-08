//
//  efros_leung.cpp
//  Bawk
//
//  Created by Eric Oh on 12/5/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "efros_leung.h"
#include "superobject.h"

typedef uint16_t block_value;

struct box_position {
    int x, y, z;
};

struct box_coordinate {
    int xl, yl, zl;
    int xu, yu, zu;
    box_coordinate() {
        xl = yl = zl = xu = yu = zu = 0;
    }
    box_coordinate(int x1, int y1, int z1, int x2, int y2, int z2) {
        xl = x1;
        yl = y1;
        zl = z1;
        xu = x2;
        yu = y2;
        zu = z2;
    }
    box_coordinate(int x, int y, int z, int window_size) {
        int half_window_size = window_size / 2;
        xl = x - half_window_size;
        yl = y - half_window_size;
        zl = z - half_window_size;
        xu = xl + window_size;
        yu = yl + window_size;
        zu = zl + window_size;
    }
    void get_dimensions(int* xd, int* yd, int* zd) {
        *xd = xu - xl;
        *yd = yu - yl;
        *zd = zu - zl;
    }
    void constrain(int lx, int ly, int lz, int ux, int uy, int uz) {
        xl = std::max(xl, lx);
        yl = std::max(yl, ly);
        zl = std::max(zl, lz);
        xu = std::min(xu, ux);
        yu = std::min(yu, uy);
        zu = std::min(zu, uz);
    }
    void add(int off) {
        xl += off;
        yl += off;
        zl += off;
        xu += off;
        yu += off;
        zu += off;
    }
};

int get_class(block_value a) {
    if (a == 0) {
        // air
        return 0;
    }
    else if (1 <= a && a <= 3) {
        // stone
        return 1;
    }
    else if (a == 10 || a == 16 || a == 20) {
        // dirt/sand/grass
        return 2;
    }
    else if (a == 38 || a == 39) {
        // water/lava
        return 3;
    }
    else if (a == 94 || a == 120 || a == 128) {
        // glass/metal/wood
        return 4;
    }
    return 5;
}

float get_difference(block_value a, block_value b) {
    if (a == b) {
        return 0.0f;
    }
    return 1.0f;
    /*
    if (a > b) {
        block_value temp = b;
        b = a;
        a = temp;
    }
    int aclass = get_class(a);
    int bclass = get_class(b);
    if (aclass == bclass) {
        return 0.2f;
    }
    if (aclass == 0) {
        // any block against air, return 1.0f
        return 1.0f;
    }
    if (aclass == 1) {
        if (bclass == 4) {
            // and kindof? similar to other building mats
            return 0.4f;
        }
        return 0.8f;
    }
    else {
        return 0.8f;
    }*/
}

// code heavily modified from
// http://www.programming-techniques.com/2013/02/gaussian-filter-generation-using-cc.html
std::vector<std::vector<std::vector<float>>> create_gaussian_filter(int window_size) {
    std::vector<std::vector<std::vector<float>>> filter;
    filter.resize(window_size);
    for (int x = 0; x < window_size; x++) {
        filter[x].resize(window_size);
        for (int y = 0; y < window_size; y++) {
            filter[x][y].resize(window_size);
        }
    }
    
    // set standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;
    
    // sum is for normalization
    double sum = 0.0;
    
    int half_window = window_size / 2;
    int lower = -half_window;
    int upper = lower + window_size;
    // generate 5x5 kernel
    for (int x = lower; x < upper; x++) {
        for (int y = lower; y < upper; y++) {
            for (int z = lower; z < upper; z++) {
                r = sqrt(x*x + y*y);
                filter[x - lower][y - lower][z - lower] = (exp(-(r*r)/s))/(M_PI * s);
                sum += filter[x - lower][y - lower][z - lower];
            }
        }
    }
    
    // normalize the kernel
    for (int i = 0; i < window_size; ++i) {
        for (int j = 0; j < window_size; ++j) {
            for (int k = 0; k < window_size; ++k) {
                filter[i][j][k] /= sum;
            }
        }
    }
    return filter;
}

void train_and_apply_efros_leung(SuperObject* src, SuperObject* dst,
                                 int xl1, int yl1, int zl1, int xu1, int yu1, int zu1,
                                 int xl2, int yl2, int zl2, int xu2, int yu2, int zu2) {
    printf("Beginning Efros-Leung\n");
    // ----- INIT -----
    int window_size = 7;
    int fill_window_size = 3;
    int half_window_size = window_size / 2;
    std::vector<std::vector<std::vector<float>>> filter = create_gaussian_filter(window_size);
    // package dimensions into a nice box
    box_coordinate src_box(xl1, yl1, zl1, xu1, yu1, zu1);
    box_coordinate dst_box(xl2, yl2, zl2, xu2, yu2, zu2);
    
    int src_xd, src_yd, src_zd;
    src_box.get_dimensions(&src_xd, &src_yd, &src_zd);
    int dst_xd, dst_yd, dst_zd;
    dst_box.get_dimensions(&dst_xd, &dst_yd, &dst_zd);
    
    // ----- INPUT ------
    printf("Copying data\n");
    // copy the data from our source object into a static array for easy access
    block_value src_vals[src_xd][src_yd][src_zd];
    for (int x = 0; x < src_xd; x++) {
        for (int y = 0; y < src_yd; y++) {
            for (int z = 0; z < src_zd; z++) {
                src_vals[x][y][z] = src->get_block_integral(x+src_box.xl,
                                                            y+src_box.yl,
                                                            z+src_box.zl).type;
            }
        }
    }
    
    
    // ----- OUTPUT -----
    // array with counts of neighbors per pixel
    int neighbor_count[dst_xd][dst_yd][dst_zd];
    memset(neighbor_count, 0, sizeof(neighbor_count[0][0][0])*dst_xd*dst_yd*dst_zd);
    
    // whether we have applied Efros-Leung at a pixel
    bool has_value[dst_xd][dst_yd][dst_zd];
    memset(has_value, false, sizeof(has_value[0][0][0])*dst_xd*dst_yd*dst_zd);
    
    // whether we've added this point to our list of search criteria
    bool has_been_queued[dst_xd][dst_yd][dst_zd];
    memset(has_been_queued, false, sizeof(has_been_queued[0][0][0])*dst_xd*dst_yd*dst_zd);
    
    // copy the data from our dst object into a static array for easy access
    block_value dst_vals[dst_xd][dst_yd][dst_zd];
    for (int x = 0; x < dst_xd; x++) {
        for (int y = 0; y < dst_yd; y++) {
            for (int z = 0; z < dst_zd; z++) {
                block_value block = dst->get_block_integral(x+dst_box.xl,
                                                            y+dst_box.yl,
                                                            z+dst_box.zl).type;
                dst_vals[x][y][z] = block;
                if (block) {
                    // we have a non air block!
                    // tell all neighbors around to increment its neighbor count
                    // TODO uncomment this, for now we don't use/count neighbros
                    /*
                    box_coordinate neighbor_box(x, y, z, window_size);
                    neighbor_box.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
                    for (int xt = neighbor_box.xl; xt < neighbor_box.xu; xt++) {
                        for (int yt = neighbor_box.yl; yt < neighbor_box.yu; yt++) {
                            for (int zt = neighbor_box.zl; zt < neighbor_box.zu; zt++) {
                                neighbor_count[xt][yt][zt]++;
                            }
                        }
                    }*/
                    // if we already have a block here, we don't need to apply Efros Leung to extrapolate
                    has_value[x][y][z] = true;
                    // exclude from list of search points when adding
                    has_been_queued[x][y][z] = true;
                    //printf("%d %d %d\n",x,y,z);
                }
            }
        }
    }
    
    printf("Setting initial search points\n");
    // search areas for the algorithm - blocks adjacent to blocks with values in them
    std::vector<ivec3> search_points;
    for (int x = 0; x < dst_xd; x++) {
        for (int y = 0; y < dst_yd; y++) {
            for (int z = 0; z < dst_zd; z++) {
                if (has_value[x][y][z]) {
                    box_coordinate neighbor_box(x, y, z, 3);
                    neighbor_box.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
                    for (int xt = neighbor_box.xl; xt < neighbor_box.xu; xt++) {
                        for (int yt = neighbor_box.yl; yt < neighbor_box.yu; yt++) {
                            for (int zt = neighbor_box.zl; zt < neighbor_box.zu; zt++) {
                                if (has_been_queued[xt][yt][zt]) {
                                    // if we don't need to do calculations, don't add it to list of calculations
                                    continue;
                                }
                                // if not, add it to my search points
                                search_points.push_back(ivec3(xt,yt,zt));
                                //printf("%d %d %d\n",xt,yt,zt);
                                has_been_queued[xt][yt][zt] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (search_points.empty()) {
        // we are building a completely! new box
        // lets start from somewhere arbitrary
        search_points.push_back(ivec3(0,0,0));
        // TODO this will fail since if our result has a big air gap it'll just keep filling air
    }
    
    // this ensures our program will end in finite time regardless of bugs in program
    int max_block_counter = dst_xd * dst_yd * dst_zd;
    int counter = 0;
    std::vector<ivec3> close_to_best;
    std::vector<float> close_to_best_ssds;
    // now go through search areas until our search areas is empty
    printf("Now filling into destination\n");
    int real_block_counter = 0;
    while (search_points.size() > 0 && counter < max_block_counter) {
        
        if (counter % 2 == 0) {
            printf("%f percent done (%d/%d)\n", counter * 100.0f / max_block_counter, counter, max_block_counter);
            printf("Real blocks set:%d\n", real_block_counter);
        }
        
        counter++;
        
        /*
        // find the position with the highest neighbor count (original + Efros processed)
        ivec3 best;
        int best_neighbor_count = -1;
        int neighbors;
        for (auto&i: search_points) {
            neighbors = neighbor_count[i.x][i.y][i.z];
            if (neighbors > best_neighbor_count) {
                best_neighbor_count = neighbors;
                best = i;
            }
        }*/
        
        // choose a neighbor at random...for now
        int choice = rand() % search_points.size();
        ivec3 point = search_points[choice];
        // swap the choice here and with the back
        search_points[choice] = search_points.back();
        search_points.pop_back();
        
        int xc = point.x;
        int yc = point.y;
        int zc = point.z;
        
        //printf("Fill in %d,%d,%d\n",xc,yc,zc);
        
        // find dimensions of our window that tightly encloses all filled pixels
        box_coordinate dst_window(xc, yc, zc, window_size);
        //dst_window.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
        box_coordinate dst_constrained_window;
        dst_constrained_window = dst_window;
        /*dst_constrained_window.xl = dst_window.xu;
        dst_constrained_window.yl = dst_window.yu;
        dst_constrained_window.zl = dst_window.zu;
        dst_constrained_window.xu = dst_window.xl;
        dst_constrained_window.yu = dst_window.yl;
        dst_constrained_window.zu = dst_window.zl;
        for (int y = dst_window.yl; y < dst_window.yu; y++) {
            for (int x = dst_window.xl; x < dst_window.xu; x++) {
                for (int z = dst_window.zl; z < dst_window.zu; z++) {
                    if (has_value[x][y][z]) {
                        //printf("%d %d %d\n",x,y,z);
                        dst_constrained_window.xl = std::min(dst_constrained_window.xl, x);
                        dst_constrained_window.yl = std::min(dst_constrained_window.yl, y);
                        dst_constrained_window.zl = std::min(dst_constrained_window.zl, z);
                        dst_constrained_window.xu = std::max(dst_constrained_window.xu, x + 1);
                        dst_constrained_window.yu = std::max(dst_constrained_window.yu, y + 1);
                        dst_constrained_window.zu = std::max(dst_constrained_window.zu, z + 1);
                    }
                }
            }
        }*/
        
        
        //
        
        // now find the top neighbors for this block from the search neighborhood, using Gaussian weighted SSD
        ivec3 best_in_src;
        // if the SSD is less than 1.3x the value of the smallest SSD, add to this list
        float threshold = 1.3f;
        close_to_best.clear();
        close_to_best_ssds.clear();
        float best_ssd = FLT_MAX;
        float ssd;
        for (int xs = 0; xs < src_xd; xs++) {
            for (int ys = 0; ys < src_yd; ys++) {
                for (int zs = 0; zs < src_zd; zs++) {
                    ssd = 0.0f;
                    // go through our window
                    float total_gauss = 0.0f;
                    for (int xd = dst_constrained_window.xl; xd < dst_constrained_window.xu; xd++) {
                        for (int yd = dst_constrained_window.yl; yd < dst_constrained_window.yu; yd++) {
                            for (int zd = dst_constrained_window.zl; zd < dst_constrained_window.zu; zd++) {
                                bool out_of_bounds = xd < 0 || yd < 0 || zd < 0 ||
                                                     xd >= dst_xd || yd >= dst_yd || zd >= dst_zd;
                                if (has_value[xd][yd][zd] || out_of_bounds) {
                                    // transpose into window offsets with corner aligned at 0,0,0
                                    int xb = xd - xc + half_window_size;
                                    int yb = yd - yc + half_window_size;
                                    int zb = zd - zc + half_window_size;
                                    // transpose xd, yd, zd into offsets on top of xs, ys, zs
                                    int xw = xd - xc + xs;
                                    int yw = yd - yc + ys;
                                    int zw = zd - zc + zs;
                                    // make sure xs, ys, zs in range
                                    block_value from_src, from_dst;
                                    if (xw < 0 || yw < 0 || zw < 0 ||
                                        xw >= src_xd || yw >= src_yd || zw >= src_zd) {
                                        from_src = -1;
                                    }
                                    else {
                                        from_src = src_vals[xw][yw][zw];
                                    }
                                    float gauss_value = filter[xb][yb][zb];
                                    total_gauss += gauss_value;
                                    if (out_of_bounds) {
                                        /*from_dst = dst->get_block_integral(xd+dst_box.xl,
                                                                                    yd+dst_box.yl,
                                                                                    zd+dst_box.zl).type;*/
                                        from_dst = -2;
                                    }
                                    else {
                                        from_dst = dst_vals[xd][yd][zd];
                                    }
                                    float diff = get_difference(from_src, from_dst);
                                    ssd += diff * gauss_value;
                                }
                            }
                        }
                    }
                    if (total_gauss == 0) {
                        printf("Our total gauss was 0 meaning we didnt iterate...\n");
                        continue;
                    }
                    ssd = ssd / total_gauss / total_gauss;
                    if (ssd < best_ssd) {
                        best_in_src = ivec3(xs,ys,zs);
                        best_ssd = ssd;
                    }
                    if (ssd <= threshold * best_ssd + 0.1f) {
                        close_to_best.push_back(ivec3(xs,ys,zs));
                        close_to_best_ssds.push_back(ssd);
                    }
                }
            }
        }
        
        // now eliminate entries that arent actually < 1.3 * best_ssd
        int i = 0;
        while (i < close_to_best.size()) {
            if (close_to_best_ssds[i] > threshold * best_ssd) {
                // this should be removed from the list
                close_to_best[i] = close_to_best.back();
                close_to_best_ssds[i] = close_to_best_ssds.back();
                close_to_best.pop_back();
                close_to_best_ssds.pop_back();
            }
            else {
                i++;
            }
        }
        
        if (close_to_best.size() == 0) {
            printf("No best SSD found???\n");
            continue;
        }
        
        // pick randomly from the close_to_best list above
        int input_choice = rand() % (close_to_best.size());
        ivec3 input_choice_vector = close_to_best[input_choice];
        
        
        // fill in the block at that point, increment/set appropriate variables
        // below will fill one block only
        /*dst_vals[xc][yc][zc] = src_vals[input_choice_vector.x][input_choice_vector.y][input_choice_vector.z];
        has_value[xc][yc][zc] = true;
        
        // iterate through neighbors of this point, add to search_points if not efros applied & not search applied
        box_coordinate neighbor_box(xc, yc, zc, 3);
        neighbor_box.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
        for (int xt = neighbor_box.xl; xt < neighbor_box.xu; xt++) {
            for (int yt = neighbor_box.yl; yt < neighbor_box.yu; yt++) {
                for (int zt = neighbor_box.zl; zt < neighbor_box.zu; zt++) {
                    if (has_been_queued[xt][yt][zt]) {
                        continue;
                    }
                    // if not, add it to my search points
                    search_points.push_back(ivec3(xt,yt,zt));
                    has_been_queued[xt][yt][zt] = true;
                }
            }
        }*/
        
        // has_been_queued[xc][yc][zc] should already be true
        
        // below will fill a window of blocks around our choice
        box_coordinate around_coord(xc, yc, zc, fill_window_size);
        around_coord.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
        for (int xt = around_coord.xl; xt < around_coord.xu; xt++) {
            for (int yt = around_coord.yl; yt < around_coord.yu; yt++) {
                for (int zt = around_coord.zl; zt < around_coord.zu; zt++) {
                    if (!has_value[xt][yt][zt]) {
                        int xoff, yoff, zoff;
                        xoff = xt - xc + input_choice_vector.x;
                        yoff = yt - yc + input_choice_vector.y;
                        zoff = zt - zc + input_choice_vector.z;
                        bool out_of_bounds = xoff < 0 || yoff < 0 || zoff < 0 ||
                            xoff >= src_xd || yoff >= src_yd || zoff >= src_zd;
                        if (!out_of_bounds) {
                            //printf("Setting value %d at %d %d %d\n", src_vals[xoff][yoff][zoff], xt, yt, zt);
                            dst_vals[xt][yt][zt] = src_vals[xoff][yoff][zoff];
                            if (dst_vals[xt][yt][zt]) {
                                real_block_counter++;
                            }
                            has_value[xt][yt][zt] = true;
                            has_been_queued[xt][yt][zt] = true;
                        }
                    }
                }
            }
        }
        around_coord = box_coordinate(xc, yc, zc, fill_window_size + 2);
        around_coord.constrain(0, 0, 0, dst_xd, dst_yd, dst_zd);
        for (int xt = around_coord.xl; xt < around_coord.xu; xt++) {
            for (int yt = around_coord.yl; yt < around_coord.yu; yt++) {
                for (int zt = around_coord.zl; zt < around_coord.zu; zt++) {
                    if (!has_been_queued[xt][yt][zt]) {
                        // if not, add it to my search points
                        search_points.push_back(ivec3(xt,yt,zt));
                        has_been_queued[xt][yt][zt] = true;
                    }
                }
            }
        }
        
        
        //printf("Filled with %d\n", dst_vals[xc][yc][zc]);
        
        // set has_value to true
    }
    printf("Finished, filled in %d/%d points\n", counter, max_block_counter);
    printf("Now setting points back into world\n");
    // set the data from our static array back into our dst object
    for (int x = 0; x < dst_xd; x++) {
        for (int y = 0; y < dst_yd; y++) {
            for (int z = 0; z < dst_zd; z++) {
                block_type block = block_type(dst_vals[x][y][z]);
                dst->set_block_integral(x+dst_box.xl,
                                        y+dst_box.yl,
                                        z+dst_box.zl, block);
            }
        }
    }
    printf("Finished!\n");
}
