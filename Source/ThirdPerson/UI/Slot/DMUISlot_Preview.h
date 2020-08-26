// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "../../PreviewStudio/DMPreviewStruct.h"
#include "DMUISlot_Preview.generated.h"


/**

	ㅁ DMPreviewStudio 를 사용합니다.

	FDMPreviewInfo 에
	- Type : EDMPreviewType
	- BPPath : String
	- SetPreviewWidget(this)
	등 필요한 정보를 셋팅하고
	DMPreviewManager::Get()->SetPreview( FDMPreviewInfo ); 를 호출합니다.

	ㅁ 사용하지 않게 될 때는 DMPreviewManager::Get()->ReleasePreview( this, true/false ); 를 호출하여 Studio를 해제합니다.

	ㅁ 사용하는 Widget의 사이즈에 맞게 FDMPreviewBaseInfo의 SceneSize를 수정해야합니다.

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
