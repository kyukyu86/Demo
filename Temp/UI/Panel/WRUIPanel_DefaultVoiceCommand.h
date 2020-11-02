// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "UI/Slot/WRUISlot_RecommendedVoiceCommand.h"
#include "WRUIPanel_DefaultVoiceCommand.generated.h"


class UWRUISlot_VoiceCommand;
class UWRUISlot_VoiceCommandText;
UCLASS()
class WR_API UWRUIPanel_DefaultVoiceCommand : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }

	void UpdateRecommendedVoiceCommand() { Slot_RecommendedVoiceCommand->UpdateRecommendedVoiceCommandData(); }
private:
	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;
	UWRUISlot_RecommendedVoiceCommand* Slot_RecommendedVoiceCommand;
};
