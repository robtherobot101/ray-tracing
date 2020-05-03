/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 axis(0, -height, 0);
    glm::vec3 theta = glm::normalize(axis);
    float m = pow(radius, 2) / pow(glm::length(axis), 2);
    glm::vec3 w = (posn - (center + glm::vec3(0, height, 0)));

    float a = glm::dot(dir, dir) - m * (pow(glm::dot(dir, theta), 2)) - pow(glm::dot(dir, theta), 2);
    float b = 2.f * (glm::dot(dir, w) - m * glm::dot(dir, theta) * glm::dot(w, theta) - glm::dot(dir, theta) * glm::dot(w,theta));
    float c = glm::dot(w, w) - m * pow(glm::dot(w, theta), 2) - pow(glm::dot(w, theta), 2);


    float delta = b*b - 4 * a * c;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = ((-b - sqrt(delta)) / (2 * a));
    float t2 = ((-b + sqrt(delta)) / (2 * a));
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;
    float t;
    if (t1>t2) t = t2;
    else t = t1;

    float r = posn.y + t*dir.y;

    if ((r > center.y) and (r < center.y + height)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 v = glm::vec3(p.x - center.x, sqrt((p.x-center.x)*(p.x-center.x) + (p.z-center.z)*(p.z-center.z)), p.z - center.z);
    v = glm::normalize(v);
    return v;
}
