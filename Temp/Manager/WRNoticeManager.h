// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "CoreMinimal.h"
#include "Queue.h"
#include "Define/WRTypeDefine.h"

class WR_API WRNoticeManager : public WRSingleton<WRNoticeManager>
{
	CDECLARE_MANAGER(WRNoticeManager);
	
private:
	WRNoticeManager();
	virtual ~WRNoticeManager();
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================
	
	void AddStyleNotice(const FString& IN InNotice);
	void AddNotice(const FString& IN InNotice);
	void AddNotice(WRTableID InTID);
	void AddItem(const WRTableID IN InTableID, const int32 IN InCount = 1);
};