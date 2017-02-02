#include <scene/materials/lambertmaterial.h>

#include <iostream>

const glm::vec3 light_color = glm::vec3(1);
const float k_diffuse = 1.0f;


const float intersectionOffsetCoeff = 0.00001f;


LambertMaterial::LambertMaterial():Material()
{}

LambertMaterial::LambertMaterial(const glm::vec3 &color):Material(color)
{}

glm::vec3 LambertMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray,  Integrator& integrator, unsigned int integrator_depth)
{

    //------------------------------------------------------------------
    //---------------------  initialization ----------------------------
    //------------------------------------------------------------------

    glm::vec3 n = isx.normal;
    n = glm::normalize(n);


    glm::vec3 light_direction = incoming_ray;
    glm::vec3 eye_direction = outgoing_ray;

    //process texture base color
    glm::vec3 real_base_color;


    if(texture != nullptr){

        glm::vec2 uv = isx.object_hit->GetUVCoordinates(isx.point);


        real_base_color = GetImageColor(uv, texture) * base_color;
    }

    else{
        real_base_color = base_color;
    }



    //------------------------------------------------------------------
    //--------------------  basic color part  --------------------------
    //------------------------------------------------------------------

    glm::vec3 totalEnergy = glm::vec3(0);

    //Note that reflected light is independent of observer (V) direction

    //use lambert's law here
    //after we normalizing two vec3, the dot product is equal to cosine theta

    float diffuseTerm  = glm::dot(n, glm::normalize(-light_direction));
    diffuseTerm  = clamp(diffuseTerm , 0, 1);

    glm::vec3 basicReflectedEnergy = light_color * k_diffuse * real_base_color * diffuseTerm;

    basicReflectedEnergy  = clamp_vec3(basicReflectedEnergy , 0, 1);

    totalEnergy = basicReflectedEnergy;




    //------------------------------------------------------------------
    //-------------------  Specular Transmission part  -----------------
    //------------------------------------------------------------------

    //if this object' material is transmissive
    if(refract_idx_in != 0 ||
       refract_idx_out != 0) {

            glm::vec3 normal_at_intersection = isx.normal;
            normal_at_intersection= glm::normalize(normal_at_intersection);

            glm::vec3 ray_direction = -outgoing_ray;
            ray_direction = glm::normalize(ray_direction);

            float critical_angle;
            float theta_in;

            //1.0 means entering
            //-1.0 means leaving
            float enterOrLeave;

            float indxeRatio;


            glm::vec3 energyFromRefractedRay;
            glm::vec3 specularRefractedEnergy;

            //Entering face case
            if(glm::dot(ray_direction, normal_at_intersection) <= 0){
                indxeRatio = refract_idx_out / refract_idx_in;

                if(indxeRatio > 1){
                    critical_angle = asin(1.0 / indxeRatio);
                }
                else {
                    critical_angle = 100000.0f;
                }

                theta_in = acos(glm::dot(glm::normalize(-ray_direction), glm::normalize(normal_at_intersection)));

                enterOrLeave = 1.0f;
            }

            //Leaving face case
            else{
                indxeRatio = refract_idx_in / refract_idx_out;

                if(indxeRatio > 1){
                    critical_angle = asin(1.0 / indxeRatio);
                }
                else {
                    critical_angle = 100000.0f;
                }

                theta_in = acos(glm::dot(glm::normalize(ray_direction), glm::normalize(normal_at_intersection)));

                enterOrLeave = -1.0f;
            }

            normal_at_intersection = enterOrLeave * normal_at_intersection;

            if(theta_in > critical_angle){
                   //do reflection here!

                   //a new reflect ray is created & setup
                   glm::vec3 new_origin = isx.point + intersectionOffsetCoeff * normal_at_intersection;

                   glm::vec3 mirror_ray_driection_against_normal = ray_direction - 2.0f * glm::dot(ray_direction, normal_at_intersection) * normal_at_intersection;

                   Ray reflectRay = Ray(new_origin, glm::normalize(mirror_ray_driection_against_normal));

                   energyFromRefractedRay  = integrator.TraceRay(reflectRay, integrator_depth + 1);

            }

            else{
                   //do refraction here!

                   //a new refract ray is created & setup
                   glm::vec3 new_origin = isx.point - intersectionOffsetCoeff * normal_at_intersection;

                   float dotProduct = glm::dot(normal_at_intersection, ray_direction);


                   glm::vec3 refract_ray_driection = (-indxeRatio * dotProduct -
                                                      (float)sqrt(1.0 - indxeRatio * indxeRatio * (1.0 - dotProduct * dotProduct)))
                                                      * normal_at_intersection
                                                      + indxeRatio * ray_direction;

                   refract_ray_driection = glm::normalize(refract_ray_driection);

                   Ray refractRay = Ray(new_origin, refract_ray_driection);

                   energyFromRefractedRay = integrator.TraceRay(refractRay, integrator_depth + 1);
           }

            specularRefractedEnergy = real_base_color * energyFromRefractedRay;

            totalEnergy = specularRefractedEnergy;

            totalEnergy = clamp_vec3(totalEnergy, 0, 1);
    }






    //------------------------------------------------------------------
    //-----------------  Specular Reflection part  ---------------------
    //------------------------------------------------------------------

    //Specular Reflection part
    //Check if reflectivity is 0, then there is non-reflective.
    if(reflectivity == 0){
        return totalEnergy ;
    }
    //if not, we need add reflectivity part
    else{



        //a new reflect ray is created & setup
        n = isx.normal;

        glm::vec3 intersection_point = isx.point + intersectionOffsetCoeff * n;
        glm::vec3 mirror_ray_driection_against_normal = -outgoing_ray - 2.0f * glm::dot(glm::normalize(-outgoing_ray), glm::normalize(n)) * glm::normalize(n);
        mirror_ray_driection_against_normal = glm::normalize(mirror_ray_driection_against_normal);
        Ray reflectRay = Ray(intersection_point, mirror_ray_driection_against_normal);

        //We trace ray another time
        glm::vec3 energyFromReflectedRay = integrator.TraceRay(reflectRay, integrator_depth + 1);

        //the color obtained from the reflected ray should be multiplied by the material's base color
        //regardless of reflectivity interpolation
        glm::vec3 specularReflectedEnergy = real_base_color * energyFromReflectedRay;

        totalEnergy = (1.0f - reflectivity) * totalEnergy + reflectivity * specularReflectedEnergy;

        totalEnergy = clamp_vec3(totalEnergy, 0, 1);

        return totalEnergy;
    }

}
