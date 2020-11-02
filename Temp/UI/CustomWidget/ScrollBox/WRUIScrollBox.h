// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Components/PanelWidget.h"
#include "WRUIScrollBox.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWROnUserScrolledEvent, float, CurrentOffset);

/**
* An arbitrary scrollable collection of widgets.  Great for presenting 10-100 widgets in a list.  Doesn't support virtualization.
*/
UCLASS()
class UWRUIScrollBox : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DELEGATE(FOnOverScrollEvent);

	FOnOverScrollEvent OnOverScrollEvent;

public:

	/** The style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (DisplayName = "Style"))
		FScrollBoxStyle WidgetStyle;

	/** The bar style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (DisplayName = "Bar Style"))
		FScrollBarStyle WidgetBarStyle;

	UPROPERTY()
		USlateWidgetStyleAsset* Style_DEPRECATED;

	UPROPERTY()
		USlateWidgetStyleAsset* BarStyle_DEPRECATED;

	/** The orientation of the scrolling and stacking in the box. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		TEnumAsByte<EOrientation> Orientation;

	/** Visibility */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		ESlateVisibility ScrollBarVisibility;

	/**  Enable to always consume mouse wheel event, even when scrolling is not possible */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		EConsumeMouseWheel ConsumeMouseWheel;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		FVector2D ScrollbarThickness;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		bool AlwaysShowScrollbar;

	/**  Disable to stop scrollbars from activating inertial overscrolling */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		bool AllowOverscroll;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		EDescendantScrollDestination NavigationDestination;

	/**
	* The amount of padding to ensure exists between the item being navigated to, at the edge of the
	* scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scroll")
		float NavigationScrollPadding;

	UFUNCTION(BlueprintCallable, Category = "Scroll")
		void SetOrientation(EOrientation NewOrientation);

	UFUNCTION(BlueprintCallable, Category = "Scroll")
		void SetScrollBarVisibility(ESlateVisibility NewScrollBarVisibility);

	UFUNCTION(BlueprintCallable, Category = "Scroll")
		void SetScrollbarThickness(const FVector2D& NewScrollbarThickness);

	UFUNCTION(BlueprintCallable, Category = "Scroll")
		void SetAlwaysShowScrollbar(bool NewAlwaysShowScrollbar);

	UFUNCTION(BlueprintCallable, Category = "Scroll")
		void SetAllowOverscroll(bool NewAllowOverscroll);
public:

	/** Called when the scroll has changed */
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FWROnUserScrolledEvent OnUserScrolled;

	/**
	* Updates the scroll offset of the scrollbox.
	* @param NewScrollOffset is in Slate Units.
	*/
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SetScrollOffset(float NewScrollOffset);

	/**
	* Gets the scroll offset of the scrollbox in Slate Units.
	*/
	UFUNCTION(BlueprintCallable, Category = "Widget")
		float GetScrollOffset() const;

	/** Scrolls the ScrollBox to the top instantly */
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ScrollToStart();

	/** Scrolls the ScrollBox to the bottom instantly during the next layout pass. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ScrollToEnd();

	/** Scrolls the ScrollBox to the widget during the next layout pass. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ScrollWidgetIntoView(UWidget* WidgetToFind, bool AnimateScroll = true, EDescendantScrollDestination ScrollDesintion = EDescendantScrollDestination::IntoView);

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
	virtual void OnDescendantSelectedByDesigner(UWidget* DescendantWidget) override;
	virtual void OnDescendantDeselectedByDesigner(UWidget* DescendantWidget) override;
	//~ End UWidget Interface
#endif

#if WITH_EDITOR
#else // WITH_EDITOR
	void ShiftChild(int32 Index, UWidget* Child);	// UPanelWidget::ShiftChild 와 같은 함수인데 에디터가 아닌곳에서 사용하려고 정의함
#endif // WITH_EDITOR

	void ShiftChildIncludeSScrollBox(int32 Index, UWidget* Child);	// 기본 ShiftChild는 SScrollBox까지 바꿔주지 않아 SScrollBox까지 바꿔주도록 함
protected:

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	void SlateHandleUserScrolled(float CurrentOffset);

protected:
	/** The desired scroll offset for the underlying scrollbox.  This is a cache so that it can be set before the widget is constructed. */	
	float DesiredScrollOffset;
	//171017 by odin
public:
	void SetPhysicalScrollOffset(float NewScrollOffset);
	float GetPhysicalScrollOffset() const;

	TSharedPtr<class WRUISScrollBox> MyScrollBox;

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	void EVENT_OnOverScroll();

#if WITH_EDITOR
	FDelegateHandle TickHandle;
#endif //WITH_EDITOR
};
