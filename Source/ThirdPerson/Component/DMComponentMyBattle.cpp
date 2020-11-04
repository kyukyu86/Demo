// Fill out your copyright notice in the Description page of Project Settings.


#include "DMComponentMyBattle.h"
#include "DMComponentFSM.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../Manager/DMCharacterManager.h"
#include "../Data/CustomData/DMAttackTable.h"
#include "../Manager/DMFSMManager.h"
#include "../Manager/DMInputManager.h"
#include "../UI/Panel/DMUIPanel_Charge.h"


#define DEF_FIRST_ATTACK_ROW_NAME "1"

void UDMComponentMyBattle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bChargeState)
	{

	}
}

bool UDMComponentMyBattle::InputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	if (DMInputManager::Get()->IsIgnoreAction())
		return false;

	switch (InInputType)
	{
	case EDMInput::MouseLButton:
	{
		return MouseLButtonEvent(InInputType, InEventType);
	}
	break;

	case EDMInput::MouseRButton:
	{
		return MouseRButtonEvent(InInputType, InEventType);
	}
	break;
	}
	return false;
}

bool UDMComponentMyBattle::MouseLButtonEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	FDMFSMData CurrentFSMData;
	GetMyFSMData(CurrentFSMData);

	switch (CurrentFSMData.FSMType)
	{
	case EDMFSMType::Idle:
	{
		// 일반 공격
		FDMAttackTable* AttackTable = DMCharacterManager::Get()->GetAttackTable(DEF_FIRST_ATTACK_ROW_NAME);
		if (FDMAttackTable::IsValidInput(AttackTable, InInputType, InEventType))
		{
			DMFSMManager::Get()->DetermineFSM(DMCharacterManager::Get()->GetMyCharacter(), FDMFSMData(AttackTable));
		}
	}
	break;
	case EDMFSMType::AttackNormal:
	{
		// 연속 공격
		FDMAttackTable* LinkedAttackTable = FDMAttackTable::GetValidLinkedAttackTable(CurrentFSMData.AttackTable, InInputType, InEventType);
		if (LinkedAttackTable)
		{
			DMFSMManager::Get()->DetermineFSM(DMCharacterManager::Get()->GetMyCharacter(), FDMFSMData(LinkedAttackTable));
			if (LinkedAttackTable->Data.InputHoldMaxTime != 0.f)
			{
				bChargeState = true;

				auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
				{
					UDMUIPanel_Charge* pPanel = Cast<UDMUIPanel_Charge>(InObject);
					if (pPanel)
					{
						pPanel->SetGoalTime(LinkedAttackTable->Data.InputHoldMaxTime);
					}
				});
				FDMOpenWidgetInfo WidgetInfo(EDMPanelKind::Charge, GetOwner()->GetActorLocation());
				WidgetInfo.CompleteDelegate = AsyncCreateComplete;
				DMUIManager::Get()->OpenPanel(WidgetInfo);
			}
			else
			{
				bChargeState = false;
				DMUIManager::Get()->ClosePanel(EDMPanelKind::Charge);
			}
		}
		else
		{
			FDMAttackTable* AttackTable = DMCharacterManager::Get()->GetAttackTable(DEF_FIRST_ATTACK_ROW_NAME);
			if (FDMAttackTable::IsValidInput(AttackTable, InInputType, InEventType))
			{
				DMFSMManager::Get()->DetermineFSM(DMCharacterManager::Get()->GetMyCharacter(), FDMFSMData(AttackTable));
			}

			bChargeState = false;
			DMUIManager::Get()->ClosePanel(EDMPanelKind::Charge);
		}
	}
	break;
	case EDMFSMType::SkillNormal:
	{
		// 스킬 사용중 입력 불가
	}
	break;
	case EDMFSMType::Hitted:
	{
		// 피격중 입력 불가
	}
	break;	
	}

	return false;
}

bool UDMComponentMyBattle::MouseRButtonEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	FDMFSMData CurrentFSMData;
	GetMyFSMData(CurrentFSMData);

	switch (CurrentFSMData.FSMType)
	{
	case EDMFSMType::AttackNormal:
	{

	}
	break;
	case EDMFSMType::SkillNormal:
	{
		// 스킬 사용중 불가
	}
	break;
	case EDMFSMType::Hitted:
	{
		// 피격중 불가
	}
	break;
	}

	return false;
}

void UDMComponentMyBattle::GetMyFSMData(FDMFSMData& OUT OutData)
{
	ADMCharacterMine* pMine = Cast<ADMCharacterMine>(GetOwner());
	if (pMine == nullptr)
		return;

	UDMComponentFSM* ComponentFSM = pMine->GetComponent<UDMComponentFSM>();
	if (ComponentFSM == nullptr)
		return;

	ComponentFSM->GetCurrentFSMData(OutData);
}

bool UDMComponentMyBattle::IsIdleState()
{
	ADMCharacterMine* pMine = Cast<ADMCharacterMine>(GetOwner());
	if (pMine == nullptr)
		return false;

	UDMComponentFSM* ComponentFSM = pMine->GetComponent<UDMComponentFSM>();
	if (ComponentFSM == nullptr)
		return false;

	if (ComponentFSM->GetCurrentFSM() == nullptr)
		return true;

	return false;
}
