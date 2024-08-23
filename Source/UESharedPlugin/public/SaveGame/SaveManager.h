#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SaveManager.generated.h"

struct FSuervivalSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSuervivalSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};
USTRUCT(BlueprintType)
struct FActorRecord
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(BlueprintReadWrite)
		UClass* ActorClass;
	//TSubclassOf<>
	UPROPERTY(BlueprintReadWrite)
		FTransform ActorTransform;
	UPROPERTY(BlueprintReadWrite)
		FName ActorName;
	UPROPERTY(BlueprintReadWrite)
		bool IsNetStartupActor;
	UPROPERTY(BlueprintReadWrite)
		TArray< uint8 >  ActorData;

};

UCLASS()
class UActorSaveGame : public USaveGame
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FName> DestroyedActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<FName,FActorRecord> ActorRecords;

};


USTRUCT(BlueprintType)
struct FSaveInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite)
		FDateTime SaveData;

	UPROPERTY(BlueprintReadWrite)
		FString SaveName;

};


UCLASS()
class USaveGameInfo : public USaveGame
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FSaveInfo> SaveInfos;

};




class UESHAREDPLUGIN_API FSavemanager
{
public:
	FSavemanager();
	~FSavemanager();
	void AddActorToSave(UActorSaveGame* SaveGame,AActor* Actor);
	void AddDeletedActorToSave(UActorSaveGame* SaveGame,AActor* Actor);
	void RemoveActorFromSave(UActorSaveGame* SaveGame, AActor* Actor);
	void InitActorFromRecord(FActorRecord& Record, AActor* Actor);

	bool SaveToFile(FString SaveName, int32 userID=0);
	UActorSaveGame* LoadSave(FString SaveName, int32 userID=0);
	void RemoveSave(FString SaveName, int32 userID = 0);
	UActorSaveGame* GetSave(FString SaveName, int32 userID = 0);
private:
	UActorSaveGame* GetOrAddSave(FString SaveName, int32 userID = 0);


	TMap<FString, UActorSaveGame*> ActorSaveGames;
};