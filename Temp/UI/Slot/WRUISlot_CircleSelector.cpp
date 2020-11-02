// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_CircleSelector.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRUIManager.h"
#include "Enum/EWRUIEnum.h"
#include "CanvasPanelSlot.h"
#include "WRUISlot_CircleSelectorElement.h"
#include "Utility/WRUIUtility.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UnrealMathUtility.h"
#include "Manager/WRInputSystemManager.h"
#include "UMG/Public/Blueprint/WidgetLayoutLibrary.h"

#define DEF_CIRCLE_SELECTOR_DEFAULT -1

void UWRUISlot_CircleSelector::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		if (Image_OnlyVisibleCircle)
		{
			Image_OnlyVisibleCircle->SetBrush(OnlyVisibleCircleBrush);
		}

		UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_OnlyVisibleCircle);
		if (CanvasPanelSlot)
		{
			CanvasPanelSlot->SetSize(FVector2D(CircleRadius * 2, CircleRadius * 2));
		}

		CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_SelectCircle);
		if (CanvasPanelSlot)
		{
			CanvasPanelSlot->SetSize(FVector2D(SelectCircleRadius * 2, SelectCircleRadius * 2));
		}

		Mat_Circle = WRUIUtility::GetDynamicMaterial(Image_SelectCircle);
		if (Mat_Circle)
		{
			Mat_Circle->SetScalarParameterValue("InnerCircle", float(InnerCircleRadius) / CircleRadius);
		}
	}
}

void UWRUISlot_CircleSelector::NativeConstruct()
{
	Super::NativeConstruct();

	SetChildNum(NumOfDivision);

	
	Image_OnlyVisibleCircle->SetBrush(OnlyVisibleCircleBrush);
	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_OnlyVisibleCircle);
	if (CanvasPanelSlot)
	{
		CanvasPanelSlot->SetSize(FVector2D(CircleRadius * 2, CircleRadius * 2));
	}

	CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_SelectCircle);
	if (CanvasPanelSlot)
	{
		CanvasPanelSlot->SetSize(FVector2D(SelectCircleRadius * 2, SelectCircleRadius * 2));
	}
	

	Mat_Circle = WRUIUtility::GetDynamicMaterial(Image_SelectCircle);
	if (Mat_Circle)
	{
		Mat_Circle->SetScalarParameterValue("InnerCircle", float(InnerCircleRadius) / CircleRadius);
	}
}

void UWRUISlot_CircleSelector::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/*static float FlowTime = 0;

	if (AsyncKeyList.Num() == 0)
	{
		FlowTime += InDeltaTime;
		if (FlowTime > 0.3f)
		{
			FlowTime = 0;
			FocusElement((FocusedSlotIndex + 1) % NumOfDivision);
		}
	}*/

	float X = 0, Y = 0;
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if(PlayerController)
		PlayerController->GetInputAnalogStickState(EControllerAnalogStick::CAS_LeftStick, X, Y);

	X = WRInputSystemManager::Get()->GetLStickScale().X;
	Y = WRInputSystemManager::Get()->GetLStickScale().Y;

	if (X == 0 && Y == 0)
	{
		//FocusElement(DEF_CIRCLE_SELECTOR_DEFAULT);
	}
	else
	{
		FVector DirVec(Y,X,0);
		FRotator Rotation = DirVec.Rotation();
		float Degree = Rotation.Yaw;
		if (Degree < 0)
			Degree += 360;
		FocusElementByDegree(Degree);
	}
}

void UWRUISlot_CircleSelector::NativeDestruct()
{
	if (AsyncKeyList.Num() > 0)
	{
		for (auto Iter : AsyncKeyList)
		{
			WRUIManager::Get()->CancelUIAsync(*Iter);
		}
		AsyncKeyList.Empty();
	}

	Super::NativeDestruct();
}

void UWRUISlot_CircleSelector::SetChildNum(const int32 IN InNum)
{
	if (ElementSlotKind == EWRUISlotKind::None)
		return;

	if (AsyncKeyList.Num() > 0)
	{
		for (auto Iter : AsyncKeyList)
		{
			WRUIManager::Get()->CancelUIAsync(*Iter);
		}
		AsyncKeyList.Empty();
	}

	CanvasPanel_ChildList->ClearChildren();

	for (int32 i = 0; i < InNum; ++i)
	{
		CreateSlotAync(i);
	}
}

void UWRUISlot_CircleSelector::CreateSlotAync(const int32 IN InIndex)
{
	FWRSlotUILoadCompletedDelegate Delegate = FWRSlotUILoadCompletedDelegate::CreateLambda([=](UWRUISlot* IN InCreatedSlot, const FString& IN InAsyncKey)
	{
		AsyncKeyList.RemoveNode(InAsyncKey);

		if (InCreatedSlot)
		{
			UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_ChildList->AddChildToCanvas(InCreatedSlot);
			if (CanvasPanelSlot)
			{
				CanvasPanelSlot->SetAnchors(FAnchors(0.5f));
				CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				CanvasPanelSlot->SetAutoSize(true);
			}

			SetElementPosition(InCreatedSlot, InIndex);
		}

		if (AsyncKeyList.Num() == 0)
			OnCreateSlotCompleted();
	});

	FString AsyncKey = WRUIManager::Get()->CreateUISlotAsync(ElementSlotKind, Delegate);
	if (AsyncKey.IsEmpty() == false)
		AsyncKeyList.AddTail(AsyncKey);
}


void UWRUISlot_CircleSelector::OnCreateSlotCompleted()
{
	FocusElement(0);
}

void UWRUISlot_CircleSelector::OnKeyEventReceived(const EWRInput IN InInput)
{

}

void UWRUISlot_CircleSelector::FocusElement(const int32 IN InIndex)
{
	if (FocusedSlotIndex == InIndex)
		return;

	if (InIndex > NumOfDivision)
		return;

	if (NumOfDivision <= 0)
		return;

	FocusedSlotIndex = InIndex;
	if (FocusedSlotIndex == DEF_CIRCLE_SELECTOR_DEFAULT)
	{
		Mat_Circle->SetScalarParameterValue("Min", 0);
		Mat_Circle->SetScalarParameterValue("Max", 0);
		return;
	}

	float PublicDegree = 360 / NumOfDivision;
	float PublicRadian = FMath::DegreesToRadians(PublicDegree);
	float Degree = PublicDegree * InIndex;
	float Radian = FMath::DegreesToRadians(Degree);

	float Min = Radian - PublicRadian * 0.5f;
	float Max = Radian + PublicRadian * 0.5f;

	if (Min < 0)
		Min += 2 * PI;

	Mat_Circle->SetScalarParameterValue("Min", Min);
	Mat_Circle->SetScalarParameterValue("Max", Max);

	
}

void UWRUISlot_CircleSelector::FocusElementByDegree(const float IN InDegree)
{
	if (NumOfDivision <= 0)
		return;

	float PublicDegree = 360 / NumOfDivision;
	float ConvertDegree = InDegree + PublicDegree * 0.5f;

	int32 Index = int32(ConvertDegree / PublicDegree) % NumOfDivision;
	FocusElement(Index);
}

void UWRUISlot_CircleSelector::SetElementPosition(UWRUISlot* IN InSlot, const int32 IN InIndex)
{
	if (InSlot == nullptr)
		return;

	if (NumOfDivision <= 0)
		return;

	float PublicDegree = 360 / NumOfDivision;
	float Degree = PublicDegree * InIndex;
	float Radian = FMath::DegreesToRadians(Degree);
	float Radius = (CircleRadius + InnerCircleRadius) * 0.5f;
	float PosX = Radius * FMath::Sin(Radian);
	float PosY = -Radius * FMath::Cos(Radian);

	InSlot->SetRenderTranslation(FVector2D(PosX, PosY)); // Áß¾Ó
}
