// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Table/WRCharacterPresetTable.h"
#include "WRUIPanel_CostumeChange.generated.h"

UCLASS()
class WR_API UWRUIPanel_CostumeChange : public UWRUIPanel
{
	GENERATED_BODY()

public:
	enum EWRChangeOrder
	{
		PreviousOrder,
		NextOrder,
	};
	//enum CostumeGender
	//{
	//	Man,
	//	Woman,
	//};

private:
	WRTableID CurrentPresetTID = 1;
	WRTableID LastPresetTID = 0;
	EWRCharacterGender Gender = EWRCharacterGender::Male_A;
	class UTextBlock* TextBlock_CharacterName;
	class AWREnvPresetCharacter* ParentPreset = nullptr;
	class AWRCharacter* SpawnedPresetCharacter = nullptr;
	float EnableClickTimeLeft = 0.0f;
	bool bButtonClickable = true;

	class UImage* Image_L1;
	class UImage* Image_R1;
	class UCanvasPanel* CanvasPanel_ChangeButton;
	class UCanvasPanel* CanvasPanel_CancelButton;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> OKkeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> CancelKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> LeftShiftKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> RightShiftKeys;

private:
	virtual void	NativeConstruct() override;
	virtual void	NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply	NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply	NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void	NativeDestruct() override;

	bool ChangeCostume(const EWRChangeOrder IN InChangeOrder);
	bool IsRespawnRequired(const WRTableID IN InCurrentTID, const WRTableID IN InChangingTargetTID);
	void SaveChangedPresetTIDToParent(const WRTableID IN InTID);
	WRTableID CalculateNextPresetTID();
	WRTableID CalculatePreviousPresetTID();
	void GetPresetStartEndTID(int8& OUT OutStartTID, int8& OUT OutEndTID);
	void SetLastTIDFromPresetCharacter();

public:
	void SetCurrentPresetTID(const WRTableID IN InTID);
	void SetGender(const EWRCharacterGender IN InGender) { Gender = InGender; }
	void SetPresetCharacter(AWRCharacter* IN InCharacter) { SpawnedPresetCharacter = InCharacter; SetLastTIDFromPresetCharacter(); }
	void SetParentEnvPreset(class AWREnvPresetCharacter* IN InParentPreset) { ParentPreset = InParentPreset; }

	void OnOKKeyClicked();
	void OnOKKeyPressed();
	void OnCancelKeyClicked();
	void OnCancelKeyPressed();
	void OnL1KeyClicked();
	void OnL1KeyPressed();
	void OnR1KeyClicked();
	void OnR1KeyPressed();
};