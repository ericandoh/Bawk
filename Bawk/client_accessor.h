//
//  client_accessor.h
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__client_accessor__
#define __Bawk__client_accessor__

#include <stdio.h>
#include "client.h"

class Player;

void set_client(Client* client);
Client* get_client();
Player* get_player();

#endif /* defined(__Bawk__client_accessor__) */
