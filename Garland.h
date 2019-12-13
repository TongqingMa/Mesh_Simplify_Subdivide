#pragma once
#ifndef GARLAND_H_
#define GARLAND_H_

#include<iostream>
#include<vector>

bool operator < (HE_Edge a, HE_Edge b)
{
	return a.precision < b.precision;
}


std::set<HE_Edge> setvec;

void HE_Mesh::Simplify_Garland()
{
	int curNode = GetNodes();
	int curFace = GetFaces();
	int curEdge = GetEdges();


	for (int i = 0; i < m_faces.size(); i++)
	{
		caculateV(m_faces[i]);
	}

	std::cout << "面的法向量计算完成" << '\n';

	for (int i = 0; i < m_verts.size(); i++)
	{
		GetQ(m_verts[i]);
	}
	std::cout << "计算每个点的误差矩阵" << '\n';

	setvec.clear();//清空set

	for (int i = 0; i < m_edges.size(); i++)
	{
		GetCost(m_edges[i]);
		setvec.insert(*m_edges[i]);
	}

	std::cout << "边的误差值计算完成" << '\n';


/*
	//输出每一条边的误差
	for (auto i = setvec.begin(); i != setvec.end(); i++)
	{
		std::cout << (*i).precision << '\n';
	}
*/

	int getFace = curFace * 0.5;
	while (curFace > getFace)
	{
		HE_Edge minE = *setvec.begin();
		setvec.erase(minE);

		if (minE.e_vert->alive == false || minE.e_succ->e_vert->alive == false || minE.e_succ->alive == false || minE.e_succ->e_succ->alive == false || minE.e_succ->e_succ->e_succ->alive == false)
		{
			continue;
		}
		HE_Edge *temp = minE.e_succ->e_succ->e_succ;
		edges ev1;

		ev1 = collapse(temp);

		/*
			这里将ev1取出来的目的就是为了用set从而提高效率
			只更改要更改的数据 并且set本身就具有排序的功能，
			因此可以极大改进效率问题
		*/

		for (int k = 0; k < ev1.size(); k++)
		{
			HE_Edge tev1 = *ev1[k];
			setvec.erase(tev1);
			GetCost(ev1[k]);
			setvec.insert(*ev1[k]);

			tev1 = *ev1[k]->e_pair;
			setvec.erase(tev1);
			GetCost(ev1[k]->e_pair);
			setvec.insert(*ev1[k]->e_pair);
		}

		curFace -= 2;

	}
	std::cout << "边折叠操作完成" << '\n';

	clear_waste();

	std::cout << GetFaces() << '\n';
}


#endif // !GARLAND_H_
