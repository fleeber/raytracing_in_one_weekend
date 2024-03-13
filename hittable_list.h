#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory> //shared_ptr
#include <vector>

using std::shared_ptr; //lets you omit std::
using std::make_shared;

//C++ INSTANCING TIP
//shared_ptr<type> is a pointer to an allocated type with reference counting
//each time you assign its value to another shared_ptr the reference count increments
//when shared_ptrs go out of scope (end of block/function) it decrements
//object is safely deleted when counter reaches zero
//example allocation: auto vec3_ptr = make_shared<vec3>(1.345239, 4.427340, 5.239743);
//shared_ptrs allow multiple geometries to share a common instance
//e.g. spheres that use the same material

//std::vector is a generic array-like collection of things that automatically grows
//as more values are added. push_back adds a value to the end of the vector member variable objects

class hittable_list : public hittable { //stores a list of hittables
    public:
        std::vector<shared_ptr<hittable>>objects;
        
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) {add(object);}
        
        void clear() {objects.clear();}

        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = ray_t.max;

            for(const auto& object : objects) {
                if(object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }
            return hit_anything;
        }
};

#endif