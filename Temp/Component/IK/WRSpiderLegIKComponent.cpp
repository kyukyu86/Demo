// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSpiderLegIKComponent.h"
#include "Actor/Character/WRCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UWRSpiderLegIKComponent::UWRSpiderLegIKComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UWRSpiderLegIKComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InOwnerCharacter = nullptr;
}

void UWRSpiderLegIKComponent::InitIKComp()
{
	InOwnerCharacter = Cast<AWRCharacter>(this->GetOwner());

	if (InOwnerCharacter == nullptr)
		return;

	BoneList.Add(TEXT("SpiderLLeg14"));
	BoneList.Add(TEXT("SpiderLLeg24"));
	BoneList.Add(TEXT("SpiderLLeg34"));
	BoneList.Add(TEXT("SpiderLLeg44"));
	BoneList.Add(TEXT("SpiderRLeg14"));
	BoneList.Add(TEXT("SpiderRLeg24"));
	BoneList.Add(TEXT("SpiderRLeg34"));
	BoneList.Add(TEXT("SpiderRLeg44"));

	for (const FString InBoneName : BoneList)
	{
		this->AddLegInfo(*InBoneName);
	}

	for (int32 i = 0; i < LegInfoList.Num(); i++)
	{
		FWRFablikLegInfo& InLegInfo = LegInfoList[i];

		InLegInfo.vBaseRelativeLocation = InOwnerCharacter->GetMesh()->GetSocketTransform(InLegInfo.BoneName, ERelativeTransformSpace::RTS_Actor).GetLocation();
		InLegInfo.tEffectorTransform = InOwnerCharacter->GetMesh()->GetSocketTransform(InLegInfo.BoneName, ERelativeTransformSpace::RTS_World);

		if (InLegInfo.BoneName == TEXT("SpiderLLeg14")) { MatchingLegsIndexs_Fir.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderLLeg24")) { MatchingLegsIndexs_Sec.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderLLeg34")) { MatchingLegsIndexs_Fir.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderLLeg44")) { MatchingLegsIndexs_Sec.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderRLeg14")) { MatchingLegsIndexs_Sec.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderRLeg24")) { MatchingLegsIndexs_Fir.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderRLeg34")) { MatchingLegsIndexs_Sec.Add(InLegInfo.BoneName, i); }
		else if (InLegInfo.BoneName == TEXT("SpiderRLeg44")) { MatchingLegsIndexs_Fir.Add(InLegInfo.BoneName, i); }
		this->SetLegTransform(InLegInfo.BoneName);

	}

	MatchingLegsIndex = &MatchingLegsIndexs_Fir;

	LegTransforms.m_fFABLIKAlpha = fFABLIKAlpha;
}

void UWRSpiderLegIKComponent::TickIKComp(float DeltaTime)
{
	FVector AccDirection = InOwnerCharacter->GetVelocity().GetSafeNormal();

	bool OutIsModified = true;

	TArray<int32> LegList;
	MatchingLegsIndex->GenerateValueArray(LegList);

	bool NeedChange = true;

	for (int32 InLegIndex : LegList)
	{
		FWRFablikLegInfo& InLegInfo = LegInfoList[InLegIndex];
		FVector LengthVec = InLegInfo.vTargetLocation - InLegInfo.tEffectorTransform.GetLocation();

		float LengthSize = LengthVec.Size();

		if (LengthSize > CheckLegSmooth)
			NeedChange = false;
	}

	if (NeedChange == true)
	{
		if (TestLegListTag == false)
		{
			TestLegListTag = true;
			MatchingLegsIndex = &MatchingLegsIndexs_Fir;
		}
		else
		{
			TestLegListTag = false;
			MatchingLegsIndex = &MatchingLegsIndexs_Sec;
		}
	}

	MatchingLegsIndex->GenerateValueArray(LegList);

	for (int32 InLegIndex : LegList)
	{
		FWRFablikLegInfo& InLegInfo = LegInfoList[InLegIndex];

		float CurrentActorAxis = InOwnerCharacter->GetActorRotation().GetComponentForAxis(EAxis::Z);
		
		FVector InBaseRelativeLocation = InLegInfo.vBaseRelativeLocation;

		FVector vStandardLegLocation = InOwnerCharacter->GetActorLocation() + InBaseRelativeLocation.RotateAngleAxis(CurrentActorAxis, InOwnerCharacter->GetActorUpVector()) + (AccDirection * fWideSize);

		if ((InLegInfo.vTargetLocation - vStandardLegLocation).Size() > fCompareLength)
		{
			GetNextLegDestination(&InLegInfo, vStandardLegLocation);
		}
		else
		{
			SetCurrentLocationInterpToTarget(&InLegInfo, DeltaTime);
		}
		
		this->SetLegTransform(InLegInfo.BoneName);
	}
}

void UWRSpiderLegIKComponent::GetNextLegDestination(FWRFablikLegInfo * InLegInfo, const FVector InLegLocation)
{
	if (InLegInfo == nullptr)
		return;


	FVector vLineStart = FVector(InLegLocation.X, InLegLocation.Y, InOwnerCharacter->GetActorLocation().Z);
	FVector vLineEnd = FVector(InLegLocation.X, InLegLocation.Y, InOwnerCharacter->GetActorLocation().Z - fTraceDistance);

	FHitResult pHitResult;
	TArray<AActor*> pIgnore;
	pIgnore.Add(GetOwner());
	EDrawDebugTrace::Type eDebug = EDrawDebugTrace::None;

	bool bResult = UKismetSystemLibrary::LineTraceSingle(GetWorld(), vLineStart, vLineEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), true, pIgnore, eDebug, pHitResult, true);

	if (bResult == false)
		return;
	if (IsDrawTargetPosition == true)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), pHitResult.Location, 50.f, 12, FLinearColor::Red, 5.f);

	InLegInfo->vPrevTargetLocation = InLegInfo->vTargetLocation;
	InLegInfo->vTargetLocation = pHitResult.Location;
}

void UWRSpiderLegIKComponent::SetCurrentLocationInterpToTarget(FWRFablikLegInfo * InLegInfo, float DeltaTime)
{
	if (InLegInfo == nullptr)
		return;

	bool bLeftFootNearly = UKismetMathLibrary::EqualEqual_VectorVector(InLegInfo->tEffectorTransform.GetLocation(), InLegInfo->vTargetLocation, 1.0f);

	if (bLeftFootNearly == true)
		return;

	FVector CurrentVector = InLegInfo->tEffectorTransform.GetLocation();

	CurrentVector = UKismetMathLibrary::VInterpTo(CurrentVector, InLegInfo->vTargetLocation, DeltaTime, this->fInterpSpeed);

	CurrentVector.Z = this->CalculateRaiseLegZ(InLegInfo->vPrevTargetLocation, CurrentVector, InLegInfo->vTargetLocation);

	InLegInfo->tEffectorTransform.SetLocation(CurrentVector);
}

