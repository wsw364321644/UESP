// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "ProcedualPlanet.h"
#include "PlanetBlock.generated.h"

UENUM()
enum class BiomeType
{
	Ocean,
	Plains,
	Desert,
	Mountains,
	Forest,
	Taiga,
	Swamp,
	River,
	Frozen_Ocean,
	Frozen_River,
	Ice_Plains
};

UCLASS()
class UESHAREDPLUGIN_API APlanetBlock : public AActor
{
	GENERATED_BODY()
		friend class ProcedualPlanet;
public:	
	// Sets default values for this actor's properties
	APlanetBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetFaceHandle(FaceHandle fh) { m_fh = fh; };

	void Show();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet, meta = (DisplayName = "BiomeType"))
		BiomeType Biome;
private:

	UPROPERTY(Category = Planet, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* m_rootComponent;
	UPROPERTY(Category = Planet, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UProceduralMeshComponent* m_procedualMeshComponent;

	FaceHandle m_fh;

	bool bHexagon;

	UPROPERTY()
		UMaterialInstanceDynamic* m_biomeMaterialDynamic;
};
