// Fill out your copyright notice in the Description page of Project Settings.


#include "DMAttackTable.h"
#include "../../Manager/DMCharacterManager.h"

bool FDMAttackTable::IsValidInput(FDMAttackTable* IN InTable, const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	if (InTable == nullptr)
		return false;

	if (InTable->Data.NeedInputKey == InInputType
		&& InTable->Data.NeedInputEvent == InEventType)
	{
		return true;
	}

	return false;
}

FDMAttackTable* FDMAttackTable::GetValidLinkedAttackTable(FDMAttackTable* IN InTable, const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	if (InTable == nullptr)
		return false;

	for (auto AttackRowName : InTable->LinkedAttackRowName)
	{
		FDMAttackTable* AttackTable = DMCharacterManager::Get()->GetAttackTable(AttackRowName);
		if (AttackTable->Data.NeedInputKey == InInputType
			&& AttackTable->Data.NeedInputEvent == InEventType)
		{
			return AttackTable;
		}
	}
	return nullptr;
}
