// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRUIComponentBase.h"
#include "WidgetPath.h"
#include "Events.h"

/*

	WRUIComponentFocus �� Panel, Slot �ȿ��� FocusButton�� ��Ŀ�� ������ �����ϱ� ���ؼ� ��������ϴ�.
	�����ϱ⸦ ���ϴ� ��� FocusButton�� ������ ���� ���ε� ������մϴ�.

	�Ϲ������� Ŭ���̺�Ʈ�� �߻���Ű���� �� Component�� ����� �ʿ䰡 �����ϴ�.

	- DefaultFocusedButton : Panel, Slot �ȿ��� �⺻���� ���� Button �Դϴ�

		UIComponentFocus->SetFocusButton( FocusButton , true);	// true�� Default���� �����ϰ� �˴ϴ�.

	- LastFocusedButton : Panel, Slot �ȿ��� ���������� ��Ŀ�� �� Button �Դϴ�.

	- ��Ŀ�� ���� �̺�Ʈ (�⺻)
	FOnFocusChanged Delegate;
	Delegate.BindUObject( WRUIComponentFocus , &WRUIComponentFocus::OnFocusChanged);	//  WRUIComponentFocus::OnFocusChanged ����
	FocusButton->SetFocusChangedDelegate(Delegate);

	- ��Ŀ�� ���� �̺�Ʈ (Ŀ����) : �� Widget���� ��Ŀ�� ������ ������ ��Ÿ ó���� �־ ������ �˰��� �Ѵٸ�
	FOnFocusChanged Delegate;
	Delegate.BindUObject( Widget , &Widget::OnFocusChanged);	//  �� Widget���� ���� �Լ��� OnFocusChanged�� ����� ���ο��� WRUIComponentFocus::OnFocusChanged�� �Բ� ȣ��������մϴ�.
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

	// ���������� ��Ŀ�� �� ��ư
	FORCEINLINE class UWRFocusButton* GetFocusedButton() { return LastFocusedButton; }
	void SetFocusButton(class UWRFocusButton* IN InFocusButton, const bool IN InDefault = false);

	// ��Ŀ�� ������
//	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);
	
	// ���������� ��Ŀ���ƴ� ��ư���� �ٽ� ��Ŀ����ŵ�ϴ�.
	void RefocusOnLastFocusedButton();

	// �⺻���� ������ ��ư���� ��Ŀ����ŵ�ϴ�.
	void FocusToDefaultFocusButton();

	// [ 2020-7-30 : kyu ] ��Ŀ�� ���� �̺�Ʈ�� ���⿡ �⺻���� �����ؾ��ϴµ�, �ΰ����� ó���� �ʿ��ϴٸ� �� �������� ���� �Լ��� �����, WRUIComponentFocus::OnFocusChanged�� ȣ���ؾ��մϴ�.
	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);

	// ��Ŀ�� ��ư Ŭ�� �̺�Ʈ
	void OnClickedFocusedButton();
};
