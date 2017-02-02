#include <raytracing/ray.h>

Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o),
    direction(d)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(glm::vec3(o), glm::vec3(d))
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction)
{
}

Ray Ray::GetTransformedCopy(const glm::mat4 &T) const
{
    //TODO: Implement this correctly!
    glm::vec4 homo_origin = glm::vec4(origin, 1.0f);
    glm::vec4 homo_direction = glm::vec4(direction, 0.0f);

    homo_origin = T * homo_origin;
    homo_direction = T * homo_direction;

    //origin = glm::vec3(homo_origin.x, homo_origin.y, homo_origin.z);
    //direction = glm::vec3(homo_direction.x, homo_direction.y, homo_direction.z);

    return Ray(homo_origin, homo_direction);
    //return *this;
}
