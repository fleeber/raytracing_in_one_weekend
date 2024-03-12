#include "color.h"
#include "vec3.h"
#include "ray.h"

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

color ray_color(const ray& r) {
    return color(0,0,0);
}

int main() {

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    //calculate image height
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    //viewport aspect ratio may differ from aspect_ratio so we use calculated image aspect ratio to determine final viewport width
    auto viewport_height = 2.0; //arbitrary value
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

    //camera
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


    //render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; //ppm header

    for(int j = 0; j < image_height; ++j) {
        std::clog << "\rscanlines remaining: " << (image_height - j) << ' ' << std::flush; //progress indicator log
        for(int i = 0; i < image_width; ++i) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center; //didn't normalize to improve speed?
            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rdone!                  \n";
}