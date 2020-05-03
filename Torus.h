/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The torus class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_TORUS
#define H_TORUS
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Torus located at 'center'
 * with the specified radius
 */
class Torus : public SceneObject
{

private:
    glm::vec3 center;
    float outer_radius;
    float inner_radius;

public:

    Torus(glm::vec3 c, float i_r, float o_r, glm::vec3 col)
		: center(c), outer_radius(o_r), inner_radius(i_r)
	{
		color = col;
	};

	float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_TORUS
