// Fill out your copyright notice in the Description page of Project Settings.


#include "DMActorComponentFSM.h"
#include "../Manager/DMFSMManager.h"

void UDMActorComponentFSM::BeginPlay()
{
	Super::BeginPlay();

}

void UDMActorComponentFSM::OnCreateComponent()
{
	
}

void UDMActorComponentFSM::OnDestroyComponent()
{
	if (CurrentFSM)
		delete CurrentFSM;
	CurrentFSM = nullptr;
}

bool UDMActorComponentFSM::DetermineFSM(EDMFSMType IN InNewFSM)
{
	ADMCharacterBase* CastedOwner = Cast<ADMCharacterBase>(GetOwner());
	if (CastedOwner == nullptr)
	{
		ensureMsgf(false, TEXT("CastedOwner is Invalid"));
		return false;
	}

	DMFSMBase* NewFSM = DMFSMManager::Get()->FactoryFSM(CastedOwner, InNewFSM);
	if (NewFSM == nullptr)
	{
		ensureMsgf(false, TEXT("NewFSM is Invalid"));
		return false;
	}

	// Old
	if (CurrentFSM != nullptr)
	{
		CurrentFSM->OnExit();
		delete CurrentFSM;
		CurrentFSM = nullptr;
	}

	// New
	CurrentFSMType = InNewFSM;
	CurrentFSM = NewFSM;
	CurrentFSM->SetOwner(CastedOwner);
	CurrentFSM->OnEnter();

	return true;
}
