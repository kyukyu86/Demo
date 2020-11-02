// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentStateMark.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRConsoleManager.h"
#include "WidgetComponent/WRWidgetComponentBase.h"
#include "Enum/EWRUIEnum.h"
#include "UI/Panel/WRUIPanel_AvailableDialogueIcon.h"


void UWRActorComponentStateMark::OnCreateComponent()
{
	Super::OnCreateComponent();


}

void UWRActorComponentStateMark::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentStateMark::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWRActorComponentStateMark::AddMark(const EWRStateMarkType IN InStateMarkType)
{
	if (StateMarkTypes.Contains(InStateMarkType))
		return;

	StateMarkTypes.AddTail(InStateMarkType);
	ShowMark(InStateMarkType);
}

void UWRActorComponentStateMark::RemoveMark(const EWRStateMarkType IN InStateMarkType)
{
	if (InStateMarkType == EWRStateMarkType::None)
		return;

	StateMarkTypes.RemoveNode(InStateMarkType);
	if (VisibleStateMarkType != InStateMarkType)
		return;
		
	HideMark(InStateMarkType);
	VisibleStateMarkType = EWRStateMarkType::None;

	EWRStateMarkType StateMarkType = EWRStateMarkType::None;
	for (auto& Iter : StateMarkTypes)
	{
		if(IsAvailableShowMark(Iter, StateMarkType) == false)
			continue;

		StateMarkType = Iter;
	}

	if (StateMarkType != EWRStateMarkType::None)
		ShowMark(StateMarkType);
}

bool UWRActorComponentStateMark::ContainsMark(const EWRStateMarkType IN InStateMarkType)
{
	return StateMarkTypes.Contains(InStateMarkType);
}

void UWRActorComponentStateMark::ShowMark(const EWRStateMarkType IN InStateMarkType)
{
	if (IsAvailableShowMark(InStateMarkType) == false)
		return;
	
	if (VisibleStateMarkType != EWRStateMarkType::None)
	{
		HideMark(VisibleStateMarkType);
	}
	
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (InStateMarkType == EWRStateMarkType::Talking)
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (StateMarkTypes.Contains(InStateMarkType) == false)
			{
				WRUIManager::Get()->CloseUI(GetOwnerCharacter(), EWRUIPanelKind::SpeakDialogueIcon);
				return;
			}

			if (InCombinedPanelData.Is3DWidget())
			{
				//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::SpeakDialogueIcon_FixedSize) * 0.0001f);
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Dialogue")));
			}
		});
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SpeakDialogueIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, OwnerCharacter);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::SpeakDialogueIcon, OwnerCharacter, FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}
	else if (InStateMarkType == EWRStateMarkType::AvailableTalk)
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (StateMarkTypes.Contains(InStateMarkType) == false)
			{
				WRUIManager::Get()->CloseUI(GetOwnerCharacter(), EWRUIPanelKind::AvailableDialogueIcon);
				return;
			}

			if (InCombinedPanelData.Is3DWidget())
			{
				//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::SpeakDialogueIcon_FixedSize) * 0.0001f);
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Dialogue")));

				UWRUIPanel_AvailableDialogueIcon* Panel_AvailableDialogueIcon = Cast<UWRUIPanel_AvailableDialogueIcon>(InCombinedPanelData.UIPanel);
				if (Panel_AvailableDialogueIcon)
					Panel_AvailableDialogueIcon->SetOwnerCharacter(Cast<AWRCharacterNPC>(OwnerCharacter));
			}
		});
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::AvailableDialogueIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, OwnerCharacter);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::AvailableDialogueIcon, OwnerCharacter, FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}

	VisibleStateMarkType = InStateMarkType;
}

void UWRActorComponentStateMark::HideMark(const EWRStateMarkType IN InStateMarkType)
{
	if (InStateMarkType == EWRStateMarkType::Talking)
	{
		WRUIManager::Get()->CloseUI(GetOwnerCharacter(), EWRUIPanelKind::SpeakDialogueIcon);
	}
	else if (InStateMarkType == EWRStateMarkType::AvailableTalk)
	{
		WRUIManager::Get()->CloseUI(GetOwnerCharacter(), EWRUIPanelKind::AvailableDialogueIcon);
	}
}

bool UWRActorComponentStateMark::IsAvailableShowMark(const EWRStateMarkType IN InStateMarkType)
{
	if (VisibleStateMarkType == EWRStateMarkType::None)
		return true;

	if (InStateMarkType < VisibleStateMarkType)
		return true;

	return false;
}

bool UWRActorComponentStateMark::IsAvailableShowMark(const EWRStateMarkType IN InStateMarkType, const EWRStateMarkType IN InVisibleMarkType)
{
	if (InVisibleMarkType == EWRStateMarkType::None)
		return true;

	if (InStateMarkType < InVisibleMarkType)
		return true;

	return false;
}
