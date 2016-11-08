// Skel_Skin.cpp : Defines the entry point for the console application.
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

//Global variable for storing ith World Matrix
static int wmCount = 0;
static int storingCnt = 0;

class GlobalWorldMatrix{
public:
	mat4 gwm = mat4(1.0);
};
GlobalWorldMatrix * globalWorldMat = NULL;

class Triangle{
public:
	vec3 trngPos = vec3(-1.0);
};

class BindingMatrix{
public:
	mat4 bndgMtx = mat4(1.0f);

	mat4 getInverse(){
		return inverse(bndgMtx);
	}
};

class SkinVert
{

public:
	
	vec3 pos = { vec3(0.0f) };
	vec3 norm = { vec3(0.0f) };
	float sknWgt[4];
	int noJnts = 0;
	int jointNo[4];

	SkinVert(){
		for (int i = 0; i < 4; i++){
			sknWgt[i] = -1.0f;
			jointNo[i] = -1;
		}
		
	}
		
};

//Class to store value of each part
class Part
{

public:

	string name;
	bool isroot = false;
	//Setting default values 
	vec3 offset = vec3(0.0);
	vec3 boxmin = vec3(-0.1);
	vec3 boxmax = vec3(0.1);
	vec2 rotxlimit = vec2(-100000, 100000);
	vec2 rotylimit = vec2(-100000, 100000);
	vec2 rotzlimit = vec2(-100000, 100000);
	vec3 pose = vec3(0.0);

	vec4 fileCoOrds[8];
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
		fileCoOrds[0][0] = fileCoOrds[1][0] = fileCoOrds[2][0] = fileCoOrds[3][0] = boxmin[0];
		fileCoOrds[0][1] = fileCoOrds[1][1] = fileCoOrds[4][1] = fileCoOrds[5][1] = boxmin[1];
		fileCoOrds[1][2] = fileCoOrds[2][2] = fileCoOrds[5][2] = fileCoOrds[6][2] = boxmin[2];
		fileCoOrds[4][0] = fileCoOrds[5][0] = fileCoOrds[6][0] = fileCoOrds[7][0] = boxmax[0];
		fileCoOrds[2][1] = fileCoOrds[3][1] = fileCoOrds[6][1] = fileCoOrds[7][1] = boxmax[1];
		fileCoOrds[0][2] = fileCoOrds[3][2] = fileCoOrds[4][2] = fileCoOrds[7][2] = boxmax[2];

		for (int i = 0; i < 8; i++){
			fileCoOrds[i][3] = 1;
		}


	}

	void checkLimits(){
		if (pose[0] < rotxlimit[0]) {
			pose[0] = rotxlimit[0];
		}
		else if (pose[0] > rotxlimit[1]) {
			pose[0] = rotxlimit[1];
		}
		if (pose[1] < rotylimit[0]) {
			pose[1] = rotylimit[0];
		}
		else if (pose[1] > rotylimit[1]) {
			pose[1] = rotylimit[1];
		}
		if (pose[2] < rotzlimit[0]) {
			pose[2] = rotzlimit[0];
		}
		else if (pose[2] > rotzlimit[1]) {
			pose[2] = rotzlimit[1];
		}
	}

	void computeLocal(float x, float y, float z, string djn)
	{
	
		localMatrix = translate(mat4(1.0), offset);
		
		//cout << "Joint Name: " << name <<"\n";
		checkLimits();
		if (name == djn){
			cout << "Bending "<<name<<" "<<x<<" "<<y<<" "<<z<<"\n";
			localMatrix = rotate(localMatrix, pose[2]+z, vec3(0, 0, 1));
			localMatrix = rotate(localMatrix, pose[1]+y, vec3(0, 1, 0));
			localMatrix = rotate(localMatrix, pose[0]+x, vec3(1, 0, 0));
		}
		else {
			localMatrix = rotate(localMatrix, pose[2], vec3(0, 0, 1));
			localMatrix = rotate(localMatrix, pose[1], vec3(0, 1, 0));
			localMatrix = rotate(localMatrix, pose[0], vec3(1, 0, 0));
		}
		

	}


	void compWorldMtx(mat4 parentMtx)
	{
		worldMtx = parentMtx * localMatrix;
	}

	void compWorldVtx()
	{
		for (int i = 0; i < 8; i++)
			coOrds[i] = worldMtx * fileCoOrds[i];
	}

	void DrawBox()
	{
		glColor3f(0, 0, 0);
		//glColor3f(1.0, 1.0, 1.0);
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

	void printMatrix(mat4 matrix){
		cout << "Mat :\n";
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				cout << matrix[i][j] << " ";
			}
			cout << "\n";
		}
	}

	void draw(mat4 parentMtx, float rotx, float roty, float rotz, string djn)
	{

		glPushMatrix();
		computeLocal(rotx, roty, rotz, djn);
		compWorldMtx(parentMtx);
		if (storingCnt < wmCount){
			globalWorldMat[storingCnt++].gwm = worldMtx;
		}
		//cout << "Parent : " << worldMtx[0][0] << "\n";
		//cout << name << " CoOrds b4: " << fileCoOrds[0][0] << "\n";
		compWorldVtx();
		//cout << "Local : " << localMatrix[0][0] << "   " << localMatrix[0][1] << "   " << localMatrix[0][2] << "   " << localMatrix[0][3] << "\n";
		//cout <<name << " CoOrds : " << coOrds[0][0] << "\n";
		//DrawBox();

		for (int i = 0; i < noChildParts; i++)
		{
			childPart[i]->draw(worldMtx, rotx, roty, rotz, djn);

		}
		glPopMatrix();

	}


	~Part()
	{
	}

};
//End of Class

class TrnSkinV{
public:
	vec4 sv = vec4(0.0);
	vec4 tnorm = vec4(0.0);

};

Part rt;
SkinVert * vert = NULL;
int noPos;
int noTrng;
Triangle * trng = NULL;
int noBndMat;
BindingMatrix * bndgMtx = NULL;

//Change Positions
GLfloat rotx, roty, rotz = 0;
string dofJointName = "";

void fetchVal(ifstream& f, vec3& v)
{
	string val;

	for (int i = 0; i < 3; i++){
		f >> val;
		v[i] = stof(val);
		//cout << v[i] << "  ";
	}
	//cout << "\n";
}

void fetchRotVal(ifstream& f, vec2& v)
{
	string val;
	//Rotation has only two values
	for (int i = 0; i < 2; i++){
		f >> val;
		v[i] = stof(val); //Degrees to radian
		//cout << v[i] << "  ";
	}
	//cout << "\n";

}

void fetchValues(ifstream& file, Part* p)

{
	//Increment World Matrix Count for every joint
	wmCount++;
	string value;

	file >> value;
	p->name = value;
	//cout << value << "\n";

	//Skip {
	file >> value;
	file >> value;

	while (value != "}") //Will return to parent when } is encountered
	{
		if (value == "offset")
		{
			//	cout << value << "  ";
			fetchVal(file, p->offset);
			file >> value;
		}

		if (value == "boxmin")
		{
			//cout << value << "  ";
			fetchVal(file, p->boxmin);
			file >> value;
		}

		if (value == "boxmax")
		{
			//cout << value << "  ";
			fetchVal(file, p->boxmax);
			file >> value;
		}

		if (value == "rotxlimit")
		{
			//cout << value << "  ";
			fetchRotVal(file, p->rotxlimit);
			file >> value;
		}

		if (value == "rotylimit")
		{
			//cout << value << "  ";
			fetchRotVal(file, p->rotylimit);
			file >> value;
		}

		if (value == "rotzlimit")
		{
			//cout << value << "  ";
			fetchRotVal(file, p->rotzlimit);
			file >> value;
		}

		if (value == "pose")
		{
			//cout << value << "  ";
			fetchVal(file, p->pose);
			file >> value;
		}

		p->frameCoordinates();


		if (value == "balljoint")
		{
			//cout << "\n" << p->name << " child " << p->noChildParts << "\n";

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

void loadFile(string fname)
{
	ifstream f;
	 
	f.open(fname);
	cout << "Loading File: " + fname << "\n";
	if (!f.is_open()) {
		cout << "File not found!!!\n";
		//return;
		system("pause");
		exit(0);
	}

	string word;
	while (f >> word)
	{
		//Starts with balljoint
		if (word == "balljoint")
		{
			rt.isroot = true;
			fetchValues(f, &rt);
		}
	}
	globalWorldMat = new GlobalWorldMatrix[wmCount];
	
}

void fetchMatrixValues(ifstream& f, mat4& bndgMtx){
	string val;
	//Skip {
	f >> val;

	//The binding matrix is a 4x3 matrix
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 3; j++){
			f >> val;
			bndgMtx[i][j] = stof(val);
		}
	}
	f >> val;
	if (val == "}")
		return;

}

void fetchBindingMtx(ifstream& f, BindingMatrix* b, int n) {
	string value;
	//Skip {
	f >> value;

	for (int i = 0; i < n; i++){
		f >> value;
		if (value == "matrix"){
			fetchMatrixValues(f, b->bndgMtx);
			b++;
		}
	}
	f >> value;
	if (value == "}")
		return;
}

void fetchTriangleCoOrds(ifstream& f, Triangle* trng, int n) {
	string value;
	//Skip {
	f >> value;

	for (int i = 0; i < n; i++){
		f >> value;
		trng[i].trngPos[0] = stoi(value);

		f >> value;
		trng[i].trngPos[1] = stoi(value);

		f >> value;
		trng[i].trngPos[2] = stoi(value);
	}

	f >> value;
	if (value == "}")
		return;
}

void fetchVertSknWt(ifstream& f, SkinVert* vert, int n){

	string value;
	//Skip {
	f >> value;
	//cout << "Skin Weight Reading: \n";
	for (int i = 0; i < n; i++){
		//Read no of joints
		f >> value;
		//cout << "No Joints : "<< value << " ";
		vert[i].noJnts = stoi(value);
		for (int j = 0; j < vert[i].noJnts; j++){	
			//Read joint no
			f >> value;
			//cout <<"Joint[" << j <<"]: "<< value << " ";
			(vert[i]).jointNo[j] = stoi(value);

			//Read joint weight
			f >> value;
			//cout << "Weight[" << j << "]: " << value << " ";
			(vert[i]).sknWgt[j] = stof(value);
		}
		//cout << "\n";
	}
	//cout << "End skin read";
	f >> value;
	if (value == "}")
		return;

}


void fetchVertPos(ifstream& f, SkinVert *vert, int n){
	string value;

	//Skip {
	f >> value;

	//cout << "Positions \n";
	for (int i = 0; i < n; i++) {
		f >> value;
		vert[i].pos[0] = stof(value);
		//cout << vert[i].pos[0] << " ";

		f >> value;
		vert[i].pos[1] = stof(value);
		//cout << vert[i].pos[1] << " ";

		f >> value;
		vert[i].pos[2] = stof(value);
		//cout << vert[i].pos[2] << "\n";
	}
	f >> value;
	if (value == "}")
		return;

}

void fetchVertNorm(ifstream& f, SkinVert* vert, int n){
	string value;

	//Skip {
	f >> value;

	for (int i = 0; i < n; i++) {
		f >> value;
		vert[i].norm[0] = stof(value);
		
		f >> value;
		vert[i].norm[1] = stof(value);
		
		f >> value;
		vert[i].norm[2] = stof(value);
		
	}
	f >> value;
	if (value == "}")
		return;
}

void printVal(SkinVert* v){
	cout << "Positions: \n";
	for (int i = 0; i < noPos; i++){
		cout << v[i].pos[0] << " ";
		cout << v[i].pos[1] << " ";
		cout << v[i].pos[2] << "\n";
	}
	cout << "Normal: \n";
	for (int i = 0; i < noPos; i++){
		cout << v[i].norm[0] << " ";
		cout << v[i].norm[1] << " ";
		cout << v[i].norm[2] << "\n";
	}
	
	cout << "Skin Weights: \n";
	for (int i = 0; i < noPos; i++){
		
		cout << v[i].noJnts << " ";
		for (int j = 0; j < v[i].noJnts; j++)
			cout << v[i].jointNo[j] << " " << v[i].sknWgt[j] << " ";
		cout << "\n";
	}
	
}

void printTrng(Triangle* t){
	cout << "Triangles: \n";
	for (int i = 0; i < noTrng; i++) {
		cout << t[i].trngPos[0] << " ";
		cout << t[i].trngPos[1] << " ";
		cout << t[i].trngPos[2] << "\n";
	}
}

void printBndMtx(BindingMatrix* b){
	cout << "Binnding Matrix: \n";
	for (int k = 0; k < noBndMat; k++){
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				cout << b[k].bndgMtx[i][j] << " ";
			}
			cout << "\n";
		}
	}

}

void loadSkinFile(string fname)
{
	ifstream sfStrm;
	sfStrm.open(fname);
	if (!sfStrm.is_open())
		return;

	string word;
	while (sfStrm >> word) {
		//Starts with Position
		if (word == "positions"){

			//Fetch no of Positions
			sfStrm >> word;
			noPos = stoi(word);
		//	cout << "Pos:" << word << "\n";
			vert = new SkinVert[noPos];
			fetchVertPos(sfStrm, vert, noPos);
			sfStrm >> word;
		}

		if (word == "normals"){
			//Ignore no of normals, since same as no of pos
			sfStrm >> word;
			//cout << "Norm:" << word << "\n";
			fetchVertNorm(sfStrm, vert, noPos);
			sfStrm >> word;
		}

		if (word == "skinweights") {
			//ignore no of skin weights as this is also same as no of pos
			sfStrm >> word;
			//cout << "Skin Wt:" << word << "\n";
			fetchVertSknWt(sfStrm, vert, noPos);
			sfStrm >> word;
		}

		if (word == "triangles") {
			//Get no of triangles
			sfStrm >> word;
			noTrng = stoi(word);
			trng = new Triangle[noTrng];
			fetchTriangleCoOrds(sfStrm, trng, noTrng);
			sfStrm >> word;
		}

		if (word == "bindings") {
			//Get no of Binding Matrices
			sfStrm >> word;
			noBndMat = stoi(word);
			bndgMtx = new BindingMatrix[noBndMat];
			fetchBindingMtx(sfStrm, bndgMtx, noBndMat);
			sfStrm >> word;
		}
	}
	//printVal(vert);
	//printTrng(trng);
	//printBndMtx(bndgMtx);
}

void printWorldMatrix(){
	cout << "World Matrix:\n";
	for (int k = 0; k < wmCount; k++){
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				cout << globalWorldMat[k].gwm[i][j] << " ";
			}
			cout << "\n";
		}
	}
	
}

void printMatrix(mat4 matrix){
	cout << "Mat :\n";
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			cout << matrix[i][j] << " ";
		}
		cout << "\n";
	}
}

void computeVertices(vec4& v, SkinVert vert, vec4& tn){
	vec4 posv = vec4(vert.pos, 1.0f);
	vec4 normv = vec4(vert.norm, 0.0f);
	mat4 sum = mat4(0.0f);
	for (int i = 0; i < vert.noJnts; i++){
		//printMatrix(globalWorldMat[vert.jointNo[i]].gwm);
		//printMatrix(bndgMtx[vert.jointNo[i]].getInverse());
		//system("pause");
		int j = vert.jointNo[i];
		//cout << j << "\n";
		//v += vert.sknWgt[i] * (globalWorldMat[j].gwm * bndgMtx[j].getInverse()) * posv;
		sum += vert.sknWgt[i] * (globalWorldMat[j].gwm * bndgMtx[j].getInverse());
	}
	//system("pause");
	//printMatrix(sum);
	v = sum * posv;
	tn = normalize(sum * normv);
	//cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << "\n";
	//system("pause");
}

TrnSkinV * tsv;
void compute(){
	tsv = new TrnSkinV[noPos];
	for (int i = 0; i < noPos; i++){
		computeVertices(tsv[i].sv, vert[i], tsv[i].tnorm);
	}
}

void drawSkin(){
	glPushMatrix();
	for (int i = 0; i < noTrng; i++){
		int v0 = trng[i].trngPos[0];
		int v1 = trng[i].trngPos[1];
		int v2 = trng[i].trngPos[2];
		//cout << tsv[v0].sv[0] << " " << tsv[v0].sv[1] << " " << tsv[v0].sv[2] << "\n";
		//cout << tsv[v1].sv[0] << " " << tsv[v1].sv[1] << " " << tsv[v1].sv[2] << "\n";
		//cout << tsv[v2].sv[0] << " " << tsv[v2].sv[1] << " " << tsv[v2].sv[2] << "\n";
		//system("pause");
		glBegin(GL_TRIANGLES);
		glColor3f(1, 1, 1);
		glNormal3f(tsv[v0].tnorm[0], tsv[v0].tnorm[1], tsv[v0].tnorm[2]);
		glVertex3f(tsv[v0].sv[0], tsv[v0].sv[1], tsv[v0].sv[2]);
		glNormal3f(tsv[v1].tnorm[0], tsv[v1].tnorm[1], tsv[v1].tnorm[2]);
		glVertex3f(tsv[v1].sv[0], tsv[v1].sv[1], tsv[v1].sv[2]);
		glNormal3f(tsv[v2].tnorm[0], tsv[v2].tnorm[1], tsv[v2].tnorm[2]);
		glVertex3f(tsv[v2].sv[0], tsv[v2].sv[1], tsv[v2].sv[2]);
		glEnd();
		
	}
	glPopMatrix();
}

void display()
{
	cout << "Calling display \n";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	storingCnt = 0;
	rt.draw(mat4(1.0), rotx, roty, rotz, dofJointName);
	//printWorldMatrix();
	compute();
	drawSkin();
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
		glRotatef(1.0, 1.0, 0.0, 0.0);
		break;
	case 'y':
		glRotatef(1.0, 0.0, 1.0, 0.0);
		break;
	case 'z':
		glRotatef(1.0, 0.0, 0.0, 1.0);
		break;
	case 'X':
		glRotatef(-1.0, 1.0, 0.0, 0.0);
		break;
	case 'Y':
		glRotatef(-1.0, 0.0, 1.0, 0.0);
		break;
	case 'Z':
		glRotatef(-1.0, 0.0, 0.0, 1.0);
		break;
	//For Wasp
	case '1':
		dofJointName = "head";
		rotx = roty = rotz = 0;
		break;
	case '2':
		dofJointName = "abdomen";
		rotx = roty = rotz = 0;
		break;
	case '3':
		dofJointName = "tail_01";
		rotx = roty = rotz = 0;
		break;
	case '4':
		dofJointName = "tail_02";
		rotx = roty = rotz = 0;
		break;
	case '5':
		dofJointName = "tail_03";
		rotx = roty = rotz = 0;
		break;
	case '6':
		dofJointName = "tail_04";
		rotx = roty = rotz = 0;
		break;
	case '7':
		dofJointName = "hip_03_r";
		rotx = roty = rotz = 0;
		break;
	case '8':
		dofJointName = "knee_03_r";
		rotx = roty = rotz = 0;
		break;
	case '9':
		dofJointName = "hip_02_r";
		rotx = roty = rotz = 0;
		break;
	case '0':
		dofJointName = "knee_02_r";
		rotx = roty = rotz = 0;
		break;
	case 'q':
		dofJointName = "hip_01_r";
		rotx = roty = rotz = 0;
		break;
	case 'w':
		dofJointName = "knee_01_r";
		rotx = roty = rotz = 0;
		break;
	case 'e':
		dofJointName = "wing_02_r";
		rotx = roty = rotz = 0;
		break;
	case 'r':
		dofJointName = "wing_01_r";
		rotx = roty = rotz = 0;
		break;
	case 't':
		dofJointName = "hip_03_l";
		rotx = roty = rotz = 0;
		break;
	case 'a':
		dofJointName = "knee_03_l";
		rotx = roty = rotz = 0;
		break;
	case 's':
		dofJointName = "hip_02_l";
		rotx = roty = rotz = 0;
		break;
	case 'd':
		dofJointName = "knee_02_l";
		rotx = roty = rotz = 0;
		break;
	case 'f':
		dofJointName = "hip_01_l";
		rotx = roty = rotz = 0;
		break;
	case 'g':
		dofJointName = "knee_01_l";
		rotx = roty = rotz = 0;
		break;
	case 'h':
		dofJointName = "wing_01_l";
		rotx = roty = rotz = 0;
		break;
	case 'j':
		dofJointName = "wing_02_l";
		rotx = roty = rotz = 0;
		break;

	//For Tube 
	case '[':
		dofJointName = "shoulder";
		rotx = roty = rotz = 0;
		break;
	case ']':
		dofJointName = "elbow";
		rotx = roty = rotz = 0;
		break;
	//For changing DOF values
	case 'l':
		rotx += 0.1;
		break;
	case 'i':
		roty += 0.1;
		break;
	case 'k':
		rotz += 0.1;
		break;
	case 'L':
		rotx -= 0.1;
		break;
	case 'I':
		roty -= 0.1;
		break;
	case 'K':
		rotz -= 0.1;
		break;

	}

	glutSwapBuffers();
	glutPostRedisplay();
}
// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20, (double)width / height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 12.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	//glTranslatef(0, 0.2f, 0);
	glFlush();
}

void initLighting(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//Light 1
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//Light 2
	GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position1[] = { -5.0, 5.0, 5.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("Assignment3 - Skeleton With Skin");
	glClearColor(0.0, 0.0, 0.0, 1);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	cout << "Enter File Name: tube or wasp: ";
	string fname;
	cin >> fname;
//	loadFile("tube.skel");
//	loadSkinFile("tube.skin");
//	loadFile("wasp.skel");
//	loadSkinFile("wasp.skin");
	loadFile(fname + ".skel");
	loadSkinFile(fname + ".skin");
	glutDisplayFunc(display);
	initLighting();
	glutKeyboardFunc(keyboard);
	glutMouseFunc(OnMouseDown);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}



