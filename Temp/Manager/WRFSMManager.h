#pragma once

#include "Actor/FSM/WRFSMBase.h"
#include "Enum/EWRCharacter.h"
#include "SingletonClass/WRSingleton.h"


class WRFSMManager : public WRSingleton<WRFSMManager>
{
public:
	WRFSMManager();
	virtual ~WRFSMManager();

public:
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

	virtual bool OnTick(float InDeltaTime) override;

public:
	FORCEINLINE WRFSMBase* GetFSM(EWRFSM FSMType)
	{
		if (mapFSMInfo.Contains(FSMType) == false)
			return nullptr;

		return mapFSMInfo[FSMType];
	}

private:
	TMap<EWRFSM, WRFSMBase*> mapFSMInfo;
};

