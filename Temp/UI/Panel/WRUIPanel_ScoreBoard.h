// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_ScoreBoard.generated.h"


class UVerticalBox;
UCLASS()
class WR_API UWRUIPanel_ScoreBoard : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
private:
	void Init();
private:
	int32 GetScore(const int8 IN InRank); // 임시 코드
	FName GetName(const int8 IN InRank); // 임시 코드
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName3;
private:
	UVerticalBox* VerticalBox_List;
};