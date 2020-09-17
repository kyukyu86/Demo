// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "DMUISlot_SlideList.generated.h"

UENUM(BlueprintType)
enum class EDMSlideStartType : uint8
{
	Head,
	Middle,
	Tail,
};

UCLASS()
class THIRDPERSON_API UDMUISlot_SlideList : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanelMain;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EDMSlideStartType SlideStartType = EDMSlideStartType::Middle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "1"))
		int32 ElementCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0"))
		float ElementSpace = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "1"))
		float MainScale = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.2"))
		float NormalMoveTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.2"))
		float RepeatMoveTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.2"))
		float ChangeMainTime = 0.4f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<class UDMUISlot_SlideElement*> ElementList;

private:
	bool IsSetted = false;	
	TArray<FVector2D> TranslationList;

	int32 MainTranslationIndex = 0;
	int32 NextMainTranslationIndex = 0;
	int32 MainTranslationDataIndex = 0;
	TArray<int32> DataList;

	// Move
	bool IsMove = false;
	bool IsLeft = false;
	bool IsMoveComplete = false;
	float DstMoveTime = 0.f;
	int32 MoveStep = 0;	
	float AccumulateMoveTime = 0.f;

private:
	void Setup();
	void SetupTransitionList();
	void SetupElementList();

	FVector2D GetTranslation(const int32 IN InTranslationIndex);

	void OnMove(const bool IN InLeft);
	void UpdateMove(const float IN InDeltaTime);

	void UpdateData();
	class UDMUISlot_SlideElement* GetElement(const int32 IN InTranslationIndex);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

};
