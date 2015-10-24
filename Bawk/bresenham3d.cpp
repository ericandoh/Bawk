
#include "bresenham3d.h"
#include <cmath>

fvec3 find_intersection_with_unit_cube(fvec3 point, fvec3 dir, BlockOrientation* orientation) {
    int x, y, z;
    if (dir.x < 0) {
        x = floorf(point.x - 0.005f);
    }
    else {
        x = floorf(point.x);
    }
    if (dir.y < 0) {
        y = floorf(point.y - 0.005f);
    }
    else {
        y = floorf(point.y);
    }
    if (dir.z < 0) {
        z = floorf(point.z - 0.005f);
    }
    else {
        z = floorf(point.z);
    }    
    float time;
    // TOFU replace this with the actual max float, fuck this shit
    float best_time = 200.0f;
    if (dir.x > 0) {
        // check against time to reach plane x+1
        time = (x + 1 - point.x) / dir.x;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::BACK;
        }
    }
    else if (dir.x < 0) {
        // check against time to reach plane x
        time = (x - point.x) / dir.x;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::FRONT;
        }
    }
    
    if (dir.y > 0) {
        // check against time to reach plane y+1
        time = (y + 1 - point.y) / dir.y;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::BOTTOM;
        }
    }
    else if (dir.y < 0) {
        // check against time to reach plane y
        time = (y - point.y) / dir.y;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::TOP;
        }
    }
    
    if (dir.z > 0) {
        // check against time to reach plane z+1
        time = (z + 1 - point.z) / dir.z;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::LEFT;
        }
    }
    else if (dir.z < 0) {
        // check against time to reach plane z
        time = (z - point.z) / dir.z;
        if (time < best_time) {
            best_time = time;
            *orientation = BlockOrientation::RIGHT;
        }
    }
    
    fvec3 result(best_time*dir.x + point.x, best_time*dir.y + point.y, best_time*dir.z + point.z);
    return result;
}

void BresenhamTracer::bresenham3D(float x1, float y1, float z1, const float x2, const float y2, const float z2) {
    
    fvec3 dir(x2 - x1 , y2 - y1, z2 - z1);
    
    int destx = (int)floorf(x2);
    int desty = (int)floorf(y2);
    int destz = (int)floorf(z2);
    
    fvec3 point = fvec3(x1, y1, z1);
    fvec3 previous;
    fvec3 halfway = point;
    BlockOrientation prev = BlockOrientation::FRONT;
    BlockOrientation side = BlockOrientation::FRONT;
    
    // keep finding intersection points along dir until we run out
    int count = 0;
    while (!((int)floorf(halfway.x) == destx && (int)floorf(halfway.y) == desty && (int)floorf(halfway.z) == destz) && count < 250) {
        previous = point;
        point = find_intersection_with_unit_cube(point, dir, &side);
        // average previous + point
        halfway.x = (previous.x + point.x) / 2.0;
        halfway.y = (previous.y + point.y) / 2.0;
        halfway.z = (previous.z + point.z) / 2.0;
        if (check_at_coord(halfway.x, halfway.y, halfway.z, prev))
            return;
        prev = side;
        count++;
    }
}
