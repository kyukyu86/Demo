// Fill out your copyright notice in the Description page of Project Settings.


#include "DMDebugManager.h"
#include "DMCharacterManager.h"
#include "DMUIManager.h"

#include "../GameInstance/DMGameInstance.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../UI/Panel/DMUIPanel_Debug.h"
#include "../Component/DMActorComponentFSM.h"
#include "../Util/DMEnumUtil.h"
#include "../Actor/FunctionComponent/DMInteractionComponent.h"


DMDebugManager::DMDebugManager()
{
	
}

DMDebugManager::~DMDebugManager()
{

}

void DMDebugManager::OnInit()
{

}

void DMDebugManager::OnShutdown()
{
	
}

bool DMDebugManager::OnTick(float InDeltaTime)
{	
	UDMUIPanel_Debug* DebugPanel = DMUIManager::Get()->FindPanel<UDMUIPanel_Debug>(EDMPanelKind::Debug);
	if (DebugPanel == nullptr)
		return true;

	FString strDebug = "";
	MakeDebugString(strDebug);
	DebugPanel->SetDebugText(strDebug);

	return true;
}

void DMDebugManager::MakeDebugString(FString& OUT OutString)
{
	OutString.Empty();
	FString AddString = "";

	ADMCharacterMine* pMine = DMCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return;

	// FSM
	UDMActorComponentFSM* FSMComponent = pMine->GetComponent<UDMActorComponentFSM>();
	if (FSMComponent)
	{
		AddString = DMEnumUtil::EnumToString<EDMFSMType>("EDMFSMType", FSMComponent->GetFSMType());
		OutString += "FSM : " + AddString + "\n";

		AddString = DMEnumUtil::EnumToString<EDMIdleType>("EDMIdleType", FSMComponent->GetIdleType());
		OutString += "Idle : " + AddString + "\n";
	}

	// Target
	AddString = "TargetComp : ";
	UDMInteractionComponent* TargetComponent = pMine->GetTargetedComponent();
	if (TargetComponent)
	{
		AActor* ParentActor = TargetComponent->GetParentActor();
		if (ParentActor)
		{
			AddString += ParentActor->GetActorLabel();
		}
		else
		{
			AddString += "Invalid ParentActor";
		}
	}
	else
	{
		AddString += "nullptr";
	}
	AddString += "\n";
	OutString += AddString;
}