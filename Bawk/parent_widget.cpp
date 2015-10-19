//
//  parent_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/18/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "parent_widget.h"

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

void ParentWidget::close_latest_child() {
    if (children.size() > 0)
        children.erase(children.begin() + children.size() - 1);
}

// called when this is scrolled
bool ParentWidget::scrolled(int mx, int my, int px) {
    for (int i = (int)children.size() - 1; i >= 0; i--) {
        if (children.at(i)->is_clicked(mx, my)) {
            children.at(i)->scrolled(mx, my, px);
            return true;
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
bool ParentWidget::onclick(int mx, int my, int button) {
    // latest widget gets input first
    for (int i = (int)children.size() - 1; i >= 0; i--) {
        if (children.at(i)->is_clicked(mx, my)) {
            children.at(i)->onclick(mx, my, button);
            return true;
        }
    }
    return false;
}