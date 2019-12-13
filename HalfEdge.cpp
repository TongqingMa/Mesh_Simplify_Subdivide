#ifndef HALF_EDGE_CPP_
#define HALF_EDGE_CPP_

#include"HalfEdge.h"
#include<Eigen/Dense>    
#include<set>


void HE_Mesh::LoadFromObj(std::string file)
{
	std::ifstream fs;
	fs.open(file);

	while (!fs.eof())
	{
		char line[100];
		fs.getline(line, 100);
		if (line[0] == 'v' && line[1] == ' ')
		{
			char * str = strtok(line, " ");
			
			str = strtok(NULL, " ");		
			float x = atof(str);

			str = strtok(NULL, " ");
			float y = atof(str);

			str = strtok(NULL, " ");
			float z = atof(str);

			InsertVertex(x, y, z);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{
			char * str = strtok(line, " ");
			
			char * v1s = strtok(NULL, " ");
			char * v2s = strtok(NULL, " ");
			char * v3s = strtok(NULL, " ");

			int v1 = atoi(v1s);
			int v2 = atoi(v2s);
			int v3 = atoi(v3s);

			InsertFace(m_verts[v1 - 1], m_verts[v2 - 1], m_verts[v3 - 1]);
		}
	}

	std::cout << "Nnode:" << GetNodes() << '\n';
	std::cout << "Nface:" << GetFaces() << '\n';
	std::cout << "Nedge:" << GetEdges() << '\n';

	fs.close();
}


HE_Vertex* HE_Mesh::InsertVertex(float x, float y, float z)
{
	HE_Vertex * vert = new HE_Vertex();
	
	vert->x = x;
	vert->y = y;
	vert->z = z;

	m_verts.push_back(vert);

	return vert;
}


HE_Face* HE_Mesh::InsertFace(HE_Vertex *v1, HE_Vertex *v2, HE_Vertex *v3)
{
	HE_Edge * e1 = InsertEdge(v1, v2);
	HE_Edge * e2 = InsertEdge(v2, v3);
	HE_Edge * e3 = InsertEdge(v3, v1);

	if (!e1 || !e2 || !e3)
	{
		return NULL;
	}

	HE_Face * face = new HE_Face;

	e1->e_succ = e2;
	e2->e_succ = e3;
	e3->e_succ = e1;

	e1->e_face = e2->e_face = e3->e_face = face;

	v1->v_face = v2->v_face = v3->v_face = face;

	face->f_edge = e1;

	face->f_verts.push_back(v1);
	face->f_verts.push_back(v2);
	face->f_verts.push_back(v3);

	this->m_faces.push_back(face);

}

HE_Edge* HE_Mesh::InsertEdge(HE_Vertex * v1, HE_Vertex * v2)
{
	if (v1 == NULL || v2 == NULL)
	{
		return NULL;
	}

	if (m_emap[vert_pair(v1,v2)]!=nullptr)
	{
		return m_emap[vert_pair(v1, v2)];
	}

	//提前建立对偶边 并建立好点 边关系
	HE_Edge * edge = new HE_Edge;
	edge->e_vert = v1;
	v1->v_edge = edge;


	HE_Edge * p_edge = new HE_Edge;
	p_edge->e_vert = v2;
	v2->v_edge = p_edge;

	//建立对偶关系
	edge->e_pair = p_edge;
	p_edge->e_pair = edge;

	m_emap[vert_pair(v1, v2)] = edge;
	m_emap[vert_pair(v2, v1)] = p_edge;

	m_edges.push_back(edge);
	m_edges.push_back(p_edge);

	return edge;

}


edges HE_Mesh::GetEdgesFromVertex(HE_Vertex * vert)
{
	edges myedges;

	HE_Edge * base_edge = vert->v_edge;
	HE_Edge * tmp = base_edge;

	//std::cout<<"初始边"<< tmp->e_vert->num << "->" << tmp->e_succ->e_vert->num << ") 是否有效"<<tmp->alive<<'\n';

	do {

/*		if (tmp->e_vert == tmp->e_pair->e_vert || tmp->e_vert == tmp->e_succ->e_vert)
			break;
		for (int i = 0; i < myedges.size(); i++)
		{
			if (myedges[i] == tmp)
			{
				return myedges;
			}
		}
*/
		tmp = tmp->e_pair;
/*		if (IsBoundary(tmp))
		{	break;  }
*/
	//	std::cout << "temp 反边：(" << tmp->e_vert->num << "->" << tmp->e_succ->e_vert->num << ") 是否有效" << tmp->alive << '\n';

		tmp = tmp->e_succ;
	//	std::cout << "temp 要加入队列的边：(" << tmp->e_vert->num << "->" << tmp->e_succ->e_vert->num << ") 是否有效" << tmp->alive << '\n';


		myedges.push_back(tmp);

	} while (tmp != base_edge);

	return myedges;
}


verts HE_Mesh::GetVertsFromVertex(HE_Vertex * vert)
{

	edges edge = GetEdgesFromVertex(vert);
	verts vertices;

	for (unsigned int i = 0; i < edge.size(); i++)
	{
		vertices.push_back(edge[i]->e_succ->e_vert);
	}

	return vertices;
}


faces HE_Mesh::GetFacesFromVertex(HE_Vertex * vert)
{
	edges edge = GetEdgesFromVertex(vert);
	faces faces;

	for (unsigned int i = 0; i < edge.size(); i++)
	{
		faces.push_back(edge[i]->e_face);
	}

	return faces;
}


edges HE_Mesh::GetEdgesFromEdge(HE_Edge * edge)
{
	edges e_edge = GetEdgesFromVertex(edge->e_succ->e_vert);
	edges p_edge = GetEdgesFromVertex(edge->e_vert);

	for (unsigned int i = 0; i < p_edge.size(); i++)
	{
		e_edge.push_back(p_edge[i]->e_pair);
	}

	return e_edge;
}
Eigen::Vector4d HE_Mesh::Getplane(HE_Face *face)
{
	Eigen::Vector4d result;

	HE_Vertex *a, *b, *c;
	a = face->f_verts[0];
	b = face->f_verts[1];
	c = face->f_verts[2];


	result(0) = (b->y - a->y)*(c->z - a->z) - (c->y - a->y)*(b->z - a->z);
	result(1) = (b->z - a->z)*(c->x - a->x) - (b->x - a->x)*(c->z - a->z);
	result(2) = (b->x - a->x)*(c->y - a->y) - (c->x - a->x)*(b->y - a->y);
	result(3) = -a->x * result(0) - a->y * result(1) - a->z * result(2);
	double temp = sqrt(result(0)*result(0) + result(1)*result(1) + result(2)*result(2));
	for (int i = 0; i < 4; i++) {
		result(i) = result(i) / temp;
	}
	return result;
}
Eigen::Matrix4d HE_Mesh::GetQ(HE_Vertex* vert)
{
	Eigen::Matrix4d m;
	m = Eigen::Matrix4d::Zero();

	faces ff = GetFacesFromVertex(vert);

	for (int j = 0; j < ff.size(); j++)
	{
		Eigen::Vector4d result = Getplane(ff[j]);
		m = m + result * result.transpose();
	}

	return m;
}
float HE_Mesh:: GetCost(HE_Edge* edge)
{
	Eigen::Matrix4d addQ = GetQ(edge->e_vert) + GetQ(edge->e_succ->e_vert);

	Eigen::Matrix4d addQ1;
	for (int k = 0; k < 3; k++) {
		for (int j = 0; j < 4; j++) {
			addQ1(k, j) = addQ(k, j);
		}
	}
	addQ1(3, 0) = 0;
	addQ1(3, 1) = 0;
	addQ1(3, 2) = 0;
	addQ1(3, 3) = 1;
	Eigen::Vector4d a;


	a << 0, 0, 0, 1;
	Eigen::Vector4d result, p1, p2;
	p1 << edge->e_vert->x, edge->e_vert->y, edge->e_vert->z, 1;
	p2 << edge->e_succ->e_vert->x, edge->e_succ->e_vert->y, edge->e_succ->e_vert->z, 1;

	if (isnan(addQ1.inverse()(0))) {
		double maxk = 0, maxcost = 1000000000;
		double cost;
		for (double k = 0; k <= 1; k = k + 0.1) {
			result = (1 - k)*p1 + k * p2;
			cost = (result.transpose()*addQ*result)(0);
			if (cost < maxcost) {
				maxk = k;
				maxcost = cost;
			}
		}
		result = (1 - maxk)*p1 + maxk * p2;
	}
	else {
		result = addQ1.inverse()*a;
	}

	edge->v_x = result(0);
	edge->v_y = result(1);
	edge->v_z = result(2);

	edge->precision = (result.transpose()*addQ*result)(0);
	return edge->precision;
}




glm::f32vec4 HE_Mesh::caculateV(HE_Face* face)
{
	HE_Vertex *a, *b, *c;
	glm::f32vec3 v1, v2, v3;
	glm::f32vec3 vv1, vv2;
	a = face->f_verts[0];
	b = face->f_verts[1];
	c = face->f_verts[2];
	v1 = glm::f32vec3(a->x, a->y, a->z);
	v2 = glm::f32vec3(b->x, b->y, b->z);
	v3 = glm::f32vec3(c->x, c->y, c->z);

	vv1 = glm::f32vec3(v2 - v1);
	vv2 = glm::f32vec3(v3 - v1);
	float d = -(face->f_verts[0]->x * face->pvec.x + face->f_verts[0]->y*face->pvec.y + face->f_verts[0]->z*face->pvec.z);
	face->pvec =glm::vec4(glm::normalize( glm::cross(vv1, vv2) ),d);

	return face->pvec;
}

glm::mat4 HE_Mesh::caculateQ(HE_Vertex* vert)
{

	float a, b, c, d;
	faces ff = GetFacesFromVertex(vert);

	glm::mat4 Kp;
	for (int j = 0; j < ff.size(); j++)
	{
		HE_Face *face = ff[j];
		a = face->pvec.x; b = face->pvec.y; c = face->pvec.z; d = face->pvec.w;
		glm::f32vec4 vec1(a*a, a*b, a*c, a*d);
		glm::f32vec4 vec2(a*b, b*b, b*c, b*d);
		glm::f32vec4 vec3(c*a, c*b, c*c, c*d);
		glm::f32vec4 vec4(d*a, d*b, d*c, d*d);
		glm::mat4 tmat(vec1, vec2, vec3, vec4);

		Kp += tmat;
	}

	v_mat[vert] = Kp;
	
	return Kp;
	/*glm::f32vec4 v = glm::f32vec4(vert->x, vert->y, vert->z, 1);
	glm::f32vec4 vv = Kp * v;
	float precision = v.x*vv.x + v.y*vv.y + v.z*vv.z + v.w*vv.w;
	vmmap[vert] = precision;*/

	//return Kp;
}

float HE_Mesh::caculateCost(HE_Edge* edge)
{
	glm::mat4 addQ = v_mat[edge->e_vert] + v_mat[edge->e_succ->e_vert];

	glm::mat4 addQ1;
	for (int k = 0; k < 3; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			addQ1[k][j] = addQ[k][j];
		}
	}
	addQ1[3][0] = addQ1[3][1] = addQ1[3][2] = 0;
	addQ1[3][3] = 1;
	glm::vec4 vec(0, 0, 0, 1);
	glm::vec4 result, p1(edge->e_vert->x,edge->e_vert->y,edge->e_vert->z,1),
		p2(edge->e_succ->e_vert->x,edge->e_succ->e_vert->y,edge->e_succ->e_vert->z,1);
	
	float ifinverse = addQ1[0][0] * addQ1[1][1] * addQ1[2][2] + addQ1[0][1] * addQ1[1][2] * addQ1[2][0] + addQ1[1][0] + addQ1[2][1] + addQ1[0][2]
		- (addQ1[0][0] * addQ1[2][1] * addQ1[1][2] + addQ1[1][0] * addQ1[0][1] * addQ1[2][2] + addQ1[0][2] + addQ1[1][1] + addQ1[2][0]);

	if (ifinverse == 0)
	{
		double maxk = 0, maxcost = 1000000000;
		double cost;
		for (double k = 0; k <= 1; k += 0.1)
		{
			result = glm::vec4((1 - k)*p1.x + k * p2.x, (1 - k)*p1.y + k * p2.y, (1 - k)*p1.z + k * p2.z, (1 - k)*p1.w + k * p2.w);
			
			float tx = result.x*addQ[0][0] + result.y*addQ[1][0] + result.z*addQ[2][0] + result.w*addQ[3][0];
			float ty = result.x*addQ[0][1] + result.y*addQ[1][1] + result.z*addQ[2][1] + result.w*addQ[3][1];
			float tz = result.x*addQ[0][2] + result.y*addQ[1][2] + result.z*addQ[2][2] + result.w*addQ[3][2];
			float tw = result.x*addQ[0][3] + result.y*addQ[1][3] + result.z*addQ[2][3] + result.w*addQ[3][3];
			
			glm::vec4 temp(tx, ty, tz, tw);
			cost = temp.x*result.x + temp.y*result.y + temp.z*result.z + temp.w*result.w;
			
			if(cost<maxcost)
			{
				maxk = k;
				maxcost = cost;
			}
		}
		result = glm::vec4((1 - maxk)*p1.x + maxk * p2.x, (1 - maxk)*p1.y + maxk * p2.y, (1 - maxk)*p1.z + maxk * p2.z, (1 - maxk)*p1.w + maxk * p2.w);
	}
	else
	{
		addQ1 = glm::inverse(addQ1);
		result = glm::vec4(addQ1[0][3], addQ1[1][3], addQ1[2][3], addQ1[3][3]);
	}
	edge->v_x = result.x; edge->v_y = result.y; edge->v_z = result.z;

	float tx = result.x*addQ[0][0] + result.y*addQ[1][0] + result.z*addQ[2][0] + result.w*addQ[3][0];
	float ty = result.x*addQ[0][1] + result.y*addQ[1][1] + result.z*addQ[2][1] + result.w*addQ[3][1];
	float tz = result.x*addQ[0][2] + result.y*addQ[1][2] + result.z*addQ[2][2] + result.w*addQ[3][2];
	float tw = result.x*addQ[0][3] + result.y*addQ[1][3] + result.z*addQ[2][3] + result.w*addQ[3][3];

	glm::vec4 temp(tx, ty, tz, tw);
	edge->precision = temp.x*result.x + temp.y*result.y + temp.z*result.z + temp.w*result.w;

	return edge->precision;

	
}

edges HE_Mesh::collapse(HE_Edge *p)
{
	/*
		这个函数实现比较复杂  现在仍有bug 但是能够运行了

		三个步骤：
		     1.找出六条边
			           注意：  在以下特殊情况中
					                       /\
                                          / |\
                                         /  | \
                                        /   |  \
                                       /   / \  \  
                                      /   /   \  \
									 /   /     \  \
									/___/_______\__\
	       这里是连起来的 右边一样->| v1|        |v2|       当我合并v1->v2或者是v2->v1的时候会发生什么？  
		   因此我在函数中增加了两个循环来解决这个问题 while (e6->e_succ->e_succ == e5->e_succ->e_pair)
		                                              while (e3->e_succ->e_succ==e4->e_succ->e_pair)
	                                  	
		2.折叠边
		   将四个半边两两合并成一个边
	    3.更改所有应该更改的信息
		   包括每一个顶点的信息  出边  顶点位置信息  所在面的信息 等等
		   每一条边的信息  边的顶点 
		   面中顶点信息 
		   最后让应该无效的东西无效
		   另外需要计算更新面的法向量  和 更新边的误差值

		ps:该函数写了好几个版本 这个是当前最优的  往后翻 下一个函数是不好的  最开始的思路就在这个函数里边


	*/
	std::cout << "删除的边:(" << p->e_vert->num << "->" << p->e_succ->e_vert->num << ")\n";
	HE_Vertex *v1, *v2;
	v1 = p->e_vert; v2 = p->e_succ->e_vert;
	v1->x = p->v_x; v1->y = p->v_y; v1->z = p->v_z;  //获得最优顶点的值
	v_mat[v1] = v_mat[v1] + v_mat[v2];//更新新点的矩阵
	
	edges ev1;

	HE_Edge *e3 = nullptr, *e4 = nullptr, *e5 = nullptr, *e6 = nullptr, *e1 = nullptr, *e2 = nullptr;

	if ((p != NULL) && !(IsBoundary(p)) && (p->e_succ->e_pair != NULL) && (p->e_succ->e_succ->e_pair != NULL ) &&
		(p->e_pair->e_succ->e_pair != NULL) && (p->e_pair->e_succ->e_succ->e_pair != NULL))
	{
		e1 = p;
//		std::cout << "e1:(" << e1->e_vert->num << "->" << e1->e_succ->e_vert->num << ")<<是否有效" << e1->alive << '\n';
		e2 = p->e_pair;
//		std::cout << "e2:(" << e2->e_vert->num << "->" << e2->e_succ->e_vert->num << ")<<是否有效" << e2->alive << '\n';
		e6 = p->e_succ->e_pair;
//		std::cout << "e6:(" << e6->e_vert->num << "->" << e6->e_succ->e_vert->num << ")<<是否有效" << e6->alive << '\n';
		e5 = p->e_succ->e_succ->e_pair;
//		std::cout << "e5:(" << e5->e_vert->num << "->" << e5->e_succ->e_vert->num << ")<<是否有效" << e5->alive << '\n';
		e4 = p->e_pair->e_succ->e_succ->e_pair;
//		std::cout << "e4:(" << e4->e_vert->num << "->" << e4->e_succ->e_vert->num << ")<<是否有效" << e4->alive << '\n';
		e3 = p->e_pair->e_succ->e_pair;
//		std::cout << "e3:(" << e3->e_vert->num << "->" << e3->e_succ->e_vert->num << ")<<是否有效" << e3->alive << '\n';


		while (e6->e_succ->e_succ == e5->e_succ->e_pair)
		{
			e6->alive = false;
			e5->alive = false;
			e6->e_pair->alive = false;
			e5->e_pair->alive = false;
			e6->e_vert->alive = false;
			e5->e_succ->alive = false;
			e5->e_succ->e_pair->alive = false;
			e5 = e5->e_succ->e_succ->e_pair;
			e6 = e6->e_succ->e_pair;
		}

		while (e3->e_succ->e_succ==e4->e_succ->e_pair)
		{
			e3->alive = false;
			e4->alive = false;
			e3->e_pair->alive = false;
			e4->e_pair->alive = false;
			e3->e_vert->alive = false;
			e4->e_succ->alive = false;
			e4->e_succ->e_pair->alive = false;
			e4 = e4->e_succ->e_succ->e_pair;
			e3 = e3->e_succ->e_pair;
		}

		if (e4 == NULL)
		{
			std::cout << "e4 is boundary" << '\n';
		}
		//删除面
		if (e1 != NULL)
		{
			e1->e_face->alive = false;
		}
		if (e2 != NULL)
		{
			e2->e_face->alive = false;
		}

		//删除边
		e3->e_pair->alive = false;
		e4->e_pair->alive = false;
		e5->e_pair->alive = false;
		e6->e_pair->alive = false;
		e1->alive = false;
		e2->alive = false;
	/*
	if ((e6->alive == false && e4->alive == false) || (e3->alive == false && e5->alive == false))
	{
		std::cout << "有重边!" << '\n';

		std::cout << "e1:(" << e1->e_vert->num << "->" << e1->e_succ->e_vert->num << ")<<是否有效" << e1->alive << '\n';
		std::cout << "e2:(" << e2->e_vert->num << "->" << e2->e_succ->e_vert->num << ")<<是否有效" << e2->alive << '\n';
		std::cout << "e6:(" << e6->e_vert->num << "->" << e6->e_succ->e_vert->num << ")<<是否有效" << e6->alive << '\n';
		std::cout << "e5:(" << e5->e_vert->num << "->" << e5->e_succ->e_vert->num << ")<<是否有效" << e5->alive << '\n';
		std::cout << "e4:(" << e4->e_vert->num << "->" << e4->e_succ->e_vert->num << ")<<是否有效" << e4->alive << '\n';
		std::cout << "e3:(" << e3->e_vert->num << "->" << e3->e_succ->e_vert->num << ")<<是否有效" << e3->alive << '\n';

		std::cout << "e1:" << e1 << ' ' << "e2:" << e2 << " e3:" << e3 << " e4:" << e4 << " e5:" << e5 << " e6:" << e6 << '\n';
		std::cout << "e1->pair:" << e1->e_pair << "  e2->pair" << e2->e_pair << "  e3->pair:" << e3->e_pair << "  e4->pair:" << e4->e_pair << "  e5->pair:" << e5->e_pair << "  e6->pair:" << e6->e_pair << '\n';


		e6->e_pair = e4;
		e4->e_pair = e6;
		e3->e_pair = e5;
		e5->e_pair = e3;
		e1->e_pair = e2;
		e2->e_pair = e1;

		e6->alive = true;
		e4->alive = true;
		e5->alive = true;
		e3->alive = true;
		e2->alive = true;
		e1->alive = true;

		system("pause");
	}*/

		//链接对边
		e4->e_pair = e3;
/*
		std::cout << "构成互相的半边:\n";
		std::cout << "                 (" << e4->e_vert->num << "->" << e4->e_succ->e_vert->num << ")\n";
		std::cout << "               (" << e4->e_pair->e_vert->num << "->" << e4->e_pair->e_succ->e_vert->num << ")\n";
*/
		e3->e_pair = e4;

/*		std::cout << "构成互相的半边:\n";
		std::cout << "                 (" << e3->e_vert->num << "->" << e3->e_succ->e_vert->num << ")\n";
		std::cout << "               (" << e3->e_pair->e_vert->num << "->" << e3->e_pair->e_succ->e_vert->num << ")\n";
*/
		e6->e_pair = e5;

/*		std::cout << "构成互相的半边:\n";
		std::cout << "                 (" << e6->e_vert->num << "->" << e6->e_succ->e_vert->num << ")\n";
		std::cout << "               (" << e6->e_pair->e_vert->num << "->" << e6->e_pair->e_succ->e_vert->num << ")\n";
*/
		e5->e_pair = e6;

/*		std::cout << "构成互相的半边:\n";
		std::cout << "                 (" << e5->e_vert->num << "->" << e5->e_succ->e_vert->num << ")\n";
		std::cout << "               (" << e5->e_pair->e_vert->num << "->" << e5->e_pair->e_succ->e_vert->num << ")\n";
*/


		/*if (e3->alive == false || e4->alive == false || e5->alive == false || e6->alive == false)
		{
			std::cout << "有重边！" << '\n';
			system("pause");
		}*/


		//改变和v2有关的所有的信息
		//点的信息(与某些点相关的线和面不能是已经删除的面和线)
		e1->e_vert->v_edge = e5;
		e2->e_vert->v_edge = e4;
		e3->e_vert->v_edge = e3;
		e6->e_vert->v_edge = e6;

		e1->e_vert->v_face = e5->e_face;
		e3->e_vert->v_face = e3->e_face;
		e6->e_vert->v_face = e6->e_face;

		//改变边的信息
		edges ev2 = GetEdgesFromVertex(v2);
		for (int i = 0; i < ev2.size(); i++)
		{
			ev2[i]->e_vert = v1;

			for (int j = 0; j < 3; j++)
			{
				if (ev2[i]->e_face->f_verts[j] == v2)
					ev2[i]->e_face->f_verts[j] = v1;
			}

			caculateV(ev2[i]->e_face);
		}

		ev1 = GetEdgesFromVertex(v1);
		for (int i = 0; i < ev1.size(); i++)
		{
			caculateV(ev1[i]->e_face);
		}

		v2->alive = false;
	}

/*	std::cout << "以下为删除的边：" << '\n';
	for (int i = 0; i < m_edges.size(); i++)
	{
		if (m_edges[i]->alive == false)
		{
			std::cout << "(" << m_edges[i]->e_vert->num << "->" << m_edges[i]->e_succ->e_vert->num << ")\n";
		}
	}
*/


	return ev1;
}

/*HE_Vertex* HE_Mesh::collapse(HE_Edge* de)
{
	HE_Vertex *v1, *v2;
	v1 = de->e_vert; v2 = de->e_succ->e_vert;
	v2->alive = false;
	
	HE_Edge *e1, *e2, *e3, *e4, *e5, *e6;
	e1 = de; e2 = e1->e_succ; e3 = e2->e_succ;
	e4 = e1->e_pair; e5 = e4->e_succ; e6 = e5->e_succ;

	if(e3->e_vert == e6->e_vert || e3->e_pair == e2 || e6->e_pair==e5 )
	{
		return NULL;
	}

	e1->alive = e2->alive = e3->alive = e4->alive = e5->alive = e6->alive = false;
	e1->e_face->alive = e4->e_face->alive = false;

	std::cout << "e1: (" << e1->e_vert->num << "->" << e1->e_succ->e_vert->num << ")\n";
	std::cout << "e2: (" << e2->e_vert->num << "->" << e2->e_succ->e_vert->num << ")\n";
	std::cout << "e3: (" << e3->e_vert->num << "->" << e3->e_succ->e_vert->num << ")\n";
	std::cout << "e4: (" << e4->e_vert->num << "->" << e4->e_succ->e_vert->num << ")\n";
	std::cout << "e5: (" << e5->e_vert->num << "->" << e5->e_succ->e_vert->num << ")\n";
	std::cout << "e6: (" << e6->e_vert->num << "->" << e6->e_succ->e_vert->num << ")\n";

	std::cout << '\n' << '\n' << "点v1:" << v1->num << "的周围边:" << '\n';
	edges eee = GetEdgesFromVertex(v1);
	for (unsigned int i = 0; i < eee.size(); i++)
	{
		eee[i]->e_vert = v1;

		std::cout << "        (" << eee[i]->e_vert->num << "->" << eee[i]->e_succ->e_vert->num << ")\n";
	}

	std::cout << '\n' << '\n' << "点v2:" << v2->num << "的周围边:（把v2改成点v1）" << '\n';
	edges ee = GetEdgesFromVertex(v2);
	for (unsigned int i = 0; i < ee.size(); i++)
	{
		ee[i]->e_vert = v1;

		std::cout<<"        ("<< ee[i]->e_vert->num << "->" << ee[i]->e_succ->e_vert->num << ")\n";
	}

	e2->e_pair->e_pair = e3->e_pair;
	e3->e_pair->e_pair = e2->e_pair;
	e5->e_pair->e_pair = e6->e_pair;
	e6->e_pair->e_pair = e5->e_pair;

	while (v1->v_edge->alive == false)
	{		
		std::cout << "1" << '\n';
		v1->v_edge = v1->v_edge->e_succ->e_succ->e_pair;
	}
	while (e3->e_vert->v_edge->alive == false)
	{
		std::cout << "2" << '\n';
		e3->e_vert->v_edge = e3->e_vert->v_edge->e_succ->e_succ->e_pair;
	}
	while (e6->e_vert->v_edge->alive == false)
	{
		std::cout << "3" << '\n';
		e6->e_vert->v_edge = e6->e_vert->v_edge->e_succ->e_succ->e_pair;
	}

	while (v1->v_face->alive == false)
	{
	 	std::cout << "4" << '\n';
		v1->v_face = v1->v_edge->e_face;
	}
	while (e3->e_vert->v_face->alive == false)
	{
		std::cout << "5" << '\n';
		e3->e_vert->v_face = e3->e_vert->v_edge->e_face;
	}
	while (e6->e_vert->v_face->alive == false)
	{
		std::cout << "6" << '\n';
		e6->e_vert->v_face = e6->e_vert->v_edge->e_face;
	}

	std::cout << '\n' << '\n' << "更改v1后点v1:" << v1->num << "的周围边:" << '\n';
	eee = GetEdgesFromVertex(v1);
	for (unsigned int i = 0; i < eee.size(); i++)
	{
		eee[i]->e_vert = v1;

		std::cout << "        (" << eee[i]->e_vert->num << "->" << eee[i]->e_succ->e_vert->num << ")\n";
	}


	faces ff = GetFacesFromVertex(v1);
	for (unsigned int i = 0; i < ff.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			if (ff[i]->f_verts[j] == v2)
			{
				ff[i]->f_verts[j] = v1;
			}
		}
	}


	return v1;
}
*/

void HE_Mesh::initialDraw()
{
	glm::f32vec3 v1, v2, v3;
	glm::f32vec3 vv1, vv2, vv3;

	indices.clear();

	std::vector<glm::f32vec3> vec;
	std::map<HE_Vertex*, int> mapp;

	/*
		这里通过map的技巧实现了通过面给定顶点的索引值
		同时计算出每个顶点的法向量
		怎么实现的来着？我想想------------------------
		我忘了  我不知道是不是索引了  我记得我实现的时候是按照索引的方式实现了 可是现在感觉不对了  嘤嘤嘤 我忘了
		反正肯定是一个点有一个独一无二的num值  肯定对应绘图的索引
	*/

	int Index = 1;
	for (int i = 0; i < m_faces.size(); i++)
	{
		HE_Vertex *a, *b, *c;
		a = m_faces[i]->f_verts[0];
		b = m_faces[i]->f_verts[1];
		c = m_faces[i]->f_verts[2];
		v1 = glm::f32vec3(a->x, a->y, a->z);
		v2 = glm::f32vec3(b->x, b->y, b->z);
		v3 = glm::f32vec3(c->x, c->y, c->z);

		vv1 = glm::f32vec3(v2 - v1);
		vv2 = glm::f32vec3(v3 - v2);
		vv3 = glm::f32vec3(v3 - v1);

		if (!mapp[a])
		{
			mapp[a] = Index++;

			a->num = mapp[a];

			vec.push_back(v1);
			v1 = glm::cross(vv1, vv3);
			vec.push_back(v1);
		}
		indices.push_back(mapp[a] - 1);
		if (!mapp[b])
		{
			mapp[b] = Index++;

			b->num = mapp[b];

			vec.push_back(v2);
			v2 = glm::cross(vv2, -vv1);
			vec.push_back(v2);
		}
		indices.push_back(mapp[b] - 1);
		if (!mapp[c])
		{
			mapp[c] = Index++;

			c->num = mapp[c];

			vec.push_back(v3);
			v3 = glm::cross(-vv3, -vv2);
			vec.push_back(v3);
		}
		indices.push_back(mapp[c] - 1);
	}



	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(glm::f32vec3), &vec[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::f32vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::f32vec3), (void*)sizeof(glm::f32vec3));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void HE_Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void HE_Mesh::DeleteDraw()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}


void HE_Mesh::clear_waste()
{
	std::vector<HE_Vertex*> m_verts2;
	std::vector<HE_Edge*> m_edges2;
	std::vector<HE_Face*> m_faces2;

	int Node = GetNodes();
	int Edge = GetEdges();
	int Face = GetFaces();

	for (int i = 0; i < Node; i++)
	{
		if (m_verts[i]->alive)
		{
			m_verts2.push_back(m_verts[i]);
		}
		else
		{
			delete m_verts[i];
		}
	}
	m_verts.assign(m_verts2.begin(), m_verts2.end());
	m_verts2.~vector();

	for (int i = 0; i < Face; i++)
	{
		if (m_faces[i]->alive)
		{
			m_faces2.push_back(m_faces[i]);
		}
		else
		{
			delete m_faces[i];
		}
	}
	m_faces.assign(m_faces2.begin(), m_faces2.end());
	m_faces2.~vector();

	for (int i = 0; i < Edge; i++)
	{
		if (m_edges[i]->alive)
		{
			m_edges2.push_back(m_edges[i]);
		}
		else
		{
			delete m_edges[i];
		}
	}
	m_edges.assign(m_edges2.begin(), m_edges2.end());
	m_edges2.~vector();
}


#endif // !HALF_EDGE_CPP_


