
#include "PolyMesh.h"
#include "UESharedPluginPrivatePCH.h"
#include <iostream>

FPolymesh::FPolymesh(FPolymesh&& moved)
{
	m_cur_face_index = moved.m_cur_face_index;
	m_cur_halfedge_index = moved.m_cur_halfedge_index;
	m_cur_vertex_index = moved.m_cur_vertex_index;

	m_directed_edge2he_index = std::move(moved.m_directed_edge2he_index);
	m_face_halfedges = std::move(moved.m_face_halfedges);
	m_halfedges = std::move(moved.m_halfedges);
	m_vertex_halfedges = std::move(moved.m_vertex_halfedges);
	m_vertices = std::move(moved.m_vertices);
}

FPolymesh& FPolymesh::operator=(FPolymesh&&  moved)
{
	if (&moved == this)
		return *this;
	m_cur_face_index = moved.m_cur_face_index;
	m_cur_halfedge_index = moved.m_cur_halfedge_index;
	m_cur_vertex_index = moved.m_cur_vertex_index;

	m_directed_edge2he_index = std::move(moved.m_directed_edge2he_index);
	m_face_halfedges = std::move(moved.m_face_halfedges);
	m_halfedges = std::move(moved.m_halfedges);
	m_vertex_halfedges = std::move(moved.m_vertex_halfedges);
	m_vertices = std::move(moved.m_vertices);
	return *this;
}

VertexHandle FPolymesh::add_vertex(FVector p)
{
	auto index = m_cur_vertex_index++;
	m_vertices.Add(index,Vertex{ p });
	m_vertex_halfedges.Add(index,-1);
	return VertexHandle{ this,index };
}

FaceHandle FPolymesh::add_face(TArray<VertexHandle> handles)
{
	int first_he_index = -1;
	int pre_he_index = -1;
	int he_index = -1;
	index_t face_index = m_cur_face_index++;
	m_face_halfedges.Add(face_index,first_he_index);
	for (int i = 0; i < handles.Num(); i++) {
		int ni = i + 1 < handles.Num() ? i + 1 : 0;
		he_index = m_cur_halfedge_index++;
		m_halfedges.Add(he_index,halfedge_t());
		if (first_he_index < 0) {
			first_he_index = he_index;
		}

		auto& he = m_halfedges[he_index];
		he.to_vertex = handles[ni].index;
		he.pre_he = pre_he_index;
		he.face = face_index;

		TPair<index_t, index_t> pair(FMath::Min(handles[i].index, handles[ni].index), FMath::Max(handles[i].index, handles[ni].index));
		index_t* heindex = m_directed_edge2he_index.Find(pair);
		if (!heindex) {
			m_directed_edge2he_index.Add(pair, he_index);
		}
		else {
			he.opposite_he = *heindex;
			m_halfedges[*heindex].opposite_he = he_index;
		}


		
		if (pre_he_index >= 0) {
			m_halfedges[pre_he_index].next_he = he_index;
		}
		pre_he_index = he_index;

		if (m_vertex_halfedges[handles[i].index] == -1) {
			m_vertex_halfedges[handles[i].index] = he_index;
		}


		
	}
	m_halfedges[he_index].next_he = first_he_index;
	m_halfedges[first_he_index].pre_he = he_index;
	m_face_halfedges[face_index] = first_he_index;

	return { this,face_index };
}

void FPolymesh::del_face(FaceHandle handle)
{
	auto face_index = handle.index;
	index_t he_index = m_face_halfedges[face_index];
	index_t itr_he_index = he_index;
	TArray<index_t> del_vertex;
	do {
		auto he_data = m_halfedges[itr_he_index];
		auto from_vertex=m_halfedges[he_data.pre_he].to_vertex;
		if (m_vertex_halfedges[from_vertex] == itr_he_index) {
			bool bDelVertex = true;
			for (auto pair : m_halfedges) {
				if (pair.Value.to_vertex == from_vertex&& pair.Value.next_he != itr_he_index) {
					bDelVertex = false;
					m_vertex_halfedges[from_vertex] = pair.Value.next_he;
				}
			}
			if (bDelVertex) {
				del_vertex.Add(from_vertex);
			}
		}
		m_halfedges.Remove(itr_he_index);
		itr_he_index = he_data.next_he;
	} while (itr_he_index != he_index);
	for (auto vertex : del_vertex) {
		m_vertex_halfedges.Remove(vertex);
		m_vertices.Remove(vertex);
	}
	m_face_halfedges.Remove(face_index);
}

TArray<FaceHandle> FPolymesh::all_faces()
{
	TArray<FaceHandle> face_arr;
	for (auto pair: m_face_halfedges) {
		face_arr.Add({ this,pair.Key });
	}
	return face_arr;
}

TArray<VertexHandle> FPolymesh::all_vertices()
{
	TArray<VertexHandle> v_arr;
	for (auto pair : m_vertex_halfedges) {
		v_arr.Add({ this,pair.Key });
	}

	return v_arr;
}


TArray<HalfedgeHandle> FPolymesh::find_halfedges(VertexHandle lh, VertexHandle rh)
{
	TArray<HalfedgeHandle> he_arr;

	TPair<index_t, index_t> pair(FMath::Min(lh.index, rh.index), FMath::Max(lh.index, rh.index));
	index_t* heindex = m_directed_edge2he_index.Find(pair);
	if (heindex) {
		he_arr.Add({ this,*heindex });
		if (m_halfedges[*heindex].opposite_he >= 0) {
			he_arr.Add({ this,m_halfedges[*heindex].opposite_he });
		}
	}
	return he_arr;
}

FVector FPolymesh::calc_centroid(VertexHandle h)
{
	return m_vertices[h.index].Point;
}

FVector FPolymesh::calc_centroid(HalfedgeHandle h)
{
	index_t to_vertex = m_halfedges[h.index].to_vertex;
	index_t from_vertex = m_halfedges[m_halfedges[h.index].pre_he].to_vertex;
	return (m_vertices[to_vertex].Point + m_vertices[from_vertex].Point) / 2;
}

FVector FPolymesh::calc_centroid(VertexHandle lh, VertexHandle rh)
{
	return (m_vertices[lh.index].Point + m_vertices[rh.index].Point) / 2;
}

FVector FPolymesh::calc_centroid(FaceHandle fh)
{
	if (faceCentroidCache.Find(fh.index)) {
		return *faceCentroidCache.Find(fh.index);
	}
	FVector centroid(0,0,0);
	TArray<VertexHandle> vharr=fh.vertices();
	auto vnum = vharr.Num();
	auto trinum = vnum - 2;
	float totalArea=0;

	for (int i = 0; i < trinum; i++) {
		auto area = FVector::CrossProduct(calc_centroid(vharr[i + 2]) - calc_centroid(vharr[0]), calc_centroid(vharr[i + 1]) - calc_centroid(vharr[0])).Size()/2;
		totalArea += area;
		centroid += (calc_centroid(vharr[0]) + calc_centroid(vharr[i + 1]) + calc_centroid(vharr[i + 2])) * area / 3;
	}
	
	faceCentroidCache.FindOrAdd(fh.index) = centroid / totalArea;
	return faceCentroidCache[fh.index];
}



void FPolymesh::clean()
{
	m_vertices.Empty();
	m_vertex_halfedges.Empty();
	m_face_halfedges.Empty();
	m_halfedges.Empty();
	m_directed_edge2he_index.Empty();


	m_cur_vertex_index = 0;
	m_cur_face_index = 0;
	m_cur_halfedge_index = 0;



	faceCentroidCache.Empty();
}
