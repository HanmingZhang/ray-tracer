#pragma once

#include <scene/geometry/geometry.h>

//A cube is assumed to have side lengths of 1 and a center of <0,0,0>. This means all vertices are of the form <+/-0.5, +/-0.5, +/-0.5>
//These attributes can be altered by applying a transformation matrix to the cube.
class Cube : public Geometry
{
public:
    Intersection GetIntersection(Ray r);

    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    void create();


    virtual glm::mat3 GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal){return glm::mat3(1);}
};
