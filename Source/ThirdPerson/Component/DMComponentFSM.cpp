// Fill out your copyright notice in the Description page of Project Settings.


#include "DMComponentFSM.h"
#include "../Manager/DMFSMManager.h"

void UDMComponentFSM::BeginPlay()
{
	Super::BeginPlay();

}

void UDMComponentFSM::OnCreateComponent()
{
	
}

void UDMComponentFSM::OnDestroyComponent()
{
	if (CurrentFSM)
		delete CurrentFSM;
	CurrentFSM = nullptr;
}

bool UDMComponentFSM::DetermineFSM(FDMFSMData IN InNewFSM)
{
	ADMCharacterBase* CastedOwner = Cast<ADMCharacterBase>(GetOwner());
	if (CastedOwner == nullptr)
	{
		ensureMsgf(false, TEXT("CastedOwner is Invalid"));
		return false;
	}

	if (CurrentFSMData.FSMType != EDMFSMType::Idle
		&& CurrentFSMData.FSMType == InNewFSM.FSMType)
	{
		CurrentFSMData = InNewFSM;
		CurrentFSM->OnReEnter(CurrentFSMData);
	}
	else
	{
		DMFSMBase* NewFSM = DMFSMManager::Get()->FactoryFSM(CastedOwner, InNewFSM.FSMType);
		if (NewFSM == nullptr)
		{
			ensureMsgf(false, TEXT("NewFSM is Invalid"));
			return false;
		}

		// Old
		if (CurrentFSM != nullptr)
		{
			CurrentFSMData.Release();
			CurrentFSM->OnExit();
			delete CurrentFSM;
			CurrentFSM = nullptr;
		}

		// New
		CurrentFSMData = InNewFSM;
		CurrentFSM = NewFSM;
		CurrentFSM->SetOwner(CastedOwner);
		CurrentFSM->OnEnter(CurrentFSMData);
	}	

	return true;
}

void UDMComponentFSM::OnMontageEnded(UAnimMontage* InMontage, bool InbInterrupted)
{
	if (InbInterrupted)
		return;

	if (CurrentFSM)
	{
		CurrentFSMData.Release();
		CurrentFSM->OnExit();
		delete CurrentFSM;
		CurrentFSM = nullptr;
	}
}

void UDMComponentFSM::GetCurrentFSMData(FDMFSMData& OUT OutFSMData)
{
	OutFSMData = CurrentFSMData;
}
