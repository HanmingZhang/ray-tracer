#include <scene/materials/material.h>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    base_color(color),
    emissive(false),
    reflectivity(0),
    refract_idx_in(0),
    refract_idx_out(0),
    texture(nullptr),
    normal_map(nullptr)
{
}

glm::vec3 Material::GetImageColor(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == nullptr)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue())/255.0f;
    }
}



float clamp(float x, float a, float b){

    return std::max(a, std::min(x ,b));

}

glm::vec3 clamp_vec3(glm::vec3 x, float a, float b){

    return glm::vec3(clamp(x.x, a, b),clamp(x.y, a, b),clamp(x.z, a, b));
}


