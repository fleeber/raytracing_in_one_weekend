#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class hit_record;

class material {
    public:
        virtual ~material() = default;
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
    //this lambertian diffuse material will always scatter rays and attenuate based on reflectance
    //(CURRENTLY OMITTED) diffuse material using random ray direction, less like real life, less shadow variance
    //using lambertian diffuse, more rays scattering towards the normal
    //so less light bounces toward camera and more light bounces in shadow areas
    public:
        lambertian(const color& a) : albedo(a) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto scatter_direction = rec.normal + random_unit_vector();
            
            if(scatter_direction.near_zero()) //if the random ray was extremely close to equal to the normal vector
                scatter_direction = rec.normal;
            
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
    
    private:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_unit_vector());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0); //absorb scatters that go below the surface due to fuzz
        }
    
    private:
        color albedo;
        double fuzz;
};

class dielectric : public material {
    //clear materials are dielectrics. a ray hitting a clear material splits into a reflected ray and refracted (transmitted) ray
    //a negative sphere radius can be used to make a hollow sphere that looks like a bubble
    //here we EVENTUALLY randomly choose one of those rays rather than generating both
    //the current material ALWAYS REFRACTS.
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
            attenuation = color(1.0,1.0,1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction ,rec.normal),1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if(cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            
            scattered = ray(rec.p, direction);
            return true;
        }
    private:
        double ir; //index of refraction

        static double reflectance(double cosine, double ref_idx) {
            //use schlick's approximation for reflectance that varies based on angle of view
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0) * pow((1 - cosine), 5);
        }
};

#endif
