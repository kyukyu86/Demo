// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentMic.h"
#include "Utility/WRActorUtility.h"
#include "Table/WRMicTable.h"
#include "Table/Base/WRTableManager.h"
#include "Network/IMMessageSender.h"
#include "Table/WRSocialTable.h"
#include "Enum/EWRCharacter.h"
#include "Utility/WRUIUtility.h"
#include "Actor/Character/WRCharacter.h"

void UWRActorComponentMic::OnCreateComponent()
{
	Super::OnCreateComponent();

}

void UWRActorComponentMic::OnDestroyComponent()
{
	WRActorUtility::ClearTimer(RandomAnimationTimerHandle);

	Super::OnDestroyComponent();
}

void UWRActorComponentMic::ActivateMic(const AID_t IN InMicAID)
{
	if (CheckActivateConditions() == false)
		return;

	MicActivating = true;
	MicAID = InMicAID;

	// Eco On

	// Volume Up

	//====================================================================================
	// for Mine
	if (GetOwnerCharacter()->IsMe() == false)
		return;

	FWRMicTable* MicTable = WRTableManager::Get()->FindRow<FWRMicTable>(1);
	if (MicTable == nullptr)
		return;

	// Random Animation
	int32 RandomTime = FMath::RandRange(MicTable->RandomAniTimeMin, MicTable->RandomAniTimeMax);
	float ConvertedRandomTime = (float)RandomTime / 1000.f;
	WRActorUtility::SetTimer(RandomAnimationTimerHandle, FTimerDelegate::CreateUObject(this, &UWRActorComponentMic::PlayRandomAnimation), ConvertedRandomTime, false);
}

void UWRActorComponentMic::DeactivateMic(const AID_t IN InMicAID)
{
	MicActivating = false;
	MicAID = INVALID_AID;

	// Eco Off

	// Volume Down

	//====================================================================================
	// for Mine
	if (GetOwnerCharacter()->IsMe() == false)
		return;

	// Random Animation
	WRActorUtility::ClearTimer(RandomAnimationTimerHandle);
}

bool UWRActorComponentMic::OnAnimation(const TID_t IN InAnimationTID)
{
	FWRSocialAnimationTable* AnimationTable = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(InAnimationTID);
	if (AnimationTable == nullptr)
		return false;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::SocialAnimation;
	if (GetOwnerCharacter()->IsMe())
	{
		FSMInfo.MontageTID = AnimationTable->BaseMontageTID;
		FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(AnimationTable->MontageName);
	}
	else
	{
		FSMInfo.MontageTID = AnimationTable->BaseSubMontageTID;
		FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(AnimationTable->SubMontageName);
	}
	GetOwnerCharacter()->DetermineFSMSignal(FSMInfo);
	return true;
}

bool UWRActorComponentMic::CheckActivateConditions()
{
	return true;
}

void UWRActorComponentMic::PlayRandomAnimation()
{
	FWRMicTable* MicTable = WRTableManager::Get()->FindRow<FWRMicTable>(1);
	if (MicTable == nullptr)
		return;

	int32 Index = FMath::RandRange(0, FMath::Max(0, MicTable->RandomAniTID.Num() - 1));
	WRTableID AnimationTID = MicTable->RandomAniTID[Index];
	if (OnAnimation(AnimationTID) == false)
		return;

	//====================================================================================
	// for Mine	
	if (GetOwnerCharacter()->IsMe())
	{
		// Request
		IMMessageSender::RequestMicAnimation(AnimationTID);

		// Random Animation Cooltime
		int32 RandomTime = FMath::RandRange(MicTable->RandomAniTimeMin, MicTable->RandomAniTimeMax);
		float ConvertedRandomTime = (float)RandomTime / 1000.f;
		WRActorUtility::SetTimer(RandomAnimationTimerHandle, FTimerDelegate::CreateUObject(this, &UWRActorComponentMic::PlayRandomAnimation), ConvertedRandomTime, false);
	}
}
