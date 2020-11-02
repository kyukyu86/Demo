// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoftObjectPtr.h"
#include "Enum/EWRPreLoad.h"
#include "WRPreLoadAsset.generated.h"

USTRUCT(BlueprintType)
struct FWRFolderInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FString FolderPath;
	UPROPERTY(EditAnywhere)
		EWRPreLoadKind Kind;
};
/**
 * 
 */
UCLASS()
class WR_API UWRPreLoadAsset : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Character")
	TArray<TSoftObjectPtr<UObject>>	Characters;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TArray<TSoftObjectPtr<UObject>>	Effects;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TArray<TSoftObjectPtr<UObject>>	Sounds;

	UPROPERTY(EditAnywhere, Category = "Folder")
	TArray<FWRFolderInfo>	Folders;
};
