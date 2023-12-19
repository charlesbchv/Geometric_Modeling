#pragma once

class myHalfedge;
class myVector3D;

class myFace
{
public:
	myHalfedge* halfEdgeAdj;
	myVector3D* normal;

	int ajoute;
	int index; //use this variable as you wish.

	void computeNormal();
	myFace(void);
	~myFace(void);
};