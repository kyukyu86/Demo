// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCapsuleCollisionComponent.h"
#include "Actor/Character/WRCharacter.h"

#include "Manager/WRVFXManager.h"

#include "Define/WRPathDefine.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Core/WRGameInstance.h"
#include "DrawDebugHelpers.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRConsoleManager.h"
#include "UI/Panel/WRUIPanel_IOPopup.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Enum/EWRCharacter.h"
#include "Table/WRPlayerTable.h"
#include "Table/Base/WRTableManager.h"
#include "Utility/WRActorUtility.h"

#include "Localization/WRLocalizationProcessor.h"

UWRCapsuleCollisionComponent::UWRCapsuleCollisionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

UWRCapsuleCollisionComponent::UWRCapsuleCollisionComponent()
{

}

void UWRCapsuleCollisionComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->RemoveCapsuleCollisionComponentInfo(CollisionType);
	}
}

void UWRCapsuleCollisionComponent::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	AWRVFX* VFX = Cast<AWRVFX>(this->GetOwner());
	AWRCharacter* Character = (VFX == nullptr) ? Cast<AWRCharacter>(this->GetOwner()) : Cast<AWRCharacter>(VFX->GetSpawner());

	if (bNewActive)
	{
		if (VFX)
		{
			OnComponentBeginOverlap.AddUniqueDynamic(VFX, &AWRVFX::OnComponentBeginOverlap);
		}
#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(Character))
		{
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			if (Character)
			{
				OnComponentBeginOverlap.AddUniqueDynamic(Character, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
			}
		}
	}
	else
	{
		if (VFX)
		{
			OnComponentBeginOverlap.RemoveAll(VFX);
		}
#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(Character))
		{
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			if (Character)
			{
				OnComponentBeginOverlap.AddUniqueDynamic(Character, &AWRCharacter::OnBeginOverlapCapsuleAttacking);
			}
		}
	}
}

void UWRCapsuleCollisionComponent::RegistAttackingSignature()
{
	OnComponentBeginOverlap.AddUniqueDynamic(Cast<AWRCharacter>(GetOwner()), &AWRCharacter::OnBeginOverlapCapsuleAttacking);
}

void UWRCapsuleCollisionComponent::RegistHittingSignature()
{
	OnComponentBeginOverlap.AddUniqueDynamic(Cast<AWRCharacter>(GetOwner()), &AWRCharacter::OnBeginOverlapCapsuleHitting);
	OnComponentEndOverlap.AddUniqueDynamic(Cast<AWRCharacter>(GetOwner()), &AWRCharacter::OnEndOverlapHittingCapsule);
}

bool UWRCapsuleCollisionComponent::IsHookable()
{
	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
		return false;

	if (WRCharacterManager::IsValid() == false)
		return false;

	if (OwnerCharacter->IsHookableState() == false)
		return false;

	AWRCharacterMine* Mine = WRCharacterManager::Get()->GetMyCharacter();
	if (Mine == nullptr)
		return false;

	if (Mine->IsHookableRange(OwnerCharacter) == false)
		return false;

	switch (OwnerCharacter->GetCharacterType())
	{
	case EWRCharacterType::Player:
	{
		AWRCharacterPlayer* OwnerPlayer = Cast<AWRCharacterPlayer>(OwnerCharacter);
		if (OwnerCharacter == nullptr)
			return false;

		FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerPlayer->GetCharacterTableID());
		if (PlayerTable == nullptr)
			return false;

		if (PlayerTable->HookType != EWRHookType::None)
			return true;
	}
	break;
	case EWRCharacterType::ENPC:
	{
		AWRCharacterNPC* OwnerNPC = Cast<AWRCharacterNPC>(OwnerCharacter);
		if (OwnerCharacter == nullptr)
			return false;

		FWRNPCBaseTable* NPCBaseTable = OwnerNPC->GetNpcBaseTable();
		if (NPCBaseTable == nullptr)
			return false;

		if (NPCBaseTable->HookType != EWRHookType::None)
			return true;
	}
	break;
	}
	return false;
}

void UWRCapsuleCollisionComponent::SetHookTargeted(const bool IN InSet)
{
	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
		return;

	if (InSet)
	{
		if (OwnerCharacter->GetTargetedCollision() == this)
			return;

		OwnerCharacter->SetTargetedCollision(this);
		OwnerCharacter->SetHighLight(EWRObjectHighLight::Targeted);

		if (bReservedUIOpen)
			return;

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
			{
				UIHandle = InCombinedPanelData.UIHandle;

				// [ 2019-8-28 : kyu ] 컨텐츠 내용에 따라 창 크기가 유동적이어야 한다.
				InCombinedPanelData.WidgetComponentBase->SetDrawAtDesiredSize(true);
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(CapsuleAttachSocket);

				// 예약상태 해제
				bReservedUIOpen = false;

				UpdateCollisionWidget(InCombinedPanelData.UIPanel);
			}
		});
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::IOPopup, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, OwnerCharacter);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::IOPopup, OwnerCharacter, FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		bReservedUIOpen = true;
	}
	else
	{
		if (OwnerCharacter->GetTargetedCollision() == nullptr)
			return;

		OwnerCharacter->SetTargetedCollision(nullptr);
		OwnerCharacter->SetHighLight(EWRObjectHighLight::None);

		WRUIManager::Get()->CloseUI(UIHandle);
		bReservedUIOpen = false;
	}
}

void UWRCapsuleCollisionComponent::UpdateCollisionWidget(class UWRUIPanel* InPanel /* = nullptr */)
{
	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
		return;

	FWRHookInfo HookUIInfo;
	switch (OwnerCharacter->GetCharacterType())
	{
	case EWRCharacterType::Player:
	{
		AWRCharacterPlayer* OwnerPlayer = Cast<AWRCharacterPlayer>(OwnerCharacter);
		if (OwnerCharacter == nullptr)
			return;

		FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerPlayer->GetCharacterTableID());
		if (PlayerTable == nullptr)
			return;

		HookUIInfo.strTargetingIconPath = PlayerTable->HookKeyIcon;
		FString HookKeyDescription = PlayerTable->HookKeyDescription;
		HookUIInfo.strTargetingDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, HookKeyDescription).ToString();

		HookUIInfo.strActionIconPath = PlayerTable->HookKeyActionIcon;
		FString HookKeyActionDescription = PlayerTable->HookKeyActionDescription;
		HookUIInfo.strActionDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, HookKeyActionDescription).ToString();
	}
	break;
	case EWRCharacterType::ENPC:
	{
		AWRCharacterNPC* OwnerNPC = Cast<AWRCharacterNPC>(OwnerCharacter);
		if (OwnerCharacter == nullptr)
			return;

		FWRNPCBaseTable* NPCBaseTable = OwnerNPC->GetNpcBaseTable();
		if (NPCBaseTable == nullptr)
			return;

		HookUIInfo.strTargetingIconPath = NPCBaseTable->HookKeyIcon;
		HookUIInfo.strTargetingDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, NPCBaseTable->HookKeyDescription).ToString();
		HookUIInfo.strActionIconPath = NPCBaseTable->HookKeyActionIcon;
		HookUIInfo.strActionDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, NPCBaseTable->HookKeyActionDescription).ToString();
	}
	break;
	default:
	{
		return;
	}
	break;
	}

	UWRUIPanel_IOPopup* pWidget = nullptr;
	if (InPanel != nullptr)
	{
		pWidget = Cast<UWRUIPanel_IOPopup>(InPanel);
	}
	else
	{
		FWRCombinedPanelData* pData = WRUIManager::Get()->FindUIData(UIHandle);
		if (pData != nullptr)
		{
			pWidget = Cast<UWRUIPanel_IOPopup>(pData->UIPanel);
		}
	}

	if (pWidget == nullptr)
		return;

	bool bConnectingPart = WRCharacterManager::Get()->IsCableTargetPart(OwnerCharacter, HitCollisionType);
	pWidget->SetHookInfo(HookUIInfo, bConnectingPart);
}

class AWRCharacter* UWRCapsuleCollisionComponent::GetOwnerCharacter()
{
	return Cast<AWRCharacter>(GetOwner());
}
