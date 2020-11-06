// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "../../Enum/DMActorEnum.h"
#include "DMUIPanel_SkillNotice.generated.h"


class UImage;
class UWidgetSwitcher;
class UCanvasPanel;

UCLASS()
class THIRDPERSON_API UDMUIPanel_SkillNotice : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UCanvasPanel* CanvasPanel_MouseClick;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UWidgetSwitcher* WidgetSwitcher_MouseClickType;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	void SetType(const EDMSkillNotice IN InType);
	void ShowOff();
};
