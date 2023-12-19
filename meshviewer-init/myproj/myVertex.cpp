#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"
#include <iostream>
using namespace std;

static int verticeDepart = 1;

myVertex::myVertex() : ajoute(verticeDepart++), point(nullptr), departO(nullptr), normal(new myVector3D(1.0, 1.0, 1.0)) {}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
	if (point) delete point;
}

void myVertex::computeNormal() {
    delete normal;
    normal = new myVector3D(0.0, 0.0, 0.0);

    myHalfedge* pasPas = departO;

    do {
        // Accumulate face normals
        *normal += *(pasPas->frontAdj->normal);
        pasPas = pasPas->twin->next;
    } while (departO != pasPas);

    // Normalize the final normal
    normal->normalize();
}

