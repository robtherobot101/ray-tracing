/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_SPHERE
#define H_SPHERE
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Sphere : public SceneObject
{

private:
    glm::vec3 center;
    float radius;

public:
	Sphere()
		: center(glm::vec3(0)), radius(1)  //Default constructor creates a unit sphere
	{
		color = glm::vec3(1);
	};

    Sphere(glm::vec3 c, float r, glm::vec3 col, float reflection_index, float transparency_index)
		: center(c), radius(r)
	{
		color = col;
		refli = reflection_index;
		transi = transparency_index;
	};

	float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_SPHERE
