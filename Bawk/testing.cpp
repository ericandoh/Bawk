//
//  testing.cpp
//  Bawk
//
//  Created by Eric Oh on 10/18/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "testing.h"
// clear imports every once in a while
#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include "basic_types.h"
#include "block_orientation.h"

void do_shit();

int testrun() {
    printf("game in playground mode (ie game isnt running)\n");
    printf("to modify this, go to main.cpp and set TESTING to 0\n");
    // fill this with whatever you desire
    do_shit();
    return 0;
}

/*
void do_shit() {
    // copy me
}
*/

// COPY DUMP CONTENTS VVV
void do_shit() {
    // copy me
}

/*
Dumps
 
 void do_shit() {
 init_orientation_rotation_calculations();
 //test_orientation_math();
 }
 
 void do_shit() {
 fvec3 a(0.99, 1.02, 3.19);
 fvec3 b(0.57, 0.63, 0.99);
 
 printf_fvec3(get_nearest_half_or_whole(a));
 printf("\n");
 printf_fvec3(get_nearest_half_or_whole(b));
 printf("\n");
 }
 
 void do_shit() {
 // copy me
 Rotation blah;
 blah = Rotation();
 fvec3 to_dir =fvec3(0.3f,0.0f,0.7f);
 to_dir = glm::normalize(to_dir);
 blah.set_to_point(to_dir, fvec3(0.0f,1.0f,0.0f));
 
 fmat4 matr(1);
 blah.add_world_rotation(&matr);
 fvec4 point(0.7,0.7,0,1);
 point = matr * point;
 printf("%f %f %f\n", point.x, point.y, point.z);
 }
 
 void do_shit() {
 // transform into my coordinates
 fmat4 reverse(1);
 fvec2 angle(-3.14/2.0, 0);
 // round angle to nearest angle
 fvec2 rounded_angle = fvec2(roundf(angle.x * 2 / M_PI) * M_PI / 2,
 roundf(angle.y * 2 / M_PI) * M_PI / 2);
 
 reverse = glm::rotate(reverse, -rounded_angle.y, fvec3(cosf(rounded_angle.x), 0, -sinf(rounded_angle.x)));
 reverse = glm::rotate(reverse, -rounded_angle.x, fvec3(0, 1, 0));
 fvec4 result(1, 0, 0, 1.0f);
 result = reverse * result;
 printf("%f,%f,%f", result.x, result.y, result.z);
 }
 
 void do_shit() {
 test_entity_coordinate_system();
 }
 
 void do_shit() {
 test_entity_holder_collision_detection();
 }

 
 void do_shit() {
 // copy me
 fvec3 pos(0, 0, 0);
 fvec2 angle(0, 3.14159265359/2);
 fvec3 up(0, 1, 0);
 fvec3 center_pos(0.5f, 0.0f, 1.5f);
 
 std::vector<fvec3> test_points;
 test_points.push_back(fvec3(0, 0, 0));
 test_points.push_back(fvec3(1, 0, 0));
 test_points.push_back(fvec3(0, 0, 2));
 test_points.push_back(fvec3(1, 0, 2));
 test_points.push_back(fvec3(0.5f, 0.0f, 1.5f));
 
 fmat4 view = glm::translate(fmat4(1), center_pos);
 view = glm::rotate(view, angle.x, fvec3(0, 1, 0));
 view = glm::rotate(view, angle.y, fvec3(cosf(angle.x), 0, -sinf(angle.x)));
 view = glm::translate(view, -center_pos);
 
 fmat4 reverse = glm::translate(fmat4(1), center_pos);
 reverse = glm::rotate(reverse, -angle.y, fvec3(cosf(angle.x), 0, -sinf(angle.x)));
 reverse = glm::rotate(reverse, -angle.x, fvec3(0, 1, 0));
 reverse = glm::translate(reverse, -center_pos);
 
 for (int i = 0; i < test_points.size(); i++) {
 fvec3 point = test_points[i];
 fvec4 homogenized(point.x, point.y, point.z, 1.0f);
 
 fvec4 result = view * homogenized;
 printf("%f %f %f turns to %f %f %f %f\n", point.x, point.y, point.z, result.x, result.y, result.z, result.w);
 result = reverse * result;
 printf("%f %f %f %f translated back\n", result.x, result.y, result.z, result.w);
 }
 
 view = glm::translate(fmat4(1), -center_pos);
 fvec4 testpoint = fvec4(0, 0, 0, 1);
 fvec4 result = view * testpoint;
 printf("%f %f %f %f\n",result.x, result.y, result.z, result.w);
 
 }
 
 static float noise2d(float x, float y, int seed, int octaves, float persistence, float strength) {
 srand(seed);
 // any more and we start losing precision
 // 2^20 is about a million, so about a million x million possible worlds. I think we good
 int add_x = rand() % (0x1 << 20);
 int add_y = rand() % (0x1 << 20);
 float sea_level = 2.0;
 
 float sum = 0;
 float scale = 1.0;
 
 for(int i = 0; i < octaves; i++) {
 sum += strength * glm::simplex(fvec2((x+add_x) / 256.0, (y+add_y) / 256.0) * scale);
 scale *= 2.0;
 strength *= persistence;
 }
 return sum + sea_level;
 }
 
 
 // COPY FUNCTION DOWN VVV
 void do_shit() {
 int seed = 1093841320; // seed usedot make perlin noise
 int octaves = 5; //how many frequencies to have, let's not change this
 float persistence = 0.8;    // how much of the higher frequencies to have?
 float strength = 10.0;      // how much the terrain should change
 // copy me
 for (float i = 0; i < 20; i++) {
 for (float j = 0; j < 20; j++) {
 strength = 10.0 + (i + j) * 30.0 / 40.0;
 printf("%d ", (int)floorf(noise2d(i, j, seed, octaves, persistence, strength)));
 }
 printf("\n");
 }
 }

 
 class Tracer: public BresenhamTracer {
 bool check_at_coord(float x, float y, float z, BlockOrientation side) override;
 };
 
 bool Tracer::check_at_coord(float x, float y, float z, BlockOrientation side) {
 printf("%f %f %f %d\n", x,y,z,side);
 printf("%d %d %d %d\n", (int)floorf(x),(int)floorf(y),(int)floorf(z),side);
 return false;
 }
 
 void do_shit() {
 
 BlockOrientation orient;
 fvec3 val = find_intersection_with_unit_cube(fvec3(1.0, 1.0, 0.0), fvec3(-0.5, -0.5, 0.0), &orient);
 
 // copy me
 Tracer checker;
 checker.bresenham3D(0.5, 0.7, 0.3, 1.4, 0.3, 0.35);
 printf("....\n");
 checker.bresenham3D(1.4, 0.3, 0.35, 0.5, 0.7, 0.3);
 printf("....\n");
 checker.bresenham3D(0.8, 2.7, 0.3, 1.5, 1.8, 0.35);
 printf("....\n");
 checker.bresenham3D( 1.5, 1.8, 0.35, 0.8, 2.7, 0.3);
 printf("....\n");
 checker.bresenham3D( 0.5, 0.3, 0.7, 0.8, -4.35, 2.7);
 printf("....\n");
 checker.bresenham3D(0.5, 2.1, 0.3, 1.5, 1.2, 0.35);
 printf("....\n");
 checker.bresenham3D(0.5, 2.1, 0.3, 1.5, 1.2, 3.2);
 }
 
 void do_shit() {
 load_game_info();
 printf("Block 1's resistance: %d\n", get_block_resistance(2));
 free_game_info();
 }
 
 void do_shit() {
 IODataObject* make = new IODataObject(get_path_to_game());
 make->save();
 int x = 78;
 make->save_value(x);
 char c = 'u';
 make->save_value(c);
 int arr[] = {3, 4, 5};
 make->save_value(3);
 make->save_pointer(arr, sizeof(arr));
 make->close();
 
 
 delete make;
 
 IODataObject* make2 = new IODataObject(get_path_to_game());
 make->read();
 int xr = make2->read_value<int>();
 char cr = make2->read_value<char>();
 int arr_size = make2->read_value<int>();
 int arrr[arr_size];
 make2->read_pointer(arrr, arr_size * sizeof(int));
 make2->close();
 
 printf("%d %c\n", xr, cr);
 printf("%d %d %d %d", arr_size, arrr[0], arrr[1], arrr[2]);
 }
 
 
 void do_shit() {
 printf("helloworld\n");
 }

*/