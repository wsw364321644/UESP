// Fill out your copyright notice in the Description page of Project Settings.
#include "ProcedualPlanet\ProcedualPlanet.h"
#include "UESharedPlugin.h"
#include "ProcedualPlanet\PlanetBlock.h"
#include <map>
static FName s_procedualMeshComponentName(TEXT("ProcedualMeshComponent"));
static FName s_sceneComponentName(TEXT("SceneComponent"));
// Sets default values
AProcedualPlanet::AProcedualPlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent= m_rootComponent =CreateDefaultSubobject<USceneComponent>(s_sceneComponentName);

	m_procedualMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(s_procedualMeshComponentName);
	m_procedualMeshComponent->SetupAttachment(RootComponent);


	


}

// Called when the game starts or when spawned
void AProcedualPlanet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProcedualPlanet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	m_mesh = FUESharedPlugin::Get()->GetGeometrySystem()->GetIcosahedronMuiltFour(true,m_level);

	TArray<FaceHandle> facehandles = m_mesh.all_faces();
	for (auto fh : facehandles) {
		FActorSpawnParameters  params;
		params.Owner = this;
		APlanetBlock* block = GetWorld()->SpawnActor<APlanetBlock>(APlanetBlock::StaticClass(), params);
		m_blocks[fh] = block;
		block->SetFaceHandle(fh);


		FVector centroid = m_mesh.calc_centroid(fh);
		centroid.Normalize();
		auto angle = FMath::Acos(FMath::Abs(FVector::DotProduct(centroid, FVector(0, 0, 1))));
		float distribution = FMath::SRand();
		float dividePi = PI / 2;
		if (angle > dividePi * 0.6) {
			if (distribution > 0.4) {
				block->Biome = BiomeType::Forest;
			}
			else {
				block->Biome = BiomeType::Plains;
			}
		}
		else if (angle > dividePi * 0.3) {
			if (distribution > 0.9) {
				block->Biome = BiomeType::Desert;
			}
			else if (distribution > 0.1) {
				block->Biome = BiomeType::Plains;
			}
			else {
				block->Biome = BiomeType::Mountains;
			}
		}
		else if (angle > dividePi * 0.15) {
			if (distribution > 0.5) {
				block->Biome = BiomeType::Plains;
			}
			else {
				block->Biome = BiomeType::Ice_Plains;
			}

		}
		else {
			block->Biome = BiomeType::Ice_Plains;
		}

		block->Show();
	}

	if (m_showindex >= 00 && m_showindex < facehandles.Num()) {
		auto fh = facehandles[m_showindex];
		FVector scale = GetActorScale();
		FVector location = GetActorLocation();
		FRotator rotation = GetActorRotation();


		std::map<VertexHandle, int32> vertexindex;
		std::map<FaceHandle, int32> facevertexindex;
		TArray<FVector> vertexarr;


		TArray<int32> triarr;
		TArray<FVector> nolmalarr;

		TArray<VertexHandle> facevertexhandles = fh.vertices();
		auto centroid = m_mesh.calc_centroid(fh);
		for (auto vh : facevertexhandles) {
			auto vdata = vh.GetData();
			vertexindex[vh] = vertexarr.Num();
			vertexarr.Add(vdata.Point - centroid);
		}

		for (int i = 0; i < facevertexhandles.Num() - 2; i++) {
			triarr.Add(vertexindex[facevertexhandles[0]]);
			triarr.Add(vertexindex[facevertexhandles[i + 1]]);
			triarr.Add(vertexindex[facevertexhandles[i + 2]]);
		}
		TArray<FLinearColor> colarr;

		TArray < FVector2D > uv5 = { {0.6,0 } ,{1,0.5} ,{0.7,1}  ,{0,0.75}  ,{0,0.25} };
		TArray < FVector2D > uv6 = { {0.5,0 } ,{1,0.25} ,{1,0.75}  ,{0.5,1} ,{0,0.75}  ,{0,0.25} };
		TArray < FVector2D >* uv = &uv6;

		FVector n = FVector::CrossProduct(vertexarr[1], vertexarr[0]);
		//FVector n = FVector::CrossProduct(vertexarr[0] - vertexarr[1], vertexarr[0] - vertexarr[2]);
		n.Normalize();


		auto rotationAxis = FVector::CrossProduct(n, { 0,0,1 });
		auto rotationCos = FVector::DotProduct(n, { 0,0,1 });
		auto rotationSin = rotationAxis.Size();


		auto RotationMatrix = [](FVector u, float rotationSin, float rotationCos)->FMatrix
		{
			u.Normalize();
			FMatrix rotatinMatrix;
			rotatinMatrix.SetIdentity();
			rotatinMatrix.M[0][0] = rotationCos + u[0] * u[0] * (1 - rotationCos);
			rotatinMatrix.M[0][1] = u[0] * u[1] * (1 - rotationCos) - u[2] * rotationSin;
			rotatinMatrix.M[0][2] = u[1] * rotationSin + u[0] * u[2] * (1 - rotationCos);

			rotatinMatrix.M[1][0] = u[2] * rotationSin + u[0] * u[1] * (1 - rotationCos);
			rotatinMatrix.M[1][1] = rotationCos + u[1] * u[1] * (1 - rotationCos);
			rotatinMatrix.M[1][2] = -u[0] * rotationSin + u[1] * u[2] * (1 - rotationCos);

			rotatinMatrix.M[2][0] = -u[1] * rotationSin + u[0] * u[2] * (1 - rotationCos);
			rotatinMatrix.M[2][1] = u[0] * rotationSin + u[1] * u[2] * (1 - rotationCos);
			rotatinMatrix.M[2][2] = rotationCos + u[2] * u[2] * (1 - rotationCos);


			return rotatinMatrix.GetTransposed();
		};
		FMatrix rotationMatrix = RotationMatrix(rotationAxis, rotationSin, rotationCos);

		//rotationMatrix=rotationMatrix.GetTransposed();
		//rotationMatrix = rotationMatrix.InverseFast();

		for (auto& point : vertexarr) {

			point = rotationMatrix.TransformVector(point);
			point.Z += 1;


			//colarr.Add(FLinearColor(FMath::FRand(), FMath::FRand(), FMath::FRand()));
		}



		bool bHexagon;
		if (vertexarr.Num() == 5) {
			bHexagon = false;
			uv = &uv5;
		}
		else {
			bHexagon = true;
		}

		//auto r = FVector{ 1,0,0 }.Rotation()-FVector{ 0, 0, 1 }.Rotation() - n.Rotation();
		//m_procedualMeshComponent->AddLocalRotation(r);
		m_procedualMeshComponent->AddWorldOffset({ 0,0, m_showHeight });


		m_procedualMeshComponent->CreateMeshSection_LinearColor(0, vertexarr, triarr, nolmalarr, *uv, colarr, TArray<FProcMeshTangent>(), true);
		m_procedualMeshComponent->SetMaterial(0, GetBiomeMaterial(bHexagon).LoadSynchronous());
	}





	//TArray<VertexHandle> vertexhandles = m_mesh.all_vertices();
	//std::map<VertexHandle, int32> vertexindex;
	//std::map<FaceHandle, int32> facevertexindex;
	//TArray<FVector> vertexarr;
	//for (auto vh : vertexhandles) {
	//	auto vdata = vh.GetData();
	//	vertexindex[vh]=vertexarr.Num();
	//	vertexarr.Add(vdata.Point);
	//}

	//TArray<int32> triarr;
	//TArray<FVector> nolmalarr;
	//for (auto fh : facehandles) {
	//	TArray<VertexHandle> facevertexhandles = fh.vertices();

	//	for (int i = 0; i < facevertexhandles.Num() - 2; i++) {
	//		triarr.Add(vertexindex[facevertexhandles[0]]);
	//		triarr.Add(vertexindex[facevertexhandles[i + 1]]);
	//		triarr.Add(vertexindex[facevertexhandles[i + 2]]);
	//	}
	//	//TArray < FVector> parr{ facevertexhandles[0].GetData().Point,facevertexhandles[1].GetData().Point,facevertexhandles[2].GetData().Point };
	//	//auto n = FVector::CrossProduct((parr[1]-parr[0] ),(parr[2]-parr[0] ));
	//	//n.Normalize();
	//	//if (FVector::DotProduct(n, parr[0]) < 0) {
	//	//	n = -n;
	//	//}
	//	//nolmalarr.Add(n);
	//}
	//TArray<FLinearColor> colarr;
	//for (auto& point : vertexarr) {
	//	auto scale = GetActorScale();
	//	auto rotation = GetActorRotation();
	//	auto location = GetActorLocation();
	//	point *= scale;
	//	point += location;
	//	//colarr.Add(FLinearColor(FMath::FRand(), FMath::FRand(), FMath::FRand()));
	//}



	//m_procedualMeshComponent->CreateMeshSection_LinearColor(0, vertexarr, triarr, nolmalarr, TArray<FVector2D>(), colarr, TArray<FProcMeshTangent>(), true);
	//m_procedualMeshComponent->SetMaterial(0,m_material.LoadSynchronous());

}

// Called every frame
void AProcedualPlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

