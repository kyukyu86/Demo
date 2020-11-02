#pragma once

#include "CoreMinimal.h"
#include "WRUIManager.h"
#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRCharacter.h"
#include "Engine/EngineBaseTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Component/Character/WRActorComponentInteractionObject.h"
#include "Network/Share/Define/Define_Type.h"


enum class EWRObjectInteractionPacketWait : uint8
{
	Begin,
	Stop,
	End,
};

enum class EWRLadderDirection : uint8
{
	None,
	Up,
	Down,
};

enum class EWRLadderState : uint8
{
	None,
	Start,
	Move,
	Finish,
};

struct FWRLadderHandle
{
	FWRLadderHandle()
	{

	}
	class UStaticMeshComponent* HandleComponent = nullptr;
	class AWRLadder* ParentLadder = nullptr;
	EWRLadderHand HandType = EWRLadderHand::None;
	bool bTop = false;
	int32 nIndex = 0;

	class AWRLadder* GetParentLadder() { return ParentLadder; }
	FVector GetLocation()
	{
		if (HandleComponent == nullptr)
			return FVector::ZeroVector;
		return HandleComponent->GetComponentToWorld().GetLocation();
	}
	FVector GetForwardVector()
	{
		if (HandleComponent == nullptr)
			return FVector::ZeroVector;
		return HandleComponent->GetForwardVector();
	}
	FVector GetRightVector()
	{
		if (HandleComponent == nullptr)
			return FVector::ZeroVector;
		return HandleComponent->GetRightVector();
	}
	FTransform GetTransform()
	{
		if (HandleComponent == nullptr)
			return FTransform::Identity;
		return HandleComponent->GetComponentToWorld();
	}
};

struct FWRLadderData
{
	FWRLadderData() : State(EWRLadderState::None), TargetHandle(nullptr), Direction(EWRLadderDirection::None), Hand(EWRLadderHand::None), LadderHandleIndex(0)
	{
		Transform.SetIdentity();
	}
	EWRLadderState State;
	FWRLadderHandle* TargetHandle;
	EWRLadderDirection Direction;
	EWRLadderHand Hand;
	int32 LadderHandleIndex;
	FTransform Transform;
	void Reset()
	{
		State = EWRLadderState::None;
		TargetHandle = nullptr;
		Direction = EWRLadderDirection::None;
		Hand = EWRLadderHand::None;
		LadderHandleIndex = 0;
		Transform.SetIdentity();
	}
};

/**
*
*/
class WR_API WRObjectInteractionManager : public WRSingleton<WRObjectInteractionManager>
{
	CDECLARE_MANAGER(WRObjectInteractionManager);
	
private:
	WRObjectInteractionManager();
	virtual ~WRObjectInteractionManager();
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual bool OnTick(float InDeltaTime) override;
	bool IsPacketWait(EWRObjectInteractionPacketWait IN InType);
	void SetPacketWait(EWRObjectInteractionPacketWait IN InType, const bool IN InSet);
	
	// Type
	FORCEINLINE void SetInteractionType(const EWRIOInteractionType IN InType) { eInteractionType = InType; }
	FORCEINLINE EWRIOInteractionType GetInteractionType() { return eInteractionType; }
	// Target
	void SetSingleInteractionTarget(class AWRCharacter* IN InTarget);
	class AWRCharacter* GetSingleInteractionTarget();
	void SetInteractionTarget(const bool IN InMulti, class AWRCharacter* IN InTarget);
	void ReleaseInteractionTarget(const bool IN InMulti, class AWRCharacter* IN InTarget);
	bool IsInteractionTarget(class AWRCharacter* IN InTarget);
	bool IsInteractingWithTarget();
	// Input
	bool OnInputPressed(const EWRInput InInputType);
	bool OnInputReleased(const EWRInput InInputType);
	void OnMoved();

	//====================================================================================	
	// 인터렉션 시작
	void OnReq_BeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType);
	void OnInf_BeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType);

	// 인터렉션 중지
	void OnReq_StopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID);
	void OnInf_StopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID);

	// 인터렉션 완료
	void OnReq_EndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const EWRIOInteractionType IN InInvokeInteractionType);
	void OnInf_EndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint16 IN InRetainCount, const uint8 IN InInteractionType);

	//====================================================================================

	void CurInteractionSkip();
	bool InputEventToInteraction(EWRInput InInputType, EInputEvent InEventType);

	// for Cheat
	bool IsCheatedObject(const AID_t IN InObjectID);

private:
	EWRIOInteractionType eInteractionType = EWRIOInteractionType::None;
	AID_t SingleInteractionTargetAID = INVALID_AID;
	class TArray<AWRCharacter*> MultiInteractionTargets;
	   	 
	TMap<EWRObjectInteractionPacketWait, bool> PacketWait;
	bool bInputEventActivate = false;

	//====================================================================================
	// Zipline
// public:
// 	void AddZipline(class AWREnvZipline* IN InZipline);
// 	void AttachZiplineHandle(class AWRCharacterIO* IN InZiplineHandle);
// 	void ActivateZipline(class AWRCharacterIO* IN InZiplineHandle);
// private:
// 	TMap<int32, class AWREnvZipline*> Ziplines;

	//====================================================================================
	// Ladder
public:
	bool DetermineLadder(EWRInput InInputType);
	bool CheckLadder(EWRInput InInputType);
	void CancelLadder();
	EWRLadderDirection GetLadderDirection();

};


// For Header
DECLARE_LOG_CATEGORY_EXTERN(ObjectInteractionManager, Display, All);
