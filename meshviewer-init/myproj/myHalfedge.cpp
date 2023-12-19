#include "myHalfedge.h"
#include <iostream>

static int halfedgePointDepart = 0;

myHalfedge::myHalfedge() : source(NULL), frontAdj(NULL), next(NULL), prev(NULL), twin(NULL), ajoute(halfedgePointDepart++) {
}

void myHalfedge::copy(myHalfedge* ie)
{
	/**** TODO ****/
}

myHalfedge::~myHalfedge(void)
{
}
