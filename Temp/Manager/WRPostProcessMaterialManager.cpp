#include "WRPostProcessMaterialManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "Camera/CameraComponent.h"
#include "WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"


WRPostProcessMaterialManager::WRPostProcessMaterialManager()
{

}

WRPostProcessMaterialManager::~WRPostProcessMaterialManager()
{

}

void WRPostProcessMaterialManager::OnInitialize()
{
	//LoadMaterial(EWRPostProcessType::Damaged, TEXT("/Game/Asset/NewMaterial.NewMaterial"));
}

void WRPostProcessMaterialManager::OnShutdown()
{
	for (auto& LoadingMaterial : LoadingMaterials)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(LoadingMaterial.Key);
	}
	LoadingMaterials.Empty();

	for (auto& Material : Materials)
	{
		Material.Value->RemoveFromRoot();
		Material.Value = nullptr;
	}
	Materials.Empty();
}

void WRPostProcessMaterialManager::OnLoadLevelStart()
{

}

void WRPostProcessMaterialManager::OnLoadLevelComplete()
{

}

void WRPostProcessMaterialManager::OnRenderStart()
{

}

bool WRPostProcessMaterialManager::OnTick(float InDeltaTime)
{

	return true;
}

void WRPostProcessMaterialManager::PostProcessMaterialSetting(UCameraComponent* InCamera)
{
	if (InCamera->IsValidLowLevel() == true)
	{
		for (auto& Material : Materials)
		{
			InCamera->AddOrUpdateBlendable(Material.Value, 0.f);
		}
	}
}

void WRPostProcessMaterialManager::LoadMaterial(EWRPostProcessType Type, FString FileName)
{
	if (LoadingMaterials.FindKey(Type) != nullptr)
		return;

	FString Key = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(FileName, FWRCompleteAsyncLoad::CreateRaw(this, &WRPostProcessMaterialManager::OnLoadMaterialComplete));
	LoadingMaterials.Add(Key, Type);
}

void WRPostProcessMaterialManager::OnLoadMaterialComplete(UObject* InObject, FString InKey)
{
	if (LoadingMaterials.Contains(InKey) == false)
		return;
	
	UMaterialInterface* Material = Cast<UMaterialInterface>(InObject);
	if (Material != nullptr)
	{
		Material->AddToRoot();
		Materials.Add(LoadingMaterials[InKey], Material);

		LoadingMaterials.Remove(InKey);
	}
}

void WRPostProcessMaterialManager::AddOrUpdateBlendable(EWRPostProcessType Type, float InValue)
{
	if (Materials.Contains(Type) == true)
	{
		AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar)
		{
			UCameraComponent* Camera = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
			if (Camera)
			{
				Camera->AddOrUpdateBlendable(Materials[Type], InValue);
			}
		}
	}
}

void WRPostProcessMaterialManager::SetPPDamageWeight(float InValue)
{
	AddOrUpdateBlendable(EWRPostProcessType::Damaged, InValue);
}
