// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMCombatBase.h"
#include "../../Manager/DMCharacterManager.h"
#include "../../UI/Panel/DMUIPanel_SkillNotice.h"
#include "../../Actor/Character/Base/DMCharacterBase.h"


static bool bSkillNotice = false;


DMFSMCombatBase::DMFSMCombatBase()
{
}

DMFSMCombatBase::~DMFSMCombatBase()
{
}

void DMFSMCombatBase::CheckSkillNotice(const FDMFSMData& IN InFSMData)
{
	if (InFSMData.AttackTable)
	{
		for (auto AttackRowName : InFSMData.AttackTable->LinkedAttackRowName)
		{
			FDMAttackTable* AttackTable = DMCharacterManager::Get()->GetAttackTable(AttackRowName);
			if (AttackTable && AttackTable->Data.InputHoldMaxTime != 0.f)
			{
				bSkillNotice = true;

				switch (AttackTable->Data.NeedInputKey)
				{
				case EDMInput::MouseLButton:
				{
					SkillNoticeType = EDMSkillNotice::MouseL;
				}
				break;
				case EDMInput::MouseRButton:
				{
					SkillNoticeType = EDMSkillNotice::MouseR;
				}
				break;
				}

				auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
				{
					UDMUIPanel_SkillNotice* pPanel = Cast<UDMUIPanel_SkillNotice>(InObject);
					if (pPanel)
					{
						EDMSkillNotice NoticeType = EDMSkillNotice::MouseL;

						pPanel->SetType(SkillNoticeType);
					}
				});
				FDMOpenWidgetInfo WidgetInfo(EDMPanelKind::SkillNotice, GetOwner()->GetActorLocation());
				WidgetInfo.CompleteDelegate = AsyncCreateComplete;
				DMUIManager::Get()->OpenPanel(WidgetInfo);

				return;
			}
		}
	}

	if (bSkillNotice)
	{
		bSkillNotice = false;
		UDMUIPanel_SkillNotice* Panel = DMUIManager::Get()->FindPanel<UDMUIPanel_SkillNotice>(EDMPanelKind::SkillNotice);
		if (Panel)
		{
			Panel->ShowOff();
		}
	}
}

void DMFSMCombatBase::OnEnter(const FDMFSMData IN InFSMData)
{
	DMFSMBase::OnEnter(InFSMData);

	CheckSkillNotice(InFSMData);
}

void DMFSMCombatBase::OnReEnter(const FDMFSMData IN InFSMData)
{
	DMFSMBase::OnReEnter(InFSMData);

	CheckSkillNotice(InFSMData);
}

void DMFSMCombatBase::OnExit()
{
	if (bSkillNotice)
	{
		bSkillNotice = false;
		UDMUIPanel_SkillNotice* Panel = DMUIManager::Get()->FindPanel<UDMUIPanel_SkillNotice>(EDMPanelKind::SkillNotice);
		if(Panel)
		{
			Panel->ShowOff();
		}
	}

	DMFSMBase::OnExit();
}

void DMFSMCombatBase::OnTick()
{
	DMFSMBase::OnTick();

}
