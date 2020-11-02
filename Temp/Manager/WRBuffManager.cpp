#include "WRBuffManager.h"
#include "WRUIManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Define/WRTypeDefine.h"
#include "WRCharacterManager.h"
#include "Table/Base/WRTableManager.h"
#include "Component/Character/WRActorComponentBuff.h"




//====================================================================================

WRBuffManager::WRBuffManager()
{
}


WRBuffManager::~WRBuffManager()
{
}

//====================================================================================

// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRBuffManager::OnInitialize()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRBuffManager::OnShutdown()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
void WRBuffManager::OnLoadLevelStart()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
void WRBuffManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
void WRBuffManager::OnRenderStart()
{
        
}

//====================================================================================

void WRBuffManager::SetBuff(const AID_t IN InAID, const std::list<stBuffInfo>& IN InBuffList)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return;

	SetBuff(FoundChar, InBuffList);
}

void WRBuffManager::SetBuff(class AWRCharacter* IN InChar, const std::list<stBuffInfo>& IN InBuffList)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return;

	std::list<stBuffInfo>::const_iterator iter = InBuffList.begin();
	std::list<stBuffInfo>::const_iterator iter_end = InBuffList.end();
	for (; iter != iter_end; ++iter)
	{
		ActorComponentBuff->AddBuff(*iter);
	}
}

void WRBuffManager::AddBuff(const AID_t IN InAID, const stBuffInfo& IN InBuffInfo)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return;

	AddBuff(FoundChar, InBuffInfo);
}

void WRBuffManager::AddBuff(class AWRCharacter* IN InChar, const stBuffInfo& IN InBuffInfo)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return;

	ActorComponentBuff->AddBuff(InBuffInfo);
}

void WRBuffManager::RemoveBuff(const AID_t IN InAID, const WRTableID IN InBuffTID)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return;

	RemoveBuff(FoundChar, InBuffTID);
}

void WRBuffManager::RemoveBuff(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return;

	ActorComponentBuff->RemoveBuff(InBuffTID);
}

void WRBuffManager::ChangeBuffStack(const AID_t IN InAID, const WRTableID IN InBuffTID, const int32 IN InStack)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return;

	ChangeBuffStack(FoundChar, InBuffTID, InStack);
}

void WRBuffManager::ChangeBuffStack(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID, const int32 IN InStack)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return;

	ActorComponentBuff->ChangeBuffStack(InBuffTID, InStack);
}

bool WRBuffManager::HaveBuff(const AID_t IN InAID, const WRTableID IN InBuffTID)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return false;

	return HaveBuff(FoundChar, InBuffTID);
}

bool WRBuffManager::HaveBuff(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return false;

	return ActorComponentBuff->HaveBuff(InBuffTID);
}

float WRBuffManager::GetBuffRemainTime(const AID_t IN InAID, const WRTableID IN InBuffTID)
{
	AWRCharacter* FoundChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundChar == nullptr)
		return 0.f;

	return GetBuffRemainTime(FoundChar, InBuffTID);
}

float WRBuffManager::GetBuffRemainTime(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID)
{
	UWRActorComponentBuff* ActorComponentBuff = InChar->GetComponent<UWRActorComponentBuff>();
	if (ActorComponentBuff == nullptr)
		return 0.f;

	return ActorComponentBuff->GetRemainTime(InBuffTID);
}

bool WRBuffManager::MakeBuff(const stBuffInfo& IN InBuffInfo, FWRBuff*& OUT OutBuff)
{
	FWRBuffTable* FoundTable = WRTableManager::Get()->FindRow<FWRBuffTable>(InBuffInfo.buffTid);
	if (FoundTable == nullptr)
	{
		ensureMsgf(false, TEXT("Invalid Buff TID"));
		return false;
	}

	OutBuff = new FWRBuff(InBuffInfo);
	OutBuff->pBuffTable = FoundTable;
	OutBuff->MaxStack = FoundTable->Stack;

	return true;
}
