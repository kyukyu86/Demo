#pragma once
#include "TimerManager.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Network/Share/Define/Define_Type.h"
#include "Enum/EWRActor.h"


#define WRACTOR_SETMATERIAL_PARAMETER_VALUE(ParamName, ParamType, Value, Function, MeshComponent)							\
TArray<UMaterialInstanceDynamic*>	MIDs;																					\
WRActorUtility::GetMeshMaterialInstanceDynamic(MeshComponent, MIDs);														\
for (int32 i = 0; i < MIDs.Num(); ++i)																						\
{																															\
	UMaterialInstanceDynamic* MID = MIDs[i];																				\
	if (MID != nullptr)																										\
		MID->Function(ParamName, Value);																					\
}

struct EWRDrawDebugType
{
	enum en
	{
		None,
		Skill,
		AIMoveTo,
		StopMove,
		Reaction,
		Teleport,
		HitRange,
		DodgeRange,
		Spawn,
		WarpTo,

		MAX
	};
};

struct FWRSkeletalMeshMergeParameters
{
public:
	FWRSkeletalMeshMergeParameters()
	{
		MeshesToMerge.Empty();
		StripTopLODS = 0;
		bNeedsCpuAccess = false;
		bSkeletonBefore = false;
		Skeleton = nullptr;
	}

public:
	// The list of skeletal meshes to merge.
	TArray<class USkeletalMesh*> MeshesToMerge;

	// The number of high LODs to remove from input meshes
	int32 StripTopLODS;

	// Whether or not the resulting mesh needs to be accessed by the CPU for any reason (e.g. for spawning particle effects).
	uint32 bNeedsCpuAccess : 1;

	// Update skeleton before merge. Otherwise, update after.
	// Skeleton must also be provided.
	uint32 bSkeletonBefore : 1;

	// Skeleton that will be used for the merged mesh.
	// Leave empty if the generated skeleton is OK.
	class USkeleton* Skeleton;
};

class WRActorUtility
{
public:
	static FVector GetDirection(class AActor* InFrom, class AActor* InTo);
	static FVector GetDirection(const FVector& InFrom, const FVector& InTo);

	static FTransform GetSocketTransform(const class AWheeledVehicle* InVehicle, const FName& InSocketName, const bool InIsComponentSpace = false, const bool InIsPure = false);
	static FTransform GetSocketTransform(const class ACharacter* InCharacter, const FName& InSocketName, const bool InIsComponentSpace = false, const bool InIsPure = false);
	static FTransform GetSocketTransform(const class AActor* InActor, const FName& InSocketName, const bool InIsComponentSpace = false, const bool InIsPure = false);
	static FTransform GetSocketLocalTransform(const class ACharacter* InCharacter, const FName& InSocketName);

	static FTransform GetBoneTransform(class AWheeledVehicle* InVehicle, const FName& InBoneName);
	static FTransform GetBoneTransform(class ACharacter* InCharacter, const FName& InBoneName);
	static FTransform GetBoneTransform(class AActor* InActor, const FName& InBoneName);

	static FVector GetSocketDirection(class ACharacter* InFrom, class ACharacter* InTo, const FName& InFromSocketName, const FName& InToSocketName);
	static FVector GetBoneDirection(class ACharacter* InFrom, class ACharacter* InTo, const FName& InFromBoneName, const FName& InToBoneName);

#ifdef UE_EDITOR
	static void IsRooted(class AActor* InActor);
#else // UE_EDITOR
#endif // UE_EDITOR

	// [ 2019-10-11 : magedoga ] Get Nearest Bone
	static FName GetNearestBoneName(class AWRCharacter* InCharacter, const FVector& InWorldLocation, const TArray<FString>& InIgnoreBoneName);
	// [ 2019-10-8 : magedoga ] Get Nearest Socket (include bone)
	static FName GetNearestSocketName(class AWRCharacter* InCharacter, const FVector& InWorldLocation);
	static FName GetNearestSocketNameWithTitleName(class AWRCharacter* InCharacter, const FVector& InWorldLocation, const FString& InTitleName, const EWRCollisionType InPart);

	static bool IsOnNavigation(const UWorld* InWorld, const FVector& InLocation);
	static FVector ProjectPointToNavigation(class AActor* InActor);
	static FVector ProjectPointToNavigation(FVector InLocation, FVector InExtent = FVector::ZeroVector);
	static FVector ProjectPointToNavigationNotExtent(FVector InLocation);
	static bool ProjectPointToNavigation(AActor* InActor, const FVector& InPoint, struct FNavLocation& OutNavLocation);

	static void SetActorTickEnable(class AActor* InActor, const bool bEnabled);

	static void SetComponentTickEnable(class UActorComponent* InComponent, const bool bInEnabled);
	static void SetComponentCollisionEnabled(class UActorComponent* InComponent, ECollisionEnabled::Type NewType);
	static void SetComponentCollisionEnabledToAll(class UActorComponent* InComponent, ECollisionEnabled::Type NewType);
	static void SetVisibility(class UActorComponent* InComponent, const bool bInEnable);

	static class USkeletalMesh* MergeMeshes(const FWRSkeletalMeshMergeParameters& InParameter);

	// [ 2020-4-2 : magedoga ] Generate Overlap Events
	static void SetGenerateOverlapEventsbyActor(const bool bInEnable, AActor* InActor, bool bInPropagateToChild = true);
	static void SetGenerateOverlapEventbyComponent(const bool bInEnable, USceneComponent* InComponent, bool bInPropagateToChild = true);

	// [ 2020-4-7 : magedoga ] UpdateOverlap FinalizeAnimUpdate
	static void SetEnableUpdateOverlapWhenFinalizeAnim(const bool bInEnable, USceneComponent* InComponent, bool bInPropagateToChild = true);

#if UE_EDITOR
	// [ 2019-8-26 : animal14 ] 페르소나 전용 함수
	static bool IsEditorPreview(class AActor* InActor);
#endif //UE_EDITOR	

	static void GetMeshMaterialInstanceDynamic(class UMeshComponent* InMeshComponent, TArray<class UMaterialInstanceDynamic*>& OutMIDs);

#define SET_MATERIAL_PARAMETER(ParamName, Value, Function, MeshComponent)						\
{																								\
	TArray<UMaterialInstanceDynamic*> MIDs;														\
	WRActorUtility::GetMeshMaterialInstanceDynamic(MeshComponent, MIDs);						\
	for (int32 i = 0; i < MIDs.Num(); ++i)														\
	{																							\
		UMaterialInstanceDynamic* MID = MIDs[i];												\
		if (MID != nullptr)																		\
		{																						\
			MID->Function(ParamName, Value);													\
		}																						\
	}																							\
}																								\

	// [ 2019-9-24 : kyu ] 
	static float GetDegreesBetween2Vectors(const FVector InVector1, const FVector InVector2);

	static bool ModifyZHeightForClient(class AActor* InActor, const FVector& Original, float& OutHeight);
	static bool ModifyZHeightForServer(class AActor* InActor, const FVector& Original, float& OutHeight);

	static float GetActorCapsuleHalfHeight(class AWRCharacter* InActor);
	static float GetLengthBetweenActor(class AWRCharacter* InA, class AWRCharacter* InB, bool bInIncludeRadius, bool bInSize2D = false);
	static float GetCurrentAttackRange(class AWRCharacter* InActor);
	static float GetActorCapsuleRadius(class AWRCharacter* InActor);
	static float GetActorDoNotOverlapCapsuleRadius(class AWRCharacter* InActor);
	static bool IsAttackableRange(class AWRCharacter* InAttacker, class AWRCharacter* InHitActor);
	static float GetDistance(const class AActor* IN InFromActor, const class AActor* IN InToActor);
	static float GetDistance(const class AActor* IN InActor, const FVector& IN InVector);
	static bool CheckInnerRange_Sector(const class AWRCharacter* IN InChar, const FTransform IN InStandardTF, const float IN InMin, const float IN InMax, const float IN InFullAngle);
	static bool CheckInnerRange_Sector(FVector IN InStandardLocation, FVector IN InDirection, float IN InDistance, float IN InMin, float IN InMax, float IN InFullAngle);

	static void SetTimer(FTimerHandle& InHandle, float InRate, bool InbLoop);
	static void SetTimer(FTimerHandle& InHandle, FTimerDelegate const& InDelegate, float InRate, bool InbLoop);
	static void ClearTimer(FTimerHandle& InHandle);
	static bool IsTimerActive(FTimerHandle InHandle);
	static float GetTimerRemaining(FTimerHandle InHandle);

	static class USplineComponent* CreateSplineComponent(class AActor* InActor, bool InbStationaryEndpoints);
	static bool DestroySplineComponent(class AActor* InActor, class USplineComponent* InSplineComponent);

	// Convert 1person to 3person Montage TID
	static int32 ConvertTo3PersonMontageTID(const int32 InSrcTID, const int32 InCharTID);
	static int32 GetRealMontageTID(AWRCharacter* InTarget, const int32 InBaseTID);

	static void DrawDebug(AWRCharacter* InTarget, const EWRDrawDebugType::en& DebugType, FVector InPosition = FVector::ZeroVector);
	static void DrawDebug(const EWRDrawDebugType::en& DebugType, FVector InPosition);
	static void DrawSector(UWorld* pWorld, FVector vStandardLoc, FVector vDir, float fMin, float fMax, float fAngle, float fLifeTime = 0.5f);

	// Actor Manage
	static class AActor* FindActorByAID(AID_t IN InAID);
	static class AActor* FindActorByActorTypeAndAID(EWRActorDetailType IN InActorType, AID_t IN InAID);
	template<class ClassType>
	static ClassType* FindActorByAID_Casted(AID_t IN InAID);
	template<class ClassType>
	static ClassType* FindActorByActorTypeAndAID_Casted(EWRActorDetailType IN InActorType, AID_t IN InAID);
};

template<class ClassType>
ClassType* WRActorUtility::FindActorByAID_Casted(AID_t IN InAID)
{
	AActor* FoundActor = WRActorUtility::FindActorByAID(InAID);
	if (FoundActor == nullptr)
		return nullptr;

	return Cast<ClassType>(FoundActor);
}

template<class ClassType>
ClassType* WRActorUtility::FindActorByActorTypeAndAID_Casted(EWRActorDetailType IN InActorType, AID_t IN InAID)
{
	AActor* FoundActor = WRActorUtility::FindActorByActorTypeAndAID(InActorType, InAID);
	if (FoundActor == nullptr)
		return nullptr;

	return Cast<ClassType>(FoundActor);
}
