#include <raytracing/integrator.h>
#include <math.h>
#include <iostream>

const float intersectionOffsetCoeff = 0.01f;
const glm::vec3 backgroundColor = glm::vec3(0);


Integrator::Integrator():
    max_depth(5), scene(nullptr), intersection_engine(nullptr)
{
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    glm::vec3 result_color = backgroundColor;

    //Check whether depth exceeds max depth
    if(depth > max_depth){
        //return result_color;
        return glm::vec3(0,0,1);
    }


    //Intersect this ray with all objects and find intersection(if any)
    //on object j that is closest to the origin of the ray
    Intersection result_intersection = intersection_engine->GetIntersection(r);



    //Check whether intersection exists,
    //If there is intersection, then we use local illumination model to compute color
    if(result_intersection.t != -1 || result_intersection.object_hit != nullptr){


        int lights_affected_count = 0;

        for(Geometry* l : scene->lights){

            //Here, we regard every light as a point light
            //and we get the position of this point light here
            glm::vec3 light_pos = glm::vec3(l->transform.T() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));


            glm::vec3 intersectionPoint = result_intersection.point + intersectionOffsetCoeff * glm::normalize(result_intersection.normal);
            glm::vec3 outgoing_ray = - r.direction;
            glm::vec3 incoming_ray = intersectionPoint - light_pos;

            Ray lightFeelerRay = Ray(intersectionPoint, glm::normalize(-incoming_ray));

            //if the light-feeler ray test say that there is an obstruction
            //we skip this light and think it won't affect the color of this intersection

            //and before that,
            //we need to consider whether the material of the obj this ray hit is transmissive
            //if it is, it shouldn't be counted

            bool isShadowedByTransmissiveObj = false;
            glm::vec3 TransmissiveMaterialMaskColor = glm::vec3(1);

            if(fabs(result_intersection.object_hit->material->refract_idx_in - 0) < 0.1f &&
               fabs(result_intersection.object_hit->material->refract_idx_out - 0) < 0.1f &&
               lightFeelerRay_ObstructionTest(lightFeelerRay, light_pos, isShadowedByTransmissiveObj, TransmissiveMaterialMaskColor)){
                continue;
            }

            //then we accumulate the color
            lights_affected_count++;


            if(isShadowedByTransmissiveObj){
                result_color += TransmissiveMaterialMaskColor * result_intersection.object_hit->material->EvaluateReflectedEnergy(result_intersection, outgoing_ray, incoming_ray, *this, depth);
            }

            else{
                result_color += result_intersection.object_hit->material->EvaluateReflectedEnergy(result_intersection, outgoing_ray, incoming_ray, *this, depth);
            }
        }

        if(lights_affected_count != 0){
            result_color = (1.0f / (float)lights_affected_count) * result_color;
        }

        return result_color;

    }


    //If there is no intersection
    else{
        //TODO;
        //Check whether ray is parallel to any of light directions
        //if it is, then make return this light's color




        //else just return background color
        return backgroundColor;
    }

}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}





bool Integrator::lightFeelerRay_ObstructionTest(Ray r, glm::vec3 light_pos, bool& isShadowedByTransmissiveObj, glm::vec3& TransmissiveMaterialMaskColor){

    QList<Intersection> tempList = intersection_engine->GetAllIntersections(r);

    Intersection temp;



    //We need to check whether the obj hit has a transmissive material,
    //If it is, we should count that hit
    int last_non_transmissive_obj_hit_idx = -1;
    int i;
    for(i = 0; i < tempList.size(); i++){
        if(fabs(tempList[i].object_hit->material->refract_idx_in - 0) < 0.1f &&
           fabs(tempList[i].object_hit->material->refract_idx_out - 0) < 0.1f){
            temp = tempList[i];
            last_non_transmissive_obj_hit_idx = i;
            break;
        }
    }

    //If i equates to the size of tempList
    //that means all objs hit are transmissive
    //or there is nothing hit ( 0 condition)
    if(i == tempList.size()){
        last_non_transmissive_obj_hit_idx = tempList.size();
    }


    //shadows cast by transmissive obj check
    //accumulate transmissive material's base color here
    if(tempList.size() != 0 && last_non_transmissive_obj_hit_idx != -1){

        for(i = 0; i < last_non_transmissive_obj_hit_idx; i++){

                    isShadowedByTransmissiveObj = true;

                    if(tempList[i].object_hit->material->texture != nullptr){

                        glm::vec2 uv = tempList[i].object_hit->GetUVCoordinates(tempList[i].point);

                        TransmissiveMaterialMaskColor *= (tempList[i].object_hit->material->GetImageColor(uv, tempList[0].object_hit->material->texture) *
                                                          tempList[i].object_hit->material->base_color);
                    }

                    else{
                        TransmissiveMaterialMaskColor *= tempList[i].object_hit->material->base_color;

                    }
        }
    }



     //if this feeler ray hits nothing, that means there is no obstruction
     if(temp.t == -1 || temp.object_hit == nullptr){
         return false;
     }

     //or if this ray hits something, we need to compare its smallest t with light's position t
     else {
         float t_light_pos;

         if(r.direction.x != 0){
             t_light_pos = (light_pos.x - r.origin.x) / r.direction.x;
         }
         else if(r.direction.y != 0){
             t_light_pos = (light_pos.y - r.origin.y) / r.direction.y;
         }
         else if(r.direction.z != 0){
             t_light_pos = (light_pos.z - r.origin.z) / r.direction.z;
         }


         if(temp.t < t_light_pos){
             return true;
         }

         else return false;
     }

 }
