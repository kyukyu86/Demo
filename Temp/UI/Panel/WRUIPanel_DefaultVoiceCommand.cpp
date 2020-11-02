// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DefaultVoiceCommand.h"
#include "Define/WRUIDefine.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"

void UWRUIPanel_DefaultVoiceCommand::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::Default););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center); );
	BIND_WIDGET(UWRUISlot_RecommendedVoiceCommand, Slot_RecommendedVoiceCommand);
}

void UWRUIPanel_DefaultVoiceCommand::NativeDestruct()
{
	Super::NativeDestruct();
}
