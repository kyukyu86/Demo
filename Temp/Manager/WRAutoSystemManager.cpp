#include "WRAutoSystemManager.h"
#include "CombatSystem/Auto/WRAutoStateBase.h"



CAutoSystemBase::CAutoSystemBase()
{
}


CAutoSystemBase::~CAutoSystemBase()
{
}

void CAutoSystemBase::Tick(float DeltaTime)
{
	if (CurrentUpdateProcess.IsValid())
	{
		if (CurrentUpdateProcess.bPause)
			return;

		for (int32 Index = 0; Index < CurrentUpdateProcess.arrUpdateAuto.Num(); ++Index)
		{
			if (CurrentUpdateProcess.arrUpdateAuto.IsValidIndex(Index))
			{
				WRAutoStateBase* UpdateState = CurrentUpdateProcess.arrUpdateAuto[Index];
				if (nullptr != UpdateState)
				{
					UpdateState->Tick(DeltaTime);
				}
			}
		}
	}
	else
	{
		if (BackupAutoProcess.IsEmpty() == false)
		{
			FWRAutoProcess backupProcess;
			if (BackupAutoProcess.Dequeue(backupProcess))
			{
				InvokeAutoProcess(backupProcess.ProcessType, backupProcess.arrUpdateAuto);
			}
		}
	}
}

void CAutoSystemBase::InvokeAutoProcess(EWRAutoProcessType::en InProcessType, const TArray<WRAutoStateBase*>& InAutoInfo, bool bBackupCurrentAuto)
{
	if (CurrentUpdateProcess.IsValid())
	{
		if (bBackupCurrentAuto)
		{
			BackupCurrentProcess();
		}
		else
		{
			for (int32 index = 0; index < CurrentUpdateProcess.arrUpdateAuto.Num(); ++index)
			{
				CurrentUpdateProcess.arrUpdateAuto[index]->Stop();
				ReturnToPool(CurrentUpdateProcess.arrUpdateAuto[index]);
			}
		}
	}

	CurrentUpdateProcess.ProcessType = InProcessType;
	for (int32 Index = 0; Index < InAutoInfo.Num(); ++Index)
	{
		CurrentUpdateProcess.arrUpdateAuto.Add(InAutoInfo[Index]);
	}

	//준비 시작
	for (int32 Index = 0; Index < CurrentUpdateProcess.arrUpdateAuto.Num(); ++Index)
	{
		CurrentUpdateProcess.arrUpdateAuto[Index]->Begin();
	}
}

void CAutoSystemBase::BackupCurrentProcess()
{
	if (CurrentUpdateProcess.IsValid() == false)
		return;

	RegistSequenceAutoProcess(CurrentUpdateProcess);
}

void CAutoSystemBase::RegistSequenceAutoProcess(EWRAutoProcessType::en InProcessType, const TArray<WRAutoStateBase*>& InAutoInfo)
{
	FWRAutoProcess tempProcess;
	tempProcess.ProcessType = InProcessType;
	for (int32 Index = 0; Index < InAutoInfo.Num(); ++Index)
	{
		tempProcess.arrUpdateAuto.Add(InAutoInfo[Index]);
	}

	RegistSequenceAutoProcess(tempProcess);
}

void CAutoSystemBase::RegistSequenceAutoProcess(const FWRAutoProcess& InUpdateProcess)
{
	BackupAutoProcess.Enqueue(InUpdateProcess);
}

//----------------------------- 스테이트 풀 -----------------------------
WRAutoStateBase* CAutoSystemBase::GetAutoStateInstanceFromPool(EWRAutoSystemState InType, EWRAutoProcessType::en InProcessType)
{
	WRAutoStateBase* AutoStatePtr = nullptr;
	switch (InType)
	{
	case EWRAutoSystemState::CommonProcess:
	{
		AutoStatePtr = GenerateAutoState<WRAutoStateBase>(InType);
	}
	break;
	case EWRAutoSystemState::ChaseTo:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateChaseTo>(InType);
	}
	break;
	case EWRAutoSystemState::Escort:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateEscort>(InType);
	}
	break;
	case EWRAutoSystemState::Searching:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateFindEnemy>(InType);
	}
	break;
	case EWRAutoSystemState::InteractTo:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateInteractTo>(InType);
	}
	break;
	case EWRAutoSystemState::MoveTo:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateMoveTo>(InType);
	}
	break;
	case EWRAutoSystemState::TalkTo:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateTalkTo>(InType);
	}
	break;
	case EWRAutoSystemState::Taming:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateTaming>(InType);
	}
	break;
	case EWRAutoSystemState::Counter:
	default:
	{
		//AutoStatePtr = GenerateAutoState<IMAutoStateCounter>(InType);
	}
	break;
	}

	if (AutoStatePtr != nullptr)
	{
		AutoStatePtr->AutoProcessType = InProcessType;
	}

	return AutoStatePtr;
}

void CAutoSystemBase::ReturnToPool(WRAutoStateBase* InAuto)
{
	if (nullptr == InAuto)
		return;

	InAuto->ResetState();

	if (AutoStatePool.Contains(InAuto->GetAutoStateType()))
	{
		AutoStatePool[InAuto->GetAutoStateType()]->AddTail(InAuto);
	}
	else
	{
		TDoubleLinkedList<WRAutoStateBase*>* TempList = new TDoubleLinkedList<WRAutoStateBase*>();

		TempList->AddTail(InAuto);
		AutoStatePool.Add(InAuto->GetAutoStateType(), TempList);
	}
}

CAutoSystemManager::CAutoSystemManager()
{

}

CAutoSystemManager::~CAutoSystemManager()
{

}

void CAutoSystemManager::Tick(float DeltaTime)
{
	CAutoSystemBase::Tick(DeltaTime);	
}

