//
//  node.hpp
//  438-CSMA-simulator
//
//  Created by Liuyi Shi on 12/2/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace nodespace{
    class node{
    private:
        static bool channel_occupied;
        static int M;
        static std::vector<int> R_list;
        
        int backoff_counter;
        int collision_counter;
        int R;
        
    public:
        node(){
            collision_counter = 0;
            set_R();
            set_backoff_counter();
        }
        
        static void set_channel_occupied_status(bool is_occupied){ channel_occupied = is_occupied; }
        
        static bool get_channel_occupied_status(){ return channel_occupied; }
        
        static void set_M(int M_){ M = M_; }
        
        static void set_R_list(std::vector<int> R_list_){ R_list = R_list_; }

        int get_backoff_counter(){ return backoff_counter; }
        void decrease_backoff_counter(){ backoff_counter--; }
        void increase_backoff_counter(){ backoff_counter++; }
        
        int get_collision_counter(){ return collision_counter; }
        void increase_collision_counter(){ collision_counter++; }
        void set_collision_counter(int collision_counter_){ collision_counter = collision_counter_; }
        
        //Set R according to R_list and collision_counter
        void set_R();
        //randomly pick a value for backoff_counter between [0, R]
        void set_backoff_counter();
        
    };
    
}

#endif /* node_hpp */
