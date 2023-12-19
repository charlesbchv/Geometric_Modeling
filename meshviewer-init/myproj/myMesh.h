#pragma once
#include "myFace.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <cstdlib> 
#include <vector>
#include <map>
#include <string>

using namespace std;

class myMesh
{
public:
	vector<myVertex*> vertices;
	vector<myHalfedge*> halfedges;
	vector<myFace*> faces;
	string sub;
	map<myFace*, myVertex*> facePoints;
	map<myHalfedge*, myVertex*> pointsDesAretes;

	void checkMesh();
	bool readFile(std::string filename);
	void computeNormals();
	void normalize();
	void myMesh::simplify();
	void myMesh::casserArete(myHalfedge* aretADetruire);
	myHalfedge* myMesh::cheminPlusCourt();

	bool myMesh::verifTwin(myHalfedge* he) {
		return he->twin && he->twin->twin == he && he->twin->source == he->next->source;
	}
	bool myMesh::verifNext(myHalfedge* he) {
		return he->next && he->next->prev == he;
	}
	bool myMesh::verifPrev(myHalfedge* he) {
		return he->prev && he->prev->next == he;
	}
	void myMesh::afficherRes(bool twin, bool suivant, bool precedent) {
		cout << (twin ? "Each halfedge has a twin!\n" : "Error! Not all edges have their twins!\n");
		cout << (precedent ? "Each halfedge has a prev!\n" : "Error! Not all edges have their prevs!\n");
		cout << (suivant ? "Each halfedge has a next\n" : "Error! Not all edges have their nexts!\n");
	}


	void myMesh::calculateBoundingBox(myPoint3D& center, double& maxDimension) {
		if (vertices.empty()) return;

		myPoint3D pointMin = *vertices.front()->point;
		myPoint3D pointMax = pointMin;

		auto it = vertices.begin();
		while (it != vertices.end()) {
			pointMin.X = min(pointMin.X, (*it)->point->X); pointMax.X = max(pointMax.X, (*it)->point->X);
			pointMin.Y = min(pointMin.Y, (*it)->point->Y); pointMax.Y = max(pointMax.Y, (*it)->point->Y);
			pointMin.Z = min(pointMin.Z, (*it)->point->Z); pointMax.Z = max(pointMax.Z, (*it)->point->Z);
			++it;
		}

		center.X = (pointMax.X + pointMin.X) / 2.0;
		center.Y = (pointMax.Y + pointMin.Y) / 2.0;
		center.Z = (pointMax.Z + pointMin.Z) / 2.0;

		maxDimension = max({ pointMax.X - pointMin.X, pointMax.Y - pointMin.Y, pointMax.Z - pointMin.Z });
	}

	void myMesh::normalizeVertices(const myPoint3D& center, double maxDimension) {
		auto it = vertices.begin();
		while (it != vertices.end()) {
			(*it)->point->X = ((*it)->point->X - center.X) /maxDimension;
			(*it)->point->Y = ((*it)->point->Y - center.Y)/maxDimension;
			(*it)->point->Z = ((*it)->point->Z - center.Z)/ maxDimension;

			++it;
		}
	}

	// Implement T
	void relierT(map<pair<int, int>, myHalfedge*>& arrayJumelle, int verticeX, int verticeB, myHalfedge* w)
	{
		map<pair<int, int>, myHalfedge*>::iterator laCase = arrayJumelle.find(make_pair(verticeB, verticeX));
		if (laCase == arrayJumelle.end()) {
			arrayJumelle[make_pair(verticeX, verticeB)] = w;
		}
		else {
			w->twin = laCase->second;
			w->twin->twin = w;
		}
	}

	void createFaceAndHalfedges(myFace*& nextFace, myHalfedge*& nextHalf, myHalfedge*& prevHalf, myHalfedge*& originHalfedge, myVertex* originVertice, std::vector<myFace*>& faces, std::vector<myHalfedge*>& halfedges, std::map<std::pair<int, int>, myHalfedge*>& tableauJumelle)
	{
		nextFace = new myFace();
		nextFace->halfEdgeAdj = originHalfedge;
		originHalfedge->frontAdj = nextFace;

		myHalfedge* originHalfedgeNext = originHalfedge->next;
		nextHalf = new myHalfedge();
		nextHalf->source = originHalfedgeNext->source;
		nextHalf->source->departO = nextHalf;
		nextHalf->frontAdj = nextFace;

		prevHalf = new myHalfedge();
		prevHalf->source = originVertice;
		prevHalf->source->departO = prevHalf;
		prevHalf->frontAdj = nextFace;

		nextHalf->prev = originHalfedge;
		nextHalf->next = prevHalf;
		prevHalf->prev = nextHalf;
		prevHalf->next = originHalfedge;
		originHalfedge->prev = prevHalf;
		originHalfedge->next = nextHalf;

		relierT(tableauJumelle, nextHalf->source->ajoute, prevHalf->source->ajoute, nextHalf);
		relierT(tableauJumelle, prevHalf->source->ajoute, originHalfedge->source->ajoute, prevHalf);

		faces.push_back(nextFace);
		halfedges.push_back(nextHalf);
		halfedges.push_back(prevHalf);

		originHalfedge = originHalfedgeNext;
	}

	void removeNullFaces(std::vector<myFace*>& faces)
	{
		for (auto it = faces.begin(); it != faces.end(); )
		{
			if (*it == nullptr)
			{
				it = faces.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void processFaceAndCreateCenterVertex(myFace* face, myPoint3D* origineR, int verticeCristal, std::vector<myVertex*>& vertices, std::vector<myFace*>& faces, std::vector<myHalfedge*>& halfedges, std::map<std::pair<int, int>, myHalfedge*>& tableauJumelle) {
		if (verticeCristal >= 4)
		{
			map<pair<int, int>, myHalfedge*> tableauJumelle;

			// Crée le centre vertex
			*origineR /= verticeCristal;
			myVertex* originVertice = new myVertex();
			originVertice->point = origineR;
			vertices.push_back(originVertice);

			myHalfedge* originHalfedge = face->halfEdgeAdj;

			do
			{
				myFace* nextFace;
				myHalfedge* nextHalf;
				myHalfedge* prevHalf;

				createFaceAndHalfedges(nextFace, nextHalf, prevHalf, originHalfedge, originVertice, faces, halfedges, tableauJumelle);

			} while (face->halfEdgeAdj != originHalfedge);

			delete face;
		}
	}

	myVertex* additionnerFace(myFace* f) {
		myVertex* facePoint = new myVertex();
		facePoint->point = new myPoint3D();

		myHalfedge* arcDebut = f->halfEdgeAdj;
		myHalfedge* arcCourant = arcDebut;
		int numVertices = 0;

		for (; arcCourant != nullptr; arcCourant = arcCourant->next) {
			*(facePoint->point) += *(arcCourant->source->point);
			numVertices++;

			if (arcCourant->next == arcDebut) {
				break;
			}
		}

		*(facePoint->point) /= numVertices;

		return facePoint;
	}

	void additionnerFacePoints(std::unordered_map<myFace*, myVertex*>& facePoints)
	{
		for (size_t i = 0; i < faces.size(); ++i) {
			myFace* f = faces[i];
			myVertex* facePoint = additionnerFace(f);
			facePoints[f] = facePoint;
		}
	}

	void subdivisionCatmullClark();
	void splitFaceTRIS(myFace*, myPoint3D*);

	void splitEdge(myHalfedge*, myPoint3D*);
	void splitFaceQUADS(myFace*, myPoint3D*);

	void triangulate();
	bool triangulate(myFace*);

	void clear();

	myMesh(void);
	~myMesh(void);
};

