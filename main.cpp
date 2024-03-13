#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

/* PPM file format

P3 #means colors are in ASCII
3 2 #width height
255 #maxcolor
r g b
r g b
r g b
r g b
r g b
r g b

*/



color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if(world.hit(r, 0, infinity, rec)) { //then render the hittable
        //each normal is the vector from the center to the surface point, where the ray origin is moved to the surface and normalized
        //or the opposite when it gets flipped inside out
        return 0.5 * (rec.normal + color(1,1,1)); //normal sphere
    }
    //if it didn't hit, make a sky gradient
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    //linear interpolation = (1 - a) * startValue + a * endValue   where 0.0<=a<=1.0
                        //white                 //blue
    return (1.0 - a) * color(1.0,1.0,1.0) + a * color(0.5,0.7,1.0);
}

int main() {

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    //calculate image height
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    //viewport aspect ratio may differ from aspect_ratio so we use calculated image aspect ratio to determine final viewport width
    auto viewport_height = 2.0; //arbitrary value
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

    // camera
    auto focal_length = 1.0;
    auto camera_center = point3(0,0,0);

    //calculate viewport width and height vectors 
    auto viewport_u = vec3(viewport_width, 0,0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    //calculate width and height delta vectors (pixel to pixel)
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    //calculate location of upper left (first) pixel
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    // render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; //ppm header

    for(int j = 0; j < image_height; ++j) {
        std::clog << "\rscanlines remaining: " << (image_height - j) << ' ' << std::flush; //progress indicator log
        for(int i = 0; i < image_width; ++i) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center; //didn't normalize to improve speed?
            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rdone!                  \n";
}