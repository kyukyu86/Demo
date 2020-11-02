// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRUIPanel_RepeatSecretDialog.generated.h"

/**
 *
 */
UCLASS()
class WR_API UWRUIPanel_RepeatSecretDialog : public UWRUIPanel
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FORCEINLINE void SetRptInfo(const AID_t InPartnerID, const CHAT_GROUP_ID_t InGroupID, const float InEndTimer = -1.f) { PartnerAID = InPartnerID; GroupID = InGroupID; EndDeltaTime = InEndTimer; }
	FORCEINLINE CHAT_GROUP_ID_t GetGroupID() { return GroupID; }
	FORCEINLINE AID_t GetPartnerID() { return PartnerAID; }

private:
	AID_t PartnerAID = INVALID_AID;
	CHAT_GROUP_ID_t GroupID;

	float CurrentDeltaTime = 0.f;
	float EndDeltaTime = -1.f;
private:
	void ClosePanel();
};
