// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "DMUISlot_SlideElement.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUISlot_SlideElement : public UDMUISlot
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Index;	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_TranslationIndex;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Data;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Main;

private:
	int32 Index = 0;
	int32 TranslationIndex = 0;
	bool bIsMain = false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:	
	FORCEINLINE bool IsMain() { return bIsMain; }
	void SetMain(const bool IN InSet);

	FORCEINLINE int32 GetTranslationIndex() { return TranslationIndex; }
	void SetTranslationIndex(const int32 IN InIndex);

	FORCEINLINE int32 GetIndex() { return Index; }
	void SetIndex(const int32 IN InIndex);

	void SetData(const int32 IN InData);
};
