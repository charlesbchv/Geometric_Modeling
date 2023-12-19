#include "myFace.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>
#include <iostream>
using namespace std;

static int frontDepart = 0;

myFace::myFace() : halfEdgeAdj(nullptr), normal(new myVector3D(1.0, 1.0, 1.0)), ajoute(frontDepart++) {}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	/**** TODO ****/
	myPoint3D* p1 = this->halfEdgeAdj->source->point;
	myPoint3D* p2 = this->halfEdgeAdj->next->source->point;
	myPoint3D* p3 = this->halfEdgeAdj->prev->source->point;

	normal->setNormal(p1, p2, p3);
}
