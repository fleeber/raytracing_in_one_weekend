#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "material.h"
#include "hittable_list.h"
#include "sphere.h"

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

int main() {

    // world
    hittable_list world;

    auto material_ground = make_shared<metal>(color(0.05,0.05,0.2), 0.15);
    /*auto material_center = make_shared<dielectric>(1.5);
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8,0.6,0.2), 0.3);
    auto material_5 = make_shared<lambertian>(color(0.8,0.1,0.7));
    auto material_6 = make_shared<metal>(color(0.0,0.0,0.2),0.01);

                                    //center x y z         radius   material
    world.add(make_shared<sphere>(point3(0.0,-100.5,-1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
    world.add(make_shared<sphere>(point3(1.2, 0.0, -.8), 0.1, material_5));
    world.add(make_shared<sphere>(point3(0.6, 0.0, -3.0), 0.2, material_6));*/
    world.add(make_shared<sphere>(point3(0.0,-100.35,-1.0), 100.0, material_ground));

    for(int a = -3; a < 19; a++) {
        for(int b = -3; b < 19; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), -0.25, b + 0.9*random_double());
            if((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
                if(choose_mat < 0.4) { //diffuse material
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.1, sphere_material));
                } else if(choose_mat < 0.75) { //metal material
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));                  
                } else if(choose_mat < 0.88) { //glass material
                    sphere_material = make_shared<dielectric>(2.2);
                    world.add(make_shared<sphere>(center, 0.1, sphere_material));
                } else { //bubble
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, -0.3, sphere_material));
                }
            }
        }
    }
    

    // camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.render(world);


}