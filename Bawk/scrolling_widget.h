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
#include "base_widget.h"

class ScrollingWidget: public BaseWidget {
    std::vector<std::vector<BaseWidget*>> widgets;
    int offset;
protected:
    int maxrows;
    int rowheight;
public:
    ScrollingWidget(int rh, int x, int y, int width, int height);
    ScrollingWidget(int rh, int mr, int x, int y, int width, int height);
    void setup();
    void scroll(int px);
    void refresh();
    // if we need to show a new row this is called
    virtual void set_row(int row, std::vector<BaseWidget*> row_widgets) = 0;
    int get_max_rows();
    void render_elements() override;
};

#endif /* defined(__Bawk__scrolling_widget__) */
