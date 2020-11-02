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
// 	void SetCombatInfoOfChar(WRCombatInfo* InCombatInfo);	//컴뱃인포 만들어 질때 정보 세팅
// 	void RemoveCharPtrFromCombatInfo(AID_t InAUID); //케릭터가 삭제 될때 combatinfo의 포인터 리셋
// 	void RemoveCombatInfoFromCharInfo(WRCombatInfo* InCombatInfo);	//컴뱃인포가 사라질때 mapCombatInfoOfChar에서 정보 삭제

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

	//공통
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

