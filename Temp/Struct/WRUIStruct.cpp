#include "WRUIStruct.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInventoryManager.h"
#include "Utility/WRHMDUtility.h"

const FVector	FWRWidgetInfo::DEFAULT_LOCATION(FVector::ZeroVector);
const FRotator	FWRWidgetInfo::DEFAULT_ROTATION(FRotator(0.f, 180.f, 0.f));
const FVector	FWRWidgetInfo::DEFAULT_SCALE(FVector::OneVector);
const FVector2D FWRWidgetInfo::DEFAULT_WIDGET_COMPONENT_PIVOT(FVector2D(0.5f, 0.5f));
const float		FWRWidgetInfo::DEFAULT_OFFSET_SCALE_FOR_FIXED_SIZE(0.f);

// FWRWidgetInfo::FWRWidgetInfo(EWRUIPanelKind _kind, EWRUIWidgetCreationBitFlag _flags /* = EWRUIWidgetCreationBitFlag::None */, const FWRUILoadCompletedDelegate* _delegate /* = nullptr */, AActor* _actor /* = nullptr */)
// {
// 	PanelKind = _kind;
// 	Flags = (BitFlag16)_flags;
// 	LoadCompleteDelegate = _delegate == nullptr ? nullptr : *_delegate;
// 	AttachActor = _actor;
// 
// 	if (WRUIManager::IsValid())
// 	{
// 		const FWRWidgetTable* FoundWidgetData = WRUIManager::Get()->GetWidgetCreationTable(_kind);
// 		if (FoundWidgetData != nullptr)
// 		{
// 			SetWidgetData(FoundWidgetData);
// 		}
// 	}
// }

FWRWidgetInfo::FWRWidgetInfo(EWRUIPanelKind IN InPanelKind, EWRUIWidgetCreationBitFlag IN InWidgetCreationBitFlag /*= EWRUIWidgetCreationBitFlag::None*/, const FWRUILoadCompletedDelegate* IN InDelegate /*= nullptr*/, AActor* ActorToAttach /*= nullptr*/)
{
	PanelKind = InPanelKind;

	// 만약 Flag 에 StereoLayer 나 WidgetComponent 모두 생성되는 Flag 가 없다면 WidgetComponent 로 생성한다.
// 	if ((InWidgetCreationBitFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer) == false && (InWidgetCreationBitFlag & EWRUIWidgetCreationBitFlag::CreateWidgetComponent) == false)
// 	{
// 		InWidgetCreationBitFlag |= EWRUIWidgetCreationBitFlag::CreateWidgetComponent;
// 	}

	InWidgetCreationBitFlag |= EWRUIWidgetCreationBitFlag::CreateWidgetComponent;

	Flags = InWidgetCreationBitFlag;
	LoadCompleteDelegate = InDelegate == nullptr ? nullptr : *InDelegate;
	AttachActor = ActorToAttach;

	if (WRUIManager::IsValid())
	{
		const FWRWidgetTable* FoundWidgetData = WRUIManager::Get()->GetWidgetCreationTable(InPanelKind);
		if (FoundWidgetData != nullptr)
		{
			SetWidgetData(FoundWidgetData);
		}
	}

	if (WRHMDUtility::IsConnected() == false || WRHMDUtility::IsDisplayEnable() == false)
	{
		if (Flags & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
		{
			Flags ^= EWRUIWidgetCreationBitFlag::CreateStereoLayer;
		}
	}
}

//FWRWidgetInfo::FWRWidgetInfo(EWRUIPanelKind IN InPanelKind, BitFlag16 IN InWidgetOptionFlags, const FWRUILoadCompletedDelegate* InDelegate /*= nullptr*/, AActor* IN InActorPositionToCreate /* = nullptr */)
//{
//	PanelKind = InPanelKind;
//	Flags = InWidgetOptionFlags;
//	LoadCompleteDelegate = InDelegate == nullptr ? nullptr : *InDelegate;
//	AttachActor = InActorPositionToCreate;
//
//	if (WRUIManager::IsValid())
//	{
//		const FWRWidgetTable* FoundWidgetData = WRUIManager::Get()->GetWidgetCreationTable(InPanelKind);
//		if (FoundWidgetData != nullptr)
//		{
//			SetWidgetData(FoundWidgetData);
//		}
//	}
//}

void FWRWidgetInfo::SetWidgetData(const FWRWidgetTable* IN InWidgetTable)
{
	if (InWidgetTable == nullptr)
		return;

	PanelKind = InWidgetTable->PanelKind;

	Location = InWidgetTable->Location;
	Rotation = InWidgetTable->Rotation;
	Scale = InWidgetTable->Scale;
	WidgetTable = InWidgetTable;
	Flags |= EWRUIWidgetCreationBitFlag(InWidgetTable->CreationTypeBits);

	WidgetComponentPivot = InWidgetTable->WidgetComponentPivot;
	OffsetScaleForFixedSize = InWidgetTable->OffsetScaleForFixedSize;
}

void FWRWidgetInfo::SetTransform(FTransform IN InTF)
{
	Location = InTF.GetLocation();
	Rotation = InTF.GetRotation().Rotator();
	Scale = InTF.GetScale3D();
}

const FTransform FWRWidgetInfo::GetTransform() const
{
	return FTransform(Rotation, Location, Scale);
}

EWRRegistrationSlotType FWRInventorySlotInfo::GetRegistrationSlotType() const
{
	EWRInventoryCategoryType CurrentCategoryType = WRInventoryManager::Get()->GetCurrentInventoryMainCategory();
	switch (CurrentCategoryType)
	{
	case EWRInventoryCategoryType::Equipment:
	{
		return EquipmentSlotType;
	}
	break;

	case EWRInventoryCategoryType::Costume:
	{
		return CostumeSlotType;
	}
	break;
	}
	return EWRRegistrationSlotType::None;
}

void FWRInventoryDiscardData::ClearData()
{
	DiscardQuantity = 1;
}
