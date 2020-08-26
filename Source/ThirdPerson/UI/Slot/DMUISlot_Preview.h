// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "../../PreviewStudio/DMPreviewStruct.h"
#include "DMUISlot_Preview.generated.h"


/**

	�� DMPreviewStudio �� ����մϴ�.

	FDMPreviewInfo ��
	- Type : EDMPreviewType
	- BPPath : String
	- SetPreviewWidget(this)
	�� �ʿ��� ������ �����ϰ�
	DMPreviewManager::Get()->SetPreview( FDMPreviewInfo ); �� ȣ���մϴ�.

	�� ������� �ʰ� �� ���� DMPreviewManager::Get()->ReleasePreview( this, true/false ); �� ȣ���Ͽ� Studio�� �����մϴ�.

	�� ����ϴ� Widget�� ����� �°� FDMPreviewBaseInfo�� SceneSize�� �����ؾ��մϴ�.

 */

UCLASS()
class THIRDPERSON_API UDMUISlot_Preview : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FDMPreviewDataWidget	PreviewData;
	
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

	FORCEINLINE FDMPreviewDataWidget& GetPreviewData() { return PreviewData; }

	void OnShow();

	void OnRotateToLeft();
	void OnRotateToRight();
	void OnRotateEnd();
	void OnZoomInStart();
	void OnZoomOutStart();
	void OnZoomEnd();
};
