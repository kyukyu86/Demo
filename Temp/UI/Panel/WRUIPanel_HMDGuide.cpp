// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_HMDGuide.h"
#include "Manager/WRSFXManager.h"
#include "WRUIPanel_Title.h"
#include "Utility/WRUIUtility.h"
#include "Utility/WRActorUtility.h"
#include "TimerManager.h"
#include "VisibilityBinding.h"
#include "DelegateSignatureImpl.inl"
#include "Manager/WRMapManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInputSystemManager.h"

void UWRUIPanel_HMDGuide::NativeConstruct()
{
	this->FillAnimationsMap();

	Super::NativeConstruct();
	SetFocusableInputMode(true, this);

	ResetOrientationKeys.Empty();
	ResetOrientationKeys.Add(EWRInput::R2);
	ResetOrientationKeys.Add(EWRInput::L2);
}

void UWRUIPanel_HMDGuide::NativeDestruct()
{
	SetFocusableInputMode(false);
	Super::NativeDestruct();
}

void UWRUIPanel_HMDGuide::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UWRUIPanel_HMDGuide::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());

	ProceedStep();

	if (DownKeys.Contains(WRInput) == true)
		DownKeys.RemoveNode(WRInput);

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UWRUIPanel_HMDGuide::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());

	if (DownKeys.Contains(WRInput) == false)
		DownKeys.AddHead(WRInput);

	if (this->IsCanProceed(ResetOrientationKeys))
	{
		WRInputSystemManager::Get()->ResetHMDOrientation();
	}


	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWRUIPanel_HMDGuide::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	for (FWRGuideStep InStep : InSteps)
	{
		UWidgetAnimation* InAnim = this->GetAnimationByName(InStep.WidgetAnimName);
		if (InAnim == Animation && InStep.IsAutoProceed == true && InStep.IsPlayed == false)
		{
			this->AutoProceed();
			break;
		}
	}
}

void UWRUIPanel_HMDGuide::StartGuideProcess()
{
	CurrentIndex = 0;
	for (int32 i = 0; i < InSteps.Num(); i++)
	{
		InSteps[i].IsPlayed = false;
	}
	this->PlayUIAnim();
}

bool UWRUIPanel_HMDGuide::IsCanProceed(const TArray<EWRInput>& IN InNeedInputKeys)
{
	bool CanProceed = false;

	for (EWRInput InKey : InNeedInputKeys)
	{
		if (DownKeys.Contains(InKey) == false)
		{
			CanProceed = false;
			break;
		}
		else
		{
			CanProceed = true;
		}
	}

	return CanProceed;
}

void UWRUIPanel_HMDGuide::ProceedStep()
{
	if (this->IsCanProceed(ReturnFirstStepKeys) == true)
	{
		this->StartGuideProcess();
		return;
	}

	if (this->IsCanProceed(FinishKeys) == true)
	{
		this->EndStep();
		return;
	}

	if (InSteps.IsValidIndex(CurrentIndex) == false)
		return;

	const FWRGuideStep InStep = InSteps[CurrentIndex];

	if (this->IsCanProceed(InStep.Keys) == true)
	{
		if (InSteps.IsValidIndex(CurrentIndex) == true)
			InSteps[CurrentIndex].IsPlayed = true;

		CurrentIndex++;

		if (CurrentIndex >= InSteps.Num())
		{
			this->EndStep();
		}
		else
		{
			this->PlayUIAnim();
		}
	}
}

void UWRUIPanel_HMDGuide::AutoProceed()
{
	if (InSteps.IsValidIndex(CurrentIndex) == true)
		InSteps[CurrentIndex].IsPlayed = true;
	CurrentIndex++;
	if (CurrentIndex >= InSteps.Num())
	{
		this->EndStep();
	}
	else
	{
		this->PlayUIAnim();
	}
}

UWidgetAnimation* UWRUIPanel_HMDGuide::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = AnimationsMap.Find(AnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

bool UWRUIPanel_HMDGuide::PlayAnimationByName(FName AnimationName, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(AnimationName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed);
		return true;
	}
	return false;
}

void UWRUIPanel_HMDGuide::FillAnimationsMap()
{
	AnimationsMap.Empty();

	FProperty* Prop = this->GetClass()->PropertyLink;

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
					AnimationsMap.Add(AnimName, WidgetAnim);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

void UWRUIPanel_HMDGuide::PlayUIAnim()
{
	//WRSFXManager::Get()->Spawn2D(TEXT("Start_logo_Cue.Start_logo_Cue"));
	if (InSteps.IsValidIndex(CurrentIndex) == false)
		return;
	const FWRGuideStep InStep = InSteps[CurrentIndex];
	//GetAnimationByName();
	UWidgetAnimation* InUIAnim = this->GetAnimationByName(InStep.WidgetAnimName);

	if (InUIAnim == nullptr)
		return;

	if (InUIAnim->IsValidLowLevel() == false)
		return;

	if (CurrentUIAnim != nullptr)
		PrevUIAnim = CurrentUIAnim;

	if (InStep.IsAttachToCamera == true)
	{
		this->SetAttachToCamera();

	}
	else
	{
		this->SetLocationOnTable();
	}

	CurrentUIAnim = InUIAnim;

	StopAnimation(PrevUIAnim);

	if (InStep.IsAnimLoop == true)
	{
		PlayAnimation(CurrentUIAnim, 0, 0);
	}
	else
	{
		PlayAnimation(CurrentUIAnim);
	}
}

void UWRUIPanel_HMDGuide::SetAttachToCamera()
{
	FWRCombinedPanelData* InPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::HmdGuide);
	if (InPanelData != nullptr)
	{
		UWRWidgetComponentBase* InWidgetComp = InPanelData->WidgetComponentBase;
		if (InWidgetComp != nullptr)
		{
			UDataTable* WidgetTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Asset/Data/Table/Custom/WidgetTable.WidgetTable"));

			if (WidgetTable == nullptr)
				return;

			TArray<FWRWidgetTable*> TempArray;
			WidgetTable->GetAllRows(FString(""), TempArray);

			for (int32 i = 0; i < TempArray.Num(); ++i)
			{
				FWRWidgetTable* pTable = TempArray[i];
				if (pTable == nullptr)
					continue;

				if (pTable->PanelKind == EWRUIPanelKind::HmdGuide)
				{
					FTransform LocalTransform = InWidgetComp->GetComponentTransform();
					LocalTransform.SetLocation(pTable->Location);
					LocalTransform.SetRotation(pTable->Rotation.Quaternion());

					break;
				}
			}


			InWidgetComp->SetAttachToCamera(true);
		}
	}
}

void UWRUIPanel_HMDGuide::SetLocationOnTable()
{
	FWRCombinedPanelData* InPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::HmdGuide);
	if (InPanelData != nullptr)
	{
		UWRWidgetComponentBase* InWidgetComp = InPanelData->WidgetComponentBase;
		if (InWidgetComp != nullptr)
		{
			InWidgetComp->SetWorldTransform(PanelTransform);
			InWidgetComp->SetAttachToCamera(false);
		}
	}
}

#define LAP_TID 10001
void UWRUIPanel_HMDGuide::EndStep()
{
	WRMapManager::Get()->LoadLevelByTable(LAP_TID);
}