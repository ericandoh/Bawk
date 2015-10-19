//
//  scrolling_widget.h
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__scrolling_widget__
#define __Bawk__scrolling_widget__

#include <stdio.h>
#include <vector>
#include "parent_widget.h"

class ScrollingWidget: public ParentWidget {
    int scroll;
    int xoffset;
    int showrows;
protected:
    int maxrows, maxcols;
    int rowwidth, rowheight;
public:
    //ScrollingWidget(int rw, int rh, int x, int y, int width, int height);
    ScrollingWidget(int rw, int rh, int mr, int x, int y, int width, int height);
    void setup();
    bool scrolled(int mx, int my, int px) override;
    void refresh();
    BaseWidget* get_child_at(int x, int y);
    void set_child_at(BaseWidget* child, int x, int y);
    // if we need to show a new row this is called
    virtual BaseWidget* set_row(int row, int col, BaseWidget* current) = 0;
    int get_max_rows();
};

#endif /* defined(__Bawk__scrolling_widget__) */
