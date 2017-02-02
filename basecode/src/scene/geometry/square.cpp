#include <scene/geometry/square.h>

Intersection SquarePlane::GetIntersection(Ray r)
{
    //TODO

    //Transform Ray to objecet space
    glm::mat4 inverse_modelMatrix = this->transform.invT();

    Ray objSpace_Ray = r.GetTransformedCopy(inverse_modelMatrix);

    glm::vec3 origin = objSpace_Ray.origin;
    glm::vec3 direction = objSpace_Ray.direction;

    glm::vec3 planeNormal = glm::vec3(0.0f, 0.0f, 1.0f);


    float t = glm::dot(planeNormal, -origin) / glm::dot(planeNormal, direction);

    //First we check whether is intersection on plane
    if(t < 0.0f){
        return Intersection();
    }

    else{
       glm::vec3 pointOnPlane = origin + t * direction;

       Intersection result;

       //Then we check whether this point is within the suqare
       if((pointOnPlane.x < 0.5f && pointOnPlane.x > -0.5f) &&
          (pointOnPlane.y < 0.5f && pointOnPlane.y > -0.5f)) {
               result.t = t;
               result.object_hit = this;
               result.point = glm::vec3(this->transform.T() * glm::vec4(pointOnPlane, 1.0f));
               result.normal = glm::vec3(this->transform.invTransT() * glm::vec4(planeNormal, 0.0f));
        }



       //----------------------------------------------
       //-------------- normal map --------------------
       //----------------------------------------------

       QImage* image = material->normal_map;
       if(image != nullptr){
           glm::vec2 uv_coord = GetUVCoordinates(result.point);

           int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
           int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);

           QColor normal_from_texture = image->pixel(X, Y);
           glm::vec3 temp =  glm::vec3((2.0 / 255.0) * normal_from_texture.red() - 1.0,
                                       (2.0 / 255.0) * normal_from_texture.green() - 1.0,
                                       (2.0 / 255.0) * normal_from_texture.blue() - 1.0);


           //glm::mat3 m = GetNormalMatrix(pointOnCube);

           glm::vec3 result_normal = temp;

           result_normal = glm::normalize(result_normal);

           result.normal = result_normal;
       }
       //----------------------------------------------




        return result;
    }
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}


glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point){

    glm::vec3 localPosition = glm::vec3(transform.invT() * glm::vec4(point, 1.0f));

    return(glm::vec2(localPosition.x + 0.5, 0.5 - localPosition.y));


}

