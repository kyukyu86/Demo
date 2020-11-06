// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_SkillNotice.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include <Components/CanvasPanel.h>
#include "../../Manager/DMUIManager.h"



void UDMUIPanel_SkillNotice::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimationByName("ShowOn");
}

void UDMUIPanel_SkillNotice::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUIPanel_SkillNotice::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	FName AnimName = Animation->MovieScene->GetFName();
	if (AnimName == "ShowOff")
	{
		DMUIManager::Get()->ClosePanel(EDMPanelKind::SkillNotice);
	}
}

void UDMUIPanel_SkillNotice::SetType(const EDMSkillNotice IN InType)
{
	switch (InType)
	{
	case EDMSkillNotice::MouseL:
	case EDMSkillNotice::MouseR:
	{
		CanvasPanel_MouseClick->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WidgetSwitcher_MouseClickType->SetActiveWidgetIndex((int32)InType);
		PlayAnimationByName("MouseClick", 0.f, 0);
	}
	break;

	}
}

void UDMUIPanel_SkillNotice::ShowOff()
{
	PlayAnimationByName("ShowOff");
}
