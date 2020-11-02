// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_Language.h"


/**
*
*/

// 

class AWRCharacter;

class WR_API WRChatBubbleManager : public WRSingleton<WRChatBubbleManager>
{
	CDECLARE_MANAGER(WRChatBubbleManager);
	
private:
	WRChatBubbleManager();
	virtual ~WRChatBubbleManager();
	
public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

public:
	void OpenChatBubble(const AWRCharacter* IN InCharacter, const FText& IN InContent, const float IN InTime = 0.f);
	void CloseChatBubble(const AWRCharacter* IN InCharacter);

	void OnInformSpeechBubbleReceived(const AID_t IN InAID, const EWRLanguageType::en IN InLanguageType, const FString& InStr);
	void OnResponseTranslation(const int32 IN InIdentify, const FString& InStr);
private:
	void RequestTranslationChat(const AID_t IN InAID, const EWRLanguageType::en IN InSrcLanguageType, const EWRLanguageType::en IN InDestLanguageType, const FString& IN InStr);

private:
	TMap<int32, AID_t> TranslationList;
};


