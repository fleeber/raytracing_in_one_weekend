#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
    public:
        double e[3]; //could use float for smaller size less precision

        vec3() : e{0,0,0} {} //constructors
        vec3(double e0, double e1, double e2) : e{e0,e1,e2} {}

        //vector channels
        double x() const {return e[0];}
        double y() const {return e[1];}
        double z() const {return e[2];}

        //operators
        vec3 operator-() const {return vec3(-e[0],-e[1],-e[2]);}
        double operator[](int i) const {return e[i];}
        double& operator[](int i) {return e[i];}
        
        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(double t) {
            return *this *= 1/t;
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        static vec3 random() { //to randomly bounce a ray
            return vec3(random_double(), random_double(), random_double());
        }

        static vec3 random(double min, double max) { //to randomly bounce a ray
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

        bool near_zero() const {
            //return true if vector is close to the zero vector
            //  used to avoid zero scatter direction vector bounces
            auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }
    };

    using point3 = vec3; //alias for clarity

    //utility functions
    inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
        return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
    }

    inline vec3 operator+(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
    }

    inline vec3 operator-(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
    }

    inline vec3 operator*(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
    }

    inline vec3 operator*(double t, const vec3 &v) {
        return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
    }

    inline vec3 operator*(const vec3 &v, double t) {
        return t * v;
    }

    inline vec3 operator/(vec3 v, double t) {
        return (1/t) * v;
    }

    inline double dot(const vec3 &u, const vec3 &v) {
        return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
    }

    inline vec3 cross(const vec3 &u, const vec3 &v) {
        return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
            u.e[2] * v.e[0] - u.e[0] * v.e[2],
            u.e[0] * v.e[1] - u.e[1] * v.e[0]);
    }

    inline vec3 unit_vector(vec3 v) {
        return v / v.length();
    }

    inline vec3 random_in_unit_sphere() {
        //rejection method for generating random vector on surface of a hemisphere
        //  generate a random point in the unit cube (-1 to 1)
        //  repeat until the point is in the unit sphere
        //  normalize the vector, invert if on wrong hemisphere
        while (true) {
            auto p = vec3::random(-1,1);
            if(p.length_squared() < 1)
                return p;
        }
    }

    inline vec3 random_unit_vector() { //normalize vector
        return unit_vector(random_in_unit_sphere());
    }

    inline vec3 random_on_hemisphere(const vec3& normal) {
        vec3 on_unit_sphere = random_unit_vector();
        if(dot(on_unit_sphere, normal) > 0.0) //in same hemisphere as normal
            return on_unit_sphere;
        else
            return -on_unit_sphere;
    }

    vec3 reflect(const vec3& v, const vec3& n) {
        //the ray reflection direction of a ray v is v+2b
        //b is orthogonal to the bounce point tangent and intersects with one end of v
        //the length of b is v dot n
        //the resulting vector will be traveling into the surface so it gets flipped with the minus sign
        return v -  2 * dot(v,n) * n;
    }


#endif
