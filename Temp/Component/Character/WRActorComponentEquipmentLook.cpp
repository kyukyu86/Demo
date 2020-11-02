// Copyright 2019-2024 WemadeXR Inc. All rights reserved.



#include "WRActorComponentEquipmentLook.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Component/Character/WRActorComponentConnectbyCable.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"

#include "Define/WRPathDefine.h"

#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WREquipmentLookTable.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Utility/WREnumUtility.h"

#include "Manager/WRCharacterManager.h"


DECLARE_CYCLE_STAT(TEXT("WRActorComponentEquipmentLook_Equip"), STAT_WRActorComponentEquipmentLook_Equip, STATGROUP_WRActorComponentEquipmentLook);

#define EQUIPMENTLOOK_SETMATERIAL_PARAMETER(ParamName, Value, Function, MeshComponent)			\
{																								\
	TArray<UMaterialInstanceDynamic*> MIDs;														\
																								\
	WRActorUtility::GetMeshMaterialInstanceDynamic(MeshComponent, MIDs);						\
																								\
	for (int32 i = 0; i < MIDs.Num(); ++i)														\
	{																							\
		UMaterialInstanceDynamic* MID = MIDs[i];												\
		if (MID != nullptr)																		\
		{																						\
			MID->Function(ParamName, Value);													\
		}																						\
	}																							\
}																								\

#define WR_LIGHT_SOCKETNAME "Light"
#define WR_DIRECT_MATERIAL_PARAMETERNAME "00.ClothChangingMain"

#if WITH_EDITOR
TMap<EWREquipPart, uint32> FWRMeshComponent::MeshComponentUIDs;
#else // WITH_EDITOR
#endif // WITH_EDITOR

DEFINE_LOG_CATEGORY(WRActorComponentEquipmentLook)

FWRMeshComponent::FWRMeshComponent(class AWRCharacter* InOwner) : OwnerCharacter(InOwner), SocketName(NAME_None), Ability(EWRAdditionalAbility::None)
{
}

FWRMeshComponent::~FWRMeshComponent()
{
	ClearMaterialInterface();
}

void FWRMeshComponent::Initialize(struct FWREquipmentLookTable* InTable)
{
	TID = InTable->GetTableID();
}

void FWRMeshComponent::SetMaterialParameterValue(FName InParameterName, float InValue, class UMeshComponent* InMeshComponent /*= nullptr*/)
{
	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InValue, SetScalarParameterValue, ((InMeshComponent == nullptr) ? GetMesh() : InMeshComponent));
}

void FWRMeshComponent::SetMaterialParameterValue(FName InParameterName, FLinearColor InValue, class UMeshComponent* InMeshComponent /*= nullptr*/)
{
	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InValue, SetVectorParameterValue, ((InMeshComponent == nullptr) ? GetMesh() : InMeshComponent));
}

void FWRMeshComponent::RestoreMaterialInterface()
{
	for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
	{
		UMaterialInterface* MaterialInterface = MaterialInterfaces[i];
		if (MaterialInterface == nullptr)
		{
			continue;
		}

		GetMesh()->SetMaterial(i, MaterialInterface);
	}
}

void FWRMeshComponent::SetMaterial(int32 ElementIndex, class UMaterialInterface* Material)
{
	if (MaterialInterfaces.Num() == 0)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MaterialInterface is not initialized."), TEXT("FWRMeshComponent::SetMaterial()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	GetMesh()->SetMaterial(ElementIndex, Material);
}

FTransform FWRMeshComponent::GetSocketTransform(ERelativeTransformSpace TransformSpace /*= RTS_World*/)
{
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::GetSocketTransform] OwnerCharacter == nullptr"));
		return FTransform::Identity;
	}

	UMeshComponent* MainMeshComponent = OwnerCharacter->GetMesh();
	if (MainMeshComponent == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::GetSocketTransform] MainMeshComponent == nullptr"));
		return FTransform::Identity;
	}

	return MainMeshComponent->GetSocketTransform(SocketName, TransformSpace);
}

bool FWRMeshComponent::IsVisible()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::GetSocketTransform()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return false;
	}
	return MeshComponent->IsVisible();
}

void FWRMeshComponent::SetVisibility(bool bNewVisibility)
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::SetVisibility()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	MeshComponent->SetVisibility(bNewVisibility);
}

void FWRMeshComponent::SetCollisionEnable(ECollisionEnabled::Type NewType)
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::SetCollisionEnable()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	MeshComponent->SetCollisionEnabled(NewType);
}

void FWRMeshComponent::SetBodyVisibility()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::SetBodyVisibility()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	BodyVisibility = MeshComponent->IsVisible();
}

void FWRMeshComponent::GetBodyVisibility()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::GetBodyVisibility()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	MeshComponent->SetVisibility(BodyVisibility);
}

void FWRMeshComponent::SettingEquipment(FName InSocketName, EWREquipPart InPart, EWRAdditionalAbility InAbility, const FString& InLight, const FString& InStaticMesh, const FString& In1st, const FString& In3rd)
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::SettingEquipment()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	if (IsMainMeshPart(InPart) == false)
	{
#if WITH_EDITOR
		MeshComponent->Rename(*MakeComponentName(InPart));
#else // WITH_EDITOR
#endif // WITH_EDITOR

		if (MeshComponent->IsRegistered() == false)
		{
			MeshComponent->RegisterComponentWithWorld(OwnerCharacter->GetWorld());
		}

		MeshComponent->CastShadow = true;
		MeshComponent->bCastDynamicShadow = true;

		OwnerCharacter->AddInstanceComponent(MeshComponent);

		USkeletalMeshComponent* MainMeshComponent = OwnerCharacter->GetMesh();
		if (MainMeshComponent == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::SettingEquipment] MainMeshComponent == nullptr"));
			return;
		}

		if (MeshComponent->IsAttachedTo(MainMeshComponent))
		{
			MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		}

		MeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
	}

	MeshComponent->bReceivesDecals = false;

	SettingEquipment(InPart, InStaticMesh, In1st, In3rd);

	if (IsWeaponMeshPart(InPart) == false)
	{
		return;
	}

	MeshComponent->SetCollisionProfileName(FName("WRAttackObject"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetGenerateOverlapEvents(true);
	if (MeshComponent->OnComponentBeginOverlap.Contains(OwnerCharacter, FName("OnBeginOverlapCapsuleAttacking")) == false)
	{
		MeshComponent->OnComponentBeginOverlap.AddDynamic(OwnerCharacter, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
	}

	ReleaseAbility();
	CreateAbility(InAbility, InLight);
}

void FWRMeshComponent::SettingEquipment(FName InSocketName, EWREquipPart InPart, EWRAdditionalAbility InAbility, const FString& InLight, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3rd)
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::SettingEquipment()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	if (IsMainMeshPart(InPart) == false)
	{
#if WITH_EDITOR
		MeshComponent->Rename(*MakeComponentName(InPart));
#else // WITH_EDITOR
#endif // WITH_EDITOR

		if (MeshComponent->IsRegistered() == false)
		{
			MeshComponent->RegisterComponentWithWorld(OwnerCharacter->GetWorld());
		}

		MeshComponent->CastShadow = true;
		MeshComponent->bCastDynamicShadow = true;

		OwnerCharacter->AddInstanceComponent(MeshComponent);

		USkeletalMeshComponent* MainMeshComponent = OwnerCharacter->GetMesh();
		if (MainMeshComponent == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::SettingEquipment] MainMeshComponent == nullptr"));
			return;
		}

		if (MeshComponent->IsAttachedTo(MainMeshComponent))
		{
			MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		}

		MeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
	}

	MeshComponent->bReceivesDecals = false;

	SettingEquipment(InPart, InStaticMesh, In1st, In3rd);

	if (IsWeaponMeshPart(InPart) == false)
	{
		return;
	}

	MeshComponent->SetCollisionProfileName(FName("WRAttackObject"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetGenerateOverlapEvents(true);
	if (MeshComponent->OnComponentBeginOverlap.Contains(OwnerCharacter, FName("OnBeginOverlapCapsuleAttacking")) == false)
	{
		MeshComponent->OnComponentBeginOverlap.AddDynamic(OwnerCharacter, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
	}

	ReleaseAbility();
	CreateAbility(InAbility, InLight);
}

#if WITH_EDITOR
FString FWRMeshComponent::MakeComponentName(EWREquipPart InPart)
{
	if (MeshComponentUIDs.Contains(InPart) == false)
	{
		MeshComponentUIDs.Add(InPart, 0);
	}

	uint32 UID = MeshComponentUIDs[InPart];
	MeshComponentUIDs[InPart] = ++UID;

	FString DisplayName = WREnumUtility::EnumToString<EWREquipPart>("EWREquipPart", InPart);

	TArray<FStringFormatArg> StringArgs;
	StringArgs.Add(DisplayName);
	StringArgs.Add(UID);

	FString RetVal = FString::Format(TEXT("{0}{1}"), StringArgs);

	return DisplayName;
}
#else // WITH_EDITOR
#endif // WITH_EDITOR

void FWRMeshComponent::CreateComponent()
{
}

void FWRMeshComponent::DestroyComponent()
{
	ReleaseAbility();

	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::DestroyComponent()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	if (OwnerCharacter == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OwnerCharacter is nullptr."), TEXT("FWRMeshComponent::DestroyComponent()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	OwnerCharacter->RemoveInstanceComponent(MeshComponent);

	MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComponent->UnregisterComponent();
	MeshComponent->ConditionalBeginDestroy();
}

void FWRMeshComponent::CreateAbility(EWRAdditionalAbility InAbility, const FString& InLight)
{
	if (Ability != EWRAdditionalAbility::None)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateAbility] %d != EWRAdditionalAbility::None"), Ability);
		return;
	}

	Ability = InAbility;

	switch (Ability)
	{
	case EWRAdditionalAbility::None:
		break;
	case EWRAdditionalAbility::Hook:
		{
			CreateHook();
		}
		break;
	case EWRAdditionalAbility::ShieldProjectile:
		break;
	case EWRAdditionalAbility::Light:
		{
			CreateLight(InLight);
		}
		break;
	}
}

void FWRMeshComponent::CreateHook()
{
	UWRActorComponentConnectbyCable* ConnectCableComponent = OwnerCharacter->GetComponent<UWRActorComponentConnectbyCable>();
	if (ConnectCableComponent == nullptr)
	{
		OwnerCharacter->CreateComponent<UWRActorComponentConnectbyCable>(EWRCharacterComponent::ConnectbyCable);
	}
}

void FWRMeshComponent::CreateLight(const FString& InLight)
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::CreateLight()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	if (MeshComponent->DoesSocketExist(WR_LIGHT_SOCKETNAME))
	{
		return;
	}

	UWorld* World = MeshComponent->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateLight] World == nullptr"));
		return;
	}

	FStringAssetReference AssetReference = FStringAssetReference(InLight);
	UWRLightValueAsset* LightValue = Cast<UWRLightValueAsset>(AssetReference.TryLoad());
	if (LightValue == nullptr)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(InLight);

		FString Text = FString::Format(TEXT("LightValue is nullptr. Check table data : {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("FWRMeshComponent::CreateLight()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	AActor* Light = nullptr;

	UPointLightComponent* PointLightComponent = nullptr;

	if (LightValue->bSpotLight)
	{
		ASpotLight* SpotLight = World->SpawnActor<ASpotLight>();
		if (SpotLight == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateLight] SpotLight == nullptr"));
			return;
		}

		SpotLight->SpotLightComponent->SetInnerConeAngle(LightValue->InnerConeAngle);
		SpotLight->SpotLightComponent->SetOuterConeAngle(LightValue->OuterConeAngle);
		SpotLight->SpotLightComponent->LightShaftConeAngle = LightValue->LightShaftConeAngle;

		PointLightComponent = Cast<UPointLightComponent>(SpotLight->SpotLightComponent);
		Light = SpotLight;
	}
	else
	{
		APointLight* PointLight = World->SpawnActor<APointLight>();
		if (PointLight == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateLight] PointLight == nullptr"));
			return;
		}

		PointLightComponent = PointLight->PointLightComponent;
		Light = PointLight;
	}

	if (Light == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateLight] Light == nullptr"));
		return;
	}
	if (PointLightComponent == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRMeshComponent::CreateLight] PointLightComponent == nullptr"));
		return;
	}

	PointLightComponent->SetMobility(EComponentMobility::Movable);
	PointLightComponent->bUseInverseSquaredFalloff = LightValue->bUseInverseSquaredFalloff;
	PointLightComponent->SetLightFalloffExponent(LightValue->LightFalloffExponent);
	PointLightComponent->SetAttenuationRadius(LightValue->AttenuationRadius);
	PointLightComponent->SetSourceRadius(LightValue->SourceRadius);
	PointLightComponent->bUseTemperature = LightValue->bUseTemperature;
	PointLightComponent->SetIntensity(LightValue->Intensity);
	PointLightComponent->SetLightColor(LightValue->Color);
	PointLightComponent->SetLightFunctionMaterial(LightValue->LightFunctionMaterial.LoadSynchronous());

	if (PointLightComponent->bUseInverseSquaredFalloff)
	{
		PointLightComponent->IntensityUnits = ELightUnits::Candelas;
	}
	else
	{
		PointLightComponent->IntensityUnits = ELightUnits::Unitless;
	}

	if (PointLightComponent->bUseTemperature)
	{
		PointLightComponent->SetTemperature(LightValue->Temperature);
	}
	else
	{
	}

	Light->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WR_LIGHT_SOCKETNAME);

#if WITH_EDITOR
	OwnerCharacter->AddInstanceComponent(PointLightComponent);
	PointLightComponent->bEditableWhenInherited = true;
#else // WITH_EDITOR
#endif // WITH_EDITOR
}

void FWRMeshComponent::ReleaseAbility()
{
	switch (Ability)
	{
	case EWRAdditionalAbility::None:
		break;
	case EWRAdditionalAbility::Hook:
		break;
	case EWRAdditionalAbility::ShieldProjectile:
		break;
	case EWRAdditionalAbility::Light:
		{
			ReleaseLight();
		}
		break;
	}

	Ability = EWRAdditionalAbility::None;
}

void FWRMeshComponent::ReleaseHook()
{
	// [ 2020-6-25 : animal14 ] 캐릭터 컴포넌트는 캐릭터 내부에서 해제하므로 ReleaseHook 내부 구현 불필요.
}

void FWRMeshComponent::ReleaseLight()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::ReleaseLight()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	const TArray<USceneComponent*> Childs = MeshComponent->GetAttachChildren();
	for (auto& Child : Childs)
	{
		APointLight* PointLight = Cast<APointLight>(Child->GetOwner());
		if (PointLight == nullptr)
		{
			continue;
		}

#if WITH_EDITOR
		OwnerCharacter->RemoveInstanceComponent(PointLight->PointLightComponent);
#else // WITH_EDITOR
#endif // WITH_EDITOR

		PointLight->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		PointLight->SetActorHiddenInGame(true);
		PointLight->ConditionalBeginDestroy();
	}
}

void FWRMeshComponent::CleanupOverrideMaterials()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MeshComponent is nullptr."), TEXT("FWRMeshComponent::CleanupOverrideMaterials()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	MeshComponent->EmptyOverrideMaterials();
}

void FWRMeshComponent::SetMaterialInterface()
{
	ClearMaterialInterface();

	for (auto& MaterialInterface : GetMesh()->GetMaterials())
	{
		MaterialInterfaces.Add(MaterialInterface);
	}
}

void FWRMeshComponent::ClearMaterialInterface()
{
	MaterialInterfaces.Empty();
}

void FWRMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3rd)
{
}

void FWRMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3rd)
{
}

FWRStaticMeshComponent::FWRStaticMeshComponent(class AWRCharacter* InOwner, struct FWREquipmentLookTable* InTable) : FWRMeshComponent(InOwner)
{
	CreateComponent();
}

FWRStaticMeshComponent::~FWRStaticMeshComponent()
{
	DestroyComponent();

	StaticMeshComponent = nullptr;
}

void FWRStaticMeshComponent::SetMesh(class UStaticMesh* InMesh)
{
	checkf(StaticMeshComponent != nullptr, TEXT("[FWRStaticMeshComponent::SetMesh] StaticMeshComponent == nullptr"));

	FWRMeshComponent::SetMesh(InMesh);
	StaticMeshComponent->SetStaticMesh(InMesh);

	SetMaterialInterface();
}

void FWRStaticMeshComponent::SetMesh()
{
	checkf(StaticMeshComponent != nullptr, TEXT("[FWRStaticMeshComponent::SetMesh] StaticMeshComponent == nullptr"));

	FWRMeshComponent::SetMesh();
	StaticMeshComponent->SetStaticMesh(nullptr);
}

void FWRStaticMeshComponent::CreateComponent()
{
	if (StaticMeshComponent)
	{
		return;
	}

	StaticMeshComponent = NewObject<UStaticMeshComponent>(OwnerCharacter);
	if (StaticMeshComponent == nullptr)
	{
		return;
	}

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// [ 2020-9-3 : animal14 ] 임시 코드
	if (OwnerCharacter->IsMe() == false)
	{
		return;
	}

	StaticMeshComponent->SetRenderCustomDepth(true);
}

void FWRStaticMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3rd)
{
	SettingEquipment(InPart, InStaticMesh, nullptr, nullptr);
}

void FWRStaticMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3rd)
{
	UStaticMesh* StaticMesh = FindObject<UStaticMesh>(nullptr, *InStaticMesh);
	if (StaticMesh == nullptr)
	{
		StaticMesh = LoadObject<UStaticMesh>(nullptr, *InStaticMesh);
	}

	SetMesh(StaticMesh);

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
	if (MyCharacter)
	{
		if (IsHeadPart(InPart))
		{
			WRActorUtility::SetVisibility(StaticMeshComponent, false);
		}
	}
}

FWRSkeletalMeshComponent::FWRSkeletalMeshComponent(AWRCharacter* InOwner, struct FWREquipmentLookTable* InTable) : FWRMeshComponent(InOwner)
{
	EquipmentPart = InTable->EquipPart;
	if (IsMainMeshPart(EquipmentPart) == false)
	{
		CreateComponent();
	}
	else
	{
		SkeletalMeshComponent = InOwner->GetMesh();
	}
}

FWRSkeletalMeshComponent::~FWRSkeletalMeshComponent()
{
	if (IsMainMeshPart(EquipmentPart) == false)
	{
		DestroyComponent();
	}

	SkeletalMeshComponent = nullptr;
}

void FWRSkeletalMeshComponent::SetMesh(class USkeletalMesh* InMesh)
{
	checkf(SkeletalMeshComponent != nullptr, TEXT("[FWRSkeletalMeshComponent::SetMesh] SkeletalMeshComponent == nullptr"));

	FWRMeshComponent::SetMesh(InMesh);
	SkeletalMeshComponent->SetSkeletalMesh(InMesh, false);

	SetMaterialInterface();
}

void FWRSkeletalMeshComponent::SetMesh()
{
	checkf(SkeletalMeshComponent != nullptr, TEXT("[FWRSkeletalMeshComponent::SetMesh] SkeletalMeshComponent == nullptr"));

	FWRMeshComponent::SetMesh();
	SkeletalMeshComponent->SetSkeletalMesh(nullptr, true);
}

void FWRSkeletalMeshComponent::CreateComponent()
{
	if (SkeletalMeshComponent)
	{
		return;
	}

	SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(OwnerCharacter);
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// [ 2020-9-3 : animal14 ] 임시 코드
	if (OwnerCharacter->IsMe() == false)
	{
		return;
	}

	SkeletalMeshComponent->SetRenderCustomDepth(true);
}

void FWRSkeletalMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3rd)
{
	if (EquipmentPart != InPart)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("EquipmentPart != InPart"), TEXT("FWRSkeletalMeshComponent::SettingEquipment()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}

	USkeletalMesh* FirstSkeletalMesh = nullptr;
	if (In1st.IsEmpty() == false)
	{
		FirstSkeletalMesh = WRActorSpawnUtility::FindOrLoadObject<USkeletalMesh>(In1st);
	}

	USkeletalMesh* ThirdSkeletalMesh = nullptr;
	if (In3rd.IsEmpty() == false)
	{
		ThirdSkeletalMesh = WRActorSpawnUtility::FindOrLoadObject<USkeletalMesh>(In3rd);
	}

	SettingEquipment(InPart, InStaticMesh, FirstSkeletalMesh, ThirdSkeletalMesh);
}

void FWRSkeletalMeshComponent::SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3rd)
{
	if (EquipmentPart != InPart)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("EquipmentPart != InPart"), TEXT("FWRSkeletalMeshComponent::SettingEquipment()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}

	if (IsMainMeshPart(EquipmentPart))
	{
		AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
		if (MyCharacter)
		{
			SetMesh(In1st);

			MyCharacter->Set3rdSkeletalMesh(In3rd);
		}
		else
		{
			SetMesh(In3rd);
		}
	}
	else
	{
		SetMesh(In3rd);

		USkeletalMeshComponent* MainMeshComponent = OwnerCharacter->GetMesh();
		if (MainMeshComponent == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWRSkeletalMeshComponent::SettingEquipment] MainMeshComponent == nullptr"));
			return;
		}

		if (SkeletalMeshComponent == MainMeshComponent)
		{
#ifdef UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SkeletalMeshComponent == MainMeshComponent"), TEXT("FWRSkeletalMeshComponent::SettingEquipment()"));
#else // UE_EDITOR
#endif // UE_EDITOR
			return;
		}

		SkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);

		AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
		if (MyCharacter)
		{
			if (IsFirstPersonPart(EquipmentPart) == false)
			{
				WRActorUtility::SetVisibility(SkeletalMeshComponent, false);
			}
		}
	}
}

FWREquipmentAsyncTask::FWREquipmentAsyncTask()
{
	Table = nullptr;
}

FWREquipmentAsyncTask::FWREquipmentAsyncTask(const FString& InHash) : FWREquipmentAsyncTask()
{
	AddHash(InHash);
}

FWREquipmentAsyncTask::~FWREquipmentAsyncTask()
{
	for (auto& Object : Objects)
	{
		if (Object.Value)
		{
			Object.Value->RemoveFromRoot();
		}
		else
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(Object.Key);
		}
	}
	Objects.Empty();

	Table = nullptr;

	CompleteDelegate.Unbind();
}

FWREquipmentDirection::FWREquipmentDirection()
{
	// [ 2020-7-29 : animal14 ] BeginPlay로 위치 변경
	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FWREquipmentDirection::Tick));
}

FWREquipmentDirection::~FWREquipmentDirection()
{
	// [ 2020-7-29 : animal14 ] OnPostDirectionComplete로 위치 변경
	if (TickHandle.IsValid() == false)
	{
		return;
	}
	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
	TickHandle.Reset();
}

bool FWREquipmentDirection::BeginPlay(class AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, struct FWREquipmentLookTable* InTable, const FName& InSocketName, FWREquipmentDirectionComplete InDelegate, USkeletalMesh* In1st, USkeletalMesh* In3rd)
{
	if (InCharacter == nullptr)
	{
		return false;
	}

	UWorld* World = InCharacter->GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	if (InTable == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWREquipmentDirection::BeginPlay] InTable == nullptr"));
		return false;
	}

	FWREquipmentDirectionData* DirectionData = new FWREquipmentDirectionData();
	if (DirectionData == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWREquipmentDirection::BeginPlay] DirectionData == nullptr"));
		return false;
	}

	UCurveFloat* SrcCurve = InTable->ChangeCurveSRC.LoadSynchronous();
	if (SrcCurve)
	{
		SrcCurve->AddToRoot();

		DirectionData->SrcCurve = SrcCurve;
	}
	UCurveFloat* DestCurve = InTable->ChangeCurveDEST.LoadSynchronous();
	if (DestCurve)
	{
		DestCurve->AddToRoot();

		DirectionData->DestCurve = DestCurve;
	}

	USkeletalMeshComponent* MainMeshComponent = InCharacter->GetCurrentSkeletalMeshComponent();
	if (MainMeshComponent == nullptr)
	{
		return false;
	}

	bool IsExchanging = false;

	EWREquipPart EquipmentPart = InTable->EquipPart;
	if (IsStaticMesh(EquipmentPart))
	{
		UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(InCharacter);
		if (StaticMeshComponent == nullptr)
		{
			return false;
		}
		StaticMeshComponent->RegisterComponentWithWorld(World);

		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
		if (StaticMesh == nullptr)
		{
			return false;
		}

		StaticMeshComponent->SetStaticMesh(StaticMesh);
		StaticMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);

		DirectionData->NextMeshComponent = StaticMeshComponent;

		IsExchanging = Cast<UStaticMeshComponent>(InCurrentMeshComponent)->GetStaticMesh() ? true : false;
	}
	else
	{
		USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(InCharacter);
		if (SkeletalMeshComponent == nullptr)
		{
			return false;
		}
		SkeletalMeshComponent->RegisterComponentWithWorld(World);

		USkeletalMesh* SkeletalMesh = IsMergedMesh(In1st, In3rd) ? In3rd : WRActorSpawnUtility::FindOrLoadObject<USkeletalMesh>(InTable->SkeletalMesh);
		if (SkeletalMesh == nullptr)
		{
			return false;
		}

		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
		SkeletalMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

		USkeletalMeshComponent* CurrentMeshComponent = Cast<USkeletalMeshComponent>(InCurrentMeshComponent);
		if (CurrentMeshComponent == nullptr)
		{
			return false;
		}

		IsExchanging = CurrentMeshComponent->SkeletalMesh ? true : false;
		if (IsExchanging)
		{
			// [ 2020-6-30 : animal14 ] 장비 교체
		}
		else
		{
			// [ 2020-6-30 : animal14 ] 장비 장착
			CurrentMeshComponent->SetSkeletalMesh(SkeletalMesh);
		}

		SkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);

		DirectionData->NextMeshComponent = SkeletalMeshComponent;
	}

	DirectionData->AccDeltaTime = 0.0F;
	DirectionData->Character = InCharacter;
	DirectionData->Table = InTable;
	DirectionData->SocketName = InSocketName;
	DirectionData->CurrentMeshComponent = InCurrentMeshComponent;
	DirectionData->DirectionDelegate = InDelegate;
	DirectionData->FirstMergedMesh = In1st;
	DirectionData->ThirdMergedMesh = In3rd;

	if (IsExchanging)
	{
		// [ 2020-6-30 : animal14 ] 장비 교체
		DirectionData->Action = EWREquipmentLookAction::Exchange;

		// [ 2020-6-29 : animal14 ] 연출
		EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 1.0F, SetScalarParameterValue, DirectionData->CurrentMeshComponent);
		EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 0.0F, SetScalarParameterValue, DirectionData->NextMeshComponent);
	}
	else
	{
		// [ 2020-6-30 : animal14 ] 장비 장착
		DirectionData->Action = EWREquipmentLookAction::Equipment;

		// [ 2020-6-29 : animal14 ] 연출
		EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 0.0F, SetScalarParameterValue, DirectionData->CurrentMeshComponent);
		EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 0.0F, SetScalarParameterValue, DirectionData->NextMeshComponent);
	}

	DirectionDatas.AddTail(DirectionData);

	return true;
}

bool FWREquipmentDirection::BeginPlay(class AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, EWREquipPart InPart, FWREquipmentDirectionComplete InDelegate)
{
	FWREquipmentDirectionData* DirectionData = new FWREquipmentDirectionData();
	if (DirectionData == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWREquipmentDirection::BeginPlay] Data == nullptr"));
		return false;
	}

	UCurveFloat* SrcCurve = WRActorSpawnUtility::FindOrLoadObject<UCurveFloat>(CURVE_PATH_4_UNEQUIPMENT);
	if (SrcCurve)
	{
		SrcCurve->AddToRoot();

		DirectionData->SrcCurve = SrcCurve;
	}

	DirectionData->AccDeltaTime = 0.0F;
	DirectionData->Character = InCharacter;
	DirectionData->Table = nullptr;
	DirectionData->EquipmentPart = InPart;
	DirectionData->SocketName = NAME_None;
	DirectionData->CurrentMeshComponent = InCurrentMeshComponent;
	DirectionData->DirectionDelegate = InDelegate;
	DirectionData->FirstMergedMesh = nullptr;
	DirectionData->ThirdMergedMesh = nullptr;

	// [ 2020-6-30 : animal14 ] 장비 해제
	DirectionData->Action = EWREquipmentLookAction::Unequipment;

	// [ 2020-6-30 : animal14 ] 연출
	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 1.0F, SetScalarParameterValue, DirectionData->CurrentMeshComponent);

	DirectionDatas.AddTail(DirectionData);

	return true;
}

void FWREquipmentDirection::StopPlay()
{
	if (IsDirecting() == false)
	{
		//UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[FWREquipmentDirection::StopPlay] Directing == false"));
		return;
	}

	for (auto& DirectionData : DirectionDatas)
	{
		OnPreDirectionComplete(DirectionData);
		OnPostDirectionComplete(DirectionData);
	}
	DirectionDatas.Empty();
}

bool FWREquipmentDirection::Tick(float DeltaTime)
{
	for (auto& DirectionData : DirectionDatas)
	{
		DirectionData->AccDeltaTime += DeltaTime;

		float SrcMaxTime = 0.0F;
		UCurveFloat* SrcCurve = DirectionData->SrcCurve;
		if (SrcCurve)
		{
			SrcCurve->GetTimeRange(SrcMaxTime, SrcMaxTime);
		}

		float DestMaxTime = 0.0F;
		UCurveFloat* DestCurve = DirectionData->DestCurve;
		if (DestCurve)
		{
			DestCurve->GetTimeRange(DestMaxTime, DestMaxTime);
		}

		float SrcValue = SrcCurve ? SrcCurve->GetFloatValue(DirectionData->AccDeltaTime) : 0.0F;
		float DestValue = DestCurve ? DestCurve->GetFloatValue(DirectionData->AccDeltaTime) : 0.0F;

		switch (DirectionData->Action)
		{
		case EWREquipmentLookAction::Equipment:
			{
				// [ 2020-6-29 : animal14 ] 연출
				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, DestValue, SetScalarParameterValue, DirectionData->NextMeshComponent);
			}
			break;
		case EWREquipmentLookAction::Exchange:
			{
				// [ 2020-6-29 : animal14 ] 연출
				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, SrcValue, SetScalarParameterValue, DirectionData->CurrentMeshComponent);
				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, DestValue, SetScalarParameterValue, DirectionData->NextMeshComponent);
			}
			break;
		case EWREquipmentLookAction::Unequipment:
			{
				// [ 2020-6-29 : animal14 ] 연출
				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, SrcValue, SetScalarParameterValue, DirectionData->CurrentMeshComponent);
			}
			break;
		}

		if (DirectionData->AccDeltaTime < FMath::Max(SrcMaxTime, DestMaxTime))
		{
			continue;
		}

		// [ 2020-6-26 : animal14 ] 완료 이벤트 전처리
		OnPreDirectionComplete(DirectionData);

		// [ 2020-6-26 : animal14 ] 완료 이벤트
		if (DirectionData->Table)
		{
			EWREquipPart EquipmentPart = DirectionData->Table->EquipPart;
			FName SocketName = DirectionData->SocketName;
			EWRAdditionalAbility Ability = DirectionData->Table->AdditionalAbility;
			FString Light = DirectionData->Table->LightValue.ToString();
			FString StaticMesh = DirectionData->Table->StaticMesh;
			FString FirstSkeletalMesh = DirectionData->Table->FirstPersonSkeletalMesh;
			FString ThirdSkeletalMesh = DirectionData->Table->SkeletalMesh;
			USkeletalMesh* FirstMergedMesh = DirectionData->FirstMergedMesh;
			USkeletalMesh* ThirdMergedMesh = DirectionData->ThirdMergedMesh;

			// [ 2020-6-29 : animal14 ] 삽입
			DirectionData->DirectionDelegate.ExecuteIfBound(EquipmentPart, SocketName, Ability, Light, StaticMesh, FirstSkeletalMesh, ThirdSkeletalMesh, FirstMergedMesh, ThirdMergedMesh);
		}
		else
		{
			EWREquipPart EquipmentPart = DirectionData->EquipmentPart;

			// [ 2020-6-29 : animal14 ] 삭제
			DirectionData->DirectionDelegate.ExecuteIfBound(EquipmentPart, NAME_None, EWRAdditionalAbility::None, "", "", "", "", nullptr, nullptr);
		}

		// [ 2020-6-26 : animal14 ] 완료 이벤트 후처리
		OnPostDirectionComplete(DirectionData);

		delete DirectionData;
		//DirectionData = nullptr;

		DirectionDatas.RemoveNode(DirectionData);
	}

	return true;
}

void FWREquipmentDirection::OnPreDirectionComplete(FWREquipmentDirectionData* InData)
{
	// [ 2020-6-29 : animal14 ] 연출
	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 1.0F, SetScalarParameterValue, InData->CurrentMeshComponent);

	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InData->NextMeshComponent);
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetMasterPoseComponent(nullptr);
	}

	if (InData->NextMeshComponent)
	{
		InData->NextMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		InData->NextMeshComponent->ConditionalBeginDestroy();
		InData->NextMeshComponent = nullptr;
	}

	InData->Character = nullptr;

	InData->CurrentMeshComponent = nullptr;
	InData->NextMeshComponent = nullptr;

	if (InData->SrcCurve)
	{
		InData->SrcCurve->RemoveFromRoot();
		InData->SrcCurve = nullptr;
	}
	if (InData->DestCurve)
	{
		InData->DestCurve->RemoveFromRoot();
		InData->DestCurve = nullptr;
	}
}

void FWREquipmentDirection::OnPostDirectionComplete(FWREquipmentDirectionData* InData)
{
	InData->DirectionDelegate.Unbind();
	InData->Table = nullptr;
}

void UWRActorComponentEquipmentLook::OnCreateComponent()
{
}

void UWRActorComponentEquipmentLook::OnDestroyComponent()
{
	for (auto& Part : Parts)
	{
#if UE_EDITOR
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::OnDestroyComponent] %s"), *WREnumUtility::EnumToString("EWREquipPart", Part.Key));
#else // UE_EDITOR
#endif // UE_EDITOR

		delete Part.Value;
		Part.Value = nullptr;
	}
	Parts.Empty();

	Director.StopPlay();

	for (auto& AsyncTask : AsyncTasks)
	{
#if UE_EDITOR
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::OnDestroyComponent] AsyncTasks : %d"), AsyncTask.Value->EquipmentPart);
#else // UE_EDITOR
#endif // UE_EDITOR

		delete AsyncTask.Value;
		AsyncTask.Value = nullptr;
	}
	AsyncTasks.Empty();
}

bool UWRActorComponentEquipmentLook::Equip(WRTableID InTID, const FName& InSocketName /*= NAME_None*/, FWREquipmentComplete InDelegate /*= nullptr*/, USkeletalMesh* In1st /*= nullptr*/, USkeletalMesh* In3rd /*= nullptr*/)
{
	return Equip(WRTableManager::Get()->FindRow<FWREquipmentLookTable>(InTID), InSocketName, InDelegate, In1st, In3rd);
}

bool UWRActorComponentEquipmentLook::Equip(struct FWREquipmentLookTable* InTable, const FName& InSocketName /*= NAME_None*/, FWREquipmentComplete InDelegate /*= nullptr*/, USkeletalMesh* In1st /*= nullptr*/, USkeletalMesh* In3rd /*= nullptr*/)
{
	if (InTable == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] Table is nullptr."));
		return false;
	}

	EWREquipPart EquipmentPart = InTable->EquipPart;

	FString Hash;
	Hash.Empty();

	bool IsDirected = true;

	auto CreateOrAddTask = [&](const FString& InHash) {
		if (InHash.IsEmpty())
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] Hash is empty."));
			return;
		}
		if (InHash == WRASYNCLOADMANAGER_INVALID)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] Hash is invalid."));
			return;
		}

		if (AsyncTasks.Contains(EquipmentPart) == false)
		{
			AsyncTasks.Add(EquipmentPart, new FWREquipmentAsyncTask());
		}
		AsyncTasks[EquipmentPart]->AddHash(InHash);
	};

#if UE_EDITOR
	auto DoesPackageExist = [&](const FString& InFullPath) {
		if (WRConsoleManager::Get()->GetInt(EWRConsole::CustomizeMessageBox) == 0)
		{
			return;
		}

		if (FPackageName::DoesPackageExist(InFullPath) == false)
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(InFullPath);

			FString Text = FString::Format(TEXT("Does Not Package Exist {0}"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRActorComponentEquipmentLook::Equip()"));
		}
	};
#else // UE_EDITOR
#endif // UE_EDITOR

	SCOPE_CYCLE_COUNTER(STAT_WRActorComponentEquipmentLook_Equip)

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (MyCharacter)
	{
		if (IsMainMeshPart(EquipmentPart))
		{
#if UE_EDITOR
			DoesPackageExist(InTable->FirstPersonSkeletalMesh);
#else // UE_EDITOR
#endif // UE_EDITOR

			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->FirstPersonSkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
			CreateOrAddTask(Hash);
		}

		IsDirected = MyCharacter->IsShowing3rdCharacter();
	}

	// [ 2020-9-4 : animal14 ] 
	WRTableID TID = GetDefaultPart(InTable);
	if (TID)
	{
		Equip(TID, NAME_None, InDelegate);
	}

	// [ 2020-6-25 : animal14 ] 장비
	if (InTable->StaticMesh.IsEmpty() == false)
	{
#if UE_EDITOR
		DoesPackageExist(InTable->StaticMesh);
#else // UE_EDITOR
#endif // UE_EDITOR

		Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->StaticMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
	}
	else if (InTable->SkeletalMesh.IsEmpty() == false)
	{
#if UE_EDITOR
		DoesPackageExist(InTable->SkeletalMesh);
#else // UE_EDITOR
#endif // UE_EDITOR

		Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->SkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
	}
	CreateOrAddTask(Hash);

	// [ 2020-9-4 : animal14 ] 
	UpdateDirection(EquipmentPart, IsDirected);
	// [ 2020-8-30 : animal14 ] 
	PreUpdateVisibility(EquipmentPart);

	// [ 2020-6-19 : animal14 ] 연출
	if (IsDirected)
	{
		if (InTable->ChangeCurveSRC.IsNull() == false)
		{
#if UE_EDITOR
			DoesPackageExist(InTable->ChangeCurveSRC.ToString());
#else // UE_EDITOR
#endif // UE_EDITOR

			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveSRC.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
			CreateOrAddTask(Hash);
		}
		if (InTable->ChangeCurveDEST.IsNull() == false)
		{
#if UE_EDITOR
			DoesPackageExist(InTable->ChangeCurveDEST.ToString());
#else // UE_EDITOR
#endif // UE_EDITOR

			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveDEST.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
			CreateOrAddTask(Hash);
		}
	}

	if (AsyncTasks.Contains(EquipmentPart) == false)
	{
		return false;
	}

	for (auto Object : AsyncTasks[EquipmentPart]->Objects)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] %s was reserved to %s."), *(Object.Key), *(GetOwnerCharacter()->GetName()));
	}

	AsyncTasks[EquipmentPart]->EquipmentPart = EquipmentPart;
	AsyncTasks[EquipmentPart]->SocketName = (InSocketName == NAME_None) ? InTable->AttachSocket : InSocketName;
	AsyncTasks[EquipmentPart]->FirstMergedMesh = In1st;
	AsyncTasks[EquipmentPart]->ThirdMergedMesh = In3rd;
	AsyncTasks[EquipmentPart]->Table = InTable;
	AsyncTasks[EquipmentPart]->CompleteDelegate = InDelegate;

	return true;
}

void UWRActorComponentEquipmentLook::Unequip(EWREquipPart InPart, FWREquipmentComplete InDelegate /*= nullptr*/)
{
	if (Parts.Contains(InPart) == false)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Unequip] Parts.Contains(%s) == false"), *WREnumUtility::EnumToString("EWREquipPart", InPart));
		return;
	}

	EWREquipPart EquipmentPart = InPart;

	bool IsDirected = true;

	SCOPE_CYCLE_COUNTER(STAT_WRActorComponentEquipmentLook_Equip)

	if (IsInventoryPart(EquipmentPart) == false)
	{
		return;
	}

	// [ 2020-10-8 : animal14 ] 디폴트가 필요한 파츠는 교체
	if (IsExchangePart(EquipmentPart) || IsBodyPart(EquipmentPart))
	{
		WRTableID TID = GetDefaultPart(EquipmentPart);
		if (TID)
		{
			Equip(TID, NAME_None, InDelegate);
		}
		return;
	}

	// [ 2020-10-8 : animal14 ] 디폴트가 필요하지 않은 파츠는 해제
	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		return;
	}

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Character);
	if (MyCharacter)
	{
		IsDirected = MyCharacter->IsShowing3rdCharacter();
	}

	if (AsyncTasks.Contains(EquipmentPart) == false)
	{
		AsyncTasks.Add(EquipmentPart, new FWREquipmentAsyncTask());
	}

	AsyncTasks[EquipmentPart]->EquipmentPart = EquipmentPart;
	AsyncTasks[EquipmentPart]->CompleteDelegate = InDelegate;

	auto RemoveTask = [&]() {
		Parts[EquipmentPart]->SetMesh();

		// [ 2020-7-3 : animal14 ] 
		if (AsyncTasks.Contains(EquipmentPart) == false)
		{
			return;
		}

		AsyncTasks[EquipmentPart]->ExecuteIfBound(Character);

		delete AsyncTasks[EquipmentPart];
		AsyncTasks[EquipmentPart] = nullptr;

		AsyncTasks.Remove(EquipmentPart);
	};

	if (IsDirected)
	{
		UMeshComponent* MeshComponent = Parts[EquipmentPart]->GetMesh();
		if (MeshComponent == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Unequip] MeshComponent == nullptr"));
			return;
		}

		bool IsSucceed = Director.BeginPlay(Character, MeshComponent, EquipmentPart, FWREquipmentDirectionComplete::CreateUObject(this, &UWRActorComponentEquipmentLook::OnUnequipComplete));
		if (IsSucceed == false)
		{
			RemoveTask();
		}
	}
	else
	{
		RemoveTask();
	}
}

void UWRActorComponentEquipmentLook::RestoreMaterialInterface()
{
	for (auto& Part : Parts)
	{
		Part.Value->RestoreMaterialInterface();
	}
}

void UWRActorComponentEquipmentLook::RestoreMaterialInterface(const EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return;
	}
	Parts[InPart]->RestoreMaterialInterface();
}

void UWRActorComponentEquipmentLook::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	for (auto& Part : Parts)
	{
		Part.Value->SetMaterial(ElementIndex, Material);
	}
}

void UWRActorComponentEquipmentLook::SetMaterial(int32 ElementIndex, UMaterialInterface* Material, const EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return;
	}
	Parts[InPart]->SetMaterial(ElementIndex, Material);
}

void UWRActorComponentEquipmentLook::UpdateEquipment(const class AWRCharacterMine* InCharacter)
{
	if (InCharacter == nullptr)
	{
		return;
	}

	auto UpdateAttachment = [&](EWREquipPart InPart, UMeshComponent* InParent)
	{
		if (IsMainMeshPart(InPart))
		{
			return;
		}
		if (Parts.Contains(InPart) == false)
		{
			return;
		}
		UMeshComponent* MeshComponent = Parts[InPart]->GetMesh();
		if (MeshComponent == nullptr)
		{
			return;
		}
		if (MeshComponent->GetAttachParent() == InParent)
		{
			return;
		}

		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		MeshComponent->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetIncludingScale, Parts[InPart]->GetAttachSocketName());
	};

	for (int32 i = (int32)EWREquipPart::RightHand; i < (int32)EWREquipPart::Max; ++i)
	{
		EWREquipPart EquipmentPart = (EWREquipPart)i;
		if (IsWeaponMeshPart(EquipmentPart) == false)
		{
			continue;
		}

		UpdateAttachment(EquipmentPart, InCharacter->GetCurrentSkeletalMeshComponent());
	}
}

bool UWRActorComponentEquipmentLook::IsVisible(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return false;
	}
	return Parts[InPart]->IsVisible();
}

void UWRActorComponentEquipmentLook::SetVisibility(EWREquipPart InPart, bool bNewVisibility)
{
	if (Parts.Contains(InPart) == false)
	{
		return;
	}
	Parts[InPart]->SetVisibility(bNewVisibility);
}

void UWRActorComponentEquipmentLook::SetCollisionEnable(EWREquipPart InPart, ECollisionEnabled::Type NewType)
{
	if (Parts.Contains(InPart) == false)
	{
		return;
	}
	Parts[InPart]->SetCollisionEnable(NewType);
}

void UWRActorComponentEquipmentLook::SetBodyVisibility()
{
	if (Parts.Contains(EWREquipPart::Body) == false)
	{
		return;
	}
	Parts[EWREquipPart::Body]->SetBodyVisibility();
}

void UWRActorComponentEquipmentLook::GetBodyVisibility()
{
	if (Parts.Contains(EWREquipPart::Body) == false)
	{
		return;
	}
	Parts[EWREquipPart::Body]->GetBodyVisibility();
}

UStaticMesh* UWRActorComponentEquipmentLook::GetStaticMesh(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return nullptr;
	}
	return Parts[InPart]->GetStaticMesh();
}

USkeletalMesh* UWRActorComponentEquipmentLook::GetSkeletalMesh(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return nullptr;
	}
	return Parts[InPart]->GetSkeletalMesh();
}

UMeshComponent* UWRActorComponentEquipmentLook::GetMesh(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return nullptr;
	}
	return Parts[InPart]->GetMesh();
}

bool UWRActorComponentEquipmentLook::IsEquipped(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return false;
	}

	if (Parts[InPart]->GetSkeletalMesh() == nullptr && Parts[InPart]->GetStaticMesh() == nullptr)
	{
		return false;
	}
	return true;
}

void UWRActorComponentEquipmentLook::OnAsyncLoadComplete(UObject* InObject, FString InHash)
{
	bool IsContains = false;
	bool IsCompleted = false;

	EWREquipPart EquipmentPart = EWREquipPart::None;

	for (auto& AsyncTask : AsyncTasks)
	{
		FWREquipmentAsyncTask* Task = AsyncTask.Value;
		if (Task->Contains(InHash) && Task->HasObjects(InHash) == false)
		{
			InObject->AddToRoot();
			Task->AddObject(InHash, InObject);
			
			IsContains = true;
			IsCompleted = WRAsyncLoadUObjectManager::IsLoadCompleteTasks(Task->Objects);

			EquipmentPart = Task->EquipmentPart;

			if (IsCompleted)
			{
				bool IsDirected = Initialize(Task);
				if (IsDirected == false)
				{
					PostUpdateVisibility(EquipmentPart);

					Task->ExecuteIfBound(GetOwnerCharacter());

					delete AsyncTasks[EquipmentPart];
					AsyncTasks[EquipmentPart] = nullptr;

					AsyncTasks.Remove(EquipmentPart);
				}
			}

			break;
		}
	}

	if (IsContains)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::OnAsyncLoadComplete] %s was completed to %s."), *InHash, *(GetOwnerCharacter()->GetName()));
	}
	else
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("It wasn't requested to load resource asynchronously."), TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}

	if (IsCompleted)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::OnAsyncLoadComplete] %s was loaded."), *WREnumUtility::EnumToString("EWREquipPart", EquipmentPart));
	}	
}

void UWRActorComponentEquipmentLook::OnDirectionComplete(EWREquipPart InPart, FName InSocketName, EWRAdditionalAbility InAbility, FString InLight, FString InStaticMesh, FString In1stSkeletalMesh, FString In3rdSkeletalMesh, USkeletalMesh* In1stMergedMesh, USkeletalMesh* In3rdMergedMesh)
{
	if (IsMergedMesh(In1stMergedMesh, In3rdMergedMesh))
	{
		Parts[InPart]->SettingEquipment(InSocketName, InPart, InAbility, InLight, InStaticMesh, In1stMergedMesh, In3rdMergedMesh);
	}
	else
	{
		Parts[InPart]->SettingEquipment(InSocketName, InPart, InAbility, InLight, InStaticMesh, In1stSkeletalMesh, In3rdSkeletalMesh);
	}

	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_DIRECT_MATERIAL_PARAMETERNAME, 1.0F, SetScalarParameterValue, Parts[InPart]->GetMesh());

	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		return;
	}

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Character);
	if (IsMainMeshPart(InPart) && MyCharacter)
	{
		UpdateEquipment(MyCharacter);
	}

	PostUpdateVisibility(InPart);

	if (AsyncTasks.Contains(InPart) == false)
	{
		return;
	}

	AsyncTasks[InPart]->ExecuteIfBound(Character);

	delete AsyncTasks[InPart];
	AsyncTasks[InPart] = nullptr;

	AsyncTasks.Remove(InPart);
}

void UWRActorComponentEquipmentLook::OnUnequipComplete(EWREquipPart InPart, FName InSocketName, EWRAdditionalAbility InAbility, FString InLight, FString InStaticMesh, FString In1stSkeletalMesh, FString In3rdSkeletalMesh, USkeletalMesh* In1stMergedMesh, USkeletalMesh* In3rdMergedMesh)
{
	if (Parts.Contains(InPart) == false)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::OnUnequipComplete] Parts.Contains(InPart) == false"));
		return;
	}

	Parts[InPart]->SetMesh();

	if (AsyncTasks.Contains(InPart) == false)
	{
		return;
	}

	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		return;
	}

	AsyncTasks[InPart]->ExecuteIfBound(Character);

	delete AsyncTasks[InPart];
	AsyncTasks[InPart] = nullptr;

	AsyncTasks.Remove(InPart);
}

bool UWRActorComponentEquipmentLook::Initialize(FWREquipmentAsyncTask* InTask)
{
	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		return false;
	}

	EWREquipPart EquipmentPart = InTask->EquipmentPart;
	FWREquipmentLookTable* Table = InTask->Table;

	bool IsDirected = true;

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Character);
	if (MyCharacter)
	{
		IsDirected = MyCharacter->IsShowing3rdCharacter();
	}

	// [ 2020-9-4 : animal14 ] 
	UpdateDirection(EquipmentPart, IsDirected);

	if (Parts.Contains(EquipmentPart) == false)
	{
		FWRMeshComponent* MeshComponent = nullptr;
		if (Table->StaticMesh.IsEmpty() == false)
		{
			MeshComponent = new FWRStaticMeshComponent(Character, Table);
		}
		else if (Table->SkeletalMesh.IsEmpty() == false)
		{
			MeshComponent = new FWRSkeletalMeshComponent(Character, Table);
		}

		if (MeshComponent == nullptr)
		{
			UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] It was failed to create WRMeshComponent."));
			return false;
		}

		Parts.Add(EquipmentPart, MeshComponent);
	}

	Parts[EquipmentPart]->Initialize(Table);
	
	auto UpdateMyCharacterEquipment = [&]() {
		if (MyCharacter)
		{
			UpdateEquipment(MyCharacter);
		}
	};

	if (IsDirected)
	{
		UMeshComponent* CurrentMeshComponent = Parts[EquipmentPart]->GetMesh();
		if (CurrentMeshComponent == nullptr)
		{
			return false;
		}

		if (IsMainMeshPart(EquipmentPart) && MyCharacter)
		{
			CurrentMeshComponent = Character->GetCurrentSkeletalMeshComponent();
		}

		bool IsSucceed = Director.BeginPlay(Character, CurrentMeshComponent, Table, InTask->SocketName, FWREquipmentDirectionComplete::CreateUObject(this, &UWRActorComponentEquipmentLook::OnDirectionComplete), InTask->FirstMergedMesh, InTask->ThirdMergedMesh);
		if (IsSucceed == false)
		{
			FName SocketName = InTask->SocketName;
			EWRAdditionalAbility Ability = Table->AdditionalAbility;
			FString Light = Table->LightValue.ToString();
			FString StaticMesh = Table->StaticMesh;
			FString FirstSkeletalMesh = Table->FirstPersonSkeletalMesh;
			FString ThirdSkeletalMesh = Table->SkeletalMesh;
			USkeletalMesh* FirstMergedMesh = InTask->FirstMergedMesh;
			USkeletalMesh* ThirdMergedMesh = InTask->ThirdMergedMesh;

			if (IsMergedMesh(FirstMergedMesh, ThirdMergedMesh))
			{
				Parts[EquipmentPart]->SettingEquipment(SocketName, EquipmentPart, Ability, Light, StaticMesh, FirstMergedMesh, ThirdMergedMesh);
			}
			else
			{
				Parts[EquipmentPart]->SettingEquipment(SocketName, EquipmentPart, Ability, Light, StaticMesh, FirstSkeletalMesh, ThirdSkeletalMesh);
			}

			UpdateMyCharacterEquipment();
			return false;
		}
	}
	else
	{
		FName SocketName = InTask->SocketName;
		EWRAdditionalAbility Ability = Table->AdditionalAbility;
		FString Light = Table->LightValue.ToString();
		FString StaticMesh = Table->StaticMesh;
		FString FirstSkeletalMesh = Table->FirstPersonSkeletalMesh;
		FString ThirdSkeletalMesh = Table->SkeletalMesh;
		USkeletalMesh* FirstMergedMesh = InTask->FirstMergedMesh;
		USkeletalMesh* ThirdMergedMesh = InTask->ThirdMergedMesh;

		if (IsMergedMesh(FirstMergedMesh, ThirdMergedMesh))
		{
			Parts[EquipmentPart]->SettingEquipment(SocketName, EquipmentPart, Ability, Light, StaticMesh, FirstMergedMesh, ThirdMergedMesh);
		}
		else
		{
			Parts[EquipmentPart]->SettingEquipment(SocketName, EquipmentPart, Ability, Light, StaticMesh, FirstSkeletalMesh, ThirdSkeletalMesh);
		}

		UpdateMyCharacterEquipment();
	}

	return IsDirected;
}

WRTableID UWRActorComponentEquipmentLook::GetDefaultPart(struct FWREquipmentLookTable* InTable)
{
	if (InTable == nullptr)
	{
		return 0;
	}

	EWREquipPart EquipmentPart = InTable->EquipPart;
	if (EquipmentPart == EWREquipPart::Top)
	{
		if (IsEquipped(EWREquipPart::Bottom) == false)
		{
			// [ 2020-9-6 : animal14 ] 티폴트 하의
			return InTable->BottomTID;
		}
	}
	if (EquipmentPart == EWREquipPart::Bottom)
	{
		if (IsEquipped(EWREquipPart::Top) == false)
		{
			// [ 2020-9-4 : animal14 ] 디폴트 상의
			return InTable->TopTID;
		}
	}

	return 0;
}

WRTableID UWRActorComponentEquipmentLook::GetDefaultPart(EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return 0;
	}

	WRTableID TID = Parts[InPart]->GetTID();
	if (TID == 0)
	{
		return 0;
	}

	FWREquipmentLookTable* Table = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(TID);
	if (Table == nullptr)
	{
		return 0;
	}

	return Table->BaseTID;
}

void UWRActorComponentEquipmentLook::UpdateDirection(EWREquipPart InPart, bool& OutIsDirected)
{
	// [ 2020-8-24 : animal14 ] 신체와 무기 파츠는 장착 연출 없음
	if (IsInventoryPart(InPart) == false || IsWeaponMeshPart(InPart))
	{
		OutIsDirected = false;
	}

	if (Parts.Contains(InPart) == false)
	{
		// [ 2020-8-24 : animal14 ] 일부 파츠는 초기 장착 연출 없음
		if (IsExchangePart(InPart) || IsBodyPart(InPart))
		{
			OutIsDirected = false;
		}
	}
	else
	{
		// [ 2020-8-30 : animal14 ] 커스터마이징 연출
		if (IsFirstPersonPart(InPart) == false)
		{	
			OutIsDirected = true;
		}
	}

	// [ 2020-9-28 : animal14 ] 특별 대화중에는 연출하지 않음
	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::UpdateDirection] Character == nullptr"));
		return;
	}

	if (Character->IsVisibilityBits(EWRVisibility::SpecialConversation))
	{
		OutIsDirected = false;
	}
}

void UWRActorComponentEquipmentLook::PreUpdateVisibility(EWREquipPart InPart)
{
	switch (InPart)
	{
	case EWREquipPart::Top:
		SetVisibility(EWREquipPart::Body, false);
		break;
	case EWREquipPart::Bottom:
		SetVisibility(EWREquipPart::Body, false);
		break;
	case EWREquipPart::Outfit:
		SetVisibility(EWREquipPart::Body, false);
		break;
	case EWREquipPart::Body:
		SetVisibility(EWREquipPart::Body, true);
		break;
	}

	// [ 2020-9-28 : animal14 ] 특별 대화중에는 보이지 않음
	AWRCharacter* Character = GetOwnerCharacter();
	if (Character == nullptr)
	{
		UE_LOG(WRActorComponentEquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::PreUpdateVisibility] Character == nullptr"));
		return;
	}

	if (Character->IsVisibilityBits(EWRVisibility::SpecialConversation))
	{
		SetVisibility(InPart, false);
	}
}

void UWRActorComponentEquipmentLook::PostUpdateVisibility(EWREquipPart InPart)
{
	switch (InPart)
	{
	case EWREquipPart::Top:
		SetVisibility(EWREquipPart::Body, IsEquipped(EWREquipPart::Bottom) ? false : true);
		break;
	case EWREquipPart::Bottom:
		SetVisibility(EWREquipPart::Body, IsEquipped(EWREquipPart::Top) ? false : true);
		break;
	case EWREquipPart::Outfit:
		SetVisibility(EWREquipPart::Body, false);
		break;
	case EWREquipPart::Body:
		SetVisibility(EWREquipPart::Body, IsEquipped(EWREquipPart::Outfit) ? false : true);
		break;
	}
}

//DEFINE_LOG_CATEGORY(EquipmentLook)
//
//#define WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME "00.ClothChangingMain"
//
//#if WITH_EDITOR
//TMap<EWREquipPart, uint32> FWREquippedMeshComponentWrapping::MeshComponentUniqueID;
//#endif // WITH_EDITOR
//
//
//#define EQUIPMENTLOOK_SETMATERIAL_PARAMETER(ParamName, Value, Function, MeshComponent)			\
//{																								\
//	TArray<UMaterialInstanceDynamic*> MIDs;														\
//	WRActorUtility::GetMeshMaterialInstanceDynamic(MeshComponent, MIDs);						\
//	for (int32 i = 0; i < MIDs.Num(); ++i)														\
//	{																							\
//		UMaterialInstanceDynamic* MID = MIDs[i];												\
//		if (MID != nullptr)																		\
//		{																						\
//			MID->Function(ParamName, Value);													\
//		}																						\
//	}																							\
//}																								\
//
//void FWREquippedMeshComponentWrapping::SetVisibility(const bool bInVisible)
//{
//	UMeshComponent* MeshComponent = this->GetMeshComponent();
//	if (MeshComponent->IsValidLowLevel() == false)
//		return;
//
//	MeshComponent->SetVisibility(bInVisible);
//}
//
//bool FWREquippedMeshComponentWrapping::IsVisible()
//{
//	UMeshComponent* MeshComponent = this->GetMeshComponent();
//	if (MeshComponent->IsValidLowLevel() == false)
//		return false;
//
//	return MeshComponent->IsVisible();
//}
//
//void FWREquippedMeshComponentWrapping::SetMaterialParameterValue(FName InParamName, float InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
//{
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParamName, InValue, SetScalarParameterValue, InMeshComponent == nullptr ? this->GetMeshComponent() : InMeshComponent);
//}
//
//void FWREquippedMeshComponentWrapping::SetMaterialParameterValue(FName InParamName, FLinearColor InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
//{
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParamName, InValue, SetVectorParameterValue, InMeshComponent == nullptr ? this->GetMeshComponent() : InMeshComponent);
//}
//
//bool FWREquippedMeshComponentWrapping::SetCollisionEnable(ECollisionEnabled::Type InCollisionEnableType)
//{
//	UMeshComponent* MeshComponent = GetMeshComponent();
//	if (MeshComponent == nullptr)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("FWREquippedMeshComponentWrapping::SetCollisionEnable - Null MeshComponent"));
//		return false;
//	}
//
//	MeshComponent->SetCollisionEnabled(InCollisionEnableType);
//	return true;
//}
//
//void FWREquippedMeshComponentWrapping::SettingForEquip(FWREquipmentLookTable* InTable)
//{
//	UMeshComponent* MeshComponent = GetMeshComponent();
//	checkf(MeshComponent != nullptr, TEXT("FWREquippedMeshComponentWrapping::SettingForEquip - Null MeshComponent."));
//
//	// [ 2020-1-6 : magedoga ] Body Part가 아닐때 처리
//	if (this->IsBodyPart(InTable->EquipPart) == false)
//	{
//		if (MeshComponent->IsRegistered() == false)
//			MeshComponent->RegisterComponentWithWorld(Owner->GetWorld());
//
//		MeshComponent->CastShadow = true;
//		MeshComponent->bCastDynamicShadow = true;
//
//		Owner->AddInstanceComponent(MeshComponent);
//
//#if WITH_EDITOR
//		MeshComponent->Rename(*this->MakeComponentNameForDetailPanel(InTable->EquipPart));
//#endif // WITH_EDITOR
//	}
//
//	MeshComponent->bReceivesDecals = false;
//
//	//if (EquipmentLookTable != nullptr)
//	if (InTable != nullptr)
//	{
//		AttachSocketName = InTable->AttachSocket;
//	}
//
//	if (this->IsBodyPart(InTable->EquipPart) == false)
//	{
//		if (MeshComponent->IsAttachedTo(Owner->GetMesh()) == true)
//		{
//			MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//		}
//
//		MeshComponent->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);
//	}
//
//	// [ 2019-9-17 : magedoga ] by Child
//	this->Setting_Internal(InTable);
//
//	// [ 2019-9-25 : magedoga ] Setting for weapon
//	if (this->IsWeaponPart(InTable->EquipPart) == true)
//	{
//		// [ 2019-10-1 : magedoga ] Collision for overlapevent
//		MeshComponent->SetCollisionProfileName(FName("WRAttackObject"));
//		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		MeshComponent->SetGenerateOverlapEvents(true);
//		if (MeshComponent->OnComponentBeginOverlap.Contains(Owner, FName("OnBeginOverlapCapsuleAttacking")) == false)
//		{
//			MeshComponent->OnComponentBeginOverlap.AddDynamic(Owner, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
//		}
//
//		this->ReleaseAdditionalAbility();
//		this->CreateAdditionalAbility(InTable->AdditionalAbility, InTable);
//	}
//}
//
//void FWREquippedMeshComponentWrapping::SettingForEquip(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh)
//{
//	UMeshComponent* MeshComponent = GetMeshComponent();
//	checkf(MeshComponent != nullptr, TEXT("[FWREquippedMeshComponentWrapping::animal20] MeshComponent is nullptr."));
//	checkf(InTable != nullptr, TEXT("[FWREquippedMeshComponentWrapping::animal20] Table is nullptr."));
//
//	AttachSocketName = InTable->AttachSocket;
//
//	EWREquipPart EquipmentPart = InTable->EquipPart;
//	if (IsBodyPart(EquipmentPart) == false)
//	{
//		if (MeshComponent->IsRegistered() == false)
//		{
//			MeshComponent->RegisterComponentWithWorld(Owner->GetWorld());
//		}
//
//		MeshComponent->CastShadow = true;
//		MeshComponent->bCastDynamicShadow = true;
//
//		Owner->AddInstanceComponent(MeshComponent);
//
//#if WITH_EDITOR
//		FString Name = MakeComponentNameForDetailPanel(EquipmentPart);
//
//		MeshComponent->Rename(*Name);
//#else // WITH_EDITOR
//#endif // WITH_EDITOR
//
//		UMeshComponent* MainMeshComponent = Owner->GetMesh();
//
//		if (MeshComponent->IsAttachedTo(MainMeshComponent))
//		{
//			MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//		}
//
//		MeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);
//	}
//	MeshComponent->bReceivesDecals = false;
//
//	// [ 2020-6-22 : animal14 ] 
//	Setting_Internal(InTable, InMergedSkeletalMesh);
//
//	if (IsWeaponPart(EquipmentPart))
//	{
//		MeshComponent->SetCollisionProfileName(FName("WRAttackObject"));
//		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		MeshComponent->SetGenerateOverlapEvents(true);
//
//		if (MeshComponent->OnComponentBeginOverlap.Contains(Owner, FName("OnBeginOverlapCapsuleAttacking")) == false)
//		{
//			MeshComponent->OnComponentBeginOverlap.AddDynamic(Owner, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
//		}
//
//		ReleaseAdditionalAbility();
//		CreateAdditionalAbility(InTable->AdditionalAbility, InTable);
//	}
//}
//
//FTransform FWREquippedMeshComponentWrapping::GetAttachSocketTransform(ERelativeTransformSpace InSpace) const
//{
//	if (Owner->IsValidLowLevel() == false || Owner->GetMesh()->IsValidLowLevel() == false)
//		return FTransform::Identity;
//
//	return Owner->GetMesh()->GetSocketTransform(AttachSocketName, InSpace);
//}
//
//void FWREquippedMeshComponentWrapping::DestroyComponent()
//{
//	this->ReleaseAdditionalAbility();
//
//	UMeshComponent* MeshComponent = GetMeshComponent();
//	if (MeshComponent != nullptr)
//	{
//		if (Owner->IsValidLowLevel() == true)
//		{
//			Owner->RemoveInstanceComponent(MeshComponent);
//		}
//
//		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//		MeshComponent->UnregisterComponent();
//		MeshComponent->ConditionalBeginDestroy();
//	}
//}
//
////====================================================================================
//
//void FWREquippedMeshComponentWrapping::CreateAdditionalAbility(EWRAdditionalAbility InAdditionalAbility, FWREquipmentLookTable* InTable)
//{
//	if (AdditionalAbility != EWRAdditionalAbility::None)
//		return;
//
//	AdditionalAbility = InAdditionalAbility;
//
//	UMeshComponent* MeshComponent = GetMeshComponent();
//	switch (InAdditionalAbility)
//	{
//	case EWRAdditionalAbility::Hook:
//	{
//		// [ 2019-10-1 : magedoga ] Create ConnectbyCable Component
//		UWRActorComponentConnectbyCable* ConnectCableComponent = Owner->GetComponent<UWRActorComponentConnectbyCable>();
//		if (ConnectCableComponent == nullptr)
//		{
//			Owner->CreateComponent<UWRActorComponentConnectbyCable>(EWRCharacterComponent::ConnectbyCable);
//		}
//
//	} break;
//	case EWRAdditionalAbility::Light:
//	{
//		if (MeshComponent->DoesSocketExist("Light") == true && InTable->LightValue.IsNull() == false)
//		{
//			UWRLightValueAsset* LightAsset = InTable->LightValue.LoadSynchronous();
//
//			auto SetCommonLightValue = [&](UWRLightValueAsset* InLightAsset, UPointLightComponent* InPointLightComponent)
//			{
//				InPointLightComponent->SetMobility(EComponentMobility::Movable);
//				InPointLightComponent->bUseInverseSquaredFalloff = InLightAsset->bUseInverseSquaredFalloff;
//				InPointLightComponent->SetLightFalloffExponent(InLightAsset->LightFalloffExponent);
//				InPointLightComponent->SetAttenuationRadius(InLightAsset->AttenuationRadius);
//				InPointLightComponent->SetSourceRadius(InLightAsset->SourceRadius);
//				InPointLightComponent->bUseTemperature = InLightAsset->bUseTemperature;
//				InPointLightComponent->SetIntensity(InLightAsset->Intensity);
//				InPointLightComponent->SetLightColor(InLightAsset->Color);
//				InPointLightComponent->SetLightFunctionMaterial(InLightAsset->LightFunctionMaterial.LoadSynchronous());
//			};
//
//			auto SetSpotLightValue = [&](UWRLightValueAsset* InLightAsset, USpotLightComponent* InSpotLightComponent)
//			{
//				InSpotLightComponent->SetInnerConeAngle(InLightAsset->InnerConeAngle);
//				InSpotLightComponent->SetOuterConeAngle(InLightAsset->OuterConeAngle);
//				InSpotLightComponent->LightShaftConeAngle = InLightAsset->LightShaftConeAngle;
//			};
//
//			AActor* LightActor = nullptr;
//			UPointLightComponent* PointLightComponent = nullptr;
//			// [ 2020-1-30 : magedoga ] Create Light Actor
//			if (LightAsset->bSpotLight == true)
//			{
//				ASpotLight* SpotLight = MeshComponent->GetWorld()->SpawnActor<ASpotLight>();
//				SetSpotLightValue(LightAsset, SpotLight->SpotLightComponent);
//
//				PointLightComponent = Cast<UPointLightComponent>(SpotLight->SpotLightComponent);
//				LightActor = SpotLight;
//			}
//			else
//			{
//				APointLight* PointLight = MeshComponent->GetWorld()->SpawnActor<APointLight>();
//
//				PointLightComponent = PointLight->PointLightComponent;
//				LightActor = PointLight;
//			}
//
//			SetCommonLightValue(LightAsset, PointLightComponent);
//
//
//			LightActor->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Light");
//
//			if (PointLightComponent->bUseInverseSquaredFalloff == false)
//			{
//				PointLightComponent->IntensityUnits = ELightUnits::Unitless;
//			}
//			else
//			{
//				PointLightComponent->IntensityUnits = ELightUnits::Candelas;
//			}
//
//			if (PointLightComponent->bUseTemperature == true)
//			{
//				PointLightComponent->SetTemperature(LightAsset->Temperature);
//			}
//
//#if WITH_EDITOR
//			Owner->AddInstanceComponent(PointLightComponent);
//			PointLightComponent->bEditableWhenInherited = true;
//#endif // WITH_EDITOR
//		}
//
//	} break;
//	}
//}
//
//void FWREquippedMeshComponentWrapping::ReleaseAdditionalAbility()
//{
//	UMeshComponent* MeshComponent = GetMeshComponent();
//
//	switch (AdditionalAbility)
//	{
//	case EWRAdditionalAbility::Hook:
//	{
//		// [ 2019-10-7 : magedoga ] 주석 : 끊김 연출때문.
//		//Owner->RemoveComponent(EWRCharacterComponent::ConnectbyCable);
//
//	} break;
//	case EWRAdditionalAbility::Light:
//	{
//		// [ 2019-10-1 : magedoga ] Temp
//		TArray<USceneComponent*> AttachedChildren = MeshComponent->GetAttachChildren();
//		for (int32 i = 0; i < AttachedChildren.Num(); ++i)
//		{
//			APointLight* PL = Cast<APointLight>(AttachedChildren[i]->GetOwner());
//			if (PL->IsValidLowLevel() == false)
//				continue;
//
//#if WITH_EDITOR
//			Owner->RemoveInstanceComponent(PL->PointLightComponent);
//#endif // WITH_EDITOR
//
//			PL->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//			PL->SetActorHiddenInGame(true);
//			PL->ConditionalBeginDestroy();
//		}
//
//	} break;
//	}
//
//	AdditionalAbility = EWRAdditionalAbility::None;
//}
//
////====================================================================================
//
//#if WITH_EDITOR
//FString FWREquippedMeshComponentWrapping::MakeComponentNameForDetailPanel(EWREquipPart InPart)
//{
//	if (MeshComponentUniqueID.Contains(InPart) == false)
//	{
//		MeshComponentUniqueID.Add(InPart, 0);
//	}
//
//	uint32 ID = MeshComponentUniqueID[InPart];	MeshComponentUniqueID[InPart] = ++ID;
//
//	FString EnumName = FString::Format(TEXT("{0}{1}"), { WREnumUtility::EnumToString<EWREquipPart>("EWREquipPart", InPart), ID });
//	return EnumName;
//}
//#endif // WITH_EDITOR
//
////====================================================================================
//
//void FWREquippedMeshComponentWrapping_StaticMesh::SetMesh(class UStaticMesh* InMesh)
//{
//	checkf(StaticMeshComponent != nullptr, TEXT("FWREquippedComponent_StaticMesh - Null MeshComponent"));
//
//	FWREquippedMeshComponentWrapping::SetMesh(InMesh);
//	StaticMeshComponent->SetStaticMesh(InMesh);
//}
//
//void FWREquippedMeshComponentWrapping_StaticMesh::SetNullMesh()
//{
//	checkf(StaticMeshComponent != nullptr, TEXT("FWREquippedComponent_StaticMesh - Null MeshComponent"));
//
//	FWREquippedMeshComponentWrapping::SetNullMesh();
//	StaticMeshComponent->SetStaticMesh(nullptr);
//}
//
////====================================================================================
//
//bool FWREquippedMeshComponentWrapping_StaticMesh::AttachActor(AActor* InActor, FName InSocketName)
//{
//	return true;
//}
//
//bool FWREquippedMeshComponentWrapping_StaticMesh::DetachActor(AActor* InActor)
//{
//	return true;
//}
//
////====================================================================================
//
//void FWREquippedMeshComponentWrapping_StaticMesh::Setting_Internal(FWREquipmentLookTable* InTable)
//{
//	if (InTable != nullptr)
//	{
//		//UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(UWRGameInstance::GetGameInstance()->GetWorld(), *InTable->StaticMesh);
//		UStaticMesh* StaticMesh = FindObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
//		if (StaticMesh == nullptr)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("FWREquippedMeshComponentWrapping_StaticMesh::Setting_Internal - Not found Object [%s]"), *InTable->StaticMesh);
//			StaticMesh = LoadObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
//		}
//
//		this->SetMesh(StaticMesh);
//	}
//}
//
//void FWREquippedMeshComponentWrapping_StaticMesh::Setting_Internal(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh)
//{
//	if (InTable == nullptr)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("[FWREquippedMeshComponentWrapping_StaticMesh::Setting_Internal] InTable is nullptr."));
//		return;
//	}
//
//	UStaticMesh* StaticMesh = FindObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
//	if (StaticMesh == nullptr)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("[FWREquippedMeshComponentWrapping_StaticMesh::Setting_Internal] It was failed to find %s."), *InTable->StaticMesh);
//		StaticMesh = LoadObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
//	}
//
//	SetMesh(StaticMesh);
//}
//
////====================================================================================
//
//void FWREquippedMeshComponentWrapping_SkeletalMesh::SetMesh(class USkeletalMesh* InMesh)
//{
//	checkf(SkeletalMeshComponent != nullptr, TEXT("FWREquippedComponent_SkeletalMesh - Null MeshComponent"));
//
//	FWREquippedMeshComponentWrapping::SetMesh(InMesh);
//	SkeletalMeshComponent->SetSkeletalMesh(InMesh, false);
//}
//
//void FWREquippedMeshComponentWrapping_SkeletalMesh::SetNullMesh()
//{
//	checkf(SkeletalMeshComponent != nullptr, TEXT("FWREquippedComponent_SkeletalMesh - Null MeshComponent"));
//
//	FWREquippedMeshComponentWrapping::SetNullMesh();
//	SkeletalMeshComponent->SetSkeletalMesh(nullptr);
//}
//
////====================================================================================
//
//bool FWREquippedMeshComponentWrapping_SkeletalMesh::AttachActor(AActor* InActor, FName InSocketName)
//{
//	return true;
//}
//
//bool FWREquippedMeshComponentWrapping_SkeletalMesh::DetachActor(AActor* InActor)
//{
//	return true;
//}
//
////====================================================================================
//
//void FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal(FWREquipmentLookTable* InTable)
//{
//	checkf(InTable != nullptr, TEXT("FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal - Null InTable"));
//
//	auto FindOrLoadObject = [&](const FString& InPath) {
//
//		USkeletalMesh* SkeletalMesh = FindObject<USkeletalMesh>(nullptr, *InPath);
//		if (SkeletalMesh == nullptr)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal - Not found object [%s]"), *InPath);
//			SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InPath);
//		}
//
//		return SkeletalMesh;
//	};
//
//	//USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InTable->SkeletalMesh);
//	USkeletalMesh* SkeletalMesh = FindOrLoadObject(InTable->SkeletalMesh);
//
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	this->SetMesh(SkeletalMesh);
//
//	if (WRCharacterManager::Get()->IsMyCharacter(Owner))
//	{
//		if (EquippedPart == EWREquipPart::Face || EquippedPart == EWREquipPart::Hair || EquippedPart == EWREquipPart::FacialHair)
//		{
//			SetVisibility(false);
//		}
//		else
//		{
//		}
//	}
//	else
//	{
//	}
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	if (EquippedPart != EWREquipPart::Body)
//	{
//		this->SetMesh(SkeletalMesh);
//	}
//	else
//	{
//		if (WRCharacterManager::Get()->IsMyCharacter(Owner) == false)
//		{
//			this->SetMesh(SkeletalMesh);
//		}
//		else
//		{
//			//LoadObject<USkeletalMesh>(nullptr, *InTable->FirstPersonSkeletalMesh);
//			USkeletalMesh* FirstPersonSkeletalMesh = FindOrLoadObject(InTable->FirstPersonSkeletalMesh);
//			this->SetMesh(FirstPersonSkeletalMesh);
//
//			// [ 2020-1-8 : magedoga ] 3rd
//			AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Owner);
//			MyCharacter->Set3rdSkeletalMesh(SkeletalMesh);
//		}
//	}
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//
//	// [ 2020-6-10 : animal14 ] 임시 코드
//	if (SkeletalMeshComponent == nullptr)
//	{
//		return;
//	}
//	if (SkeletalMeshComponent == Owner->GetMesh())
//	{
//		return;
//	}
//	SkeletalMeshComponent->SetMasterPoseComponent(Owner->GetMesh());
//
//	//USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetMeshComponent());
//	//SkeletalMeshComponent->bEnableUpdateRateOptimizations = true;
//}
//
//void FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh)
//{
//	if (InMergedSkeletalMesh == nullptr)
//	{
//		if (InTable == nullptr)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("[FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal] InTable is nullptr."));
//			return;
//		}
//
//		auto FindOrLoadObject = [&](const FString& InPath) {
//
//			USkeletalMesh* SkeletalMesh = FindObject<USkeletalMesh>(nullptr, *InPath);
//			if (SkeletalMesh == nullptr)
//			{
//				UE_LOG(EquipmentLook, Display, TEXT("[FWREquippedMeshComponentWrapping_SkeletalMesh::Setting_Internal] It was failed to find %s."), *InTable->SkeletalMesh);
//				SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InPath);
//			}
//
//			return SkeletalMesh;
//		};
//
//		USkeletalMesh* SkeletalMesh = FindOrLoadObject(InTable->SkeletalMesh);
//
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		SetMesh(SkeletalMesh);
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		if (EquippedPart == EWREquipPart::Body)
//		{
//			if (WRCharacterManager::Get()->IsMyCharacter(Owner))
//			{
//				USkeletalMesh* FirstPersonSkeletalMesh = FindOrLoadObject(InTable->FirstPersonSkeletalMesh);
//				SetMesh(FirstPersonSkeletalMesh);
//
//				AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Owner);
//				MyCharacter->Set3rdSkeletalMesh(SkeletalMesh);
//			}
//			else
//			{
//				SetMesh(SkeletalMesh);
//			}
//		}
//		else
//		{
//			SetMesh(SkeletalMesh);
//		}
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	}
//	else
//	{
//		SetMesh(InMergedSkeletalMesh);
//	}
//
//	// [ 2020-6-22 : animal14 ] 
//	if (SkeletalMeshComponent == nullptr)
//	{
//		return;
//	}
//
//	USkeletalMeshComponent* MainMeshComponent = Owner->GetMesh();
//	if (SkeletalMeshComponent == MainMeshComponent)
//	{
//		return;
//	}
//
//	SkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);
//}
//
////====================================================================================
//
//
//
////====================================================================================
//
//void UWRActorComponentEquipmentLook::OnDestroyComponent()
//{
//	for (auto It = EquippedPart.CreateIterator(); It; ++It)
//	{
//		delete It.Value();
//	}
//
//	EquippedPart.Empty();
//
//	ChangeEquipLookDirector.StopDirect();
//
//	for (auto It = AsyncTasks.CreateIterator(); It; ++It)
//	{
//		FWREquipAsyncTask* Value = It.Value();
//		for (auto It_Value = Value->LoadedObjects.CreateIterator(); It_Value; ++It_Value)
//		{
//			UObject* Object = It_Value.Value();
//			if (Object->IsValidLowLevel() == false)
//			{
//				WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(It_Value.Key());
//			}
//			else
//			{
//				Object->RemoveFromRoot();
//			}
//		}
//
//		delete Value;
//	}
//
//	AsyncTasks.Empty();
//}
//
//#ifdef ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//bool UWRActorComponentEquipmentLook::Equip(WRTableID InTableID, USkeletalMesh* InMergedSkeletalMesh /*= nullptr*/, FWREquipCompleteDelegate InDelegate /*= nullptr*/)
//{
//	return Equip(WRTableManager::Get()->FindRow<FWREquipmentLookTable>(InTableID), InMergedSkeletalMesh, InDelegate);
//}
//
//bool UWRActorComponentEquipmentLook::Equip(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh /*= nullptr*/, FWREquipCompleteDelegate InDelegate /*= nullptr*/)
//{
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//	if (ChangeEquipLookDirector.IsDirecting())
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] "));
//		return false;
//	}
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//	if (InTable == nullptr)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] Table is nullptr."));
//		return false;
//	}
//
//	EWREquipPart EquipmentPart = InTable->EquipPart;
//
//	FString Hash;
//	Hash.Empty();
//
//	bool IsDirecting = true;
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//	auto CreateOrAddTask = [&](const FString& InHash) {
//		if (InHash == "Invalid")
//		{
//			return;
//		}
//
//		if (AsyncTasks.Contains(EquipmentPart) == false)
//		{
//			AsyncTasks.Add(EquipmentPart, new FWREquipAsyncTask());
//		}
//		AsyncTasks[EquipmentPart]->AddHash(InHash);
//	};
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	AWRCharacterMine* OwnerCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
//	if (OwnerCharacter)
//	{
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		IsDirecting = OwnerCharacter->IsShowingAppearance();
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		if (EquipmentPart == EWREquipPart::Body)
//		{
//			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->FirstPersonSkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			CreateOrAddTask(Hash);
//		}
//
//		IsDirecting = OwnerCharacter->IsShowing3rdCharacter();
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	}
//
//	if (InMergedSkeletalMesh == nullptr)
//	{
//		if (InTable->SkeletalMesh.IsEmpty() == false)
//		{
//			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->SkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//		}
//		else if (InTable->StaticMesh.IsEmpty() == false)
//		{
//			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->StaticMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//		}
//		CreateOrAddTask(Hash);
//	}
//	else
//	{
//		// [ 2020-6-19 : animal14 ] 비동기를 사용하지 않으므로 여기에서 모두 처리
//		if (EquippedPart.Contains(EquipmentPart) == false)
//		{
//			FWREquippedMeshComponentWrapping* EquippedComponent = nullptr;
//
//			if (InTable->StaticMesh.IsEmpty() == false)
//			{
//				EquippedComponent = new FWREquippedMeshComponentWrapping_StaticMesh(InTable, GetOwnerCharacter());
//			}
//			else if (InTable->SkeletalMesh.IsEmpty() == false)
//			{
//				EquippedComponent = new FWREquippedMeshComponentWrapping_SkeletalMesh(InTable, GetOwnerCharacter());
//			}
//
//			if (EquippedComponent == nullptr)
//			{
//				UE_LOG(EquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] Please check table data."));
//				return false;
//			}
//
//			EquippedPart.Add(EquipmentPart, EquippedComponent);
//		}
//
//		// [ 2020-6-19 : animal14 ] 연출
//		auto EquipmentSetting = [&]()
//		{
//			EquippedPart[EquipmentPart]->SettingForEquip(InTable, InMergedSkeletalMesh);
//
//			if (WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == false)
//			{
//				return;
//			}
//
//			Update_AttachItemToSuitableMesh(OwnerCharacter);
//		};
//
//		if (IsDirecting)
//		{
//			UMeshComponent* CurrentPartMeshComponent = EquippedPart[EquipmentPart]->GetMeshComponent();
//			if (EquipmentPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter))
//			{
//				CurrentPartMeshComponent = OwnerCharacter->GetCurrentSkeletalMeshComponent();
//			}
//
//			bool bSuccess = ChangeEquipLookDirector.StartDirect(OwnerCharacter, CurrentPartMeshComponent, InTable, FWROnCompleteEquipLookChangeDirect::CreateUObject(this, &UWRActorComponentEquipmentLook::OnEndChangeEquipLookDirect));
//			if (bSuccess == false)
//			{
//				EquipmentSetting();
//			}
//		}
//		else
//		{
//			EquipmentSetting();
//
//			// [ 2020-6-19 : animal14 ] 이벤트
//			InDelegate.ExecuteIfBound(OwnerCharacter, EquipmentPart);
//		}
//	}
//
//	// [ 2020-6-19 : animal14 ] 연출
//	if (IsDirecting)
//	{
//		if (InTable->ChangeCurveSRC.IsNull() == false)
//		{
//			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveSRC.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			CreateOrAddTask(Hash);
//		}
//		if (InTable->ChangeCurveDEST.IsNull() == false)
//		{
//			Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveDEST.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			CreateOrAddTask(Hash);
//		}
//	}
//
//#if UE_BUILD_SHIPPING
//#else // UE_BUILD_SHIPPING
//	for (auto Iter = AsyncTasks.CreateIterator(); Iter; ++Iter)
//	{
//		FWREquipAsyncTask* AsyncTask = Iter.Value();
//		for (auto InnerIter = AsyncTask->LoadedObjects.CreateIterator(); InnerIter; ++InnerIter)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("[UWRActorComponentEquipmentLook::Equip] %s"), *(InnerIter.Key()));
//		}
//	}
//#endif // UE_BUILD_SHIPPING
//
//	if (AsyncTasks.Contains(EquipmentPart))
//	{
//		AsyncTasks[EquipmentPart]->RequestedTable = InTable;
//		AsyncTasks[EquipmentPart]->EquipCompleteDelegate = InDelegate;
//	}
//	return AsyncTasks.Num();
//}
//#else // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//bool UWRActorComponentEquipmentLook::Equip(FWREquipmentLookTable* InTable, FWREquipCompleteDelegate InEquipeCompleteDelegate /*= nullptr*/)
//{
//#if !WR_CHANGEEQUIP_MULTI_DIRECT
//	if (ChangeEquipLookDirector.IsDirecting() == true)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Directing Change Equipment Look."));
//		return false;
//	}
//#endif // !WR_CHANGEEQUIP_MULTI_DIRECT
//
//	if (InTable == nullptr)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Null Table"));
//		return false;
//	}
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	auto CreateOrAddTask = [&](const FString& InHashKey) {
//
//		if (AsyncTasks.Contains(InTable->EquipPart) == false)
//		{
//			AsyncTasks.Add(InTable->EquipPart, new FWREquipAsyncTask());
//		}
//
//		FWREquipAsyncTask* Task = AsyncTasks[InTable->EquipPart];
//		Task->AddHash(InHashKey);
//	};
//
//	FString HashKey = "";
//	bool bNeedChangeDirection = true;
//
//	// [ 2020-4-28 : magedoga ] 내 캐릭터 Body = 1인칭 추가
//	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
//	if (MyCharacter)
//	{
//		bNeedChangeDirection = MyCharacter->IsShowingAppearance();
//	}
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	if (WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//	{
//		if (InTable->EquipPart == EWREquipPart::Body)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->FirstPersonSkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			if (HashKey != WRASYNCLOADMANAGER_INVALID)
//			{
//				CreateOrAddTask(HashKey);
//			}
//		}
//
//		bNeedChangeDirection = Cast<AWRCharacterMine>(OwnerCharacter)->IsShowing3rdCharacter();
//	}
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//
//	// [ 2020-4-28 : magedoga ] SkeletalMesh or StaticMesh
//	if (InTable->SkeletalMesh.IsEmpty() == false)
//	{
//		HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->SkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//	}
//	else if (InTable->StaticMesh.IsEmpty() == false)
//	{
//		HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->StaticMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//	}
//
//	if (HashKey != WRASYNCLOADMANAGER_INVALID)
//	{
//		CreateOrAddTask(HashKey);
//	}
//
//	// [ 2020-4-28 : magedoga ] 연출 필요여부
//	if (EquippedPart.Contains(InTable->EquipPart) == false)
//	{
//		bNeedChangeDirection = false;
//	}
//
//	if (bNeedChangeDirection == true)
//	{
//		if (InTable->ChangeCurveSRC.IsNull() == false)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveSRC.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			CreateOrAddTask(HashKey);
//		}
//
//		if (InTable->ChangeCurveDEST.IsNull() == false)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveDEST.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			CreateOrAddTask(HashKey);
//		}
//	}
//
//#if !UE_BUILD_SHIPPING
//	for (auto It = AsyncTasks.CreateIterator(); It; ++It)
//	{
//		FWREquipAsyncTask* AsyncTask = It.Value();
//		for (auto It_Value = AsyncTask->LoadedObjects.CreateIterator(); It_Value; ++It_Value)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - %s"), *(It_Value.Key()));
//		}
//	}
//
//#endif // !UE_BUILD_SHIPPING
//
//	if (AsyncTasks.Contains(InTable->EquipPart) == true)
//	{
//		FWREquipAsyncTask* Task = AsyncTasks[InTable->EquipPart];
//		Task->RequestedTable = InTable;
//		Task->EquipCompleteDelegate = InEquipeCompleteDelegate;
//	}
//
//	return AsyncTasks.Num();
//
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	if (AsyncTasks.Num() > 0)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Async Loading for pre requested Equip"));
//		return false;
//	}
//
//	RequestedTable = InTable;
//	EquipCompleteDelegate = InEquipeCompleteDelegate;
//
//
//
//	//====================================================================================
//	// [ 2020-4-28 : magedoga ] Collect Async Load Task
//	FString HashKey = "";
//	bool bNeedChangeDirection = true;
//
//	// [ 2020-4-28 : magedoga ] 내 캐릭터 Body = 1인칭 추가
//	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
//	if (WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//	{
//		if (InTable->EquipPart == EWREquipPart::Body)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->FirstPersonSkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			if (HashKey != WRASYNCLOADMANAGER_INVALID)
//			{
//				//HashKeys.AddTail(HashKey);
//				AsyncTasks.Add(HashKey, nullptr);
//			}
//		}
//
//		bNeedChangeDirection = Cast<AWRCharacterMine>(OwnerCharacter)->IsShowing3rdCharacter();
//	}
//
//	// [ 2020-4-28 : magedoga ] SkeletalMesh or StaticMesh
//	if (InTable->SkeletalMesh.IsEmpty() == false)
//	{
//		HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->SkeletalMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//	}
//	else if (InTable->StaticMesh.IsEmpty() == false)
//	{
//		HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->StaticMesh, FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//	}
//
//	if (HashKey != WRASYNCLOADMANAGER_INVALID)
//	{
//		AsyncTasks.Add(HashKey, nullptr);
//	}
//
//	// [ 2020-4-28 : magedoga ] 연출 필요여부
//	if (EquippedPart.Contains(InTable->EquipPart) == false)
//	{
//		bNeedChangeDirection = false;
//	}
//
//	if (bNeedChangeDirection == true)
//	{
//		if (InTable->ChangeCurveSRC.IsNull() == false)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveSRC.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			AsyncTasks.Add(HashKey, nullptr);
//		}
//
//		if (InTable->ChangeCurveDEST.IsNull() == false)
//		{
//			HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InTable->ChangeCurveDEST.ToString(), FWRCompleteAsyncLoad::CreateUObject(this, &UWRActorComponentEquipmentLook::OnAsyncLoadComplete));
//			AsyncTasks.Add(HashKey, nullptr);
//		}
//	}
//
//#if !UE_BUILD_SHIPPING
//	for (auto It = AsyncTasks.CreateIterator(); It; ++It)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - %s"), *(It.Key()));
//	}
//	UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Requested Num [ %d ]"), AsyncTasks.Num());
//#endif // !UE_BUILD_SHIPPING
//
//	return AsyncTasks.Num() > 0;
//
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//}
//
//bool UWRActorComponentEquipmentLook::Equip(WRTableID InTableID, FWREquipCompleteDelegate InEquipeCompleteDelegate /*= nullptr*/)
//{
//	return this->Equip(WRTableManager::Get()->FindRow<FWREquipmentLookTable>(InTableID), InEquipeCompleteDelegate);
//}
//#endif // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//
//void UWRActorComponentEquipmentLook::UnEquip(EWREquipPart InPart)
//{
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//
//	UMeshComponent* CurrentMeshComponent = EquippedPart[InPart]->GetMeshComponent();
//	if (CurrentMeshComponent == nullptr)
//	{
//		return;
//	}
//
//	if (ChangeEquipLookDirector.StartDirect(GetOwnerCharacter(), CurrentMeshComponent, nullptr, FWROnCompleteEquipLookChangeDirect::CreateUObject(this, &UWRActorComponentEquipmentLook::OnDirectionComplete), InPart))
//	{
//		return;
//	}
//
//	EquippedPart[InPart]->SetNullMesh();
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//	EquippedPart[InPart]->SetNullMesh();
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//}
//
////====================================================================================
//
//bool UWRActorComponentEquipmentLook::IsEquipedVisible(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return false;
//	}
//	return EquippedPart[InPart]->IsVisible();
//}
//
//void UWRActorComponentEquipmentLook::SetMesh(EWREquipPart InPart, class UStaticMesh* InStaticMesh)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//	EquippedPart[InPart]->SetMesh(InStaticMesh);
//}
//
//void UWRActorComponentEquipmentLook::SetMesh(EWREquipPart InPart, class USkeletalMesh* InSkeletalMesh)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//	EquippedPart[InPart]->SetMesh(InSkeletalMesh);
//}
//
////====================================================================================
//
//FName UWRActorComponentEquipmentLook::GetAttachSocketName(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return NAME_None;
//	}
//	return EquippedPart[InPart]->GetAttachSocketName();
//}
//
//FTransform UWRActorComponentEquipmentLook::GetAttachSocketTransform(EWREquipPart InPart, ERelativeTransformSpace InSpace)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return FTransform::Identity;
//	}
//	return EquippedPart[InPart]->GetAttachSocketTransform(InSpace);
//}
//
//UStaticMesh* UWRActorComponentEquipmentLook::GetStaticMesh(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return nullptr;
//	}
//	return EquippedPart[InPart]->GetStaticMesh();
//}
//
//USkeletalMesh* UWRActorComponentEquipmentLook::GetSkeletalMesh(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return nullptr;
//	}
//	return EquippedPart[InPart]->GetSkeletalMesh();
//}
//
//UMeshComponent* UWRActorComponentEquipmentLook::GetMeshComponent(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return nullptr;
//	}
//	return EquippedPart[InPart]->GetMeshComponent();
//}
//
//EWRAdditionalAbility UWRActorComponentEquipmentLook::GetAdditionalAbility(EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return EWRAdditionalAbility::None;
//	}
//	return EquippedPart[InPart]->GetAdditionalAbilityType();
//}
//
//void UWRActorComponentEquipmentLook::Update_AttachItemToSuitableMesh(AWRCharacterMine* InContext)
//{
//	if (InContext->IsValidLowLevel() == false)
//		return;
//
//	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
//	if (MyCharacter != InContext)
//		return;
//
//	auto CompareAndAttach = [&](EWREquipPart InPart, UMeshComponent* InTargetComponent)
//	{
//		if (InTargetComponent->IsValidLowLevel() == false)
//			return;
//
//		if (EquippedPart.Contains(InPart) == false)
//			return;
//
//
//		if (InPart == EWREquipPart::Body)
//			return;
//
//		FWREquippedMeshComponentWrapping* Wrapping = EquippedPart[InPart];
//		UMeshComponent* EquippedComponent = Wrapping->GetMeshComponent();
//		if (EquippedComponent->GetAttachParent() == InTargetComponent)
//			return;
//
//		EquippedComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//		EquippedComponent->AttachToComponent(InTargetComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, Wrapping->GetAttachSocketName());
//
//	};
//
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	CompareAndAttach(EWREquipPart::LeftHand, MyCharacter->GetMesh());
//	CompareAndAttach(EWREquipPart::RightHand, MyCharacter->GetMesh());
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	// [ 2020-1-9 : magedoga ] showing 3rd
//	if (MyCharacter->IsShowing3rdCharacter() == true)
//	{
//		CompareAndAttach(EWREquipPart::LeftHand, MyCharacter->Get3rdSkeletalMeshComponent());
//		CompareAndAttach(EWREquipPart::RightHand, MyCharacter->Get3rdSkeletalMeshComponent());
//	}
//	else
//	{
//		CompareAndAttach(EWREquipPart::LeftHand, MyCharacter->GetMesh());
//		CompareAndAttach(EWREquipPart::RightHand, MyCharacter->GetMesh());
//	}
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//}
//
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//void UWRActorComponentEquipmentLook::OnEndChangeEquipLookDirect(FWREquipmentLookTable* InTable, EWREquipPart InPart)
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//void UWRActorComponentEquipmentLook::OnEndChangeEquipLookDirect(FWREquipmentLookTable* InTable)
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//{
//	EquippedPart[InTable->EquipPart]->SettingForEquip(InTable);
//
//	//EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1.0f, SetScalarParameterValue, EquippedPart[InTable->EquipPart]->GetMeshComponent());
//
//	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
//	// [ 2020-2-10 : magedoga ] 내 캐릭터 몸 바꾸면 현재 Attach중인 장비들을 바꾼 Mesh에 다시 장착
//	if (InTable->EquipPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//	{
//		this->Update_AttachItemToSuitableMesh(Cast<AWRCharacterMine>(OwnerCharacter));
//	}
//
//	// [ 2020-5-8 : magedoga ] 
//	if (AsyncTasks.Contains(InTable->EquipPart) == true)
//	{
//		FWREquipAsyncTask* ProcessedTask = AsyncTasks[InTable->EquipPart];
//		if (ProcessedTask->HasDirect() == true)
//		{
//			ProcessedTask->ExcuteIfBound(GetOwner());
//
//			EWREquipPart _EquipPart = ProcessedTask->RequestedTable->EquipPart;
//			delete AsyncTasks[_EquipPart];
//			AsyncTasks.Remove(_EquipPart);
//		}
//	}
//}
//
//void UWRActorComponentEquipmentLook::OnDirectionComplete(FWREquipmentLookTable* InTable, EWREquipPart InPart)
//{
//	EWREquipPart EquipmentPart = InPart;
//	if (EquippedPart.Contains(EquipmentPart) == false)
//	{
//		return;
//	}
//
//	EquippedPart[EquipmentPart]->SetNullMesh();
//}
//
//void UWRActorComponentEquipmentLook::OnAsyncLoadComplete(UObject* InObject, FString InHashKey)
//{
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//	bool bProcessed = false;
//	FWREquipAsyncTask* ProcessedTask = nullptr;
//	for (auto It = AsyncTasks.CreateIterator(); It; ++It)
//	{
//		FWREquipAsyncTask* Task = It.Value();
//		if (Task->ContainsHash(InHashKey) == true && Task->HasObject(InHashKey) == false)
//		{
//			InObject->AddToRoot();
//			Task->AddObject(InHashKey, InObject);
//
//			ProcessedTask = Task;
//			bProcessed = true;
//			break;
//		}
//	}
//
//	if (bProcessed == false)
//	{
//		UE_LOG(EquipmentLook, Fatal, TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete - Not Requested Hash Key."));
//		return;
//	}
//
//	UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete - %s"), *InHashKey);
//
//	if (WRAsyncLoadUObjectManager::IsLoadCompleteTasks(ProcessedTask->LoadedObjects) == true)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete - Load Complete."));
//		bool bNeedChangeDirection = NextStepAfterAsyncLoad(ProcessedTask->RequestedTable);
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		if (bNeedChangeDirection == false)
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		if (ProcessedTask->HasDirect() == false)
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//		{
//			ProcessedTask->ExcuteIfBound(GetOwner());
//
//			EWREquipPart _EquipPart = ProcessedTask->RequestedTable->EquipPart;
//			delete AsyncTasks[_EquipPart];
//			AsyncTasks.Remove(_EquipPart);
//		}
//	}
//
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	if (AsyncTasks.Contains(InHashKey) == false)
//	{
//		UE_LOG(EquipmentLook, Fatal, TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete - Not Requested Hash Key."));
//		return;
//	}
//
//#if !UE_BUILD_SHIPPING
//	UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::OnAsyncLoadComplete - %s"), *InHashKey);
//#endif // !UE_BUILD_SHIPPING
//
//	InObject->AddToRoot();
//	AsyncTasks[InHashKey] = InObject;
//
//	if (WRAsyncLoadUObjectManager::IsLoadCompleteTasks(AsyncTasks) == true)
//	{
//		this->NextStepAfterAsyncLoad();
//	}
//
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//}
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//bool UWRActorComponentEquipmentLook::NextStepAfterAsyncLoad(FWREquipmentLookTable* InTable)
//#else
//void UWRActorComponentEquipmentLook::NextStepAfterAsyncLoad()
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//{
//	bool bNeedChangeDirection = true;
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//	bool bAlreadyNew = false;
//
//	// [ 2019-9-11 : magedoga ] 해당 부위 이미 생성된 Component가 있는지 확인 (없으면 생성)
//	if (EquippedPart.Contains(InTable->EquipPart) == false)
//	{
//		FWREquippedMeshComponentWrapping* EquippedComponent = nullptr;
//
//		// [ 2019-9-17 : magedoga ] Table에 StaticMesh가 있으면 StaticMeshComponent로 생성, SkeletalMesh가 있으면 SkeletalMeshComponent로 생성
//		// 1. StaticMesh -> 2. SkeletalMesh -> 3. Invalid return
//		if (InTable->StaticMesh.IsEmpty() == false)
//		{
//			EquippedComponent = new FWREquippedMeshComponentWrapping_StaticMesh(InTable, GetOwnerCharacter());
//		}
//		else if (InTable->SkeletalMesh.IsEmpty() == false)
//		{
//			EquippedComponent = new FWREquippedMeshComponentWrapping_SkeletalMesh(InTable, GetOwnerCharacter());
//		}
//
//		if (EquippedComponent == nullptr)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Null EquippedComponent. Please check data table"));
//			return false;
//		}
//
//		EquippedPart.Add(InTable->EquipPart, EquippedComponent);
//
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//		bNeedChangeDirection = false;
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//		bAlreadyNew = true;
//	}
//
//	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
//
//	// [ 2020-2-10 : magedoga ] 내 캐릭터 연출여부 체크
//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
//	if (MyCharacter)
//	{
//		if (MyCharacter->IsShowingAppearance() == false)
//		{
//			bNeedChangeDirection = false;
//		}
//	}
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//	if (WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//	{
//		if (Cast<AWRCharacterMine>(OwnerCharacter)->IsShowing3rdCharacter() == false)
//		{
//			bNeedChangeDirection = false;
//		}
//	}
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//
//	if (bAlreadyNew == false)
//	{
//		bNeedChangeDirection = InTable->ChangeCurveDEST.IsNull() == false;
//	}
//
//	// [ 2020-2-10 : magedoga ] 연출 불필요
//	if (bNeedChangeDirection == false)
//	{
//		EquippedPart[InTable->EquipPart]->SettingForEquip(InTable);
//
//		// [ 2020-2-10 : magedoga ] 내 캐릭터 몸 바꾸면 현재 Attach중인 장비들을 바꾼 Mesh에 다시 장착
//		if (/*InTable->EquipPart == EWREquipPart::Body &&*/ WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//		{
//			this->Update_AttachItemToSuitableMesh(Cast<AWRCharacterMine>(OwnerCharacter));
//		}
//
//	}
//	// [ 2020-2-10 : magedoga ] 연출 필요
//	else
//	{
//		UMeshComponent* CurrentPartMeshComponent = EquippedPart[InTable->EquipPart]->GetMeshComponent();
//		if (InTable->EquipPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//		{
//			CurrentPartMeshComponent = OwnerCharacter->GetCurrentSkeletalMeshComponent();
//		}
//
//		bool bSuccess = ChangeEquipLookDirector.StartDirect(OwnerCharacter, CurrentPartMeshComponent, InTable, FWROnCompleteEquipLookChangeDirect::CreateUObject(this, &UWRActorComponentEquipmentLook::OnEndChangeEquipLookDirect));
//		if (bSuccess == false)
//		{
//			EquippedPart[InTable->EquipPart]->SettingForEquip(InTable);
//
//			// [ 2020-2-10 : magedoga ] 내 캐릭터 몸 바꾸면 현재 Attach중인 장비들을 바꾼 Mesh에 다시 장착
//			if (/*InTable->EquipPart == EWREquipPart::Body &&*/ WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//			{
//				this->Update_AttachItemToSuitableMesh(Cast<AWRCharacterMine>(OwnerCharacter));
//			}
//		}
//
//	}
//
//	return bNeedChangeDirection;
//
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	// [ 2019-9-11 : magedoga ] 해당 부위 이미 생성된 Component가 있는지 확인 (없으면 생성)
//	if (EquippedPart.Contains(RequestedTable->EquipPart) == false)
//	{
//		FWREquippedMeshComponentWrapping* EquippedComponent = nullptr;
//
//		// [ 2019-9-17 : magedoga ] Table에 StaticMesh가 있으면 StaticMeshComponent로 생성, SkeletalMesh가 있으면 SkeletalMeshComponent로 생성
//		// 1. StaticMesh -> 2. SkeletalMesh -> 3. Invalid return
//		if (RequestedTable->StaticMesh.IsEmpty() == false)
//		{
//			EquippedComponent = new FWREquippedMeshComponentWrapping_StaticMesh(RequestedTable, GetOwnerCharacter());
//		}
//		else if (RequestedTable->SkeletalMesh.IsEmpty() == false)
//		{
//			EquippedComponent = new FWREquippedMeshComponentWrapping_SkeletalMesh(RequestedTable, GetOwnerCharacter());
//		}
//
//		if (EquippedComponent == nullptr)
//		{
//			UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::Equip - Null EquippedComponent. Please check data table"));
//			return;
//		}
//
//		EquippedPart.Add(RequestedTable->EquipPart, EquippedComponent);
//
//		bNeedChangeDirection = false;
//	}
//
//	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
//
//	// [ 2020-2-10 : magedoga ] 내 캐릭터 연출여부 체크
//	if (WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//	{
//		if (Cast<AWRCharacterMine>(OwnerCharacter)->IsShowing3rdCharacter() == false)
//		{
//			bNeedChangeDirection = false;
//		}
//	}
//
//	auto EmptyAsyncTask = [&]() {
//		// [ 2020-4-28 : magedoga ] Flush Async Task
//		for (auto It = AsyncTasks.CreateIterator(); It; ++It)
//		{
//			UObject* Value = It.Value();
//			if (Value->IsValidLowLevel() == true && Value->IsRooted())
//			{
//				Value->RemoveFromRoot();
//			}
//		}
//
//		AsyncTasks.Empty();
//	};
//
//
//	// [ 2020-2-10 : magedoga ] 연출 불필요
//	if (bNeedChangeDirection == false)
//	{
//		EquippedPart[RequestedTable->EquipPart]->SettingForEquip(RequestedTable);
//
//		// [ 2020-2-10 : magedoga ] 내 캐릭터 몸 바꾸면 현재 Attach중인 장비들을 바꾼 Mesh에 다시 장착
//		if (RequestedTable->EquipPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//		{
//			this->Update_AttachItemToSuitableMesh(Cast<AWRCharacterMine>(OwnerCharacter));
//		}
//
//		EmptyAsyncTask();
//	}
//	// [ 2020-2-10 : magedoga ] 연출 필요
//	else
//	{
//		UMeshComponent* CurrentPartMeshComponent = EquippedPart[RequestedTable->EquipPart]->GetMeshComponent();
//		if (RequestedTable->EquipPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//		{
//			CurrentPartMeshComponent = OwnerCharacter->GetCurrentSkeletalMeshComponent();
//		}
//
//		bool bSuccess = ChangeEquipLookDirector.StartDirect(OwnerCharacter, CurrentPartMeshComponent, RequestedTable, FWROnCompleteEquipLookChangeDirect::CreateUObject(this, &UWRActorComponentEquipmentLook::OnEndChangeEquipLookDirect));
//		if (bSuccess == false)
//		{
//			EquippedPart[RequestedTable->EquipPart]->SettingForEquip(RequestedTable);
//
//			// [ 2020-2-10 : magedoga ] 내 캐릭터 몸 바꾸면 현재 Attach중인 장비들을 바꾼 Mesh에 다시 장착
//			if (RequestedTable->EquipPart == EWREquipPart::Body && WRCharacterManager::Get()->IsMyCharacter(OwnerCharacter) == true)
//			{
//				this->Update_AttachItemToSuitableMesh(Cast<AWRCharacterMine>(OwnerCharacter));
//			}
//		}
//
//		EmptyAsyncTask();
//	}
//
//	if (EquipCompleteDelegate.IsBound() == true)
//	{
//		EquipCompleteDelegate.ExecuteIfBound(GetOwner());
//		EquipCompleteDelegate.Unbind();
//	}
//
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//}
//
////====================================================================================
//
//void UWRActorComponentEquipmentLook::SetVisibility(EWREquipPart InPart, const bool bInVisible)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//	EquippedPart[InPart]->SetVisibility(bInVisible);
//}
//
//bool UWRActorComponentEquipmentLook::SetCollisionEnable(EWREquipPart InPart, ECollisionEnabled::Type InCollisionEnableType)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		UE_LOG(EquipmentLook, Display, TEXT("UWRActorComponentEquipmentLook::SetCollisionEnable - Not contains [ %s ]"), *WREnumUtility::EnumToString<EWREquipPart>("EWREquipPart", InPart));
//		return false;
//	}
//	return EquippedPart[InPart]->SetCollisionEnable(InCollisionEnableType);
//}
////====================================================================================
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//bool FWREquipmentLookChangeDirector::StartDirect(AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, FWREquipmentLookTable* InTable, FWROnCompleteEquipLookChangeDirect InCallback, EWREquipPart InPart /*= EWREquipPart::Body*/)
//{
//	if (InCharacter == nullptr)
//	{
//		return false;
//	}
//	if (InCurrentMeshComponent == nullptr)
//	{
//		return false;
//	}
//	if (InCallback.IsBound() == false)
//	{
//		return false;
//	}
//
//	// [ 2020-6-23 : animal14 ] 장비 해제
//	if (InTable == nullptr)
//	{
//		FWREquipDirecteData* DirectingData = new FWREquipDirecteData();
//
//		UCurveFloat* SrcCurve = LoadObject<UCurveFloat>(nullptr, TEXT(""));
//		if (SrcCurve)
//		{
//			SrcCurve->AddToRoot();
//
//			DirectingData->CurveSRC = SrcCurve;
//		}
//
//		DirectingData->TargetCharacter = InCharacter;
//		DirectingData->NextEquipLookTable = nullptr;
//		DirectingData->Part = InPart;
//		DirectingData->CompleteCallback = InCallback;
//		DirectingData->CurrentPartMeshComponent = InCurrentMeshComponent;
//		DirectingData->AccTime = 0.0F;
//
//		DirectingDatas.AddTail(DirectingData);
//
//		DirectingData->Action = EWREquipmentLookAction::Unequipment;
//
//		EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//	}
//	else
//	{
//		FWREquipDirecteData* DirectingData = new FWREquipDirecteData();
//
//		UCurveFloat* SrcCurve = InTable->ChangeCurveSRC.LoadSynchronous();
//		if (SrcCurve)
//		{
//			SrcCurve->AddToRoot();
//
//			DirectingData->CurveSRC = SrcCurve;
//		}
//
//		UCurveFloat* DestCurve = InTable->ChangeCurveDEST.LoadSynchronous();
//		if (DestCurve)
//		{
//			DestCurve->AddToRoot();
//
//			DirectingData->CurveDEST = DestCurve;
//		}
//
//		DirectingData->TargetCharacter = InCharacter;
//		DirectingData->NextEquipLookTable = InTable;
//		DirectingData->Part = InTable->EquipPart;
//		DirectingData->CompleteCallback = InCallback;
//		DirectingData->CurrentPartMeshComponent = InCurrentMeshComponent;
//		DirectingData->AccTime = 0.0F;
//
//		DirectingDatas.AddTail(DirectingData);
//
//		EWREquipPart EquipmentPart = InTable->EquipPart;
//		if (IsStaticMesh(EquipmentPart))
//		{
//			// [ 2020-6-23 : animal14 ] 임시 메시 컴포넌트 생성
//			CreateStaticMeshComponent(InCharacter, DirectingData, InTable);
//
//			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(InCurrentMeshComponent);
//			if (StaticMeshComponent == nullptr)
//			{
//				return false;
//			}
//
//			// [ 2020-6-23 : animal14 ] 장비 장착
//			if (StaticMeshComponent->GetStaticMesh() == nullptr)
//			{
//				DirectingData->Action = EWREquipmentLookAction::Equipment;
//
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 0, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//			}
//			// [ 2020-6-23 : animal14 ] 장비 교체
//			else
//			{
//				DirectingData->Action = EWREquipmentLookAction::Exchange;
//
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 0, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//			}
//		}
//		else
//		{
//			// [ 2020-6-23 : animal14 ] 임시 메시 컴포넌트 생성
//			CreateSkeletalMeshComponent(InCharacter, DirectingData, InTable);
//
//			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InCurrentMeshComponent);
//			if (SkeletalMeshComponent == nullptr)
//			{
//				return false;
//			}
//
//			// [ 2020-6-23 : animal14 ] 장비 장착
//			if (SkeletalMeshComponent->SkeletalMesh == nullptr)
//			{
//				DirectingData->Action = EWREquipmentLookAction::Equipment;
//
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 0, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//			}
//			// [ 2020-6-23 : animal14 ] 장비 교체
//			else
//			{
//				DirectingData->Action = EWREquipmentLookAction::Exchange;
//
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//				EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 0, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//			}
//		}
//	}
//	return true;
//}
//
//void FWREquipmentLookChangeDirector::CreateStaticMeshComponent(AWRCharacter* InCharacter, FWREquipDirecteData* InDirectingData, FWREquipmentLookTable* InTable)
//{
//	if (InCharacter == nullptr)
//	{
//		return;
//	}
//
//	USkeletalMeshComponent* MainMeshComponent = InCharacter->GetCurrentSkeletalMeshComponent();
//	if (MainMeshComponent == nullptr)
//	{
//		return;
//	}
//
//	UStaticMeshComponent* TempStaticMeshComponent = NewObject<UStaticMeshComponent>(InCharacter);
//	if (TempStaticMeshComponent)
//	{
//		TempStaticMeshComponent->RegisterComponentWithWorld(InCharacter->GetWorld());
//
//		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *InTable->StaticMesh);
//		if (StaticMesh == nullptr)
//		{
//#if UE_EDITOR
//			TArray<FStringFormatArg> StringArgs;
//			StringArgs.Add(InTable->StaticMesh);
//
//			FString Text = FString::Format(TEXT("StaticMesh ({0}) is nullptr."), StringArgs);
//
//			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRActorComponentEquipmentLook::StartDirect()"));
//#else // UE_EDITOR
//#endif // UE_EDITOR
//		}
//
//		TempStaticMeshComponent->SetStaticMesh(StaticMesh);
//		TempStaticMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);	// [ 2020-6-23 : animal14 ] 임시 코드
//
//		InDirectingData->ChangePartMeshComponent = TempStaticMeshComponent;
//	}
//#if UE_EDITOR
//	else
//	{
//		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("StaticMeshComponent is nullptr."), TEXT("UWRActorComponentEquipmentLook::StartDirect()"));
//	}
//#else // UE_EDITOR
//#endif // UE_EDITOR
//}
//
//void FWREquipmentLookChangeDirector::CreateSkeletalMeshComponent(AWRCharacter* InCharacter, FWREquipDirecteData* InDirectingData, FWREquipmentLookTable* InTable)
//{
//	if (InCharacter == nullptr)
//	{
//		return;
//	}
//
//	USkeletalMeshComponent* MainMeshComponent = InCharacter->GetCurrentSkeletalMeshComponent();
//	if (MainMeshComponent == nullptr)
//	{
//		return;
//	}
//
//	USkeletalMeshComponent* TempSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(InCharacter);
//	if (TempSkeletalMeshComponent)
//	{
//		TempSkeletalMeshComponent->RegisterComponentWithWorld(InCharacter->GetWorld());
//
//		USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InTable->SkeletalMesh);
//		if (SkeletalMesh == nullptr)
//		{
//#if UE_EDITOR
//			TArray<FStringFormatArg> StringArgs;
//			StringArgs.Add(InTable->SkeletalMesh);
//
//			FString Text = FString::Format(TEXT("SkeletalMesh ({0}) is nullptr."), StringArgs);
//
//			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRActorComponentEquipmentLook::StartDirect()"));
//#else // UE_EDITOR
//#endif // UE_EDITOR
//		}
//
//		TempSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
//		// [ 2020-6-24 : animal14 ] 장비 교체
//		if (MainMeshComponent->SkeletalMesh)
//		{
//			TempSkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);
//		}
//		// [ 2020-6-24 : animal14 ] 장비 장착
//		else
//		{
//			TempSkeletalMeshComponent->SetAnimClass(MainMeshComponent->GetAnimClass());
//		}
//		TempSkeletalMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
//
//		InCharacter->AddInstanceComponent(TempSkeletalMeshComponent);
//		InCharacter->AddOwnedComponent(TempSkeletalMeshComponent);
//
//		InDirectingData->ChangePartMeshComponent = TempSkeletalMeshComponent;
//	}
//#if UE_EDITOR
//	else
//	{
//		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SkeletalMeshComponent is nullptr."), TEXT("UWRActorComponentEquipmentLook::StartDirect()"));
//	}
//#else // UE_EDITOR
//#endif // UE_EDITOR
//}
//
//void FWREquipmentLookChangeDirector::StopDirect()
//{
//	if (IsDirecting() == false)
//	{
//		return;
//	}
//
//	for (auto& DirectingData : DirectingDatas)
//	{
//		OnPreEndDirect(DirectingData);
//		OnPostEndDirect(DirectingData);
//
//		DirectingDatas.RemoveNode(DirectingData);
//	}
//}
//
//bool FWREquipmentLookChangeDirector::TickDirect(float DeltaTime)
//{
//	for (auto& DirectingData : DirectingDatas)
//	{
//		DirectingData->AccTime += DeltaTime;
//
//		float SrcMaxTime = 0.0F;
//		UCurveFloat* SrcCurve = DirectingData->CurveSRC;
//		if (SrcCurve)
//		{
//			SrcCurve->GetTimeRange(SrcMaxTime, SrcMaxTime);
//		}
//
//		float DestMaxTime = 0.0F;
//		UCurveFloat* DestCurve = DirectingData->CurveDEST;
//		if (DestCurve)
//		{
//			DestCurve->GetTimeRange(DestMaxTime, DestMaxTime);
//		}
//
//		float SrcValue = SrcCurve ? SrcCurve->GetFloatValue(DirectingData->AccTime) : 0.0F;
//		float DestValue = DestCurve ? DestCurve->GetFloatValue(DirectingData->AccTime) : 0.0F;
//
//		switch (DirectingData->Action)
//		{
//		case EWREquipmentLookAction::Equipment:
//		{
//			EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, DestValue, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//		}
//		break;
//		case EWREquipmentLookAction::Unequipment:
//		{
//			EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, SrcValue, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//		}
//		break;
//		case EWREquipmentLookAction::Exchange:
//		{
//			EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, SrcValue, SetScalarParameterValue, DirectingData->CurrentPartMeshComponent);
//			EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, DestValue, SetScalarParameterValue, DirectingData->ChangePartMeshComponent);
//		}
//		break;
//#if UE_EDITOR
//		default:
//			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT(""), TEXT("UWRActorComponentEquipmentLook::TickDirect()"));
//			break;
//#else // UE_EDITOR
//#endif // UE_EDITOR
//		}
//
//		if (DirectingData->AccTime < FMath::Max(SrcMaxTime, DestMaxTime))
//		{
//			continue;
//		}
//
//		OnPreEndDirect(DirectingData);
//
//		DirectingData->CompleteCallback.ExecuteIfBound(DirectingData->NextEquipLookTable, DirectingData->Part);
//
//		OnPostEndDirect(DirectingData);
//
//		DirectingDatas.RemoveNode(DirectingData);
//	}
//	return true;
//}
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//bool FWREquipmentLookChangeDirector::StartDirect(AWRCharacter* InTargetCharacter, UMeshComponent* InCurrentPartMeshComponent, FWREquipmentLookTable* InTable, FWROnCompleteEquipLookChangeDirect InCompleteCallback)
//{
//	if (InTargetCharacter->IsValidLowLevel() == false)
//		return false;
//
//	if (InCompleteCallback.IsBound() == false)
//		return false;
//
//	if (InCurrentPartMeshComponent->IsValidLowLevel() == false)
//		return false;
//
//	if (InTable == nullptr)
//		return false;
//
//	if (InTable->ChangeCurveSRC.IsNull() == true)
//		return false;
//
//	if (InTable->ChangeCurveDEST.IsNull() == true)
//		return false;
//
//	// [ 2020-2-10 : magedoga ] Attach할 메인메시
//	USkeletalMeshComponent* MainMeshComponent = InTargetCharacter->GetCurrentSkeletalMeshComponent();
//	if (MainMeshComponent->IsValidLowLevel() == false)
//		return false;
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//
//	FWREquipDirecteData* DirectData = new FWREquipDirecteData();
//	FStringAssetReference ChangeCurveSRC = FStringAssetReference(InTable->ChangeCurveSRC.ToString());
//	DirectData->CurveSRC = Cast<UCurveFloat>(ChangeCurveSRC.ResolveObject());
//	if (DirectData->CurveSRC->IsRooted() == false)
//	{
//		DirectData->CurveSRC->AddToRoot();
//	}
//
//	FStringAssetReference ChangeCurveDEST = FStringAssetReference(InTable->ChangeCurveDEST.ToString());
//	DirectData->CurveDEST = Cast<UCurveFloat>(ChangeCurveDEST.ResolveObject());
//	if (DirectData->CurveDEST->IsRooted() == false)
//	{
//		DirectData->CurveDEST->AddToRoot();
//	}
//
//	// [ 2020-2-10 : magedoga ] 임시 MeshComponent 생성 및 Attach
//	if (IsStaticMesh(InTable->EquipPart) == false)
//	{
//		USkeletalMeshComponent* TempSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(InTargetCharacter);
//		TempSkeletalMeshComponent->RegisterComponentWithWorld(InTargetCharacter->GetWorld());
//
//		USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InTable->SkeletalMesh);
//		TempSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
//		TempSkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);
//		TempSkeletalMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
//
//		DirectData->ChangePartMeshComponent = TempSkeletalMeshComponent;
//	}
//	else
//	{
//		UStaticMeshComponent* TempStaticMeshComponent = NewObject<UStaticMeshComponent>(InTargetCharacter);
//		TempStaticMeshComponent->RegisterComponentWithWorld(InTargetCharacter->GetWorld());
//
//		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(UWRGameInstance::GetGameInstance()->GetWorld(), *InTable->StaticMesh);
//		TempStaticMeshComponent->SetStaticMesh(StaticMesh);
//		TempStaticMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, InTable->AttachSocket);
//
//		DirectData->ChangePartMeshComponent = TempStaticMeshComponent;
//	}
//
//	// [ 2020-2-10 : magedoga ] 초기 Material Parameter 값 설정
//	this->SetParameter(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1.0f, 0.0f, DirectData);
//
//	DirectData->TargetCharacter = InTargetCharacter;
//	DirectData->NextEquipLookTable = InTable;
//	DirectData->CompleteCallback = InCompleteCallback;
//	DirectData->CurrentPartMeshComponent = InCurrentPartMeshComponent;
//	DirectData->AccTime = 0.0f;
//
//	DirectingDatas.AddTail(DirectData);
//
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//	FStringAssetReference ChangeCurveSRC = FStringAssetReference(InTable->ChangeCurveSRC.ToString());
//	CurveSRC = Cast<UCurveFloat>(ChangeCurveSRC.ResolveObject());
//	if (CurveSRC->IsRooted() == false)
//	{
//		CurveSRC->AddToRoot();
//	}
//
//	FStringAssetReference ChangeCurveDEST = FStringAssetReference(InTable->ChangeCurveDEST.ToString());
//	CurveDEST = Cast<UCurveFloat>(ChangeCurveDEST.ResolveObject());
//	if (CurveDEST->IsRooted() == false)
//	{
//		CurveDEST->AddToRoot();
//	}
//
//
//	// [ 2020-2-10 : magedoga ] 임시 MeshComponent 생성 및 Attach
//	if (InTable->EquipPart == EWREquipPart::Body)
//	{
//		USkeletalMeshComponent* TempSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(InTargetCharacter);
//		TempSkeletalMeshComponent->RegisterComponentWithWorld(InTargetCharacter->GetWorld());
//
//		USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InTable->SkeletalMesh);
//		TempSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
//		TempSkeletalMeshComponent->SetMasterPoseComponent(MainMeshComponent);
//		TempSkeletalMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
//
//		ChangePartMeshComponent = TempSkeletalMeshComponent;
//	}
//	else
//	{
//		UStaticMeshComponent* TempStaticMeshComponent = NewObject<UStaticMeshComponent>(InTargetCharacter);
//		TempStaticMeshComponent->RegisterComponentWithWorld(InTargetCharacter->GetWorld());
//
//		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(UWRGameInstance::GetGameInstance()->GetWorld(), *InTable->StaticMesh);
//		TempStaticMeshComponent->SetStaticMesh(StaticMesh);
//		TempStaticMeshComponent->AttachToComponent(MainMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, InTable->AttachSocket);
//
//		ChangePartMeshComponent = TempStaticMeshComponent;
//	}
//
//	// [ 2020-2-10 : magedoga ] 초기 Material Parameter 값 설정
//	this->SetParameter(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1.0f, 0.0f);
//
//	TargetCharacter = InTargetCharacter;
//	NextEquipLookTable = InTable;
//	CompleteCallback = InCompleteCallback;
//
//	CurrentPartMeshComponent = InCurrentPartMeshComponent;
//
//	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FWREquipmentLookChangeDirector::TickDirect));
//
//	AccTime = 0.0f;
//
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//	return true;
//}
//
//void FWREquipmentLookChangeDirector::StopDirect()
//{
//	if (this->IsDirecting() == false)
//		return;
//
//	for (auto Task : DirectingDatas)
//	{
//		this->OnPreEndDirect(Task);
//		this->OnPostEndDirect(Task);
//
//		DirectingDatas.RemoveNode(Task);
//	}
//}
//
//bool FWREquipmentLookChangeDirector::TickDirect(float InDeltaTime)
//{
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//
//	for (auto Task : DirectingDatas)
//	{
//		Task->AccTime += InDeltaTime;
//		this->SetParameter(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, Task->CurveSRC->GetFloatValue(Task->AccTime), Task->CurveDEST->GetFloatValue(Task->AccTime), Task);
//
//		float SRCMaxTime, DESTMaxTime;
//		Task->CurveSRC->GetTimeRange(SRCMaxTime, SRCMaxTime);
//		Task->CurveDEST->GetTimeRange(DESTMaxTime, DESTMaxTime);
//
//		// [ 2020-2-10 : magedoga ] 종료
//		if (Task->AccTime >= SRCMaxTime && Task->AccTime >= DESTMaxTime)
//		{
//			this->OnPreEndDirect(Task);
//
//			Task->CompleteCallback.ExecuteIfBound(Task->NextEquipLookTable);
//
//			this->OnPostEndDirect(Task);
//
//			DirectingDatas.RemoveNode(Task);
//		}
//	}
//
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//	AccTime += InDeltaTime;
//	this->SetParameter(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, CurveSRC->GetFloatValue(AccTime), CurveDEST->GetFloatValue(AccTime));
//
//	float SRCMaxTime, DESTMaxTime;
//	CurveSRC->GetTimeRange(SRCMaxTime, SRCMaxTime);
//	CurveDEST->GetTimeRange(DESTMaxTime, DESTMaxTime);
//
//	// [ 2020-2-10 : magedoga ] 종료
//	if (AccTime >= SRCMaxTime && AccTime >= DESTMaxTime)
//	{
//		this->OnPreEndDirect();
//
//		CompleteCallback.ExecuteIfBound(NextEquipLookTable);
//
//		this->OnPostEndDirect();
//
//		return false;
//	}
//
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//	return true;
//}
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//void FWREquipmentLookChangeDirector::SetParameter(FName InParameterName, float InSRCValue, float InDESTValue, FWREquipDirecteData* InDirectData)
//{
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InSRCValue, SetScalarParameterValue, InDirectData->CurrentPartMeshComponent);
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InDESTValue, SetScalarParameterValue, InDirectData->ChangePartMeshComponent);
//}
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//void FWREquipmentLookChangeDirector::SetParameter(FName InParameterName, float InSRCValue, float InDESTValue)
//{
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InSRCValue, SetScalarParameterValue, CurrentPartMeshComponent);
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(InParameterName, InDESTValue, SetScalarParameterValue, ChangePartMeshComponent);
//}
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//void FWREquipmentLookChangeDirector::OnPreEndDirect(FWREquipDirecteData* InData)
//{
//	// [ 2020-2-10 : magedoga ] 원복
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1.0f, SetScalarParameterValue, InData->CurrentPartMeshComponent);
//
//	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InData->ChangePartMeshComponent);
//	if (SkeletalMeshComponent->IsValidLowLevel() == true)
//	{
//		SkeletalMeshComponent->SetMasterPoseComponent(nullptr);
//	}
//
//	if (InData->ChangePartMeshComponent)
//	{
//		InData->ChangePartMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//		InData->ChangePartMeshComponent->ConditionalBeginDestroy();
//		InData->ChangePartMeshComponent = nullptr;
//	}
//
//	InData->TargetCharacter = nullptr;
//
//	InData->CurrentPartMeshComponent = nullptr;
//	InData->ChangePartMeshComponent = nullptr;
//
//	if (InData->CurveSRC->IsValidLowLevel() == true)
//	{
//		InData->CurveSRC->RemoveFromRoot();
//		InData->CurveSRC = nullptr;
//	}
//
//	if (InData->CurveDEST->IsValidLowLevel() == true)
//	{
//		InData->CurveDEST->RemoveFromRoot();
//		InData->CurveDEST = nullptr;
//	}
//}
//
//void FWREquipmentLookChangeDirector::OnPostEndDirect(FWREquipDirecteData* InData)
//{
//	InData->CompleteCallback.Unbind();
//	InData->NextEquipLookTable = nullptr;
//}
//
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//void FWREquipmentLookChangeDirector::OnPreEndDirect()
//{
//	// [ 2020-2-10 : magedoga ] 원복
//	EQUIPMENTLOOK_SETMATERIAL_PARAMETER(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, 1.0f, SetScalarParameterValue, CurrentPartMeshComponent);
//
//	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ChangePartMeshComponent);
//	if (SkeletalMeshComponent->IsValidLowLevel() == true)
//	{
//		SkeletalMeshComponent->SetMasterPoseComponent(nullptr);
//	}
//
//	ChangePartMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//	ChangePartMeshComponent->ConditionalBeginDestroy();
//	ChangePartMeshComponent = nullptr;
//
//	TargetCharacter = nullptr;
//
//	CurrentPartMeshComponent = nullptr;
//	ChangePartMeshComponent = nullptr;
//
//	if (CurveSRC->IsValidLowLevel() == true)
//	{
//		CurveSRC->RemoveFromRoot();
//		CurveSRC = nullptr;
//	}
//
//	if (CurveDEST->IsValidLowLevel() == true)
//	{
//		CurveDEST->RemoveFromRoot();
//		CurveDEST = nullptr;
//	}
//
//
//	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
//	TickHandle.Reset();
//}
//
//void FWREquipmentLookChangeDirector::OnPostEndDirect()
//{
//	CompleteCallback.Unbind();
//	NextEquipLookTable = nullptr;
//}
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT