//
//  base_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_widget.h"
#include "display.h"

BaseWidget::BaseWidget() {
    x = 0;
    y = 0;
    width = 1;
    height = 1;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

BaseWidget::BaseWidget(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

BaseWidget::BaseWidget(int width, int height) {
    int swidth, sheight;
    get_window_size(&swidth, &sheight);
    
    int xgap = swidth - width;
    int ygap = sheight - height;
    
    this->x = xgap / 2;
    this->y = ygap / 2;
    this->width = width;
    this->height = height;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

void BaseWidget::set_dimensions(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void BaseWidget::render() {
    glViewport(x, y, width, height);
    render_elements();
}

bool BaseWidget::is_clicked(int mx, int my) {
    if (mx >= x && my >= y && mx <= (x + width) && my <= (y + height)) {
        printf("Clicked at %d %d\n", mx, my);
        return true;
    }
    return false;
}

// --- InputReceiver ---
bool BaseWidget::mouse_clicked_callback(Action do_this) {
    double mx, my;
    display_get_cursor_position(&mx, &my);
    return onclick((int)mx, (int)my, do_this);
}

bool BaseWidget::scroll_callback(double xdiff, double ydiff) {
    double mx, my;
    display_get_cursor_position(&mx, &my);
    return scrolled((int)mx, (int)my, ydiff * 30);
}




