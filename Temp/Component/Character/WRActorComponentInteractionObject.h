// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"

#include "Enum/EWRUIEnum.h"
#include "Enum/EWRCharacter.h"

#include "Component/Base/WRActorComponent.h"

#include "Table/WRObjectTable.h"
#include "GameFramework/PlayerController.h"
#include "Network/Share/Define/Define_Actor.h"
#include "WRActorComponentConnectbyCable.h"
#include "Manager/WRAsyncLoadUObjectManager.h"

#include "WRActorComponentInteractionObject.generated.h"


/*
 enum class EWRInteractionType : uint8
 {
	None,
	Direct,
	Hook,
 };
 enum class EWRInteractionInput : uint8
 {
	None,
	Key,
	Gyro,
 };
 enum class EWRInteractionResult : uint8
 {
	None,
	Drop,
	Trigger,
 };
 enum class EWRInteractionResultAfter : uint8
 {
	None,
	Retain,
	Delete,
	RetainHide,
 };
 struct FWRDirection
 {
	int32	MontageTID;
	FString AnimSection;
	TSoftObjectPtr<class UAnimSequenceBase> AnimAsset;
	TSoftClassPtr<class AWRVFX>				VFX;
	TSoftObjectPtr<class USoundBase>		SFX;

	class AWRVFX* pFX;
	class UAudioComponent* pAudioComp;
 };
 struct FWRDirectionForm
 {
	FWRDirection ObjectProgressDirection;			// ������Ʈ�� ���ͷ��� ���� ����
	FWRDirection CharacterProgressDirection;		// �ɸ����� ���ͷ��� ���� ����
	EWRInteractionResult ProgressAction;			// ���ͷ��� �� �׼� (��� or Ʈ����)
	int32 ProgressActionID;							// �ش� ID
	float ProgressDirectionFinishTime;				// ���ͷ��� ���� ���� ���� �ð�
	bool bCharacterDependentOnProgress;				// �ɸ��Ͱ� ���ͷ��� ���� ���⿡ �������ΰ�? (�ܹ�/�ݺ��� üũ)
 };
 struct FWRInteractionStep
 {
	EWRInteractionInput InputType;					// Ű�Է� or ���̷�

	EKeys PressKey;									// Ű
	float fPressGoalTime;							// �����ð�
	TSoftObjectPtr<class UTexture2D> KeyIcon;		// Ű ������
	FString KeyDescription;							// Ű ����

	FVector GyroAxis;								// ���̷� ��Ī ��
	TSoftObjectPtr<class UTexture2D> GyroIcon;		// ���̷� ������
	FString GyroDescription;						// ���̷� ����

	FWRDirectionForm ProgressDirection;				// ���ͷ��� ���� ����
	FWRDirectionForm ResultDirection;				// ���ͷ��� ��� ����

	EWRInteractionResultAfter ResultAfter;			// ���ͷ��� ��� ������Ʈ ���� (���� or ���� or ����)
	int32 RetainCount;								// ���� Ƚ��

	bool bValid;
 };
 struct FWRInteractionSelect
 {
	TArray<FWRInteractionStep> Steps;				// �� ���������� ������ ���� �� �ִ�
	uint16 CurStep;									// ���� ����

	bool bValid;
 };
 struct FWRInteractionForm
 {
	EWRInteractionType InteractionType;				// ����, ����
	int32 ConditionQuestProgress;					// ����Ʈ ���� ����

	TArray<FWRInteractionSelect> Interactions;		// ���ͷ��� ������ (�� �������� ������ �ִ�)
	FWRInteractionSelect* SelectedInteraction;		// ���õ� ������

	bool bValid;
 };
struct FWRInteraction
{
	FWRDirection CreateDirection;					// ������ ����
	FWRDirection DestroyDirection;					// ������ ����

	TArray<FWRInteractionForm> Interactions;		// ���ͷ��� ����
	FWRInteractionForm* SelectedInteraction;		// ���õ� ���ͷ���

	bool bValid;
};
*/


UENUM(BlueprintType)
enum class EWRObjectUIType : uint8
{
	FixedLocation,
	FixedLocationAndBillboard,
	AttachChar,
	AttachCharAndBillboard,
	RotateToMeAndBillboard,
	RotateAxisZToMeAndBillboard,
};

UENUM(BlueprintType)
enum class EWRIOInteractionType : uint8
{
	None,
	DirectInteraction,
	HookInteraction,
};

UENUM(BlueprintType)
enum class EWRIODirectionType : uint8
{
	None,
	Creation,
	Deletion,
	ObjectProgress,
	ObjectInteraction,
	ObjecGyroStart,
	CharacterProgress,
	CharacterInteraction,
	CharacterGyroStart,
	HookInteraction,
	HookArrival,
};

UENUM(BlueprintType)
enum class EWRInteractionGroup : uint8
{
	None,
	Creation,
	Deletion,
	DirectProgress,
	DirectInteraction,
	DirectInteractionFinish,
	Gyro,
	HookInteraction,
	HookInteractionFinish,
	HookArrival,
};

UENUM(BlueprintType)
enum class EWRInteractionIKType : uint8
{
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EWRIOGyroAction : uint8
{
	None,
	Pull,
	Push,
	Angle,
};

UENUM(BlueprintType)
enum class EWRIOGyroAxis : uint8
{
	None,
	X,
	Y,
	Z,
};

USTRUCT(BlueprintType)
struct FWRIODirection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		int32	MontageTID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		FString AnimSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		TSoftObjectPtr<class UAnimSequenceBase> AnimAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool AnimationManualStop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
		TSoftClassPtr<class AWRVFX> VFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
		bool VFXManualStop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		TSoftObjectPtr<class USoundBase> SFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		bool SFXManualStop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForceFeedback")
		int32 ForceFeedbackTID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForceFeedback")
		bool ForceFeefbackManualStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForceFeedback")
		bool ForceFeefbackBroadCast = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
		bool LHandIK = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
		bool RHandIK = false;

	class AWRCharacter* pOwnerIO = nullptr;
	class AWRCharacter* pTarget = nullptr;
	class AWRVFX* pFX = nullptr;
	class UAudioComponent* pAudioComp = nullptr;
	bool bActivate = false;
	FString AsyncVFXKey = WRASYNCLOADMANAGER_INVALID;

public:
	bool InvokeAction(class AWRCharacter* IN InIO, class AWRCharacter* IN InTarget, const FVector IN InLocation = FVector::ZeroVector, const bool IN InUnusable = false);
	void ReleaseAction();
	void UpdateDirection();

private:
	bool OnAnimation();
	void OnFXs(const FVector IN InLocation = FVector::ZeroVector);
	void OnForceFeedback();
	void OnHandsIK();
};

USTRUCT(BlueprintType)
struct FWRIOGyroDirection
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RoundDegree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ForceFeedbackTID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
		TSoftClassPtr<class AWRVFX> VFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
		TSoftObjectPtr<class USoundBase> SFX;

	class AWRVFX* pFX = nullptr;
	class UAudioComponent* pAudioComp = nullptr;

public:
	void ReleaseGyroDirection();
};

USTRUCT(BlueprintType)
struct FWRIOGyro
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWRIOGyroAction GyroType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<class UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
		FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bGyroAngle"))
		EWRIOGyroAxis Axis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180.0, ClampMax = 0.0, EditCondition = "bGyroAngle"))
		float MinDegree = -90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 180.0, EditCondition = "bGyroAngle"))
		float MaxDegree = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180.0, ClampMax = 180.0, EditCondition = "bGyroAngle"))
		float LimitDegree = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180.0, ClampMax = 180.0, EditCondition = "bGyroAngle"))
		float GoalDegree = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 180.0, EditCondition = "bGyroAngle"))
		float LerpTermDegree = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, EditCondition = "bGyroAngle"))
		float LerpTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, EditCondition = "bGyroAngle"))
		TArray<FWRIOGyroDirection> Directions;

	UPROPERTY()
		bool bGyroAngle = false;

	bool bActive = false;

};




#define INVALID_START_DEGREE 999

//====================================================================================

UCLASS()
class WR_API UWRActorComponentInteractionObject : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class AWRCharacterIO* GetOwnerObject();

	//====================================================================================
	//	:: Direction
	//====================================================================================
public:
	void SaveDirections(TMap<EWRIODirectionType, FWRIODirection>* IN InDirections);
	bool InvokeDirectionGroup(EWRInteractionGroup IN InGroupType, FVector IN InLocation = FVector::ZeroVector);
	void ReleaseDirectionGroup(EWRInteractionGroup IN InGroupType, FVector IN InLocation = FVector::ZeroVector);		
	void OnAnimationEnded();
private:
	void OnInteractionGroup(const EWRInteractionGroup IN InType, const bool IN InOn = true, FVector IN InLocation = FVector::ZeroVector);
	void DirectionLinkFunction(EWRIOResultAction IN InAction, int32 IN InID);
	FWRIODirection* GetDirection(EWRIODirectionType IN InType);
	void UpdateDirection();
	//====================================================================================
	//	:: Occupation
	//====================================================================================
public:
	void UpdateToOccupationSync(class AWRCharacterPlayer* IN InPlayer);
	//====================================================================================
	// :: ���� �Ÿ� (����)
	// ���� �Ÿ� üũ�� Update�� �����˻�
	// ���������� ���� 1�������� ���͸� ��
	// �������� ���� ����� '���� ��ȣ�ۿ� ����'�̳� '���� ����'�� üũ���� ����
	//====================================================================================
public:
	bool IsRecognizableState();
private:
	bool IsRecognizableObject();
	void RecognitionAreaIN();
	void RecognitionAreaOUT();
	void UpdateRecognitionArea();
	//====================================================================================
	//	:: 1. ���� ��ȣ�ۿ� �Ÿ�
	// ��ȣ�ۿ� �Ÿ� üũ�� Collision Check�� �˻�
	// '���� ����'���� ������.
	//====================================================================================
public:
	FTransform GetInteractionTransform();
	bool IsInteractionableState();
	void OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	bool IsInteractionableObject();
	void UpdateInteractionableAreaOverappingState();
	void UpdateInteractionMinDist();
	void InteractionAreaIN(class AWRCharacterPlayer* IN InPlayer);
	void InteractionAreaOUT(class AWRCharacterPlayer* IN InPlayer);
	//====================================================================================
	// :: 2. ���� ��ȣ�ۿ� ����
	// ���� ��ȣ�ۿ� �Ÿ��� Update�� �����˻�
	// '���� ��ȣ�ۿ� ����'���� ������.
	// ������ ������ ������ �� OnHookArrival�� HookArrival������ ������
	// ��� �� OnHookEventToTarget�� InteractionStart -> End���� �����Ѵ�. HookInteraction������ ���´�
	//====================================================================================
public:
	bool IsHookableState();
	bool IsHookableObject();
	void OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InImpactLocation, class AWRCharacter* IN InHookCaster);
	void OnHookCutted();
	void OnHookEventToTarget(class AWRCharacter* IN InHookCaster);
private:
	void UpdateHookArea();
	void HookAreaIN();
	void HookAreaOUT();
	//====================================================================================
	//	:: Ÿ���� - ����ȭ �Ű� �Ⱦ�
	//====================================================================================
public:
	void OnTargeted(const bool IN InMulti = false, const bool IN InByMe = true);
	void ReleasedTarget(const bool IN InMulti = false, const bool IN InByMe = true);
	void UpdateTargetUI();
private:
	// Target UI
	void ShowOnTargetUI();
	void ShowOffTargetUI();
	bool IsTargetableObject();
	// Progress UI
	void ShowOnProgress(const float IN InGoalTime);
	void ShowOffProgress(const bool IN InComplete);
	FTransform GetWidgetTransform();
	EWRUIWidgetCreationBitFlag GetWidgetComponentShowType();
	//====================================================================================
	//	:: �Է�
	//====================================================================================
public:
	// �Է� �� ���⸦ ���� ���´�.
	bool OnPressedEvent(const EWRInput IN InInputType);
	bool OnReleasedEvent(const EWRInput IN InInputType);
	void OnPadEvent(const EWRInput IN InEvent);
private:
	void UpdatePressedTime(float IN InDeltaTime);
	void ReleaseProgressing();
	// �Է¿� �´� ���ͷ����� �ִ°�?
	EWRIOInteractionType FindMatchKeyEvent(const EWRInput IN InInputType);
	bool FindMatchPadEvent(const EWRInput IN InEvent);
	//====================================================================================
	//	:: Gyro
	//====================================================================================
public:
	void SaveGyroSteps(TArray<FWRIOGyro>* IN InGyroSteps);
	FWRIOGyro* GetGyroStep(int32 IN InStep);	
private:
	void OnGyroStart();
	void OnGyroEnd();
	bool IsUseGyro();
	void ShowOnGyroInteraction();
	void ShowOffGyroInteraction();
	void UpdateGyroTransform(float IN InDeltaTime);
	void ReleaseGyroData();
	bool CalcGyroLerpDegree(float IN InDeltaTime, float IN InGyroValue, FWRIOGyro* IN InGyroInfo);
	void UpdateGyroDirection(float IN InCurDegree, FWRIOGyro* IN InGyroInfo);
	void ChangeForceFeedback(FWRIOGyroDirection* IN InNewDirection);
	void ChangeVFX(FWRIOGyroDirection* IN InNewDirection);
	void ChangeSFX(FWRIOGyroDirection* IN InNewDirection);
	void StopGyroDirection(FWRIOGyro* IN InGyroInfo);

public:
	// Table
	void SetInteractionTable(FWRInteractionObjectTable* IN InObjectTable);
	// Move
	bool CheckMoveDuringInteraction();
	// High Light
	void UpdateHighLight();
	// Skip
	void OnSkipToTest();
	// for Cheat
	bool IsCheatedObject();
	void AddResultCount(const EWRIOInteractionType IN InType);
	
	// Result
public:
	void AfterProcess_SpawnCompleted();
private:
	void AfterProcess_GyroResult();
	void AfterProcess_Attached();

	// Interaction Anim Ended
private:
	void OnInteractionResult();
	void ReleaseInteractionInfo();

	// Interaction State
public:
	FORCEINLINE void SetInteractionState(const EWRInteractionState::en IN InState) { eState = InState; }
	FORCEINLINE EWRInteractionState::en GetInteractionState() { return eState; }
	FORCEINLINE bool IsInteracting() { return bInteracting; }

	// Interaction Count
public:
	FORCEINLINE int32 GetDirectRetainCount() { return DirectRetainCount; }
	FORCEINLINE int32 GetHookRetainCount() { return HookRetainCount; }
	void UpdateRetainCount(EWRIOInteractionType IN InType, const uint16 IN InCount);	
private:
	bool IsInteractionableCount(const EWRIOInteractionType IN InType);


	//====================================================================================
	//	:: Packet
	//	Start	: ���ͷ��� �õ� (Progress, Gyro �� �߻������� ����. ����� End�ٷ� ȣ��)
	//	Stop	: ���ͷ��� ����
	//	End		: ���ͷ��� �Ϸ� (Progress, Gyro, Input �� �Ϸ� ��)
	//====================================================================================
public:
	void OnReq_InteractionStart(const EWRIOInteractionType IN InInvokeInteractionType);
	void OnInf_InteractionStart(const AID_t IN InCharID, const EWRIOInteractionType IN InInvokeInteractionType);
	void OnReq_InteractionStop();
	void OnInf_InteractionStop(const AID_t IN InCharID);
	void OnReq_InteractionEnd();
	void OnInf_InteractionEnd(const AID_t IN InCharID, const AID_t IN InObjectID, const uint16 IN InRetainCount, const uint8 IN InInteractionType);



private:
	FWRInteractionObjectTable* ObjectTable = nullptr; // for Interaction Object
	class AWRCharacterPlayer* InteractionPlayer = nullptr;

protected:

	EWRInteractionState::en eState = EWRInteractionState::None;	
	bool bInteracting = false;
	bool bInteractionEnding = false;

	// Direction
	EWRInteractionGroup eInteractionGroup = EWRInteractionGroup::None;
	TMap<EWRIODirectionType, FWRIODirection>* Directions = nullptr;

	// UI
	bool bReservedUIOpen = false;
	EWRUIPanelKind TargetUIKind;

	// Press Input
	bool bPressedInput = false;
	float fPressedTime = 0.0f;
	EWRIOInteractionType CurrentInteractionType;

	// Area
	bool bRecognitionArea = false;
	bool bInteractionableArea = false;
	bool bHookableArea = false;

	// Result
	uint16 DirectRetainCount = 0;
	uint16 HookRetainCount = 0;

	// Gyro
	TArray<FWRIOGyro>* GyroSteps = nullptr;
	bool bGyroMode = false;
	bool bGyroLerpMove = false;
	uint8 GyroStep = 0;
	FWRIOGyroDirection* CurGyroDirection = nullptr;
	FRotator SrcRotate;
	float fStartDegree = INVALID_START_DEGREE;
	float fSrcDegree = 0.f;
	float fDstDegree = 0.f;
	float fLerpDegree = 0.f;
	float fLerpSectionMin = 0.f;
	float fLerpSectionMax = 0.f;
	float fElapsedTime = 0.f;

};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRActorComponentInteraction, Display, All);
