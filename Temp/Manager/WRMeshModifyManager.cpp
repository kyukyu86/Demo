// Fill out your copyright notice in the Description page of Project Settings.


#include "WRMeshModifyManager.h"


WRMeshModifyManager::WRMeshModifyManager()
{
}

WRMeshModifyManager::~WRMeshModifyManager()
{
}

void WRMeshModifyManager::RemovModifiedSkelMeshs()
{
	for (auto& Element : SlicedCharacters)
	{
		AWRCharacter* InCharacter = Element.Value;
		
		//if(InCharacter == nullptr)
			//continue;
		if (InCharacter->IsValidLowLevel() == false)
			return;

		InCharacter->RemoveFromRoot();
		InCharacter->OnPreDestroy();
		InCharacter->Destroy();
	}
}

void WRMeshModifyManager::OnInitialize()
{
}

void WRMeshModifyManager::OnShutdown()
{
	this->RemovModifiedSkelMeshs();
}

void WRMeshModifyManager::OnLoadLevelStart()
{
	this->RemovModifiedSkelMeshs();
}

void WRMeshModifyManager::OnLoadLevelComplete()
{
}

void WRMeshModifyManager::OnRenderStart()
{
}

bool WRMeshModifyManager::OnTick(float InDeltaTime)
{
	return false;
}
