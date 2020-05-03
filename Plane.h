/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_PLANE
#define H_PLANE

#include <glm/glm.hpp>
#include "SceneObject.h"

class Plane : public SceneObject
{
private:
    glm::vec3 a, b, c, d;  //The four vertices

public:	
	Plane(void);
	
    Plane(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pd, glm::vec3 col, float reflection_index, float transparency_index)
		: a(pa), b(pb), c(pc), d(pd)
	{
		color = col;
		refli = reflection_index;
		transi = transparency_index;
	};

	bool isInside(glm::vec3 pt);
	
	float intersect(glm::vec3 posn, glm::vec3 dir);
	
	glm::vec3 normal(glm::vec3 pos);

};

#endif //!H_PLANE
