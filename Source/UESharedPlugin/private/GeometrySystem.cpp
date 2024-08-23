
#include "GeometrySystem.h"
#include <iostream>
#include<map>
FPolymesh FGeometrySystem::GetIcosahedronMuiltFour(bool dual, int level)
{
	FPolymesh mesh;
	TArray<VertexHandle>  vhandles;
	float m = FMath::Sqrt(50 - 10 * FMath::Sqrt(5.0)) / 10;
	float n = FMath::Sqrt(50 + 10 * FMath::Sqrt(5.0)) / 10;
	float vertexes[] = { m, 0, n, m,0,-n, -m,0,n, -m,0,-n
	,0, n, m,0, -n, m,0, n, -m,0, -n, -m
	,n, m, 0,-n, m, 0,n, -m, 0,-n, -m, 0 };
	for (int i = 0; i < 12; i++) {
		vhandles.Add(mesh.add_vertex(FVector(vertexes[i*3], vertexes[i*3+1], vertexes[i*3+2])));
	}


	TArray<VertexHandle>  face_vhandles;
	int Ts[] = {6, 4, 8, 9, 4, 6, 6, 3, 9, 6, 1, 3, 6, 8, 1,
		8, 10, 1, 8, 0, 10, 8, 4, 0, 4, 2, 0, 4, 9, 2,
		9, 11, 2, 9, 3, 11, 3, 1, 7, 1, 10, 7, 10, 0, 5,
		0, 2, 5, 2, 11, 5, 3, 7, 11, 5, 11, 7, 10, 5, 7};


	for (int i = 0; i < 20; i++) {
		face_vhandles.Empty();
		face_vhandles.Add(vhandles[Ts[i * 3 + 2]]);
		face_vhandles.Add(vhandles[Ts[i * 3 + 1]]);
		face_vhandles.Add(vhandles[Ts[i * 3 + 0]]);
		mesh.add_face(face_vhandles);
	}


	FPolymesh tmpmesh;
	std::map<VertexHandle, std::map <VertexHandle, VertexHandle>> recordedmidvertexes;
	std::map<VertexHandle,  VertexHandle> recordedvertexes;
	for (int l = 0; l < level; l++) {	
		tmpmesh.clean();
		recordedmidvertexes.clear();
		recordedvertexes.clear();
		TArray <FaceHandle> faces= mesh.all_faces();
		/// iter face handle
		for (auto face:faces) {

			/// get edges  handle in face
			TArray <HalfedgeHandle> halfedges = face.halfedges();
			for (auto he:halfedges) {

				vhandles.Empty();
				bool maxrecord = false;
				bool minrecord = false;
				bool midrecord = false;
				VertexHandle maxh = FMath::Max(he.from(), he.to());
				VertexHandle minh = FMath::Min(he.from(), he.to());
				
				if (recordedmidvertexes.find(minh) != recordedmidvertexes.end()&& recordedmidvertexes[minh].find(maxh) != recordedmidvertexes[minh].end()) {
					minrecord=midrecord = maxrecord = true;
				}
				else {
					if (recordedvertexes.find(minh) != recordedvertexes.end()) {
						minrecord = true;
					}
					if (recordedvertexes.find(maxh) != recordedvertexes.end()) {
						maxrecord = true;
					}
				}
				if (!maxrecord) {
					recordedvertexes[maxh]= tmpmesh.add_vertex(mesh.calc_centroid(maxh));
				}
				if (!minrecord) {
					recordedvertexes[minh]=tmpmesh.add_vertex(mesh.calc_centroid(minh));
				}
				if (!midrecord) {
					auto point = mesh.calc_centroid(he);
					point.Normalize();
					auto h=tmpmesh.add_vertex(point);
					recordedmidvertexes[minh][maxh] = h;
				}

			}
			auto findmid = [&](VertexHandle fromindex, VertexHandle toindex)->VertexHandle {
				VertexHandle maxh = FMath::Max(fromindex, toindex);
				VertexHandle minh = FMath::Min(fromindex, toindex);
				return recordedmidvertexes[minh][maxh];
			};
			/// divide vertices   in face
			int DTs[] = { 0,3,5,3,1,4,5,3,4,5,4,2};
			vhandles =face.vertices();

			vhandles.Add(findmid(vhandles[0], vhandles[1]));
			vhandles.Add(findmid(vhandles[1], vhandles[2]));
			vhandles.Add(findmid(vhandles[2], vhandles[0]));
			vhandles[0] = recordedvertexes[vhandles[0]];
			vhandles[1] = recordedvertexes[vhandles[1]];
			vhandles[2] = recordedvertexes[vhandles[2]];


			for (int i = 0; i < 4; i++) {
				face_vhandles.Empty();
				face_vhandles.Add(vhandles[DTs[i * 3 + 0]]);
				face_vhandles.Add(vhandles[DTs[i * 3 + 1]]);
				face_vhandles.Add(vhandles[DTs[i * 3 + 2]]);
				tmpmesh.add_face(face_vhandles);
			}
		}
		mesh = std::move(tmpmesh);
	}

	if (dual) {
		FPolymesh dmesh;
		TArray<VertexHandle> vharr = mesh.all_vertices();

		std::map<FaceHandle, VertexHandle> recordedfacevertexes;
		for (auto vh : vharr) {
			vhandles.Empty();
			auto fharr = vh.faces();

			for (auto fh : fharr) {

				if (recordedfacevertexes.find(fh) == recordedfacevertexes.end()) {
					recordedfacevertexes[fh] = dmesh.add_vertex(mesh.calc_centroid(fh));
				}
				vhandles.Insert(recordedfacevertexes[fh], 0);
			}
			dmesh.add_face(vhandles);
		}
		return dmesh;
	}
	return mesh;
}

FPolymesh FGeometrySystem::GetIcosahedron(bool dual,int m, int n)
{
	//FPolymesh mesh= GetIcosahedronMuiltFour(false,0);
	//TArray<VertexHandle>  vhandles;
	//FPolymesh tmpmesh;
	//std::map<std::pair<VertexHandle, VertexHandle>, std::vector<VertexHandle>> recordedgeodesicvertexes;//old->new
	//std::map<VertexHandle, VertexHandle> recordedvertexes; //old->new


	//	tmpmesh.clean();
	//	recordedgeodesicvertexes.clear();
	//	recordedvertexes.clear();
	//	TArray <FaceHandle> faces = mesh.all_faces();
	//	/// iter face handle
	//	for (auto face : faces) {
	//		TArray<VertexHandle> oldvers=face.vertices();
	//		TArray <HalfedgeHandle> halfedges = face.halfedges();
	//		for (auto he : halfedges) {

	//			vhandles.Empty();
	//			bool maxrecord = false;
	//			bool minrecord = false;
	//			bool midrecord = false;
	//			VertexHandle maxh = FMath::Max(he.from(), he.to());
	//			VertexHandle minh = FMath::Min(he.from(), he.to());

	//			if (recordedgeodesicvertexes.find(std::pair<VertexHandle, VertexHandle>(minh, maxh))!= recordedgeodesicvertexes.end()) {
	//				minrecord = midrecord = maxrecord = true;
	//			}
	//			else {
	//				if (recordedvertexes.find(minh) != recordedvertexes.end()) {
	//					minrecord = true;
	//				}
	//				if (recordedvertexes.find(maxh) != recordedvertexes.end()) {
	//					maxrecord = true;
	//				}
	//			}
	//			if (!maxrecord) {
	//				recordedvertexes[maxh] = tmpmesh.add_vertex(mesh.calc_centroid(maxh));
	//			}
	//			if (!minrecord) {
	//				recordedvertexes[minh] = tmpmesh.add_vertex(mesh.calc_centroid(minh));
	//			}
	//			if (!midrecord) {
	//				auto fromdata= he.from().GetData();
	//				auto todata = he.to().GetData();
	//				auto point = mesh.calc_centroid(he);
	//				point.Normalize();
	//				auto h = tmpmesh.add_vertex(point);
	//				for () {

	//				}
	//			}

	//		}
	//		auto findmid = [&](VertexHandle fromindex, VertexHandle toindex)->VertexHandle {
	//			VertexHandle maxh = FMath::Max(fromindex, toindex);
	//			VertexHandle minh = FMath::Min(fromindex, toindex);
	//			return recordedmidvertexes[minh][maxh];
	//		};
	//		/// divide vertices   in face
	//		int DTs[] = { 0,3,5,3,1,4,5,3,4,5,4,2 };
	//		vhandles = face.vertices();

	//		vhandles.Add(findmid(vhandles[0], vhandles[1]));
	//		vhandles.Add(findmid(vhandles[1], vhandles[2]));
	//		vhandles.Add(findmid(vhandles[2], vhandles[0]));
	//		vhandles[0] = recordedvertexes[vhandles[0]];
	//		vhandles[1] = recordedvertexes[vhandles[1]];
	//		vhandles[2] = recordedvertexes[vhandles[2]];


	//		for (int i = 0; i < 4; i++) {
	//			face_vhandles.Empty();
	//			face_vhandles.Add(vhandles[DTs[i * 3 + 0]]);
	//			face_vhandles.Add(vhandles[DTs[i * 3 + 1]]);
	//			face_vhandles.Add(vhandles[DTs[i * 3 + 2]]);
	//			tmpmesh.add_face(face_vhandles);
	//		}



	//		TArray<VertexHandle>
	//			TArray<VertexHandle>
	//		for (int i = 0; i < m; i++) {
	//			int fnum = i * 2 + 1;
	//			
	//		}
	//	}
	//	mesh = std::move(tmpmesh);
	return FPolymesh();
}
