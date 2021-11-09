#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color=0;
    state.image_depth=0;
    state.image_color = new pixel[width*height];

    for(pixel* p = state.image_color; p < state.image_color + (width*height); p++){
        *p = make_pixel(0,0,0);
    }
    //std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    //std::cout<<"TODO: implement rendering."<<std::endl;
    if(type == render_type::triangle){
        data_geometry* geometry = new data_geometry[3];

        for(int i = 0; i < state.num_vertices; i++){
            size_t index = i % 3;
            float* this_data = state.vertex_data + (i * state.floats_per_vertex);

            geometry[index].data = this_data;

            data_vertex temp_vertex;
            temp_vertex.data = this_data;

            state.vertex_shader(temp_vertex, geometry[index], state.uniform_data);

            if(index == 2)
                rasterize_triangle(state, geometry[0], geometry[1], geometry[2]);
        }
        delete [] geometry;

    }




}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2,int face)
{
    if(face==6)
    {
        rasterize_triangle(state, v0, v1, v2);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,v0,v1,v2,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2)
{
    int x[3];
    int y[3];

    x[0] = static_cast<int>(0.5 * ((state.image_width * v0.gl_Position[0] / v0.gl_Position[3]) + state.image_width - 1.0));

    y[0] = static_cast<int>(0.5 * ((state.image_height * v0.gl_Position[1] / v0.gl_Position[3]) + state.image_height  - 1.0));

    x[1] = static_cast<int>(0.5 * ((state.image_width * v1.gl_Position[0] / v1.gl_Position[3]) + state.image_width - 1.0));

    y[1] = static_cast<int>(0.5 * ((state.image_height * v1.gl_Position[1] / v1.gl_Position[3]) + state.image_height  - 1.0));

    x[2] = static_cast<int>(0.5 * ((state.image_width * v2.gl_Position[0] / v2.gl_Position[3]) + state.image_width - 1.0));

    y[2] = static_cast<int>(0.5 * ((state.image_height * v2.gl_Position[1] / v2.gl_Position[3]) + state.image_height  - 1.0));

    int min_x = std::min(std::min(x[0], x[1]), x[2]);
    int max_x = std::max(std::max(x[0], x[1]), x[2]);
    int min_y = std::min(std::min(y[0], y[1]), y[2]);
    int max_y = std::max(std::max(y[0], y[1]), y[2]);

    //keep bounds within pixel grid
    min_x = (min_x < 0 ? 0 : min_x);
    min_y = (min_y < 0 ? 0 : min_y);
    max_x = (max_x > state.image_width  ? state.image_width  - 1 : max_x);
    max_y = (max_y > state.image_height ? state.image_height - 1 : max_y);

    float ABC_area = triangle_area(x[0],y[0], x[1],y[1], x[2], y[2]);

    for (int pixel_x = min_x; pixel_x <= max_x; pixel_x++) {
        for (int pixel_y = min_y; pixel_y <= max_y; pixel_y++) {

            float bary_a = ABC_area / triangle_area(pixel_x, pixel_y, x[1], y[1], x[2], y[2]);
            float bary_b = ABC_area / triangle_area(x[0], y[0], pixel_x, pixel_y, x[2], y[2]);
            float bary_c = ABC_area / triangle_area(x[0], y[0], x[1], y[1], pixel_x, pixel_y);

            //if (pixel_x, pixel_y) is in the triangle, draw it
            if (   bary_a >= 0
                && bary_b >= 0
                && bary_c >= 0) {

                state.image_color[pixel_x + (pixel_y * state.image_width)] = make_pixel(255, 255, 255);
            }
        }
    }

    
    //std::cout<<"TODO: implement rasterization"<<std::endl;
}


float triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {

    return 0.5 * (
          ((x2 * y3) - (x3 * y2))
        - ((x1 * y3) - (x3 * y1))
        + ((x1 * y2) - (x2 * y1))
        );
}