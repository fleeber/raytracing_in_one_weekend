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

double hit_sphere(const point3& center, double radius, const ray& r) {
    //given a (centered at origin) sphere, radius r, and a point (x,y,z) ON the sphere,
    //x*x + y*y + z*z = r*r
    //                <      point is inside sphere
    //                >      point is outside sphere
    //given an arbitrarily positioned sphere at (Cx, Cy, Cz)
    //(x-Cx)(x-Cx) + (y-Cy)(y-Cy) + (z-Cz)(z-Cz) = r*r    (Cx => C_x  not C*x)
    //vector from center C to point P(x,y,z) is (P-C)
    //(P-C) dot (P-C) = (x-Cx)(x-Cx) + (y-Cy)(y-Cy) + (z-Cz)(z-Cz)
    //equivalently (P-C) dot (P-C) = r*r
    //a point P that satisfies the above equation is on the sphere.
    
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b*b - 4*a*c;
    //positive discriminant = ray passes in and out at 2 points on sphere
    //negative discriminant = ray never touches sphere
    //zero discriminant = ray touches sphere once tangentially
    if(discriminant < 0) {
        return -1.0;
    } else { //find the location of the hit
        return (-b - sqrt(discriminant)) / (2.0 * a); //right now it's less of a ray and more of a line
    }
}

color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0,0,-1), 0.5, r); //define a sphere
    if(t > 0.0) { //then render the sphere
        vec3 N = unit_vector(r.at(t) - vec3(0,0,-1));
        //each normal is the vector from the center to the surface point, where the ray origin is moved to the surface and normalized
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

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