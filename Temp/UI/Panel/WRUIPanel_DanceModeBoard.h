// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_DanceModeBoard.generated.h"

/**
 * 
 */

class UImage;
UCLASS()
class WR_API UWRUIPanel_DanceModeBoard : public UWRUIPanel
{
	GENERATED_BODY()
public:

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
public:
	void SetData(UTexture2D* IN InStartTexture, UTexture2D* IN InKeepTexture);
	void SetData(const FString& IN InStartTexturePath, const FString& IN InKeepTexturePath);
private:
	void HideStartImg();

	void OnLoadCompletedForStartTexture(UObject* IN InStartTexture, FString IN InHash);
	void OnLoadCompletedForKeepTexture(UObject* IN InKeepTexture, FString IN InHash);

private:
	UImage* Image_Start;
	UImage* Image_Keep;

	float RemainTime;
	FString AsyncLoadHashForStartTexture;
	FString AsyncLoadHashForKeepTexture;
};
