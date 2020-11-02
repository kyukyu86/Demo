

#include "WRObjectInteractionManager.h"
#include "WRInputSystemManager.h"
#include "WRCharacterManager.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterMine.h"

#include "UI/Base/WRUIPanel.h"
#include "UI/Panel/WRUIPanel_IOPopup.h"
#include "UI/Panel/WRUIPanel_TargetName.h"

#include "Network/IMMessageSender.h"

#include "Component/Character/WRActorComponentInteractionObject.h"
#include "Actor/EnvActor/Zipline/WREnvZipline.h"
#include "Actor/EnvActor/Ladder/WRLadder.h"
#include "Component/Character/WRActorComponentLadder.h"
#include "Component/Character/WRActorComponentInteraction.h"




// For Cpp
DEFINE_LOG_CATEGORY(ObjectInteractionManager)


WRObjectInteractionManager::WRObjectInteractionManager()
{

}

WRObjectInteractionManager::~WRObjectInteractionManager()
{

}

// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRObjectInteractionManager::OnInitialize()
{
	PacketWait.Add(EWRObjectInteractionPacketWait::Begin, false);
	PacketWait.Add(EWRObjectInteractionPacketWait::Stop, false);
	PacketWait.Add(EWRObjectInteractionPacketWait::End, false);
}

// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRObjectInteractionManager::OnShutdown()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
void WRObjectInteractionManager::OnLoadLevelStart()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
void WRObjectInteractionManager::OnLoadLevelComplete()
{

}

// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
void WRObjectInteractionManager::OnRenderStart()
{
        
}

bool WRObjectInteractionManager::OnTick(float InDeltaTime)
{
	
	return true;
}

bool WRObjectInteractionManager::IsPacketWait(EWRObjectInteractionPacketWait IN InType)
{
	if (PacketWait.Contains(InType))
	{
		return PacketWait[InType];
	}
	return false;
}

void WRObjectInteractionManager::SetPacketWait(EWRObjectInteractionPacketWait IN InType, const bool IN InSet)
{
	if (PacketWait.Contains(InType))
	{
		PacketWait[InType] = InSet;
	}
}

//====================================================================================
//
//====================================================================================

void WRObjectInteractionManager::SetSingleInteractionTarget(class AWRCharacter* IN InTarget)
{
	if (InTarget == nullptr)
	{
		SingleInteractionTargetAID = INVALID_AID;
	}
	else
	{
		SingleInteractionTargetAID = InTarget->GetActorID();
	}
}

class AWRCharacter* WRObjectInteractionManager::GetSingleInteractionTarget()
{
	if (SingleInteractionTargetAID == INVALID_AID)
		return nullptr;

	AWRCharacter* pSingleInteractionTarget = WRCharacterManager::Get()->FindbyAID(SingleInteractionTargetAID);
	if (pSingleInteractionTarget == nullptr)
	{
		SingleInteractionTargetAID = INVALID_AID;
	}
	return pSingleInteractionTarget;
}

void WRObjectInteractionManager::SetInteractionTarget(const bool IN InMulti, class AWRCharacter* IN InTarget)
{
	if (InMulti)
	{
		MultiInteractionTargets.Add(InTarget);
	}
	else
	{
		SingleInteractionTargetAID = InTarget->GetActorID();
	}
}

void WRObjectInteractionManager::ReleaseInteractionTarget(const bool IN InMulti, class AWRCharacter* IN InTarget)
{
	if (InMulti)
	{
		MultiInteractionTargets.Remove(InTarget);
	}
	else
	{
		if(SingleInteractionTargetAID == InTarget->GetActorID())
			SingleInteractionTargetAID = INVALID_AID;
	}
}

bool WRObjectInteractionManager::IsInteractionTarget(AWRCharacter* IN InTarget)
{
	// Check Single
	if (SingleInteractionTargetAID == InTarget->GetActorID())
		return true;

	// Check Multi
	for (AWRCharacter* Target : MultiInteractionTargets)
	{
		if (Target == InTarget)
			return true;
	}
	return false;
}

bool WRObjectInteractionManager::IsInteractingWithTarget()
{
	// Check Single
	AWRCharacter* pSingleInteractionTarget = GetSingleInteractionTarget();
	if(pSingleInteractionTarget != nullptr && pSingleInteractionTarget->IsInteracting())
		return true;

	// Check Multi
	for (AWRCharacter* Target : MultiInteractionTargets)
	{
		if (Target != nullptr && Target->IsInteracting())
			return true;
	}
	return false;
}

//====================================================================================
//
//====================================================================================

bool WRObjectInteractionManager::OnInputPressed(const EWRInput InInputType)
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return false;

	AWRCharacter* pTarget = GetSingleInteractionTarget();
	if (pTarget == nullptr)
	{
		pTarget = const_cast<AWRCharacter*>(WRCharacterManager::Get()->GetMyTarget());
	}

	if (pTarget == nullptr)
		return false;

	// ��󿡰� �Է� ����
	return pTarget->OnPressedEvent(InInputType);
}

bool WRObjectInteractionManager::OnInputReleased(const EWRInput InInputType)
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return false;

	AWRCharacter* pTarget = GetSingleInteractionTarget();
	if (pTarget == nullptr)
	{
		pTarget = const_cast<AWRCharacter*>(WRCharacterManager::Get()->GetMyTarget());
	}

	if (pTarget == nullptr)
		return false;

	// ��󿡰� �Է� ����
	return pTarget->OnReleasedEvent(InInputType);
}

void WRObjectInteractionManager::OnMoved()
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return;

	CancelLadder();

	// [ 2020-7-23 : kyu ] �̵��� ���� ����
	UWRActorComponentInteraction* pInteractionComp = WRCharacterManager::Get()->GetMyCharacter()->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->SetOccupiedIO(nullptr);
	}
}

//====================================================================================
//	Packet
//====================================================================================

// Interaction Start

void WRObjectInteractionManager::OnReq_BeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType)
{
	if (IsPacketWait(EWRObjectInteractionPacketWait::Begin))
		return;

	SetPacketWait(EWRObjectInteractionPacketWait::Begin, true);

	// [ 2019-12-4 : kyu ] ġƮ��
	if (IsCheatedObject(InObjectID))
	{
		OnInf_BeginInteraction(InCharID, InObjectID, InInvokeInteractionType);
	}
	else
	{
		IMMessageSender::RequestBeginInteraction(InCharID, InObjectID, (uint8)InInvokeInteractionType);
	}
}

void WRObjectInteractionManager::OnInf_BeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType)
{
	AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (pPlayer == nullptr)
		return;

	if (pPlayer->IsMe())
	{
		SetPacketWait(EWRObjectInteractionPacketWait::Begin, false);
	}

	AWRCharacterIO* pObject = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterIO>(InObjectID);
	if (pObject == nullptr)
		return;

	UWRActorComponentInteractionObject* pComponent = pObject->GetComponent<UWRActorComponentInteractionObject>();
	if (pComponent == nullptr)
		return;
	   
	pComponent->OnInf_InteractionStart(InCharID, InInvokeInteractionType);
}

// Interaction Stop

void WRObjectInteractionManager::OnReq_StopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID)
{
	if (IsPacketWait(EWRObjectInteractionPacketWait::Stop))
		return;

	SetPacketWait(EWRObjectInteractionPacketWait::Stop, true);

	// [ 2019-12-4 : kyu ] ġƮ��
	if (IsCheatedObject(InObjectID))
	{
		OnInf_StopInteraction(InCharID, InObjectID);
	}
	else
	{
		IMMessageSender::RequestStopInteraction(InCharID, InObjectID);
	}
}

void WRObjectInteractionManager::OnInf_StopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID)
{
	AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (pPlayer == nullptr)
		return;

	if (pPlayer->IsMe())
	{
		SetPacketWait(EWRObjectInteractionPacketWait::Stop, false);
	}

	AWRCharacterIO* pObject = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterIO>(InObjectID);
	if (pObject == nullptr)
		return;

	UWRActorComponentInteractionObject* pComponent = pObject->GetComponent<UWRActorComponentInteractionObject>();
	if (pComponent == nullptr)
		return;

	pComponent->OnInf_InteractionStop(InCharID);
}

// Interaction Finish

void WRObjectInteractionManager::OnReq_EndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType)
{
	if (IsPacketWait(EWRObjectInteractionPacketWait::End))
		return;

	SetPacketWait(EWRObjectInteractionPacketWait::End, true);

	// [ 2019-12-4 : kyu ] ġƮ��
	if (IsCheatedObject(InObjectID))
	{
		AWRCharacterIO* pObject = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterIO>(InObjectID);
		UWRActorComponentInteractionObject* pComponent = pObject->GetComponent<UWRActorComponentInteractionObject>();
		pComponent->AddResultCount(InInvokeInteractionType);
		OnInf_EndInteraction(InCharID, InObjectID, pComponent->GetDirectRetainCount(), pComponent->GetHookRetainCount());
	}
	else
	{
		IMMessageSender::RequestEndInteraction(InCharID, InObjectID, (uint8)InInvokeInteractionType);
	}
}

void WRObjectInteractionManager::OnInf_EndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint16 IN InRetainCount, const uint8 IN InInteractionType)
{
	AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (pPlayer == nullptr)
		return;

	if (pPlayer->IsMe())
	{
		SetPacketWait(EWRObjectInteractionPacketWait::End, false);
	}

	AWRCharacterIO* pObject = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterIO>(InObjectID);
	if (pObject == nullptr)
		return;

	UWRActorComponentInteractionObject* pComponent = pObject->GetComponent<UWRActorComponentInteractionObject>();
	if (pComponent == nullptr)
		return;

	pComponent->OnInf_InteractionEnd(InCharID, InObjectID, InRetainCount, InInteractionType);
}

//====================================================================================
//
//====================================================================================

void WRObjectInteractionManager::CurInteractionSkip()
{
	AWRCharacter* pSingleInteractionTarget = GetSingleInteractionTarget();
	if (pSingleInteractionTarget == nullptr)
		return;

	UWRActorComponentInteractionObject* pComponent = pSingleInteractionTarget->GetComponent<UWRActorComponentInteractionObject>();
	if (pComponent == nullptr)
		return;

	pComponent->OnSkipToTest();
}

bool WRObjectInteractionManager::IsCheatedObject(const AID_t IN InObjectID)
{
	AWRCharacterIO* pObject = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterIO>(InObjectID);
	if (pObject == nullptr)
		return false;

	return pObject->IsCheatedObject();
}

//====================================================================================
//	Zipline
//====================================================================================

// void WRObjectInteractionManager::AddZipline(AWREnvZipline* IN InZipline)
// {
// 	Ziplines.Add(InZipline->UniqueKey, InZipline);
// }
// 
// void WRObjectInteractionManager::AttachZiplineHandle(AWRCharacterIO* IN InZiplineHandle)
// {
// 	AWREnvZipline* FoundZipline = Ziplines.FindRef(InZiplineHandle->GetZiplineUniqueKey());
// 	if (FoundZipline == nullptr)
// 		return;
// 
// 	FoundZipline->AttachZiplineHandle(InZiplineHandle);
// }
// 
// void WRObjectInteractionManager::ActivateZipline(class AWRCharacterIO* IN InZiplineHandle)
// {
// 	AWREnvZipline* FoundZipline = Ziplines.FindRef(InZiplineHandle->GetZiplineUniqueKey());
// 	if (FoundZipline == nullptr)
// 		return;
// 
// 	FoundZipline->ActiveZipline();
// }

//====================================================================================
// Ladder
//====================================================================================

bool WRObjectInteractionManager::InputEventToInteraction(EWRInput InInputType, EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{
		bInputEventActivate = false;

		// ��ٸ� üũ
		if (DetermineLadder(InInputType))
			bInputEventActivate = true;

		// ���ͷ��� üũ
		if (OnInputPressed(InInputType))
			bInputEventActivate = true;

		return bInputEventActivate;
	}
	break;
	case IE_Released:
	{
		bool bCheck = bInputEventActivate;
		bInputEventActivate = false;

		//if (IsInteractingWithTarget())
		if(OnInputReleased(InInputType))
		{

		}
		else
		{
			AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
			if (MyChar->IsValidLowLevel() == false)
				return false;

			if (MyChar->GetTargetAID() != INVALID_AID)
			{
				AWRCharacter* pFoundCharacter = WRCharacterManager::Get()->FindbyAID(MyChar->GetTargetAID());
				if (pFoundCharacter && pFoundCharacter->IsValidLowLevel() && pFoundCharacter->IsIO())
				{
					pFoundCharacter->OnReleasedEvent(InInputType);
				}
			}
		}

		return bCheck;
	}
	break;
	}

	return false;
}

bool WRObjectInteractionManager::DetermineLadder(EWRInput InInputType)
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return false;

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return false;

	UWRActorComponentLadder* pLadderComp = pMine->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return false;

	return pLadderComp->DetermineLadder(InInputType, GetLadderDirection());
}

bool WRObjectInteractionManager::CheckLadder(EWRInput InInputType)
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return false;

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return false;
	
	UWRActorComponentLadder* pLadderComp = pMine->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return false;

	if (pLadderComp->IsUsingLadder())
		return true;

	return false;
}

void WRObjectInteractionManager::CancelLadder()
{
	if (WRCharacterManager::Get()->GetMyCharacter() == nullptr)
		return;

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = pMine->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->CancelLadder();
}

EWRLadderDirection WRObjectInteractionManager::GetLadderDirection()
{
	FRotator rot = WRInputSystemManager::Get()->GetHMDRotation();
	if (rot.Pitch >= 0)
	{
		return EWRLadderDirection::Up;
	}
	else
	{
		return EWRLadderDirection::Down;
	}
	return EWRLadderDirection::None;
}