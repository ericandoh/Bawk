//
//  efros_leung_two.h
//  Bawk
//
//  Created by Eric Oh on 12/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__efros_leung_two__
#define __Bawk__efros_leung_two__

#include <stdio.h>

class SuperObject;

// trains the efros leung algorithm on a search area rectangle (xl/yl/zl) to (xu/yu/zu) from a source src
void setup_efros(SuperObject* src, SuperObject* dst,
                 int xl1, int yl1, int zl1, int xu1, int yu1, int zu1,
                 int xl2, int yl2, int zl2, int xu2, int yu2, int zu2);

// return true if done
bool step_efros();

#endif /* defined(__Bawk__efros_leung_two__) */
