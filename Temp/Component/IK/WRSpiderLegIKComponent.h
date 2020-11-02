// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WRSpiderLegIKComponent.generated.h"


//! IK Anim Instance Value struct
USTRUCT(Atomic, BlueprintType)
struct FWRFablikLegTransforms
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_LLeg1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_RLeg1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_LLeg2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_RLeg2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_LLeg3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_RLeg3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_LLeg4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform		m_fEffectorFTransform_RLeg4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		    m_fFABLIKAlpha;
};

//! Line Trace info struct
//! IK Anim Instance Value struct
USTRUCT(Atomic, BlueprintType)
struct FWRFablikLegInfo
{
	GENERATED_BODY()
public:
	FName BoneName;
	FVector vBaseRelativeLocation;
	FVector vPrevTargetLocation;
	FVector vTargetLocation;
	FTransform tEffectorTransform;
	bool bMoving;

public:
	FWRFablikLegInfo() {}
	FWRFablikLegInfo(FName InBoneName) : vBaseRelativeLocation(FVector::ZeroVector),
		vPrevTargetLocation(FVector::ZeroVector),
		vTargetLocation(FVector::ZeroVector),
		tEffectorTransform(FTransform::Identity),
		bMoving(false)
	{
		BoneName = InBoneName;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WR_API UWRSpiderLegIKComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
		class AWRCharacter* InOwnerCharacter;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		TArray<FString> BoneList;
	
	// 발이 퍼지는 거리 조정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fWideSize = 0.f;

	// CompareLength 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fCompareLength = 0.f;

	// LegTrace Distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fTraceDistance = 50.f;

	// FABLIK ALPHA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fFABLIKAlpha = 1.f;

	// InterpSpeed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fInterpSpeed = 17.f;

	// HeightOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fOffset = 0.f;

	// LegRaise Height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float fLegRaise = 1.f;

	// CheckLegSmooth
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		float CheckLegSmooth = 40.f;

	// InterpSpeed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		bool IsDrawTargetPosition = false;

	// CanUse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK BoneInfo")
		bool IsCanUse = false;

private:
	TArray<FWRFablikLegInfo> LegInfoList;
	bool TestLegListTag = false;

	TMap<FName, int32> MatchingLegsIndexs_Fir;
	TMap<FName, int32> MatchingLegsIndexs_Sec;

	TMap<FName, int32>* MatchingLegsIndex;

private:
	FORCEINLINE void AddLegInfo(const FName& InBoneName)
	{
		if (this->IsContainLegInfo(InBoneName) == true)
			return;
		this->LegInfoList.Add(FWRFablikLegInfo(InBoneName));
	}

	FORCEINLINE bool IsContainLegInfo(const FName& InBoneName)
	{
		for (FWRFablikLegInfo InLegInfo : this->LegInfoList)
		{
			if (InLegInfo.BoneName == InBoneName)
				return true;
		}
		return false;
	}

	FORCEINLINE FTransform GetIKBoneTransform(const FName InBoneName)
	{
		for (FWRFablikLegInfo InLegInfo : this->LegInfoList)
		{
			if (InLegInfo.BoneName == InBoneName)
				return InLegInfo.tEffectorTransform;
		}
		return FTransform::Identity;
	}

	FORCEINLINE void SetLegTransform(const FName InBoneName)
	{
		if (InBoneName == TEXT("SpiderLLeg14"))
		{
			LegTransforms.m_fEffectorFTransform_LLeg1 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderLLeg24"))
		{
			LegTransforms.m_fEffectorFTransform_LLeg2 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderLLeg34"))
		{
			LegTransforms.m_fEffectorFTransform_LLeg3 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderLLeg44"))
		{
			LegTransforms.m_fEffectorFTransform_LLeg4 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderRLeg14"))
		{
			LegTransforms.m_fEffectorFTransform_RLeg1 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderRLeg24"))
		{
			LegTransforms.m_fEffectorFTransform_RLeg2 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderRLeg34"))
		{
			LegTransforms.m_fEffectorFTransform_RLeg3 = GetIKBoneTransform(InBoneName);
		}
		else if (InBoneName == TEXT("SpiderRLeg44"))
		{
			LegTransforms.m_fEffectorFTransform_RLeg4 = GetIKBoneTransform(InBoneName);
		}

	}

	FORCEINLINE float CalculateRaiseLegZ(const FVector& InStandardVector, const FVector& InCurrentVector, const FVector& InTargetVector)
	{
		FVector BetweenStandardToTarget = InTargetVector - InStandardVector;
		FVector BetweenStandardToCurrent = InCurrentVector - InStandardVector;

		float InTargetSize = BetweenStandardToTarget.Size();
		float InCurrentSize = BetweenStandardToCurrent.Size();

		if (InTargetSize == 0.f || InCurrentSize == 0.f)
			return InCurrentVector.Z + fOffset;

		float ResultPositionOnSin = InCurrentSize / InTargetSize;

		ResultPositionOnSin = FMath::Min(ResultPositionOnSin, 1.f);
		ResultPositionOnSin = FMath::Max(ResultPositionOnSin, 0.f);
		ResultPositionOnSin = ResultPositionOnSin * UKismetMathLibrary::GetPI();

		return (FMath::Sin(ResultPositionOnSin) * fLegRaise) + InCurrentVector.Z + fOffset;
	}
public:	
	// Sets default values for this component's properties
	UWRSpiderLegIKComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		
	void InitIKComp();

	void TickIKComp(float DeltaTime);

	void GetNextLegDestination(FWRFablikLegInfo* InLegInfo, const FVector InLegLocation);

	void SetCurrentLocationInterpToTarget(FWRFablikLegInfo* InLegInfo, float DeltaTime);

public:
	FWRFablikLegTransforms LegTransforms;
	
public:		

	FORCEINLINE FWRFablikLegTransforms& GetLegTransforms() { return LegTransforms; }
	FORCEINLINE bool GetIsCanUse() { return IsCanUse; }
};
