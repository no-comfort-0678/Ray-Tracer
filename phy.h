#ifndef PHY_H
#define PHY_H

#include "vec.h"

struct ray{
    vec origin, dir;
    ray(vec o, vec d){
        origin = o;
        dir = d;
    }
    vec dir_to(fl t) {
        return origin.add(dir.mult(t));
    }
};

#endif