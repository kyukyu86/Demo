// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_Notice.generated.h"


UCLASS()
class WR_API UWRUIPanel_Notice : public UWRUIPanel
{
	GENERATED_BODY()

private:
	class UWRTextBlock* WRTextBlock_Notice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_Notice", meta = (AllowPrivateAccess = "true"))
		float ShowOnTime = 3.0f;

	TQueue<FString> Notices;
	float fShowDuringTime = 0.0f;
	bool bShowOff = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void AddNotice(const FString& IN InNotice);

private:
	bool GetNextNotice(FString& OUT OutNotice);
	void ShowOnNotice(FString IN InNotice);
	void ShowOffNotice();
};