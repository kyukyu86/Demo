// Fill out your copyright notice in the Description page of Project Settings.


#include "WRSlideList.h"
#include "WRSlideElement.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "Components/Overlay.h"
#include "Blueprint/WidgetTree.h"
#include "Components/OverlaySlot.h"
#include "Manager/WRUIManager.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Define/WRUIDelegate.h"


//#define DEF_SLIDE_LIST_DEBUG

void UWRSlideList::NativePreConstruct()
{
	Super::NativePreConstruct();
		
#ifdef WITH_EDITOR
	if (IsDesignTime())
	{
		SetDataSize(TestDataSize);
		Setup();
	}
#else//WITH_EDITOR
#endif//WITH_EDITOR
}

void UWRSlideList::NativeConstruct()
{
	Super::NativeConstruct();

	SetFocusableInputMode(true, this);	
		
	// [ 2020-9-23 : kyu ] DesiredSize문제로 Construct가 아닌 Tick에서 하나 이상의 프리패스가 지난 후에 정상화 되기때문에 Tick에서 한번만 처리해준다.
	GetCachedWidget()->SlatePrepass();
	Setup();
}

void UWRSlideList::NativeDestruct()
{
	SetFocusableInputMode(false);
	ReleaseElementList();

	Super::NativeDestruct();
}

void UWRSlideList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// [ 2020-9-23 : kyu ] DesiredSize문제로 Construct가 아닌 Tick에서 하나 이상의 프리패스가 지난 후에 정상화 되기때문에 Tick에서 한번만 처리해준다.
	//Setup();

	UpdateMove(InDeltaTime);
}

FReply UWRSlideList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Handled();

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::RStickLeft:
		{
			OnMove(EWRSlideMoveType::Left);
			return FReply::Handled();
		}
		break;
		case EWRInput::RStickRight:
		{
			OnMove(EWRSlideMoveType::Right);
			return FReply::Handled();
		}
		break;
		}
	}

	return FReply::Unhandled();
}

void UWRSlideList::Setup()
{
	if (ElementWidgetClass == nullptr)
		return;

	if (IsSetted)
		return;

	if (IsMadeList == false)
		MakeElementList();

	if (IsMadeList == false)
		return;

	if (ElementList.IsValidIndex(0) == false || ElementList[0].SlideElement->IsValidLowLevel() == false)
		return;

#ifdef WITH_EDITOR
	if (IsDesignTime())
	{
		if (TestElementDesiredSize.IsZero())
			return;
	}
	else
	{
		if (ElementList[0].SlideElement->GetDesiredSize().IsZero())
			return;
	}
#else//WITH_EDITOR
	if (ElementList[0].SlideElement->GetDesiredSize().IsZero())
		return;
#endif//WITH_EDITOR	

	SetupTransitionList();
	SetupElementList();

	UpdateElementList(true);

	IsSetted = true;
}

void UWRSlideList::MakeElementList()
{
	if (ElementList.Num() != ElementCount)
	{
		// Make List
		for (int32 i = 0; i < ElementCount; ++i)
		{
			FWRSlideElementWidget NewElement;			
			ElementList.Add(NewElement);
		}
	}	

	// for Async
	bool bCreateCompletedAll = true;
	for (auto& ElementWidget : ElementList)
	{
		if (ElementWidget.CreateElement(this, ElementWidgetClass) == false)
		{
			checkf(false, TEXT("Creat Failure"));
			return;
		}

		// Wait for Async Load Completed
		if (ElementWidget.IsCreateCompleted())
		{
			ElementWidget.SlideElement->SetParent(this);
			CanvasPanelMain->AddChildToCanvas(ElementWidget.Overlay);
		}
		else
		{
			bCreateCompletedAll = false;
		}
	}

	if (bCreateCompletedAll)
	{
		IsMadeList = true;
	}
}

void UWRSlideList::ReleaseElementList()
{
	for (auto& ElementWidget : ElementList)
	{
		ElementWidget.ReleaseElement();
	}
	ElementList.Empty();
}

void UWRSlideList::SetupTransitionList()
{
	if (ElementList.IsValidIndex(0) == false || ElementList[0].SlideElement == nullptr)
		return;

	UWRSlideElement* ElementWidget = ElementList[0].SlideElement;
	FVector2D ChildSize = ElementWidget->GetDesiredSize();
#ifdef WITH_EDITOR
	if (IsDesignTime())
		ChildSize = TestElementDesiredSize;
#else//WITH_EDITOR
#endif//WITH_EDITOR

	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	if (CanvasPanelSlot == nullptr)
		return;

	FVector2D ParentSize = CanvasPanelSlot->GetSize();
	FVector2D ParentMiddlePosition = ParentSize / 2.0f;

	for (int32 i = 0; i < ElementCount; ++i)
	{
		FVector2D ResultPosition = FVector2D::ZeroVector;
		if (i == 0)
		{
			// Middle
			FVector2D ChildPosition;
			ChildPosition.X = ChildSize.X - (ChildSize / MainCalcScale).X;
			ChildPosition.Y = ChildSize.Y - (ChildSize / MainCalcScale).Y;
			ResultPosition.X = ParentMiddlePosition.X - ChildPosition.X;
			ResultPosition.Y = ParentMiddlePosition.Y - ChildPosition.Y + MainOffset;
			TranslationList.Add(ResultPosition);
		}
		else if (i % 2 == 1)
		{
			// Left
			ResultPosition.X = ParentMiddlePosition.X - (ChildSize.X / 2.f);
			float AddSpace = ElementSpace + ChildSize.X;
			AddSpace *= (i % 2) + (i / 2);
			ResultPosition.X -= AddSpace;
			ResultPosition.Y = ParentMiddlePosition.Y - (ChildSize.Y / 2.f);
			TranslationList.Insert(ResultPosition, 0);
		}
		else
		{
			// Right
			ResultPosition.X = ParentMiddlePosition.X + (ChildSize.X / 2.f) + ElementSpace;
			float AddSpace = ElementSpace + ChildSize.X;
			AddSpace *= ((i / 2) - 1);
			ResultPosition.X += AddSpace;
			ResultPosition.Y = ParentMiddlePosition.Y - (ChildSize.Y / 2.f);
			TranslationList.Add(ResultPosition);
		}
	}
}

void UWRSlideList::SetupElementList()
{
	int32 Index = 0;
	for (auto& ElementWidget : ElementList)
	{		
		UOverlay* Overlay = ElementWidget.Overlay;
		UWRSlideElement* SlideElement = ElementWidget.SlideElement;

		// Data
		SlideElement->SetIndex(Index);
		SlideElement->SetTranslationIndex(Index);

		// Translation & Visibility
		FVector2D SlotTranslation = GetTranslation(Index);
		Overlay->SetRenderTranslation(SlotTranslation);
		Overlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		// Align
		UOverlaySlot* ElementOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(SlideElement);
		ElementOverlaySlot->SetVerticalAlignment(VAlign_Center);
		ElementOverlaySlot->SetHorizontalAlignment(HAlign_Center);

		UCanvasPanelSlot* OverlayCanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Overlay);
		OverlayCanvasPanelSlot->SetPosition(FVector2D::ZeroVector);
		OverlayCanvasPanelSlot->SetSize(SlideElement->GetDesiredSize());
#ifdef WITH_EDITOR
		if (IsDesignTime())
			OverlayCanvasPanelSlot->SetSize(TestElementDesiredSize);
#else//WITH_EDITOR
#endif//WITH_EDITOR

		if (Index == GetMainTranslationIndex())	// 메인은 메인설정
		{
			SlideElement->SetMain(true);
			SlideElement->SetRenderScale(FVector2D(MainScale, MainScale));
			OverlayCanvasPanelSlot->SetZOrder(1);
		}
		else
		{
			SlideElement->SetMain(false);
		}

		++Index;
	}
}

void UWRSlideList::UpdateElement(class UWRSlideElement* IN InElement)
{
	int32 DataIndex = StartingDataIndex + InElement->GetTranslationIndex();
	if (DataIndex < 0 || DataIndex > DataSize - 1)
	{
		InElement->SetEmpty();
	}
	else
	{
#ifdef WITH_EDITOR
		// [ 2020-9-24 : kyu ] DesignTime에는 TestData를 얻어오지 못해 Default만 출력될것
		if (IsDesignTime())
			return;
#else//WITH_EDITOR
#endif//WITH_EDITOR

		InElement->SetData(DataIndex);

		OnSlideSideChangedDelegate.ExecuteIfBound(DataIndex);
	}
}

void UWRSlideList::OnMove(const EWRSlideMoveType IN InMoveType)
{
	if (IsSetted == false)
		return;

	if (IsMoving)
	{		
		if(MoveType == InMoveType)
		{
			if (InMoveType == EWRSlideMoveType::Left && MainTranslationDataIndex - MoveStep <= 0)
			{
				TestDebuggingLog("Input(Moving) : Last Left");
				return;
			}

			if (InMoveType == EWRSlideMoveType::Right && MainTranslationDataIndex + MoveStep >= DataSize - 1)
			{
				TestDebuggingLog("Input(Moving) : Last Right");
				return;
			}

			TestDebuggingLog("Input(Moving) : Add Step");

			IsRevertTranslation = false;
			++MoveStep;
			DstMoveTime = RepeatMoveTime;
		}
		else
		{
			TestDebuggingLog("Input(Moving) : Revert");

			IsRevertTranslation = true;
			MoveStep = 1;
			DstMoveTime = NormalMoveTime;
		}
	}
	else
	{
		if (InMoveType == EWRSlideMoveType::Left && MainTranslationDataIndex <= 0)
		{
			TestDebuggingLog("Input(Not Moving) : Last Left");
			return;
		}

		if (InMoveType == EWRSlideMoveType::Right && MainTranslationDataIndex >= DataSize - 1)
		{
			TestDebuggingLog("Input(Not Moving) : Last Right");
			return;
		}

		TestDebuggingLog("Input(Not Moving) : Normal Step");

		IsRevertTranslation = false;
		MoveStep = 1;
		DstMoveTime = NormalMoveTime;
	}


	if (IsRevertTranslation)
	{
		NextMainTranslationIndex = GetMainTranslationIndex();
		AccumulateMoveTime = 0.f;
	}
	else
	{
		NextMainTranslationIndex = InMoveType == EWRSlideMoveType::Left ? GetMainTranslationIndex() - 1 : GetMainTranslationIndex() + 1;	// 왼쪽이 눌리면 왼쪽이 중앙으로 와야하므로 왼쪽이 다음 순번
//	 	if (NextMainTranslationIndex <= 0 || NextMainTranslationIndex >= TranslationList.Num())
// 			return;
	}


	//AccumulateMoveTime = 0.f;
	IsMoving = true;
	MoveType = InMoveType;
}

void UWRSlideList::UpdateMove(const float IN InDeltaTime)
{
	if (IsSetted == false)
		return;

	if (IsMoving == false)
		return;

// 	if (IsRevertTranslation == false)
// 	{
// 		if (MoveType == EWRSlideMoveType::Left && MainTranslationDataIndex <= 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Update : Last Left"));
// 			ReleaseMove();
// 			return;
// 		}
// 
// 		if (MoveType == EWRSlideMoveType::Right && MainTranslationDataIndex >= DataSize - 1)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Update : Last Right"));
// 			ReleaseMove();
// 			return;
// 		}
// 	}	

	AccumulateMoveTime += InDeltaTime;	
	float GlobalRate = AccumulateMoveTime / DstMoveTime;
	if (GlobalRate >= 1.f)
	{
		GlobalRate = 1.f;
	}

	for (auto& ElementWidget : ElementList)
	{
		UOverlay* Overlay = ElementWidget.Overlay;
		UWRSlideElement* Element = ElementWidget.SlideElement;

		// Translation
		int32 CurrentTranslationIndex = Element->GetTranslationIndex();
		int32 NextTranslationIndex = CurrentTranslationIndex;
		if (IsRevertTranslation == false)
		{
			TestDebuggingLog("Normal Moving");
			NextTranslationIndex = MoveType == EWRSlideMoveType::Left ? Element->GetTranslationIndex() + 1 : Element->GetTranslationIndex() - 1;	// 왼쪽이면 오른쪽으로 이동
		}
		else
		{
			TestDebuggingLog("Revert Moving");
		}

		if ((MoveType == EWRSlideMoveType::Left || IsRevertTranslation) && CurrentTranslationIndex == ElementCount - 1)
		{
			NextTranslationIndex = 0;	// 왼쪽입력시 슬라이드는 오른쪽으로 이동한다. 제일 오른쪽은 제일 왼쪽으로 이동
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			Overlay->SetRenderTranslation(DstTranslation);
		}
		else if ((MoveType == EWRSlideMoveType::Right || IsRevertTranslation) && CurrentTranslationIndex == 0)
		{
			NextTranslationIndex = TranslationList.Num() - 1;	// 오른쪽입력시 슬라이드는 왼쪽으로 이동한다. 제일 왼쪽은 제일 오른쪽으로 이동
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			Overlay->SetRenderTranslation(DstTranslation);
		}
		else
		{
			FVector2D SrcTranslation = Overlay->RenderTransform.Translation;//GetTranslation(CurrentTranslationIndex);
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			FVector2D LerpTranslation;
			LerpTranslation.X = UKismetMathLibrary::Lerp(SrcTranslation.X, DstTranslation.X, GlobalRate);
			LerpTranslation.Y = UKismetMathLibrary::Lerp(SrcTranslation.Y, DstTranslation.Y, GlobalRate);
			Overlay->SetRenderTranslation(LerpTranslation);
		}		

		// Scale
		float ScaleRate = GlobalRate;
		if(NextTranslationIndex == GetMainTranslationIndex())
		{
			if (MainScaleCurve != nullptr)
			{
				ScaleRate = MainScaleCurve->GetFloatValue(GlobalRate);
			}

			FVector2D LerpScale;
			LerpScale.X = LerpScale.Y = UKismetMathLibrary::Lerp(Element->RenderTransform.Scale.X, MainScale, ScaleRate);
			Element->SetRenderScale(LerpScale);

			if (ScaleRate >= ChangeMainTime)
			{
				Element->SetMain(true);
				UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Overlay);
				CanvasPanelSlot->SetZOrder(1);
			}
		}
		else
		{
			if (Element->RenderTransform.Scale.X != 1.f)
			{
				FVector2D LerpScale;
				LerpScale.X = LerpScale.Y = UKismetMathLibrary::Lerp(Element->RenderTransform.Scale.X, 1.f, ScaleRate);
				Element->SetRenderScale(LerpScale);
			}
			if (ScaleRate >= ChangeMainTime)
			{
				Element->SetMain(false);
				UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Overlay);
				CanvasPanelSlot->SetZOrder(0);
			}
		}
	}

	// Finish
	if (GlobalRate >= 1.f)
	{
		if (IsRevertTranslation == false)
		{
			TestDebuggingLog("Check");

			if (MoveType == EWRSlideMoveType::Left)
			{
				--MainTranslationDataIndex;
				StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);
			}
			else if(MoveType == EWRSlideMoveType::Right)
			{
				++MainTranslationDataIndex;
				StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);
			}
		}		

		for (auto& ElementWidget : ElementList)
		{
			bool bSwap = false;
			UOverlay* Overlay = ElementWidget.Overlay;
			UWRSlideElement* Element = ElementWidget.SlideElement;

			int32 CurrentTranslationIndex = Element->GetTranslationIndex();
			int32 ModifyTranslationIndex = CurrentTranslationIndex;
			if (IsRevertTranslation == false)
			{
				ModifyTranslationIndex = MoveType == EWRSlideMoveType::Left ? Element->GetTranslationIndex() + 1 : Element->GetTranslationIndex() - 1;

				if ((MoveType == EWRSlideMoveType::Left && CurrentTranslationIndex == ElementCount - 1))
				{
					ModifyTranslationIndex = 0;	// 왼쪽입력시 슬라이드는 오른쪽으로 이동한다. 제일 오른쪽은 제일 왼쪽으로 이동
					bSwap = true;
				}
				else if (MoveType == EWRSlideMoveType::Right && CurrentTranslationIndex == 0)
				{
					ModifyTranslationIndex = TranslationList.Num() - 1;	// 오른쪽입력시 슬라이드는 왼쪽으로 이동한다. 제일 왼쪽은 제일 오른쪽으로 이동
					bSwap = true;
				}
			}

			if (ModifyTranslationIndex == GetMainTranslationIndex())
			{
				Element->SetMain(true);
			}
			else
			{
				Element->SetMain(false);
			}

			Element->SetTranslationIndex(ModifyTranslationIndex);
			FVector2D DstTranslation = GetTranslation(ModifyTranslationIndex);
			Overlay->SetRenderTranslation(DstTranslation);

			if (bSwap)
			{
				UpdateElement(ElementWidget.SlideElement);
			}
		}		

		--MoveStep;
		if (MoveStep <= 0)
		{
			TestDebuggingLog("Auto : Release");
			ReleaseMove();
		}		
		else
		{
			if (MoveType == EWRSlideMoveType::Left && MainTranslationDataIndex <= 0)
			{
				TestDebuggingLog("Auto : Last Left. Release");
				ReleaseMove();
				return;
			}

			if (MoveType == EWRSlideMoveType::Right && MainTranslationDataIndex >= DataSize - 1)
			{
				TestDebuggingLog("Auto : Last Right. Release");
				ReleaseMove();
				return;
			}

			TestDebuggingLog("Auto : Next Move");

			AccumulateMoveTime = 0.f;
			OnSlideMainChangedDelegate.ExecuteIfBound(MainTranslationDataIndex);
		}
	}
}

void UWRSlideList::ReleaseMove()
{
	AccumulateMoveTime = 0.f;
	IsMoving = false;
	MoveStep = 0;
	MoveType = EWRSlideMoveType::None;
	IsRevertTranslation = false;
}

void UWRSlideList::UpdateElementList(const bool IN InInit)
{
	if (InInit)
	{
		switch (SlideStartType)
		{
		case EWRSlideStartType::Head:
		{
			MainTranslationDataIndex = 0;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 중앙->오른쪽으로 데이터 삽입
		}
		break;
		case EWRSlideStartType::Middle:
		{
			MainTranslationDataIndex = DataSize / 2;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 왼쪽<-중앙으로 인덱스-1씩 데이터 삽입
			// + 중앙->오른쪽으로 인덱스+1씩 데이터 삽입
		}
		break;
		case EWRSlideStartType::Tail:
		{
			MainTranslationDataIndex = DataSize - 1;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 왼쪽<-중앙으로 인덱스 -1씩 데이터 삽입
		}
		break;
		}
	}	

	for (auto& ElementWidget : ElementList)
	{
		UpdateElement(ElementWidget.SlideElement);
	}
}

void UWRSlideList::ChangeElementListByDataIndex(const int32 IN InDataIndex)
{
	MainTranslationDataIndex = InDataIndex;
	StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

	UpdateElementList(false);
}

void UWRSlideList::TestDebuggingLog(FString IN InLog)
{
#ifdef DEF_SLIDE_LIST_DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, InLog);
#else//DEF_SLIDE_LIST_DEBUG
#endif//DEF_SLIDE_LIST_DEBUG
}

UWRSlideElement* UWRSlideList::GetElement(const int32 IN InTranslationIndex)
{
	for (auto& ElementWidget : ElementList)
	{
		if (ElementWidget.SlideElement->GetTranslationIndex() == InTranslationIndex)
			return ElementWidget.SlideElement;
	}
	return nullptr;
}

bool FWRSlideElementWidget::CreateElement(UUserWidget* InOwner, TSubclassOf<class UWRSlideElement> InElementWidget)
{
	if (bIsCreateCompleted)
		return true;

	if (strAsyncKey.IsEmpty() == false)
		return true;

	UWidgetTree* WidgetTree = InOwner->WidgetTree;

	Overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT(""));
	if (Overlay == nullptr)
		return false;

#ifdef WITH_EDITOR
	if (InOwner->IsDesignTime())
	{
		SlideElement = WidgetTree->ConstructWidget<UWRSlideElement>(InElementWidget.Get(), TEXT(""));
		if (SlideElement)
		{
			bIsCreateCompleted = true;
			Overlay->SetVisibility(ESlateVisibility::Hidden);
			Overlay->AddChildToOverlay(SlideElement);
			return true;
		}
	}
	else
	{
		FString WidgetPath = InElementWidget->GetPathName();	// 풀경로/이름.이름_C
		strAsyncKey = WRUIManager::Get()->CreateUIFullPathASync(WidgetPath, FWRSlotUILoadCompletedDelegate::CreateRaw(this, &FWRSlideElementWidget::OnCreateCompleted));
		if (strAsyncKey.IsEmpty() == false)
			return true;
	}
#else//WITH_EDITOR
	FString WidgetPath = InElementWidget->GetPathName();	// 풀경로/이름.이름_C
	strAsyncKey = WRUIManager::Get()->CreateUIFullPathASync(WidgetPath, FWRSlotUILoadCompletedDelegate::CreateRaw(this, &FWRSlideElementWidget::OnCreateCompleted));
	if (strAsyncKey.IsEmpty() == false)
		return true;
#endif//WITH_EDITOR

	return false;
}

void FWRSlideElementWidget::OnCreateCompleted(UWRUISlot* IN InCreatedSlot, FString InAsyncKey)
{
	if (InCreatedSlot)
	{
		SlideElement = Cast<UWRSlideElement>(InCreatedSlot);
		if (SlideElement)
		{
			strAsyncKey.Empty();
			bIsCreateCompleted = true;
			Overlay->SetVisibility(ESlateVisibility::Hidden);
			Overlay->AddChildToOverlay(SlideElement);
		}
	}
}

void FWRSlideElementWidget::ReleaseElement()
{
	if (strAsyncKey.IsEmpty())
		return;

	WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(strAsyncKey);
}
