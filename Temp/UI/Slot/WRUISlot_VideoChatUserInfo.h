// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "Controller/WRVideoChatController.h"
#include "WRUISlot_VideoChatUserInfo.generated.h"

class UWRTextBlock;
class UImage;
UCLASS()
class WR_API UWRUISlot_VideoChatUserInfo : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	void SetUserInfo(const FWRVideoChatUserData& IN InVideoChatUserInfo);
private:
	UWRTextBlock* WRTextBlock_NickName;
	UWRTextBlock* WRTextBlock_Online;
	UWRTextBlock* WRTextBlock_CharacterName;
	UWRTextBlock* WRTextBlock_ShortcutNumber;

	UImage* Image_Profile;
};