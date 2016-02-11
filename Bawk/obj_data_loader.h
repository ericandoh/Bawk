//
//  obj_data_loader.h
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__obj_data_loader__
#define __Bawk__obj_data_loader__

#include <stdio.h>
#include <string>

class ModelData;

void read_obj_file(ModelData* dst, std::string obj_filename);

#endif /* defined(__Bawk__obj_data_loader__) */
