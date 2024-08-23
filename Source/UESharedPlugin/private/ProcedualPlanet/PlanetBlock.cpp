// Fill out your copyright notice in the Description page of Project Settings.
#include "ProcedualPlanet\PlanetBlock.h"
static FName s_procedualMeshComponentName1(TEXT("ProcedualMeshComponent"));
static FName s_sceneComponentName1(TEXT("SceneComponent"));
APlanetBlock::APlanetBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent= m_rootComponent =CreateDefaultSubobject<USceneComponent>(s_sceneComponentName1);

	m_procedualMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(s_procedualMeshComponentName1);
	m_procedualMeshComponent->SetupAttachment(RootComponent);


	


}

// Called when the game starts or when spawned
void APlanetBlock::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlanetBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlanetBlock::Show()
{
	AProcedualPlanet* owner = Cast< AProcedualPlanet>(GetOwner());
	FVector scale = owner->GetActorScale();
	FVector location = owner->GetActorLocation();
	FRotator rotation = owner->GetActorRotation();
	auto& mesh = owner->GetMeshData();


	std::map<VertexHandle, int32> vertexindex;
	std::map<FaceHandle, int32> facevertexindex;
	TArray<FVector> vertexarr;


	TArray<int32> triarr;
	TArray<FVector> nolmalarr;

	TArray<VertexHandle> facevertexhandles = m_fh.vertices();
	auto centroid = mesh.calc_centroid(m_fh);
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
	for (auto& point : vertexarr) {
		point *= scale;
		//colarr.Add(FLinearColor(FMath::FRand(), FMath::FRand(), FMath::FRand()));
	}
	if (vertexarr.Num() == 5) {
		bHexagon = false;
		uv = &uv5;
	}
	else {
		bHexagon = true;
	}
	SetActorLocation(location + centroid * scale);



	m_procedualMeshComponent->CreateMeshSection_LinearColor(0, vertexarr, triarr, nolmalarr, *uv, colarr, TArray<FProcMeshTangent>(), true);
	if (owner->GetBiomeMaterial(bHexagon).IsValid()) {
		m_biomeMaterialDynamic = m_procedualMeshComponent->CreateDynamicMaterialInstance(0, owner->GetBiomeMaterial(bHexagon).LoadSynchronous());

		switch (Biome) {
		case BiomeType::Plains:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(0, 1, 0.04));
			break;
		case BiomeType::Forest:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(0, 0.31, 0.013));
			break;
		case BiomeType::Desert:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(0.6, 0.6, 0.13));
			break;
		case BiomeType::Ice_Plains:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(1, 1, 1));
			break;
		case BiomeType::Mountains:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(0.6, 0.6, 0.6));
			break;
		default:
			m_biomeMaterialDynamic->SetVectorParameterValue("Color", FLinearColor(0, 0, 0));
		}
	}
}
	

