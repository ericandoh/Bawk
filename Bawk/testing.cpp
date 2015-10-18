//
//  testing.cpp
//  Bawk
//
//  Created by Eric Oh on 10/18/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "testing.h"
// clear imports every once in a while
#include "block_loader.h"

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

// COPY FUNCTION DOWN VVV

void do_shit() {
    IODataObject* make = new IODataObject();
    make->save_to_game();
    int x = 78;
    make->save_value(x);
    char c = 'u';
    make->save_value(c);
    int arr[] = {3, 4, 5};
    make->save_value(3);
    make->save_pointer(arr, sizeof(arr));
    make->close();
    
    
    delete make;
    
    IODataObject* make2 = new IODataObject();
    make2->read_from_game();
    
    int xr = make2->read_value<int>();
    char cr = make2->read_value<char>();
    int arr_size = make2->read_value<int>();
    int arrr[arr_size];
    make2->read_pointer(arrr, arr_size * sizeof(int));
    make2->close();
    
    printf("%d %c\n", xr, cr);
    printf("%d %d %d %d", arr_size, arrr[0], arrr[1], arrr[2]);
}

// COPY DUMP CONTENTS VVV

/*
Dumps
 void do_shit() {
 printf("helloworld\n");
 }

*/