#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &position)
{
    float d0 = glm::distance(position, points[0]);
    float d1 = glm::distance(position, points[1]);
    float d2 = glm::distance(position, points[2]);
    return (normals[0] * d0 + normals[1] * d1 + normals[2] * d2)/(d0+d1+d2);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}

//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r)
{
    //TODO
    glm::vec3 origin = r.origin;
    glm::vec3 direction = r.direction;


    //Do calculation
    glm::vec3 E1 = points[1] - points[0];
    glm::vec3 E2 = points[2] - points[0];
    glm::vec3 T = origin - points[0];


    float coefficient = 1.0f / glm::dot(glm::cross(direction, E2) , E1);

    float t = coefficient * glm::dot(glm::cross(T, E1) , E2);
    float u = coefficient * glm::dot(glm::cross(direction, E2) , T);
    float v = coefficient * glm::dot(glm::cross(T, E1) , direction);




    //Now check t, u, v
    if( u >= 0.0f &&
        v >= 0.0f &&
        (u + v) <= 1.0f &&
        t >= 0.0f){

        Intersection result;

        result.t = t;
        result.object_hit = this;
        result.point = glm::vec3(this->transform.T() * glm::vec4((origin + t * direction), 1.0f));
        result.normal = glm::vec3(this->transform.invTransT() *
                        glm::vec4(glm::normalize(glm::cross(points[1] - points[0], points[2] - points[0])), 0.0f));

        glm::vec3 ori_normal = result.normal;


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


glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point){

    float s = 0.5 * glm::length(glm::cross(points[0] - points[1], points[2] - points[1]));
    float s1 = 0.5 * glm::length(glm::cross(point - points[1], points[2] - points[1]));
    float s2 = 0.5 * glm::length(glm::cross(point - points[2], points[0] - points[2]));
    float s3 = 0.5 * glm::length(glm::cross(point - points[0], points[1] - points[0]));

    return (uvs[0] * s1 / s) + (uvs[1] * s2 / s) + (uvs[2] * s3 / s);

}

glm::mat3 Triangle::GetNormalMatrix (glm::vec3 local_pos, glm::vec2 uv, glm::vec3 ori_normal){

    glm::mat3 result = glm::mat3(1);

    glm::vec3 pos0 = local_pos;
    glm::vec3 pos1 = points[1];
    glm::vec3 pos2 = points[2];

    glm::vec2 uv0 = uv;
    glm::vec2 uv1 = uvs[1];
    glm::vec2 uv2 = uvs[2];


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






Intersection Mesh::GetIntersection(Ray r)
{
    //TODO


    //First, we get all intersections of triangles
    QList<Intersection> Intersections;


    glm::mat4 inverse_modelMatrix = transform.invT();


    for(int i = 0; i < faces.size(); i++){

        //Transform Ray to objecet space
        //glm::mat4 inverse_modelMatrix = faces[i]->transform.invT();

        Ray objSpace_Ray = r.GetTransformedCopy(inverse_modelMatrix);

        Intersection temp = faces[i]->GetIntersection(objSpace_Ray);

        if(temp.t != -1 && temp.object_hit != nullptr){

            temp.object_hit = this;
            temp.point = glm::vec3(this->transform.T() * glm::vec4(temp.point, 1.0f));
            temp.normal = glm::vec3(this->transform.invTransT() * glm::vec4(temp.normal, 0.0f));


            Intersections.append(temp);
        }
    }


    //Then we find the intersection with smallest t value
    if(Intersections.size() == 0){
        return Intersection();
    }

    else{
        Intersection result = Intersections[0];

        for(int i = 1; i < faces.size(); i++){
            if (Intersections[i].t < result.t){
                result = Intersections[i];
            }
        }

        return result;
    }
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}


void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point){

        for(Triangle* each : faces){

            //here, we find which Triangle this point is on
            glm::vec3 test_vec3 = each->points[0] - point;
            if(fabs(glm::dot(test_vec3, each->plane_normal)) < 0.1){
                return each->GetUVCoordinates(point);
            }
        }

        return glm::vec2(0.5, 0.5);
}


//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
