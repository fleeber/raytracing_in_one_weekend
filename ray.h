#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray { //P(t) = A + tb     P=position A=rayorigin b=raydirection t=param
    public:
        ray() {} //constructors
        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        point3 origin() const {return orig;}
        vec3 direction() const {return dir;}

        point3 at(double t) const {
            return orig + t*dir; //position along ray
        }
    private:
        point3 orig;
        vec3 dir;
};

#endif