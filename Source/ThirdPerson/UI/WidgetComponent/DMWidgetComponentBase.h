// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "../../Enum/DMUIEnum.h"
#include "DMWidgetComponentBase.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class THIRDPERSON_API UDMWidgetComponentBase : public UWidgetComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|BackGroundWidget", meta = (AllowPrivateAccess = "true"))
		bool bUseBGWidgetComp = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|BackGroundWidget", meta = (AllowPrivateAccess = "true", EditCondition = "bUseBGWidgetComp"))
		TSubclassOf<UWidgetComponent> BGWidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|BackGroundWidget", meta = (AllowPrivateAccess = "true", EditCondition = "bUseBGWidgetComp"))
		FVector BGOffset = FVector::ZeroVector;

private:
	EDMWidgetComponentFlag Flags = EDMWidgetComponentFlag::None;
	UWidgetComponent* BackGroundWidgetComponent = nullptr;

private:
	FVector GetCameraLocation();
	FRotator GetCameraRotation();

	void UpdateBillboard();

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetFlag(EDMWidgetComponentFlag& IN InFlags);
};






/*
const FString strPath = "/Game/Blueprints/Actor/WidgetActor/WidgetActorBG.WidgetActorBG_C";
//TSubclassOf<UObject> LoadTest = DMActorUtil::GetGeneratedClass(strPath);

FStringAssetReference rAssetRef = FStringAssetReference(strPath);
UObject* pObject = rAssetRef.ResolveObject();
if (pObject == nullptr)
{
	pObject = rAssetRef.TryLoad();
}
if(BackGroundWidgetComponent == nullptr)
{
	BackGroundWidgetComponent = Cast<UWidgetComponent>(pObject);
}
if (BackGroundWidgetComponent == nullptr)
{
	pObject = StaticLoadObject(UWidgetComponent::StaticClass(), this, *strPath);
	BackGroundWidgetComponent = Cast<UWidgetComponent>(pObject);
}
if (BackGroundWidgetComponent == nullptr)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	UObject* pNewObject = StreamableManager.LoadSynchronous(strPath);
	if (pNewObject != nullptr)
	{
		BackGroundWidgetComponent = Cast<UWidgetComponent>(pNewObject);
	}
	if (BackGroundWidgetComponent == nullptr)
	{
		BackGroundWidgetComponent = LoadObject<UWidgetComponent>(nullptr, *strPath);
	}
	if (BackGroundWidgetComponent == nullptr)
	{
		BackGroundWidgetComponent = NewObject<UWidgetComponent>(this, (UClass*)pNewObject);
	}
	if (BackGroundWidgetComponent == nullptr)
	{
		FStringAssetReference AssetRef = FStringAssetReference(pNewObject);
		if (AssetRef.IsValid())
		{
			FString str = pNewObject->GetClass()->GetDescription();
			str += "'";
			str += AssetRef.ToString();
			str += "'";

			UObject* LoadedObject = StaticLoadObject(UWidgetComponent::StaticClass(), NULL, *strPath);
		}
	}
}
*/