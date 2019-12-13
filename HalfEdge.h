#pragma once
/*
	two week
	The Fifth Expirement of the CG course
	including  Mesh Subdivsion and Mesh Simplification
	name：Ma
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
	顶点的定义
	{
		出边  邻面  位置  有效  索引
	}
*/
struct HE_Edge;
/*
	半边的定义 每一条边分别会对应两条半边
	{
		对应 后继 顶点 邻面  误差值  折叠后形成新顶点的位置  有效
	}
*/
struct HE_Face;
/*
	面的定义
	{
		面内一条边  三个顶点信息  法向量  有效 
	}
*/

struct HE_Vertex
{
	HE_Vertex() { alive = true; }
	HE_Edge * v_edge;//以该顶点为首顶点的出边(任意一条)
	HE_Face * v_face;//包含该顶点的面(任意一张)
	float x, y, z;   //该点的位置信息
	bool alive;      //该点是否有效
	int num;         //顶点的索引  并没有用到 调试的时候可用
};

struct HE_Edge
{
	HE_Edge() { alive = true; }
	HE_Edge * e_pair;		//对偶边
	HE_Edge * e_succ;		//后继边
	HE_Vertex * e_vert;	    //首顶点
	HE_Face * e_face;		//右侧面
	float precision;        //误差值
	float v_x, v_y, v_z;    //新顶点位置
	bool alive;             //有效
};

struct HE_Face
{
	HE_Face() { alive = true; }
	HE_Edge * f_edge;               //面内的任意一条边
	std::vector<HE_Vertex*> f_verts;//三个顶点
	glm::f32vec4 pvec;              //法向量
	bool alive;                     //有效
};





typedef std::vector<HE_Vertex*> verts;
/*
	顶点数组
*/
typedef std::vector<HE_Face*> faces;
/*
	面数组
*/
typedef std::vector<HE_Edge*> edges;
/*
	边数组
*/

typedef std::pair<HE_Vertex*, HE_Vertex*> vert_pair;
/*
	顶点到顶点的合作关系 为了方便找到它对应的边信息
*/
typedef std::map<vert_pair, HE_Edge*> Map;
/*
	对应上一个顶点合作  通过map映射的就是边
	在构建半边的过程中需要用到，为的是获取之前已经建立过的半边的信息
	好比 v1->v2 建立一条半边  那么当我建立 v2->v1的时候岂不是直接就能用了 
	具体代码有体现
*/
typedef std::map<vert_pair, int> MyMap;
/*
	对应顶点合作关系 映射索引  
	在细分的时候需要用这个索引定位边产生的新的顶点的位置  
	因为我并没有给顶点单独设置一个数组存储
*/

//误差值和边的对应 用来插入堆实现排序----定义 未用
typedef std::pair<float, HE_Edge *> pre_edge;
//点和矩阵的对应 用来实现获得某个点的误差矩阵--定义 未用
typedef std::map<HE_Vertex*, glm::mat4 > V_Matmap;
//点和误差值的对应关系  -------定义 未用
typedef std::map<HE_Vertex*, float > V_prevalue;


class HE_Mesh
{
public:

	//读入OBJ文件
	void LoadFromObj(std::string file);

	//细分操作
	void Subdivision();
	//简化操作
	void Simplify_Garland();

	//定义着色器数据使用方式  以及相关顶点索引的绑定等等
	void initialDraw();
	//绘制模型
	void Draw();
	//删除GPU缓冲
	void DeleteDraw();

	//判断边界条件  不知道对不对？
	bool IsBoundary(HE_Edge *edge)
	{	
		if (edge == nullptr)
		{	return true;	}
		else
		{	return false;	}
	}
	int GetNodes()
	{//得到顶点数目
		return m_verts.size();
	}
	int GetFaces()
	{//得到面的数目
		return m_faces.size();
	}
	int GetEdges()
	{//得到边的数目
		return m_edges.size();
	}

protected:

	//插入点的操作
	HE_Vertex* InsertVertex(float x, float y, float z);
	//插入面的操作
	HE_Face*  InsertFace(HE_Vertex *v1, HE_Vertex *v2, HE_Vertex *v3);
	//插入边的操作
	HE_Edge* InsertEdge(HE_Vertex * v1, HE_Vertex * v2);

	//访问以某个点为首顶点的所有半边
	edges GetEdgesFromVertex(HE_Vertex * vert);
	//访问某个点的相邻点
	verts GetVertsFromVertex(HE_Vertex * vert);
	//访问某个点的所有相邻面
	faces GetFacesFromVertex(HE_Vertex * vert);
	//访问某条半边的所有相邻半边
	edges GetEdgesFromEdge(HE_Edge * edge);
	//计算某点的误差矩阵Q 并且添加到vmmap中----已实现 未用
	glm::mat4 caculateQ(HE_Vertex* vert);
	//计算所某个面的法向量
	glm::f32vec4 caculateV(HE_Face *face);
	//计算某边的cost----已实现 未用
	float caculateCost(HE_Edge *edge);

	//构建一个平面的四个a b c d 的值
 	Eigen::Vector4d Getplane(HE_Face *face);
	//得到某个顶点的误差矩阵Q
	Eigen::Matrix4d GetQ(HE_Vertex* vert);
	//得到某边的cost
	float GetCost(HE_Edge* edge);
	//collapse操作
	edges collapse(HE_Edge* edge);
	//垃圾清理
	void clear_waste();

private:
	
	std::vector<HE_Vertex*> m_verts;
	//顶点数组
	std::vector<HE_Edge*> m_edges;
	//边数组
	std::vector<HE_Face*> m_faces;
	//面数组
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	std::vector<int> indices;
	//绘制时候顶点的索引信息

	Map m_emap;
	//两个点映射一条边
	V_Matmap v_mat;//没用到
};



#endif // !HALFEDGE_H_
