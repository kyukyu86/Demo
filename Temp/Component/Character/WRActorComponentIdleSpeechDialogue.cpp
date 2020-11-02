// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRActorComponentIdleSpeechDialogue.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRDialogueManager.h"
#include "Manager/WRCharacterManager.h"
#include "Utility/WRActorUtility.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Delegate.h"



void UWRActorComponentIdleSpeechDialogue::OnCreateComponent()
{
	Super::OnCreateComponent();

	LoopTime = 0.f;
	
	MyCharacterStartMoveDelegateHandle = WRCharacterManager::Get()->AddMyCharacterStartMoveDelegate(FSimpleDelegate::CreateUObject(this, &UWRActorComponentIdleSpeechDialogue::OnMyCharacterMoveStarted));
	MyCharacterStopMoveDelegateHandle = WRCharacterManager::Get()->AddMyCharacterStopMoveDelegate(FSimpleDelegate::CreateUObject(this, &UWRActorComponentIdleSpeechDialogue::OnMyCharacterMoveStoped));

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter && OwnerCharacter)
	{
		if (MyCharacter->IsInputPad())
		{
			SetEnableTick(true);
		}
		else if (WRActorUtility::GetDistance(OwnerCharacter, Cast<AWRCharacter>(MyCharacter)) < SpeechRange)
		{
			SetEnableTick(true);
		}
		else
		{
			SetEnableTick(false);
		}
	}
	

}

void UWRActorComponentIdleSpeechDialogue::OnDestroyComponent()
{
	if (WRCharacterManager::Get())
	{
		WRCharacterManager::Get()->RemoveMyCharacterStartMoveDelegate(MyCharacterStartMoveDelegateHandle);
		WRCharacterManager::Get()->RemoveMyCharacterStopMoveDelegate(MyCharacterStopMoveDelegateHandle);
	}

	Super::OnDestroyComponent();
}

void UWRActorComponentIdleSpeechDialogue::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LoopTime += DeltaTime;

	if (LoopTime > WRConsoleManager::Get()->GetInt(EWRConsole::UI_IdleSpeechTime))
	{
		AWRCharacter* OwnerCharacter = GetOwnerCharacter();
		if (OwnerCharacter == nullptr)
			return;
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;
		
		if(WRActorUtility::GetDistance(OwnerCharacter, Cast<AWRCharacter>(MyCharacter)) < SpeechRange)
		{
			RegisterDialogue();
			LoopTime = 0.f;
		}
	}
}

void UWRActorComponentIdleSpeechDialogue::SetData(const int32 IN InSpeechRange, const TArray <WRTableID>& IN InDialogueTIDs)
{
	SpeechRange = InSpeechRange;
	DialogueTIDs = InDialogueTIDs;
}

void UWRActorComponentIdleSpeechDialogue::OnMyCharacterMoveStarted()
{
	SetEnableTick(true);
}

void UWRActorComponentIdleSpeechDialogue::OnMyCharacterMoveStoped()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
	{
		SetEnableTick(false);
		return;
	}
		
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		SetEnableTick(false);
		return;
	}

	if (WRActorUtility::GetDistance(OwnerCharacter, Cast<AWRCharacter>(MyCharacter)) < SpeechRange)
	{
		SetEnableTick(true);
	}
	else
	{
		SetEnableTick(false);
	}
}

void UWRActorComponentIdleSpeechDialogue::RegisterDialogue()
{
	if (DialogueTIDs.Num() == 0)
		return;

	int32 RandIndex = FMath::Rand() % DialogueTIDs.Num();
	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::NpcIdleSpeech, DialogueTIDs[RandIndex]);
}
