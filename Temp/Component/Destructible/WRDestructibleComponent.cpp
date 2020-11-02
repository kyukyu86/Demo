// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRDestructibleComponent.h"
#include "Components/ArrowComponent.h"
#include "Actor/Character/WRCharacter.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Manager/WRConsoleManager.h"
#include "Define/WRPathDefine.h"
#include "Manager/WRCharacterManager.h"

void UWRDestructibleComponent::BeginPlay()
{
	Super::BeginPlay();
	this->CreateDynamicMaterials();
	this->InitializePhysicsDirection();
	this->SetEnableTick(true);
	
}


void UWRDestructibleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//this->ProcessRemoveDisplay(DeltaTime);
	this->SetDynamicMaterialValue(DeltaTime);
}

void UWRDestructibleComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void UWRDestructibleComponent::SetEnableTick(bool IsTickEnable)
{
	if (IsTickEnable == true)
	{
		if (this->PrimaryComponentTick.IsTickFunctionRegistered() == false)
		{
			this->PrimaryComponentTick.RegisterTickFunction(this->GetWorld()->GetCurrentLevel());
			this->PrimaryComponentTick.Target = this;
		}
	}

	this->PrimaryComponentTick.bCanEverTick = IsTickEnable;
	this->PrimaryComponentTick.SetTickFunctionEnable(IsTickEnable);

	if (IsTickEnable == false)
	{
		if (this->PrimaryComponentTick.IsTickFunctionRegistered() == true)
		{
			this->PrimaryComponentTick.UnRegisterTickFunction();
		}
	}
}

FVector UWRDestructibleComponent::GetImpulseStartLocation()
{
	FVector OutLocation = FVector::ZeroVector;

	switch (HitType)
	{
	case EWRDestructibleHitPositionType::Static:
		if (InPhysicsDirection != nullptr)
		{
			OutLocation = InPhysicsDirection->GetComponentLocation();			
		}
		else
		{
			OutLocation = this->GetOwner()->GetActorLocation();//this->GetOwner()->GetActorLocation() +this->GetRelativeLocation();	
		}
		break;
	case EWRDestructibleHitPositionType::Dynamic:
		OutLocation = this->GetOwner()->GetActorLocation() + this->GetRelativeLocation(); // 현재 상세한 위치에 파괴가 제대로 되는지 여부를 확인하지 못함 추후 갱신예정
		break;
	default:
		OutLocation = this->GetOwner()->GetActorLocation() + this->GetRelativeLocation();
		break;
	}
	return OutLocation;
}

FVector UWRDestructibleComponent::GetImpulseDirection()
{
	FVector OutDirection = FVector::ZeroVector;

	if (InPhysicsDirection != nullptr)
	{
		OutDirection = InPhysicsDirection->GetForwardVector();
	}
	else
	{
		AWRCharacter* InOwner = Cast<AWRCharacter>(this->GetOwner());
		if (InOwner == nullptr)
		{
			return FVector::ZeroVector;
		}

		OutDirection = InOwner->GetPhysicsDirection();
	}
	return OutDirection;
}

void UWRDestructibleComponent::DestructionByRadial()
{
	FVector HurtOrigin = this->GetImpulseStartLocation();

	this->ApplyRadiusDamage(ImpulsePower, HurtOrigin, ImpulseRadius, ImpulseStrength, true);
}

void UWRDestructibleComponent::DestructionByDirection()
{
	FVector HurtOrigin = this->GetImpulseStartLocation();
	FVector HurtDirection = this->GetImpulseDirection();

	if (HurtDirection == FVector::ZeroVector)
	{
		this->DestructionByRadial();
		return;
	}
	this->ApplyDamage(ImpulsePower, HurtOrigin, HurtDirection, ImpulseStrength);
}

void UWRDestructibleComponent::InitializePhysicsDirection()
{
	AActor*InOwner = this->GetOwner();

	if (InOwner == nullptr)
		return;

	TArray<UActorComponent*> InArrComps = InOwner->GetComponentsByTag(UArrowComponent::StaticClass(), TEXT("Reserve"));

	for (UActorComponent*It : InArrComps)
	{
		InPhysicsDirection = Cast<UArrowComponent>(It);
		break;
	}
}

void UWRDestructibleComponent::CreateDynamicMaterials()
{
	DissolveDuration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");

	auto ConvertToMID = [&](UMaterialInterface* InMaterialInterface)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);
		if (MID == nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(InMaterialInterface, this);
		}

		return MID;
	};

	int32 NumOfMaterial = this->GetNumMaterials();

	for (int32 i = 0; i < NumOfMaterial; ++i)
	{
		UMaterialInterface* MaterialInterface = this->GetMaterial(i);
		UMaterialInstanceDynamic* MID = ConvertToMID(MaterialInterface);
		if (this->GetMaterial(i) != MID)
			this->SetMaterial(i, MID);

		MIDs.Add(MID);
	}
}

void UWRDestructibleComponent::SetDynamicMaterialValue(float InDeltaTime)
{
	if (IsBeginDestruction == false)
	{
		return;
	}

	DissolveDeltaTime += InDeltaTime;

	float Ratio = FMath::Clamp(DissolveDeltaTime / DissolveDuration, 0.0F, 1.0F);
	for (UMaterialInstanceDynamic* MID : MIDs)
	{
		MID->SetScalarParameterValue("Dissolve", Ratio);
	}

	if (Ratio >= 1.f)
	{
		this->CompleteDissolve();
	}
}


void UWRDestructibleComponent::ProcessRemoveDisplay(float InDeltaTime)
{
	if (IsBeginDestruction == false)
	{
		return;
	}

	FVector CurrentLocation = this->GetComponentLocation();

	// [ 2019-11-28 : animal14 ] 이동량 확인
	float Difference = FMath::Abs(PreviousLocation.Size() - CurrentLocation.Size());
	float MovementAmount = WRConsoleManager::Get()->GetFloat(EWRConsole::Movement_Amount);
	if (Difference > MovementAmount)
	{
		PreviousLocation = CurrentLocation;
	}
	else
	{
		AccDeltaTime += InDeltaTime;

		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dead_Duration);
		if (Duration > AccDeltaTime)
		{
			return;
		}

		this->MakeRemoveDisplay();
	}
}

void UWRDestructibleComponent::MakeRemoveDisplay()
{
	// [ 2020-2-19 : animal14 ] 이팩트 생성
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, this->GetOwner(), nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	FVector ComponentScale = this->GetComponentScale();
	float AverageScale = ComponentScale.SizeSquared() / 3;;

	VFX->SetFloatParameter(TEXT("ENPC_Size"), FMath::Sqrt(AverageScale));

	// [ 2019-11-13 : animal14 ] 이벤트 바인드
	VFX->OnComplete().AddUObject(this, &UWRDestructibleComponent::CompleteDissolve);

	// [ 2020-2-19 : animal14 ] 디졸브 연출
	IsBeginDestruction = false;
}

void UWRDestructibleComponent::BeginDestruction()
{
	switch (ImpulseType)
	{
	case EWRDestructibleImpulseType::Radial:
		this->DestructionByRadial();
		break;
	case EWRDestructibleImpulseType::Direction:
		this->DestructionByDirection();
		break;
	default:
		break;
	}
	IsBeginDestruction = true;
}

void UWRDestructibleComponent::CompleteDissolve()
{
	AWRCharacter* InCharacter = Cast<AWRCharacter>(this->GetOwner());

	if (InCharacter != nullptr)
	{
		WRCharacterManager::Get()->Remove(InCharacter);
	}

	/* EnvActor를 관리하는 매니저를 만들예정 */
}
