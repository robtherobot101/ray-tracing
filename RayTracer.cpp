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
#include "Cone.h"
#include "TextureBMP.h"
#include "Ray.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 700;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
TextureBMP *texture = new TextureBMP((char*) "texture.bmp");
TextureBMP *background = new TextureBMP((char*) "bliss.bmp");
int view = 0;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


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

    // Make a chequered floor
    if(ray.xindex == 0)
    {
        if((int(floor(ray.xpt.x / 5)) % 2 + int(floor(ray.xpt.z / 5)) % 2) % 2) {
            materialCol = glm::vec3(1, 0, 0);
        }
        else materialCol = glm::vec3(1, 1, 1);
    }

    SceneObject *object = sceneObjects[ray.xindex]; // Get the incident scene object

    glm::vec3 normalVector = object->normal(ray.xpt); // Get normal vector

// Procedural Cone spiral texture
    if(ray.xindex == 4)
    {
        if(fmod(ray.xpt.y + 80 + 3 * atan((normalVector.x + 1) / (normalVector.z + 1)) / M_PI, 3) < 1) {
            materialCol = glm::vec3(0, 1, 0);
        }
        else {
            materialCol = glm::vec3(1, 0, 1);
        }

    }

    // Textured sphere
    if(ray.xindex == 3)
    {
        materialCol = texture->getColorAt((normalVector.x + 1) / 2, (normalVector.y + 1) / 2);

    }
    
    // Backdrop
    if(ray.xindex == 6){
	materialCol = background->getColorAt((ray.xpt.x + 400) / 800, (ray.xpt.y + 180) / 580);
    }

    glm::vec3 colorSum = ambientCol * materialCol; // Light it with ambient light


    // Generate a ray throught the object if it is transparent
    if(object->getTransi() != 0 && step < MAX_STEPS)
    {
        colorSum = transparency(object, ray, colorSum, step);
    }

    //Refraction
    if(ray.xindex == 2 && step < MAX_STEPS) {
        glm::vec3 g = glm::refract(ray.dir, normalVector, 1.1f);
        Ray refrRay1(ray.xpt, g);
        refrRay1.normalize();
        refrRay1.closestPt(sceneObjects);
        if(refrRay1.xindex > -1) {
            glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
            glm::vec3 h = glm::refract(g, -m, 1.0f/1.1f);
            Ray aray(refrRay1.xpt, glm::normalize(h));
            aray.normalize();
            colorSum = trace(aray, step+1);
        }
    }

    //Generate a shadow ray towards the light source
    bool b = true;
    float shadowIntensity = 1;
    Ray shadow(ray.xpt, light - ray.xpt);
    shadow.normalize();
    shadow.closestPt(sceneObjects);
    if(shadow.xindex > -1 && shadow.xdist < glm::distance(ray.xpt, light)) {
        if(sceneObjects[shadow.xindex]->getTransi() > 0) {
            shadowIntensity = sceneObjects[shadow.xindex]->getTransi();
        }
        else b = false;
    }
    if(b) {
        float rDotv;
        glm::vec3 lightVector = glm::normalize(light - ray.xpt);
        float lDotn = glm::dot(lightVector, normalVector);
        if(lDotn >= 0) {
            colorSum += (1 - object->getTransi()) * shadowIntensity * lDotn * materialCol;

            glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
            rDotv = glm::dot(reflVector, lightVector);
            if(rDotv < 0) rDotv = 0;
            else rDotv = pow(rDotv, 10);
            glm::vec3 specCol = rDotv * glm::vec3(1, 1, 1);
            if(ray.xindex < 6) colorSum += shadowIntensity * specCol;

        }
    }

    // Generate reflections if necessary
    if(object->getRefli() != 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        reflectedRay.closestPt(sceneObjects);
        if(reflectedRay.xindex > -1) {
            glm::vec3 reflectedCol = trace(reflectedRay, step+1);
            colorSum = (1.2f - object->getRefli())*colorSum +  object->getRefli()*(object->getRefli()*reflectedCol);
        }
    }

    return colorSum;
}

glm::vec3 transparency(SceneObject *object, Ray ray, glm::vec3 colorSum, int step) {
    Ray *transRay = new Ray(ray.xpt, ray.dir);
    transRay->closestPt(sceneObjects);
    if(sceneObjects[transRay->xindex] == object) {
        transRay = new Ray(transRay->xpt, ray.dir);
        transRay->normalize();
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
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye;
    if(view == 0) eye = glm::vec3(0., 0., 0.);
    else if(view == 1) eye = glm::vec3(-300., 0., -130.);
    else if(view == 2) eye = glm::vec3(0., 0., -300.);
    else eye = glm::vec3(300., 0., -130.);

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
            glm::vec3 dir;

            if(view == 0) dir = glm::vec3(xp+0.5*cellX, yp+0.5*cellY, -EDIST);
            else if(view == 1) dir = glm::vec3(EDIST, yp+0.5*cellY, xp+0.5*cellX);
            else if(view == 2) dir = glm::vec3(-xp-0.5*cellX, yp+0.5*cellY, EDIST);
            else dir = glm::vec3(-EDIST, yp+0.5*cellY, -xp-0.5*cellX);


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

void special(int key, int x, int y)
{
    if(key==GLUT_KEY_LEFT) view = (view + 3) % 4;	 //next view
    else if(key==GLUT_KEY_RIGHT) view = (view + 1) % 4;   //previous view
    glutPostRedisplay();
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
    Sphere *sphere1 = new Sphere(glm::vec3(-15.0, -5.0, -140.0), 15, glm::vec3(0, 0, 1), 0.8, 0.6);
    Sphere *sphere2 = new Sphere(glm::vec3(11, -15.0, -85), 5.0, glm::vec3(0, 0, 0), 0, 1);
    Sphere *sphere3 = new Sphere(glm::vec3(10, 5, -70.0), 4, glm::vec3(1, 0, 0), 0, 0);

    // Create a floor plane
    Plane *plane = new Plane(glm::vec3(-200., -20, 0),	//Point A
                             glm::vec3(200., -20, 0),	//Point B
                             glm::vec3(200., -20, -250),	//Point C
                             glm::vec3(-200., -20, -250),	//Point D
                             glm::vec3(0.5, 0.5, 0), 0.5, 0);	//Colour

    // Create a cube
    Cube *cube = new Cube(glm::vec3(20.0, -16, -200.0), 8.0, glm::vec3(0, 1, 1), 0, 0, -45);

    // Create a Cone
    Cone *cone = new Cone(glm::vec3(0, -20, -130), 5, 15, glm::vec3(0, 0, 0), 0, 0);


    Plane *back = new Plane(glm::vec4( - 400,  + 400,  - 1400, 1),
                            glm::vec4( - 400,  - 180,  - 1400, 1),
                            glm::vec4( + 400,  - 180,  - 1400, 1),
                            glm::vec4( + 400,  + 400,  - 1400, 1),
                            glm::vec3(0.5, 0.7, 0.2), 0, 0);

    //--Add the above to the list of scene objects.
    sceneObjects.push_back(plane);
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(cone);
    sceneObjects.push_back(cube);
    sceneObjects.push_back(back);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    glutSpecialFunc(special);
    initialize();

    glutMainLoop();
    return 0;
}
