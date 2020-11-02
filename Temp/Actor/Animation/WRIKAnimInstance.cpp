// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRIKAnimInstance.h"

UWRIKAnimInstance::UWRIKAnimInstance()
{

}

void UWRIKAnimInstance::NativeBeginPlay()
{
	InitIKFootRef();
}

void UWRIKAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	TickIKFoot(DeltaSeconds);

}

void UWRIKAnimInstance::InitIKFootRef()
{
	//! Get IKFoot Component from owner
	APawn* pOwner = TryGetPawnOwner();
	if (pOwner != nullptr)
	{
		UActorComponent* pActorComp = pOwner->GetComponentByClass(UWRSpiderLegIKComponent::StaticClass());
		if (pActorComp != nullptr)
		{
			InSpiderLegComp = Cast<UWRSpiderLegIKComponent>(pActorComp);
			if (InSpiderLegComp == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("IKFootRef is nullptr"));
				return;
			}
			InSpiderLegComp->InitIKComp();
			IsCanUse = InSpiderLegComp->GetIsCanUse();
			IsInitialize = true;
		}
	}
}

void UWRIKAnimInstance::TickIKFoot(float DeltaSeconds)
{
	if (InSpiderLegComp == nullptr || IsCanUse == false || IsMoving == false)
		return;
	InSpiderLegComp->TickIKComp(DeltaSeconds);
	InLegTransforms = InSpiderLegComp->GetLegTransforms();
}
