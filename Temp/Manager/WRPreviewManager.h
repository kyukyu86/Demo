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

	FString SetPreview(FWRPreviewInfo& IN InPreviewInfo);

	class AWRPreviewStudio* GetPreviewStudio(class UWRUISlot_Preview* IN InPreviewWidget);

	bool ReleasePreview(const FString IN InAsyncKey, const bool IN InParentDestroy = false);
	void ReleasePreview(class UWRUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy = false);

};