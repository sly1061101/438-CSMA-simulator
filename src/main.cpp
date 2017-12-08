//
//  main.cpp
//  438-CSMA-simulator
//
//  Created by Liuyi Shi on 12/2/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "node.hpp"
#include "file_io.hpp"
#include <vector>
#include <fstream>
#include <math.h>
#include <iostream>

//#define __DEBUG__

float avg ( std::vector<int>& v )
{
    float return_value = 0.0;
    int n = (int)v.size();
    
    for ( int i=0; i < n; i++)
    {
        return_value += v[i];
    }
    
    return ( return_value / (float)n);
}
//****************End of average funtion****************


//Function for variance
float variance ( std::vector<int> & v , float mean )
{
    float sum = 0.0;
    float temp =0.0;
    
    for ( int j =0; j <= v.size()-1; j++)
    {
        temp = pow(((float)v[j] - mean),2);
        sum += temp;
    }
    
    return sum/(v.size());
}


int main(){
    std::ofstream ofs;
    ofs.open ("/Users/ziyangliu/Documents/git/438-CSMA-simulator/output.txt", std::ofstream::out | std::ofstream::app);
    srand(clock());
    int N,L,M,T;
    std::vector<int> R_list;
    read_file("/Users/ziyangliu/Documents/git/438-CSMA-simulator/input.txt",N,L,R_list,M,T);
    
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
    std::vector<int> node_send_next;
    
    nodespace::node::set_channel_occupied_status(false);
    channel_busy_time = 0;

    for(clk = 0; clk<T; ++clk){
        #ifdef __DEBUG__
        for(auto &n:allnode){
            printf("%d\t",n.get_backoff_counter());
        }
        #endif
        //if there is no node transmitting
        if(nodespace::node::get_channel_occupied_status() == false){
            //vector to store the nodes that count down to 0 at this time
            node_to_send.clear();
            //vector to store the nodes that will count down to 0 at next time (or counter is resetted to 0)
            node_send_next.clear();
            
            for(int i = 0; i<allnode.size(); ++i){
                if(allnode[i].get_backoff_counter() == 0)
                    node_to_send.push_back(i);
            }
            
            //if no node counts to zero at this time, it is idle
            if(node_to_send.size() == 0){
                #ifdef __DEBUG__
                printf("U");
                #endif
                total_unused_time++;
                //we must figure out how many nodes will count down to 0 at next time
                for(int i = 0; i<allnode.size(); ++i){
                    if(allnode[i].get_backoff_counter() == 1)
                        node_send_next.push_back(i);
                }
                //if only one node will count down to 0, it will start transmitting next time, then other nodes should not count down
                if(node_send_next.size() != 1){
                    for(auto &n:allnode)
                        n.decrease_backoff_counter();
                }
                //if more than one nodes will count down to 0, collision will happen and there will be no transmitting, so all nodes should count down
                else{
                    allnode[node_send_next[0]].decrease_backoff_counter();
                }
            }
            //if only one node counts to zero at this time, it will starting transmitting
            else if(node_to_send.size() == 1){
                #ifdef __DEBUG__
                printf("T");
                #endif
                total_used_time++;
                num_succ_trans[node_to_send[0]]++;
                nodespace::node::set_channel_occupied_status(true);
            }
            //if more than one node count to zero at this time, collision happens
            else if(node_to_send.size() > 1){
                #ifdef __DEBUG__
                printf("C");
                #endif
                total_collision_time++;
                //deal with all the nodes that count to zero at this time
                for(auto &n:node_to_send){
                    num_collisions[n]++;
                    
                    allnode[n].increase_collision_counter();
                    //if collision counter reaches M, give up this packet and reset the counter
                    if(allnode[n].get_collision_counter() == M)
                        allnode[n].set_collision_counter(0);
                    //set R according to collision counter
                    allnode[n].set_R();
                    //randomly set backoff counter between [0,R]
                    allnode[n].set_backoff_counter();
                    //collect the nodes whose backoff counter is resetted to 0 so that expect to transmitt at next time (but may not successful because of collision)
                    if(allnode[n].get_backoff_counter() == 0)
                        node_send_next.push_back(n);
                }
                //collect the nodes that will count down to 0 at next time so that expect to transmitt at next time (should exclude nodes resetted just now)
                for(int i = 0; i<allnode.size(); ++i){
                    if( std::find(node_to_send.begin(), node_to_send.end(), i) == node_to_send.end() ){
                        if(allnode[i].get_backoff_counter() == 1)
                            node_send_next.push_back(i);
                    }
                }
                //if only one node expect to transmitt at next time, it will start transmitting next time, then other nodes should not count down
                if(node_send_next.size() != 1){
                    for(int i = 0; i<allnode.size(); ++i){
                        //if this node is not resetted just now, which means it counts down to 0 instead of being resetted to 0
                        if( std::find(node_to_send.begin(), node_to_send.end(), i) == node_to_send.end() ){
                            allnode[i].decrease_backoff_counter();
                        }
                    }
                }
                //if more than one nodes expect to transmitt at next time, collision will happen and there will be no transmitting, so all nodes should count down
                else{
                    //should exclude nodes that were resetted just now
                    if( std::find(node_to_send.begin(), node_to_send.end(), node_send_next[0]) == node_to_send.end() )
                        allnode[node_send_next[0]].decrease_backoff_counter();
                }
            }
        }
        //if a node is transmitting at this time
        else{
            #ifdef __DEBUG__
            printf("T");
            #endif
            total_used_time++;
            channel_busy_time++;
            if(channel_busy_time == L-1){
                channel_busy_time = 0;
                nodespace::node::set_channel_occupied_status(false);
                allnode[node_to_send[0]].set_collision_counter(0);
                allnode[node_to_send[0]].set_R();
                allnode[node_to_send[0]].set_backoff_counter();
                node_send_next.clear();
                //similar process as dealing with collision
                //should figure out how many nodes expect to transmit at next time and decrease the counters accordingly
                if(allnode[node_to_send[0]].get_backoff_counter() == 0)
                    node_send_next.push_back(node_to_send[0]);
                for(int i = 0; i<allnode.size(); ++i){
                    if( i != node_to_send[0] ){
                        if(allnode[i].get_backoff_counter() == 1)
                            node_send_next.push_back(i);
                    }
                }
                if(node_send_next.size() != 1){
                    for(int i = 0; i<allnode.size(); ++i){
                        if( i != node_to_send[0] ){
                            allnode[i].decrease_backoff_counter();
                        }
                    }
                }
                else{
                    if( node_send_next[0] != node_to_send[0] ){
                        allnode[node_send_next[0]].decrease_backoff_counter();
                    }
                }
            }
        }
        #ifdef __DEBUG__
        printf("\n");
        #endif
    }
    printf("utilization:%f idle:%f collision:%d\n",(float)total_used_time/T, (float)total_unused_time/T,total_collision_time);
    ofs << "utilization:"<<100*(float)total_used_time/T<<"% idle:"<<100*(float)total_unused_time/T<<"% collision:"<<(float)total_collision_time<<std::endl;
    ofs<<"Variance in number of successful transmissions:"<<variance(num_succ_trans, avg(num_succ_trans))<<std::endl;
    ofs<<"Variance in number of collisions:"<<variance(num_collisions, avg(num_collisions))<<std::endl;
    ofs.close();
    return 0;
}
