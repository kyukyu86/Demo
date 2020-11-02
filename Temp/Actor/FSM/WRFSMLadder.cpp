#include "WRFSMLadder.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRPlayerTable.h"

#include "../Character/WRCharacterPlayer.h"
#include "../Character/WRCharacterMine.h"
#include "../EnvActor/Ladder/WRLadder.h"
#include "../Animation/WRAnimInstance.h"

#include "Component/Character/WRActorComponentLadder.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"

#include "Utility/WRActorUtility.h"

#include "Manager/WRInputSystemManager.h"
#include "Manager/WRObjectInteractionManager.h"


#define DEF_MY_LADDER_MONTAGE_TID 1010

WRFSMLadder::WRFSMLadder()
	: WRFSMBase(EWRFSM::Ladder, EWRFSM::Idle)
{

}

WRFSMLadder::~WRFSMLadder()
{

}

void WRFSMLadder::Begin(const WRFSMInfo& FSMInfo)
{
	if (FSMInfo.VoidPtr == nullptr)
		return;

	FWRLadderData* pData = (FWRLadderData*)(const_cast<void*>(FSMInfo.VoidPtr));
	if (pData == nullptr)
		return;

	if (FSMInfo.OwnerCharacter == nullptr)
		return;

	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(FSMInfo.OwnerCharacter);
	if (CastedPlayer == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = CastedPlayer->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->OnFSMLadderBegin(*pData);

	ProcessAnimation(FSMInfo);	
}

void WRFSMLadder::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(OwnerCharacter);
	if (CastedPlayer == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = CastedPlayer->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->OnFSMLadderTick(DeltaTime);
}

void WRFSMLadder::End(const WRFSMInfo& InReservedFSM)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(InReservedFSM.OwnerCharacter);
	if (CastedPlayer == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = CastedPlayer->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->OnFSMLadderEnd();
}

void WRFSMLadder::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMLadder::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMLadder::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
