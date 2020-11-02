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
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
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


