//====================================================================================
#include "WRTableManager.h"

#include <Engine/StreamableManager.h>
#include <Engine/Engine.h>
#include <Engine/AssetManager.h>

#include "Manager/WRCharacterManager.h"
#include "Manager/WRAnimManager.h"

#include "../WRSkillTable.h"
#include "../WRMontageTable.h"
#include "../WRWeaponTable.h"
#include "../WREquipmentLookTable.h"
#include "../WRFootstepsTable.h"
#include "../WREnpcTable.h"
#include "../WRObjectTable.h"
#include "../WRForceFeedbackTable.h"
#include "../WRMapTable.h"
#include "../WRDynamicMovementTable.h"
#include "../WRQuestTable.h"
#include "../WRCoordinateTable.h"
#include "../WRFoliageTable.h"
#include "../WRItemTable.h"
#include "../WRDialogueTable.h"
#include "../WRTutorialTable.h"
#include "../WRMovieTable.h"
#include "../WRVoiceRecognitionTable.h"
#include "../WRSkillEffectParamTable.h"
#include "../WRSocialTable.h"
#include "../WRFootPrintsTable.h"
#include "../WRCharacterPresetTable.h"
#include "../WRFactionTable.h"
#include "../WRTransportTable.h"
#include "../WRPlayerTable.h"
#include "../WRHitPartialCollisionTable.h"
#include "../WRCustomTextTable.h"
#include "../WREffectPathTable.h"
#include "../WREnvObjectTable.h"
#include "../WRCurrencyTable.h"
#include "../WRRewardTable.h"
#include "../WRCustomizingCategoryTable.h"

#include "../../Network/Share/Define/Define_BluePrint.h"
#include "../WRVehicleTable.h"
#include "../WRMicTable.h"
#include "../WRVideoChatTable.h"

	
// [ 2019-7-12 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRTableManager::OnInitialize()
{
	TablesToStream.Empty();

	//AddToTablePath<FWRSkillTable>();
	AddToTablePath<FWRPlayerSkillTable>();
	AddToTablePath<FWREnpcSkillTable>();

	AddToTablePath<FWRMontageTable>();
	AddToTablePath<FWRWeaponTable>();
	AddToTablePath<FWREquipmentLookTable>();
	AddToTablePath<FWRFootstepsTable>();
	AddToTablePath<FWRNpcTable>();
	AddToTablePath<FWREnpcTable>();
	AddToTablePath<FWRInteractionObjectTable>();
	AddToTablePath<FWRActionPropTable>();
	AddToTablePath<FWRBrickTable>();
	AddToTablePath<FWRForceFeedbackTable>();
	AddToTablePath<FWRMapLapTable>(); 
	AddToTablePath<FWRMapFieldTable>();
	AddToTablePath<FWRMapScenarioTable>();
	AddToTablePath<FWRMapStageTable>();
	AddToTablePath<FWRMapTownTable>();
	AddToTablePath<FWRMapBattleFieldTable>();
	AddToTablePath<FWRDynamicMovementTable>();
	AddToTablePath<FWRMainQuestTable>();
	AddToTablePath<FWRQuestMoveTaskTable>();
	AddToTablePath<FWRQuestTriggerTaskTable>();
	AddToTablePath<FWRQuestTutorialTaskTable>();
	AddToTablePath<FWRQuestActiveIOTaskTable>();
	AddToTablePath<FWRQuestActiveGroupIOTaskTable>();
	AddToTablePath<FWRCoordinateTable>();
	AddToTablePath<FWRFoliageTable>();
	AddToTablePath<FWRWeaponItemTable>();
	AddToTablePath<FWRArmorItemTable>();
	AddToTablePath<FWRTemporaryItemTable>();
	AddToTablePath<FWRCostumeItemTable>();
	AddToTablePath<FWRMaterialItemTable>();
	AddToTablePath<FWRInteriorItemTable>();
	AddToTablePath<FWRVehicleItemTable>();
	AddToTablePath<FWRMainQuestDialogueTable>();
	AddToTablePath<FWRNpcEventTalkDialogueTable>();
	AddToTablePath<FWRFeedbackTalkDialogueTable>();
	AddToTablePath<FWRTriggerNpcTalkDialogueTable>();
	AddToTablePath<FWRNpcIdleSpeechDialogueTable>();
	AddToTablePath<FWRNpcTalkDialogueTable>();
	AddToTablePath<FWRDialogueSetTable>();
	AddToTablePath<FWRDialogueSetGroupTable>();
	AddToTablePath<FWRTutorialTable>();
	AddToTablePath<FWRMovieTable>();
	AddToTablePath<FWRVoiceCommandData>();
	AddToTablePath<FWRVoiceEventDialogue>();
	AddToTablePath<FWRVoiceRevisionData>();
	AddToTablePath<FWRVoiceSubCommandTable>();
	AddToTablePath<FWRBuffTable>();
	AddToTablePath<FWRSocialAnimationTable>();
	AddToTablePath<FWRDanceGroupTable>();
	AddToTablePath<FWRDanceTable>();	
	AddToTablePath<FWRSocialMenuSettingTable>();	
	AddToTablePath<FWRFootPrintsTable>();
	AddToTablePath<FWRCharacterPresetTable>();
	AddToTablePath<FWRFactionTable>();
	AddToTablePath<FWRSOTable>();
	AddToTablePath<FWRElevatorTable>();
	AddToTablePath<FWRFloatingBoardTable>();
	AddToTablePath<FWRTardisTable>();
	AddToTablePath<FWRPlayerTable>();
	AddToTablePath<FWRHitPartialCollisionTable>();
	AddToTablePath<FWRCustomTextTable>();
	AddToTablePath<FWRFXPathTable>();
	AddToTablePath<FWRMovingWalkwayTable>();
	AddToTablePath<FWRCurrencyTable>();
	AddToTablePath<FWRRecommendedVoiceCommandTable>();
	AddToTablePath<FWRDefaultRecommendedVoiceCommandTable>();
	AddToTablePath<FWRDialogueRewardTable>();
	AddToTablePath<FWRSecretDialogAreaTable>();
	AddToTablePath<FWRVehicleTable>();
	AddToTablePath<FWRMicTable>();
	AddToTablePath<FWRVideoChatVoiceSubCommandTable>();
	AddToTablePath<FWRVideoChatDialogTable>();
	AddToTablePath<FWRCustomizingCategoryTable>();

	LoadTableAsset();
}

// [ 2019-7-12 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRTableManager::OnShutdown()
{
	if (StreamHandle.IsValid())
	{
		StreamHandle->ReleaseHandle();
		StreamHandle = nullptr;
	}
	
	for (auto Iter = Tables.CreateIterator(); Iter; ++Iter)
	{
		Iter.Value()->RemoveFromRoot();
	}

	Tables.Empty();
	TablesToStream.Empty();
}

// [ 2019-7-12 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRTableManager::OnLoadLevelStart()
{
	
}

// [ 2019-7-12 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRTableManager::OnLoadLevelComplete()
{
	
}

// [ 2019-7-12 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRTableManager::OnRenderStart()
{
	
}

void WRTableManager::LoadTableAsset()
{
#ifdef TABLE_DATA_ASYNCLOAD_BY_HAN_190916
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	StreamHandle = Streamable.RequestAsyncLoad(TablesToStream, FStreamableDelegate::CreateRaw(this, &WRTableManager::OnLoadTableComplete));
#else //TABLE_DATA_ASYNCLOAD_BY_HAN_190916

	for (int i = 0; i < TablesToStream.Num(); ++i)
	{
		Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, *TablesToStream[i].GetAssetPathString()));
	}
	OnLoadTableComplete();
#endif //TABLE_DATA_ASYNCLOAD_BY_HAN_190916
}

float WRTableManager::GetLoadingTablePercent()
{
	if(StreamHandle.IsValid() == false)
		return 1.f;

	return StreamHandle->GetProgress();
}

bool WRTableManager::IsLoading()
{
	return StreamHandle.IsValid() && StreamHandle->IsLoadingInProgress();
}

FWRMapTableBase* WRTableManager::FindRowInMapTable(int32 InMapTID, bool bInUsingEnsureMsg/* = true*/)
{
	EWRMapType::en MapType = FWRMapTableBase::GetMapType(InMapTID);
	FWRMapTableBase* FoundTable = nullptr;

	switch (MapType)
	{
	case EWRMapType::Lab:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapLapTable>(InMapTID);
		break;
	case EWRMapType::Town:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapTownTable>(InMapTID);
		break;
	case EWRMapType::Field:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapFieldTable>(InMapTID);
		break;
	case EWRMapType::Scenario:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapScenarioTable>(InMapTID);
		break;
	case EWRMapType::Stage:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapStageTable>(InMapTID);
		break;
	case EWRMapType::BattleField:
		FoundTable = WRTableManager::Get()->FindRow<FWRMapBattleFieldTable>(InMapTID);
		break;
	default:
		if (bInUsingEnsureMsg == true)
		{
			ensureMsgf(false, TEXT("Entered map TID is not allowed TID."));
		}
		
		break;
	}

	return FoundTable;
}

struct FWRItemTableBase* WRTableManager::FindRowInItemTable(const WRTableID IN InItemTID)
{
	EWRItemMainType ItemMainType = FWRItemTableBase::GetItemMainType(InItemTID);
	FWRItemTableBase* FoundTable = nullptr;

	switch (ItemMainType)
	{
	case EWRItemMainType::Weapon:
		FoundTable = WRTableManager::Get()->FindRow<FWRWeaponItemTable>(InItemTID);
		break;
	case EWRItemMainType::Armor:
		FoundTable = WRTableManager::Get()->FindRow<FWRArmorItemTable>(InItemTID);
		break;
	case EWRItemMainType::Temporary:
		FoundTable = WRTableManager::Get()->FindRow<FWRTemporaryItemTable>(InItemTID);
		break;
	case EWRItemMainType::Costume:
		FoundTable = WRTableManager::Get()->FindRow<FWRCostumeItemTable>(InItemTID);
		break;
	case EWRItemMainType::Material:
		FoundTable = WRTableManager::Get()->FindRow<FWRMaterialItemTable>(InItemTID);
		break;
	case EWRItemMainType::Interior:
		FoundTable = WRTableManager::Get()->FindRow<FWRInteriorItemTable>(InItemTID);
		break;
	case EWRItemMainType::Vehicle:
		FoundTable = WRTableManager::Get()->FindRow<FWRVehicleItemTable>(InItemTID);
		break;
	default:
// 		if (bInUsingEnsureMsg == true)
// 		{
// 			ensureMsgf(false, TEXT("Entered Item TID is not allowed TID."));
// 		}
		break;
	}

	return FoundTable;
}

void WRTableManager::OnLoadTableComplete()
{
	for (int i = 0; i < TablesToStream.Num(); ++i)
	{
		TAssetPtr<UDataTable> NewTable(TablesToStream[i]);
		
		if(NewTable.IsValid() == false)
			continue;

		NewTable.Get()->AddToRoot();
		Tables.Add(TablesToStream[i].GetAssetName(), NewTable.Get());

		// [ 2019-10-2 : magedoga ] Check Valid Data
		TArray<FName>	RowKeys;
		NewTable->GetRowMap().GetKeys(RowKeys);
		for (int32 j = 0; j < RowKeys.Num(); ++j)
		{
			FName RowKey = RowKeys[j];
			FWRTableRow* FoundRow = NewTable->FindRow<FWRTableRow>(RowKey, TEXT("GENERAL"));
			if (FoundRow != nullptr)
			{
#if WITH_EDITOR
				FoundRow->CheckValidData();
#endif // WITH_EDITOR
				FoundRow->RowName = RowKey;

				FoundRow->GenerateCustomData();
			}
		}
	}
	
#ifdef TABLE_DATA_ASYNCLOAD_BY_HAN_190916
	StreamHandle->ReleaseHandle();
	StreamHandle = nullptr;
#endif //TABLE_DATA_ASYNCLOAD_BY_HAN_190916
	TablesToStream.Empty();
}

//====================================================================================

