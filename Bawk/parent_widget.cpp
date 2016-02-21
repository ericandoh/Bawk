//
//  parent_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/18/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "parent_widget.h"
#include "display.h"

ParentWidget::ParentWidget(): BaseWidget() {};

ParentWidget::ParentWidget(int x, int y, int width, int height): BaseWidget(x, y, width, height) {};

ParentWidget::ParentWidget(int width, int height): BaseWidget(width, height) {};

void ParentWidget::add_child(BaseWidget* child) {
    children.push_back(child);
}

void ParentWidget::close_child(BaseWidget* child) {
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children.at(i) == child) {
            children.erase(children.begin() + i);
            break;
        }
    }
}

bool ParentWidget::has_child(BaseWidget* child) {
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children.at(i) == child) {
            return true;
        }
    }
    return false;
}

void ParentWidget::toggle_child(BaseWidget* child) {
    if (has_child(child)) {
        close_child(child);
    }
    else {
        add_child(child);
    }
}

void ParentWidget::close_latest_child() {
    if (children.size() > 0)
        children.erase(children.begin() + children.size() - 1);
}

int ParentWidget::count_children() {
    return (int)children.size();
}

// called when this is scrolled
bool ParentWidget::scrolled(int mx, int my, int px) {
    for (int i = (int)children.size() - 1; i >= 0; i--) {
        if (children.at(i)->is_clicked(mx, my)) {
            return children.at(i)->scrolled(mx, my, px);
        }
    }
    return false;
}

// render the children
void ParentWidget::render_elements() {
    for (auto &child: children) {
        child->render();
    }
}

// action to do when clicked
bool ParentWidget::onclick(int mx, int my, Action button) {
    // latest widget gets input first
    for (int i = (int)children.size() - 1; i >= 0; i--) {
        if (children.at(i)->is_clicked(mx, my)) {
            return onclick(children[i], mx, my, button);
        }
    }
    return false;
}

bool ParentWidget::onclick(BaseWidget* clicked_child, int mx, int my, Action button) {
    return clicked_child->onclick(mx, my, button);
}

bool ParentWidget::key_callback(Action do_this, int ms) {
    if (do_this == CANCEL) {
        close_latest_child();
        if (count_children() == 1) {
            display_disable_cursor();
        }
        return true;
    }
    // TODO should we transmit the key push to our children?
    return false;
}