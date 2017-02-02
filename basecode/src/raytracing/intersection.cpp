#include <raytracing/intersection.h>
#include <iostream>

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    t(-1),
    object_hit(nullptr)
{
}

IntersectionEngine::IntersectionEngine()
    : scene(nullptr)
{
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
    //TODO
    QList<Intersection> Intersections = GetAllIntersections(r);

    Intersection result;

    if(Intersections.size() != 0){

        result = Intersections[0];

    }

    return result;
}


bool variantLessThan(const Intersection &v1, const Intersection &v2)
{
    return v1.t < v2.t;
}

QList<Intersection> IntersectionEngine::GetAllIntersections(Ray r){

    QList<Intersection> Intersections;

    //Here, wo loop every objects in scene to see whether there is an intersection
    for(Geometry* obj : scene->objects){


        //Since light source objects are also included in the objects QList,
        //so, if this intersection is the light source, we exclude it.
        if(obj->name == QString("Light Source") ||
           obj->name == QString("Light")){
                continue;
            }

        Intersection temp = obj->GetIntersection(r);



        if(temp.t != -1 && temp.object_hit != nullptr){

            Intersections.append(temp);
        }
    }

    //we sort intersections by t value
    qSort(Intersections.begin(), Intersections.end(), variantLessThan);


    return Intersections;
}
