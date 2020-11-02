// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSingletonManager.h"

#include "List.h"
#include "Ticker.h"

#include "../WRMapManager.h"
#include "../WRPathManager.h"
#include "../WRConsoleManager.h"
#include "../WRPopupManager.h"
#include "../WRCharacterManager.h"
#include "../WRUIPathManager.h"
#include "../WRUIManager.h"
#include "../WRFSMManager.h"
#include "../WRCombatManager.h"
#include "../WRInputSystemManager.h"
#include "../WRSFXManager.h"
#include "../WRVFXManager.h"
#include "../../Table/base/WRTableManager.h"
#include "../WRAnimManager.h"
#include "../WRObjectInteractionManager.h"
#include "../WRMaterialParameterCollectionManager.h"
#include "../WRTriggerManager.h"
#include "../WRSkillManager.h"
#include "../WRQuestManager.h"
#include "../WRSequenceManager.h"
#include "../WRNoticeManager.h"
#include "../WRTODManager.h"
#include "../WRItemManager.h"
#include "../WRInventoryManager.h"
#include "../WREnvInteractionManager.h"
#include "../../Network/IMNetwork.h"
#include "../WRAccountManager.h"
#include "../WRDialogueManager.h"
#include "../WRCompassManager.h"
#include "../WRTutorialManager.h"
#include "../WRMeshModifyManager.h"
#include "../WRBuffManager.h"
#include "../WRTimeManager.h"
#include "../WRMediaPlayerManager.h"
#include "../WRCheersManager.h"
#include "../WRChatBubbleManager.h"
#include "../WRDoubleClickManager.h"
#include "../WRAsyncLoadUObjectManager.h"
#include "../WRCurrencyManager.h"
#include "../WRPreviewManager.h"
#include "../WRVoiceContentsManager.h"
#include "../WRVehicleManager.h"
#include "../WRTargetLocationGuideManager.h"
#include "../WRInteriorManager.h"
#include "../WRSpawnSimulationManager.h"
#include "../WRAutoPilotManager.h"
#include "../WRCharacterCustomizingManager.h"
#include "../WRPostProcessMaterialManager.h"


// For CPP
DECLARE_CYCLE_STAT(TEXT("WRSingletonManager_CollectInstanceForTick"), STAT_WRSingletonManager_CollectInstanceForTick, STATGROUP_WRSingletonManager);
DECLARE_CYCLE_STAT(TEXT("WRSingletonManager_ProcessTick"), STAT_WRSingletonManager_ProcessTick, STATGROUP_WRSingletonManager);


TDoubleLinkedList<class WRSingletonObject*> WRSingletonManager::SingletonList;
//TMap<EWRManagerTickGroup, class WRSingletonObject*> WRSingletonManager::SingletonsForTick;
FDelegateHandle WRSingletonManager::TickHandle;


void WRSingletonManager::Start()
{
	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateStatic(WRSingletonManager::Tick));
}

void WRSingletonManager::Shutdown()
{
	// [ 2019-9-3 : magedoga ] Remove Tick
	if (TickHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}
	
	TDoubleLinkedList<WRSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->Shutdown();
		Node = Node->GetPrevNode();
	}

#if WITH_EDITOR
	// [ 2019-10-28 : magedoga ] Shutdonw 여부 확인
	for (auto Singleton : SingletonList)
	{
		checkf(Singleton->bShutdown == true, TEXT("Not called OnShutdown."));
	}
#endif // WITH_EDITOR

	WRSingletonManager::DestroySingletonInstance();
}

void WRSingletonManager::LoadLevelStart()
{
	for (auto Singleton : SingletonList)
	{
		Singleton->OnLoadLevelStart();
	}
}

void WRSingletonManager::LoadLevelComplete()
{
	for (auto Singleton : SingletonList)
	{
		Singleton->OnLoadLevelComplete();
	}
}

void WRSingletonManager::RenderStart()
{
	for (auto Singleton : SingletonList)
	{
		Singleton->OnRenderStart();
	}
}

void WRSingletonManager::EndLoadLevelProcess()
{
	for (auto Singleton : SingletonList)
	{
		Singleton->EndLoadLevelProcess();
	}
}

void WRSingletonManager::Add(class WRSingletonObject* InSingleton)
{
	if (SingletonList.Contains(InSingleton) == true)
		return;

	SingletonList.AddTail(InSingleton);
}

void WRSingletonManager::DestroySingletonInstance()
{
	TDoubleLinkedList<WRSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->DestroyInstance();
		Node = Node->GetPrevNode();
	}

// 	for (auto Singleton : SingletonList)
// 	{
// 		Singleton->DestroyInstance();
// 	}

	SingletonList.Empty();
}

bool WRSingletonManager::Tick(float InDeltaTime)
{
	TMap<EWRManagerTickGroup, TSet<WRSingletonObject*>>	CollectedInstanceForTick;
	{
		SCOPE_CYCLE_COUNTER(STAT_WRSingletonManager_CollectInstanceForTick);
		for (auto SingletonObject : SingletonList)
		{
			EWRManagerTickGroup TickGroup = SingletonObject->GetTickGroup();
			TSet<WRSingletonObject*>& FoundOrAddedList = CollectedInstanceForTick.FindOrAdd(TickGroup);
			FoundOrAddedList.Add(SingletonObject);
		}
	}

	CollectedInstanceForTick.Remove(EWRManagerTickGroup::TG_None);
	CollectedInstanceForTick.Remove(EWRManagerTickGroup::TG_Max);
	CollectedInstanceForTick.KeySort([](EWRManagerTickGroup A, EWRManagerTickGroup B){
		return (uint32)A < (uint32)B;
	});

	{
		SCOPE_CYCLE_COUNTER(STAT_WRSingletonManager_ProcessTick);
		for (auto It = CollectedInstanceForTick.CreateIterator(); It ; ++It)
		{
			for(auto& SingletonObject : It->Value)
			{
				SingletonObject->OnTick(InDeltaTime);
			}
		}
	}
	
	return true;
}

void WRSingletonManager::CreateManagerWhen_StartGame()
{
/*	//====================================================================================
	// [ 2019-6-17 : magedoga ] Manager 생성 샘플
	//====================================================================================
	// [ 2019-6-17 : magedoga ] Core Delegate 연결 미사용, Tick 미사용
		WRPathManager::CreateInstance(TEXT("PathManager"));

	// [ 2019-6-17 : magedoga ] Core Delegate 연결 미사용, Tick 사용
		WRPathManager::CreateInstance(TEXT("PathManager"), true);

	// [ 2019-6-17 : magedoga ] Core Delegate 연결 사용, Tick 사용
		WRPathManager::CreateInstance(TEXT("PathManager"), FCBindDelegateFlag(FCBindDelegateFlag::EnterBackground | FCBindDelegateFlag::EnterForeground), true);
	//==================================================================================== */

	WRConsoleManager::CreateInstance("ConsoleManager");
	WRPathManager::CreateInstance("PathManager");
	WRAsyncLoadUObjectManager::CreateInstance("AsyncLoadUObjectManager");

	WRMapManager::CreateInstance("MapManager");
	WRUIPathManager::CreateInstance("UIPathManager");
	WRAccountManager::CreateInstance(TEXT("AccountManager"));
	IMNetwork::CreateInstance(TEXT("Network"), true);
	WRDoubleClickManager::CreateInstance(TEXT("DoubleClickManager"), true);
	WRInputSystemManager::CreateInstance(TEXT("InputSystemManager"), true, EWRManagerTickGroup::TG_PreProcess);
	WRCharacterManager::CreateInstance("CharacterManager", true);
	WRUIManager::CreateInstance("UIManager", true);
	//WRUIStereoLayerManager::CreateInstance("StereoLayerManager");
	WRPopupManager::CreateInstance("PopupManager");
	WRFSMManager::CreateInstance("FSMManager");
	WRCombatManager::CreateInstance("CombatManager", true);
	WRAnimManager::CreateInstance("WRAnimManager");
	WRTableManager::CreateInstance(TEXT("TableManager"));
    WRSFXManager::CreateInstance(TEXT("SFXManager"));
	WRVFXManager::CreateInstance("VFXManager", true);
	WRObjectInteractionManager::CreateInstance("WRObjectInteractionManager", true);
	WRMaterialParameterCollectionManager::CreateInstance("MaterialParameterCollectionManager", true);
	WRTriggerManager::CreateInstance(TEXT("TriggerManager"), true);
	WRSkillManager::CreateInstance(TEXT("SkillManager"));
	WRCompassManager::CreateInstance(TEXT("CompassManager"), true);
	WRQuestManager::CreateInstance(TEXT("QuestManager"), true);
	WRDialogueManager::CreateInstance(TEXT("DialogueManager"), true);
	WRSequenceManager::CreateInstance(TEXT("SequenceManager"));
	WRNoticeManager::CreateInstance(TEXT("NoticeManager"));
	WRTODManager::CreateInstance(TEXT("TODManager"), true);
	WREnvInteractionManager::CreateInstance(TEXT("EnvInteractionManager"));
	WRTutorialManager::CreateInstance(TEXT("WRTutorialManager"), true);
	WRItemManager::CreateInstance(TEXT("ItemManager"));
	WRInventoryManager::CreateInstance(TEXT("InventoryManager"));	
	WRMeshModifyManager::CreateInstance(TEXT("WRMeshModifyManager"));
	WRBuffManager::CreateInstance(TEXT("WRBuffManager"));
	WRTimeManager::CreateInstance(TEXT("WRTimeManager"));
	WRMediaPlayerManager::CreateInstance(TEXT("WRMediaPlayerManager"));
	WRCheersManager::CreateInstance(TEXT("WRCheersManager"), true);
	WRChatBubbleManager::CreateInstance(TEXT("ChatBubbleManager"));
	WRCurrencyManager::CreateInstance(TEXT("CurrencyManager"), true);
	WRPreviewManager::CreateInstance(TEXT("PreviewManager"));
	WRVoiceContentsManager::CreateInstance(TEXT("VoiceContentsManager"), true);
	WRTargetLocationGuideManager::CreateInstance(TEXT("TargetLocationGuideManager"), true);
	WRInteriorManager::CreateInstance(TEXT("InteriorManager"), true);
	WRAutoPilotManager::CreateInstance(TEXT("AutoPilotManager"), true);
	WRCharacterCustomizingManager::CreateInstance(TEXT("CharacterCustomizingManager"), true);
	WRPostProcessMaterialManager::CreateInstance(TEXT("PostProcessMaterialManager"));

#ifdef UE_EDITOR
	WRVehicleManager::CreateInstance(TEXT("VehicleManager"), true);
	WRSpawnSimulationManager::CreateInstance(TEXT("SpawnSimulationManager"), true);
#else //UE_EDITOR
	WRVehicleManager::CreateInstance(TEXT("VehicleManager"));
	WRSpawnSimulationManager::CreateInstance(TEXT("SpawnSimulationManager"));
#endif //UE_EDITOR
}
