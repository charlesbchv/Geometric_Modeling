#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include "myvector3d.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;

	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			cout << "v " << x << " " << y << " " << z << endl;

			myVertex* v = new myVertex();
			v->point = new myPoint3D(x, y, z);
			vertices.push_back(v);
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			faceids.clear();

			myFace* f = new myFace();

			while (myline >> u) faceids.push_back(atoi((u.substr(0, u.find("/"))).c_str()) - 1);

			if (faceids.size() < 3) continue;


			hedges = new myHalfedge * [faceids.size()];
			for (unsigned int i = 0; i < faceids.size(); i++) {
				hedges[i] = new myHalfedge();
			}

			f = new myFace();
			f->adjacent_halfedge = hedges[0];

			for (unsigned int i = 0; i < faceids.size(); i++) {
				int iplusone = (i + 1) % faceids.size();
				int iminusone = (i - 1 + faceids.size()) % faceids.size();
				hedges[i]->next = hedges[iplusone];
				hedges[i]->prev = hedges[iminusone];
				hedges[i]->adjacent_face = f;
				hedges[i]->source = vertices[faceids[i]];

				if ((it = twin_map.find(make_pair(faceids[iplusone], faceids[i]))) == twin_map.end())
					twin_map[make_pair(faceids[i], faceids[iplusone])] = hedges[i];
				else { hedges[i]->twin = it->second; it->second->twin = hedges[i]; }

				vertices[faceids[i]]->originof = hedges[i];
				halfedges.push_back(hedges[i]);
			}

			faces.push_back(f);
		}

	}

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		myHalfedge* e1 = vertices[i]->originof;
		myHalfedge* e2 = e1;

		if (e1 == NULL) {
			vertices.erase(vertices.begin() + i);
			i--;
			continue;
		}

		do {
			if (e2->twin == NULL) {
				vertices[i]->originof = e2;
				break;
			}
			e2 = e2->twin->next;
		} while (e2 != e1);
	}

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	for (myVertex* vertex : vertices) {
		vertex->normal = new myVector3D(0.0, 0.0, 0.0);
	}

	for (myFace* face : faces) {
		myHalfedge* he = face->adjacent_halfedge;
		myVector3D faceNormal;
		myVector3D v1 = *(he->source->point) - *(he->next->source->point);
		he = he->next;
		myVector3D v2 = *(he->source->point) - *(he->next->source->point);

		faceNormal.crossproduct(v1, v2);

		he = face->adjacent_halfedge;
		do {
			*(he->source->normal) += faceNormal;
			he = he->next;
		} while (he != face->adjacent_halfedge);
	}
	for (myVertex* vertex : vertices) {
		vertex->normal->normalize();
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}


void myMesh::triangulate()
{
	for (myFace* f : faces)
		myMesh::triangulate(f);

	checkMesh();
}

bool myMesh::triangulate(myFace* f)
{
	if (f->adjacent_halfedge->next->next->next == f->adjacent_halfedge) {
		return false;
	}

	// Calcul du vertex au centre de la face
	myVertex* centerVertex = new myVertex;
	centerVertex->point = new myPoint3D(0.0, 0.0, 0.0);
	myHalfedge* firstEdge = f->adjacent_halfedge;
	int numVertices = 0;

	do {
		*(centerVertex->point) += *(firstEdge->source->point);
		numVertices++;
		firstEdge = firstEdge->next;
	} while (firstEdge != f->adjacent_halfedge);

	*(centerVertex->point) /= numVertices;
	vertices.push_back(centerVertex);

	// Là on commence à créer les triangles : on crée 1 triangle pour chaque vertex
	myHalfedge* currentEdge = f->adjacent_halfedge;
	int i = 0;
	myHalfedge* oldNewEdge2;
	while (i < numVertices) {
		myHalfedge* newEdge1 = new myHalfedge();
		myHalfedge* newEdge2 = new myHalfedge();
		myFace* newFace = new myFace();

		newEdge1->source = centerVertex;
		newEdge1->next = currentEdge;
		newEdge1->prev = newEdge2;
		newEdge1->adjacent_face = newFace;

		// hormis pour le tt premier triangle, on set le twin de newEdge1 (avec l'ancien newEdge2) pour chaque iteration
		if (i != 0) {
			newEdge1->twin = oldNewEdge2;
			oldNewEdge2->twin = newEdge1;
		}

		newEdge2->source = currentEdge->next->source;
		newEdge2->next = newEdge1;
		newEdge2->prev = currentEdge;
		newEdge2->adjacent_face = newFace;
		oldNewEdge2 = newEdge2;		// servira lors de la prochaine iteration

		myHalfedge* oldNext = currentEdge->next;
		currentEdge->next = newEdge2;
		currentEdge->prev = newEdge1;
		currentEdge->adjacent_face = newFace;

		newFace->adjacent_halfedge = currentEdge;

		// push back tt le monde
		halfedges.push_back(newEdge1);
		halfedges.push_back(newEdge2);
		faces.push_back(newFace);

		// passage au prochain halfedge
		currentEdge = oldNext;

		i++;
	}

	// On set les derniers twin, celui du tout premier triangle, et du tout dernier
	f->adjacent_halfedge->prev->twin = oldNewEdge2;
	oldNewEdge2->twin = f->adjacent_halfedge->prev;

	return true;
}

//void myMesh::triangulate()
//{
//	std::map<std::pair<int, int>, myHalfedge*> halfEdgeMap;
//	vector<myFace*> toDelete;
//	vector<myFace*> oldfaces = faces;
//
//	// Utilisation d'un itérateur pour parcourir le vecteur de faces
//	auto faceIterator = oldfaces.begin();
//	while (faceIterator != oldfaces.end()) {
//		myFace* f = *faceIterator;
//		checkMesh();
//		// Si la face n'est pas un triangle, triangule-la
//		if (triangulate(f)) {
//			myVertex* centerVertex = new myVertex();
//			centerVertex->point = new myPoint3D();
//
//			int count = 0;
//			double sumX = 0;
//			double sumY = 0;
//			double sumZ = 0;
//
//			myHalfedge* he = f->adjacent_halfedge;
//			myHalfedge* initialHe = he;
//
//			// Utilisation d'une boucle "do-while" pour parcourir les demi-arêtes
//			do {
//				count++;
//				sumX += he->source->point->X;
//				sumY += he->source->point->Y;
//				sumZ += he->source->point->Z;
//
//				he = he->next;
//			} while (he != initialHe);
//
//			centerVertex->point->X = sumX / count;
//			centerVertex->point->Y = sumY / count;
//			centerVertex->point->Z = sumZ / count;
//
//			vertices.push_back(centerVertex);
//
//			he = initialHe;
//
//			int i = 0;
//			while (i < count) {
//				myFace* newface = new myFace();
//				myHalfedge* he1 = new myHalfedge();
//				myHalfedge* he2 = new myHalfedge();
//				myHalfedge* he3 = new myHalfedge();
//
//				he1->source = he->source;
//				he->source->originof = he1;
//				he1->next = he2;
//				he1->prev = he3;
//				he1->adjacent_face = newface;
//
//				he2->source = centerVertex;
//				centerVertex->originof = he2;
//				he2->next = he3;
//				he2->prev = he1;
//				he2->adjacent_face = newface;
//
//				he3->source = he->next->source;
//				he->next->source->originof = he3;
//				he3->next = he1;
//				he3->prev = he2;
//				he3->adjacent_face = newface;
//
//				newface->adjacent_halfedge = he1;
//
//				if (halfEdgeMap.find({ he3->source->index, he1->source->index }) != halfEdgeMap.end()) {
//					he3->twin = halfEdgeMap[{he3->source->index, he1->source->index}];
//					halfEdgeMap[{he3->source->index, he1->source->index}]->twin = he3;
//				}
//				else {
//					halfEdgeMap[{he1->source->index, he3->source->index}] = he1;
//				}
//				if (halfEdgeMap.find({ he1->source->index, he2->source->index }) != halfEdgeMap.end()) {
//					he1->twin = halfEdgeMap[{he1->source->index, he2->source->index}];
//					halfEdgeMap[{he1->source->index, he2->source->index}]->twin = he1;
//				}
//				else {
//					halfEdgeMap[{he2->source->index, he1->source->index}] = he2;
//				}
//
//				if (halfEdgeMap.find({ he2->source->index, he3->source->index }) != halfEdgeMap.end()) {
//					he2->twin = halfEdgeMap[{he2->source->index, he3->source->index}];
//					halfEdgeMap[{he2->source->index, he3->source->index}]->twin = he2;
//				}
//				else {
//					halfEdgeMap[{he3->source->index, he2->source->index}] = he3;
//				}
//
//				faces.push_back(newface);
//				halfedges.push_back(he1);
//				halfedges.push_back(he2);
//
//				he = he->next;
//				i++;
//			}
//			toDelete.push_back(f);
//
//			faceIterator++;
//		}
//		else {
//			// Si la face est déjà un triangle, avance simplement l'itérateur
//			faceIterator++;
//		}
//	}

//	auto deleteFaceIterator = toDelete.begin();
//	while (deleteFaceIterator != toDelete.end()) {
//		myFace* oldface = *deleteFaceIterator;
//
//		//  erase avec l'itérateur pour supprimer l'élément
//		faces.erase(std::remove(faces.begin(), faces.end(), oldface), faces.end());
//		delete oldface;
//
//		// Avance l'itérateur de face à supprimer
//		deleteFaceIterator++;
//	}
//}
//
//
//bool myMesh::triangulate(myFace* f)
//{
//	if (f->adjacent_halfedge->next->next->next == f->adjacent_halfedge)
//	{
//		return false;
//	}
//	else
//	{
//		return true;
//	}
//}

