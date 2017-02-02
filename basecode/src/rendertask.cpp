#include <rendertask.h>

const bool AAx2 = false;



void RenderTask::run(){

    unsigned int upper_left_x = minPixel.x;
    unsigned int upper_left_y = minPixel.y;

    unsigned int lower_right_x = maxPixel.x;
    unsigned int lower_right_y = maxPixel.y;

    for(unsigned int i = upper_left_x; i < lower_right_x; i++)
    {
        for(unsigned int j = upper_left_y; j < lower_right_y; j++)
        {
            if(AAx2){
                Ray r1 = camera->Raycast((float)i + 0.25f, (float)j + 0.25f);
                Ray r2 = camera->Raycast((float)i + 0.5f, (float)j + 0.25f);
                Ray r3 = camera->Raycast((float)i + 0.25f, (float)j + 0.5f);
                Ray r4 = camera->Raycast((float)i + 0.5f, (float)j + 0.5f);



                unsigned int depth = 0;
                film->pixels[i][j] = 0.25f * (integrator->TraceRay(r1, depth) +
                                              integrator->TraceRay(r2, depth) +
                                              integrator->TraceRay(r3, depth) +
                                              integrator->TraceRay(r4, depth));
            }

            else{
                 Ray r = camera->Raycast((float)i, (float)j);
                 unsigned int depth = 0;
                 film->pixels[i][j] = integrator->TraceRay(r, depth);
            }
        }
    }
}
