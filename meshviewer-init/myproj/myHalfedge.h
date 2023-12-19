#pragma once

#include <stdio.h>
#include <tchar.h>

class myVertex;
class myFace;
class myPoint3D;

class myHalfedge
{
public:
	myVertex* source;
	myFace* frontAdj;
	myHalfedge* next;
	myHalfedge* prev;
	myHalfedge* twin;
	myHalfedge* nextHalf;
	myHalfedge* prevHalf;

	bool supprime = false;

	int ajoute;
	int index; //use as you wish.

	myHalfedge(void);
	void copy(myHalfedge*);
	~myHalfedge(void);
};