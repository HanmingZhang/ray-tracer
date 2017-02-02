#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;

Intersection Sphere::GetIntersection(Ray r)
{
    //TODO

    //Transform Ray to objecet space
    glm::mat4 inverse_modelMatrix = this->transform.invT();

    Ray objSpace_Ray = r.GetTransformedCopy(inverse_modelMatrix);


    //Compute quadratic sphere coefficients
    float dx = objSpace_Ray.direction.x;
    float dy = objSpace_Ray.direction.y;
    float dz = objSpace_Ray.direction.z;

    float ox = objSpace_Ray.origin.x;
    float oy = objSpace_Ray.origin.y;
    float oz = objSpace_Ray.origin.z;

    float radius = 0.5f;

    float a = dx * dx + dy * dy + dz * dz;
    float b = 2 * (dx * ox + dy * oy + dz * oz);
    float c = ox * ox + oy * oy + oz * oz - radius * radius;

    float discriminant = b * b - 4.0f * a * c;


    // If discriminat is negative, then we're done
    if(discriminant < 0.0f)
    {
        return Intersection();
    }

    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);

    if(t0 > 0.0f){
        Intersection result;
        result.t = t0;
        result.object_hit = this;

        result.point = objSpace_Ray.origin + objSpace_Ray.direction * t0;
        //since this is a sphere centered at origin, so the normal now is the same as intersection point
        result.normal = glm::normalize(result.point);
        glm::vec3 ori_normal = glm::normalize(result.point);

        //transform it from object space to world space
        result.point = glm::vec3(this->transform.T() * glm::vec4(result.point, 1.0f));
        result.normal = glm::vec3(this->transform.invTransT() * glm::vec4(result.normal, 0.0f));
        result.normal = glm::normalize(result.normal);


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


            glm::mat3 m = GetNormalMatrix(glm::vec3(this->transform.invT() * glm::vec4(result.point, 1.0f)),
                                          GetUVCoordinates(result.point),
                                          ori_normal);

            glm::vec3 result_normal = m * temp;

            result_normal = glm::normalize(result_normal);

            result.normal = result_normal;
        }
        //----------------------------------------------



        return result;
    }

    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);

    if(t1 > 0.0f){
        Intersection result;
        result.t = t1;
        result.object_hit = this;

        result.point = objSpace_Ray.origin + objSpace_Ray.direction * t1;
        //since this is a sphere centered at origin, so the normal now is the same as intersection point
        result.normal = glm::normalize(result.point);
        glm::vec3 ori_normal = glm::normalize(result.point);

        //transform it from object space to world space
        result.point = glm::vec3(this->transform.T() * glm::vec4(result.point, 1.0f));
        result.normal = glm::vec3(this->transform.invTransT() * glm::vec4(result.normal, 0.0f));
        result.normal = glm::normalize(result.normal);


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


            glm::mat3 m = GetNormalMatrix(glm::vec3(this->transform.invT() * glm::vec4(result.point, 1.0f)),
                                          GetUVCoordinates(result.point),
                                          ori_normal);

            glm::vec3 result_normal = m * temp;

            result_normal = glm::normalize(result_normal);

            result.normal = result_normal;
        }
        //----------------------------------------------

        return result;
    }


    else{
        return Intersection();
    }

}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}


void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}


void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}


glm::vec2 Sphere::GetUVCoordinates(const glm::vec3 &point){

    glm::vec3 localPosition = glm::vec3(transform.invT() * glm::vec4(point, 1.0f));

    float fei = acos(localPosition.y / 0.5);

    float theta;

    if(localPosition.x == 0){

        if(localPosition.z > 0){
            theta = PI / 2.0;
        }
        else{
            theta = - PI / 2.0;

        }
    }
    else{
      theta = atan(localPosition.z / localPosition.x);
    }


    if(localPosition.x < 0){
        theta = - theta - PI;
    }

    return glm::vec2((theta / (2.0 * PI)) + 0.75, (fei / PI));

}

glm::mat3 Sphere::GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal){

        glm::mat3 result = glm::mat3(1);

        glm::vec3 pos0 = local_pos;
        glm::vec3 pos1 = glm::vec3(0.5, 0, 0);
        glm::vec3 pos2 = glm::vec3(0, 0.5, 0);

        glm::vec2 uv0 = uv;
        glm::vec2 uv1 = glm::vec2(0.75, 0.5);
        glm::vec2 uv2 = glm::vec2(0.75, 0);



        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = pos1 - pos0;
        glm::vec3 deltaPos2 = pos2 - pos0;;

        // UV delta
        glm::vec2 deltaUV1 = uv1-uv0;
        glm::vec2 deltaUV2 = uv2-uv0;


        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

        tangent = glm::normalize(tangent);
        bitangent = glm::normalize(bitangent);

        result = glm::mat3(tangent, bitangent, ori_normal);

        return result;

}

