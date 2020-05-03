/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CONE
#define H_CONE
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Cone located at 'center'
 * with the specified radius
 */
class Cone : public SceneObject
{

private:
    glm::vec3 center;
    float radius;
    float height;

public:

    Cone(glm::vec3 c, float r, float h, glm::vec3 col, float reflection_index, float transparency_index)
		: center(c), radius(r), height(h)
	{
		color = col;
		refli = reflection_index;
		transi = transparency_index;
	};

	float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CONE
