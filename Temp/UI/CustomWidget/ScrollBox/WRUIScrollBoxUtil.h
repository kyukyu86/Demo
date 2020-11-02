#pragma once
#include "WRUIScrollBox.h"
#include "Enum/EWRUIEnum.h"
#include "UniformGridSlot.h"
#include "UniformGridPanel.h"

//20171019 by odin
#if PLATFORM_WINDOWS
#define _CALLBACK	__stdcall
#else
#define _CALLBACK
#endif

typedef void    (_CALLBACK *RefreshContentCallback)(void *UserPtr,int32 StartIdx);	// 슬롯하나하나 리프레쉬 필요할때 호출할 콜백함수.
typedef void    (_CALLBACK *RefreshAllContentsCallback)(void *UserPtr);				// 전체 리프레쉬 호출할 콜백함수.

//  [4/7/2018 LHJ] : 무한 스크롤 구현하기 위해 마지막 스크롤이 갱신되었을때 호출되는 콜백함수 선언
typedef void    (_CALLBACK *RefreshEndContentsCallback)(void *UserPtr);				// 마지막 스크롤이라는 것을 알려주는 콜백

class UWRUIScrollBox;

class WRUIScrollBoxUtil
{
protected:
	RefreshContentCallback RefreshContentCallbackFunc;
	RefreshAllContentsCallback RefreshAllContentsCallbackFunc;
	RefreshEndContentsCallback RefreshEndContentsCallbackFunc;
	int32 NumContents;
	int32 NumScrolling[2];	//눈에 보이는 가로,세로 스크롤링할 사이즈. 최소사이즈(눈에보이는것 + 1)로 한다.
	int32 StartSlotIdx;	//시작 인덱스 
	UUserWidget* ScrollFrame;
	FVector2D ScrollBoxLength;
	FVector2D ScrollSlotLength;
	FVector2D ScrollSlotPadding;	//경우에 따라UUniformGridPanel 패딩이 존재해서 패딩값을 처리해줘야 한다.
	UWRUIScrollBox* ScrollBox;
	UUniformGridPanel* GridPanel;
	void *RefreshContentFuncUserPtr;
	void *RefreshAllContentsFuncUserPtr;
	void *RefreshEndContentsFuncUserPtr;
	bool UsedGridPanel;
	TArray<UUniformGridSlot*> GridPanelSlots;
	int32 RollingCount;

protected:
	void RefeshContents();
	int32 GetScrollDirection();
	void RollingChild(int32 offset, int8 direction);
	void EVENT_OnOverScroll();

public:
	WRUIScrollBoxUtil();
	~WRUIScrollBoxUtil();

	/* 현재는 스크롤이 위/아래로만 된다.따라서 RealYl이 5개라면 실제 화면에는 4개칸이 보이는 크기이고 아래 한칸 더 만들어서 스크롤시 그걸로 무한스크롤이 가능하게 한다. */
	bool Create(
		int32 NumXl,
		int32 NumYl,
		UUserWidget *Widget,
		EWRUISlotKind SlotKind,
		FName ScrollBoxName,
		FName GridPanelName,
		float ScaleRate = 1.0f,
		ESlateVisibility DeafultSlotVisibility = ESlateVisibility::SelfHitTestInvisible);
	void Destroy();
	void Tick(float DeltaTime, int32 TotalNumContents);
	void ScrollToStart();

	UUserWidget* GetScrollWidget(int32 ListIdx);	//해당되는 리스트의 Idx를 얻어온다.
	//슬롯하나하나 리프레쉬 필요할때 호출할 콜백함수.
	void SetRefreshContentCallbackFunc(void *UserPtr, RefreshContentCallback CallFunc) { RefreshContentFuncUserPtr = UserPtr;	 RefreshContentCallbackFunc = CallFunc; };
	//전체 리프레쉬 호출할 콜백함수.
	void SetRefreshAllContentsCallbackFunc(void *UserPtr, RefreshAllContentsCallback CallFunc) { RefreshAllContentsFuncUserPtr = UserPtr;	 RefreshAllContentsCallbackFunc = CallFunc; };
	//마지막 리프레쉬 호출할 콜백함수.
	void SetRefreshEndContentsCallbackFunc(void *UserPtr, RefreshEndContentsCallback CallFunc) { RefreshEndContentsFuncUserPtr = UserPtr;	 RefreshEndContentsCallbackFunc = CallFunc; };


	void SetVisibility(const ESlateVisibility IN InVisibility);
	void SetScrollOffset(const float IN InOffset);
	void ScrollBySlotIndex(const int32 IN InIndex);

};