// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRSlideList.generated.h"

DECLARE_DELEGATE_OneParam(FWRSlideMainChanged, int32);
DECLARE_DELEGATE_OneParam(FWRSlideSideChanged, int32);

UENUM(BlueprintType)
enum class EWRSlideStartType : uint8
{
	Head,
	Middle,
	Tail,
};

enum class EWRSlideMoveType : uint8
{
	None,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FWRSlideElementWidget
{
	GENERATED_BODY();

private:
	FString strAsyncKey = "";
	bool bIsCreateCompleted = false;

public:
	class UOverlay* Overlay = nullptr;
	class UWRSlideElement* SlideElement = nullptr;	

	bool CreateElement(UUserWidget* InOwner, TSubclassOf<class UWRSlideElement> InElementWidget);
	void OnCreateCompleted(UWRUISlot* IN InCreatedSlot, FString InAsyncKey);
	bool IsCreateCompleted() { return bIsCreateCompleted; }
	void ReleaseElement();
};

UCLASS()
class WR_API UWRSlideList : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanelMain;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<FWRSlideElementWidget> ElementList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true", Tooltip = "Element widget class"))
		TSubclassOf<class UWRSlideElement> ElementWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Normal", meta = (AllowPrivateAccess = "true"))
		EWRSlideStartType SlideStartType = EWRSlideStartType::Middle;
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
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Test", meta = (AllowPrivateAccess = "true", ClampMin = "1", Tooltip = "For test"))
		int32 TestDataSize = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlideList|Test", meta = (AllowPrivateAccess = "true", Tooltip = "For test"))
		FVector2D TestElementDesiredSize;

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
	EWRSlideMoveType MoveType = EWRSlideMoveType::None;
	bool IsMoving = false;
	float DstMoveTime = 0.f;
	int32 MoveStep = 0;	
	float AccumulateMoveTime = 0.f;

	bool IsRevertTranslation = false;

	FWRSlideMainChanged OnSlideMainChangedDelegate;		// return Main(Middle) Data Index
	FWRSlideSideChanged OnSlideSideChangedDelegate;		// return Sides(Head,Tail) Data Index

private:
	// Setup
	void Setup();
	void MakeElementList();
	void ReleaseElementList();
	void SetupTransitionList();
	void SetupElementList();

	FORCEINLINE FVector2D GetTranslation(const int32 IN InTranslationIndex)
	{
		if (TranslationList.IsValidIndex(InTranslationIndex))
		{
			return TranslationList[InTranslationIndex];
		}
		return FVector2D::ZeroVector;
	}
	FORCEINLINE int32 GetMainTranslationIndex()
	{
		return (ElementCount - 1) / 2;
	}

	// Move
	void OnMove(const EWRSlideMoveType IN InMoveType);
	void UpdateMove(const float IN InDeltaTime);
	void ReleaseMove();

	// Element
	void UpdateElement(class UWRSlideElement* IN InElement);

	// Test
	void TestDebuggingLog(FString IN InLog);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:	
	FORCEINLINE void SetDataSize(const int32 IN InSize) { DataSize = InSize; }
	FORCEINLINE int32 GetMainDataIndex() { return MainTranslationDataIndex; }
	class UWRSlideElement* GetElement(const int32 IN InTranslationIndex);

	void UpdateElementList(const bool IN InInit);					// 리스트 갱신
	void ChangeElementListByDataIndex(const int32 IN InDataIndex);	// 중앙의 데이터 인덱스를 강제 갱신

	template<class T>
	void GetSlideElementList(TArray<T*>& OUT OutList);
};

template<class T>
void UWRSlideList::GetSlideElementList(TArray<T*>& OUT OutList)
{
	for (auto EachElement : ElementList)
	{
		T* CastedElement = Cast<T>(EachElement.SlideElement);
		if (CastedElement == nullptr)
			return;
		
		OutList.Add(EachElement.SlideElement);
	}
}
