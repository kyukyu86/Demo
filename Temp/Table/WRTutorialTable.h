#pragma once

#include "Base/WRTableRow.h"
#include "Enum/EWRTutorial.h"
#include "Enum/EWRCharacter.h"
#include "WRTutorialTable.generated.h"



USTRUCT(BlueprintType)
struct FWRTutorialInvokePreCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRTutorialPreConditionType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 Value;
};

USTRUCT(BlueprintType)
struct FWRTutorialInvokeCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRTutorialConditionType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRInput Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 Value;
};

USTRUCT(BlueprintType)
struct FWRTutorialCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRTutorialConditionType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRInput Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 RepeatCount;
};

USTRUCT(BlueprintType)
struct FWRTutorialTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TutorialData_Tutorial"); }

	//====================================================================================
	//	Direct Interaction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		EWRTutorialPanel TutorialPanel = EWRTutorialPanel::Main;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString Title; // 튜토리얼 제목

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Menu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 NextTID; // 다음 튜토리얼 (연속성)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool ShowPopup;	// 팝업을 띄울지 (팝업 없이 전투 등 튜토리얼 진행)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Skip; // 현재 튜토리얼 스킵기능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Movable = true;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
 		EWRTutorialType PopupType; // 팝업의 형태

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 PopupHoldingTime;	// 팝업 띄워지는 시간 (0이면 아무키입력으로 꺼짐)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial, meta = (MultiLine = "true"))
		FString PopupDescrition; // 팝업 설명글

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString PopupCanvasImagePath; // 팝업 이미지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString PopupCanvasBPPath; // 팝업 이미지용 애니메이션 있는 WidgetBP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 LimitTime; // 제한시간 내 튜토리얼 진행안할 시 팝업을 다시 띄우기 위한 시간. 팝업이 없으면 LimitReturnTID 로 돌아가서 다시 시작

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 LimitReturnTID;	// 제한시간 내 튜토리얼 진행안할 시 돌아갈 튜토리얼 TID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		TArray<FWRTutorialCondition> Condition; // 튜토리얼 완료를 위한 조건들

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial, meta = (MultiLine = "true"))
		FString CompleteText; // 완료시 알림글

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString StartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString CompleteSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FWRTutorialInvokePreCondition InvokePreCondition; // 튜토리얼 발생을 위한 전제 조건들

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		TArray<FWRTutorialInvokeCondition> InvokeCondition; // 튜토리얼 발생을 위한 조건들

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 VoiceSubCommandTID; 
		
};