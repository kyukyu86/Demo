// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRPath.h"

/**
 * 
 */
class WR_API WRPathManager : public WRSingleton<WRPathManager>
{
	CDECLARE_MANAGER(WRPathManager)

public:

	FString GetPath(EWRPath InKindofPath);

private:
	WRPathManager();
	virtual ~WRPathManager();

	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() override;
	//====================================================================================

private:

	void AddPath(EWRPath InKindofPath, FString InPath);
	void AddPath(EWRPath InKindofPath, FString InPath, EWRPath InKindofUpperPath);

	void CheckAndAddSlash(FString& InPath);

private:

	TMap<EWRPath, FString>	CollectedPaths;

};

#define ADDPATH(KindofPath, Path)										{ this->AddPath(KindofPath, Path); }
#define ADDPATH_COMBINE_UPPER(KindofPath, Path, KindofUpperPath);		{ this->AddPath(KindofPath, Path, KindofUpperPath); }



DECLARE_LOG_CATEGORY_EXTERN(PathManager, Display, All);
