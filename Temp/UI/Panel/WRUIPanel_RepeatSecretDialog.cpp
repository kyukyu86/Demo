// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_RepeatSecretDialog.h"
#include "Network/IMMessageSender.h"

void UWRUIPanel_RepeatSecretDialog::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWRUIPanel_RepeatSecretDialog::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_RepeatSecretDialog::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (EndDeltaTime < 0.f)
		return;

	CurrentDeltaTime += InDeltaTime;

	if (CurrentDeltaTime >= EndDeltaTime)
	{
		this->ClosePanel();
	}

}

void UWRUIPanel_RepeatSecretDialog::ClosePanel()
{
	if (this->GetPartnerID() == INVALID_AID)
		return;

	IMMessageSender::ReportAgreeToSecretDialog(this->GetPartnerID(), this->GetGroupID(), false);
}
