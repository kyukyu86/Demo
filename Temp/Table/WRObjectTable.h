#pragma once
#include "Base/WRTableRow.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Enum/EWRCharacter.h"
#include "WRObjectTable.generated.h"


//#define DEF_ADD_ZIPLINE

UENUM(BlueprintType)
enum class EWRIOMainType : uint8
{
	None,
	Interaction,
	ActionProp,
	Brick,

	Research,
};

// UENUM(BlueprintType)
// enum class EWRIOSubType : uint8
// {
// 	None,
// 	Interaction,
// 	ZiplineHandle,
// };

UENUM(BlueprintType)
enum class EWRIODirectType : uint8
{
	None,
	KeyInput,
};

UENUM(BlueprintType)
enum class EWRIOResultAction : uint8
{
	None,
	Drop,
	Trigger,
	SingleTrigger,
};

UENUM(BlueprintType)
enum class EWRIOResultAfter : uint8
{
	None,
	Retain,
	RetainHide,
	Delete,
};

UENUM(BlueprintType)
enum class EWRAPResponseType : uint8
{
	None,
	Trigger,
	TOD,
};

USTRUCT(BlueprintType)
struct FWRObjectBaseTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		FString NameLocalkey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		EWRIOMainType MainType;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		EWRIOSubType SubType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		FString MainBPPath;
};

USTRUCT(BlueprintType)
struct FWRInteractionObjectTable : public FWRObjectBaseTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ObjectData_InteractionObject"); }
		
	//====================================================================================
	//	Direct Interaction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		int32 DirectConditionQuestProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		EWRIODirectType DirectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		EWRInput DirectKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		float DirectKeyPressTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		FString DirectKeyIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		FString DirectKeyDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		FLinearColor DirectKeyFontColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		EWRIOResultAction DirectResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		int32 DirectResultID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		EWRIOResultAfter DirectResultAfter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		int32 DirectRetainCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
		bool DirectOccupiable;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DirectInformation)
// 		int32 ZiplineUniqueKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DirectInformation)
		FString SpawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DirectInformation)
		FString SpawnSoundPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DirectInformation)
		FString UnspawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DirectInformation)
		FString UnspawnSoundPath;

	//====================================================================================
	//	Hook Interaction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		EWRHookType HookType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FString HookKeyIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FString HookKeyDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FString HookKeyActionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FString HookKeyActionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		EWRIOResultAction HookResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 HookResultID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		EWRIOResultAfter HookResultAfter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 HookRetainCount;
	//====================================================================================

};

USTRUCT(BlueprintType)
struct FWRActionPropTable : public FWRObjectBaseTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ObjectData_ActionProp"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWRAPResponseType ResponseType = EWRAPResponseType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TODStartHour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TODFinishHour;
};

USTRUCT(BlueprintType)
struct FWRBrickTable : public FWRObjectBaseTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ObjectData_Brick"); }

};
