// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// Widget Animation
	TMap<FName, UWidgetAnimation*> WidgetAnimationList;

protected:
	UPROPERTY(EditAnywhere, Category = "WRUserWidget", DisplayName = "Use Widget Anim")
		bool bUseWidgetAnimation = false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetFocusableInputMode(const bool IN InUIMode, UWidget* IN InFocusWidget = nullptr);

	// Widget Animation
	UWidgetAnimation* GetAnimationByName(FName IN InAnimationName) const;
	void CollectWidgetAnimation();

public:
	// Widget Animation
	bool PlayAnimationByName(const FName InAnimationName, float StartAtTime = 0.0f, int32 NumLoopsToPlay = 1, EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward, float PlaybackSpeed = 1.0f, bool bRestoreState = false);
	void StopAnimationByName(const FName InAnimationName);
	bool IsAnimationPlayingByName(const FName InAnimationName);
};




// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버변수와의 Widget Binding 을 위한 Macro.
#define BIND_WIDGET(WidgetType, WidgetVariable)																																\
	WidgetVariable = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																							\
	if (WidgetVariable == nullptr)																																			\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
	ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}

// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버변수와의 Widget Binding 을 위한 Macro. (TSharedPtr 사용시)
#define BIND_SHARED_WIDGET(WidgetType, WidgetVariable)																														\
	{ WidgetType* tempWidget = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																					\
	WidgetVariable = MakeShareable(tempWidget);																																\
	if (WidgetVariable.Get() == nullptr)																																	\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
	ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}}

// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버변수와의 Widget Binding 을 위한 Macro. Binding 성공시 구현부 추가 가능
#define BIND_WIDGET_WITH_IMPLEMENTATION(WidgetType, WidgetVariable, ImplementationWhenWidgetCreatedSuccessfully)															\
	WidgetVariable = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																							\
	if (WidgetVariable == nullptr)																																			\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
		ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}	\
	else																																									\
	{	ImplementationWhenWidgetCreatedSuccessfully  }

// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버변수와의 Widget Binding 을 위한 Macro. Binding 성공시 구현부 추가 가능 (TSharedPtr 사용시)
#define BIND_SHARED_WIDGET_WITH_IMPLEMENTATION(WidgetType, WidgetVariable, ImplementationWhenWidgetCreatedSuccessfully)														\
	{ WidgetType* tempWidget = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																					\
	WidgetVariable = MakeShareable(tempWidget);																																\
	if (WidgetVariable.Get() == nullptr)																																	\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
		ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}	\
	else																																									\
	{	ImplementationWhenWidgetCreatedSuccessfully  } }

// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버 변수가 아닌 Widget Binding 을 위한 Macro.
#define BIND_NONMEMBER_WIDGET(WidgetType, WidgetVariable)																													\
	WidgetType* WidgetVariable = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																				\
if (WidgetVariable == nullptr)																																				\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
	ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}

// [8/27/2019 vuvle] : Widget Binding Macro Function. UMG 와 C++ 멤버 변수가 아닌 Widget Binding 을 위한 Macro. Binding 성공시 구현부 추가 가능
#define BIND_NONMEMBER_WIDGET_WITH_IMPLEMENTATION(WidgetType, WidgetVariable, ImplementationWhenWidgetCreatedSuccessfully)													\
	WidgetType* WidgetVariable = Cast<WidgetType>(GetWidgetFromName(TEXT(#WidgetVariable)));																				\
if (WidgetVariable == nullptr)																																				\
	{	FString _TempWidgetTypeName = #WidgetType, _TempWidgetVariableName = #WidgetVariable, _TempFunctionName = __FUNCTION__;												\
	ensureMsgf(false, TEXT("\n%s : <%s> %s binding failed. It might occur client-crash\n"), *_TempFunctionName, *_TempWidgetTypeName, *_TempWidgetVariableName);	}		\
	else																																									\
	{	ImplementationWhenWidgetCreatedSuccessfully  }
