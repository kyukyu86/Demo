// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WidgetComponent/WRWidgetComponentBase.h"
#include "Enum/EWRUIEnum.h"
#include "Enum/EWRCharacter.h"
#include "Define/WRUIDelegate.h"
#include "Table/Custom/WRWidgetTable.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Network/Share/Define/Define_Type.h"
#include "SlateBrush.h"

#include "WRUIStruct.generated.h"


namespace EWRDebugUIBitFlag
{
	enum en : uint16
	{
		None = 0x0000,
		Actor = 0x0001,
		Env = 0x0002,
		EnvElevator = 0x0004,
		Tardis = 0x0008,
		Floating = 0x0010,
		Player = 0x0020,
		IO = 0x0040,

		All = 0x8000,
	};
	static uint16 ConvertToUint16(FString str)
	{
		if (str.Equals("None"))				return EWRDebugUIBitFlag::None;
		else if (str.Equals("Actor"))		return EWRDebugUIBitFlag::Actor;
		else if (str.Equals("Env"))			return EWRDebugUIBitFlag::Env;
		else if (str.Equals("EnvElevator"))	return EWRDebugUIBitFlag::EnvElevator;
		else if (str.Equals("Tardis"))		return EWRDebugUIBitFlag::Tardis;
		else if (str.Equals("Floating"))	return EWRDebugUIBitFlag::Floating;
		else if (str.Equals("Player"))		return EWRDebugUIBitFlag::Player;
		else if (str.Equals("IO"))			return EWRDebugUIBitFlag::IO;

		else if (str.Equals("All"))			return EWRDebugUIBitFlag::All;
		else
		{
			return EWRDebugUIBitFlag::None;
		}
	}
}

struct FWRDebugUIData
{
	EWRDebugUIBitFlag::en Flag = EWRDebugUIBitFlag::None;
	class AActor* Actor = nullptr;
	class AActor* CompareActor = nullptr;
	float fDistance = 0.f;
	FString Contents;
	TArray<FString> ArrayContents;
	FVector Offset = FVector::ZeroVector;
};

struct FWRWidgetInfo
{
	FWRWidgetInfo(EWRUIPanelKind IN InPanelKind, EWRUIWidgetCreationBitFlag IN InWidgetCreationBitFlag = EWRUIWidgetCreationBitFlag::None, const FWRUILoadCompletedDelegate* IN InDelegate = nullptr, AActor* ActorToAttach = nullptr);
	//FWRWidgetInfo(EWRUIPanelKind IN InPanelKind, BitFlag16 IN InWidgetOptionFlags = EWRUIWidgetCreationBitFlag::CreateStereoLayer, const FWRUILoadCompletedDelegate* IN InDelegate = nullptr, AActor* IN InActorPositionToCreate = nullptr);
	//BitFlag16 = (BitFlag16) EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::AttachToCamera;

	static const FVector DEFAULT_LOCATION;
	static const FRotator DEFAULT_ROTATION;
	static const FVector DEFAULT_SCALE;
	static const FVector2D DEFAULT_WIDGET_COMPONENT_PIVOT;
	static const float DEFAULT_OFFSET_SCALE_FOR_FIXED_SIZE;

public:
	AActor* AttachActor = nullptr;

	// Default Values
	FVector Location = DEFAULT_LOCATION;
	FRotator Rotation = DEFAULT_ROTATION;
	FVector Scale = DEFAULT_SCALE;
	FVector2D WidgetComponentPivot = DEFAULT_WIDGET_COMPONENT_PIVOT;
	float OffsetScaleForFixedSize = DEFAULT_OFFSET_SCALE_FOR_FIXED_SIZE;
	const FWRWidgetTable* WidgetTable = nullptr;			// [ 2020-7-22 : vuvle ] StereoLayer 의 추가로 인해 많은정보들을 가지고 있어야 하는데 일단 Widget Table 을 추가함. 추후 모든 Data 를 테이블에서 쓸지 고민해봐야함.

	bool SaveCreatingState = false;

protected:
	EWRUIPanelKind PanelKind = EWRUIPanelKind::None;
	FWRUILoadCompletedDelegate LoadCompleteDelegate = nullptr;
	EWRUIWidgetCreationBitFlag Flags = EWRUIWidgetCreationBitFlag::None;

public:
	FORCEINLINE EWRUIPanelKind GetPanelKind() const { return PanelKind; }
	FORCEINLINE FWRUILoadCompletedDelegate GetLoadCompleteDelegate() const { return LoadCompleteDelegate; }
	FORCEINLINE EWRUIWidgetCreationBitFlag GetFlags() const { return Flags; }
	void SetWidgetData(const FWRWidgetTable* IN InWidgetTable);
	void SetTransform(FTransform IN InTF);
	const FTransform GetTransform() const;
};

USTRUCT(BlueprintType)
struct FWRInventorySlotInfo
{
	GENERATED_BODY()

public:
	int32 SlotIndex = 0;

	bool bEmptySlot = true;
	ISN_t ISN = INVALID_ISN;
	WRTableID TID = INVALID_TID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWRInventorySlotGroup SlotGroupType = EWRInventorySlotGroup::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SlotGroupType == EWRInventorySlotGroup::EquipList"))
		EWRRegistrationSlotType EquipmentSlotType = EWRRegistrationSlotType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SlotGroupType == EWRInventorySlotGroup::EquipList", MultiLine = "true"))
		FText EquipmentSlotName = FText::FromString(TEXT(""));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SlotGroupType == EWRInventorySlotGroup::EquipList"))
		EWRRegistrationSlotType CostumeSlotType = EWRRegistrationSlotType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SlotGroupType == EWRInventorySlotGroup::EquipList", MultiLine = "true"))
		FText CostuneSlotName = FText::FromString(TEXT(""));

public:
	EWRRegistrationSlotType GetRegistrationSlotType() const;	
};

struct FWRInventoryDiscardData
{
	int32 DiscardQuantity = 1;

	void ClearData();
};

struct FWRPopupQuantityInfo
{
	FOnPopupQuantityChangeDelegate Delegate_QuantityChanged;
	ITEM_COUNT_t MaxQuantity = 999999;
};

struct FWRPopupInfo
{
	FString Title;
	FString Contents;
	FString FirstBtnText;
	FString SecondBtnText;
	EWRInput FirstBtnKey = EWRInput::Circle;
	EWRInput SecondBtnKey = EWRInput::Cross;
	EPopupButtonFlag eButtonFlag = EPopupButtonFlag::None;
	BitFlag8 eOptionFlag = (BitFlag8)EPopupOptionFlag::None;
	FOnPopupButtonDelegate Delegate_First;
	FOnPopupButtonDelegate Delegate_Second;
	// + Quantity
	FWRPopupQuantityInfo QuantityInfo;

	bool bFocusReleaseWhenClosed = true;
};

USTRUCT(BlueprintType)
struct FWRChartSlateBrush : public FSlateBrush
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Brush)
		float LineTickness = 5.f;
};

struct FWRFluctuationateInfo
{
	class UWRTextBlock* Price;
	class UWRTextBlock* Fluctuationate;
	class UImage* UpDownTriangle;
	float fFluctuationate = 0.f;

	
};