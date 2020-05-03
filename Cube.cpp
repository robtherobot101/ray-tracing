/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cube class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cube.h"
#include <math.h>

/**
* Cubes's intersection method.  The input is a ray (pos, dir). 
*/
float Cube::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float min = MAXFLOAT;
    float next = top->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    next = bottom->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    next = left->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    next = right->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    next = front->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    next = back->intersect(posn, dir);
    if(next > 0 && next < min){
        min = next;
    }
    if(min == MAXFLOAT) return -1;
    else return min;
    
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cube.
*/
glm::vec3 Cube::normal(glm::vec3 p)
{
    if(top->isInside(p)){
        return -top->normal(p);
    }
    else if(bottom->isInside(p)){
        return -bottom->normal(p);
    }
    else if(left->isInside(p)){
        return -left->normal(p);
    }
    else if(right->isInside(p)){
        return -right->normal(p);
    }
    else if(front->isInside(p)){
        return -front->normal(p);
    }
    else return -back->normal(p);
}
