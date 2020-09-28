// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Base/DMUISlot.h"
#include "DMUISlideElement.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUISlideElement : public UDMUISlot
{
	GENERATED_BODY()

	friend class UDMUISlideList;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Index;	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_TranslationIndex;	

private:
	int32 Index = 0;
	int32 TranslationIndex = 0;
	bool bIsMain = false;

protected:
	class UDMUISlideList* ParentSlideListWidget = nullptr;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:	
	FORCEINLINE bool IsMain() { return bIsMain; }
	virtual void SetMain(const bool IN InSet);

	FORCEINLINE int32 GetTranslationIndex() { return TranslationIndex; }
	void SetTranslationIndex(const int32 IN InIndex);

	FORCEINLINE int32 GetIndex() { return Index; }	// 현재로서는 무쓸모
	void SetIndex(const int32 IN InIndex);

	virtual void SetData(const int32 IN InDataIndex)	{}
	virtual void SetEmpty()								{}
	   


	// Test
	void SetParent(class UDMUISlideList* InParent) { ParentSlideListWidget = InParent; }
};