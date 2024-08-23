#pragma once


typedef long index_t;


class FPolymesh;
struct HalfedgeHandle;
struct FaceHandle;
struct VertexHandle;


struct Vertex
{
    FVector Point;
};

struct BaseHandle
{
    friend class FPolymesh;
    BaseHandle() :index(-1)
    {}
    BaseHandle(FPolymesh* inmesh, int i) :index(i), mesh(inmesh)
    {}
    index_t index;
    FPolymesh* mesh;
    bool IsValid() { return index >= 0; }
    index_t idx() { return index; }

    bool operator == (const BaseHandle& h) const {
        return index == h.index;
    }
    bool operator != (const BaseHandle& h)const {
        return index != h.index;
    }
    bool operator > (const BaseHandle& h)const {
        return index > h.index;
    }
    bool operator >=  (const BaseHandle& h)const {
        return index >= h.index;
    }

    bool operator < (const BaseHandle& h) const {
        return index < h.index;
    }
    bool operator <= (const BaseHandle& h)const {
        return index <= h.index;
    }
};


struct UESHAREDPLUGIN_API VertexHandle:BaseHandle
{
   
    VertexHandle() :BaseHandle()
    {}
    VertexHandle(FPolymesh* inmesh, int i) :BaseHandle(inmesh,i)
    {}

    TArray<FaceHandle> faces();
    Vertex& GetData ()const;
};
struct UESHAREDPLUGIN_API FaceHandle:BaseHandle
{

    FaceHandle() :BaseHandle()
    {}
 
    FaceHandle(FPolymesh* inmesh, int i) :BaseHandle(inmesh, i)
    {}
    
    TArray< VertexHandle> vertices();
    TArray< HalfedgeHandle> halfedges();
};
struct UESHAREDPLUGIN_API HalfedgeHandle:BaseHandle
{

    HalfedgeHandle() :BaseHandle()
    {}
    HalfedgeHandle(FPolymesh* inmesh, int i) :BaseHandle(inmesh, i)
    {}

    VertexHandle from();
    VertexHandle to();
};


