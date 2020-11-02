// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <Kismet/BlueprintPlatformLibrary.h>

UENUM(BlueprintType)
enum class EWRManagerTickGroup : uint8
{
	TG_None,
	TG_PreProcess,			// Ticking before NormalTick
	TG_Normal,				// Ticking before PostTick
	TG_PostProcess,			// Ticking after Called NormalTick group
	TG_Max,
};

struct FWRBindDelegateFlag
{
public:
	FWRBindDelegateFlag(const int32 InFlag) : Flag(InFlag) {}

	enum en
	{
		Deactivate = 1,
		Reactivate = 2,
		EnterBackground = 4,
		EnterForeground = 8,
		Terminate = 16,
		ScreenOrientationChanged = 32,
	};

	bool CheckFlag(FWRBindDelegateFlag::en InFlag) const { return Flag & (int32)InFlag; }

private:
	FWRBindDelegateFlag() {}

private:
	int32 Flag;
};

// [ 2019-6-14 : magedoga ] Manager에 필수적으로 정의해야함.
class IWRSingleton
{
public:
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize() = 0;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown() = 0;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart() = 0;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() = 0;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() = 0;
};

/**
 * 
 */
class WR_API WRSingletonObject : public IWRSingleton
{
public:

	friend class WRSingletonManager;

	FName GetName() const { return Name; }

	void SetEnableCoreDelegate(FWRBindDelegateFlag InFlag);

	EWRManagerTickGroup GetTickGroup() const { return TickGroup; }

protected:
	WRSingletonObject();
	virtual ~WRSingletonObject();

	// [ 2019-6-17 : magedoga ] TODO : Tick Delay 필요할경우 추후 수정
	void Initialize(const FName& InName, const bool bInEnableTick = false, EWRManagerTickGroup InTickGroupType = EWRManagerTickGroup::TG_Normal);
	void Initialize(const FName& InName, FWRBindDelegateFlag InFlag, const bool bInEnableTick = false, EWRManagerTickGroup InTickGroupType = EWRManagerTickGroup::TG_Normal);
	void Shutdown();
	void DestroyInstance();

	virtual bool OnTick(float InDeltaTime) { return true; }
	virtual void OnDestroyInstance() {}

	virtual void OnDeactivate() {}
	virtual void OnReactivate() {}
	virtual void OnEnterBackground() {}
	virtual void OnEnterForeground() {}
	virtual void OnTerminate() {}
	virtual void OnScreenOrientationChanged(EScreenOrientation::Type InType) {}

	virtual void EndLoadLevelProcess() {}

	void SetEnableTick(const bool bInEnable, EWRManagerTickGroup InTickGroupType = EWRManagerTickGroup::TG_Normal);

#if WITH_EDITOR
	bool bShutdown = false;
#endif // WITH_EDITOR

protected:

	FName				Name;

private:

	FDelegateHandle		HandleTicker;

	FDelegateHandle		HandleDeactivate;
	FDelegateHandle		HandleReactivate;
	FDelegateHandle		HandleEnterBackground;
	FDelegateHandle		HandleEnterForeground;
	FDelegateHandle		HandleTerminate;
	FDelegateHandle		HandleScreenOrientationChanged;

	EWRManagerTickGroup	TickGroup;
};
