// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../../Enum/DMUIEnum.h"
#include "DMWidgetTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDMWidgetTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EDMPanelKind PanelKind = EDMPanelKind::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EDMWidgetType WidgetType = EDMWidgetType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "WidgetType == EDMWidgetType::WidgetActor"))
		TSubclassOf<class ADMWidgetActorBase> WidgetActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector AddLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRotator AddRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector AddScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "EDMWidgetComponentFlag"))
		int32 Flags = 0;
};
