// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>
#include "DMUIFocusButton.generated.h"

class UButton;
class UImage;
class UTextBlock;

UENUM(BlueprintType)
enum class EDMFocusButtonFocusType : uint8
{
	Change,
	Cover,
};

UENUM(BlueprintType)
enum class EDMFocusButtonTestType : uint8
{
	Normal,
	Focus,
	Select,
};

UCLASS()
class THIRDPERSON_API UDMUIFocusButton : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Normal;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Focus;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Select;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UButton* Button_Contents;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UTextBlock* TextBlock_Contents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Brush", meta = (AllowPrivateAccess = "true"))
		FSlateBrush NormalBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Brush", meta = (AllowPrivateAccess = "true"))
		FSlateBrush FocusBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Brush", meta = (AllowPrivateAccess = "true"))
		FSlateBrush SelectBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Brush", meta = (AllowPrivateAccess = "true"))
		EDMFocusButtonFocusType FocusType = EDMFocusButtonFocusType::Change;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Contents", meta = (AllowPrivateAccess = "true"))
		FString DefaultString = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Test", meta = (AllowPrivateAccess = "true"))
		EDMFocusButtonTestType TestType = EDMFocusButtonTestType::Normal;

private:
	bool bIsFocus = false;
	bool bIsSelect = false;

private:
	void UpdateState();

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintCallable, Category = "UDMFocusButton")
		void OnClickedButton();
	UFUNCTION(BlueprintCallable, Category = "UDMFocusButton")
		void OnHoveredButton();
	UFUNCTION(BlueprintCallable, Category = "UDMFocusButton")
		void OnUnHoveredButton();

	void SetSelect(const bool IN InSet);
};
