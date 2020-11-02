#include "WRTriggerManager.h"

#include "Trigger/WRTriggerSystem.h"
#include "Trigger/Helper/WRTriggerHelper.h"

#include "Kismet/KismetStringLibrary.h"
#include "WRPathManager.h"
#include <LocalTimestampDirectoryVisitor.h>
#include <Paths.h>
#include "Table/Base/WRTableManager.h"
#include "Table/WRMapTable.h"
#include "WRMapManager.h"


DEFINE_LOG_CATEGORY(WRTriggerLog)

void WRTriggerManager::OnInitialize()
{
	
}

void WRTriggerManager::OnShutdown()
{
	UsedTriggerIDs.Empty();
	RemoveAll();
}

bool WRTriggerManager::OnTick(float DeltaTime)
{
	// TriggerSystems 가 비어 있으면 return
	if (1 > TriggerSystems.Num())
		return false;

	for (auto It = TriggerSystems.CreateIterator(); It; ++It)
	{
		It.Value()->Tick(DeltaTime);
	}

#ifdef WITH_EDITOR
// 	if (IMConsoleManager::Get()->GetInt(EIMConsole::Debug_Draw_Trigger) != 0)
// 	{
// 		for (auto It = TriggerSystems.CreateIterator(); It; ++It)
// 		{
// 			It.Value()->DrawTriggerShape();
// 		}
// 	}
#endif // WITH_EDITOR

	return true;
}

void WRTriggerManager::RemoveAll()
{
	bStartTriggerRun = false;
	bStartElapsedTime = 0.f;

	for (auto It = TriggerSystems.CreateIterator(); It; ++It)
	{
		delete It.Value();
	}

	TriggerSystems.Empty();
	PlayStreamingPostTriggerIDs.Empty();
}

bool WRTriggerManager::FindTriggerFiles(const FString InFileName, OUT TArray<FString>& FoundFiles)
{
	UE_LOG(WRTriggerLog, Display, TEXT("Load Trigger Name : %s"), *(InFileName));
	FString Path = WRPathManager::Get()->GetPath(EWRPath::Data_Map).Replace(TEXT("/Game"), TEXT("Content"));
	FString FullPath = FPaths::ProjectDir() + Path + InFileName;

	
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.FindFiles(FoundFiles, *FullPath, TEXT(".trg"));

	if (FoundFiles.Num() == 0)
	{
		UE_LOG(WRTriggerLog, Display, TEXT("TriggerNot Found => Name : %s"), *(FullPath));
		return false;
	}
	return true;
}

bool WRTriggerManager::LoadTrigger(const FString& InFileName, bool bBuild)
{
	TArray<FString> FoundFiles;
	FindTriggerFiles(InFileName, FoundFiles);

	bool bLoaded = false;
	for (int i = 0; i < FoundFiles.Num(); ++i)
	{
		if(FoundFiles[i].Contains(TEXT("Server")) == true)
			continue;

		FString FileName = FPaths::GetBaseFilename(FoundFiles[i]);

		WRTriggerSystem* TriggerSystem = this->AddTriggerSystem(*FileName);
		if (TriggerSystem == nullptr)
		 	return false;
		 
		if (TriggerSystem->LoadFromJson(FoundFiles[i], bBuild) == false || TriggerSystem->IsValid() == false)
		{
			this->RemoveTriggerSystem(*FileName);
			continue;
		}
		else
			bLoaded = true;
		 
		if (FileName.Contains(TEXT("Client")) == true)
		 	TriggerSystem->SetClientTriggerSystem();
	}
	
	return bLoaded;
}


void WRTriggerManager::RemoveFollowerID(int32 nActorID)
{
	for (auto It = TriggerSystems.CreateIterator(); It; ++It)
	{
		It.Value()->RemoveFollowerID(nActorID);
	}
}

void WRTriggerManager::SetCollisionActor(class AWRCharacter* Actor)
{
	for (auto It = TriggerSystems.CreateIterator(); It; ++It)
	{
		It.Value()->SetCollisionActor(Actor);
	}
}

void WRTriggerManager::RemoveCollisionActor(class AWRCharacter* IN InActor)
{
	for (auto It = TriggerSystems.CreateIterator(); It; ++It)
	{
		It.Value()->RemoveCollisionActor(InActor);
	}
}

const TArray<class WRTriggerActionNodeBase*>* WRTriggerManager::GetSpawnActionNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetSpawnActionNodes();
	}
	return nullptr;
}

const TArray<class WRTriggerActionNodeBase*>* WRTriggerManager::GetSpawnActionNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
//
//	return GetSpawnActionNodes(Name);
	return nullptr;
}

const TArray<class WRTriggerNodeBase*>* WRTriggerManager::GetTriggerNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetTriggerNodes();
	}
	return nullptr;
}

const TArray<class WRTriggerNodeBase*>* WRTriggerManager::GetTriggerNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
// 
// 	return GetTriggerNodes(Name);

	return nullptr;
}

const TArray<class WRTriggerActionNodeBase*>* WRTriggerManager::GetActionNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetActionNodes();
	}
	return nullptr;
}

const TArray<class WRTriggerActionNodeBase*>* WRTriggerManager::GetActionNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
// 
// 	return GetActionNodes(Name);
	return nullptr;
}

const TArray<class WRTriggerTransitionNodeBase*>* WRTriggerManager::GetTransitionNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetTransitionNodes();
	}
	return nullptr;
}

const TArray<class WRTriggerTransitionNodeBase*>* WRTriggerManager::GetTransitionNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
// 
// 	return GetTransitionNodes(Name);
	return nullptr;
}

const TArray<class WRTriggerSpecialNodeBase*>* WRTriggerManager::GetSpecialNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetSpecialNodes();
	}
	return nullptr;
}

const TArray<class WRTriggerSpecialNodeBase*>* WRTriggerManager::GetSpecialNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
// 
// 	return GetSpecialNodes(Name);
	return nullptr;

}

const TMap<int32, class WRTriggerNode_MainAreaBox*>* WRTriggerManager::GetMainAreaTriggerNodes(const FString& InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return nullptr;

	WRTriggerSystem* TriggerSystem = TriggerSystems[InName];
	if (TriggerSystem)
	{
		return TriggerSystem->GetMainAreaTriggerNodes();
	}
	return nullptr;
}

const TMap<int32, class WRTriggerNode_MainAreaBox*>* WRTriggerManager::GetMainAreaTriggerNodes()
{
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name = FPaths::GetBaseFilename(MapRecord->TriggerDataJsonFileName);
// 
// 	return GetMainAreaTriggerNodes(Name);
	return nullptr;
}

void WRTriggerManager::OnResultTriggerBoxEvent(int32 TriggerID, uint32 TransitionType)
{
	WRTriggerBaseNode* BaseNode = FindTriggerNodeFromCurrentMapTrigger(TriggerID);
	if (BaseNode && EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == true)
	{
		WRTriggerNodeBase* TriggerNode = static_cast<WRTriggerNodeBase*>(BaseNode);
		if (TriggerNode)
		{
			if (TransitionType == EWRTriggerNodeType::Transition_Enter)
				TriggerNode->OnEnter();
			else if (TransitionType == EWRTriggerNodeType::Transition_Exit)
				TriggerNode->OnExit();
		}
	}
	
}

void WRTriggerManager::OnLoadLevelStart()
{
	UE_LOG(WRLog, Log, TEXT("WRTriggerManager::OnLoadLevelStart()"));
	RemoveAll();
}

void WRTriggerManager::OnLoadLevelComplete()
{

}

void WRTriggerManager::OnRenderStart()
{

}

void WRTriggerManager::OnResurrectionRevive()
{
	const TArray<class WRTriggerNodeBase*>* TriggerNodes = GetTriggerNodes();
	if (TriggerNodes)
	{
		for (int i = 0; i < TriggerNodes->Num(); ++i)
		{
			(*TriggerNodes)[i]->EnteringActorReset();
		}
	}
}

void WRTriggerManager::OnReconnect()
{
	const TArray<class WRTriggerNodeBase*>* TriggerNodes = GetTriggerNodes();
	if (TriggerNodes)
	{
		for (int i = 0; i < TriggerNodes->Num(); ++i)
		{
			(*TriggerNodes)[i]->EnteringActorResetCheckUsed();
		}
	}
}

WRTriggerBaseNode* WRTriggerManager::FindTriggerNodeFromCurrentMapTrigger(int32 TriggerID)
{
	WRTableID CurrentMapTid = WRMapManager::Get()->GetCurrentMapTID();
	FWRMapTableBase* MapTable = WRTableManager::Get()->FindRowInMapTable(CurrentMapTid);
	if (MapTable == nullptr)
		return nullptr;

	TArray<FString> FoundFiles;
	FindTriggerFiles(MapTable->LevelName.ToString(), FoundFiles);
	for (int i = 0; i < FoundFiles.Num(); ++i)
	{
		FString FileName = FPaths::GetBaseFilename(FoundFiles[i]);
		if (FileName.Contains(TEXT("Client")) == true)
		{
			if (TriggerSystems.Contains(FileName) == false)
				continue;

			return TriggerSystems[FileName]->GetNodeByUid(TriggerID);
		}
	}
	
	return nullptr;
}



void WRTriggerManager::SetUsedTriggerList(const std::list<TID_t>& UsedTriggerTids)
{
	for (auto iter = UsedTriggerTids.begin(); iter != UsedTriggerTids.end(); ++iter)
	{
		WRTriggerBaseNode* BaseNode = FindTriggerNodeFromCurrentMapTrigger(*iter);
		if (BaseNode && EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == true)
		{
			WRTriggerNodeBase* TriggerNode = static_cast<WRTriggerNodeBase*>(BaseNode);
			if (TriggerNode)
			{
				TriggerNode->OnDisable();
			}
		}
	}
}

class WRTriggerBaseNode* WRTriggerManager::FindSpawnActionFromCurrentMapTrigger(int32 TriggerID, EWRTriggerNodeType::en SpawnActionType)
{
// 	if (EWRTriggerNodeType::IsSpawnActionNode(SpawnActionType) == false)
// 		return nullptr;
// 
// 	IMMapBaseRecord* MapRecord = IMMapRecordManager::Get()->Find(IMMapManager::Get()->GetLoadedMapTID());
// 	if (MapRecord == nullptr)
// 		return nullptr;
// 
// 	FString Name;
// 
// 	switch (SpawnActionType)
// 	{
// 	case EWRTriggerNodeType::Action_NpcSpawn:
// 		Name = FPaths::GetBaseFilename(MapRecord->NpcSpawnDataFilePath4Json);
// 		break;
// 	case EWRTriggerNodeType::Action_ENpcSpawn:
// 		Name = FPaths::GetBaseFilename(MapRecord->ENpcSpawnDataFilePath4Json);
// 		break;
// 	case EWRTriggerNodeType::Action_PortalSpawn:
// 		Name = FPaths::GetBaseFilename(MapRecord->PortalSpawnDataFilePath4Json);
// 		break;
// 	case EWRTriggerNodeType::Action_ObjectSpawn:
// 		Name = FPaths::GetBaseFilename(MapRecord->ObjectSpawnDataFilePath4Json);
// 		break;
// 	case EWRTriggerNodeType::Action_IOSpawn:
// 		Name = FPaths::GetBaseFilename(MapRecord->IOSpawnDataFilePath4Json);
// 		break;
// // 	case EWRTriggerNodeType::Action_SOSpawn:
// // 		Name = FPaths::GetBaseFilename(MapRecord->);
// // 		break;
//  	case EWRTriggerNodeType::Action_AOSpawn:
//  		Name = FPaths::GetBaseFilename(MapRecord->AOSpawnDataFilePath4Json);
//  		break;
// 	}
// 
// 	if (TriggerSystems.Contains(Name) == false)
// 		return nullptr;
// 
// 	WRTriggerBaseNode* BaseNode = TriggerSystems[Name]->GetNodeByUid(TriggerID);
// 	//  	if (EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == false)
// 	//  		return nullptr;
// 
// 	return /*static_cast<WRTriggerNodeBase*>*/(BaseNode);
	return nullptr;
}

void WRTriggerManager::RunActiveTrigger(int32 TriggerID)
{
	WRTriggerBaseNode* BaseNode = FindTriggerNodeFromCurrentMapTrigger(TriggerID);
	if (BaseNode && EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == true)
	{
		WRTriggerNodeBase* TriggerNode = static_cast<WRTriggerNodeBase*>(BaseNode);
		if (TriggerNode)
		{
			TriggerNode->OnActive();
		}
	}
}

void WRTriggerManager::RunDisableTrigger(int32 TriggerID)
{
	WRTriggerBaseNode* BaseNode = FindTriggerNodeFromCurrentMapTrigger(TriggerID);
	if (BaseNode && EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == true)
	{
		WRTriggerNodeBase* TriggerNode = static_cast<WRTriggerNodeBase*>(BaseNode);
		if (TriggerNode)
		{
			TriggerNode->OnDisable();
		}
	}
	
}

class WRTriggerSystem* WRTriggerManager::AddTriggerSystem(const TCHAR* InName /*= nullptr*/)
{
	if (TriggerSystems.Contains(InName) == true)
		return nullptr;

	WRTriggerSystem* TriggerSystem = new WRTriggerSystem();

	if (InName != nullptr)
		TriggerSystem->SetName(InName);

	TriggerSystems.Add(InName, TriggerSystem);
	return TriggerSystem;
}

void WRTriggerManager::RemoveTriggerSystem(const TCHAR* InName)
{
	if (TriggerSystems.Contains(InName) == false)
		return;

	WRTriggerSystem* FoundTriggerSystem = TriggerSystems[InName];
	if (FoundTriggerSystem != nullptr)
		delete FoundTriggerSystem;

	TriggerSystems.Remove(InName);
}

void WRTriggerManager::LoadUsedTriggerData()
{
// 	UsedTriggerIDs.Empty();
// 	FString LoadString;
// 	FString FileName = TEXT("Trigger_") + WRCharacterManager::Get()->GetMyCharacterName() + TEXT(".dat");
// 
// 
// #if PLATFORM_ANDROID
// 	bool bLoad = FFileHelper::LoadFileToString(LoadString, *(IMUtility::GetExternPath() + FileName));
// #else 
// 	bool bLoad = FFileHelper::LoadFileToString(LoadString, *(FPaths::GameUserDir() + FileName));
// #endif //PLATFORM_ANDROID
// 	
// 
// 	if (bLoad == false)
// 	{
// 		return;
// 	}
// 	
// 	TArray<FString> LoadStrings = IMStringUtil::Split(LoadString, '\n');
// 
// 	for (int i = 0; i < LoadStrings.Num(); ++i)
// 	{
// 		TArray<FString> LineString = IMStringUtil::Split(LoadStrings[i], ':');
// 		if (LineString.Num() >= 2)
// 		{
// 			TArray<FString> TidsString = IMStringUtil::Split(LineString[1], ',');
// 			TArray<int32> Tids;
// 			for (int j = 0; j < TidsString.Num(); ++j)
// 			{
// 				Tids.Add(UKismetStringLibrary::Conv_StringToInt(TidsString[j]));
// 			}
// 			UsedTriggerIDs.Add((int32)UKismetStringLibrary::Conv_StringToInt(LineString[0]), Tids);
// 		}
// 	}
}

void WRTriggerManager::SaveUsedTriggerData(int32 TriggerSaveNum)
{
// 	int32 MapTid = IMMapManager::Get()->GetLoadedMapTID();
// 	if (UsedTriggerIDs.Contains(MapTid) == true)
// 	{
// 		UsedTriggerIDs[MapTid].Add(TriggerSaveNum);
// 	}
// 	else
// 	{
// 		TArray<int32> Tids;
// 		Tids.Add(TriggerSaveNum);
// 		UsedTriggerIDs.Add(MapTid, Tids);
// 	}
// 
// 	FString SaveString;
// 	for (auto iter = UsedTriggerIDs.CreateIterator(); iter; ++iter)
// 	{
// 		SaveString.Append(FString::Printf(TEXT("%d:"), iter.Key()));
// 		TArray<int32>& Tids = iter.Value();
// 		for (int i = 0; i < Tids.Num(); ++i)
// 		{
// 			if (i != 0)
// 			{
// 				SaveString.Append(",");
// 			}
// 			SaveString.Append(FString::Printf(TEXT("%d"), Tids[i]));
// 		}
// 		SaveString.Append("\n");
// 	}
// 	
// 	FString FileName = TEXT("Trigger_") + WRCharacterManager::Get()->GetMyCharacterName() + TEXT(".dat");
// #if PLATFORM_ANDROID
// 	FFileHelper::SaveStringToFile(SaveString, *(IMUtility::GetExternPath() + FileName));
// #else 
// 	FFileHelper::SaveStringToFile(SaveString, *(FPaths::GameUserDir() + FileName));
// #endif //PLATFORM_ANDROID
}

bool WRTriggerManager::IsUsedTriggerData(int32 TriggerSaveNum)
{
// 	int32 MapTid = IMMapManager::Get()->GetLoadedMapTID();
// 	if (UsedTriggerIDs.Contains(MapTid) == true)
// 	{
// 		for (int i = 0; i < UsedTriggerIDs[MapTid].Num(); ++i)
// 		{
// 			if (UsedTriggerIDs[MapTid][i] == TriggerSaveNum)
// 				return true;
// 		}
// 	}
	return false;
}

void WRTriggerManager::OnPlayStreamingMovieFinished()
{
	for (int i = 0; i < PlayStreamingPostTriggerIDs.Num(); ++i)
	{
		RunActiveTrigger(PlayStreamingPostTriggerIDs[i]);
	}
	PlayStreamingPostTriggerIDs.Empty();
}

bool WRTriggerManager::CheckCondition(WRTriggerNodeBase* TriggerNode)
{
// 	EWRTriggerConditionType::en ConditionType = TriggerNode->GetConditionType();
// 	int32 ConditionValue = TriggerNode->GetConditionValue();
// 	switch (ConditionType)
// 	{
// 	case EWRTriggerConditionType::None:
// 		return true;
// 	case EWRTriggerConditionType::CharacterClass:
// 	{
// 		AWRCharacter* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
// 		if (MyCharacter->IsValidLowLevel())
// 		{
// 			const IMCharacterRecord* MyCharRecord = MyCharacter->GetCharacterRecord();
// 			if (MyCharRecord)
// 			{
// 				if (MyCharRecord->ClassType == ConditionValue)
// 				{
// 					return true;
// 				}
// 				else
// 				{
// 					const IMCharacterRecord* CharRecord = static_cast<const IMCharacterRecord*>(IMCharacterRecordManager::Get()->Find(ConditionValue));
// 					FString ClassName = TEXT("");
// 					if (CharRecord)
// 						ClassName = GetLocalStringByFString(IMUtility::FNameToFString(CharRecord->LocalKeyName), EIMLocalStringNameSpace::Character);
// 
// 					FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_class"), EIMLocalStringNameSpace::UI_Word);
// 					Content.ReplaceInline(TEXT("{class}"), *ClassName);
// 
// 					ShowCommonPopupMessage(FText::FromString(Content));
// 					return false;
// 				}
// 			}
// 		}
// 		return false;
// 	}
// 	case EWRTriggerConditionType::Realm:
// 	{
// 		EIMRealmType::en RealmType = WRCharacterManager::Get()->GetMyCharacterRealmType();
// 		if ((int)RealmType == ConditionValue)
// 			return true;
// 
// 		if (ConditionValue == 4 && (RealmType == EIMRealmType::Ignis || RealmType == EIMRealmType::Airless))
// 			return true;
// 
// 		const IMRealmRecord* RealmRecord = IMRealmRecordManager::Get()->Find(ConditionValue);
// 		FString RealmName = TEXT("");
// 		if (RealmRecord)
// 			RealmName = GetLocalStringByFString(IMUtility::FNameToFString(RealmRecord->NameLocalKey), EIMLocalStringNameSpace::Realm);
// 
// 		FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_realm"), EIMLocalStringNameSpace::UI_Word);
// 		Content.ReplaceInline(TEXT("{realm}"), *RealmName);
// 
// 		ShowCommonPopupMessage(FText::FromString(Content));
// 		return false;
// 	}
// 		break;
// 	case EWRTriggerConditionType::Guild:
// 	{
// 		if ((int)IMGuildManager::Get()->HasGuild() == ConditionValue)
// 		{
// 			return true;
// 		}
// 
// 		FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_guild"), EIMLocalStringNameSpace::UI_Word);
// 		ShowCommonPopupMessage(FText::FromString(Content));
// 		return false;
// 	}
// 		break;
// 	case EWRTriggerConditionType::Quest:
// 	{
// 		if (IMQuestManager::Get()->IsQuestCompleted(ConditionValue) == true)
// 		{
// 			return true;
// 		}
// 
// 		const IMQuestRecord* QuestRecord = IMQuestRecordManager::Get()->Find(ConditionValue);
// 		FString QuestName = TEXT("");
// 		if (QuestRecord)
// 			QuestName = GetLocalStringByFString(IMUtility::FNameToFString(QuestRecord->LocalKeyTitle), EIMLocalStringNameSpace::Quest_UI);
// 
// 		FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_quest"), EIMLocalStringNameSpace::UI_Word);
// 		Content.ReplaceInline(TEXT("{quest_name}"), *QuestName);
// 		ShowCommonPopupMessage(FText::FromString(Content));
// 		return false;
// 	}
// 		break;
// 	case EWRTriggerConditionType::Level:
// 	{
// 		int MyLevel = WRCharacterManager::Get()->GetMyCharacterAwakenLevel() * 50 + WRCharacterManager::Get()->GetMyCharacterLevel();
// 		if (MyLevel >= ConditionValue)
// 		{
// 			return true;
// 		}
// 
// 		int ConditionAwakenLevel = ConditionValue / 50;
// 		int ConditionLevel = ConditionValue % 50;
// 		if (ConditionAwakenLevel > 0)
// 		{
// 			FString AwakenLevel = FString::FormatAsNumber(ConditionAwakenLevel);
// 			FString Level = FString::FormatAsNumber(ConditionLevel);
// 			FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_awakenlevel"), EIMLocalStringNameSpace::UI_Word);
// 			Content.ReplaceInline(TEXT("{awaken_level}"), *AwakenLevel);
// 			Content.ReplaceInline(TEXT("{level}"), *Level);
// 			ShowCommonPopupMessage(FText::FromString(Content));
// 			
// 		}
// 		else
// 		{
// 			FString Level = FString::FormatAsNumber(ConditionLevel);
// 			FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_level"), EIMLocalStringNameSpace::UI_Word);
// 			Content.ReplaceInline(TEXT("{level}"), *Level);
// 			ShowCommonPopupMessage(FText::FromString(Content));
// 		}
// 		return false;
// 	}
// 		break;
// 	case EWRTriggerConditionType::Item:
// 	{
// 		if (IMInventoryManager::Get()->GetItemCountByRecordID(ConditionValue) != 0)
// 		{
// 			return true;
// 		}
// 
// 		const IMItemBaseRecord* ItemRecord = IMItemRecordManager::Get()->Find(ConditionValue);
// 		FString ItemName = TEXT("");
// 
// 		if (ItemRecord)
// 			ItemName = GetLocalStringByFString(IMUtility::FNameToFString(ItemRecord->LocalKeyName), EIMLocalStringNameSpace::Item);
// 
// 		FString Content = GetLocalStringByFString(TEXT("ui_sysmsg_content_locked_item"), EIMLocalStringNameSpace::UI_Word);
// 		Content.ReplaceInline(TEXT("{item}"), *ItemName);
// 		ShowCommonPopupMessage(FText::FromString(Content));
// 		return false;
// 	}
// 		break;
// 	
// 	default:
// 		return false;
// 	}
	return true;
}

