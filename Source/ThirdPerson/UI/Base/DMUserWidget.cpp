// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUserWidget.h"
#include "../../GameInstance/DMGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UDMUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bUseWidgetAnimation)
	{
		CollectWidgetAnimation();
	}
}

void UDMUserWidget::NativeDestruct()
{
	WidgetAnimationList.Empty();

	Super::NativeDestruct();
}

void UDMUserWidget::SetFocusableInputMode(const bool IN InUIMode, UWidget* IN InFocusWidget /*= nullptr*/)
{
	if (UDMGameInstance::GetGameInstance() == nullptr)
		return;

	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	if (InUIMode)
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, InFocusWidget);
// #ifdef UE_EDITOR
// 		WRUIManager::Get()->SaveFocusedUI(InFocusWidget);
// #endif // UE_EDITOR
	}
	else
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
// #ifdef UE_EDITOR
// 		WRUIManager::Get()->ReleaseFocusedUI();
// #endif // UE_EDITOR
	}
}

UWidgetAnimation* UDMUserWidget::GetAnimationByName(FName IN InAnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = WidgetAnimationList.Find(InAnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

void UDMUserWidget::CollectWidgetAnimation()
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

void UDMUserWidget::SetFocusThis()
{
	SetFocusableInputMode(true, this);
}

bool UDMUserWidget::PlayAnimationByName(const FName InAnimationName, float StartAtTime /*= 0.0f*/, int32 NumLoopsToPlay /*= 1*/, EUMGSequencePlayMode::Type PlayMode /*= EUMGSequencePlayMode::Forward*/, float PlaybackSpeed /*= 1.0f*/, bool bRestoreState /*= false*/)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed, bRestoreState);
		return true;
	}
	return false;
}

void UDMUserWidget::StopAnimationByName(const FName InAnimationName)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		StopAnimation(WidgetAnim);
	}
}

bool UDMUserWidget::IsAnimationPlayingByName(const FName InAnimationName)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(InAnimationName);
	if (WidgetAnim)
	{
		return IsAnimationPlaying(WidgetAnim);
	}
	return false;
}
