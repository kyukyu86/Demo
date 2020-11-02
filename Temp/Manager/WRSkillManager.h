#pragma once
#include "Manager/SingletonClass/WRSingleton.h"
#include "Enum/EWRCharacter.h"

class WRSkillManager : public WRSingleton<WRSkillManager>
{
public:
	WRSkillManager();
	virtual ~WRSkillManager();

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

public:
	bool IsGuardBreakSkill(WRTableID InTableID);
	bool IsPlayingMontageGuardBreakSkill(class AWRCharacter* InCharacter);
	EWRFSM GetFSMTypeDependOnSkillTable(AWRCharacter* InActor, WRTableID InTID);
	bool IsStarterSkill(WRTableID InTID, AWRCharacter* InCaster);
	bool IsInRectangle(AWRCharacter* InCaster, AWRCharacter* InTarget, float InWidth, float InLength);
};

