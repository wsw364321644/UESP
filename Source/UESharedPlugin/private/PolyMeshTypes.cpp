#include "PolyMeshTypes.h"
#include "PolyMesh.h"
VertexHandle HalfedgeHandle::from()
{
	return  { mesh,mesh->m_halfedges[mesh->m_halfedges[index].pre_he].to_vertex };
}

VertexHandle HalfedgeHandle::to()
{
	return  { mesh, mesh->m_halfedges[index].to_vertex };
}

TArray<VertexHandle> FaceHandle::vertices()
{
	TArray<VertexHandle> v_arr;
	index_t he_index = mesh->m_face_halfedges[index];
	v_arr.Add({ mesh,mesh->m_halfedges[he_index].to_vertex });
	for (index_t i = mesh->m_halfedges[he_index].next_he; i != he_index; i = mesh->m_halfedges[i].next_he){
		v_arr.Add({ mesh,mesh->m_halfedges[i].to_vertex });
	}
	return v_arr;
}

TArray<HalfedgeHandle> FaceHandle::halfedges()
{
	TArray<HalfedgeHandle> he_arr;
	index_t he_index = mesh->m_face_halfedges[index];
	he_arr.Add({ mesh,he_index });
	for (int i = mesh->m_halfedges[he_index].next_he; i != he_index; i = mesh->m_halfedges[i].next_he) {
		he_arr.Add({ mesh,i });
	}

	return he_arr;
}

TArray<FaceHandle> VertexHandle::faces()
{
	TArray<FaceHandle> arr;
	auto first_he_index=mesh->m_vertex_halfedges[index];
	arr.Add({ mesh, mesh->m_halfedges[first_he_index].face });
	for (int i = mesh->m_halfedges[mesh->m_halfedges[first_he_index].opposite_he].next_he; i != first_he_index; i = mesh->m_halfedges[mesh->m_halfedges[i].opposite_he].next_he) {
		arr.Add({ mesh, mesh->m_halfedges[i].face });
	}
	return arr;
}

Vertex& VertexHandle::GetData()const
{
	return mesh->m_vertices[index];
}
