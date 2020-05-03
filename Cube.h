/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cube class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CUBE
#define H_CUBE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SceneObject.h"
#include "Plane.h"
#include "math.h"

/**
 * Defines a simple Cube located at 'center'
 * with the specified length
 */
class Cube : public SceneObject
{

private:
    glm::vec3 center;
    float length;
    Plane *top, *bottom, *left, *right, *front, *back;

public:

    Cube(glm::vec3 c, float l, glm::vec3 col, float reflection_index, float transparency_index, float theta)
        : center(c), length(l)
    {
        color = col;
        
        glm::mat4 rot(1.0f);
        rot = glm::rotate(rot, float(theta * M_PI / 180), glm::vec3(0.0f, 1.0, 0.0));
        
        
        // define vertices
        glm::vec3 v1(rot * glm::vec4( - length / 2,  + length / 2,  + length / 2, 1));
        glm::vec3 v2(rot * glm::vec4( - length / 2,  + length / 2,  - length / 2, 1));
        glm::vec3 v3(rot * glm::vec4( + length / 2,  + length / 2,  - length / 2, 1));
        glm::vec3 v4(rot * glm::vec4( + length / 2,  + length / 2,  + length / 2, 1));
        glm::vec3 v5(rot * glm::vec4( - length / 2,  - length / 2,  + length / 2, 1));
        glm::vec3 v6(rot * glm::vec4( - length / 2,  - length / 2,  - length / 2, 1));
        glm::vec3 v7(rot * glm::vec4( + length / 2,  - length / 2,  - length / 2, 1));
        glm::vec3 v8(rot * glm::vec4( + length / 2,  - length / 2,  + length / 2, 1));
        
        top = new Plane(center + v1, center + v2, center + v3, center + v4, color, reflection_index, transparency_index);

        bottom = new Plane(center + v8, center + v7, center + v6, center + v5, color, reflection_index, transparency_index);

        left = new Plane(center + v1, center + v5, center + v6, center + v2, color, reflection_index, transparency_index);

        right = new Plane(center + v3, center + v7, center + v8, center + v4, color, reflection_index, transparency_index);

        front = new Plane(center + v2, center + v6, center + v7, center + v3, color, reflection_index, transparency_index);
        
        back = new Plane(center + v1, center + v4, center + v8, center + v5, color, reflection_index, transparency_index);
        
        refli = reflection_index;
        transi = transparency_index;
    };

    float intersect(glm::vec3 posn, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CUBE
