#pragma once
#include <scene/geometry/geometry.h>

//A square is aligned with the XY plane with a normal aligned with the positive Z axis. Its side length is 1, and it is centered at the origin.
//These attributes can be altered by applying a transformation matrix to the square.
class SquarePlane : public Geometry
{
public:
    Intersection GetIntersection(Ray r);
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    void create();

    virtual glm::mat3 GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal){return glm::mat3(1);}
};
