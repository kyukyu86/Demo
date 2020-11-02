#include "WRCombatManager.h"
#include "CombatSystem/Combat/WRCombatInfo.h"
#include "Actor/Character/WRCharacter.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "WRCharacterManager.h"



WRCombatManager::WRCombatManager()
{
}


WRCombatManager::~WRCombatManager()
{
}

void WRCombatManager::OnInitialize()
{
	LastCombatSeqID = 0;
}

void WRCombatManager::OnShutdown()
{
	LastCombatSeqID = 0;

	for (auto Iter = mapCombatInfo.CreateIterator(); Iter; ++Iter)
	{
		TArray<WRCombatInfo*> setCombatInfo = Iter.Value();
		for (int32 Idx = 0; Idx < setCombatInfo.Num(); ++Idx)
		{
			setCombatInfo[Idx]->Reset();
			delete setCombatInfo[Idx];
		}
	}

	for (auto& iter : CombatInfoPool)
	{
		delete iter;
	}

	mapCombatInfo.Empty();
	CombatInfoPool.Empty();
}

void WRCombatManager::OnLoadLevelStart()
{

}

void WRCombatManager::OnLoadLevelComplete()
{

}

void WRCombatManager::OnRenderStart()
{

}

SKILL_SEQ_t WRCombatManager::GenerateCombatSeqIDForStandalone()
{
	++LastCombatSeqID;

	return LastCombatSeqID;
}

bool WRCombatManager::OnTick(float InDeltaTime)
{
	TArray<TPair<AID_t, WRCombatInfo*>> ToDestroy;
	for (auto Iter = mapCombatInfo.CreateIterator(); Iter; ++Iter)
	{
		TArray<WRCombatInfo*> setCombatInfo = Iter.Value();
		for (int32 Idx = 0; Idx < setCombatInfo.Num(); ++Idx)
		{
			WRCombatInfo* CombatInfo = setCombatInfo[Idx];
			if (nullptr == CombatInfo)
				continue;

			CombatInfo->Tick(InDeltaTime);

			if (CombatInfo->IsReservedDestroy() && CombatInfo->IsDelayedPacketTimerActive() == false)
			{
				TPair<AID_t, WRCombatInfo*> temp(Iter.Key(), CombatInfo);

				ToDestroy.Add(temp);
			}
		}
	}


	for (int32 Idx = 0; Idx < ToDestroy.Num(); ++Idx)
	{
		UnregisterCombat(ToDestroy[Idx].Key, ToDestroy[Idx].Value);
	}

	return true;
}

WRCombatInfo* WRCombatManager::FindBySeqID(AID_t InAID, SKILL_SEQ_t InSeqID)
{
	TArray<WRCombatInfo*> arrCombat = mapCombatInfo.FindRef(InAID);
	if (arrCombat.Num() != 0)
	{
		for (int32 Idx = 0; Idx < arrCombat.Num(); ++Idx)
		{
			if (arrCombat[Idx]->GetCombatSeqID() != InSeqID)
				continue;

			return arrCombat[Idx];
		}
	}

	return nullptr;
}

WRCombatInfo* WRCombatManager::FindLastCombatInfo(AID_t InAID)
{
	TArray<WRCombatInfo*> arrCombat = mapCombatInfo.FindRef(InAID);
	if (arrCombat.Num() != 0)
	{
		int32 Idx = arrCombat.Num() - 1;
		return arrCombat[Idx];
	}

	return nullptr;
}

TArray<WRCombatInfo*> WRCombatManager::FindAll(AID_t InAID)
{
	return mapCombatInfo.FindRef(InAID);	
}

void WRCombatManager::CreateInfo(const FWRCombatParam& InCombatParams)
{
	WRCombatInfo* CombatInfo = GenerateCombatInfo();
	if (nullptr == CombatInfo)
		return;

	CombatInfo->InitializeCombatInfo(InCombatParams);

	RegisterCombat(InCombatParams.AttackerUID, CombatInfo);
}

void WRCombatManager::RegisterCombat(AID_t InAID, WRCombatInfo* InCombat)
{
	if (mapCombatInfo.Contains(InAID))
	{
		TArray<WRCombatInfo*>* arrCombat = mapCombatInfo.Find(InAID);
		arrCombat->Add(InCombat);
	}
	else
	{
		TArray<WRCombatInfo*> temp;
		temp.Add(InCombat);
		mapCombatInfo.Add(InAID, temp);
	}
}

void WRCombatManager::Unregister(SKILL_SEQ_t InCombatSeqID, bool bForce /*= false*/)
{
	WRCombatInfo* CombatInfo = nullptr;
	for (auto iter = mapCombatInfo.CreateIterator(); iter; ++iter)
	{
		TArray<WRCombatInfo*> arrTemp = iter.Value();
		for (int32 Idx = 0; Idx < arrTemp.Num(); ++Idx)
		{
			if (arrTemp[Idx]->GetCombatSeqID() == InCombatSeqID)
			{
				CombatInfo = arrTemp[Idx];
				break;
			}
		}

		if (CombatInfo)
			break;
	}

	Unregister(CombatInfo, bForce);
}

void WRCombatManager::Unregister(WRCombatInfo* InCombat, bool bForce)
{
	if (nullptr == InCombat)
		return;

	if (nullptr == InCombat->GetSkillTable())
		return;
	
	if (false == bForce)
	{
		if (IsProjectile(InCombat->GetSkillTable()))
			return;
	}	

	InCombat->Destroy(bForce);
}

void WRCombatManager::UnregisterCurrentCombat(AID_t InActorID, bool bForce)
{
	TArray<WRCombatInfo*> arrCombat = mapCombatInfo.FindRef(InActorID);
	if (arrCombat.Num() == 0)
		return;

	int32 Idx = arrCombat.Num() - 1;
	WRCombatInfo* CombatInfo = arrCombat[Idx];
	if (nullptr == CombatInfo)
		return;

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(InActorID);
	if (nullptr == pChar)
		return;

	FWRCombatParam CombatParam = CombatInfo->GetCombatSeqParam();
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(CombatParam.SkillTID, pChar->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	if (bForce)
	{
		CombatInfo->Destroy();
	}
	else
	{
		if (IsProjectile(SkillTable))
		{
			if (CombatInfo->IsValidProjectileCombat())
			{
				//이때는 전투정보를 남겨두고 해당 투사체가 할일이 다 끝나면 그때 처리 한다.
			}
			else
			{
				CombatInfo->Destroy();
			}
		}
		else
		{
			CombatInfo->Destroy();
		}
	}
}

void WRCombatManager::UnregisterUsingDeleteTimer(AID_t InCombatSeqID)
{
	WRCombatInfo* CombatInfo = nullptr;
	for (auto iter = mapCombatInfo.CreateIterator(); iter; ++iter)
	{
		TArray<WRCombatInfo*> arrTemp = iter.Value();
		for (int32 Idx = 0; Idx < arrTemp.Num(); ++Idx)
		{
			if (arrTemp[Idx]->GetCombatSeqID() == InCombatSeqID)
			{
				CombatInfo = arrTemp[Idx];
				break;
			}
		}

		if (CombatInfo)
			break;
	}

	if (CombatInfo)
	{
		CombatInfo->DeleteTimer();
	}
}

void WRCombatManager::UnregisterAll(AID_t InActorID)
{
	TArray<WRCombatInfo*> arrCombat = mapCombatInfo.FindRef(InActorID);
	for (int32 Idx = 0; Idx < arrCombat.Num(); ++Idx)
	{
		arrCombat[Idx]->Destroy();
	}
}


bool WRCombatManager::IsProjectile(FWRSkillTable* InTable)
{
	if (nullptr == InTable)
		return false;

	switch (InTable->SkillType)
	{
	case EWRSkillType::ChainProjectile:
	case EWRSkillType::Projectile:
	{
		return true;
	}
	break;
	default:
		break;
	}

	return false;
}

void WRCombatManager::UnregisterCombat(AID_t InActorID, WRCombatInfo* InCombat)
{
	TArray<WRCombatInfo*>* arrCombatInfo = mapCombatInfo.Find(InActorID);
	arrCombatInfo->Remove(InCombat);
	if (arrCombatInfo->Num() == 0)
	{
		mapCombatInfo.Remove(InActorID);
	}

	ReturnToPool(InCombat);
}

void WRCombatManager::UnregisterCombatAll(AID_t InActorID)
{
	TArray<WRCombatInfo*> arrCombatInfo = mapCombatInfo.FindRef(InActorID);
	for (int32 Idx = 0; Idx < arrCombatInfo.Num(); ++Idx)
	{
		ReturnToPool(arrCombatInfo[Idx]);
	}

	mapCombatInfo.Remove(InActorID);
}

WRCombatInfo* WRCombatManager::GenerateCombatInfo()
{
	WRCombatInfo* CombatInfo = nullptr;
	auto Iter = CombatInfoPool.GetHead();
	if (Iter)
	{
		CombatInfo = Iter->GetValue();
		CombatInfoPool.RemoveNode(Iter);
	}
	else
	{
		CombatInfo = new WRCombatInfo();
	}

	return CombatInfo;
}

void WRCombatManager::ReturnToPool(WRCombatInfo* InCombatInfo)
{
	if (nullptr == InCombatInfo)
		return;

	if (nullptr == CombatInfoPool.FindNode(InCombatInfo))
	{
		//RemoveCombatInfoFromCharInfo(InCombatInfo);

		InCombatInfo->Reset();
		CombatInfoPool.AddTail(InCombatInfo);
	}
}
