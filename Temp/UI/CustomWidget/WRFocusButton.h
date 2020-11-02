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
	// [ 2020-1-28 : kyu ] Ŀ���� ������
	int32 nIndex = 0;	// �ַ� Numbering�� ���Դϴ�
	int32 nValue = 0;	// �ַ� �߰� ����(��)�� ���Դϴ�

	float MaxScaleForZoom;
	float CurScaleForZoom;
	float ZoomSpeed;

public:
	FOnClickedFocusButton OnClicked;		// Ŭ�� �̺�Ʈ�� ���ؼ� �̸� ������ nIndex�� ���Ϲް��� �Ѵٸ� ���⿡ Delegate�� �����ϼ���

											// �Ϲ� Ŭ���̺�Ʈ�� GetButton()->OnClicked�� ���ε��ϸ� �˴ϴ�
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
		bool bZoomInFocus = false;	// ��Ŀ�� �ɋ� Ȯ�� ��� ��� ����
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

	// [ 2020-1-31 : kyu ] Ű�������� ���� ��Ŀ���� Tick���� HasAnyUserFocus���� üũ�� �ؾ�������
	// WidgetInteraction�� ���� ��쿡�� HasAnyUserFocus�� üũ�� �Ұ����Ͽ� Button�� OnHovered, OnUnHovered�� ��Ŀ�̿��� üũ�ؾ��Ѵ�.
	// ���� Tick���� üũ�� �� �� ������ �� UI���� �Ǵ��Ͽ� On/Off �ؾ��Ѵ�.
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
	// [ 2020-1-28 : kyu ] Ŀ���� �Լ�
	void SetIndex(const int32 IN InIndex) { nIndex = InIndex; }
	int32 GetIndex() { return nIndex; }
	void SetValue(const int32 IN InValue) { nValue = InValue; }
	int32 GetValue() { return nValue; }

};
