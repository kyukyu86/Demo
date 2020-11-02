#include "WRCheersManager.h"
#include "WRUIManager.h"
#include "WRCharacterManager.h"
#include "WRInventoryManager.h"
#include "WRInputSystemManager.h"
#include "WRNoticeManager.h"
#include "WRPopupManager.h"

#include "UI/Panel/WRUIPanel_Drink.h"
#include "UI/Panel/WRUIPanel_DrinkAlert.h"
#include "UI/Panel/WRUIPanel_DrinkIcon.h"

#include "Network/IMMessageSender.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Table/WRSkillTable.h"
#include "Table/WRCustomTextTable.h"
#include "DelegateSignatureImpl.inl"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "UI/Panel/WRUIPanel_DanceMode.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "WRItemManager.h"

#include "Localization/WRLocalizationProcessor.h"

#define DEF_CHEERS_SKILL_TID 5002
#define DEF_SUGGESTED_TEXT_TID 8
#define DEF_ALL_REJECT_TEXT_TID 12

WRCheersManager::WRCheersManager()
{
}


WRCheersManager::~WRCheersManager()
{
}

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRCheersManager::OnInitialize()
{
	
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRCheersManager::OnShutdown()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRCheersManager::OnLoadLevelStart()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRCheersManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRCheersManager::OnRenderStart()
{
        
}

bool WRCheersManager::OnTick(float InDeltaTime)
{
	UpdateSuggestTime(InDeltaTime);
	UpdateForCheersPossiblePlayers(InDeltaTime);

	return true;
}

//====================================================================================
// Common
//====================================================================================

bool WRCheersManager::CheckSocialInput(EWRInput IN InInput)
{
	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return false;

	switch (InInput)
	{
	case EWRInput::R2:
	{
		WREquipItemBase* pTemporaryItem = WRItemManager::Get()->FindEquippedTemporaryWeapon();
		if (pTemporaryItem != nullptr)
		{
			if (pTemporaryItem->GetSubType() == EWRItemSubType::Drink)
			{
				// [ 2020-4-20 : kyu ] 예외 추가
				UWRUIPanel_Drink* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Drink>(EWRUIPanelKind::Drink);
				if (pWidget != nullptr && pWidget->IsVisible())
				{
					Req_SuggestCheers();
				}
				return true;
			}
		}
	}
	break;
	case EWRInput::Circle:
	{
		WREquipItemBase* pTemporaryItem = WRItemManager::Get()->FindEquippedTemporaryWeapon();
		if (pTemporaryItem != nullptr)
		{
			if (pTemporaryItem->GetSubType() == EWRItemSubType::Drink)
			{
				EWRCheersConditionType eCheersCondition = pMine->GetCheersCondition();
				if (eCheersCondition == EWRCheersConditionType::WaitingAnswer)
				{
					// [ 2020-4-20 : kyu ] 예외 추가
					UWRUIPanel_Drink* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Drink>(EWRUIPanelKind::Drink);
					if (pWidget != nullptr && pWidget->IsVisible())
					{
						Req_AnswerCheers(true);
					}
					return true;
				}
			}
		}		
	}
	break;
	case EWRInput::Cross:
	{
		WREquipItemBase* pTemporaryItem = WRItemManager::Get()->FindEquippedTemporaryWeapon();
		if (pTemporaryItem != nullptr)
		{
			if (pTemporaryItem->GetSubType() == EWRItemSubType::Drink)
			{
				EWRCheersConditionType eCheersCondition = pMine->GetCheersCondition();
				if (eCheersCondition == EWRCheersConditionType::WaitingAnswer)
				{
					// [ 2020-4-20 : kyu ] 예외 추가
					UWRUIPanel_Drink* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Drink>(EWRUIPanelKind::Drink);
					if (pWidget != nullptr && pWidget->IsVisible())
					{
						Req_AnswerCheers(false);
					}
					return true;
				}
			}
		}
	}
	break;
	}

	return false;
}

//====================================================================================
// Cheers
//====================================================================================

void WRCheersManager::AddCheersAnswerAID(const AID_t IN InAID)
{
	EWRCheersState& State = CheersAnswerAIDs.FindOrAdd(InAID);
	State = EWRCheersState::None;
}

void WRCheersManager::RemoveCheersAnswerAID(const AID_t IN InAID)
{
	CheersAnswerAIDs.Remove(InAID);
}

void WRCheersManager::RemoveCheersAnswer(const AID_t IN InAID)
{
	CheersAnswerAIDs.Remove(InAID);
	CloseCheersIcon(InAID);
	WRCharacterManager::Get()->SetCheersCondition(InAID, EWRCheersConditionType::None);
}

void WRCheersManager::ChangeCheersAnswerState(const AID_t IN InAID, const EWRCheersState IN InState, const bool IN InUpdateIcon /* = true */)
{
	EWRCheersState& State = CheersAnswerAIDs.FindOrAdd(InAID);
	State = InState;

	if (InUpdateIcon && InState != EWRCheersState::None)
	{
		OpenCheersIcon(InAID, InState);
	}
}

void WRCheersManager::ClearCheersInfo(const bool IN InEachCharacterRelease /* = false */)
{
	// 요청자 아이콘 해제
	CloseCheersIcon(CheersSuggesterAID);
	CheersSuggesterAID = INVALID_AID;

	// 응답자 아이콘 해제
	for (auto Answer : CheersAnswerAIDs)
	{
		CloseCheersIcon(Answer.Key);

// 		if (InEachCharacterRelease)
// 		{
// 			WRCharacterManager::Get()->SetCheersCondition(Answer.Key, EWRCheersConditionType::None);
// 		}
	}

	// 데이터 해제
	CheersAnswerAIDs.Empty();
	bUseCheersSuggestTime = false;
	fCheersSuggestElapsedTime = 0.f;		
	WRCharacterManager::Get()->SetMyCheersCondition(EWRCheersConditionType::None);
	
	// Cheers Guide UI
	if (WRItemManager::Get()->FindEquippedTemporaryWeapon() == nullptr)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
	}
	else
	{
		OpenCheersUI_Guide();
	}
}

void WRCheersManager::OpenCheersIcon(const AID_t IN InAID, const EWRCheersState IN InState)
{
	if (WRUIManager::IsValid() == false)
		return;

	AWRCharacter* pCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
	if (pCharacter == nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));

			EWRDrinkIconType DrinkIconType = EWRDrinkIconType::None;
			if (InState == EWRCheersState::AnswerWait)
				DrinkIconType = EWRDrinkIconType::AnswerWaiting;
			else if (InState == EWRCheersState::AnswerYes)
				DrinkIconType = EWRDrinkIconType::AnswerYes;
			else if (InState == EWRCheersState::AnswerNo)
				DrinkIconType = EWRDrinkIconType::AnswerNo;
			else if (InState == EWRCheersState::Suggest)
				DrinkIconType = EWRDrinkIconType::Suggester;
			else if (InState == EWRCheersState::CheersPossible)
				DrinkIconType = EWRDrinkIconType::Possible;
			else
				WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);

			UWRUIPanel_DrinkIcon* Panel = Cast<UWRUIPanel_DrinkIcon>(InCombinedPanelData.UIPanel);
			if (Panel)
				Panel->SetIconType(DrinkIconType);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DrinkIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, pCharacter);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 	FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::DrinkIcon);
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DrinkIcon,
// 		pCharacter,
// 		FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 		EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 		&Delegate);
}

void WRCheersManager::CloseCheersIcon(const AID_t IN InAID)
{
	AWRCharacter* pCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
	if (pCharacter == nullptr)
		return;

	WRUIManager::Get()->CloseUI(pCharacter, EWRUIPanelKind::DrinkIcon);
}

void WRCheersManager::OpenCheersUI_Suggest()
{
	if (WRUIManager::IsValid() == false)
		return;

	bHideCheersContents = false;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
			UWRUIPanel_Drink* Panel = Cast<UWRUIPanel_Drink>(InCombinedPanelData.UIPanel);
			if (Panel)
				Panel->SetToSuggest();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Drink, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 	FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::Drink);
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Drink,
// 		Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()),
// 		FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 		EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 		&Delegate);
}

void WRCheersManager::RotateComplete(const AID_t IN InSuggesterAID, const AID_t IN InAnswerAID)
{
	AWRCharacter* pSuggester = WRCharacterManager::Get()->FindbyAID(InSuggesterAID);
	AWRCharacter* pAnswer = WRCharacterManager::Get()->FindbyAID(InAnswerAID);
	if (pSuggester == nullptr || pAnswer == nullptr)
		return;

	if (pAnswer->GetCharacterType() == EWRCharacterType::NPC)
	{
		AWRCharacterNPC* CastedNPC = Cast<AWRCharacterNPC>(pAnswer);
		if (CastedNPC != nullptr)
		{
			//CastedNPC->OnEmotion(EWRNPCEmotion::Cheers, pSuggester);
			CastedNPC->SetLookIKTarget(nullptr);
		}		
	}
	else
	{
		// + Player도 회전해야하면 적용
	}
}

void WRCheersManager::UpdateForCheersPossiblePlayers(float fDeltaTime)
{
	if (bHideCheersContents)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return;

	fUpdateForCheersDeltaTime += fDeltaTime;
	if (fUpdateForCheersDeltaTime < 1.f)
		return;

	fUpdateForCheersDeltaTime = 0.f;

	//---------------------------------------------------------------------
	// exception
	//---------------------------------------------------------------------
	if (pMine->IsEquippedBottle() == false)
	{
		// + 혹여 있으면 모두 끄기
		if (CheersPossiblePlayers.Num() != 0)
		{
			for (auto AID : CheersPossiblePlayers)
			{
				CloseCheersIcon(AID);
			}
			CheersPossiblePlayers.Empty();
		}		
		return;
	}
		
	switch (pMine->GetCheersCondition())
	{
	case EWRCheersConditionType::Suggest:
	{
		// + 내가 제안자면 UI는 그대로 둠
		if (CheersPossiblePlayers.Num() != 0)
		{
			CheersPossiblePlayers.Empty();
		}
		return;
	}
	break;
	case EWRCheersConditionType::WaitingBegin:
	case EWRCheersConditionType::WaitingAnswer:
	case EWRCheersConditionType::Answer:
	{
		// + 내가 제안자가 아닐 때 제안자 제외하고 모두 끄기
		if (CheersPossiblePlayers.Num() != 0)
		{
			for (auto AID : CheersPossiblePlayers)
			{
				if (CheersSuggesterAID == AID)
					continue;

				CloseCheersIcon(AID);
			}
			CheersPossiblePlayers.Empty();
		}
		return;
	}
	break;
	}

	if (bUseCheersSuggestTime)
		return;
	//---------------------------------------------------------------------

	FWRSkillTable* FoundSkillData = WRTableManager::Get()->FindRow<FWRSkillTable>(DEF_CHEERS_SKILL_TID, "SkillPlayer_Atk");
	if (FoundSkillData == nullptr)
		return;
	
	FWRPlayerSkillTable* pPlayerSkillTable = static_cast<FWRPlayerSkillTable*>(FoundSkillData);
	if (pPlayerSkillTable == nullptr)
		return;

	// Sector Find
	TDoubleLinkedList<AWRCharacter*> NearByCharacters;
	WRCharacterManager::Get()->GetActorListInnerSector(NearByCharacters, pMine->GetActorTransform(), pPlayerSkillTable->MinRange, pPlayerSkillTable->Range, pPlayerSkillTable->Angle);
	if (NearByCharacters.Num() == 0)
	{
		for (auto AID : CheersPossiblePlayers)
		{
			CloseCheersIcon(AID);
		}
		CheersPossiblePlayers.Empty();
	}
	else
	{
		// Collect Possible Players
		TArray<AID_t> NewCheersPossiblePlayers;		
		for (AWRCharacter* Character : NearByCharacters)
		{
			if (IsValidDrinker(Character, true) == false)
				continue;

			NewCheersPossiblePlayers.Add(Character->GetActorID());
		}

		// Result
		if (NewCheersPossiblePlayers.Num() == 0)
		{
			// Close All
			for (auto AID : CheersPossiblePlayers)
			{
				CloseCheersIcon(AID);
			}
			CheersPossiblePlayers.Empty();
		}
		else
		{
			// New Player Open
			for (auto AID : NewCheersPossiblePlayers)
			{
				if (CheersPossiblePlayers.Find(AID) == INDEX_NONE)
				{
					OpenCheersIcon(AID, EWRCheersState::CheersPossible);
				}
			}

			// Delete Player Close
			for (auto AID : CheersPossiblePlayers)
			{
				if (NewCheersPossiblePlayers.Find(AID) == INDEX_NONE)
				{
					CloseCheersIcon(AID);
				}
			}
			CheersPossiblePlayers.Empty();
			CheersPossiblePlayers.Append(NewCheersPossiblePlayers);
		}
	}
}

void WRCheersManager::OpenCheersUI_Guide()
{
	if (WRUIManager::IsValid() == false)
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;
	
	// 예외 추가
	bool bException = false;
	UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
	if (DanceModeComponent && DanceModeComponent->IsEnterDanceArea())
	{
		bException = true;
	}
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
	{
		bException = true;
	}

	if (bException)
	{
		// 혹시 켜져있으면 끔
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Drink))
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
		bHideCheersContents = true; // 외부요인에 의해 숨겨짐
		return;
	}

	bHideCheersContents = false;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
			UWRUIPanel_Drink* Panel = Cast<UWRUIPanel_Drink>(InCombinedPanelData.UIPanel);
			if (Panel)
				Panel->SetToGuide();
		}
	});
	
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Drink, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

bool WRCheersManager::AttemptToOpenCheersContents()
{
	if (bHideCheersContents == false)
		return false;

	if (WRItemManager::Get()->FindEquippedTemporaryWeapon() != nullptr)
	{
		OpenCheersUI_Guide();
		return true;
	}

	return false;
}

bool WRCheersManager::PauseCheersContents()
{
	if (bHideCheersContents)
		return false;

	bHideCheersContents = true;

	// 1. Drink Panel
	UWRUIPanel_Drink* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Drink>(EWRUIPanelKind::Drink);
	if (pWidget != nullptr && pWidget->GetUIType() == EWRDrinkType::Guide)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
	}
	
	// 2. Other Characters Icon
	for (auto AID : CheersPossiblePlayers)
	{
		CloseCheersIcon(AID);
	}
	CheersPossiblePlayers.Empty();

	return true;
}

void WRCheersManager::OnUnEquipBottle()
{
	// + 술병 버렸을 떄 혹시나 거절이나 취소가 가능하면 보내기
	Req_AnswerCheers(false);
	Req_CancelCheers();

	ClearCheersInfo(true);
}

void WRCheersManager::OpenCheersUI_Suggested()
{
	if (WRUIManager::IsValid() == false)
		return;

	AWRCharacter* pFoundCharacter = WRCharacterManager::Get()->FindbyAID(CheersSuggesterAID);
	if (pFoundCharacter == nullptr)
		return;

	bHideCheersContents = false;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			UWRUIPanel_Drink* Panel = Cast<UWRUIPanel_Drink>(InCombinedPanelData.UIPanel);
			if (Panel)
			{				
				FString strContents = "Unknown";
				if (pFoundCharacter != nullptr)
				{
					strContents = pFoundCharacter->GetCharacterName().ToString();
				}
				FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_SUGGESTED_TEXT_TID);
				if (pTextTable != nullptr)
				{
					FString LocalKey = pTextTable->Localkey.ToString();
					strContents += WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString();
				}
				Panel->SetToSuggested(strContents);
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Drink, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 	FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::Drink);
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Drink,
// 		Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()),
// 		FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 		EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 		&Delegate);
}

void WRCheersManager::OpenCheersUI_Impossible()
{
	if (WRUIManager::IsValid() == false)
		return;

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DrinkAlert, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 	FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::DrinkAlert);
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DrinkAlert,
// 		Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()),
// 		FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 		EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 		&Delegate);
}

void WRCheersManager::StartSuggestTime()
{
	bUseCheersSuggestTime = true;
	fCheersSuggestElapsedTime = 0.f;
}

void WRCheersManager::UpdateSuggestTime(const float IN InDeltaTime)
{
	if (bUseCheersSuggestTime == false)
		return;
	
	fCheersSuggestElapsedTime += InDeltaTime;
	if (fCheersSuggestElapsedTime >= (DEF_CHEERS_SUGGEST_LIMIT_TIME/1000.f))
	{
		bUseCheersSuggestTime = false;
		fCheersSuggestElapsedTime = 0.f;
				
		if (CheersSuggesterAID == WRCharacterManager::Get()->GetMyAID())
		{
			// Close Suggest UI
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);

			// Req Cheers
			Req_BeginCheers();
		}
		else
		{
			// 응답 대기중에 제안시간 끝나면 자동으로 취소를 보낸다
			if (MyAnswerState == EWRCheersState::AnswerWait)
			{
				Req_CancelCheers();
			}
		}
	}
}

bool WRCheersManager::IsValidDrinker(const AID_t IN InAID, const bool IN InUseSuggest)
{
	AWRCharacter* pCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
	return IsValidDrinker(pCharacter, InUseSuggest);
}

bool WRCheersManager::IsValidDrinker(class AWRCharacter* IN InChar, const bool IN InUseSuggest)
{
	if (InChar == nullptr)
		return false;

	if (InChar->GetCharacterType() == EWRCharacterType::Player)
	{
		AWRCharacterPlayer* pPlayer = Cast<AWRCharacterPlayer>(InChar);
		if (pPlayer == nullptr)
			return false;

		if (InUseSuggest && pPlayer->GetCheersCondition() != EWRCheersConditionType::None)
			return false;

		if (pPlayer->GetTemporaryItemTID() == INVALID_TABLE_TID)
			return false;

		FWRTemporaryItemTable* pTemporaryItem = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, pPlayer->GetTemporaryItemTID());
		if (pTemporaryItem == nullptr)
			return false;

		if (pTemporaryItem->SubType != EWRItemSubType::Drink)
			return false;
	}
	else if (InChar->GetCharacterType() == EWRCharacterType::NPC)
	{
		AWRCharacterNPC* pNpc = Cast<AWRCharacterNPC>(InChar);
		if (pNpc == nullptr)
			return false;

		if (pNpc->GetNpcJobType() != EWRNpcJobType::Drinker)
			return false;

		if (InUseSuggest && pNpc->GetCheersCondition() != EWRCheersConditionType::None)
			return false;
	}
	else
	{
		return false;
	}
	return true;
}

bool WRCheersManager::IsAllCheersAnswerNo()
{
	for (auto Answer : CheersAnswerAIDs)
	{
		if (Answer.Value != EWRCheersState::AnswerNo)
			return false;
	}
	return true;
}

void WRCheersManager::OnStartCheers()
{
	//--------------------------------------------------
	// + 여기에 UI 예외 추가하세요 (건배요청 받음 시작)
	//--------------------------------------------------
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);
		}
	}

	WRPopupManager::Get()->ClosePopup();
}

void WRCheersManager::OnEndCheers()
{
	// 거절해서 종료
	// 취소해서 종료
	// 완료해서 종료

	//--------------------------------------------------
	// + 여기에 UI 예외 추가하세요 (건배요청 받음 종료)
	//--------------------------------------------------
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
		{
			DanceModeComponent->ShowStartDanceUI();
		}
	}
}

//====================================================================================
// 건배 제안하기
void WRCheersManager::Req_SuggestCheers()
{
	if (bUseCheersSuggestTime)
		return;

	CheersAnswerAIDs.Empty();

	if (CheersPossiblePlayers.Num() == 0)
	{
		OpenCheersUI_Impossible();
		return;
	}

	for (auto AID : CheersPossiblePlayers)
	{
		IMMessageSender::RequestSuggestCheers(AID);
	}
	CheersSuggesterAID = WRCharacterManager::Get()->GetMyAID();
	StartSuggestTime();
}

void WRCheersManager::Ack_SuggestCheers(const AID_t IN InAnswerAID)
{
	
}

void WRCheersManager::Inf_SuggestCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition)
{
	if (InSuggestAID == WRCharacterManager::Get()->GetMyAID())
	{
		CheersSuggesterAID = InSuggestAID;
		AddCheersAnswerAID(InAnswerAID);
		OpenCheersUI_Suggest();
		ChangeCheersAnswerState(InAnswerAID, EWRCheersState::AnswerWait);
	}
	else if (InAnswerAID == WRCharacterManager::Get()->GetMyAID())
	{
		MyAnswerState = EWRCheersState::AnswerWait;
		CheersSuggesterAID = InSuggestAID;
		OpenCheersUI_Suggested();
		OpenCheersIcon(InSuggestAID, EWRCheersState::Suggest);
		StartSuggestTime();

		OnStartCheers();
	}
	else
	{
		// + 다른 사람이 봤을 때도 술잔아이콘이 동기화가 되어야하는가? -> 생각해보니 복잡해질 것 같음
	}

	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// 건배 응답 보냄
void WRCheersManager::Req_AnswerCheers(bool IN InAnswer)
{
	if (CheersSuggesterAID == INVALID_AID)
		return;

	IMMessageSender::RequestAnswerCheers(CheersSuggesterAID, InAnswer);
	WRCharacterManager::Get()->SetMyCheersCondition(EWRCheersConditionType::Answer);

	MyAnswerState = InAnswer ? EWRCheersState::AnswerYes : EWRCheersState::AnswerNo;
}

void WRCheersManager::Ack_AnswerCheers(const AID_t IN InSuggestAID, bool IN InAnswer)
{

}

void WRCheersManager::Inf_AnswerCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition, const bool IN InAccept)
{
	if (InSuggestAID == WRCharacterManager::Get()->GetMyAID())
	{
		if (InAccept)
		{			
			ChangeCheersAnswerState(InAnswerAID, EWRCheersState::AnswerYes);
		}
		else
		{		
			ChangeCheersAnswerState(InAnswerAID, EWRCheersState::AnswerNo, false);
			bool bAllReject = IsAllCheersAnswerNo();
			if(bAllReject)
			{
				// [ 2020-4-2 : kyu ] 모두가 거절인 경우 더 기다리지 않고 모두 거절됨 알림
				FString strNoticeKey = "All Reject";
				FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_ALL_REJECT_TEXT_TID);
				if (pTextTable != nullptr)
					strNoticeKey = pTextTable->Localkey.ToString();
				WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, strNoticeKey).ToString());
				ClearCheersInfo(true);
			}
			else
			{
				OpenCheersIcon(InAnswerAID, EWRCheersState::AnswerNo);
			}
		}
		
	}
	else if (InAnswerAID == WRCharacterManager::Get()->GetMyAID())
	{
		if (InAccept)
		{
			//WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
			OpenCheersUI_Guide();
		}
		else
		{
			// 거절인 경우 해제
			ClearCheersInfo(false);

			OnEndCheers();
		}
	}
	else
	{
		// + 다른 사람이 봤을 때도 술잔 아이콘에 변화가 있어야하는가? -> 생각해보니 복잡해질 것 같음
	}
		
	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// 건배 시작 보냄
void WRCheersManager::Req_BeginCheers()
{
	if (CheersAnswerAIDs.Num() == 0)
	{
		ClearCheersInfo(true);
		return;
	}

	bool bSend = false;
	for (auto Answer : CheersAnswerAIDs)
	{
		switch (Answer.Value)
		{
		case EWRCheersState::AnswerYes:
		{
			if (IsValidDrinker(Answer.Key, false) == false)
				continue;

			IMMessageSender::RequestBeginCheers(Answer.Key);
			bSend = true;
		}
		break;
		default:
		{
			continue;
		}
		break;
		}
	}

	// 보낸 요청이 없으면 취소보냄
	if (bSend == false)
	{
		Req_CancelCheers();
	}
}

void WRCheersManager::Ack_BeginCheers(const AID_t IN InAnswerAID)
{

}

void WRCheersManager::Inf_BeginCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition)
{
	// 회전은 비동기라, 다른 사람이 봤을 때도 Target이 Suggester에게 회전해야한다.
	AWRCharacter* pSuggester = WRCharacterManager::Get()->FindbyAID(InSuggestAID);
	AWRCharacter* pAnswer = WRCharacterManager::Get()->FindbyAID(InAnswerAID);
	if (pSuggester == nullptr || pAnswer == nullptr)
		return;

	if (InSuggestAID == WRCharacterManager::Get()->GetMyAID())
	{
		// 제안자 액션 (나일 때만 액션함)
		WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
		pSuggester->DetermineFSMSignal(fsmInfo);

		UseSkillParam UseParam;
		UseParam.Caster = pSuggester;
		UseParam.SetSkillTID(DEF_CHEERS_SKILL_TID, pSuggester->GetSkillTableFileName());
		IMMessageSender::RequestUseSkill(UseParam);
		
		// 응답자 액션 (NPC일 때만 액션함)
		if (pAnswer->GetCharacterType() == EWRCharacterType::NPC)
		{
			// + NPC일 때는 회전보간하게 수정. Delegate : RotateComplete(InSuggestAID, InAnswerAID)
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotationLerpToTarget(pSuggester, FWRRotationEndDelegate::CreateRaw(this, &WRCheersManager::RotateComplete, pSuggester->GetActorID(), pAnswer->GetActorID()));

			// + RotateComplete에서 처리할거면 삭제
			AWRCharacterNPC* CastedNPC = Cast<AWRCharacterNPC>(pAnswer);
			if (CastedNPC != nullptr)
				CastedNPC->OnEmotion(EWRNPCEmotion::Cheers, pSuggester);
		}
		else
		{
			// + NPC가 아니면 회전만
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));
		}

		// 건배 종료. 모두 해제
		ClearCheersInfo(true);
	}
	else if (InAnswerAID == WRCharacterManager::Get()->GetMyAID())
	{		
		if (MyAnswerState == EWRCheersState::AnswerYes)
		{
			// [ 2020-4-9 : kyu ] 응답자가 나 일때는 그냥 바로 돈다
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));

			WRCharacterManager::Get()->ResetVRRotateOrientation();
			//WRInputSystemManager::Get()->ResetHMDOrientation();

			// 응답자 액션
			WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
			pAnswer->DetermineFSMSignal(fsmInfo);

			// [ 2020-3-28 : kyu ] Inf_UseSkill에서 Rot는 서버기준 Rot라서 임의Rot는 적용대상이 아니다. 동기해야함
			IMMessageSender::RequestPlayerMoveStop(pAnswer);

			UseSkillParam UseParam;
			UseParam.Caster = pAnswer;
			UseParam.SetSkillTID(DEF_CHEERS_SKILL_TID, pAnswer->GetSkillTableFileName());
			IMMessageSender::RequestUseSkill(UseParam);
		}		

		// 건배 종료. 모두 해제
		ClearCheersInfo(true);

		OnEndCheers();
	}
	else
	{
		// 제안자 액션
		WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
		pSuggester->DetermineFSMSignal(fsmInfo);
		
		// 응답자 액션
		if (pAnswer->GetCharacterType() == EWRCharacterType::NPC)
		{
			// + NPC일 때는 회전보간하게 수정. Delegate : RotateComplete(InSuggestAID, InAnswerAID)
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotationLerpToTarget(pSuggester, FWRRotationEndDelegate::CreateRaw(this, &WRCheersManager::RotateComplete, pSuggester->GetActorID(), pAnswer->GetActorID()));

			// + RotateComplete에서 처리할거면 삭제
			AWRCharacterNPC* CastedNPC = Cast<AWRCharacterNPC>(pAnswer);
			if (CastedNPC != nullptr)
				CastedNPC->OnEmotion(EWRNPCEmotion::Cheers, pSuggester);
		}
		else
		{
			// + NPC가 아니면 일단 그냥 회전
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));

			//pAnswer->DetermineFSMSignal(fsmInfo);
		}
	}
	
	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// 건배 취소 보냄
void WRCheersManager::Req_CancelCheers()
{
	if (CheersSuggesterAID == INVALID_AID)
		return;

	if (CheersSuggesterAID == WRCharacterManager::Get()->GetMyAID())
	{
		// Suggester가 나면 거절이 아닌 모두에게 취소 보냄
		bool bSend = false;
		for (auto Answer : CheersAnswerAIDs)
		{
			if (Answer.Value == EWRCheersState::AnswerNo)
				continue;

			if (IsValidDrinker(Answer.Key, false) == false)
				continue;

			IMMessageSender::RequestCancelCheers(Answer.Key);
			bSend = true;
		}		
		if (bSend == false)
		{
			ClearCheersInfo(true);
		}
	}
	else
	{
		// Suggester에게 취소 보냄
		IMMessageSender::RequestCancelCheers(CheersSuggesterAID);		
	}	
}

void WRCheersManager::Ack_CancelCheers(const AID_t IN InTargetAID)
{
	
}

void WRCheersManager::Inf_CancelCheers(const AID_t IN InCancellerAID, uint8 IN InCancellerCondition, const AID_t IN InTargetAID, uint8 IN InTargetCondition, const AID_t IN InSuggesterAID)
{
	// 취소자가 내 그룹 제안자
	if (InCancellerAID == InSuggesterAID && InCancellerAID == CheersSuggesterAID)
	{
		// 대상이 나
		if (InTargetAID == WRCharacterManager::Get()->GetMyAID())
		{
			// 모두 해제
			ClearCheersInfo(true);
			OnEndCheers();
		}
		else if(InCancellerAID == WRCharacterManager::Get()->GetMyAID())
		{
			// 모두 해제
			ClearCheersInfo(true);
		}
	}
	// 대상이 내 그룹 제안자
	else if(InTargetAID == InSuggesterAID && InTargetAID == CheersSuggesterAID)
	{		
		// 내가 취소자
		if (InCancellerAID == WRCharacterManager::Get()->GetMyAID())
		{
			ClearCheersInfo(false); // 나만 해제
			OnEndCheers();
		}
		// 다른 사람이 취소자
		else
		{			
			ChangeCheersAnswerState(InCancellerAID, EWRCheersState::AnswerNo, false);
			bool bAllReject = IsAllCheersAnswerNo();
			if (bAllReject)
			{
				// [ 2020-4-2 : kyu ] 취소로 인해 모두가 거절한 경우 모든 상태 해제
				FString strNotice = "All Reject";
				FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_ALL_REJECT_TEXT_TID);
				if (pTextTable != nullptr)
					strNotice = pTextTable->Localkey.ToString();
				WRNoticeManager::Get()->AddNotice(strNotice);
				ClearCheersInfo(true);
			}
			else
			{
				OpenCheersIcon(InCancellerAID, EWRCheersState::AnswerNo);
			}
		}
	}

	WRCharacterManager::Get()->SetCheersCondition(InCancellerAID, (EWRCheersConditionType)InCancellerCondition);
	WRCharacterManager::Get()->SetCheersCondition(InTargetAID, (EWRCheersConditionType)InTargetCondition);
}


//====================================================================================
//
//====================================================================================
