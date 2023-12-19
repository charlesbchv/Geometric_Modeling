#pragma once
#include "mypoint3d.h"

class myHalfedge;
class myVector3D;

class myVertex
{
public:
	myPoint3D* point;
	myHalfedge* departO;

	int ajoute;
	int index;  //use as you wish.
	friend double calculDistance(myVertex* a, myVertex* b) {
		double dx = a->point->X - b->point->X;
		double dy = a->point->Y - b->point->Y;
		double dz = a->point->Z - b->point->Z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	myVector3D* normal;

	void computeNormal();
	myVertex(void);
	~myVertex(void);
};
