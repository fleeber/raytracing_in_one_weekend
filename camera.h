#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>
#include <chrono>

using namespace std::chrono;

class camera {
    public:
        //public parameters
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10; //random samples for each pixel
        int max_depth = 10; //maximum number of ray bounces into scene (otherwise a ray could take a ton of bounces 
        //...(way too many) before it descends into the void)

        void render(const hittable& world) {

            auto start = high_resolution_clock::now();

            initialize();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; //ppm header

            for(int j = 0; j < image_height; ++j) {
                std::clog << "\rscanlines remaining: " << (image_height - j) << ' ' << std::flush; //progress indicator log
                for(int i = 0; i < image_width; ++i) {
                    color pixel_color(0,0,0);
                    for(int sample = 0; sample < samples_per_pixel; ++sample) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    write_color(std::cout, pixel_color, samples_per_pixel);
                }
            }
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            auto seconds = duration.count() / 1000000.0;
            std::clog << "\rdone!                 \n";
            std::clog << "time elapsed: " << seconds << " seconds\n";
        }

    private:
        //private variables
        int image_height;
        point3 center; //camera center
        point3 pixel00_loc; //location of top left pixel 0,0
        vec3 pixel_delta_u; //offset to right pixel
        vec3 pixel_delta_v; //offset to pixel below

        void initialize() {
            //calculate image height
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            center = point3(0,0,0);

            //viewport aspect ratio may differ from aspect_ratio so we use calculated image aspect ratio to determine final viewport width
            auto viewport_height = 2.0; //arbitrary value
            auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
            auto focal_length = 1.0;

            //calculate viewport width and height vectors 
            auto viewport_u = vec3(viewport_width, 0,0);
            auto viewport_v = vec3(0, -viewport_height, 0);

            //calculate width and height delta vectors (pixel to pixel)
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            //calculate location of upper left (first) pixel
            auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            hit_record rec;

            //if ray bounces exceeded, no more light gathered
            if(depth <= 0)
                return color(0,0,0);
            //next we use 0.001 to ignore hits that are very close to the intersection point as a result of floating point rounding errors
            //because these result in points on the surface that get darkened too many times ("shadow acne")
            if(world.hit(r, interval(0.001, infinity), rec)) { //then render the hittable
                //each normal is the vector from the center to the surface point, where the ray origin is moved to the surface and normalized
                //or the opposite when it gets flipped inside out

                ray scattered;
                color attenuation;
                if(rec.mat->scatter(r, rec, attenuation, scattered))
                    return attenuation * ray_color(scattered, depth-1, world);
                return color(0,0,0);
            }
            //if it didn't hit, make a sky gradient
            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            //linear interpolation = (1 - a) * startValue + a * endValue   where 0.0<=a<=1.0
                                //white                 //blue
            return (1.0 - a) * color(1.0,1.0,1.0) + a * color(0.5,0.7,1.0);
        }

        ray get_ray(int i, int j) const {
            //get a randomly sampled camera ray for pixel at i,j
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_sample_square();

            auto ray_origin = center;
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 pixel_sample_square() const {
            //return a random point in the square surrounding a pixel at the origin
            auto px = -0.5 + random_double();
            auto py = -0.5 + random_double();
            return (px * pixel_delta_u) + (py * pixel_delta_v);
        }
};

#endif