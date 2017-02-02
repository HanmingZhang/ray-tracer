#pragma once

#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/materials/material.h>
#include <scene/transform.h>
#include <scene/intersectable.h>
#include <algorithm>

class Intersection;//Forward declaration because Intersection and Geometry cross-include each other
class Material;

//Geometry is an abstract class since it contains a pure virtual function (i.e. a virtual function that is set to 0)
class Geometry : public Drawable, public Intersectable
{
public:
    Geometry():
    name("GEOMETRY"), transform(), material(nullptr)
    {}

    virtual ~Geometry(){}
    virtual void SetMaterial(Material* m){material = m;}
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point) = 0;
    QString name;//Mainly used for debugging purposes
    Transform transform;
    Material* material;


    virtual glm::mat3 GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal) = 0;


    //virtual Intersection GetIntersection(Ray r) {return Intersection();}
};
