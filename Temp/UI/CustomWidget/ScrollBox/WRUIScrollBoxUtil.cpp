
#include "WRUIScrollBoxUtil.h"
#include "WRUISScrollBox.h"
#include "CanvasPanelSlot.h"
#include "WRUIScrollBox.h"
#include "Manager/WRUIManager.h"
#include "UI/Base/WRUISlot.h"

WRUIScrollBoxUtil::WRUIScrollBoxUtil()
	: UsedGridPanel(false)
{
	RefreshContentCallbackFunc = nullptr;
	RefreshAllContentsCallbackFunc = nullptr;
}

WRUIScrollBoxUtil::~WRUIScrollBoxUtil()
{

}


bool WRUIScrollBoxUtil::Create(int32 NumXl, int32 NumYl, UUserWidget *Widget, EWRUISlotKind SlotKind, FName ScrollBoxName, FName GridPanelName, float ScaleRate /* = 1.0f */, ESlateVisibility DeafultSlotVisibility /*= ESlateVisibility::SelfHitTestInvisible*/)
{
	NumScrolling[0] = NumXl;
	NumScrolling[1] = NumYl;
	StartSlotIdx = 0;
	ScrollSlotLength = FVector2D::ZeroVector;
	ScrollBoxLength = FVector2D::ZeroVector;
	GridPanelSlots.Empty();
	RollingCount = 0;

	ScrollBox = Cast<UWRUIScrollBox>(Widget->GetWidgetFromName(ScrollBoxName));
	if (!ScrollBox)
	{
		return false;
	}
	ScrollBox->SetVisibility(ESlateVisibility::Visible);

	ScrollBox->SetScrollOffset(1);
	GridPanel = Cast<UUniformGridPanel>(Widget->GetWidgetFromName(GridPanelName));
	ScrollSlotPadding = FVector2D::ZeroVector;
	if (GridPanel)
	{
		UsedGridPanel = true;
		ScrollSlotPadding.X = GridPanel->SlotPadding.Left + GridPanel->SlotPadding.Right;
		ScrollSlotPadding.Y = GridPanel->SlotPadding.Top + GridPanel->SlotPadding.Bottom;
	}
	int32 i, j;

	for (i = 0; i < NumScrolling[1]; i++)
	{
		for (j = 0; j < NumScrolling[0]; j++)
		{
			UWRUISlot* CreatedSlot = WRUIManager::Get()->CreateUISlot(SlotKind);
			if (GridPanel)
			{
				if (ScaleRate != 1.0f)
				{
					CreatedSlot->SetRenderScale(FVector2D(ScaleRate, ScaleRate));
				}
				UUniformGridSlot* GridSlot = GridPanel->AddChildToUniformGrid(CreatedSlot);
				GridSlot->SetColumn(j);
				GridSlot->SetRow(i);
				GridPanelSlots.Add(GridSlot);
			}
			else
			{
				ScrollBox->AddChild(CreatedSlot);
			}
			CreatedSlot->SetVisibility(DeafultSlotVisibility);
		}
	}

	if (GridPanel && GetScrollDirection() == 0)
	{
		//[180205 SeungYeop ] : 그리드패널로 가로 스크롤을 쓸일이 없을것으로 판단됩니다.
		// 가로 스크롤이 세로로 두줄이상일때 유저 입장에서 상당히 불편할거 같고 
		// 현재 그렇게 사용하고 있는게임이 없습니다. 따라서 해당 내용에 대한 대응을 하지 않았고 
		// 그렇게 쓸일이 있을시에는 승엽을 찾아주세요
		
	}

	//  [4/9/2018 LHJ : OverScroll되었을때 호출 함수 바인딩]
	if (ScrollBox->IsValidLowLevel() == true)
	{
		ScrollBox->OnOverScrollEvent = UWRUIScrollBox::FOnOverScrollEvent::CreateRaw(this, &WRUIScrollBoxUtil::EVENT_OnOverScroll);
	}

	return true;
}

void WRUIScrollBoxUtil::Destroy()
{
	ScrollSlotLength = FVector2D::ZeroVector;

	if (GridPanel)
		GridPanel->ClearChildren();
	GridPanel = nullptr;
	NumContents = 0;
	GridPanelSlots.Empty();	

}
void WRUIScrollBoxUtil::RefeshContents()
{
	if (RefreshAllContentsCallbackFunc)
	{
		RefreshAllContentsCallbackFunc(RefreshAllContentsFuncUserPtr);
	}
	if (!RefreshContentCallbackFunc)
		return;
	if (!ScrollBox->MyScrollBox.IsValid())
		return;
	int32 i, j;

	if (ScrollBox->MyScrollBox->GetOrientation() == Orient_Vertical)
	{
		int32 NumTotal = NumScrolling[0] * NumScrolling[1];
		for (i = StartSlotIdx*NumScrolling[0]; i < StartSlotIdx*NumScrolling[0] + NumTotal; i++)
		{
			RefreshContentCallbackFunc(RefreshContentFuncUserPtr, i);
		}
	}
	else
	{
		for (i = 0; i < NumScrolling[1]; i++)
		{
			for (j = StartSlotIdx; j < StartSlotIdx + NumScrolling[0]; j++)
			{
				RefreshContentCallbackFunc(RefreshContentFuncUserPtr, i*NumScrolling[0] + j);
			}
		}
	}
}

int32 WRUIScrollBoxUtil::GetScrollDirection()
{
	if (!ScrollBox->MyScrollBox.IsValid())
		return 1;
	return ScrollBox->MyScrollBox->GetScrollDirection();
}

void WRUIScrollBoxUtil::RollingChild(int32 offset, int8 direction)
{
	if (GridPanel)
	{
		if (NumScrolling[0] * NumScrolling[1] != GridPanelSlots.Num())
		{
			//ensureMsgf(false, Error, "Invalid State");
			return;
		}

		int32 r_direction = (direction ? 0 : 1);
		
		for (int32 i = 0; i < NumScrolling[direction]; ++i)
		{
			int32 _ChangeIndex = i - offset - RollingCount;
			_ChangeIndex = _ChangeIndex % NumScrolling[direction];
			if (_ChangeIndex < 0)
				_ChangeIndex += NumScrolling[direction];

			for (int32 j = 0; j < NumScrolling[r_direction]; ++j)
			{
				int32 _Index = i * NumScrolling[r_direction] + j;

				if (direction)
				{
					GridPanelSlots[_Index]->SetRow(_ChangeIndex);
				}
				else
				{
					GridPanelSlots[_Index]->SetColumn(_ChangeIndex);
				}
			}
		}

		RollingCount += offset;
		RollingCount = RollingCount % NumScrolling[direction];					
	}
	else
	{
		if (offset > 0)
		{
			for (int32 i = 0; i < offset; ++i)
			{
				ScrollBox->ShiftChildIncludeSScrollBox(NumScrolling[direction] - 1, ScrollBox->GetChildAt(0));
			}
		}
		else
		{
			for (int32 i = 0; i < offset * -1; ++i)
			{
				ScrollBox->ShiftChildIncludeSScrollBox(0, ScrollBox->GetChildAt(NumScrolling[direction] - 1));
			}
		}		
	}
}

void WRUIScrollBoxUtil::EVENT_OnOverScroll()
{
	if (RefreshEndContentsCallbackFunc)
		RefreshEndContentsCallbackFunc(RefreshEndContentsFuncUserPtr);
}

UUserWidget* WRUIScrollBoxUtil::GetScrollWidget(int32 ListIdx)
{
	//// 실제 리얼 index를 구하자.
	//if (GetScrollDirection() == 1)	//세로 스크롤 경우.
	//{
	//	// 1*10의 경우 0번째의 값이 
	//	// 0 * (10-1) = 0
	//	// 1 * (10-1) = 9 가 되어버려 안맞음.
	//	//ListIdx -= StartSlotIdx * (NumScrolling[1] - 1);
	//	ListIdx -= StartSlotIdx * NumScrolling[0];
	//}
	//else
	//{
	//	ListIdx -= StartSlotIdx;
	//}

	if (GridPanel)
	{		
		int32 direction = GetScrollDirection();
		int32 r_direction = (direction ? 0 : 1);

		ListIdx -= NumScrolling[r_direction] * StartSlotIdx;
		if (ListIdx >= GridPanelSlots.Num() || ListIdx < 0)
			return nullptr;

		int32 _Index = ListIdx + RollingCount * NumScrolling[r_direction];
		if (_Index < 0)
			_Index += GridPanelSlots.Num();
		else if (_Index >= GridPanelSlots.Num())
			_Index -= GridPanelSlots.Num();

		return Cast<UUserWidget>(GridPanelSlots[_Index]->Content);
	}
	else
	{
		ListIdx -= StartSlotIdx;

		if (ListIdx >= ScrollBox->GetChildrenCount() || ListIdx < 0)
			return nullptr;

		return Cast<UUserWidget>(ScrollBox->GetChildAt(ListIdx));
	}
	
	return nullptr;
}


void WRUIScrollBoxUtil::SetVisibility(const ESlateVisibility IN InVisibility)
{
	if (ScrollBox == nullptr)
		return;

	ScrollBox->SetVisibility(InVisibility);
}

void WRUIScrollBoxUtil::SetScrollOffset(const float IN InOffset)
{
	if (ScrollBox == nullptr)
		return;

	ScrollBox->SetScrollOffset(InOffset);
}


void WRUIScrollBoxUtil::ScrollBySlotIndex(const int32 IN InIndex)
{
	if (ScrollBox == nullptr)
		return;

	ScrollBox->SetScrollOffset(ScrollSlotLength[GetScrollDirection()] * InIndex);	
}

void WRUIScrollBoxUtil::Tick(float DeltaTime, int32 TotalNumContents)
{
	if (!ScrollBox || NumScrolling[1] * NumScrolling[0] == 0)
		return;
	if (!ScrollBox->MyScrollBox.IsValid())
		return;
#define CHECK_PHYSICAL_OFFSET	

#ifdef CHECK_PHYSICAL_OFFSET	
	float now_offset = ScrollBox->GetPhysicalScrollOffset();
#else
	float now_offset = ScrollBox->GetScrollOffset();
#endif
	int32 direction = GetScrollDirection();
	int32 r_direction = 1 - direction;

	if (ScrollBoxLength[r_direction] == 0 && ScrollBox)
	{
		ScrollBoxLength = Cast<UCanvasPanelSlot>(ScrollBox->Slot)->GetSize();
	}

	if (ScrollSlotLength[direction] == 0)
	{
		if (GridPanel)
		{
			UWidget* _Widget = GridPanel->GetChildAt(0);
			if(_Widget && _Widget->GetDesiredSize() != FVector2D::ZeroVector)
				ScrollSlotLength = _Widget->GetDesiredSize() + ScrollSlotPadding;
		}
		else
		{
			UWidget* _Widget = ScrollBox->GetChildAt(0);
			if (_Widget && _Widget->GetDesiredSize() != FVector2D::ZeroVector)
				ScrollSlotLength = _Widget->GetDesiredSize() + ScrollSlotPadding;
		}


		/// 토탈넘이 바뀌었을떄 처리 쪽으로 변경

		ScrollBox->MyScrollBox->ScrollBoxInfo.ScrollSlotLength = ScrollSlotLength[direction];

	}
	else
	{
		int vr_ylength = (TotalNumContents + NumScrolling[r_direction] - 1) / NumScrolling[r_direction];

#define _CHECK_YLENGTH	(ScrollSlotLength[direction])

		bool bRefresh = false;
		if (NumContents != TotalNumContents)
		{
			if (TotalNumContents < NumContents)
			{
				int32 NewStartSlotIdxY = vr_ylength - NumScrolling[direction];
				if (NewStartSlotIdxY < 0)
				{
					//ScrollBox->SetScrollOffset(1);
					NewStartSlotIdxY = 0;
				}
				if (NewStartSlotIdxY < StartSlotIdx)
				{
					StartSlotIdx = NewStartSlotIdxY;
				}
			}
			NumContents = TotalNumContents;
			bRefresh = true;
		}

		int32 ChangeStartSlotIdx = StartSlotIdx;
		int32 ChangeIdx = now_offset / ScrollSlotLength[direction];
		
		if (now_offset < 0)
			--ChangeIdx;

		if (StartSlotIdx + ChangeIdx != StartSlotIdx)
		{
			if (StartSlotIdx + ChangeIdx < 0)
			{				
				ChangeStartSlotIdx = 0;
			}
			else if (StartSlotIdx + ChangeIdx + NumScrolling[direction] > vr_ylength)
			{
				ChangeStartSlotIdx = vr_ylength - NumScrolling[direction];
				if (ChangeStartSlotIdx < 0)
					ChangeStartSlotIdx = 0;
			}
			else
			{
				ChangeStartSlotIdx = StartSlotIdx + ChangeIdx;
			}		

			if (ChangeStartSlotIdx != StartSlotIdx)
			{				
				if (ChangeStartSlotIdx - StartSlotIdx < NumScrolling[direction])
					RollingChild(ChangeStartSlotIdx - StartSlotIdx, direction);
				

				StartSlotIdx = ChangeStartSlotIdx;
				bRefresh = true;
			}

			//if(FMath::Abs(ChangeStartSlotIdx - StartSlotIdx) < )
		}	
		
		//if (now_offset < 1)
		//{
		//	if (StartSlotIdx > 0)
		//	{
		//		ScrollBox->SetScrollOffset(ScrollBox->GetScrollOffset() - 1);	//한번만 리프레쉬를 위해
		//		StartSlotIdx--;
		//		bRefresh = true;
		//	}
		//}
		//else if (now_offset > _CHECK_YLENGTH)
		//{
		//	if (StartSlotIdx + NumScrolling[direction] < vr_ylength)
		//	{
		//		ScrollBox->SetScrollOffset(ScrollBox->GetScrollOffset() + 1);	//한번만 리프레쉬를 위해
		//		StartSlotIdx++;
		//		bRefresh = true;
		//	}
		//	// 			if (vr_ylength <=  NumScrolling[direction] - 1)
		//	// 				ScrollBox->SetScrollOffset(0);
		//}
		//else
		//{
		//	//  			if( vr_ylength <=  NumScrolling[direction] - 1)
		//	//  				ScrollBox->SetScrollOffset(0);
		//}
		FWRScrollBoxInfo ScrollBoxInfo;
		ScrollBoxInfo.TotalNumContents = TotalNumContents;
		ScrollBoxInfo.NumScrolling[0] = NumScrolling[0];
		ScrollBoxInfo.NumScrolling[1] = NumScrolling[1];
		ScrollBoxInfo.ScrollSlotLength = ScrollSlotLength[direction];
		ScrollBoxInfo.StartSlotIdx = StartSlotIdx;
		ScrollBox->MyScrollBox->ScrollBoxInfo = ScrollBoxInfo;

		if (bRefresh)
		{
			//static int aaa=0;
			//FString Str = TEXT("bRefresh");
			//GEngine->AddOnScreenDebugMessage(-11212+aaa--, 1.0f, FColor::Yellow, Str);




			////////////////// 시퀀스 매니저 대체 필요
			RefeshContents();
			//IMSeqDelayManager::Get()->AddItem(1, FIMSeqDelayBase::FSeqDelayDele::CreateRaw(this, &WRUIScrollBoxUtil::RefeshContents));




			//RefeshContents();
		}
	}
}

void WRUIScrollBoxUtil::ScrollToStart()
{
	ScrollBox->ScrollToStart();
}
