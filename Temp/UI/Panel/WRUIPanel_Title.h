// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_Title.generated.h"


enum class EWRInput : uint8;
class UButton;
UCLASS()
class WR_API UWRUIPanel_Title : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	void OnAnyButtonClicked();
	void SetWaitLoginPacket(const bool IN bInWaitLoginPacket) { bWaitLoginPacket = bInWaitLoginPacket; }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EWRInput> AnyKeys;

	bool bWaitLoginPacket = false;


	//====================================================================================
	// [ 2019-12-11 : kyu ] Test
private:
#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
	void BeginTestMaterial();
	void UpdateTestMaterial(float IN InDeltaTime);
#endif
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UImage* Image_Test1;
	UPROPERTY()
		class UMaterialInstanceDynamic* MateraiInstance = nullptr;
	UPROPERTY()
		class UTextureRenderTarget2D*	pRenderTarget = nullptr;
		class FWidgetRenderer*			pWidgetRenderer = nullptr;
	UPROPERTY()
		class UUserWidget*				pTestWidget = nullptr;
	TSharedPtr<SWidget>					SlateWidget;
	float fOpacity = 0.0f;
};