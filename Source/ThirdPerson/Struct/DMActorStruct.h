// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enum/DMActorEnum.h"
#include "../Data/CustomData/DMAttackTable.h"

struct FDMFSMData
{
	FDMFSMData() {}
	FDMFSMData(EDMFSMType InFSMType)
		: FSMType(InFSMType)
	{

	}
	FDMFSMData(FDMAttackTable* InAttackTable)
		: AttackTable(InAttackTable), FSMType(InAttackTable->FSMType)
	{

	}

	void Release()
	{
		FSMType = EDMFSMType::Idle;
	}

	FDMAttackTable* AttackTable = nullptr;
	EDMFSMType FSMType = EDMFSMType::Idle;
};