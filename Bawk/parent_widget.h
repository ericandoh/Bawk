//
//  parent_widget.h
//  Bawk
//
//  Created by Eric Oh on 10/18/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__parent_widget__
#define __Bawk__parent_widget__

#include <stdio.h>
#include <vector>
#include "base_widget.h"

class ParentWidget: public BaseWidget {
    // think of the children!!!
protected:
    std::vector<BaseWidget*> children;
public:
    ParentWidget();
    ParentWidget(int x, int y, int width, int height);
    ParentWidget(int width, int height);
    
    void add_child(BaseWidget* child);
    void close_child(BaseWidget* child);
    bool has_child(BaseWidget* child);
    void toggle_child(BaseWidget* child);
    void close_latest_child();
    int count_children();
    
    // --- BaseWidget ---
    // called when this is scrolled
    virtual bool scrolled(int mx, int my, int px) override;
    // render the children
    virtual void render_elements() override;
    // action to do when clicked
    bool onclick(int mx, int my, Action button) override;
    virtual bool onclick(BaseWidget* clicked_child, int mx, int my, Action button);
    
    // -- Displayable ---
    virtual bool key_callback(Action do_this, int ms) override;
};

#endif /* defined(__Bawk__parent_widget__) */
