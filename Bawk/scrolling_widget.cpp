//
//  scrolling_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "scrolling_widget.h"

ScrollingWidget::ScrollingWidget(int rh, int x, int y, int width, int height) {
    rowheight = rh;
    maxrows = 0;
    int numrows = int(width / rowheight) + 1;
    
    for (int i = 0; i < numrows; i++) {
        widgets.push_back(std::vector<BaseWidget*>());
    }
}

ScrollingWidget::ScrollingWidget(int rh, int mr,
                                 int x, int y, int width, int height): BaseWidget(x, y, width, height) {
    rowheight = rh;
    maxrows = mr;
    int numrows = int(width / rowheight) + 1;
    
    for (int i = 0; i < numrows; i++) {
        widgets.push_back(std::vector<BaseWidget*>());
    }
}

void ScrollingWidget::setup() {
    for (int i = 0; i < widgets.size(); i++) {
        set_row(i, widgets[i % widgets.size()]);
    }
}

void ScrollingWidget::scroll(int px) {
    int old_startindex = offset / rowheight;
    offset += px;
    if (offset < 0) {
        offset = 0;
    }
    // TODO refine this so that the bottom row will show at the bottom not at the top
    if (offset >= rowheight * get_max_rows()) {
        offset = rowheight * get_max_rows();
    }
    int new_startindex = offset / rowheight;
    
    if (new_startindex != old_startindex) {
        if (new_startindex > old_startindex) {
            int start = imax(old_startindex + (int)widgets.size(), new_startindex);
            for (int i = start; i < new_startindex + widgets.size(); i++) {
                set_row(i, widgets[i % widgets.size()]);
            }
        }
        else {
            int end = imin(new_startindex + (int)widgets.size(), old_startindex);
            for (int i = new_startindex; i < end; i++) {
                set_row(i, widgets[i % widgets.size()]);
            }
        }
    }
    for (int i = 0; i < widgets.size(); i++) {
        for (int j = 0; j < widgets[i].size(); j++) {
            widgets[(i + new_startindex) % widgets.size()][j]->y = (i + new_startindex) * rowheight - offset;
        }
    }
}

void ScrollingWidget::refresh() {
    int new_startindex = offset / rowheight;
    for (int i = 0; i < widgets.size(); i++) {
        set_row(i + new_startindex, widgets[(i + new_startindex) % widgets.size()]);
    }
}

int ScrollingWidget::get_max_rows() {
    return maxrows;
}

void ScrollingWidget::render_elements() {
    for (auto &i: widgets) {
        for (auto &j: i) {
            j->render();
        }
    }
}