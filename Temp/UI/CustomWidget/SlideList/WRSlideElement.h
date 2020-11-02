// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRSlideElement.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRSlideElement : public UWRUISlot
{
	GENERATED_BODY()

	friend class UWRSlideList;

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
	class UWRSlideList* ParentSlideListWidget = nullptr;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:	
	FORCEINLINE bool IsMain() { return bIsMain; }
	FORCEINLINE int32 GetTranslationIndex() { return TranslationIndex; }
	FORCEINLINE int32 GetIndex() { return Index; }		// 현재로서는 무쓸모
	void SetTranslationIndex(const int32 IN InIndex);
	void SetIndex(const int32 IN InIndex);

	//========================================
	// Need to Override 3 Functions
	FORCEINLINE virtual void SetMain(const bool IN InSet) { bIsMain = InSet; }
	FORCEINLINE virtual void SetData(const int32 IN InDataIndex) {}
	FORCEINLINE virtual void SetEmpty() {}
	//========================================


	// Test
	void SetParent(class UWRSlideList* InParent) { ParentSlideListWidget = InParent; }
};