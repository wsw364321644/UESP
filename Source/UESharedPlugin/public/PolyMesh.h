#pragma once


#include "PolyMeshTypes.h"
#include "CoreMinimal.h"



class UESHAREDPLUGIN_API FPolymesh
{
public:
    FPolymesh() { clean(); }
    FPolymesh(FPolymesh&&);
    FPolymesh(FPolymesh&)=delete;

    FPolymesh& operator=(FPolymesh&&);

	VertexHandle add_vertex(FVector p);
    FaceHandle add_face(TArray< VertexHandle> handles);

    void del_face(FaceHandle handle);

    TArray < FaceHandle> all_faces();
    TArray <VertexHandle> all_vertices();
    TArray<HalfedgeHandle> find_halfedges(VertexHandle lh, VertexHandle rh);

    FVector calc_centroid(VertexHandle);
    FVector calc_centroid(HalfedgeHandle);
    FVector calc_centroid(VertexHandle, VertexHandle);
    FVector calc_centroid(FaceHandle);
	void clean();

   


    struct halfedge_t
    {
        // Index into the vertex array.
        index_t to_vertex;
        // Index into the face array.
        index_t face;

        // Index into the halfedges array.
        index_t opposite_he;
        // Index into the halfedges array.
        index_t next_he;
        // Index into the halfedges array.
        index_t pre_he;
        halfedge_t() :
            to_vertex(-1),
            face(-1),
            opposite_he(-1),
            next_he(-1)
        {}
    };

	// Offsets into the 'halfedges' sequence, one per vertex.
    TMap<index_t,  index_t > m_vertex_halfedges;
	// Offset into the 'halfedges' sequence, one per face.
    TMap<  index_t, index_t > m_face_halfedges;
    // Offset
    TMap< index_t, halfedge_t > m_halfedges;

    TMap<index_t, Vertex> m_vertices;

    TMap<index_t, FVector> faceCentroidCache;
    index_t m_cur_vertex_index;
    index_t m_cur_face_index;
    index_t m_cur_halfedge_index;

    typedef TMap<TPair< index_t, index_t >, index_t > directed_edge2index_map_t;
    directed_edge2index_map_t m_directed_edge2he_index;
};

