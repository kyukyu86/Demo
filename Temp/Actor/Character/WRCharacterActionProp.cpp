// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterActionProp.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRTODManager.h"
#include "Utility/WRActorUtility.h"

AWRCharacterActionProp::AWRCharacterActionProp()
{

}

AWRCharacterActionProp::AWRCharacterActionProp(const FObjectInitializer& ObjectInitializer)
	: AWRCharacterObjectBase(ObjectInitializer)
{

}

void AWRCharacterActionProp::BeginPlay()
{
	Super::BeginPlay();

	CollectMIDs();

}

void AWRCharacterActionProp::OnAnimation(const EWRActionPropAnimType IN InAnimType, const bool IN InLoop)
{
	// 이전의 Async 해제
	if (AsyncLoadKey.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadKey);
	}

	TSoftObjectPtr<class UAnimSequenceBase> AnimSeqBase = AnimList.FindRef(InAnimType);
	if (AnimSeqBase.IsNull() == false)
	{
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
		{
			AsyncLoadKey.Empty();

			UAnimSequenceBase* CastedAnimSeq = Cast<UAnimSequenceBase>(InObject);
			if (CastedAnimSeq == nullptr)
				return;

			PlayAnimationSequence(CastedAnimSeq, InLoop);
		});

		AsyncLoadKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(AnimSeqBase.ToString(), AsyncCreateComplete);// FWRCompleteAsyncLoad::CreateUObject(this, &AWRCharacterActionProp::OnAsyncLoadComplete));
	}
}

void AWRCharacterActionProp::TickForTODResponse()
{
	float CurrentTODHour = WRTODManager::Get()->GetCurrent();

	// 시간은 0 ~ 23 시간이다.

	EWRAPState::en NewState = EWRAPState::Close;

	// 시작시간이 종료시간보다 클 때 (3시 ~ 5시)
	if (ActionPropTable->TODStartHour < ActionPropTable->TODFinishHour)
	{
		if (ActionPropTable->TODStartHour <= CurrentTODHour && CurrentTODHour < ActionPropTable->TODFinishHour)
		{
			NewState = EWRAPState::Open;
		}
	}
	// 시작시간이 종료시간보다 작을 때 (23시 ~ 1시)
	else if (ActionPropTable->TODStartHour > ActionPropTable->TODFinishHour)
	{
		if (ActionPropTable->TODStartHour <= CurrentTODHour && CurrentTODHour <= 0)
		{
			NewState = EWRAPState::Open;
		}
		else if (0 <= CurrentTODHour && CurrentTODHour < ActionPropTable->TODFinishHour)
		{
			NewState = EWRAPState::Open;
		}
	}

	OnChangeState(NewState);
}

void AWRCharacterActionProp::TickForTrigger(const float IN InDeltatime)
{
	// [ 2020-8-28 : kyu ] Plane Material Effect
	UpdateMIDEffect(InDeltatime);

}

void AWRCharacterActionProp::CollectMIDs()
{
	TArray<UActorComponent*> GettedComponents = this->GetComponentsByTag(UStaticMeshComponent::StaticClass(), "MIDEffect");
	if (GettedComponents.Num() != 0)
	{
		for (auto& Comp : GettedComponents)
		{
			UStaticMeshComponent* CastedComp = Cast<UStaticMeshComponent>(Comp);
			if (CastedComp != nullptr)
			{
				TArray<UMaterialInstanceDynamic*> OutputMIDs;
				WRActorUtility::GetMeshMaterialInstanceDynamic(CastedComp, OutputMIDs);
				for (int32 i = 0; i < OutputMIDs.Num(); ++i)
				{
					UMaterialInstanceDynamic* MID = OutputMIDs[i];
					if (MID != nullptr)
					{
						MIDs.AddTail(MID);
					}
				}
			}
		}
	}
}

void AWRCharacterActionProp::OnMIDEffect(const bool IN InActivate)
{
	IsMIDActive = InActivate;

	if (IsMIDActive)
		MIDElapsedTime = 0.f;
	else
		MIDElapsedTime = 1.f;
}

void AWRCharacterActionProp::UpdateMIDEffect(const float IN InDeltatime)
{
	if (IsIdle)
		return;

	if (IsMIDActive)
	{
		if (MIDElapsedTime >= 1.f)
			return;

		MIDElapsedTime += InDeltatime;
		if (MIDElapsedTime > 1.f)
			MIDElapsedTime = 1.f;
	}
	else
	{
		if (MIDElapsedTime <= 0.f)
			return;

		MIDElapsedTime -= InDeltatime;
		if (MIDElapsedTime < 0.f)
			MIDElapsedTime = 0.f;
	}
	
	for (auto& MID : MIDs)
	{
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue("OpenMain", MIDElapsedTime);
		}
	}
}

void AWRCharacterActionProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActionPropTable == nullptr)
		return;

	switch (ActionPropTable->ResponseType)
	{
	case EWRAPResponseType::TOD:
	{
		TickForTODResponse();
	}
	break;
	case EWRAPResponseType::Trigger:
	{
		TickForTrigger(DeltaTime);
	}
	break;
	}	
}

void AWRCharacterActionProp::OnPreDestroy()
{
	if (AsyncLoadKey.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadKey);
	}

	Super::OnPreDestroy();
}

void AWRCharacterActionProp::OnAnimEnded(UAnimSequenceBase* AnimSeq)
{
	switch (CurrentAPState)
	{
	case EWRAPState::Open:
	{
		if (IsIdle == false)
		{
			OnAnimation(EWRActionPropAnimType::TransformIdle, true);
			IsIdle = true;
		}
	}
	break;
	case EWRAPState::Close:
	{
		if (IsIdle == false)
		{
			OnAnimation(EWRActionPropAnimType::NormalIdle, true);
			IsIdle = true;
		}
	}
	break;
	}
}

void AWRCharacterActionProp::OnSpawnComplete(const pt_Game_Actor::Inf_APAppear_G2U& InInform, FWRActionPropTable* IN InTableData, const bool IN InManualSpawn /*= false*/)
{
	ActionPropTable = InTableData;

	if (InManualSpawn == false)
	{
		// Location Set
		SetLocation(FVector(InInform.posX, InInform.posY, InInform.posZ), true);
	}

	OnChangeState((EWRAPState::en)InInform.apState, true);
}

void AWRCharacterActionProp::OnChangeState(const EWRAPState::en IN InState, const bool IN InSpawn /* = false */)
{
	if (CurrentAPState == InState)
		return;

	CurrentAPState = InState;

	switch (InState)
	{
	case EWRAPState::Open:
	{
		if (InSpawn)
		{
			OnAnimation(EWRActionPropAnimType::TransformIdle, true);
			IsIdle = true;
		}
		else
		{
			OnAnimation(EWRActionPropAnimType::TransformActivate, false);
			IsIdle = false;

			OnMIDEffect(true);
		}
	}
	break;
	case EWRAPState::Close:
	{
		if (InSpawn)
		{
			OnAnimation(EWRActionPropAnimType::NormalIdle, true);
			IsIdle = true;
		}
		else
		{
			OnAnimation(EWRActionPropAnimType::TransformDeactivate, false);
			IsIdle = false;

			OnMIDEffect(false);
		}
	}
	break;
	}
}