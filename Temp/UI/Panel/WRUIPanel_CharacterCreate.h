// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "WRUIPanel_CharacterCreate.generated.h"

UCLASS()
class WR_API UWRUIPanel_CharacterCreate : public UWRUIPanel
{
	GENERATED_BODY()

private:
	enum UIMode 
	{
		GenderSelect,
		CharacterCreate,
	};

	enum EWRChangeOrder
	{
		PreviousOrder,
		NextOrder,
	};

	enum UIGender
	{
		Male,
		Female,
	};

	bool bIsProducing = false;
	bool bButtonClickable = true;
	bool bButtonSecondClickable = true;
	float BlurTime;
	float ProduceTime = 0.0f;
	float EnableClickTimeLeft = 0.0f;
	WRTableID CurrentPresetTID = 1;
	FPostProcessSettings* PostProcessSettings;
	class AWRCharacter* SpawnedPresetCharacter = nullptr;
	UIMode Mode = GenderSelect;
	UIGender Gender = Male;
	//FWREquipCompleteDelegate EquipCompleteDelegate;
	FDelegateHandle ProduceTickHandle;
	
	class UTextBlock* TextBlock_CharacterName;
	class UImage* Image_L1;
	class UImage* Image_R1;
	class UImage* Image_Male;
	class UImage* Image_Female;
	class UCanvasPanel* CanvasPanel_CreateButton;
	class UCanvasPanel* CanvasPanel_CancelButton;
	class UCanvasPanel* CanvasPanel_ChangeName;
	class UWidgetSwitcher* WidgetSwitcher_Mode;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> OKkeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> CancelKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> ModifyNameKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> LeftShiftKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> RightShiftKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* FadeInAnimation;
	UPROPERTY(VisibleAnywhere, Category = "WRUIPanel_CharacterCreate")
		UCurveFloat* CurveEndEffect;
	UPROPERTY(VisibleAnywhere, Category = "WRUIPanel_CharacterCreate")
		UCurveFloat* CurveStartEffect;

private:
	virtual void	NativeConstruct() override;
	virtual void	NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void	NativeDestruct() override;
	virtual void	OnAnimationFinished(const UWidgetAnimation* Animation);
	virtual FReply	NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply	NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void InitializeDOF();
	void TurnDOFoff();									// [ 2020-2-18 : vuvle ] : 현재 사용하지 않음.
	void OnOKKeyClicked();
	void OnOKKeyPressed();
	void ChangeCostume(EWRChangeOrder IN InOrder);
	bool TickProduceStartEffect(float InDeltaTime);
	void OnCancelKeyClicked();
	void OnCancelKeyPressed();
	void OnModifyNameKeyClicked();
	void OnModifyNameKeyPressed();
	void OnL1KeyClicked();
	void OnL1KeyPressed();
	void OnR1KeyClicked();
	void OnR1KeyPressed();
	bool IsButtonClickable() const { return bButtonClickable; }
	bool IsCharacterSpawnNeeded(const WRTableID InCurrentPresetTID, const WRTableID InPresetToChangeTID);
	void OnEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquippedPart);
	WRTableID CalculateNextPresetTID();
	WRTableID CalculatePreviousPresetTID();
	void SwitchGender();
	AWRCharacter* SpawnPresetCharacter(const WRTableID IN InTableID);
	void LoadEffectCurve();
	void UnloadEffectCurve();
	void SetMaterialParameterValue(const float InValue);

public:
	void SetCharacterName(const FString& IN InString);


#if 0 // 더이상 사용되지 않음
private:
	bool TickProduceEndEffect(float InDeltaTime);
public:
	void OnPresetCharacterSpawned(AWRCharacter* IN InSpawnedCharacter);
#endif
};