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
		FString Title; // Ʃ�丮�� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Menu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 NextTID; // ���� Ʃ�丮�� (���Ӽ�)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool ShowPopup;	// �˾��� ����� (�˾� ���� ���� �� Ʃ�丮�� ����)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Skip; // ���� Ʃ�丮�� ��ŵ���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		bool Movable = true;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
 		EWRTutorialType PopupType; // �˾��� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 PopupHoldingTime;	// �˾� ������� �ð� (0�̸� �ƹ�Ű�Է����� ����)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial, meta = (MultiLine = "true"))
		FString PopupDescrition; // �˾� �����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString PopupCanvasImagePath; // �˾� �̹���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString PopupCanvasBPPath; // �˾� �̹����� �ִϸ��̼� �ִ� WidgetBP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 LimitTime; // ���ѽð� �� Ʃ�丮�� ������� �� �˾��� �ٽ� ���� ���� �ð�. �˾��� ������ LimitReturnTID �� ���ư��� �ٽ� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 LimitReturnTID;	// ���ѽð� �� Ʃ�丮�� ������� �� ���ư� Ʃ�丮�� TID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		TArray<FWRTutorialCondition> Condition; // Ʃ�丮�� �ϷḦ ���� ���ǵ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial, meta = (MultiLine = "true"))
		FString CompleteText; // �Ϸ�� �˸���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString StartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FString CompleteSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		FWRTutorialInvokePreCondition InvokePreCondition; // Ʃ�丮�� �߻��� ���� ���� ���ǵ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		TArray<FWRTutorialInvokeCondition> InvokeCondition; // Ʃ�丮�� �߻��� ���� ���ǵ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		int32 VoiceSubCommandTID; 
		
};