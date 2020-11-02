// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/WRTypeDefine.h"
#include "UI/Component/WRUIComponentFocus.h"
#include "WRUserWidget.generated.h"



UCLASS()
class WR_API UWRUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// Widget Animation
	TMap<FName, UWidgetAnimation*> WidgetAnimationList;

	// UI Components
	TMap<EWRUIComponentType, WRUIComponentBase*> UIComponentList;

protected:
	UPROPERTY(EditAnywhere, Category = "WRUserWidget", DisplayName = "Use Widget Anim")
		bool bUseWidgetAnimation = false;

protected:	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);

	// Widget Animation
	UWidgetAnimation* GetAnimationByName(FName IN InAnimationName) const;
	void CollectWidgetAnimation();

	// UI Components
	template<class T>
	T* AddUIComponent(const EWRUIComponentType IN InUIComponentType);
	template<class T>
	T* FindUIComponent(const EWRUIComponentType IN InUIComponentType);
	bool IsContainedUIComponent(const EWRUIComponentType IN InUIComponentType);

public:
	void SetFocusableInputMode(const bool IN InUIMode, UWidget* IN InFocusWidget = nullptr);
	
	// Widget Animation
	bool PlayAnimationByName(const FName InAnimationName, float StartAtTime = 0.0f, int32 NumLoopsToPlay = 1, EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward, float PlaybackSpeed = 1.0f, bool bRestoreState = false);
	void StopAnimationByName(const FName InAnimationName);
	bool IsAnimationPlayingByName(const FName InAnimationName);

	// Focus
	void OnClickedFocusedButton();		// 포커스된 버튼에 클릭 이벤트 호출
	void RefocusOnLastFocusedButton();	// 마지막으로 포커스된 버튼에 재포커싱
	void FocusToDefaultFocusButton();	// 기본 포커스버튼으로 포커싱
	class UWRFocusButton* GetDefaultFocusButton();

};




template<class T>
T* UWRUserWidget::AddUIComponent(const EWRUIComponentType IN InUIComponentType)
{
	if (IsContainedUIComponent(InUIComponentType))
	{
		return nullptr;
	}

	WRUIComponentBase* NewUIComponent = nullptr;

	switch (InUIComponentType)
	{
	case EWRUIComponentType::Focus:
	{
		NewUIComponent = new WRUIComponentFocus;
	}
	break;
	}

	if (NewUIComponent != nullptr)
	{
		NewUIComponent->SetOwnerWidget(this);
		UIComponentList.Add(InUIComponentType, NewUIComponent);
		return static_cast<T*>(NewUIComponent);
	}
	return nullptr;
}

template<class T>
T* UWRUserWidget::FindUIComponent(const EWRUIComponentType IN InUIComponentType)
{
	WRUIComponentBase* UIComponentBase = UIComponentList.FindRef(InUIComponentType);
	if (UIComponentBase == nullptr)
		return nullptr;

	return static_cast<T*>(UIComponentBase);
}
