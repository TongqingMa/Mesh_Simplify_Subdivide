
#include"HalfEdge.h"

#define PI 3.1415926

void HE_Mesh::Subdivision()
{
	std::cout << "����ϸ�ֺ���!" << '\n';

	faces face2;
	edges edge2;

	int Nnode = m_verts.size();//δ�����¶���ʱ�Ķ������

	std::cout << "Nnodefirst:" << Nnode << '\n';

	/////////////////////////////////////////////////////////////////
	MyMap mapone;
	int Nedge = m_edges.size();

	std::cout << "Nedge:" << Nedge << '\n';
	float qx, qy, qz;
	for (int i = 0; i < Nedge; i++)
	{
		if (!mapone[vert_pair(m_edges[i]->e_vert, m_edges[i]->e_succ->e_vert)])
		{
			HE_Vertex* v1 = m_edges[i]->e_vert;
			HE_Vertex* v2 = m_edges[i]->e_succ->e_vert;
			HE_Vertex* v3 = m_edges[i]->e_succ->e_succ->e_vert;
			HE_Vertex* v4 = m_edges[i]->e_pair->e_succ->e_succ->e_vert;

			qx = 0.375*(v1->x + v2->x) + 0.125*(v3->x + v4->x);
			qy = 0.375*(v1->y + v2->y) + 0.125*(v3->y + v4->y);
			qz = 0.375*(v1->z + v2->z) + 0.125*(v3->z + v4->z);

			InsertVertex(qx, qy, qz);

			mapone[vert_pair(m_edges[i]->e_vert, m_edges[i]->e_succ->e_vert)] = m_verts.size() - 1;
			mapone[vert_pair(m_edges[i]->e_succ->e_vert, m_edges[i]->e_vert)] = m_verts.size() - 1;
			//���ﷸ��һ������  �����˰���  ����Ϊ��������Ӧ��û�ж����ö�Ӧ��m_vertes.size()-1;
			//�⵼��ĳһ���ߵĶ�Ӧ�ߵ�����Ϊ0�ˣ���ֵΪ��Ӧ�ߵĲ����ں��������Ǿͻ�Ѷ�Ӧ�߻���һ�� ��ͻᵼ�����Ӻö���µ�
			//�ոս��  ������������  ���������ܽ�
		}
	}
	std::cout << "�µ��������!" << '\n';
	//�µ��Ҿ�ֱ�Ӽ����˶�����������  û�е�������


	int n;
	float p_sumx, p_sumy, p_sumz;
	float px, py, pz;
	float beta;

	for (int i = 0; i < Nnode; i++)
	{
		verts othernode = GetVertsFromVertex(m_verts[i]);
		n = othernode.size();
		p_sumx = p_sumy = p_sumz = 0;
		for (int j = 0; j < n; j++)
		{
			p_sumx += othernode[j]->x;
			p_sumy += othernode[j]->y;
			p_sumz += othernode[j]->z;
		}
		beta = 1 / (double)n*(0.625 - pow(0.375 + 0.25*cos(2 * PI / n), 2));

		px = (1 - n * beta)*m_verts[i]->x + beta * p_sumx;
		py = (1 - n * beta)*m_verts[i]->y + beta * p_sumy;
		pz = (1 - n * beta)*m_verts[i]->z + beta * p_sumz;

		m_verts[i]->x = px;
		m_verts[i]->y = py;
		m_verts[i]->z = pz;
	}
	std::cout << "�ϵ�������!" << '\n';


	int Nface = m_faces.size();

	face2.assign(m_faces.begin(), m_faces.end());
	m_faces.clear();
	m_edges.clear();
	m_emap.clear();
	//����ԭ�����潨�µ��� ͬʱ�����µİ��
	//һ��Ҫ��֮ǰ������ߵĶ�Ӧ��ϵ��֮ǰ�������� �����鶼��հ�������������������
	HE_Vertex *a, *b, *c, *d, *e, *f;
	for (int i = 0; i < Nface; i++)
	{
		a = face2[i]->f_verts[0];
		b = face2[i]->f_verts[1];
		c = face2[i]->f_verts[2];
		d = m_verts[mapone[vert_pair(a, b)]];
		e = m_verts[mapone[vert_pair(b, c)]];
		f = m_verts[mapone[vert_pair(a, c)]];

		InsertFace(a, d, f);
		InsertFace(d, b, e);
		InsertFace(d, e, f);
		InsertFace(f, e, c);

	}
	std::cout << "�����±��������!" << '\n';

	std::cout << "Nnode:" << GetNodes() << '\n';
	std::cout << "Nface:" << GetFaces() << '\n';
	std::cout << "Nedge:" << GetEdges() << '\n';

}



