#pragma once
#include "Manager/SingletonClass/WRSingleton.h"
#include "Enum/EWRCharacter.h"

class WRSkillManager : public WRSingleton<WRSkillManager>
{
public:
	WRSkillManager();
	virtual ~WRSkillManager();

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

public:
	bool IsGuardBreakSkill(WRTableID InTableID);
	bool IsPlayingMontageGuardBreakSkill(class AWRCharacter* InCharacter);
	EWRFSM GetFSMTypeDependOnSkillTable(AWRCharacter* InActor, WRTableID InTID);
	bool IsStarterSkill(WRTableID InTID, AWRCharacter* InCaster);
	bool IsInRectangle(AWRCharacter* InCaster, AWRCharacter* InTarget, float InWidth, float InLength);
};

