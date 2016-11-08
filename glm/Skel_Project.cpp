// Skel_Project.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <GL\AntTweakBar.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <iostream>
#include <fstream>
#include <string>
#define GLM_FORCE_RADIANS
#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"
using namespace std;
using namespace glm;

//Class to store value of each part
class Part
{

public:

	string name;
	//Setting default values 
	vec3 offset =  vec3(0.0);
	vec3 boxmin = vec3(-0.1);
	vec3 boxmax = vec3(0.1);
	vec2 rotxlimit = vec2(-100000, 100000);
	vec2 rotylimit = vec2(-100000, 100000);
	vec2 rotzlimit = vec2(-100000, 100000);
	vec3 pose = vec3(0.0);
	
	vec4 coOrds[8];
	mat4 localMatrix = mat4(1.0);
	mat4 worldMtx = mat4(1.0);
	

	int noChildParts = 0;
	//Pointer for no of Child nodes
	Part* childPart[15];

	Part()
	{

	}

	void frameCoordinates()
	{
		coOrds[0][0] = coOrds[1][0] = coOrds[2][0] = coOrds[3][0] = boxmin[0];
		coOrds[0][1] = coOrds[1][1] = coOrds[4][1] = coOrds[5][1] = boxmin[1];
		coOrds[1][2] = coOrds[2][2] = coOrds[5][2] = coOrds[6][2] = boxmin[2];
		coOrds[4][0] = coOrds[5][0] = coOrds[6][0] = coOrds[7][0] = boxmax[0];
		coOrds[2][1] = coOrds[3][1] = coOrds[6][1] = coOrds[7][1] = boxmax[1];
		coOrds[0][2] = coOrds[3][2] = coOrds[4][2] = coOrds[7][2] = boxmax[2];

		for (int i = 0; i < 8; i++){
			coOrds[i][3] = 1;
		}
			
	}

	void computeLocal()
	{
		localMatrix = translate(mat4(1.0), offset);
		localMatrix = rotate(localMatrix, pose[0], vec3(1, 0, 0));
		localMatrix = rotate(localMatrix, pose[1], vec3(0, 1, 0));
		localMatrix = rotate(localMatrix, pose[2], vec3(0, 0, 1));
	}


	void compWorldMtx(mat4 parentMtx)
	{
		worldMtx = parentMtx * localMatrix;
	}

	void compWorldVtx()
	{
		for (int i = 0; i < 8; i++)
			coOrds[i] = worldMtx * coOrds[i];
	}

	void DrawBox(float rotx, float roty, float rotz)
	{
		cout << "Draw :" << rotx << " \n";
		glRotatef(rotx, 1.0, 0.0, 0.0);
		glRotatef(roty, 0.0, 1.0, 0.0);
		glRotatef(rotz, 0.0, 0.0, 1.0);

		glColor3f(1, 1, 1);
		//LEFT
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[0][0], coOrds[0][1], coOrds[0][2]);
		glVertex3f(coOrds[1][0], coOrds[1][1], coOrds[1][2]);
		glVertex3f(coOrds[2][0], coOrds[2][1], coOrds[2][2]);
		glVertex3f(coOrds[3][0], coOrds[3][1], coOrds[3][2]);
		glEnd();

		//RIGHT
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[4][0], coOrds[4][1], coOrds[4][2]);
		glVertex3f(coOrds[5][0], coOrds[5][1], coOrds[5][2]);
		glVertex3f(coOrds[6][0], coOrds[6][1], coOrds[6][2]);
		glVertex3f(coOrds[7][0], coOrds[7][1], coOrds[7][2]);
		glEnd();

		//BACK
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[2][0], coOrds[2][1], coOrds[2][2]);
		glVertex3f(coOrds[1][0], coOrds[1][1], coOrds[1][2]);
		glVertex3f(coOrds[5][0], coOrds[5][1], coOrds[5][2]);
		glVertex3f(coOrds[6][0], coOrds[6][1], coOrds[6][2]);
		glEnd();

		//FRONT
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[0][0], coOrds[0][1], coOrds[0][2]);
		glVertex3f(coOrds[4][0], coOrds[4][1], coOrds[4][2]);
		glVertex3f(coOrds[7][0], coOrds[7][1], coOrds[7][2]);
		glVertex3f(coOrds[3][0], coOrds[3][1], coOrds[3][2]);
		glEnd();

		//BOTTOM
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[0][0], coOrds[0][1], coOrds[0][2]);
		glVertex3f(coOrds[4][0], coOrds[4][1], coOrds[4][2]);
		glVertex3f(coOrds[5][0], coOrds[5][1], coOrds[5][2]);
		glVertex3f(coOrds[1][0], coOrds[1][1], coOrds[1][2]);
		glEnd();

		//TOP
		glBegin(GL_LINE_LOOP);
		glVertex3f(coOrds[2][0], coOrds[2][1], coOrds[2][2]);
		glVertex3f(coOrds[6][0], coOrds[6][1], coOrds[6][2]);
		glVertex3f(coOrds[7][0], coOrds[7][1], coOrds[7][2]);
		glVertex3f(coOrds[3][0], coOrds[3][1], coOrds[3][2]);
		glEnd();

	}

	void draw(mat4 parentMtx, float rotx, float roty, float rotz)
	{
		glPushMatrix();
		
		computeLocal();
		compWorldMtx(parentMtx);
		compWorldVtx();
		DrawBox(rotx, roty, rotz);
		glPopMatrix();
		for (int i = 0; i < noChildParts; i++)
		{
			childPart[i]->draw(worldMtx, rotx, roty, rotz);
		}

	}


	~Part()
	{
	}

};
//End of Class

Part rt;
//Rotation variables
float rotx, roty, rotz;

void fetchVal(ifstream& f, vec3& v)
{
	string val;

	for (int i = 0; i < 3; i++){
		f >> val;
		v[i] = stof(val);
		cout << v[i] << "  ";
	}
	cout << "\n";
}

void fetchRotVal(ifstream& f, vec2& v)
{
	string val;
	//Rotation has only two values
	for (int i = 0; i < 2; i++){
		f >> val;
		v[i] = stof(val); //Degrees to radian
		cout << v[i] << "  ";
	}
	cout << "\n";
	
}

void fetchValues(ifstream& file, Part* p)

{
	string value;

	file >> value;
	p->name = value;
	cout << value << "\n";

	//Skip {
	file >> value; 
	file >> value;

	while (value != "}") //Will return to parent when } is encountered
	{
		if (value == "offset")
		{
			cout << value <<"  ";
			fetchVal(file, p->offset);
			file >> value;
		}

		if (value == "boxmin")
		{
			cout << value << "  ";
			fetchVal(file, p->boxmin);
			file >> value;
		}

		if (value == "boxmax")
		{
			cout << value << "  ";
			fetchVal(file, p->boxmax);
			file >> value;
		}

		if (value == "rotxlimit")
		{
			cout << value << "  ";
			fetchRotVal(file, p->rotxlimit);
			file >> value;
		}

		if (value == "rotylimit")
		{
			cout << value << "  ";
			fetchRotVal(file, p->rotylimit);
			file >> value;
		}

		if (value == "rotzlimit")
		{
			cout << value << "  ";
			fetchRotVal(file, p->rotzlimit);
			file >> value;
		}

		if (value == "pose")
		{
			cout << value << "  ";
			fetchVal(file, p->pose);
			file >> value;
		}

		p->frameCoordinates();


		if (value == "balljoint")
		{
			cout << value << " " << p->noChildParts << "\n";

			p->childPart[p->noChildParts] = new Part();
			Part* pc = p->childPart[p->noChildParts];
			p->noChildParts = p->noChildParts + 1;

			//Fetch values recursively for Child Parts
			fetchValues(file, pc);
			file >> value;
		}
	}

	return;

}

void loadFile()
{
	ifstream f;

	f.open("dragon.skel");
	
	if (!f.is_open()) return;

	string word;
	while (f >> word)
	{
		//Starts with balljoint
		if (word == "balljoint")
		{
			fetchValues(f, &rt);
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rt.draw(mat4(1.0), rotx, roty, rotz);
	glutSwapBuffers();
}

int oldX, oldY;
bool rot = false;
float theta = 0, phi = 0;
void OnMouseDown(int button, int state, int x, int y) {
	rot = false;
	if (button == GLUT_LEFT_BUTTON) {
		oldX = x;
		oldY = y;
		rot = true;
	}
}

void OnMouseMove(int x, int y) {
	if (rot) {
		//you might need to adjust this multiplier(0.01)
		theta += (x - oldX)*0.01f;
		phi += (y - oldY)*0.01f;
	}
	oldX = x;
	oldY = y;
	glutPostRedisplay();
}
float eyeX = 20;
float eyeY = 0;
float eyeZ = 15;
float pickObjX = 0;
float pickObjY = 0;
float pickObjZ = 0;
float radius = 200;
void OnRender() {
	eyeX = pickObjX + radius*cos(phi)*sin(theta);
	eyeY = pickObjY + radius*sin(phi)*sin(theta);
	eyeZ = pickObjZ + radius*cos(theta);

	//assuming modelview matrix mode is set 
	gluLookAt(eyeX, eyeY, eyeZ, pickObjX, pickObjY, pickObjZ, 0, 1, 0);

	//rest of stuff ...
	glutSwapBuffers();
}

void keyboard(unsigned char c, int x, int y)
{
	switch (c){
	case 'x':
		rotx += 1;
		break;
	}
	glutPostRedisplay();
}
// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, (double)width / height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cout << "Reshape";
	gluLookAt(20.0, 0, 15.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	glTranslatef(0, 0.6f, -1);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("Skel File draw - Dragon");
	glClearColor(0.0, 0.0, 0.0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	loadFile();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(OnMouseDown);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}

