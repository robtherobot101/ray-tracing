/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Plane.h"
#include "Cube.h"
//#include "Torus.h"
#include "Ray.h"
#include <GL/glut.h>
//#include "alglib/solvers.h"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 150;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

float theta = 0;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

void myTimer(int value)
{
  theta += 5;
	glutPostRedisplay();
	glutTimerFunc(20, myTimer, 0);
}


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 transparency(SceneObject * object, Ray ray, glm::vec3 colorSum, int step);

glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);
    glm::vec3 light(10, 40, -3);
    glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray
    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else get object's colour

    glm::vec3 colorSum = ambientCol * materialCol; // Light it with ambient light

    SceneObject *object = sceneObjects[ray.xindex]; // Get the incident scene object

    glm::vec3 normalVector = object->normal(ray.xpt); // Get normal vector

    // Generate reflections if necessary
    if(object->getRefli() != 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (object->getRefli()*reflectedCol);
    }
    // Generate a ray throught the object if it is transparent
    if(object->getTransi() != 0 && step < MAX_STEPS)
    {
        colorSum = transparency(object, ray, colorSum, step);
    }
    // Generate a shadow ray towards the light source
    float shadowIntensity = 1;
    Ray shadow(ray.xpt, light - ray.xpt);
    shadow.normalize();
    shadow.closestPt(sceneObjects);
    if(shadow.xindex > -1 && shadow.xdist < glm::distance(ray.xpt, light)) {
        if(sceneObjects[shadow.xindex]->getTransi() > 0) {
            shadowIntensity = sceneObjects[shadow.xindex]->getTransi();
        }
        else return colorSum;
    }
    glm::vec3 lightVector = glm::normalize(light - ray.xpt);
    float lDotn = glm::dot(lightVector, normalVector);
    if(lDotn >= 0) {
        colorSum += (1 - object->getTransi()) * shadowIntensity * lDotn * materialCol;

        glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
        float rDotv = glm::dot(reflVector, lightVector);
        if(rDotv < 0) rDotv = 0;
        else rDotv = pow(rDotv, 10);
        glm::vec3 specCol = rDotv * glm::vec3(1, 1, 1);

        colorSum += shadowIntensity * specCol;

    }

    return colorSum;
}

glm::vec3 transparency(SceneObject *object, Ray ray, glm::vec3 colorSum, int step) {
    Ray *transRay = new Ray(ray.xpt, ray.dir);
    transRay->closestPt(sceneObjects);
    if(sceneObjects[transRay->xindex] == object) {
        transRay = new Ray(transRay->xpt, ray.dir);
        transRay->closestPt(sceneObjects);
    }
    return (1 - object->getTransi()) * colorSum + (object->getTransi() * (trace(*transRay, step + 1)));
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    sceneObjects[4] = new Cube(glm::vec3(-5.0, 0, -40.0), 5.0, glm::vec3(0, 1, 1), 0, 0, theta);
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

            Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            ray.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);				//Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 0.0, -90.0), 15.0, glm::vec3(0, 0, 1), 0.8, 0.3);
    Sphere *sphere2 = new Sphere(glm::vec3(5.0, -15.0, -70.0), 5.0, glm::vec3(0, 1, 0), 0, 0);
    Sphere *sphere3 = new Sphere(glm::vec3(6, 3, -70.0), 4, glm::vec3(1, 0, 0), 0.8, 0.9);
    Sphere *sphere4 = new Sphere(glm::vec3(-5.0, 0, -40.0), 3, glm::vec3(1, 1, 0), 0, 0);

    // Create a floor plane
    Plane *plane = new Plane(glm::vec3(-200., -20, 0),	//Point A
                             glm::vec3(200., -20, 0),	//Point B
                             glm::vec3(200., -20, -250),	//Point C
                             glm::vec3(-200., -20, -250),	//Point D
                             glm::vec3(0.5, 0.5, 0), 0, 0);	//Colour

    // Create a cube
    Cube *cube = new Cube(glm::vec3(-5.0, 0, -40.0), 5.0, glm::vec3(0, 1, 1), 0, 0, theta);

    // Create a torus
    //Torus *torus = new Torus(glm::vec3(0, 0, -40), 2, 4, glm::vec3(1, 0, 1));

    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(plane);
    sceneObjects.push_back(cube);
    sceneObjects.push_back(sphere4);
    //sceneObjects.push_back(torus);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");
    glutTimerFunc(20, myTimer, 0);
    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
