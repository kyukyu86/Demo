// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentBuff.h"
#include "Network/Share/Define/Define_Skill.h"



void UWRActorComponentBuff::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRActorComponentBuff::OnDestroyComponent()
{
	for (auto& Each : Buffs)
	{		
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			delete Element;
		}
		List.Empty();
	}
	Buffs.Empty();

	Super::OnDestroyComponent();
}

void UWRActorComponentBuff::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto Each : Buffs)
	{
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			if(Element->Activate == false)
				continue;

			Element->RemainTime -= DeltaTime;
			if (Element->RemainTime < 0.f)
			{				
				Element->Activate = false;
				Element->RemainTime = 0.f;

				// Remove...서버에서 삭제 날려줄것임
			}
		}
	}
}

//====================================================================================

bool UWRActorComponentBuff::AddBuff(const stBuffInfo& IN InBuffInfo)
{
	FWRBuff* NewBuff = nullptr;
	if (WRBuffManager::Get()->MakeBuff(InBuffInfo, NewBuff))
	{
		if (NewBuff == nullptr)
		{
			ensureMsgf(false, TEXT("Invalid Buff"));
			return false;
		}

		if (NewBuff->pBuffTable->Type == EWRBuffType::Buff)
		{
			TArray<FWRBuff*>& List = Buffs.FindOrAdd(EWRBuffType::Buff);
			List.Add(NewBuff);
		}
		else if (NewBuff->pBuffTable->Type == EWRBuffType::DeBuff)
		{
			TArray<FWRBuff*>& List = Buffs.FindOrAdd(EWRBuffType::DeBuff);
			List.Add(NewBuff);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void UWRActorComponentBuff::RemoveBuff(const WRTableID IN InBuffTID)
{
	for (auto& Each : Buffs)
	{
		FWRBuff* pRemoveBuff = nullptr;
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			if (Element->BuffTID == InBuffTID)
			{
				pRemoveBuff = Element;
				break;
			}
		}

		if (pRemoveBuff != nullptr)
		{
			List.Remove(pRemoveBuff);
			delete pRemoveBuff;
		}
	}
}

void UWRActorComponentBuff::ChangeBuffStack(const WRTableID IN InBuffTID, const int32 IN InStack)
{
	for (auto& Each : Buffs)
	{
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			if (Element->BuffTID == InBuffTID)
			{
				Element->Stack = InStack;

				// 스택이 쌓였을 때 RemainTm 초기화는?

				return;
			}
		}
	}
}

bool UWRActorComponentBuff::HaveBuff(const WRTableID IN InBuffTID)
{
	for (auto& Each : Buffs)
	{
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			if (Element->BuffTID == InBuffTID)
			{
				if (Element->Activate)
				{
					return true; // 이건 좀 생각중
				}
			}
		}
	}
	return false;
}

float UWRActorComponentBuff::GetRemainTime(const WRTableID IN InBuffTID)
{
	for (auto& Each : Buffs)
	{
		TArray<FWRBuff*>& List = Each.Value;
		for (auto& Element : List)
		{
			if (Element->BuffTID == InBuffTID)
				return Element->RemainTime;
		}
	}
	return 0.f;
}

TArray<FWRBuff*>* UWRActorComponentBuff::GetBuffs(const EWRBuffType IN InType)
{
	TArray<FWRBuff*>* List = Buffs.Find(InType);
	if (List == nullptr || List->Num() == 0)
		return nullptr;

	return List;
}
