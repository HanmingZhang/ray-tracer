#pragma once

#include <scene/geometry/geometry.h>

//A sphere is assumed to have a radius of 0.5 and a center of <0,0,0>.
//These attributes can be altered by applying a transformation matrix to the sphere.
class Sphere : public Geometry
{
public:
    Intersection GetIntersection(Ray r);
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    void create();

    virtual glm::mat3 GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal);

};
