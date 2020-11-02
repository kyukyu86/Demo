// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRUIComponentBase.h"
#include "WidgetPath.h"
#include "Events.h"

/*

	WRUIComponentFocus 은 Panel, Slot 안에서 FocusButton의 포커스 변동에 관여하기 위해서 만들었습니다.
	관여하기를 원하는 모든 FocusButton을 일일이 직접 바인딩 해줘야합니다.

	일반적으로 클릭이벤트만 발생시키려면 이 Component를 사용할 필요가 없습니다.

	- DefaultFocusedButton : Panel, Slot 안에서 기본으로 잡힐 Button 입니다

		UIComponentFocus->SetFocusButton( FocusButton , true);	// true면 Default에도 셋팅하게 됩니다.

	- LastFocusedButton : Panel, Slot 안에서 마지막으로 포커싱 된 Button 입니다.

	- 포커스 변동 이벤트 (기본)
	FOnFocusChanged Delegate;
	Delegate.BindUObject( WRUIComponentFocus , &WRUIComponentFocus::OnFocusChanged);	//  WRUIComponentFocus::OnFocusChanged 연결
	FocusButton->SetFocusChangedDelegate(Delegate);

	- 포커스 변동 이벤트 (커스텀) : 각 Widget에서 포커스 변동에 대응할 기타 처리가 있어서 별도로 알고자 한다면
	FOnFocusChanged Delegate;
	Delegate.BindUObject( Widget , &Widget::OnFocusChanged);	//  각 Widget에서 별도 함수로 OnFocusChanged를 만들고 내부에서 WRUIComponentFocus::OnFocusChanged를 함께 호출해줘야합니다.
	FocusButton->SetFocusChangedDelegate(Delegate);


 */
class WR_API WRUIComponentFocus : public WRUIComponentBase
{
private:
	class UWRFocusButton* DefaultFocusedButton = nullptr;
	class UWRFocusButton* LastFocusedButton = nullptr;

public:
	WRUIComponentFocus();
	~WRUIComponentFocus();

	class UWRFocusButton* const GetDefaultFocusButton() { return DefaultFocusedButton; }

	// 마지막으로 포커싱 된 버튼
	FORCEINLINE class UWRFocusButton* GetFocusedButton() { return LastFocusedButton; }
	void SetFocusButton(class UWRFocusButton* IN InFocusButton, const bool IN InDefault = false);

	// 포커스 변동시
//	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);
	
	// 마지막으로 포커스됐던 버튼으로 다시 포커스시킵니다.
	void RefocusOnLastFocusedButton();

	// 기본으로 설정한 버튼으로 포커스시킵니다.
	void FocusToDefaultFocusButton();

	// [ 2020-7-30 : kyu ] 포커스 변경 이벤트는 여기에 기본으로 설정해야하는데, 부가적인 처리가 필요하다면 각 위젯에서 동일 함수를 만들고, WRUIComponentFocus::OnFocusChanged를 호출해야합니다.
	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);

	// 포커스 버튼 클릭 이벤트
	void OnClickedFocusedButton();
};
