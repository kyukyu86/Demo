// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUITileView.h"


bool UDMUITileView::AddScrollOffset(const float IN InAddOffset)
{
// 	const double AdditionalOffset = (MouseEvent.GetWheelDelta() >= 0.f ? -1.f : 1.f) * GetNumItemsPerLine();
// 	const double NewScrollOffset = FMath::Max(0., DesiredScrollOffset + AdditionalOffset);
// 	AmountScrolledInItems = this->ScrollTo(NewScrollOffset);
// }
// 	// No required offset to maintain, so we scroll by units
// 	AmountScrolledInItems = this->ScrollBy(MyGeometry, -MouseEvent.GetWheelDelta() * WheelScrollMultiplier, EAllowOverscroll::No);

	TSharedRef<STableViewBase, ESPMode::NotThreadSafe> TileView = StaticCastSharedRef<STableViewBase>(TakeWidget());
	float fNewOffset = TileView->GetScrollOffset() + InAddOffset;
	if (fNewOffset < 0.f)
		fNewOffset = 0.f;

	if (fNewOffset <= 0.f && TileView->GetScrollOffset() <= 0.f)
		return false;

	TileView->AddScrollOffset(InAddOffset, true);
	//TileView_List->SetScrollOffset(InAnalogEvent.GetAnalogValue());

	return true;
}
