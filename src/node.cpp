//
//  node.cpp
//  438-CSMA-simulator
//
//  Created by Liuyi Shi on 12/2/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include "node.hpp"

//initialization of static members
bool nodespace::node::channel_occupied = false;
int nodespace::node::M = 10;
std::vector<int> nodespace::node::R_list;

void nodespace::node::set_R(){
    if(collision_counter < R_list.size())
        R = R_list[collision_counter];
    else
        R = R_list.back();
}

void nodespace::node::set_backoff_counter(){
    backoff_counter = rand()%(R+1);
}
