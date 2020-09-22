// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "DMUISlot_SlideList.generated.h"

DECLARE_DELEGATE_OneParam(FDMSlideMainChanged, int32);
DECLARE_DELEGATE_OneParam(FDMSlideSideChanged, int32);

UENUM(BlueprintType)
enum class EDMSlideStartType : uint8
{
	Head,
	Middle,
	Tail,
};

enum class EDMSlideMoveType : uint8
{
	None,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FDMSlideElementWidget
{
	GENERATED_BODY();

	class UOverlay* Overlay = nullptr;
	class UDMUISlot_SlideElement* SlideElement = nullptr;	

	bool CreateElement(UUserWidget* InOwner, const FString IN InElementWidgetPath);
};

// Test
USTRUCT(BlueprintType)
struct FDMCustomSlideData
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ImagePath;
};

UCLASS()
class THIRDPERSON_API UDMUISlot_SlideList : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanelMain;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<FDMSlideElementWidget> ElementList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true", Tooltip = "Element widget reference full path"))
		FString ElementWidgetPath = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true"))
		EDMSlideStartType SlideStartType = EDMSlideStartType::Middle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
		int32 ElementCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
		float ElementSpace = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|MainSlot", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
		float MainScale = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|MainSlot", meta = (AllowPrivateAccess = "true", Tooltip = "Main slot translation offset"))
		float MainOffset = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|MainSlot", meta = (AllowPrivateAccess = "true", Tooltip = "For test"))
		float MainCalcScale = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|MainSlot", meta = (AllowPrivateAccess = "true", Tooltip = "Scaling curve float"))
		UCurveFloat* MainScaleCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Move", meta = (AllowPrivateAccess = "true", ClampMin = "0.2"))
		float NormalMoveTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Move", meta = (AllowPrivateAccess = "true", ClampMin = "0.2", Tooltip = "When repeat input"))
		float RepeatMoveTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Move", meta = (AllowPrivateAccess = "true", ClampMin = "0.2", Tooltip = "For change ZOrder"))
		float ChangeMainTime = 0.4f;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Test", meta = (AllowPrivateAccess = "true", ClampMin = "0.2", Tooltip = "For test"))
		TArray<FDMCustomSlideData> TestDataList;	

private:
	bool IsSetted = false;		
	bool IsMadeList = false;
	// Translation
	int32 NextMainTranslationIndex = 0;
	TArray<FVector2D> TranslationList;
	// Data
	int32 StartingDataIndex = 0;
	int32 MainTranslationDataIndex = 0;
	int32 DataSize = 0;
	// Move
	EDMSlideMoveType MoveType = EDMSlideMoveType::None;
	bool IsMoving = false;
	float DstMoveTime = 0.f;
	int32 MoveStep = 0;	
	float AccumulateMoveTime = 0.f;

	bool IsRevertTranslation = false;

	FDMSlideMainChanged OnSlideMainChangedDelegate;		// return Main(Middle) Data Index
	FDMSlideSideChanged OnSlideSideChangedDelegate;		// return Sides(Head,Tail) Data Index

private:
	void Setup();
	void MakeElementList();
	void SetupTransitionList();
	void SetupElementList();

	class UDMUISlot_SlideElement* GetElement(const int32 IN InTranslationIndex);
	FVector2D GetTranslation(const int32 IN InTranslationIndex);	

	void OnMove(const EDMSlideMoveType IN InMoveType);
	void UpdateMove(const float IN InDeltaTime);
	void ReleaseMove();

	void UpdateElement(class UDMUISlot_SlideElement* IN InElement);
	void UpdateElementList(const bool IN InInit);					// ����Ʈ ����
	void ChangeElementListByDataIndex(const int32 IN InDataIndex);	// �߾��� ������ �ε����� ���� ����

	FORCEINLINE int32 GetMainTranslationIndex()
	{
		return (ElementCount - 1) / 2;
	}

	void TestDebuggingLog(FString IN InLog);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

	// Test
	TArray<FDMCustomSlideData>& GetTestData() { return TestDataList; }
};
