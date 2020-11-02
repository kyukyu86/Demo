// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Actor/PreviewActor/WRPreviewStudio.h"
#include "Struct/WRPreviewStruct.h"
#include "WRUISlot_Preview.generated.h"

/**
 
	ㅁ WRPreviewStudio 를 사용합니다.

	FWRPreviewInfo 에 
	- Type : EWRPreviewType
	- BPPath : String
	- SetPreviewWidget(this)
	등 필요한 정보를 셋팅하고 
	WRPreviewManager::Get()->SetPreview( FWRPreviewInfo ); 를 호출합니다.

	ㅁ 사용하지 않게 될 때는 WRPreviewManager::Get()->ReleasePreview( this, true/false ); 를 호출하여 Studio를 해제합니다.

	ㅁ 사용하는 Widget의 사이즈에 맞게 FWRPreviewBaseInfo의 SceneSize를 수정해야합니다.

 */

UCLASS()
class WR_API UWRUISlot_Preview : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FWRPreviewDataWidget	PreviewData;

private:
	void OnExit();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	FORCEINLINE FWRPreviewDataWidget& GetPreviewData() { return PreviewData; }

	void OnShow();

	void OnRotateToLeft();
	void OnRotateToRight();
	void OnRotateEnd();
	void OnZoomInStart();
	void OnZoomOutStart();
	void OnZoomEnd();
};
