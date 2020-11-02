// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Main.h"
#include "Define/WRUIDefine.h"
#include "TextBlock.h"
#include "Core/WRGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRInputSystem.h"

void UWRUIPanel_Main::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRUISlot_Compass, Slot_Compass);
	BIND_WIDGET(UWRUISlot_DropPopup, Slot_DropPopup);
	BIND_WIDGET(UWRUISlot_GyroInteraction, Slot_GyroInteraction);
	BIND_WIDGET(UWRUISlot_TutorialSub, Slot_TutorialSub);
	BIND_WIDGET(UWRUISlot_TutorialAlert, Slot_TutorialAlert);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::Default););
	BIND_WIDGET(UWRUISlot_ChatRecording, Slot_ChatRecording);

	BIND_WIDGET(UTextBlock, TextBlock_GyroTransform);
}

void UWRUIPanel_Main::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	// [ 2019-11-15 : kyu ] for Test
	if (TextBlock_GyroTransform != nullptr)
	{
		FString strGyroTransform;

		APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetWorld()->GetFirstPlayerController();
		if (PlayerController->IsValidLowLevel())
		{
			FVector Empty;
			FVector Gravity;
			PlayerController->GetInputMotionState(Empty, Empty, Gravity, Empty);
			
			strGyroTransform = FString::Printf(TEXT("Loc : %.1f, %.1f, %.1f\n(%.1f, %.1f, %.1f)"), Gravity.X, Gravity.Y, Gravity.Z, Gravity.GetSafeNormal().X, Gravity.GetSafeNormal().Y, Gravity.GetSafeNormal().Z);
			WRUIUtility::SetTextSafe(TextBlock_GyroTransform, strGyroTransform);

			FVector vFrontFront(2000.f, 16000.f, 3000.f);	// 0,1,0
			FVector vFrontBack(2000.f, -16000.f, 3000.f);	// 0,-1,0
			FVector vFrontUp(2000.f, 3000.f, -16000.f);		// 0,0,-1
			FVector vFrontDown(2000.f, 3000.f, 16000.f);	// 0,0,1
			FVector vLeftDown(16000.f, 0.f, 0.f);			// 1,0,0
			FVector vRightDown(-16000.f, 0.f, 0.f);			// -1,0,0
		}
	}
}
