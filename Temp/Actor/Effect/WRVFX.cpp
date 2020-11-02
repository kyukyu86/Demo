// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFX.h"

#include "Particles/ParticleSystemComponent.h"
#include "ParticleEmitterInstances.h"

//#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
//#include "Components/MeshComponent.h"
#include "Components/DecalComponent.h"

#include "Component/Effect/WRVFXComponentAttach.h"
#include "Component/Effect/WRVFXComponentMove.h"
#include "Component/Effect/WRVFXComponentSound.h"
#include "Component/Effect/WRVFXComponentSpawn.h"
#include "Component/Effect/WRVFXComponentLaser.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"

#include "Manager/WRVFXManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCombatManager.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include <NiagaraComponent.h>
#include "NiagaraDataInterfaceSkeletalMesh.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include <Engine/EngineTypes.h>

#include "../Character/WRCharacter.h"


// Sets default values
AWRVFX::AWRVFX()
{
#if WITH_EDITORONLY_DATA
	SpriteScale = 0.01F;
#endif //WITH_EDITORONLY_DATA
}

// Called when the game starts or when spawned
void AWRVFX::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWRVFX::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool IsSpawned = VFX.Tick(DeltaTime);
	if (IsSpawned)
	{
#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(this->GetSpawner()))
		{
			SetActive(false);
			Destroy();
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			WRVFXManager::Get()->Remove(this);
		}
	}
	else
	{
		this->Tick4Parameter();
		this->Tick4Mesh();
	}
}

void AWRVFX::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (this->IsBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult) == false)
	{
		return;
	}

	this->OnComponentBeginOverlap(OtherActor, OtherComp, SweepResult);
}

void AWRVFX::Initialize(AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const EWRVFX InBits, const FRotator& InAngle)
{
	Spawner = InSpawner;
	Target = InTarget;
	Parent = InParent;

	AWRCharacter* _Spawner = Cast<AWRCharacter>(InSpawner);
	if (_Spawner)
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(_Spawner->GetActorID());
		if (CombatInfo)
		{
			CombatSeqID = CombatInfo->GetCombatSeqID();
		}
	}

	this->BuildComponents();

	UWRVFXComponentMove* MoveComponent = this->GetCastedComponent<UWRVFXComponentMove>(EWRVFXComponent::Move);
	if (MoveComponent)
	{
		MoveComponent->SetAdditionalAngle(InAngle);
	}
	UMeshComponent* MeshComponent = this->GetCastedComponent<UMeshComponent>(EWRVFXComponent::Mesh);
	if (MeshComponent)
	{
		FromScale = MeshComponent->GetComponentScale();
	}

	VFX.Reset();

	LastLifespan = -1.0F;

	Bits = InBits;
	this->Check();

	this->SetActive(true);
}

void AWRVFX::Release()
{
	if (Components.Contains(EWRVFXComponent::Move))
	{
		WRCombatManager::Get()->UnregisterUsingDeleteTimer(CombatSeqID);
	}	
	CombatSeqID = INVALID_SKILL_SEQ;

	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent)
	{
		NiagaraComponent->OnSystemFinished.RemoveDynamic(this, &AWRVFX::OnNiagaraSystemFinished);
	}
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent)
	{
		ParticleSystemComponent->OnSystemFinished.RemoveDynamic(this, &AWRVFX::OnSystemFinished);
		//ParticleSystemComponent->OnParticleCollide.RemoveDynamic(this, &AWRVFX::OnParticleCollide);

		ParticleSystemComponent->TranslucencySortPriority = WRConsoleManager::Get()->GetInt(EWRConsole::TranslucencySortPriority4VFX) + AdditionalTranslucencySortPriority;
	}

	// [ 2019-8-12 : animal14 ] 매번 새로 생성하고 삭제할 필요성X
	//Components.Empty();

	this->SetActive(false);

	WRVFXManager::Get()->Unregister(this);

	Spawner = nullptr;
	Target = nullptr;

	// [ 2019-8-12 : animal14 ] 부모의 자식을 삭제
	if (Parent && Parent->IsValidLowLevel())
	{
		Parent->Abandon(this);
	}
	Parent = nullptr;

	// [ 2019-8-12 : animal14 ] 자식들의 부모를 삭제
	for (auto& Child : Childs)
	{
		Child.Value->Abandon();
	}
	Childs.Empty();
}

void AWRVFX::SetActive(const bool bEnabled)
{
	WRActorUtility::SetActorTickEnable(this, bEnabled);

	for (auto& Component : Components)
	{
		Component.Value->SetActive(bEnabled);
	}

	if (bEnabled)
	{

	}
	else
	{
		for (auto& Component : Components)
		{
			Component.Value->RemoveFromRoot();
		}
	}
}

uint64 AWRVFX::GetSpawnerAID()
{
	AWRCharacter* SpawnerCharacter = Cast<AWRCharacter>(Spawner);
	if (SpawnerCharacter == nullptr)
	{
		return INVALID_AID;
	}
	return SpawnerCharacter->GetActorID();
}

uint64 AWRVFX::GetTargetAID()
{
	AWRCharacter* TargetCharacter = Cast<AWRCharacter>(Target);
	if (TargetCharacter == nullptr)
	{
		return INVALID_AID;
	}
	return TargetCharacter->GetActorID();
}

uint32 AWRVFX::GetParentUID()
{
	if (Parent == nullptr)
	{
		return 0;
	}
	return Parent->GetUniqueID();
}

void AWRVFX::RemoveChilds()
{
	for (auto& Child : Childs)
	{
		WRVFXManager::Get()->Remove(Child.Value);
	}
	Childs.Empty();
}

#ifdef WR_EFFECT_NIAGARA
const struct FNiagaraParameterStore* AWRVFX::GetOverrideParameters()
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return nullptr;
	}
	return &NiagaraComponent->GetOverrideParameters();
}

void AWRVFX::SetNiagaraDataInterfaceSkeletalMesh()
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}

	const FNiagaraParameterStore& OverrideParameters = NiagaraComponent->GetOverrideParameters();

	const TArray<UNiagaraDataInterface*>& DataInterfaces = OverrideParameters.GetDataInterfaces();
	for (auto& DataInterface : DataInterfaces)
	{
		UNiagaraDataInterfaceSkeletalMesh* DataInterfaceSkeletalMesh = Cast<UNiagaraDataInterfaceSkeletalMesh>(DataInterface);
		if (DataInterfaceSkeletalMesh == nullptr)
		{
			continue;
		}

		ACharacter* Character = Cast<ACharacter>(this->GetSpawner());
		if (Character == nullptr)
		{
			continue;
		}

		USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
		if (SkeletalMeshComponent == nullptr)
		{
			continue;
		}

		USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->SkeletalMesh;
		if (SkeletalMesh == nullptr)
		{
			continue;
		}

		for (int32 i = 0; i < SkeletalMesh->GetLODNum(); ++i)
		{
#if UE_EDITOR
			if (SkeletalMesh->GetLODInfo(i)->bAllowCPUAccess)
			{
				continue;
			}
#else // UE_EDITOR
#endif // UE_EDITOR
			SkeletalMesh->GetLODInfo(i)->bAllowCPUAccess = true;
		}

#if WITH_EDITORONLY_DATA
		DataInterfaceSkeletalMesh->DefaultMesh = SkeletalMesh;
#endif //WITH_EDITORONLY_DATA
		DataInterfaceSkeletalMesh->Source = Character;
	}

	// [ 2019-11-11 : animal14 ] 
	NiagaraComponent->ResetSystem();
}

void AWRVFX::SetNiagaraVariableActor(const FString& InVariableName, AActor* InActor)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableActor(InVariableName, InActor);
}

AActor* AWRVFX::GetNiagaraVariableActor(const FString& InVariableName)
{
	return nullptr;
}

void AWRVFX::SetNiagaraVariableBool(const FString& InVariableName, bool InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableBool(InVariableName, InValue);
}

bool AWRVFX::GetNiagaraVariableBool(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return false;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<bool>(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableFloat(const FString& InVariableName, float InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableFloat(InVariableName, InValue);
}

float AWRVFX::GetNiagaraVariableFloat(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return 0.0F;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<float>(FNiagaraVariable(FNiagaraTypeDefinition::GetFloatDef(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableInt(const FString& InVariableName, int32 InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableInt(InVariableName, InValue);
}

int32 AWRVFX::GetNiagaraVariableInt(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return 0;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<int32>(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableLinearColor(const FString& InVariableName, const FLinearColor& InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableLinearColor(InVariableName, InValue);
}

FLinearColor AWRVFX::GetNiagaraVariableLinearColo(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return FLinearColor::White;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<FLinearColor>(FNiagaraVariable(FNiagaraTypeDefinition::GetColorDef(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableQuat(const FString& InVariableName, const FQuat& InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableQuat(InVariableName, InValue);
}

FQuat AWRVFX::GetNiagaraVariableQuat(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return FQuat::Identity;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<FQuat>(FNiagaraVariable(FNiagaraTypeDefinition::GetQuatDef(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableVec2(const FString& InVariableName, FVector2D InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableVec2(InVariableName, InValue);
}

FVector2D AWRVFX::GetNiagaraVariableVec2(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return FVector2D::ZeroVector;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<FVector2D>(FNiagaraVariable(FNiagaraTypeDefinition::GetVec2Def(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableVec3(const FString& InVariableName, FVector InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableVec3(InVariableName, InValue);
}

FVector AWRVFX::GetNiagaraVariableVec3(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return FVector::ZeroVector;
	} 
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<FVector>(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), *InVariableName));
}

void AWRVFX::SetNiagaraVariableVec4(const FString& InVariableName, const FVector4& InValue)
{
	UNiagaraComponent* NiagaraComponent = this->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}
	NiagaraComponent->SetNiagaraVariableVec4(InVariableName, InValue);
}

FVector4 AWRVFX::GetNiagaraVariableVec4(const FString& InVariableName)
{
	UNiagaraComponent* NiagaraComponent = GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return FLinearColor::White;
	}
	return NiagaraComponent->GetOverrideParameters().GetParameterValue<FVector4>(FNiagaraVariable(FNiagaraTypeDefinition::GetVec4Def(), *InVariableName));
}
#else // WR_EFFECT_NIAGARA
#endif // WR_EFFECT_NIAGARA

void AWRVFX::SetActorParameter(const FName& InParameterName, class AActor* InParam)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetActorParameter(InParameterName, InParam);
}

bool AWRVFX::GetActorParameter(const FName& InParameterName, class AActor*& OutActor)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return false;
	}
	return ParticleSystemComponent->GetActorParameter(InParameterName, OutActor);
}

void AWRVFX::SetColorParameter(const FName& InParameterName, const FLinearColor& InParam)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetColorParameter(InParameterName, InParam);
}

bool AWRVFX::GetColorParameter(const FName& InParameterName, FLinearColor& OutColor)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return false;
	}
	return ParticleSystemComponent->GetColorParameter(InParameterName, OutColor);
}

void AWRVFX::SetFloatParameter(const FName& InParameterName, const float InParam)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetFloatParameter(InParameterName, InParam);
}

bool AWRVFX::GetFloatParameter(const FName& InParameterName, float& OutFloat)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return false;
	}
	return ParticleSystemComponent->GetFloatParameter(InParameterName, OutFloat);
}

void AWRVFX::SetFloatRandParameter(const FName& InParameterName, const float InParam, const float InParamLow)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetFloatRandParameter(InParameterName, InParam, InParamLow);
}

void AWRVFX::SetVectorParameter(const FName& InParameterName, const FVector& InParam)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetVectorParameter(InParameterName, InParam);
}

bool AWRVFX::GetVectorParameter(const FName& InParameterName, FVector& OutVector)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return false;
	}
	return ParticleSystemComponent->GetVectorParameter(InParameterName, OutVector);
}

void AWRVFX::SetVectorRandParameter(const FName& InParameterName, const FVector& InParam, const FVector& InParamLow)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetVectorRandParameter(InParameterName, InParam, InParamLow);
}

void AWRVFX::SetMaterialParameter(const FName& InParameterName, class UMaterialInterface* InParam)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	ParticleSystemComponent->SetMaterialParameter(InParameterName, InParam);
}

bool AWRVFX::GetMaterialParameter(const FName& InParameterName, class UMaterialInterface*& OutMaterial)
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return false;
	}
	return ParticleSystemComponent->GetMaterialParameter(InParameterName, OutMaterial);
}

void AWRVFX::SetAdditionalAngle(const FRotator& InAngle)
{
	UWRVFXComponentMove* VFXComponentMove = this->GetCastedComponent<UWRVFXComponentMove>(EWRVFXComponent::Move);
	if (VFXComponentMove == nullptr)
	{
		return;
	}
	VFXComponentMove->SetAdditionalAngle(InAngle);
}

void AWRVFX::BuildComponents()
{
	UWRVFXComponentAttach* AttachComponent = this->GetComponentBy<UWRVFXComponentAttach>(EWRVFXComponent::Attach);
	if (AttachComponent)
	{
		AttachComponent->SetFXComponentType(EWRFXComponent::VFXAttach);
	}
	UWRVFXComponentMove* MoveComponent = this->GetComponentBy<UWRVFXComponentMove>(EWRVFXComponent::Move);
	if (MoveComponent)
	{
		MoveComponent->SetFXComponentType(EWRFXComponent::VFXMove);
	}
	UWRVFXComponentSound* SoundComponent = this->GetComponentBy<UWRVFXComponentSound>(EWRVFXComponent::Sound);
	if (SoundComponent)
	{
		SoundComponent->SetFXComponentType(EWRFXComponent::VFXSound);
	}
	UWRVFXComponentSpawn* SpawnComponent = this->GetComponentBy<UWRVFXComponentSpawn>(EWRVFXComponent::Spawn);
	if (SpawnComponent)
	{
		SpawnComponent->SetFXComponentType(EWRFXComponent::VFXSpawn);
	}
	UWRVFXComponentLaser* LaserComponent = this->GetComponentBy<UWRVFXComponentLaser>(EWRVFXComponent::Laser);
	if (LaserComponent)
	{
		LaserComponent->SetFXComponentType(EWRFXComponent::VFXLaser);
	}

	UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(GetComponentByClass(UNiagaraComponent::StaticClass()));
	if (NiagaraComponent)
	{
		if (Components.Contains(EWRVFXComponent::FX))
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Key already exist : FX"), TEXT("AWRVFX::BuildComponents()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		}
		else
		{
			NiagaraComponent->OnSystemFinished.AddUniqueDynamic(this, &AWRVFX::OnNiagaraSystemFinished);
			NiagaraComponent->AddToRoot();

			this->AddOwnedComponent(NiagaraComponent);
			Components.Add(EWRVFXComponent::FX, NiagaraComponent);
		}
	}
	UParticleSystemComponent* ParticleSystemComponent = Cast<UParticleSystemComponent>(GetComponentByClass(UParticleSystemComponent::StaticClass()));
	if (ParticleSystemComponent)
	{
		if (Components.Contains(EWRVFXComponent::FX))
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Key already exist : FX"), TEXT("AWRVFX::BuildComponents()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		}
		else
		{
			//ParticleSystemComponent->OnParticleCollide.AddUniqueDynamic(this, &AWRVFX::OnParticleCollide);
			ParticleSystemComponent->OnSystemFinished.AddUniqueDynamic(this, &AWRVFX::OnSystemFinished);
			ParticleSystemComponent->AddToRoot();

			this->AddOwnedComponent(ParticleSystemComponent);
			Components.Add(EWRVFXComponent::FX, ParticleSystemComponent);
		}
	}

	this->GetComponentBy<UCapsuleComponent>(EWRVFXComponent::Collision);
	this->GetComponentBy<USplineComponent>(EWRVFXComponent::Spline);

	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (SkeletalMeshComponent)
	{
		if (Components.Contains(EWRVFXComponent::Mesh))
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Key already exist : Mesh"), TEXT("AWRVFX::BuildComponents()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		}
		else
		{
			SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			this->AddOwnedComponent(SkeletalMeshComponent);
			Components.Add(EWRVFXComponent::Mesh, SkeletalMeshComponent);
		}
	}
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (StaticMeshComponent)
	{
		if (Components.Contains(EWRVFXComponent::Mesh))
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Key already exist : Mesh"), TEXT("AWRVFX::BuildComponents()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		}
		else
		{
			StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			this->AddOwnedComponent(StaticMeshComponent);
			Components.Add(EWRVFXComponent::Mesh, StaticMeshComponent);
		}
	}
}

void AWRVFX::Check()
{
	if (VFX.Lifespan < 0.0F)
	{
	}
	else
	{
		if (WREnumUtility::IsBit((uint64)Bits, (uint64)EWRVFX::Component))
		{
#if UE_EDITOR
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(this));

			FString Text = FString::Format(TEXT("This is Component VFX {0}."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::Check()"));
#else // UE_EDITOR
#endif // UE_EDITOR
			VFX.Lifespan = WR_INFINITE_LIFESPAN;
		}
	}

#if UE_EDITOR
	for (auto& VFXParameter : VFXParameters)
	{
		if (VFXParameter.Duration < 0.0F)
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(this));

			FString Text = FString::Format(TEXT("Duration {0} is less than 0 seconds"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::Component()"));
		}
		else
		{
		}

		if (VFXParameter.ParameterName == NAME_None)
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(this));

			FString Text = FString::Format(TEXT("Parameter Name {0} is None"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::Component()"));
		}

		if (VFX.Lifespan < 0.0F)
		{
		}
		else
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(this));

			FString Text = FString::Format(TEXT("Lifespan {0} is not Infinite"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::Component()"));
		}

		if (WREnumUtility::IsBit((uint64)Bits, (uint64)EWRVFX::Component))
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(this));

			FString Text = FString::Format(TEXT("This is Component VFX {0}."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::Component()"));
		}
	}
#else // UE_EDITOR
#endif // UE_EDITOR
}

void AWRVFX::Adopted(AWRVFX* InActor)
{
	if (InActor == nullptr)
	{
		return;
	}

	InActor->Adopting(this);
}

void AWRVFX::Adopting(AWRVFX* InActor)
{
	if (InActor == nullptr)
	{
		return;
	}

	uint32 UID = InActor->GetUniqueID();
	if (Childs.Contains(UID))
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Child already exist"), TEXT("AWRVFX::Adopt()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}

	Childs.Add(UID, InActor);
}

void AWRVFX::Abandon(AWRVFX* InActor)
{
	if (InActor == nullptr)
	{
		return;
	}

	uint32 UID = InActor->GetUniqueID();
	if (Childs.Contains(UID) == false)
	{
		return;
	}
	Childs.Remove(UID);
}

void AWRVFX::Abandon()
{
	Parent = nullptr;
}

bool AWRVFX::IsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(OtherActor))
	{
		AActor* CollisionCharacter = Cast<AActor>(OtherActor);
		if (CollisionCharacter == nullptr)
		{
			return false;
		}
		return true;
	}
	else
#else // UE_EDITOR
#endif // UE_EDITOR
	{
		//시전자가 죽은 상태면 충돌 일어났을때 바로 사라지게 처리.
		if (nullptr == Spawner)
			return true;

		if (OtherActor == Spawner)
		{
			return false;
		}

		AWRCharacter* CollisionCharacter = Cast<AWRCharacter>(OtherActor);
		if (CollisionCharacter == nullptr)
		{
			return false;
		}

		//팩션 체크
		EWRFactionRelation FactionRelation = CollisionCharacter->GetRelationToTarget(Cast<AWRCharacter>(Spawner));
		if (FactionRelation != EWRFactionRelation::Hostile)
		{
			return false;
		}

		//CollisionCharacter->OnBeginOverlapCapsuleHitting(OtherComp, this, OverlappedComponent, INDEX_NONE, bFromSweep, bFromSweep ? FHitResult::GetReversedHit(SweepResult) : SweepResult);

		return true;
	}
}

void AWRVFX::OnComponentBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult)
{
	UWRVFXComponentSpawn* SpawnComponent = this->GetCastedComponent<UWRVFXComponentSpawn>(EWRVFXComponent::Spawn);
	if (SpawnComponent)
	{
		SpawnComponent->OnComponentBeginOverlap();
	}
	
	switch (CollisionResult)
	{
	case EWRVFXCollisionResult::Destroy:
	{
#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(this->GetSpawner()))
		{
			this->Destroy();
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			// [ 2020-2-12 : animal14 ] 다음 틱에 삭제
			WRVFXManager::Get()->Remove(this, false ,true);
		}
	}
	break;
	case EWRVFXCollisionResult::Attach:
	{
		UWRVFXComponentMove* MoveComponent = this->GetCastedComponent<UWRVFXComponentMove>(EWRVFXComponent::Move);
		if (MoveComponent)
		{
			MoveComponent->Attach();
		}
	}
	break;
	case EWRVFXCollisionResult::Stop:
	{
		UWRVFXComponentMove* MoveComponent = this->GetCastedComponent<UWRVFXComponentMove>(EWRVFXComponent::Move);
		if (MoveComponent)
		{
			MoveComponent->Stop();
		}
	}
	break;
	//case EWRVFXCollisionResult::Propagation:
	//{
	//}
	//break;
	}
}

void AWRVFX::OnNiagaraSystemFinished(class UNiagaraComponent * InComponent)
{
	bool IsSpawned = VFX.Tick(0.0F);
	if (IsSpawned)
	{
	}
	else
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(this));

		FString Text = FString::Format(TEXT("Lifespan of BP {0} is larger than Lifespan of VFX"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::OnNiagaraSystemFinished()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}
}

void AWRVFX::OnSystemFinished(class UParticleSystemComponent* InComponent)
{
	bool IsSpawned = VFX.Tick(0.0F);
	if (IsSpawned)
	{
	}
	else
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(this));

		FString Text = FString::Format(TEXT("Lifespan of BP {0} is larger than Lifespan of VFX"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRVFX::OnSystemFinished()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}
}

//void AWRVFX::OnParticleCollide(FName InEventName, float InEmitterTime, int32 InParticleTime, FVector InLocation, FVector InVelocity, FVector InDirection, FVector InNormal, FName InBoneName, UPhysicalMaterial* InPhysMat)
//{
//	if (MaterialInstance.IsNull())
//	{
//		return;
//	}
//
//	FVector DecalSize = FVector::ZeroVector;
//	DecalSize.X = FMath::FRandRange(20.0F, 128.0F);
//	DecalSize.Y = FMath::FRandRange(20.0F, 128.0F);
//	DecalSize.Z = 256.0F;
//
//	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(this, MaterialInstance.LoadSynchronous(), DecalSize, InLocation, InNormal.ToOrientationRotator());
//	if (DecalComponent == nullptr)
//	{
//		return;
//	}
//
//	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::BloodDecal_Duration);
//	DecalComponent->SetFadeOut(0.0F, Duration);
//}

void AWRVFX::Tick4Parameter()
{
	for (auto& VFXParameter : VFXParameters)
	{
		if (VFXParameter.Duration < 0.0F)
		{
			continue;
		}
		else
		{
		}
		if (VFXParameter.ParameterName == NAME_None)
		{
			continue;
		}

		// [ 2019-9-10 : animal14 ] 페이드 인
		if (Type == EWRVFXFade::In)
		{
			// [ 2019-9-5 : animal14 ] Duration이 초과
			if (VFX.AccLifespan - LastLifespan > VFXParameter.Duration)
			{
				continue;
			}

			float Ratio = FMath::Clamp((VFX.AccLifespan - LastLifespan) / VFXParameter.Duration, 0.0F, 1.0F);

			switch (VFXParameter.Distribution)
			{
			case EWRVFXParameter::Scalar:
				this->SetFloatParameter(VFXParameter.ParameterName, FMath::Lerp(0.0F, VFXParameter.Scalar, Ratio));
				break;
			case EWRVFXParameter::Vector:
				this->SetVectorParameter(VFXParameter.ParameterName, FMath::Lerp(FVector::ZeroVector, VFXParameter.Vector, Ratio));
				break;
			}
		}

		// [ 2019-9-10 : animal14 ] 페이드 아웃
		if (Type == EWRVFXFade::Out)
		{
			// [ 2019-9-5 : animal14 ] Duration이 초과
			if (VFX.AccLifespan - LastLifespan > VFXParameter.Duration)
			{
				WRVFXManager::Get()->Remove(this);
				continue;
			}

			float Ratio = FMath::Clamp((VFX.AccLifespan - LastLifespan) / VFXParameter.Duration, 0.0F, 1.0F);

			switch (VFXParameter.Distribution)
			{
			case EWRVFXParameter::Scalar:
				this->SetFloatParameter(VFXParameter.ParameterName, FMath::Lerp(VFXParameter.LastScalar, 0.0F, Ratio));
				break;
			case EWRVFXParameter::Vector:
				this->SetVectorParameter(VFXParameter.ParameterName, FMath::Lerp(VFXParameter.LastVector, FVector::ZeroVector, Ratio));
				break;
			}
		}
	}
}

void AWRVFX::Tick4Mesh()
{
	this->Tick4Scale();
	this->Tick4Rotation();
}

void AWRVFX::Tick4Scale()
{
	if (SizeByLife == false)
	{
		return;
	}

	UMeshComponent* MeshComponent = this->GetCastedComponent<UMeshComponent>(EWRVFXComponent::Mesh);
	if (MeshComponent == nullptr)
	{
		return;
	}

	float Alpha = FMath::Clamp(VFX.AccLifespan / VFX.Lifespan, 0.0F, 1.0F);
	FVector Scale = FMath::Lerp(FromScale, ToScale, Alpha);

	MeshComponent->SetRelativeScale3D(Scale);
}

void AWRVFX::Tick4Rotation()
{
	if (Rotation == false)
	{
		return;
	}

	UMeshComponent* MeshComponent = this->GetCastedComponent<UMeshComponent>(EWRVFXComponent::Mesh);
	if (MeshComponent == nullptr)
	{
		return;
	}

	NewRotation += Axis * Rate;

	MeshComponent->SetRelativeRotation(NewRotation);
}

void AWRVFX::PostInitialize(const bool InShouldFadeParticle)
{
	if (InShouldFadeParticle == false)
	{
		return;
	}

	LastLifespan = VFX.AccLifespan;
	Type = EWRVFXFade::In;
}

void AWRVFX::PreDestroy()
{
	for (auto& VFXParameter : VFXParameters)
	{
		switch (VFXParameter.Distribution)
		{
		case EWRVFXParameter::Scalar:
			this->GetFloatParameter(VFXParameter.ParameterName, VFXParameter.LastScalar);
			break;
		case EWRVFXParameter::Vector:
			this->GetVectorParameter(VFXParameter.ParameterName, VFXParameter.LastVector);
			break;
		}
	}

	LastLifespan = VFX.AccLifespan;
	Type = EWRVFXFade::Out;
}

//EWRVFXMove AWRVFX::GetMovementType()
//{
//	UWRVFXComponentMove* MoveComponent = this->GetCastedComponent<UWRVFXComponentMove>(EWRVFXComponent::Move);
//	if (MoveComponent == nullptr)
//	{
//		return EWRVFXMove::None;
//	}
//	return MoveComponent->Type;
//}

bool FWRVFXs::Tick(float DeltaTime)
{
#if UE_EDITOR
	AccLifespan += DeltaTime;
#else // UE_EDITOR
#endif // UE_EDITOR
	if (Lifespan < 0.0F)
	{
		return false;
	}

#if UE_EDITOR
#else // UE_EDITOR
	AccLifespan += DeltaTime;
#endif // UE_EDITOR
	if (AccLifespan < Lifespan)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void FWRVFXs::Reset()
{
	AccLifespan = 0.0F;
}