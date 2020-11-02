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

