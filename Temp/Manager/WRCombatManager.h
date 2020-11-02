#pragma once
#include "Enum/EWRCharacter.h"
#include "SingletonClass/WRSingleton.h"
#include "CombatSystem/Combat/WRCombatInfo.h"


class WRCombatManager : public WRSingleton<WRCombatManager>
{
public:
	WRCombatManager();
	virtual ~WRCombatManager();

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
// 	void SetCombatInfoOfChar(WRCombatInfo* InCombatInfo);	//�Ĺ����� ����� ���� ���� ����
// 	void RemoveCharPtrFromCombatInfo(AID_t InAUID); //�ɸ��Ͱ� ���� �ɶ� combatinfo�� ������ ����
// 	void RemoveCombatInfoFromCharInfo(WRCombatInfo* InCombatInfo);	//�Ĺ������� ������� mapCombatInfoOfChar���� ���� ����

public:
	SKILL_SEQ_t GenerateCombatSeqIDForStandalone();

	WRCombatInfo* FindBySeqID(AID_t InAID, SKILL_SEQ_t InSeqID);
	WRCombatInfo* FindLastCombatInfo(AID_t InAID);
	TArray<WRCombatInfo*> FindAll(AID_t InAID);

	void CreateInfo(const FWRCombatParam& InCombatParams);
	void RegisterCombat(AID_t InAID, WRCombatInfo* InCombat);
	void Unregister(SKILL_SEQ_t InCombatSeqID, bool bForce = false);
	void Unregister(WRCombatInfo* InCombat, bool bForce = false);
	void UnregisterCurrentCombat(AID_t InActorID, bool bForce = false);
	void UnregisterUsingDeleteTimer(AID_t InCombatSeqID);
	void UnregisterAll(AID_t InActorID);

	bool IsProjectile(FWRSkillTable* InTable);

private:
	WRCombatInfo* GenerateCombatInfo();
	void ReturnToPool(WRCombatInfo* InCombatInfo);
	void UnregisterCombat(AID_t InActorID, WRCombatInfo* InCombat);
	void UnregisterCombatAll(AID_t InActorID);

	//����
private:
	TMap<AID_t, TArray<WRCombatInfo*>>	mapCombatInfo;
	TDoubleLinkedList<WRCombatInfo*>	CombatInfoPool;

	//Online
private:
	TMap<AID_t, TArray<WRCombatInfo*>>	mapLinkedCharacterWithCombatInfo;


	//StandAlone
private:
	SKILL_SEQ_t LastCombatSeqID = 0;

};

