// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "UI/CustomWidget/ScrollBox/WRUIScrollBoxUtil.h"
#include "WRUIPanel_Status.generated.h"




UCLASS()
class WR_API UWRUIPanel_Status : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	WRUIScrollBoxUtil ScrollBoxUtil;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	
	static void RefreshCallback(void* _Ptr);
	void UpdateScrollBox();
};