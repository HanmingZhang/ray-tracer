#include <rendertask.h>

void RenderTask::run(){

    unsigned int upper_left_x = minPixel.x;
    unsigned int upper_left_y = minPixel.y;

    unsigned int lower_right_x = maxPixel.x;
    unsigned int lower_right_y = maxPixel.y;

    for(unsigned int i = upper_left_x; i < lower_right_x; i++)
    {
        for(unsigned int j = upper_left_y; j < lower_right_y; j++)
        {
            Ray r = camera->Raycast((float)i, (float)j);
            unsigned int depth = 0;
            film->pixels[i][j] = integrator->TraceRay(r, depth);
        }
    }
}
