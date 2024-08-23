// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "GeometrySystem.h"

#include <map>
#include "ProcedualPlanet.generated.h"

class APlanetBlock;
UCLASS()
class UESHAREDPLUGIN_API AProcedualPlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcedualPlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void  PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FPolymesh& GetMeshData() { return m_mesh; }

	TAssetPtr<UMaterial> GetBiomeMaterial(bool isHexagon=true) { if (isHexagon) return m_materialHexagon; else return m_materialPentagon; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet,meta = (DisplayName = "level"))
		int32 m_level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet, meta = (DisplayName = "Pentagon material"))
		TAssetPtr<UMaterial> m_materialPentagon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet, meta = (DisplayName = "Hexagon material"))
		TAssetPtr<UMaterial> m_materialHexagon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet, meta = (DisplayName = "show index"))
		int32 m_showindex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet, meta = (DisplayName = "show height"))
		float m_showHeight;
private:
	

	UPROPERTY(Category = Planet, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* m_procedualMeshComponent;

	UPROPERTY(Category = Planet, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* m_rootComponent;

	FPolymesh m_mesh;


	std::map<FaceHandle, APlanetBlock*> m_blocks;



};
