// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

#include "TextBlock.h"
#include "RichTextBlock.h"
#include "CanvasPanel.h"
#include "Image.h"
#include "WidgetSwitcher.h"
#include "UI/CustomWidget/WRTextBlock.h"

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


#define INVALID_UI_HANDLE 0