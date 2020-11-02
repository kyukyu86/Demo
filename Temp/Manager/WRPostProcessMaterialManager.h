#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRPostProcessMaterial.h"
#include "Materials/MaterialInterface.h"

class WR_API WRPostProcessMaterialManager : public WRSingleton<WRPostProcessMaterialManager>
{
	CDECLARE_MANAGER(WRPostProcessMaterialManager);

private:
	WRPostProcessMaterialManager();
	virtual ~WRPostProcessMaterialManager();

public:
	virtual void OnInitialize() override;

	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

	virtual bool OnTick(float InDeltaTime) override;

	void PostProcessMaterialSetting(class UCameraComponent* InCamera);

private:

	void LoadMaterial(EWRPostProcessType Type, FString FileName);
	void OnLoadMaterialComplete(UObject* InObject, FString InKey);
	void AddOrUpdateBlendable(EWRPostProcessType Type, float InValue);
	
	//TWeakPtr<class UCameraComponent> PostProcessCamera;
	TMap<FString, EWRPostProcessType>		LoadingMaterials;
	TMap<EWRPostProcessType, class UMaterialInterface*> Materials;
public:

	void SetPPDamageWeight(float InValue);
};