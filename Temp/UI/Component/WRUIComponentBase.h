// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"



enum class EWRUIComponentType : uint8
{
	Focus,
};

class WR_API WRUIComponentBase
{
protected:
	class UWRUserWidget* OwnerWidget = nullptr;

public:
	WRUIComponentBase();
	~WRUIComponentBase();

	FORCEINLINE void SetOwnerWidget(class UWRUserWidget* IN InUserWidget) { OwnerWidget = InUserWidget; }
	FORCEINLINE class UWRUserWidget* GetOwnerWidget() { return OwnerWidget; }
};
