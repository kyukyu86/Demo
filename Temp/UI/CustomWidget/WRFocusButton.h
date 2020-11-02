// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUserWidget.h"

#include "Image.h"
#include "Button.h"
#include "TextBlock.h"

#include "WRFocusButton.generated.h"


DECLARE_DELEGATE_OneParam(FOnClickedFocusButton, int32)
DECLARE_DELEGATE_TwoParams(FOnFocusChanged, class UWRFocusButton*, bool)


UENUM(BlueprintType)
enum class EWRFocusButtonState : uint8
{
	None,
	Focus,
	Select,
};

UENUM(BlueprintType)
enum class EWRFocusUITypeInFocusButton : uint8 
{ 
	Change,
	Add 
};

USTRUCT(BlueprintType)
struct FWRFocusButtonManualInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FSlateBrush FocusBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FSlateBrush NotFocusBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FSlateBrush SelectBrush;	
};



UCLASS()
class WR_API UWRFocusButton : public UWRUserWidget
{
	GENERATED_BODY()

private:
	bool bFocused = false;
	bool bUseTick = true;

	//====================================================================================
	// [ 2020-1-28 : kyu ] 커스텀 변수들
	int32 nIndex = 0;	// 주로 Numbering에 쓰입니다
	int32 nValue = 0;	// 주로 추가 정보(값)에 쓰입니다

	float MaxScaleForZoom;
	float CurScaleForZoom;
	float ZoomSpeed;

public:
	FOnClickedFocusButton OnClicked;		// 클릭 이벤트에 대해서 미리 설정한 nIndex를 리턴받고자 한다면 여기에 Delegate를 연결하세요

											// 일반 클릭이벤트는 GetButton()->OnClicked에 바인딩하면 됩니다
	FOnFocusChanged OnFocusChanged;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Focus;
	UPROPERTY(BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_NotFocus;
	UPROPERTY(BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Select;
	UPROPERTY(BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BindWidget))
		UButton* Button_Contents;
	UPROPERTY(BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Contents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		FWRFocusButtonManualInfo FocusButtonManualInfo;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		EWRFocusButtonState TestState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		EWRFocusUITypeInFocusButton FocusUIType = EWRFocusUITypeInFocusButton::Change;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Zoom")
		bool bZoomInFocus = false;	// 포커스 될떄 확대 기능 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Zoom", meta = (EditCondition = "bZoomInFocus"))
		float ZoomInRate = 1.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Zoom", meta = (EditCondition = "bZoomInFocus"))
		float ZoomInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Focus")
		bool bNotUsedNotFocusImg = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Focus")
		bool bUseFocusAnim = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Focus")
		float FocusAnimPlayBackSpeed = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Font")
		FText DefaultString = FText::FromString("-");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Font")
		FString FontStyle = "Default";
	
private:
	void Tick_Zoom(const float IN InDeltaTime);

public:	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual void SynchronizeProperties() override;

	// Set
	virtual void SetContents(FString IN InContents);
	void SetSelect(const bool IN InSelect);
	void SetFocusImg(UTexture2D* IN InTexture);
	void SetNotFocusImg(UTexture2D* IN InTexture);
	void SetSelectImg(UTexture2D* IN InTexture);
	void SetFocusUIType(const EWRFocusUITypeInFocusButton IN InFocusUIType) { FocusUIType = InFocusUIType; }
	void SetFocusChangedDelegate(FOnFocusChanged& IN InDelegate);
	void SetButtonContentAlignment(EHorizontalAlignment InHorizontalAlignment);

	// Brush
	void SetFocusBrush(const FSlateBrush& IN InBrush);
	void SetNotFocusBrush(const FSlateBrush& IN InBrush);
	void SetSelectBrush(const FSlateBrush& IN InBrush);

	// [ 2020-1-31 : kyu ] 키조작으로 인한 포커싱은 Tick에서 HasAnyUserFocus으로 체크를 해야하지만
	// WidgetInteraction을 쓰는 경우에는 HasAnyUserFocus로 체크가 불가능하여 Button의 OnHovered, OnUnHovered로 포커싱여부 체크해야한다.
	// 따라서 Tick으로 체크를 쓸 지 말지를 각 UI에서 판단하여 On/Off 해야한다.
	FORCEINLINE void SetTickFocusCheck(const bool InSet) { bUseTick = InSet; }

	UFUNCTION(BlueprintCallable, Category = "UWRFocusButton")
		UButton* GetButton() { return Button_Contents; }

	UFUNCTION(BlueprintCallable, Category = "UWRFocusButton")
		void OnClickedButton();
	UFUNCTION(BlueprintCallable, Category = "UWRFocusButton")
		void OnHoveredButton();
	UFUNCTION(BlueprintCallable, Category = "UWRFocusButton")
		void OnUnHoveredButton();

	//====================================================================================
	// [ 2020-1-28 : kyu ] 커스텀 함수
	void SetIndex(const int32 IN InIndex) { nIndex = InIndex; }
	int32 GetIndex() { return nIndex; }
	void SetValue(const int32 IN InValue) { nValue = InValue; }
	int32 GetValue() { return nValue; }

};
