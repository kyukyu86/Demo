// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_Video.generated.h"




UCLASS()
class WR_API UWRUIPanel_Video : public UWRUIPanel
{
	GENERATED_BODY()
	
public:

	class UMaterialInstanceDynamic* VideoDynamicMaterial;
	class UImage* Image_Video;
	float TotalTIme;
public:
	virtual void NativeConstruct() override;

public:
	void UpdateMaterial(const float IN InDeltaTime);
	UMaterialInstanceDynamic* GetDynamicMaterial();
};