#pragma once
#ifndef READOBJ_H_
#define READOBJ_H_

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include"HalfEdge.h"

typedef std::vector<Vertex> AVertex;
typedef std::vector<Face> AFace;
typedef std::vector<HalfEdge*> Aedge;
typedef std::vector<int> vec;
typedef std::pair<int, int> Pair;
typedef std::map<Pair, int> MyMap;

void readOBJ(std::string filename,AVertex &vertex,AFace &face,int &Nnode,int &Nface)
{
	Nnode = Nface = 0;
	std::ifstream in;
	in.open(filename);
	if (!in)
	{
		std::cout << "Open Failed!" << "check filename:"<< filename << "is right?" << '\n';
	}

	std::string buffer, type;
	while(in>>type)
	{
		if (type == "v")
		{
			Nnode++;
			Vertex ver;
			in >> ver.x >> ver.y >> ver.z;
			vertex.push_back(ver);
		}

		if (type == "f")
		{
			Nface++;
			Face f;
			in >> f.order[0] >> f.order[1] >> f.order[2];
			
			f.order[0] -= 1;
			f.order[1] -= 1;
			f.order[2] -= 1;

			face.push_back(f);
		}

	}

	in.close();
}

void initEdge(Aedge &edge,int &Nedge,AVertex &vertex,AFace &face)
{
	int Nnode = vertex.size()+10;

	MyMap mapone;
	
	int e_num = 1;
	for (int i = 0; i < face.size(); i++)
	{
		HalfEdge *edge1 = new HalfEdge();
		HalfEdge *edge2 = new HalfEdge();
		HalfEdge *edge3 = new HalfEdge();

		edge1->origin = face[i].order[0];
		edge2->origin = face[i].order[1];
		edge3->origin = face[i].order[2];

		edge1->next = edge2;
		edge2->next = edge3;
		edge3->next = edge1;

		HalfEdge *temp = new HalfEdge();

		if (mapone[Pair(face[i].order[1],face[i].order[0])] != 0)
		{
			temp = edge[mapone[Pair(face[i].order[1], face[i].order[0])] - 1];
			edge1->opposite = temp;
			temp->opposite = edge1;
		}
		else
		{
			edge1->opposite = NULL;
			mapone[Pair(face[i].order[0],face[i].order[1])] = e_num;
		}
		e_num++;

		if (mapone[Pair(face[i].order[2],face[i].order[1])] != 0)
		{
			temp = edge[mapone[Pair(face[i].order[2], face[i].order[1])] - 1];
			edge2->opposite = temp;
			temp->opposite = edge2;
		}
		else
		{
			edge2->opposite = NULL;
			mapone[Pair(face[i].order[1],face[i].order[2])] = e_num;
		}
		e_num++;

		if (mapone[Pair(face[i].order[0],face[i].order[2])] != 0)
		{
			temp = edge[mapone[Pair(face[i].order[0], face[i].order[2])] - 1];
			edge3->opposite = temp;
			temp->opposite = edge3;
		}
		else
		{
			edge3->opposite = NULL;
			mapone[Pair(face[i].order[2],face[i].order[0])] = e_num;
		}
		e_num++;

		edge1->IncFace = i;
		edge2->IncFace = i;
		edge3->IncFace = i;

		edge.push_back(edge1);
		edge.push_back(edge2);
		edge.push_back(edge3);
	}

	Nedge = edge.size();


	/*for (int i = 0; i < Nedge; i++)
	{
		std::cout << edge[i]->IncFace << ' ' << edge[i]->next << ' ' << edge[i]->opposite << ' ' << edge[i]->origin << '\n';
	}*/

}

HalfEdge* findOriginEdge(int v,int Nedge,Aedge &edge)
{
	for (int k = 0; k < Nedge; k++)
	{
		if (edge[k]->origin == v)
		{
			return edge[k];
		}
	}
	return NULL;
}


#endif // !READOBJ_H_

