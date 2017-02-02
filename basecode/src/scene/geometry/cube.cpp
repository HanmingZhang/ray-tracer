#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;


void swap(float& x, float& y){
    if(x > y){
        float temp;
        temp = y;
        y = x;
        x = temp;
    }
}

Intersection Cube::GetIntersection(Ray r)
{
    //TODO
    //Transform Ray to objecet space
    glm::mat4 inverse_modelMatrix = this->transform.invT();

    Ray objSpace_Ray = r.GetTransformedCopy(inverse_modelMatrix);

    glm::vec3 origin = objSpace_Ray.origin;
    glm::vec3 direction = objSpace_Ray.direction;


    //First, we check parallel conditions
    if((direction.x == 0.0f) &&
       (origin.x < -0.5f || origin.x > 0.5f)){
        return Intersection();
    }

    if((direction.y == 0.0f) &&
       (origin.y < -0.5f || origin.y > 0.5f)){
        return Intersection();
    }

    if((direction.z == 0.0f) &&
       (origin.z < -0.5f || origin.z > 0.5f)){
        return Intersection();
    }

    //Second, we compute t0 and t1
    float t0, t1;
    float t_near = -999999.0f;
    float t_far = 999999.0f;

    // X part
    t0 = (-0.5f - origin.x) / direction.x;
    t1 = (0.5f - origin.x) / direction.x;
    swap(t0, t1);
    if(t0 > t_near) {t_near = t0;}
    if(t1 < t_far) {t_far = t1;}



    // Y part
    t0 = (-0.5f - origin.y) / direction.y;
    t1 = (0.5f - origin.y) / direction.y;
    swap(t0, t1);
    if(t0 > t_near) {t_near = t0;}
    if(t1 < t_far) {t_far = t1;}



    // Z part
    t0 = (-0.5f - origin.z) / direction.z;
    t1 = (0.5f - origin.z) / direction.z;
    swap(t0, t1);
    if(t0 > t_near) {t_near = t0;}
    if(t1 < t_far) {t_far = t1;}


    if(t_near > t_far){
        return Intersection();
    }

    float t;
    if(t_near < 0.0f && t_far <0.0f){
        return Intersection();
    }
    else{
        if(t_near >= 0.0f){t = t_near;}
        else{ t = t_far;}
    }

    glm::vec3 pointOnCube = origin + t * direction;
    glm::vec3 normal;
    Intersection result;

    result.t = t;
    result.object_hit = this;
    result.point = glm::vec3(this->transform.T() * glm::vec4(pointOnCube, 1.0f));

    float px = pointOnCube.x;
    float py = pointOnCube.y;
    float pz = pointOnCube.z;

    if(fabs(px) > fabs(py) && fabs(px) > fabs(pz)){
        if(px >= 0.0f){
            normal = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else{
            normal = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
    }

    if(fabs(py) > fabs(px) && fabs(py) > fabs(pz)){
        if(py >= 0.0f){
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else{
            normal = glm::vec3(0.0f, -1.0f, 0.0f);
        }
    }

    if(fabs(pz) > fabs(px) && fabs(pz) > fabs(py)){
        if(pz >= 0.0f){
            normal = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else{
            normal = glm::vec3(0.0f, 0.0f, -1.0f);
        }
    }

    result.normal = glm::vec3(this->transform.invTransT() * glm::vec4(normal, 0.0f));


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

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}


glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point){


    glm::vec3 localPosition = glm::vec3(transform.invT() * glm::vec4(point, 1.0f));

    if(fabs(localPosition.z - 0.5f) < 0.1f ||
       fabs(localPosition.z + 0.5f) < 0.1f){
        return glm::vec2(localPosition.x + 0.5, 0.5 - localPosition.y);
    }

    if(fabs(localPosition.y - 0.5f) < 0.1f ||
       fabs(localPosition.y + 0.5f) < 0.1f){
        return glm::vec2(localPosition.x + 0.5, 0.5 - localPosition.z);
    }

    if(fabs(localPosition.x - 0.5f) < 0.1f ||
       fabs(localPosition.x + 0.5f) < 0.1f){
        return glm::vec2(localPosition.z + 0.5, 0.5 - localPosition.y);
    }

    return glm::vec2(0.5f, 0.5f);
}


//glm::mat3 Cube::GetNormalMatrix (const glm::vec3 local_pos){

//    glm::vec3 localPosition = local_pos;

//    glm::mat3 result = glm::mat3(1);

//    glm::vec3 pos0 = local_pos;
//    glm::vec2 uv0 = GetUVCoordinates(pos0);

//    glm::vec3 pos1;
//    glm::vec2 uv1;

//    glm::vec3 pos2;
//    glm::vec2 uv2;

//    glm::vec3 n;


//    if(fabs(localPosition.x - 0.5f) < 0.1f){
//        pos1 = glm::vec3(0.5f, 0, 0);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(0.5f, 0, -0.1);
//        uv2 = GetUVCoordinates(pos2);

//        n = glm::vec3(1,0,0);
//    }

//    if(fabs(localPosition.x + 0.5f) < 0.1f){

//        pos1 = glm::vec3(-0.5f, 0, 0);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(-0.5f, 0, -0.1);
//        uv2 = GetUVCoordinates(pos2);

//        n = glm::vec3(-1,0,0);
//    }


//    if(fabs(localPosition.y - 0.5f) < 0.1f){
//        pos1 = glm::vec3(0, 0.5f, 0);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(0, 0.5f, -0.1);
//        uv2 = GetUVCoordinates(pos2);

//         n = glm::vec3(0,1,0);
//    }

//    if(fabs(localPosition.y + 0.5f) < 0.1f){

//        pos1 = glm::vec3(0, -0.5f, 0);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(0, -0.5f, -0.1);
//        uv2 = GetUVCoordinates(pos2);

//         n = glm::vec3(0,-1,0);
//    }

//    if(fabs(localPosition.z - 0.5f) < 0.1f){
//        pos1 = glm::vec3(0, 0, 0.5f);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(-0.1, 0, 0.5f);
//        uv2 = GetUVCoordinates(pos2);

//         n = glm::vec3(0,0,1);
//    }

//    if(fabs(localPosition.z + 0.5f) < 0.1f){

//        pos1 = glm::vec3(0, 0, -0.5f);
//        uv1 = GetUVCoordinates(pos1);

//        pos2 = glm::vec3(-0.1, 0, -0.5f);
//        uv2 = GetUVCoordinates(pos2);

//        n = glm::vec3(0,0,-1);
//    }

//    // Edges of the triangle : postion delta
//    glm::vec3 deltaPos1 = pos1 - pos0;
//    glm::vec3 deltaPos2 = pos2 - pos0;;

//    // UV delta
//    glm::vec2 deltaUV1 = uv1-uv0;
//    glm::vec2 deltaUV2 = uv2-uv0;


//    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
//    glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
//    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

//    tangent = glm::normalize(tangent);
//    bitangent = glm::normalize(bitangent);

//    result = glm::mat3(tangent, bitangent, n);

//}
