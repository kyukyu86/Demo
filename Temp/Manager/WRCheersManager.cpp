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

// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRCheersManager::OnInitialize()
{
	
}

// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRCheersManager::OnShutdown()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
void WRCheersManager::OnLoadLevelStart()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
void WRCheersManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
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
				// [ 2020-4-20 : kyu ] ���� �߰�
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
					// [ 2020-4-20 : kyu ] ���� �߰�
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
					// [ 2020-4-20 : kyu ] ���� �߰�
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
	// ��û�� ������ ����
	CloseCheersIcon(CheersSuggesterAID);
	CheersSuggesterAID = INVALID_AID;

	// ������ ������ ����
	for (auto Answer : CheersAnswerAIDs)
	{
		CloseCheersIcon(Answer.Key);

// 		if (InEachCharacterRelease)
// 		{
// 			WRCharacterManager::Get()->SetCheersCondition(Answer.Key, EWRCheersConditionType::None);
// 		}
	}

	// ������ ����
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
		// + Player�� ȸ���ؾ��ϸ� ����
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
		// + Ȥ�� ������ ��� ����
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
		// + ���� �����ڸ� UI�� �״�� ��
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
		// + ���� �����ڰ� �ƴ� �� ������ �����ϰ� ��� ����
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
	
	// ���� �߰�
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
		// Ȥ�� ���������� ��
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Drink))
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
		bHideCheersContents = true; // �ܺο��ο� ���� ������
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
	// + ���� ������ �� Ȥ�ó� �����̳� ��Ұ� �����ϸ� ������
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
			// ���� ����߿� ���Ƚð� ������ �ڵ����� ��Ҹ� ������
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
	// + ���⿡ UI ���� �߰��ϼ��� (�ǹ��û ���� ����)
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
	// �����ؼ� ����
	// ����ؼ� ����
	// �Ϸ��ؼ� ����

	//--------------------------------------------------
	// + ���⿡ UI ���� �߰��ϼ��� (�ǹ��û ���� ����)
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
// �ǹ� �����ϱ�
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
		// + �ٸ� ����� ���� ���� ���ܾ������� ����ȭ�� �Ǿ���ϴ°�? -> �����غ��� �������� �� ����
	}

	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// �ǹ� ���� ����
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
				// [ 2020-4-2 : kyu ] ��ΰ� ������ ��� �� ��ٸ��� �ʰ� ��� ������ �˸�
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
			// ������ ��� ����
			ClearCheersInfo(false);

			OnEndCheers();
		}
	}
	else
	{
		// + �ٸ� ����� ���� ���� ���� �����ܿ� ��ȭ�� �־���ϴ°�? -> �����غ��� �������� �� ����
	}
		
	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// �ǹ� ���� ����
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

	// ���� ��û�� ������ ��Һ���
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
	// ȸ���� �񵿱��, �ٸ� ����� ���� ���� Target�� Suggester���� ȸ���ؾ��Ѵ�.
	AWRCharacter* pSuggester = WRCharacterManager::Get()->FindbyAID(InSuggestAID);
	AWRCharacter* pAnswer = WRCharacterManager::Get()->FindbyAID(InAnswerAID);
	if (pSuggester == nullptr || pAnswer == nullptr)
		return;

	if (InSuggestAID == WRCharacterManager::Get()->GetMyAID())
	{
		// ������ �׼� (���� ���� �׼���)
		WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
		pSuggester->DetermineFSMSignal(fsmInfo);

		UseSkillParam UseParam;
		UseParam.Caster = pSuggester;
		UseParam.SetSkillTID(DEF_CHEERS_SKILL_TID, pSuggester->GetSkillTableFileName());
		IMMessageSender::RequestUseSkill(UseParam);
		
		// ������ �׼� (NPC�� ���� �׼���)
		if (pAnswer->GetCharacterType() == EWRCharacterType::NPC)
		{
			// + NPC�� ���� ȸ�������ϰ� ����. Delegate : RotateComplete(InSuggestAID, InAnswerAID)
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotationLerpToTarget(pSuggester, FWRRotationEndDelegate::CreateRaw(this, &WRCheersManager::RotateComplete, pSuggester->GetActorID(), pAnswer->GetActorID()));

			// + RotateComplete���� ó���ҰŸ� ����
			AWRCharacterNPC* CastedNPC = Cast<AWRCharacterNPC>(pAnswer);
			if (CastedNPC != nullptr)
				CastedNPC->OnEmotion(EWRNPCEmotion::Cheers, pSuggester);
		}
		else
		{
			// + NPC�� �ƴϸ� ȸ����
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));
		}

		// �ǹ� ����. ��� ����
		ClearCheersInfo(true);
	}
	else if (InAnswerAID == WRCharacterManager::Get()->GetMyAID())
	{		
		if (MyAnswerState == EWRCheersState::AnswerYes)
		{
			// [ 2020-4-9 : kyu ] �����ڰ� �� �϶��� �׳� �ٷ� ����
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));

			WRCharacterManager::Get()->ResetVRRotateOrientation();
			//WRInputSystemManager::Get()->ResetHMDOrientation();

			// ������ �׼�
			WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
			pAnswer->DetermineFSMSignal(fsmInfo);

			// [ 2020-3-28 : kyu ] Inf_UseSkill���� Rot�� �������� Rot�� ����Rot�� �������� �ƴϴ�. �����ؾ���
			IMMessageSender::RequestPlayerMoveStop(pAnswer);

			UseSkillParam UseParam;
			UseParam.Caster = pAnswer;
			UseParam.SetSkillTID(DEF_CHEERS_SKILL_TID, pAnswer->GetSkillTableFileName());
			IMMessageSender::RequestUseSkill(UseParam);
		}		

		// �ǹ� ����. ��� ����
		ClearCheersInfo(true);

		OnEndCheers();
	}
	else
	{
		// ������ �׼�
		WRFSMInfo fsmInfo(EWRFSM::Skill, DEF_CHEERS_SKILL_TID, 0, 0);
		pSuggester->DetermineFSMSignal(fsmInfo);
		
		// ������ �׼�
		if (pAnswer->GetCharacterType() == EWRCharacterType::NPC)
		{
			// + NPC�� ���� ȸ�������ϰ� ����. Delegate : RotateComplete(InSuggestAID, InAnswerAID)
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotationLerpToTarget(pSuggester, FWRRotationEndDelegate::CreateRaw(this, &WRCheersManager::RotateComplete, pSuggester->GetActorID(), pAnswer->GetActorID()));

			// + RotateComplete���� ó���ҰŸ� ����
			AWRCharacterNPC* CastedNPC = Cast<AWRCharacterNPC>(pAnswer);
			if (CastedNPC != nullptr)
				CastedNPC->OnEmotion(EWRNPCEmotion::Cheers, pSuggester);
		}
		else
		{
			// + NPC�� �ƴϸ� �ϴ� �׳� ȸ��
			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(pAnswer->GetActorLocation(), pSuggester->GetActorLocation());
			pAnswer->SetRotation(FRotator(pAnswer->GetActorRotation().Pitch, Rotation2RotateDir.Yaw, pAnswer->GetActorRotation().Roll));

			//pAnswer->DetermineFSMSignal(fsmInfo);
		}
	}
	
	WRCharacterManager::Get()->SetCheersCondition(InSuggestAID, (EWRCheersConditionType)InSuggesterCondition);
	WRCharacterManager::Get()->SetCheersCondition(InAnswerAID, (EWRCheersConditionType)InAnswerCondition);
}

//====================================================================================
// �ǹ� ��� ����
void WRCheersManager::Req_CancelCheers()
{
	if (CheersSuggesterAID == INVALID_AID)
		return;

	if (CheersSuggesterAID == WRCharacterManager::Get()->GetMyAID())
	{
		// Suggester�� ���� ������ �ƴ� ��ο��� ��� ����
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
		// Suggester���� ��� ����
		IMMessageSender::RequestCancelCheers(CheersSuggesterAID);		
	}	
}

void WRCheersManager::Ack_CancelCheers(const AID_t IN InTargetAID)
{
	
}

void WRCheersManager::Inf_CancelCheers(const AID_t IN InCancellerAID, uint8 IN InCancellerCondition, const AID_t IN InTargetAID, uint8 IN InTargetCondition, const AID_t IN InSuggesterAID)
{
	// ����ڰ� �� �׷� ������
	if (InCancellerAID == InSuggesterAID && InCancellerAID == CheersSuggesterAID)
	{
		// ����� ��
		if (InTargetAID == WRCharacterManager::Get()->GetMyAID())
		{
			// ��� ����
			ClearCheersInfo(true);
			OnEndCheers();
		}
		else if(InCancellerAID == WRCharacterManager::Get()->GetMyAID())
		{
			// ��� ����
			ClearCheersInfo(true);
		}
	}
	// ����� �� �׷� ������
	else if(InTargetAID == InSuggesterAID && InTargetAID == CheersSuggesterAID)
	{		
		// ���� �����
		if (InCancellerAID == WRCharacterManager::Get()->GetMyAID())
		{
			ClearCheersInfo(false); // ���� ����
			OnEndCheers();
		}
		// �ٸ� ����� �����
		else
		{			
			ChangeCheersAnswerState(InCancellerAID, EWRCheersState::AnswerNo, false);
			bool bAllReject = IsAllCheersAnswerNo();
			if (bAllReject)
			{
				// [ 2020-4-2 : kyu ] ��ҷ� ���� ��ΰ� ������ ��� ��� ���� ����
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
