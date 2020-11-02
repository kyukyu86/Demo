// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/WRCharacterObjectBase.h"
#include "Network/Share/Define/Define_Actor.h"
#include "List.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WRCharacterActionProp.generated.h"


UENUM(BlueprintType)
enum class EWRActionPropAnimType : uint8
{
	None,
	TransformActivate,
	TransformDeactivate,
	NormalIdle,
	TransformIdle,
};


UCLASS()
class WR_API AWRCharacterActionProp : public AWRCharacterObjectBase
{
	GENERATED_BODY()

public:
	AWRCharacterActionProp();
	AWRCharacterActionProp(const FObjectInitializer& ObjectInitializer);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		TMap<EWRActionPropAnimType, TSoftObjectPtr<class UAnimSequenceBase>> AnimList;

	EWRAPState::en CurrentAPState = EWRAPState::MAX;
	struct FWRActionPropTable* ActionPropTable = nullptr;
	FString AsyncLoadKey = "";
	
	bool IsIdle = false;

	// [ 2020-8-28 : kyu ] Plane Material Effect
	TDoubleLinkedList<UMaterialInstanceDynamic*> MIDs;
	bool IsMIDActive = false;
	float MIDElapsedTime = 0.f;
	
private:
	void OnAnimation(const EWRActionPropAnimType IN InAnimType, const bool IN InLoop);
	void TickForTODResponse();
	void TickForTrigger(const float IN InDeltatime);

	// [ 2020-8-28 : kyu ] Plane Material Effect
	void CollectMIDs();
	void OnMIDEffect(const bool IN InActivate);
	void UpdateMIDEffect(const float IN InDeltatime);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPreDestroy() override;
	virtual void OnAnimEnded(UAnimSequenceBase* AnimSeq) override;
	void OnSpawnComplete(const pt_Game_Actor::Inf_APAppear_G2U& InInform, FWRActionPropTable* IN InTableData, const bool IN InManualSpawn = false);
	void OnChangeState(const EWRAPState::en IN InState, const bool IN InSpawn = false);

};
