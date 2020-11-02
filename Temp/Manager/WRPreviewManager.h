// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "CoreMinimal.h"
#include "Queue.h"
#include "Define/WRTypeDefine.h"
#include "Struct/WRPreviewStruct.h"




class WR_API WRPreviewManager : public WRSingleton<WRPreviewManager>
{
	CDECLARE_MANAGER(WRPreviewManager);
	
private:
	TMap<class UWRUISlot_Preview*, FWRPreviewInfo>	PreviewList;
	TMap<FString, FWRPreviewInfo>	AsyncPreviewList;

private:
	WRPreviewManager();
	virtual ~WRPreviewManager();

	void ReleaseData();

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

	FString SetPreview(FWRPreviewInfo& IN InPreviewInfo);

	class AWRPreviewStudio* GetPreviewStudio(class UWRUISlot_Preview* IN InPreviewWidget);

	bool ReleasePreview(const FString IN InAsyncKey, const bool IN InParentDestroy = false);
	void ReleasePreview(class UWRUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy = false);

};