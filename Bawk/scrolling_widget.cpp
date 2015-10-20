//
//  scrolling_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "scrolling_widget.h"

/*
ScrollingWidget::ScrollingWidget(int rh, int x, int y, int width, int height) {
    rowheight = rh;
    maxrows = 0;
    int numrows = int(width / rowheight) + 1;
    
    for (int i = 0; i < numrows; i++) {
        widgets.push_back(std::vector<BaseWidget*>());
    }
}*/

ScrollingWidget::ScrollingWidget(int rw, int rh, int mr,
                                 int x, int y,
                                 int width, int height): ParentWidget(x, y, width, height) {
    rowwidth = rw;
    rowheight = rh;
    maxrows = mr;
    maxcols = width / rw;
    xoffset = (width - maxcols * rw) / 2;
    scroll = 0;
    showrows = height / rowheight + 1;
}

void ScrollingWidget::setup() {
    refresh();
}

BaseWidget* ScrollingWidget::get_child_at(int x, int y) {
    x = x % showrows;
    if (children.size() > x * maxcols + y) {
        return children.at(x * maxcols + y);
    }
    return 0;
}

void ScrollingWidget::set_child_at(BaseWidget* child, int x, int y) {
    x = x % showrows;
    if (children.size() <= x * maxcols + y) {
        while (children.size() <= x * maxcols + y) {
            children.push_back(0);
        }
    }
    children[x * maxcols + y] = child;
}

bool ScrollingWidget::scrolled(int mx, int my, int px) {
    int oldstart = scroll / rowheight;
    scroll += px;
    if (scroll < 0) {
        scroll = 0;
    }
    // TODO refine this so that the bottom row will show at the bottom not at the top
    if (scroll >= rowheight * maxrows) {
        scroll = rowheight * maxrows;
    }
    int newstart = scroll / rowheight;
    
    if (newstart > oldstart) {
        int start = imax(oldstart + showrows, newstart);
        for (int i = start; i < newstart + showrows; i++) {
            for (int j = 0; j < maxcols; j++) {
                set_child_at(set_row(i, j, get_child_at(i, j)), i, j);
            }
        }
    }
    else if (newstart < oldstart) {
        int end = imin(newstart + showrows, oldstart);
        for (int i = newstart; i < end; i++) {
            for (int j = 0; j < maxcols; j++) {
                set_child_at(set_row(i, j, get_child_at(i, j)), i, j);
            }
        }
    }
    
    for (int i = newstart; i < showrows + newstart; i++) {
        for (int j = 0; j < maxcols; j++) {
            BaseWidget* widget = get_child_at(i, j);
            if (widget) {
                widget->x = xoffset + rowwidth*j + x;
                widget->y = scroll + height - rowheight*i + y;
                widget->width = rowwidth;
                widget->height = rowheight;
            }
        }
    }
    return true;
}

void ScrollingWidget::refresh() {
    int newstart = scroll / rowheight;
    for (int i = newstart; i < showrows + newstart; i++) {
        for (int j = 0; j < maxcols; j++) {
            set_child_at(set_row(i, j, get_child_at(i, j)), i, j);
        }
    }
    for (int i = newstart; i < showrows + newstart; i++) {
        for (int j = 0; j < maxcols; j++) {
            BaseWidget* widget = get_child_at(i, j);
            if (widget) {
                widget->x = xoffset + rowwidth*j + x;
                widget->y = scroll + height - rowheight*i + y;
                widget->width = rowwidth;
                widget->height = rowheight;
            }
        }
    }
}

int ScrollingWidget::get_max_rows() {
    return maxrows;
}
