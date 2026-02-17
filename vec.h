#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <bits/stdc++.h>
using namespace std;
#define fl float


struct vec{
    fl x, y;
    vec(){
        x = 0;
        y = 0;
    }
    vec(fl inx, fl iny){
        x = inx;
        y = iny;
    }
    vec add(vec v){
        return vec(x + v.x, y + v.y);
    }
    vec sub(vec v){
        return vec(x - v.x, y - v.y);
    }
    vec mult(fl n) {
        return vec(x * n, y * n);
    }
    fl dot(vec v) {
        return (x * v.x) + (y * v.y);
    }
    vec norm(){
        fl len = sqrt((x * x) + (y * y));
        return vec(x/len, y/len);
    }
};

#endif