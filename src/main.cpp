//
//  main.cpp
//  438-CSMA-simulator
//
//  Created by Liuyi Shi on 12/2/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include <stdio.h>
#include "node.hpp"
#include "file_io.hpp"
#include <vector>


int main(){
    int N,L,M,T;
    std::vector<int> R_list;
    read_file("input.txt",N,L,R_list,M,T);
    
    nodespace::node::set_channel_occupied_status(false);
    nodespace::node::set_M(M);
    nodespace::node::set_R_list(R_list);
    
    std::vector<nodespace::node> allnode;
    allnode.resize(N);
    
    int total_used_time = 0;
    int total_unused_time = 0;
    int total_collision_time = 0;
    std::vector<int> num_succ_trans;
    std::vector<int> num_collisions;
    num_succ_trans.resize(N);
    num_collisions.resize(N);
    
    int clk;
    int channel_busy_time;
    std::vector<int> node_to_send;
    
    nodespace::node::set_channel_occupied_status(false);
    channel_busy_time = 0;

    for(clk = 0; clk<T; ++clk){
        if(nodespace::node::get_channel_occupied_status() == false){
            node_to_send.clear();
            for(int i = 0; i<allnode.size(); ++i){
                if(allnode[i].get_backoff_counter() == 0)
                    node_to_send.push_back(i);
            }
            if(node_to_send.size() == 0){
                total_unused_time++;
                for(auto &n:allnode)
                    n.decrease_backoff_counter();
            }
            else if(node_to_send.size() == 1){
                total_unused_time++;
                num_succ_trans[node_to_send[0]]++;
                nodespace::node::set_channel_occupied_status(true);
            }
            else if(node_to_send.size() > 1){
                total_collision_time++;
                for(auto &n:node_to_send){
                    num_collisions[n]++;
                    allnode[n].increase_collision_counter();
                    
                    if(allnode[n].get_collision_counter() == M)
                        allnode[n].set_collision_counter(0);
                    
                    allnode[n].set_R();
                    allnode[n].set_backoff_counter();
                }
            }
        }
        else{
            total_used_time++;
            channel_busy_time++;
            if(channel_busy_time == L){
                channel_busy_time = 0;
                nodespace::node::set_channel_occupied_status(false);
                allnode[node_to_send[0]].set_collision_counter(0);
                allnode[node_to_send[0]].set_R();
                allnode[node_to_send[0]].set_backoff_counter();
            }
        }
    }
    printf("utilization:%f idle:%f number of collision:%d\n",(float)total_used_time/T, (float)total_unused_time/T,total_collision_time);
    return 0;
}
