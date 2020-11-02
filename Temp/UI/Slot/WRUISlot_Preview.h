// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Actor/PreviewActor/WRPreviewStudio.h"
#include "Struct/WRPreviewStruct.h"
#include "WRUISlot_Preview.generated.h"

/**
 
	�� WRPreviewStudio �� ����մϴ�.

	FWRPreviewInfo �� 
	- Type : EWRPreviewType
	- BPPath : String
	- SetPreviewWidget(this)
	�� �ʿ��� ������ �����ϰ� 
	WRPreviewManager::Get()->SetPreview( FWRPreviewInfo ); �� ȣ���մϴ�.

	�� ������� �ʰ� �� ���� WRPreviewManager::Get()->ReleasePreview( this, true/false ); �� ȣ���Ͽ� Studio�� �����մϴ�.

	�� ����ϴ� Widget�� ����� �°� FWRPreviewBaseInfo�� SceneSize�� �����ؾ��մϴ�.

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
