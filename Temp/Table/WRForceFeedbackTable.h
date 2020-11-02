// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Base/WRTableRow.h"
#include <GameFramework/ForceFeedbackEffect.h>

#include "WRForceFeedbackTable.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FWRForceFeedbackTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ForceFeedbackData_Pad"); }

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	//	TSoftObjectPtr<UForceFeedbackEffect> Forcefeedbackfile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		FString Forcefeedbackfile;

 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
 		bool Looping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		bool Playwhilepaused;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		FName Tag;
};
