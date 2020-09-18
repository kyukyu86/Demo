// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_SlideList.h"
#include "DMUISlot_SlideElement.h"

#include "../../Manager/DMUIManager.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "DMUISlot_CustomSlideElement.h"


void UDMUISlot_SlideList::NativePreConstruct()
{
	Super::NativePreConstruct();
		
}

void UDMUISlot_SlideList::NativeConstruct()
{
	Super::NativeConstruct();

	SetFocusableInputMode(true, this);	

	// Test
	SetupElement("Slot_CustomSlideElement");
}

void UDMUISlot_SlideList::NativeDestruct()
{
	Super::NativeDestruct();

}

void UDMUISlot_SlideList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Setup();

	UpdateMove(InDeltaTime);
}

FReply UDMUISlot_SlideList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Unhandled();

	if (InKeyEvent.GetKey() == EKeys::Q)
	{
		OnMove(true);
		return FReply::Handled();
	}
	else if (InKeyEvent.GetKey() == EKeys::E)
	{
		OnMove(false);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UDMUISlot_SlideList::Setup()
{
	if (SlideElementPath.IsEmpty())
		return;

	if (IsSetted)
		return;
	
	if(IsMadeList == false)
		MakeElementList();

	if (ElementList.IsValidIndex(0) == false || ElementList[0]->IsValidLowLevel() == false)
		return;

	if (ElementList[0]->GetDesiredSize().IsZero())
		return;

	SetupTransitionList();
	SetupElementList();

	UpdateElementList(true);

	IsSetted = true;
}

void UDMUISlot_SlideList::MakeElementList()
{
	// Make List
	for (int32 i = 0; i < ElementCount; ++i)
	{
		UDMUISlot_SlideElement* CreatedElementWidget = DMUIManager::Get()->CreateUISync_Casted<UDMUISlot_SlideElement>(SlideElementPath);
		if (CreatedElementWidget)
		{
			CreatedElementWidget->SetParent(this);
			CanvasPanelMain->AddChildToCanvas(CreatedElementWidget);
			ElementList.Add(CreatedElementWidget);

			FVector2D DesiredSizeThis = this->TakeWidget()->GetDesiredSize();
			FVector2D DesiredSizeElement = CreatedElementWidget->TakeWidget()->GetDesiredSize();
			float Width = DesiredSizeThis.X;
			float Height = DesiredSizeThis.Y;
		}
	}

	IsMadeList = true;

	// Test. 외부에서 지정해줘야한다.
	DataSize = TestDataList.Num();
}

void UDMUISlot_SlideList::SetupTransitionList()
{
	UDMUISlot_SlideElement* ElementWidget = ElementList[0];
	FVector2D ChildSize = ElementWidget->GetDesiredSize();

	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	FVector2D ParentSize = CanvasPanelSlot->GetSize();
	FVector2D ParentMiddlePosition = ParentSize / 2.0f;

	for (int32 i = 0; i < ElementCount; ++i)
	{
		FVector2D ResultPosition = FVector2D::ZeroVector;
		if (i == 0)
		{
			// Middle
			FVector2D ChildPosition;
			ChildPosition.X = ChildSize.X - (ChildSize / 3.f).X;
			ChildPosition.Y = ChildSize.Y - (ChildSize / 3.f).Y;
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

	MainTranslationIndex = (ElementCount - 1) / 2;	
}

void UDMUISlot_SlideList::SetupElementList()
{
	int32 Index = 0;
	for (auto ElementWidget : ElementList)
	{		
		ElementWidget->SetIndex(Index);
		ElementWidget->SetTranslationIndex(Index);
		FVector2D SlotTranslation = GetTranslation(Index);
		ElementWidget->SetRenderTranslation(SlotTranslation);

		if (Index == MainTranslationIndex)	// 메인은 메인설정
		{
			ElementWidget->SetMain(true);
			ElementWidget->SetRenderScale(FVector2D(MainScale, MainScale));
			UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ElementWidget);
			CanvasPanelSlot->SetZOrder(1);
		}
		else
		{
			ElementWidget->SetMain(false);
		}

		++Index;
	}
}

FVector2D UDMUISlot_SlideList::GetTranslation(const int32 IN InTranslationIndex)
{
	if (TranslationList.IsValidIndex(InTranslationIndex))
	{
		return TranslationList[InTranslationIndex];
	}
	return FVector2D::ZeroVector;
}

void UDMUISlot_SlideList::UpdateElement(class UDMUISlot_SlideElement* IN InElement)
{
	int32 DataIndex = StartingDataIndex + InElement->GetTranslationIndex();
	if (DataIndex < 0 || DataIndex > DataSize - 1)
	{
		InElement->SetEmpty();
	}
	else
	{
		InElement->SetData(DataIndex);
	}
}

void UDMUISlot_SlideList::OnMove(const bool IN InLeft)
{
	if (IsSetted == false)
		return;

	if (InLeft && MainTranslationDataIndex <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Last Left"));
		return;
	}

	if (InLeft == false && MainTranslationDataIndex >= DataSize - 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Last Right"));
		return;
	}
	
	if (IsMove && !IsMoveComplete)
	{
		if (IsLeft == InLeft)
			++MoveStep;
		else
			MoveStep = 1;
		DstMoveTime = RepeatMoveTime;
	}
	else
	{
		MoveStep = 1;
		DstMoveTime = NormalMoveTime;
	}

	NextMainTranslationIndex = InLeft ? MainTranslationIndex - 1 : MainTranslationIndex + 1;	// 왼쪽이 눌리면 왼쪽이 중앙으로 와야하므로 왼쪽이 다음 순번
// 	if (NextMainTranslationIndex <= 0 || NextMainTranslationIndex >= TranslationList.Num())
// 		return;

	//AccumulateMoveTime = 0.f;
	IsMoveComplete = false;
	IsMove = true;
	IsLeft = InLeft;
}

void UDMUISlot_SlideList::UpdateMove(const float IN InDeltaTime)
{
	if (IsSetted == false)
		return;

	if (IsMoveComplete)
		return;

	if (IsMove == false)
		return;

	if (IsLeft && MainTranslationDataIndex <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Last Left"));
		ReleaseMove();
		return;
	}

	if (IsLeft == false && MainTranslationDataIndex >= DataSize - 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Last Right"));
		ReleaseMove();
		return;
	}

	AccumulateMoveTime += InDeltaTime;	
	float Rate = AccumulateMoveTime / DstMoveTime;
	if (Rate >= 1.f)
	{
		Rate = 1.f;
	}

	for (auto ElementWidget : ElementList)
	{
		// Translation
		int32 CurrentTranslationIndex = ElementWidget->GetTranslationIndex();
		int32 NextTranslationIndex = IsLeft ? ElementWidget->GetTranslationIndex() + 1 : ElementWidget->GetTranslationIndex() - 1;	// 왼쪽이면 오른쪽으로 이동
		if (IsLeft && CurrentTranslationIndex == ElementCount - 1)
		{
			NextTranslationIndex = 0;	// 왼쪽입력시 슬라이드는 오른쪽으로 이동한다. 제일 오른쪽은 제일 왼쪽으로 이동
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			ElementWidget->SetRenderTranslation(DstTranslation);
		}
		else if (IsLeft == false && CurrentTranslationIndex == 0)
		{
			NextTranslationIndex = TranslationList.Num() - 1;	// 오른쪽입력시 슬라이드는 왼쪽으로 이동한다. 제일 왼쪽은 제일 오른쪽으로 이동
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			ElementWidget->SetRenderTranslation(DstTranslation);
		}
		else
		{
			FVector2D SrcTranslation = ElementWidget->RenderTransform.Translation;//GetTranslation(CurrentTranslationIndex);
			FVector2D DstTranslation = GetTranslation(NextTranslationIndex);
			FVector2D LerpTranslation;
			LerpTranslation.X = UKismetMathLibrary::Lerp(SrcTranslation.X, DstTranslation.X, Rate);
			LerpTranslation.Y = UKismetMathLibrary::Lerp(SrcTranslation.Y, DstTranslation.Y, Rate);
			ElementWidget->SetRenderTranslation(LerpTranslation);
		}		

		// Scale
		if(NextTranslationIndex == MainTranslationIndex)
		{
			FVector2D LerpScale;
			LerpScale.X = LerpScale.Y = UKismetMathLibrary::Lerp(ElementWidget->RenderTransform.Scale.X, MainScale, Rate);
			ElementWidget->SetRenderScale(LerpScale);

			if (Rate >= ChangeMainTime)
			{
				ElementWidget->SetMain(true);
				UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ElementWidget);
				CanvasPanelSlot->SetZOrder(1);
			}
		}
		else
		{
			if (ElementWidget->RenderTransform.Scale.X != 1.f)
			{
				FVector2D LerpScale;
				LerpScale.X = LerpScale.Y = UKismetMathLibrary::Lerp(ElementWidget->RenderTransform.Scale.X, 1.f, Rate);
				ElementWidget->SetRenderScale(LerpScale);
			}
			if (Rate >= ChangeMainTime)
			{
				ElementWidget->SetMain(false);
				UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ElementWidget);
				CanvasPanelSlot->SetZOrder(0);
			}
		}
	}

	// Finish
	if (Rate >= 1.f)
	{
		if (IsLeft)
		{
			--MainTranslationDataIndex;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);
		}
		else
		{
			++MainTranslationDataIndex;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);
		}

		for (auto ElementWidget : ElementList)
		{
			bool bSwap = false;
			int32 CurrentTranslationIndex = ElementWidget->GetTranslationIndex();
			int32 ModifyTranslationIndex = IsLeft ? ElementWidget->GetTranslationIndex() + 1 : ElementWidget->GetTranslationIndex() - 1;
			if (IsLeft && CurrentTranslationIndex == ElementCount - 1)
			{
				ModifyTranslationIndex = 0;	// 왼쪽입력시 슬라이드는 오른쪽으로 이동한다. 제일 오른쪽은 제일 왼쪽으로 이동
				bSwap = true;
			}
			else if (IsLeft == false && CurrentTranslationIndex == 0)
			{
				ModifyTranslationIndex = TranslationList.Num() - 1;	// 오른쪽입력시 슬라이드는 왼쪽으로 이동한다. 제일 왼쪽은 제일 오른쪽으로 이동
				bSwap = true;
			}

			if (ModifyTranslationIndex == MainTranslationIndex)
			{
				ElementWidget->SetMain(true);
			}
			else
			{
				ElementWidget->SetMain(false);
			}

			ElementWidget->SetTranslationIndex(ModifyTranslationIndex);
			FVector2D DstTranslation = GetTranslation(ModifyTranslationIndex);
			ElementWidget->SetRenderTranslation(DstTranslation);

			if (bSwap)
			{
				UpdateElement(ElementWidget);
			}
		}		

		--MoveStep;
		if (MoveStep == 0)
		{
			ReleaseMove();
		}		
		else
		{
			AccumulateMoveTime = 0.f;

			OnSlideChanged(MainTranslationDataIndex);
		}
	}
}

void UDMUISlot_SlideList::ReleaseMove()
{
	AccumulateMoveTime = 0.f;
	IsMoveComplete = true;
	IsMove = false;
	IsLeft = false;
	MoveStep = 0;
}

void UDMUISlot_SlideList::UpdateElementList(const bool IN InInit)
{
	if (InInit)
	{
		switch (SlideStartType)
		{
		case EDMSlideStartType::Head:
		{
			MainTranslationDataIndex = 0;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 중앙->오른쪽으로 데이터 삽입
		}
		break;
		case EDMSlideStartType::Middle:
		{
			MainTranslationDataIndex = DataSize / 2;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 왼쪽<-중앙으로 인덱스-1씩 데이터 삽입
			// + 중앙->오른쪽으로 인덱스+1씩 데이터 삽입
		}
		break;
		case EDMSlideStartType::Tail:
		{
			MainTranslationDataIndex = DataSize - 1;
			StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

			// + 왼쪽<-중앙으로 인덱스 -1씩 데이터 삽입
		}
		break;
		}
	}	

	for (auto ElementWidget : ElementList)
	{
		UpdateElement(ElementWidget);
	}
}

void UDMUISlot_SlideList::ChangeElementListByDataIndex(const int32 IN InDataIndex)
{
	MainTranslationDataIndex = InDataIndex;
	StartingDataIndex = MainTranslationDataIndex - (TranslationList.Num() / 2);

	UpdateElementList(false);
}

UDMUISlot_SlideElement* UDMUISlot_SlideList::GetElement(const int32 IN InTranslationIndex)
{
	for (auto ElementWidget : ElementList)
	{
		if (ElementWidget->GetTranslationIndex() == InTranslationIndex)
			return ElementWidget;
	}
	return nullptr;
}

void UDMUISlot_SlideList::SetupElement(FString IN InPath)
{
	SlideElementPath = InPath;
}

void UDMUISlot_SlideList::OnSlideChanged(int32 InDataIndex)
{
	// + Delegate


}