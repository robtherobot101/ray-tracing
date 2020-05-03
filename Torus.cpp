/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The torus class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Torus.h"
#include <math.h>

/**
* Torus's intersection method.  The input is a ray (pos, dir). 
*/
float Torus::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 vdif = posn - center;
    float b = glm::dot(dir, vdif);
    float len = glm::length(vdif);
    float c = len*len - outer_radius * outer_radius;
    float delta = b*b - c;
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;

	return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the torus.
*/
glm::vec3 Torus::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    glm::vec3 n2(n.x, 0, n.z);
    n = n - glm::normalize(n2) * ((outer_radius - 3 * inner_radius) / 2 );
    
    n = glm::normalize(n);
    return n;
}
