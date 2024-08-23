#include "SaveGame\SaveManager.h"
#include "Kismet/GameplayStatics.h"

FSavemanager::FSavemanager()
{
	//UObjectBaseUtility::AddToRoot(ActorSaveGames);

}
FSavemanager::~FSavemanager()
{
	for (auto ActorSaveGame : ActorSaveGames) {
		ActorSaveGame.Value->RemoveFromRoot();
	}
}
void FSavemanager::AddActorToSave(UActorSaveGame* SaveGame, AActor* Actor)
{
	if (!SaveGame) { return; }
	FActorRecord ActorRecord;
	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSuervivalSaveGameArchive Archive(MemoryWriter);

	Actor->Serialize(Archive);
	ActorRecord.ActorName = Actor->GetFName();
	ActorRecord.ActorTransform = Actor->GetActorTransform();
	ActorRecord.ActorClass = Actor->GetClass();
	ActorRecord.IsNetStartupActor = Actor->IsNetStartupActor();
	SaveGame->ActorRecords.FindOrAdd(ActorRecord.ActorName)= ActorRecord;
}

void FSavemanager::AddDeletedActorToSave(UActorSaveGame* SaveGame, AActor* Actor)
{
	if (!SaveGame) { return; }
	SaveGame->DestroyedActors.Add(Actor->GetFName());

}

void FSavemanager::RemoveActorFromSave(UActorSaveGame* SaveGame, AActor* Actor)
{
	if (!SaveGame) { return; }
	SaveGame->ActorRecords.Remove(Actor->GetFName());
}

void FSavemanager::InitActorFromRecord(FActorRecord& ActorRecord, AActor* Actor)
{

	FMemoryReader MemoryReader(ActorRecord.ActorData, true);
	FSuervivalSaveGameArchive Archive(MemoryReader);
	Actor->Serialize(Archive);
	Actor->SetActorTransform(ActorRecord.ActorTransform);
}

bool FSavemanager::SaveToFile(FString SaveName,int32 userID)
{
	auto savedata=ActorSaveGames.Find(SaveName);
	if (savedata) {
		return UGameplayStatics::SaveGameToSlot(*savedata, SaveName, 0);
	}
	return false;
}

UActorSaveGame* FSavemanager::LoadSave(FString SaveName, int32 userID)
{
    bool doesItExist = UGameplayStatics::DoesSaveGameExist(SaveName, userID); // 0 = user index


    // If it exists, load from the slot.
    if (doesItExist)
    {
		auto savegame = UGameplayStatics::LoadGameFromSlot(SaveName, userID);
		savegame->AddToRoot();
		RemoveSave(SaveName);
        ActorSaveGames.FindOrAdd (SaveName)=Cast<UActorSaveGame>(savegame);
		return  Cast<UActorSaveGame>(savegame);
    }

    return nullptr;
	
}

void FSavemanager::RemoveSave(FString SaveName, int32 userID )
{
	if (ActorSaveGames.Find(SaveName)) {
		(*ActorSaveGames.Find(SaveName))->RemoveFromRoot();
		ActorSaveGames.Remove(SaveName);
	}
}

UActorSaveGame* FSavemanager::GetSave(FString SaveName, int32 userID)
{
	if (ActorSaveGames.Find(SaveName)) {
		return *ActorSaveGames.Find(SaveName);
	}
	return nullptr;
}

UActorSaveGame* FSavemanager::GetOrAddSave(FString SaveName, int32 userID )
{
	if (!ActorSaveGames.Find(SaveName)) {
		ActorSaveGames.FindOrAdd(SaveName) = NewObject<UActorSaveGame>();
		(*ActorSaveGames.Find(SaveName))->AddToRoot();
	}

	return *ActorSaveGames.Find(SaveName);
}


