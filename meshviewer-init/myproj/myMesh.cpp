#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include <algorithm>
#include "myvector3d.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh() {

	while (!vertices.empty()) {
		delete vertices.back(); 
		vertices.pop_back();
	}
	while (!halfedges.empty()) {
		delete halfedges.back();
		halfedges.pop_back();
	}
	while (!faces.empty()) {
		delete faces.back();
		faces.pop_back();
	}
}


void myMesh::clear() {
	while (!vertices.empty()) {
		delete vertices.back();
		vertices.pop_back();
	}

	while (!halfedges.empty()) {
		delete halfedges.back();
		halfedges.pop_back();
	}

	while (!faces.empty()) {
		delete faces.back();
		faces.pop_back();
	}

	vector<myVertex*> empty_vertices;
	vector<myHalfedge*> empty_halfedges;
	vector<myFace*> empty_faces;

	vertices.swap(empty_vertices);
	halfedges.swap(empty_halfedges);
	faces.swap(empty_faces);
}


void myMesh::checkMesh() {
	bool tousOntJumeau = true;
	bool tousOntSuivant = true;
	bool tousOntPrecedent = true;

	for (myHalfedge* he : halfedges) {
		if (!he) continue; 

		if (tousOntJumeau) {
			tousOntJumeau = verifTwin(he);
		}

		if (tousOntSuivant) {
			tousOntSuivant = verifNext(he);
		}

		if (tousOntPrecedent) {
			tousOntPrecedent = verifPrev(he);
		}

		if (!tousOntJumeau || !tousOntSuivant || !tousOntPrecedent) {
			break;
		}
	}

	afficherRes(tousOntJumeau, tousOntSuivant, tousOntPrecedent);
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	sub = filename;

	map<pair<int, int>, myHalfedge*> tableauJumelle;
	map<pair<int, int>, myHalfedge*>::iterator it;

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

			myVertex* vertex = new myVertex();
			vertex->point = new myPoint3D(x, y, z);
			vertices.push_back(vertex);
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			vector<int> list;
			int verticesSize = vertices.size();

			while (myline >> u)
			{
				int vertexIdx = atoi((u.substr(0, u.find("/"))).c_str());
				vertexIdx = vertexIdx >= 0 ? vertexIdx - 1 : vertexIdx % verticesSize;
				list.push_back(vertexIdx);
			}

			int tailleTab = list.size();
			myFace* face = new myFace();
			myHalfedge* originHalfedge;
			myHalfedge* prevHalfedge = nullptr;
			size_t i = 0;

			do {
				myHalfedge* e = new myHalfedge();
				e->source = this->vertices.at(list[i]);
				e->source->departO = e;
				e->frontAdj = face;
				e->prev = prevHalfedge;

				(i == 0) ? (originHalfedge = e, prevHalfedge = e) : (prevHalfedge->next = e);

				int vertex_a = list[i];
				int vertex_b = list[(i + 1) % tailleTab];
				auto it = tableauJumelle.find(std::make_pair(vertex_b, vertex_a));

				(it == tableauJumelle.end()) ? tableauJumelle[std::make_pair(vertex_a, vertex_b)] = e : (e->twin = it->second, e->twin->twin = e);

				(i == tailleTab - 1) ? (e->next = originHalfedge, originHalfedge->prev = e) : void();


				halfedges.push_back(e);
				prevHalfedge = e;
				i++;
			} while (i < tailleTab);
			face->halfEdgeAdj = originHalfedge;
			faces.push_back(face);
		}
	}

	checkMesh();
	normalize();
	return true;
}


void myMesh::computeNormals() {
	/**** TODO ****/
	size_t faceIndex = 0;
	while (faceIndex < faces.size()) {
		faces[faceIndex]->computeNormal();
		faceIndex++;
	}

	size_t vertexIndex = 0;
	while (vertexIndex < vertices.size()) {
		vertices[vertexIndex]->computeNormal();
		vertexIndex++;
	}
}


void myMesh::normalize() {
	myPoint3D center;
	double maxDimension;
	calculateBoundingBox(center, maxDimension);
	normalizeVertices(center, maxDimension);
}



void myMesh::splitFaceTRIS(myFace* f, myPoint3D* p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge* e1, myPoint3D* p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace* f, myPoint3D* p)
{
	/**** TODO ****/
}

void myMesh::subdivisionCatmullClark()
{
	std::unordered_map<myFace*, myVertex*> facePoints;
	additionnerFacePoints(facePoints);
	vector<myFace*> startF;
	vector<myHalfedge*> startHE;
	vector<myVertex*> startV;

	for (size_t index = 0; index < halfedges.size(); index++) {
		myHalfedge* halfedge = halfedges[index];
		if (pointsDesAretes.find(halfedge) == pointsDesAretes.end() && pointsDesAretes.find(halfedge->twin) == pointsDesAretes.end()) {
			myVertex* edgePoint = new myVertex();
			edgePoint->point = new myPoint3D();

			myPoint3D sourcePoint = *(halfedge->source->point);
			myPoint3D twinPoint = *(halfedge->twin->source->point);

			if (halfedge->frontAdj && halfedge->twin->frontAdj) {
				myPoint3D frontAdjPointA = *(facePoints[halfedge->frontAdj]->point);
				myPoint3D frontAdjPointF = *(facePoints[halfedge->twin->frontAdj]->point);

				*(edgePoint->point) = (frontAdjPointA + frontAdjPointF + sourcePoint + twinPoint) / 4;
			}
			pointsDesAretes[halfedge] = edgePoint;
			pointsDesAretes[halfedge->twin] = pointsDesAretes[halfedge];
		}
	}

	for (myVertex* vertexB : vertices) {
		myPoint3D* originalPos = nullptr;
		myPoint3D* moyenneFace = new myPoint3D();
		myPoint3D* moyenneArete = new myPoint3D();
		if (vertexB->point) {
			originalPos = new myPoint3D(*(vertexB->point));
		}

		int index = 0;
		myHalfedge* startEdge = vertexB->departO; myHalfedge* aretCourant = startEdge;

		for (myHalfedge* aretCourant = startEdge; aretCourant != startEdge || index == 0; aretCourant = aretCourant->twin->next, ++index) {
			*moyenneFace += *facePoints[aretCourant->frontAdj]->point;
			myPoint3D midpoint = (*(aretCourant->source->point) + *(aretCourant->twin->source->point)) / 2;
			*moyenneArete += midpoint;
		}

		// Calcul de moyenneFace et moyenneArete
		*moyenneFace /= index;
		*moyenneArete /= index;

		// Calcul de la nouvelle position
		myPoint3D newPosition = *moyenneFace + *moyenneArete * 2 + (*originalPos) * (index - 3);
		newPosition /= index;

		// Affectation de la nouvelle position à vertexB
		*(vertexB->point) = newPosition;
		delete[] originalPos;
	}

	auto addVertexIfNotExists = [&](myVertex* vertex) {
		if (std::find(startV.begin(), startV.end(), vertex) == startV.end()) {
			startV.push_back(vertex);
		}
	};

	for (myFace* oldFace : faces) {
		myHalfedge* startEdge = oldFace->halfEdgeAdj;
		myHalfedge* aretCourant = startEdge;

		do {
			myFace* newFace = new myFace();
			myHalfedge* edge[4];

			for (int i = 0; i < 4; i++) {
				edge[i] = new myHalfedge();
			}

			myVertex* updatedVertex = aretCourant->source;
			myVertex* edgePoint1 = pointsDesAretes[aretCourant];
			myVertex* facePoint = facePoints[oldFace];
			myVertex* edgePoint2 = pointsDesAretes[aretCourant->prev];

			for (int i = 0; i < 4; i++) {
				edge[i]->source = (i == 0) ? updatedVertex : (i == 1) ? edgePoint1 : (i == 2) ? facePoint : edgePoint2;
				edge[i]->next = edge[(i + 1) % 4];
				edge[i]->prev = edge[(i + 3) % 4];
				edge[i]->frontAdj = newFace;
				edge[i]->source->departO = edge[i];
			}

			newFace->halfEdgeAdj = edge[0];

			startF.push_back(newFace);
			startHE.insert(startHE.end(), edge, edge + 4);

			addVertexIfNotExists(updatedVertex);
			addVertexIfNotExists(edgePoint1);
			addVertexIfNotExists(facePoint);
			addVertexIfNotExists(edgePoint2);

			aretCourant = aretCourant->next;
		} while (aretCourant != startEdge);
	}

	std::vector<int> startAx(startHE.size(), -1);

	for (size_t i = 0; i < startHE.size(); ++i) {
		myHalfedge* startA = startHE[i];
		for (size_t j = 0; j < startHE.size(); ++j) {
			if (i != j) {
				myHalfedge* nouveauJum = startHE[j];
				if (startA->source == nouveauJum->next->source && startA->next->source == nouveauJum->source) {
					startA->twin = nouveauJum;
					startAx[i] = j;
					break;
				}
			}
		}
	}

	for (int i = faces.size() - 1, j = halfedges.size() - 1, k = startAx.size() - 1; i >= 0 || j >= 0 || k >= 0; --i, --j, --k) {
		if (i >= 0) {
			faces.erase(faces.begin() + i);
		}
		if (j >= 0) {
			halfedges.erase(halfedges.begin() + j);
		}
		if (k >= 0 && startAx[k] == -1) {
			startHE.erase(startHE.begin() + k);
		}
	}

	faces = startF; halfedges = startHE; vertices = startV;

	checkMesh();

}


myHalfedge* myMesh::cheminPlusCourt() {
	double longueurMin = std::numeric_limits<double>::max();
	myHalfedge* areteLaPlusCourte = nullptr;

	for (myHalfedge* arete : halfedges) {
		if (arete < arete->twin) {
			double longueur = calculDistance(arete->source, arete->next->source);

			if (longueur < longueurMin) {
				longueurMin = longueur;
				areteLaPlusCourte = arete;
			}
		}
	}

	return areteLaPlusCourte;
}


void myMesh::casserArete(myHalfedge* aretADetruire) {
	if (aretADetruire == nullptr) {
		return;
	}

	myVertex* vertexToRemove = aretADetruire->next->source;
	*aretADetruire->source->point = (*(aretADetruire->source->point) + *(vertexToRemove->point)) / 2;

	myHalfedge* aretCourant = vertexToRemove->departO;
	do {
		aretCourant->source = aretADetruire->source;

		aretCourant = aretCourant->twin->next;
	} while (aretCourant != vertexToRemove->departO);

	myHalfedge* prevEdge = aretADetruire->prev;
	myHalfedge* nextEdge = aretADetruire->next;
	prevEdge->next = nextEdge;
	nextEdge->prev = prevEdge;

	myHalfedge* prevJumAret = aretADetruire->twin->prev;
	myHalfedge* nextJumAret = aretADetruire->twin->next;
	prevJumAret->next = nextJumAret;
	nextJumAret->prev = prevJumAret;

	aretADetruire->frontAdj->halfEdgeAdj = nextEdge;
	aretADetruire->twin->frontAdj->halfEdgeAdj = nextJumAret;

	vertices.erase(std::remove(vertices.begin(), vertices.end(), vertexToRemove), vertices.end());

	aretADetruire->supprime = true;
	aretADetruire->twin->supprime = true;
}

void myMesh::simplify() {
	if (vertices.size() <= 3) return;

	myHalfedge* areteAEffondrer = cheminPlusCourt();
	casserArete(areteAEffondrer);
	std::map<myVertex*, myHalfedge*> nouveauDepartO;
	size_t i = 0;
	while (i < halfedges.size()) {
		myHalfedge* he = halfedges[i];
		if (he->next->next == he) {
			myVertex* verticeA = he->source;
			myVertex* v2 = he->next->source;

			nouveauDepartO[verticeA] = he->twin->next;
			nouveauDepartO[v2] = he->next->twin->next;
			myHalfedge* jumelleHe = he->twin;
			myHalfedge* jumelleHeSuivant = he->next->twin;

			if (jumelleHe->twin != nullptr) jumelleHe->twin = jumelleHeSuivant;
			if (jumelleHeSuivant->twin != nullptr) jumelleHeSuivant->twin = jumelleHe;
			faces.erase(std::remove(faces.begin(), faces.end(), he->frontAdj), faces.end());

			he->supprime = true;
			he->next->supprime = true;
		}
		++i;
	}

	for (auto& paire : nouveauDepartO) {
		myVertex* sommet = paire.first;
		myHalfedge* nouveauDepartO = paire.second;

		if (sommet != nullptr && nouveauDepartO != nullptr) {
			sommet->departO = nouveauDepartO;
		}
	}

	// Supprime les arêtes
	halfedges.erase(std::remove_if(halfedges.begin(), halfedges.end(),
		[](myHalfedge* e) { return e->supprime; }),
		halfedges.end());

	checkMesh();
}


void myMesh::triangulate()
{
	int cristal = 0;
	int constante = 4;
	/**** TODO ****/
	size_t variable = 0;
	do
	{
		myFace* lico = faces[variable];
		if (triangulate(lico))
		{
			variable++;
			continue;
		}

		myPoint3D* origineR = new myPoint3D();

		double verticeCristal = 0;
		myHalfedge* renvoieHalfEdge = lico->halfEdgeAdj;
		for (myHalfedge* renvoieHalfEdge = lico->halfEdgeAdj; ; renvoieHalfEdge = renvoieHalfEdge->next)
		{
			*origineR += *(renvoieHalfEdge->source->point);
			verticeCristal++;

			if (renvoieHalfEdge->next == lico->halfEdgeAdj)
			{
				break;
			}
		}

		if (verticeCristal >= constante)
		{
			map<pair<int, int>, myHalfedge*> tableauJumelle; *origineR /= verticeCristal;
			myVertex* originVertice = new myVertex(); originVertice->point = origineR;
			vertices.push_back(originVertice);

			myHalfedge* originHalfedge = lico->halfEdgeAdj;

			do
			{
				myFace* nextFace;
				myHalfedge* nextHalf;
				myHalfedge* prevHalf;

				createFaceAndHalfedges(nextFace, nextHalf, prevHalf, originHalfedge, originVertice, faces, halfedges, tableauJumelle);
			} while (lico->halfEdgeAdj != originHalfedge);

			delete this->faces[variable];
			this->faces[variable] = nullptr;
		}

		variable++;
	} while (variable < faces.size());

	removeNullFaces(faces);
	checkMesh();
}

bool myMesh::triangulate(myFace* f)
{
	if (f->halfEdgeAdj->next->next->next == f->halfEdgeAdj)
	{
		return true;
	}
	else
	{
		return false;
	}
}