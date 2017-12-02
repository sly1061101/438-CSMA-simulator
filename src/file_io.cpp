//
//  file_io.cpp
//  438-CSMA-simulator
//
//  Created by Liuyi Shi on 12/2/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include "file_io.hpp"
#include <vector>

void read_file(char path[], int &N, int &L, std::vector<int> &R_list, int &M, int &T){
    FILE *fp;
    char c;
    char str[100];
    
    fp = fopen(path, "r");
    fscanf(fp, "%c %d\n",&c,&N);
    fscanf(fp, "%c %d\n",&c,&L);
    
    fgets(str, 99, fp);
    std::vector<int> pos;
    int num_of_R = 0;
    for(int i = 0; i<strlen(str); ++i){
        if(str[i] == ' '){
            num_of_R++;
            pos.push_back(i);
        }
    }
    pos.push_back(strlen(str));
    
    std::string s(str);
    for(int i = 0; i<num_of_R; ++i)
        R_list.push_back(stoi(s.substr(pos[i],pos[i+1])));
    
    fscanf(fp, "%c %d\n",&c,&M);
    fscanf(fp, "%c %d\n",&c,&T);
    
    fclose(fp);
}
