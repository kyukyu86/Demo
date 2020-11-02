// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUserWidget.h"
#include "Core/WRGameInstance.h"
#include "WidgetBlueprintLibrary.h"
#include "Manager/WRUIManager.h"
#include "GameFramework/PlayerController.h"
#include "UI/Component/WRUIComponentFocus.h"
#include "WidgetPath.h"
#include "Events.h"
#include "UI/CustomWidget/WRFocusButton.h"


void UWRUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bUseWidgetAnimation)
	{
		CollectWidgetAnimation();
	}
}

void UWRUserWidget::NativeDestruct()
{
	WidgetAnimationList.Empty();
	for (auto& EachUIComponent : UIComponentList)
	{
		delete EachUIComponent.Value;
	}
	UIComponentList.Empty();

	Super::NativeDestruct();
}

void UWRUserWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	// for Focus Button 
	// [ 2020-7-30 : kyu ] ÇöÀç ¾È¾¸
// 	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
// 	if (UIComponentFocus != nullptr)
// 	{
// 		UIComponentFocus->OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
// 	}
}

UWidgetAnimation* UWRUserWidget::GetAnimationByName(FName IN InAnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = WidgetAnimationList.Find(InAnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

void UWRUserWidget::CollectWidgetAnimation()
{
	WidgetAnimationList.Empty();

	FProperty* Prop = GetClass()->PropertyLink;

	// Run through all properties of this class to find any widget animations
	while (Prop != nullptr)
	{
		// Only interested in object properties
		if (Prop->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop);

			// Only want the properties that are widget animations
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnim != nullptr && WidgetAnim->MovieScene != nullptr)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					WidgetAnimationList.Add(AnimName, WidgetAnim);
				}
			}
		}

		Prop = Prop->PropertyLinkNext;
	}
}

bool UWRUserWidget::IsContainedUIComponent(const EWRUIComponentType IN InUIComponentType)
{
	return UIComponentList.Contains(InUIComponentType);
}

bool UWRUserWidget::PlayAnimationByName(const FName InAnimationName, float StartAtTime /* = 0.0f */, int32 NumLoopsToPlay /* = 1 */, EUMGSequencePlayMode::Type PlayMode /* = EUMGSequencePlayMode::Forward */, float PlaybackSpeed /* = 1.0f */, bool bRestoreState /* = false */)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed, bRestoreState);
		return true;
	}
	return false;
}

void UWRUserWidget::StopAnimationByName(const FName InAnimationName)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		StopAnimation(WidgetAnim);
	}
}

bool UWRUserWidget::IsAnimationPlayingByName(const FName InAnimationName)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		return IsAnimationPlaying(WidgetAnim);
	}
	return false;
}

void UWRUserWidget::OnClickedFocusedButton()
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		UIComponentFocus->OnClickedFocusedButton();
	}
}

void UWRUserWidget::RefocusOnLastFocusedButton()
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		UIComponentFocus->RefocusOnLastFocusedButton();
	}
}

void UWRUserWidget::FocusToDefaultFocusButton()
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		UIComponentFocus->FocusToDefaultFocusButton();
	}
}

class UWRFocusButton* UWRUserWidget::GetDefaultFocusButton()
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		return UIComponentFocus->GetDefaultFocusButton();
	}
	return nullptr;
}

void UWRUserWidget::SetFocusableInputMode(const bool IN InUIMode, UWidget* IN InFocusWidget /*= nullptr*/)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return;

	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	if (InUIMode)
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, InFocusWidget);
#if WITH_EDITOR || PLATFORM_WINDOWS
		WRUIManager::Get()->SaveFocusedUI(InFocusWidget);
#endif // WITH_EDITOR || PLATFORM_WINDOWS
	}
	else
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
#if WITH_EDITOR || PLATFORM_WINDOWS
		WRUIManager::Get()->ReleaseFocusedUI();
#endif // WITH_EDITOR || PLATFORM_WINDOWS
	}
}