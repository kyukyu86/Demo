// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIScrollBox.h"
#include "Components/ScrollBox.h"
#include "Containers/Ticker.h"
#include "Components/ScrollBoxSlot.h"
#include "WRUISScrollBox.h"
#include "WRUIScrollBoxSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UWRUIScrollBox

UWRUIScrollBox::UWRUIScrollBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Orientation(Orient_Vertical)
	, ScrollBarVisibility(ESlateVisibility::Visible)
	, ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	, ScrollbarThickness(5.0f, 5.0f)
	, AlwaysShowScrollbar(false)
	, AllowOverscroll(true)
	, NavigationDestination(EDescendantScrollDestination::IntoView)
	, NavigationScrollPadding(0.0f)
{
	bIsVariable = false;

	SScrollBox::FArguments Defaults;
	Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());
	Clipping = EWidgetClipping::ClipToBounds;

	WidgetStyle = *Defaults._Style;
	WidgetBarStyle = *Defaults._ScrollBarStyle;
}

void UWRUIScrollBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyScrollBox.Reset();
}

UClass* UWRUIScrollBox::GetSlotClass() const
{
	return UWRUIScrollBoxSlot::StaticClass();
}

void UWRUIScrollBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyScrollBox.IsValid())
	{
		CastChecked<UWRUIScrollBoxSlot>(InSlot)->BuildSlot(MyScrollBox.ToSharedRef());
	}
}

void UWRUIScrollBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyScrollBox.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyScrollBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedRef<SWidget> UWRUIScrollBox::RebuildWidget()
{
	MyScrollBox = SNew(WRUISScrollBox)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled));

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UWRUIScrollBoxSlot* TypedSlot = Cast<UWRUIScrollBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
		}
	}

	if (MyScrollBox.IsValid())
	{
		// Bind to FIMUIScrollBoxUtil::EVENT_OnOverScroll 
		MyScrollBox->OnOverScrollEvent = BIND_UOBJECT_DELEGATE(WRUISScrollBox::FOnOverScrollEvent, EVENT_OnOverScroll);
	}

	return MyScrollBox.ToSharedRef();
}

void UWRUIScrollBox::EVENT_OnOverScroll()
{
	// Called to [FIMUIScrollBoxUtil::EVENT_OnOverScroll]
	OnOverScrollEvent.ExecuteIfBound();
}

void UWRUIScrollBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyScrollBox->SetScrollOffset(DesiredScrollOffset);
	MyScrollBox->SetOrientation(Orientation);
	MyScrollBox->SetScrollBarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(ScrollBarVisibility));
	MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
}

float UWRUIScrollBox::GetScrollOffset() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffset();
	}

	return 0;
}

void UWRUIScrollBox::SetScrollOffset(float NewScrollOffset)
{
	DesiredScrollOffset = NewScrollOffset;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollOffset(NewScrollOffset);
	}
}

void UWRUIScrollBox::ScrollToStart()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToStart();
	}
}
void UWRUIScrollBox::SetPhysicalScrollOffset(float NewScrollOffset)
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetPhysicalScrollOffset(NewScrollOffset);
	}
}


float UWRUIScrollBox::GetPhysicalScrollOffset() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetPhysicalScrollOffset();
	}
	return 0;
}

void UWRUIScrollBox::ScrollToEnd()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToEnd();
	}
}

void UWRUIScrollBox::ScrollWidgetIntoView(UWidget* WidgetToFind, bool AnimateScroll, EDescendantScrollDestination InScrollDestination)
{
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MyScrollBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to scroll which is necessary to avoid warnings/ensures 
		//       when we request to scroll to a widget and later remove that widget!
		MyScrollBox->ScrollDescendantIntoView(SlateWidgetToFind, AnimateScroll, InScrollDestination);
	}
}

void UWRUIScrollBox::PostLoad()
{
	Super::PostLoad();

	if (GetLinkerUE4Version() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS)
	{
		if (Style_DEPRECATED != nullptr)
		{
			const FScrollBoxStyle* StylePtr = Style_DEPRECATED->GetStyle<FScrollBoxStyle>();
			if (StylePtr != nullptr)
			{
				WidgetStyle = *StylePtr;
			}

			Style_DEPRECATED = nullptr;
		}

		if (BarStyle_DEPRECATED != nullptr)
		{
			const FScrollBarStyle* StylePtr = BarStyle_DEPRECATED->GetStyle<FScrollBarStyle>();
			if (StylePtr != nullptr)
			{
				WidgetBarStyle = *StylePtr;
			}

			BarStyle_DEPRECATED = nullptr;
		}
	}
}

void UWRUIScrollBox::SetOrientation(EOrientation NewOrientation)
{
	Orientation = NewOrientation;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetOrientation(Orientation);
	}
}

void UWRUIScrollBox::SetScrollBarVisibility(ESlateVisibility NewScrollBarVisibility)
{
	ScrollBarVisibility = NewScrollBarVisibility;

	if (MyScrollBox.IsValid())
	{
		switch (ScrollBarVisibility)
		{
		case ESlateVisibility::Collapsed:				MyScrollBox->SetScrollBarVisibility(EVisibility::Collapsed); break;
		case ESlateVisibility::Hidden:					MyScrollBox->SetScrollBarVisibility(EVisibility::Hidden); break;
		case ESlateVisibility::HitTestInvisible:		MyScrollBox->SetScrollBarVisibility(EVisibility::HitTestInvisible); break;
		case ESlateVisibility::SelfHitTestInvisible:	MyScrollBox->SetScrollBarVisibility(EVisibility::SelfHitTestInvisible); break;
		case ESlateVisibility::Visible:					MyScrollBox->SetScrollBarVisibility(EVisibility::Visible); break;
		}
	}
}

void UWRUIScrollBox::SetScrollbarThickness(const FVector2D& NewScrollbarThickness)
{
	ScrollbarThickness = NewScrollbarThickness;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	}
}

void UWRUIScrollBox::SetAlwaysShowScrollbar(bool NewAlwaysShowScrollbar)
{
	AlwaysShowScrollbar = NewAlwaysShowScrollbar;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	}
}

void UWRUIScrollBox::SetAllowOverscroll(bool NewAllowOverscroll)
{
	AllowOverscroll = NewAllowOverscroll;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	}
}

void UWRUIScrollBox::SlateHandleUserScrolled(float CurrentOffset)
{
	OnUserScrolled.Broadcast(CurrentOffset);
}

#if WITH_EDITOR

const FText UWRUIScrollBox::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

void UWRUIScrollBox::OnDescendantSelectedByDesigner(UWidget* DescendantWidget)
{
	UWidget* SelectedChild = UWidget::FindChildContainingDescendant(this, DescendantWidget);
	if (SelectedChild)
	{
		ScrollWidgetIntoView(SelectedChild, true);

		if (TickHandle.IsValid())
		{
			FTicker::GetCoreTicker().RemoveTicker(TickHandle);
			TickHandle.Reset();
		}
	}
}

void UWRUIScrollBox::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	if (TickHandle.IsValid())
	{
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	// because we get a deselect before we get a select, we need to delay this call until we're sure we didn't scroll to another widget.
	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float) -> bool
	{
		this->ScrollToStart();
		return false;
	}));
}

#endif

#if WITH_EDITOR
#else // WITH_EDITOR
void UWRUIScrollBox::ShiftChild(int32 Index, UWidget* Child)
{
	int32 CurrentIndex = GetChildIndex(Child);
	Slots.RemoveAt(CurrentIndex);
	Slots.Insert(Child->Slot, FMath::Clamp(Index, 0, Slots.Num()));
}
#endif // WITH_EDITOR

void UWRUIScrollBox::ShiftChildIncludeSScrollBox(int32 Index, UWidget* Child)
{
	int32 CurIdx = GetChildIndex(Child);
	ShiftChild(Index, Child);
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->Move(CurIdx, Index);
	}
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
