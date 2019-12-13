#pragma once
/*
	two week
	The Fifth Expirement of the CG course
	including  Mesh Subdivsion and Mesh Simplification
	name��Ma
	ps: maybe it has bugs!!!

	class HE_Mesh  :::: HalfEdge_H
	make class HE_Mesh achieve ::::  subdivsion_H
	                                 Garland_H
									 HalfEdge_CPP
	main cpp::learn_loop_subdivsion_cpp

	Environment: Windows 10 64bit;
	             GLFW  OpenGL  3.3
				 GLM
				 Eigen
	
				include::::  myown::#include<learnopengl/camera.h>
									#include<learnopengl/shader.h>  Be Careful  !!!!

	IN ADDITION : BE CAREFUL!!!!!!!!!!
	              [The model must be no-boundary , if not ,it will exit or cycle forever]

	Update at 2019/11/28 21:27

*/

#ifndef HALFEDGE_H_
#define HALFEDGE_H_
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<vector>
#include<string.h>
#include<vector>
#include<queue>
#include<fstream>
#include<set>
#include<map>
#include<Eigen/Dense>    

struct HE_Vertex;
/*
	����Ķ���
	{
		����  ����  λ��  ��Ч  ����
	}
*/
struct HE_Edge;
/*
	��ߵĶ��� ÿһ���߷ֱ���Ӧ�������
	{
		��Ӧ ��� ���� ����  ���ֵ  �۵����γ��¶����λ��  ��Ч
	}
*/
struct HE_Face;
/*
	��Ķ���
	{
		����һ����  ����������Ϣ  ������  ��Ч 
	}
*/

struct HE_Vertex
{
	HE_Vertex() { alive = true; }
	HE_Edge * v_edge;//�Ըö���Ϊ�׶���ĳ���(����һ��)
	HE_Face * v_face;//�����ö������(����һ��)
	float x, y, z;   //�õ��λ����Ϣ
	bool alive;      //�õ��Ƿ���Ч
	int num;         //���������  ��û���õ� ���Ե�ʱ�����
};

struct HE_Edge
{
	HE_Edge() { alive = true; }
	HE_Edge * e_pair;		//��ż��
	HE_Edge * e_succ;		//��̱�
	HE_Vertex * e_vert;	    //�׶���
	HE_Face * e_face;		//�Ҳ���
	float precision;        //���ֵ
	float v_x, v_y, v_z;    //�¶���λ��
	bool alive;             //��Ч
};

struct HE_Face
{
	HE_Face() { alive = true; }
	HE_Edge * f_edge;               //���ڵ�����һ����
	std::vector<HE_Vertex*> f_verts;//��������
	glm::f32vec4 pvec;              //������
	bool alive;                     //��Ч
};





typedef std::vector<HE_Vertex*> verts;
/*
	��������
*/
typedef std::vector<HE_Face*> faces;
/*
	������
*/
typedef std::vector<HE_Edge*> edges;
/*
	������
*/

typedef std::pair<HE_Vertex*, HE_Vertex*> vert_pair;
/*
	���㵽����ĺ�����ϵ Ϊ�˷����ҵ�����Ӧ�ı���Ϣ
*/
typedef std::map<vert_pair, HE_Edge*> Map;
/*
	��Ӧ��һ���������  ͨ��mapӳ��ľ��Ǳ�
	�ڹ�����ߵĹ�������Ҫ�õ���Ϊ���ǻ�ȡ֮ǰ�Ѿ��������İ�ߵ���Ϣ
	�ñ� v1->v2 ����һ�����  ��ô���ҽ��� v2->v1��ʱ������ֱ�Ӿ������� 
	�������������
*/
typedef std::map<vert_pair, int> MyMap;
/*
	��Ӧ���������ϵ ӳ������  
	��ϸ�ֵ�ʱ����Ҫ�����������λ�߲������µĶ����λ��  
	��Ϊ�Ҳ�û�и����㵥������һ������洢
*/

//���ֵ�ͱߵĶ�Ӧ ���������ʵ������----���� δ��
typedef std::pair<float, HE_Edge *> pre_edge;
//��;���Ķ�Ӧ ����ʵ�ֻ��ĳ�����������--���� δ��
typedef std::map<HE_Vertex*, glm::mat4 > V_Matmap;
//������ֵ�Ķ�Ӧ��ϵ  -------���� δ��
typedef std::map<HE_Vertex*, float > V_prevalue;


class HE_Mesh
{
public:

	//����OBJ�ļ�
	void LoadFromObj(std::string file);

	//ϸ�ֲ���
	void Subdivision();
	//�򻯲���
	void Simplify_Garland();

	//������ɫ������ʹ�÷�ʽ  �Լ���ض��������İ󶨵ȵ�
	void initialDraw();
	//����ģ��
	void Draw();
	//ɾ��GPU����
	void DeleteDraw();

	//�жϱ߽�����  ��֪���Բ��ԣ�
	bool IsBoundary(HE_Edge *edge)
	{	
		if (edge == nullptr)
		{	return true;	}
		else
		{	return false;	}
	}
	int GetNodes()
	{//�õ�������Ŀ
		return m_verts.size();
	}
	int GetFaces()
	{//�õ������Ŀ
		return m_faces.size();
	}
	int GetEdges()
	{//�õ��ߵ���Ŀ
		return m_edges.size();
	}

protected:

	//�����Ĳ���
	HE_Vertex* InsertVertex(float x, float y, float z);
	//������Ĳ���
	HE_Face*  InsertFace(HE_Vertex *v1, HE_Vertex *v2, HE_Vertex *v3);
	//����ߵĲ���
	HE_Edge* InsertEdge(HE_Vertex * v1, HE_Vertex * v2);

	//������ĳ����Ϊ�׶�������а��
	edges GetEdgesFromVertex(HE_Vertex * vert);
	//����ĳ��������ڵ�
	verts GetVertsFromVertex(HE_Vertex * vert);
	//����ĳ���������������
	faces GetFacesFromVertex(HE_Vertex * vert);
	//����ĳ����ߵ��������ڰ��
	edges GetEdgesFromEdge(HE_Edge * edge);
	//����ĳ���������Q ������ӵ�vmmap��----��ʵ�� δ��
	glm::mat4 caculateQ(HE_Vertex* vert);
	//������ĳ����ķ�����
	glm::f32vec4 caculateV(HE_Face *face);
	//����ĳ�ߵ�cost----��ʵ�� δ��
	float caculateCost(HE_Edge *edge);

	//����һ��ƽ����ĸ�a b c d ��ֵ
 	Eigen::Vector4d Getplane(HE_Face *face);
	//�õ�ĳ�������������Q
	Eigen::Matrix4d GetQ(HE_Vertex* vert);
	//�õ�ĳ�ߵ�cost
	float GetCost(HE_Edge* edge);
	//collapse����
	edges collapse(HE_Edge* edge);
	//��������
	void clear_waste();

private:
	
	std::vector<HE_Vertex*> m_verts;
	//��������
	std::vector<HE_Edge*> m_edges;
	//������
	std::vector<HE_Face*> m_faces;
	//������
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	std::vector<int> indices;
	//����ʱ�򶥵��������Ϣ

	Map m_emap;
	//������ӳ��һ����
	V_Matmap v_mat;//û�õ�
};



#endif // !HALFEDGE_H_
