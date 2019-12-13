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

	std::cout << "��ķ������������" << '\n';

	for (int i = 0; i < m_verts.size(); i++)
	{
		GetQ(m_verts[i]);
	}
	std::cout << "����ÿ�����������" << '\n';

	setvec.clear();//���set

	for (int i = 0; i < m_edges.size(); i++)
	{
		GetCost(m_edges[i]);
		setvec.insert(*m_edges[i]);
	}

	std::cout << "�ߵ����ֵ�������" << '\n';


/*
	//���ÿһ���ߵ����
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
			���ｫev1ȡ������Ŀ�ľ���Ϊ����set�Ӷ����Ч��
			ֻ����Ҫ���ĵ����� ����set����;�������Ĺ��ܣ�
			��˿��Լ���Ľ�Ч������
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
	std::cout << "���۵��������" << '\n';

	clear_waste();

	std::cout << GetFaces() << '\n';
}


#endif // !GARLAND_H_
