#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material;

class hit_record { //a way to stuff a bunch of arguments into a class to send them as a group

    public:
        point3 p;
        vec3 normal;
        double t;
        bool front_face;
        shared_ptr<material> mat;

        //we compare the ray with the normal to figure out which side of the surface of the hittable the ray is hitting it from
        //make it so that normals always point against the incident ray
        void set_face_normal(const ray& r, const vec3& outward_normal) {
            //sets the hit record normal vector
            //outward_normal assumed to have unit length (handled in geometry code)
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable { //cleaner solution than just having an array of objects
    public:
        virtual ~hittable() = default;
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif