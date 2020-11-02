// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentRevive.h"

#include "Actor/Character/WRCharacterMine.h"

#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRSFXManager.h"

#include "Define/WRPathDefine.h"

void UWRActorComponentRevive::OnCreateComponent()
{
	Super::OnCreateComponent();

	MaterialInterface = LoadObject<UMaterialInterface>(nullptr, *MATERIAL_PATH_4_REVIVE);
	if (MaterialInterface == nullptr)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MaterialInterface == nullptr"), TEXT("UWRActorComponentRevive::OnCreateComponent()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
}

void UWRActorComponentRevive::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentRevive::ReviveOut()
{
	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::WhiteOutIn_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::ReviveOut, Duration);

	this->SetReviveVisibility(true);

	WRSFXManager::Get()->Spawn2D(SOUND_PATH_4_REVIVE);

	// UI Ãâ·Â
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Revive, EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Revive, FTransform(FRotator(0.f, 180.f, 0.f), FVector(200, 0, 0), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll, nullptr);
}

void UWRActorComponentRevive::ReviveIn()
{
	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::WhiteOutIn_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::ReviveIn, Duration);

	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::ReviveIn).AddUObject(this, &UWRActorComponentRevive::CompleteReviveIn);
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::Revive);
}

void UWRActorComponentRevive::SetReviveVisibility(bool bNewVisibility)
{
	AWRCharacterMine* Me = Cast<AWRCharacterMine>(this->GetOwner());
	if (Me == nullptr)
	{
		return;
	}

	UStaticMeshComponent* StaticMeshComponent = Me->GetSphereFlippedNormalComponent();
	if (StaticMeshComponent == nullptr)
	{
		return;
	}

	StaticMeshComponent->SetVisibility(bNewVisibility);

	if (bNewVisibility)
	{
		if (MaterialInterface == nullptr)
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("MaterialInterface == nullptr"), TEXT("UWRActorComponentRevive::SetReviveVisibility()"));
#else // UE_EDITOR
#endif // UE_EDITOR
			return;
		}

		TranslucencySortPriority = StaticMeshComponent->TranslucencySortPriority;
		StaticMeshComponent->TranslucencySortPriority = WRConsoleManager::Get()->GetInt(EWRConsole::TranslucencySortPriority4Revive);

		NewScale3D = StaticMeshComponent->GetRelativeScale3D();
		StaticMeshComponent->SetRelativeScale3D(FVector::OneVector * 0.25F);

		Materials = StaticMeshComponent->GetMaterials();
		for (int32 i = 0; i < Materials.Num(); ++i)
		{
			StaticMeshComponent->SetMaterial(i, MaterialInterface);
		}
	}
	else
	{
		StaticMeshComponent->TranslucencySortPriority = TranslucencySortPriority;
		StaticMeshComponent->SetRelativeScale3D(NewScale3D);

		for (int32 i = 0; i < Materials.Num(); ++i)
		{
			StaticMeshComponent->SetMaterial(i, Materials[i]);
		}
	}
}

void UWRActorComponentRevive::CompleteReviveIn()
{
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::ReviveIn).RemoveAll(this);

	this->SetReviveVisibility(false);
}