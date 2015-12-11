//
//  efros_leung_two.cpp
//  Bawk
//
//  Created by Eric Oh on 12/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "efros_leung_two.h"
#include "superobject.h"

typedef uint16_t block_value;
typedef std::vector<std::vector<std::vector<float>>> three_d_float;
typedef std::vector<std::vector<std::vector<block_value>>> three_d_box;

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

// state variables
struct efros_parameters {
    int window_size;
    int fill_window_size;
    // the gaussian filter
    three_d_float filter;

    SuperObject* src;
    SuperObject* dst;
    
    box_coordinate src_box;
    box_coordinate dst_box;
    
    int src_xd, src_yd, src_zd;
    int dst_xd, dst_yd, dst_zd;

    three_d_box src_data;
    
    three_d_box has_value;
    three_d_box has_been_queued;
    
    std::vector<ivec3> search_points;
    
    int max_block_counter;
    int counter;
};

efros_parameters* current_params = 0;

void allocate_three_d_float(three_d_float &var, int xt, int yt, int zt) {
    var.resize(xt);
    for (int x = 0; x < xt; x++) {
        var[x].resize(yt);
        for (int y = 0; y < yt; y++) {
            var[x][y].resize(zt);
        }
    }
}

void allocate_three_d_box(three_d_box &var, int xt, int yt, int zt) {
    var.resize(xt);
    for (int x = 0; x < xt; x++) {
        var[x].resize(yt);
        for (int y = 0; y < yt; y++) {
            var[x][y].resize(zt);
            for (int z = 0; z < zt; z++) {
                var[x][y][z] = 0;
            }
        }
    }
}

// code heavily modified from
// http://www.programming-techniques.com/2013/02/gaussian-filter-generation-using-cc.html
three_d_float create_gaussian_filter(int window_size) {
    three_d_float filter;
    allocate_three_d_float(filter, window_size, window_size, window_size);
    
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

void setup_efros(SuperObject* src, SuperObject* dst,
                 int xl1, int yl1, int zl1, int xu1, int yu1, int zu1,
                 int xl2, int yl2, int zl2, int xu2, int yu2, int zu2) {
    current_params = new efros_parameters();
    current_params->window_size = 9;
    current_params->fill_window_size = 5;
    current_params->filter = create_gaussian_filter(current_params->window_size);
    
    current_params->src = src;
    current_params->dst = dst;
    
    current_params->src_box = box_coordinate(xl1, yl1, zl1, xu1, yu1, zu1);
    current_params->dst_box = box_coordinate(xl2, yl2, zl2, xu2, yu2, zu2);
    
    current_params->src_box.get_dimensions(&current_params->src_xd, &current_params->src_yd, &current_params->src_zd);
    current_params->dst_box.get_dimensions(&current_params->dst_xd, &current_params->dst_yd, &current_params->dst_zd);
    
    // ----- INPUT ------
    printf("Copying data\n");
    // copy the data from our source object into a static array for easy access
    allocate_three_d_box(current_params->src_data, current_params->src_xd, current_params->src_yd, current_params->src_zd);
    for (int x = 0; x < current_params->src_xd; x++) {
        for (int y = 0; y < current_params->src_yd; y++) {
            for (int z = 0; z < current_params->src_zd; z++) {
                current_params->src_data[x][y][z] = src->get_block_integral(x+current_params->src_box.xl,
                                                                            y+current_params->src_box.yl,
                                                                            z+current_params->src_box.zl).type;
            }
        }
    }
    allocate_three_d_box(current_params->has_value, current_params->dst_xd, current_params->dst_yd, current_params->dst_zd);
    allocate_three_d_box(current_params->has_been_queued, current_params->dst_xd, current_params->dst_yd, current_params->dst_zd);
    
    // mark values that have already been set
    for (int x = 0; x < current_params->dst_xd; x++) {
        for (int y = 0; y < current_params->dst_yd; y++) {
            for (int z = 0; z < current_params->dst_zd; z++) {
                block_value block = dst->get_block_integral(x+current_params->dst_box.xl,
                                                                            y+current_params->dst_box.yl,
                                                                            z+current_params->dst_box.zl).type;
                if (block) {
                    current_params->has_value[x][y][z] = 1;
                    current_params->has_been_queued[x][y][z] = 1;
                }
            }
        }
    }
    printf("Finding initial search points\n");
    current_params->search_points.clear();
    // find the initial set of points to look at
    // these are all points that are some 1 index away from a point that has a value
    for (int x = 0; x < current_params->dst_xd; x++) {
        for (int y = 0; y < current_params->dst_yd; y++) {
            for (int z = 0; z < current_params->dst_zd; z++) {
                if (current_params->has_value[x][y][z]) {
                    box_coordinate neighbor_box(x, y, z, 3);
                    neighbor_box.constrain(0, 0, 0, current_params->dst_xd, current_params->dst_yd, current_params->dst_zd);
                    for (int xt = neighbor_box.xl; xt < neighbor_box.xu; xt++) {
                        for (int yt = neighbor_box.yl; yt < neighbor_box.yu; yt++) {
                            for (int zt = neighbor_box.zl; zt < neighbor_box.zu; zt++) {
                                if (current_params->has_been_queued[xt][yt][zt]) {
                                    // if we don't need to do calculations, don't add it to list of calculations
                                    continue;
                                }
                                // if not, add it to my search points
                                current_params->search_points.push_back(ivec3(xt,yt,zt));
                                // set this to number of neighbors, by calculating around...
                                current_params->has_been_queued[xt][yt][zt] = 1;
                                current_params->dst->set_block_integral(xt+current_params->dst_box.xl,
                                                                        yt+current_params->dst_box.yl,
                                                                        zt+current_params->dst_box.zl, block_type(38));
                            }
                        }
                    }
                }
            }
        }
    }
    if (current_params->search_points.empty()) {
        current_params->search_points.push_back(ivec3(0,0,0));
    }
    printf("%d search points found\n", (int)current_params->search_points.size());
    current_params->max_block_counter = current_params->dst_xd * current_params->dst_yd * current_params->dst_zd;
    current_params->counter = 0;
}

float get_difference(block_value a, block_value b) {
    /*if (a == 37) {
        a = 0;
    }
    if (b == 37) {
        b = 0;
    }*/
    if (a == b) {
        return 0.0f;
    }
    if (a == 0 || b == 0) {
        return 1.0f;
    }
    return 0.8f;
}

bool step_efros() {
    if (current_params->counter >= current_params->max_block_counter) {
        // force end it if we've iterated more than the number of blocks in our dst (impossible)
        printf("Done with algorithm - counter overfill\n");
        return true;
    }
    if (current_params->search_points.empty()) {
        // if we run out of search points, also return
        printf("Done with algorithm - no more search points\n");
        return true;
    }
    if (current_params->counter % 2 == 0) {
        printf("%f percent done (%d/%d)\n", current_params->counter * 100.0f / current_params->max_block_counter,
               current_params->counter, current_params->max_block_counter);
    }
    
    // choose a neighbor at random...for now
    // (we actually want the neighbor with most neighbors)
    
    bool found_choice = false;
    int choice = 0;
    ivec3 point;
    while (!found_choice) {
        if (current_params->search_points.size() == 0) {
            break;
        }
        choice = rand() % current_params->search_points.size();
        point = current_params->search_points[choice];
        if (!current_params->has_value[point.x][point.y][point.z]) {
            found_choice = true;
        }
        current_params->search_points[choice] = current_params->search_points.back();
        current_params->search_points.pop_back();
    }
    
    if (!found_choice) {
        printf("couldn't find a nonfilled point out of choices of neighbors...\n");
        return false;
    }
    
    // swap the choice here and with the back
    current_params->search_points[choice] = current_params->search_points.back();
    current_params->search_points.pop_back();
    
    int xc = point.x;
    int yc = point.y;
    int zc = point.z;
    
    //printf("Fill in %d,%d,%d\n",xc,yc,zc);
    
    // find dimensions of our window that tightly encloses all filled pixels
    box_coordinate dst_window(xc, yc, zc, current_params->window_size);
    // calculate window in dst that we'll be stretching over our src
    three_d_box dst_window_vals;
    allocate_three_d_box(dst_window_vals, current_params->window_size, current_params->window_size, current_params->window_size);
    three_d_box dst_has_vals;
    allocate_three_d_box(dst_has_vals, current_params->window_size, current_params->window_size, current_params->window_size);
    for (int y = dst_window.yl; y < dst_window.yu; y++) {
        for (int x = dst_window.xl; x < dst_window.xu; x++) {
            for (int z = dst_window.zl; z < dst_window.zu; z++) {
                bool out_of_bounds = x < 0 || y < 0 || z < 0 ||
                x >= current_params->dst_xd || y >= current_params->dst_yd || z >= current_params->dst_zd;
                if (out_of_bounds) {
                    // if out of bounds, set has_val to true, and value to outside value
                    dst_has_vals[x - dst_window.xl][y - dst_window.yl][z - dst_window.zl] = 1;
                    dst_window_vals[x - dst_window.xl][y - dst_window.yl][z - dst_window.zl] =
                    current_params->dst->get_block_integral(x+current_params->dst_box.xl,
                                                            y+current_params->dst_box.yl,
                                                            z+current_params->dst_box.zl).type;
                }
                else {
                    // if in bounds, set has_val to true iff has_vals is true
                    if (current_params->has_value[x][y][z]) {
                        dst_has_vals[x - dst_window.xl][y - dst_window.yl][z - dst_window.zl] = 1;
                        dst_window_vals[x - dst_window.xl][y - dst_window.yl][z - dst_window.zl] =
                        current_params->dst->get_block_integral(x+current_params->dst_box.xl,
                                                                y+current_params->dst_box.yl,
                                                                z+current_params->dst_box.zl).type;
                    }
                }
            }
        }
    }
    
    std::vector<ivec3> close_to_best;
    std::vector<float> close_to_best_ssds;
    ivec3 best_in_src;
    // if the SSD is less than 1.3x the value of the smallest SSD, add to this list
    float threshold = 1.3f;
    close_to_best.clear();
    close_to_best_ssds.clear();
    float best_ssd = FLT_MAX;
    float ssd;
    int half_window_size = current_params->window_size / 2;
    // now stretch our box over all possible locations in the source
    for (int xs = 0; xs < current_params->src_xd; xs++) {
        for (int ys = 0; ys < current_params->src_yd; ys++) {
            for (int zs = 0; zs < current_params->src_zd; zs++) {
                ssd = 0.0f;
                // go through our window
                float total_gauss = 0.0f;
                bool skip_this = false;
                for (int xd = 0; xd < current_params->window_size; xd++) {
                    for (int yd = 0; yd < current_params->window_size; yd++) {
                        for (int zd = 0; zd < current_params->window_size; zd++) {
                            if (dst_has_vals[xd][yd][zd]) {
                                // calculation needed
                                // see if the transposed window in source is out of bounds
                                // if so, toss it
                                int xw = xs + xd - half_window_size;
                                int yw = ys + yd - half_window_size;
                                int zw = zs + zd - half_window_size;
                                block_value from_src, from_dst;
                                if (xw < 0 || yw < 0 || zw < 0 ||
                                    xw >= current_params->src_xd || yw >= current_params->src_yd || zw >= current_params->src_zd) {
                                    //skip_this = true;
                                    from_src = current_params->src->get_block_integral(xw+current_params->src_box.xl,
                                                                                       yw+current_params->src_box.yl,
                                                                                       zw+current_params->src_box.zl).type;
                                }
                                else {
                                    from_src = current_params->src_data[xw][yw][zw];
                                }
                                from_dst = dst_window_vals[xd][yd][zd];
                                float gauss_value = current_params->filter[xd][yd][zd];
                                total_gauss += gauss_value;
                                float diff = get_difference(from_src, from_dst);
                                ssd += diff * gauss_value;
                            }
                        }
                        if (skip_this) {
                            break;
                        }
                    }
                    if (skip_this) {
                        break;
                    }
                }
                if (skip_this)
                    continue;
                if (total_gauss == 0) {
                    printf("Our total gauss was 0 meaning we didnt iterate...\n");
                    continue;
                }
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
        return false;
    }
    
    // pick randomly from the close_to_best list above
    int input_choice = rand() % (close_to_best.size());
    ivec3 input_choice_vector = close_to_best[input_choice];
    
    // fill all blocks in a radius of fill_window_size around our chosen point
    box_coordinate around_coord(xc, yc, zc, current_params->fill_window_size);
    around_coord.constrain(0, 0, 0, current_params->dst_xd, current_params->dst_yd, current_params->dst_zd);
    for (int xt = around_coord.xl; xt < around_coord.xu; xt++) {
        for (int yt = around_coord.yl; yt < around_coord.yu; yt++) {
            for (int zt = around_coord.zl; zt < around_coord.zu; zt++) {
                if (!current_params->has_value[xt][yt][zt]) {
                    int xoff, yoff, zoff;
                    xoff = xt - xc + input_choice_vector.x;
                    yoff = yt - yc + input_choice_vector.y;
                    zoff = zt - zc + input_choice_vector.z;
                    bool out_of_bounds = xoff < 0 || yoff < 0 || zoff < 0 ||
                    xoff >= current_params->src_xd || yoff >= current_params->src_yd || zoff >= current_params->src_zd;
                    if (!out_of_bounds) {
                        //printf("Setting value %d at %d %d %d\n", src_vals[xoff][yoff][zoff], xt, yt, zt);
                        block_value val = current_params->src_data[xoff][yoff][zoff];
                        /*if (val == 0) {
                            val = 37;
                        }*/
                        current_params->dst->set_block_integral(xt+current_params->dst_box.xl,
                                                                yt+current_params->dst_box.yl,
                                                                zt+current_params->dst_box.zl, block_type(val));
                        current_params->has_value[xt][yt][zt] = true;
                        current_params->has_been_queued[xt][yt][zt] = true;
                        current_params->counter++;
                    }
                }
            }
        }
    }
    // mark all blocks immediately outside our fill_window_size as needing check
    around_coord = box_coordinate(xc, yc, zc, current_params->fill_window_size + 2);
    around_coord.constrain(0, 0, 0, current_params->dst_xd, current_params->dst_yd, current_params->dst_zd);
    for (int xt = around_coord.xl; xt < around_coord.xu; xt++) {
        for (int yt = around_coord.yl; yt < around_coord.yu; yt++) {
            for (int zt = around_coord.zl; zt < around_coord.zu; zt++) {
                if (!current_params->has_been_queued[xt][yt][zt]) {
                    // if not, add it to my search points
                    current_params->search_points.push_back(ivec3(xt,yt,zt));
                    current_params->has_been_queued[xt][yt][zt] = true;
                    
                    current_params->dst->set_block_integral(xt+current_params->dst_box.xl,
                                                            yt+current_params->dst_box.yl,
                                                            zt+current_params->dst_box.zl, block_type(38));
                }
            }
        }
    }
    return false;
}