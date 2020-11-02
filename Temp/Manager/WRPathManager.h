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
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
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
