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
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;
	//====================================================================================
	
	void AddStyleNotice(const FString& IN InNotice);
	void AddNotice(const FString& IN InNotice);
	void AddNotice(WRTableID InTID);
	void AddItem(const WRTableID IN InTableID, const int32 IN InCount = 1);
};