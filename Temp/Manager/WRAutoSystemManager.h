#pragma once
#include <Platform.h>
#include <Array.h>
#include <List.h>
#include <Queue.h>
#include "CombatSystem/Auto/WRAutoStateBase.h"

struct FWRAutoProcess
{
	EWRAutoProcessType::en ProcessType = EWRAutoProcessType::None;
	TArray<WRAutoStateBase*> arrUpdateAuto;

	int32 QuestTID = 0;
	bool bPause = false;


	bool IsValid()
	{
		return ProcessType != EWRAutoProcessType::None;
	}

	void Reset()
	{
		ProcessType = EWRAutoProcessType::None;
		QuestTID = 0;
		bPause = false;
	}

	bool ProcessTypeIs(EWRAutoProcessType::en InProcess)
	{
		return ProcessType == InProcess;
	}
};


class CAutoSystemBase
{
public:
	CAutoSystemBase();
	virtual ~CAutoSystemBase();


public:
	virtual void Tick(float DeltaTime);

protected:
	void InvokeAutoProcess(EWRAutoProcessType::en InProcessType, const TArray<WRAutoStateBase*>& InAutoInfo, bool bBackupCurrentAuto = false);
	void BackupCurrentProcess();
	void RegistSequenceAutoProcess(EWRAutoProcessType::en InProcessType, const TArray<WRAutoStateBase*>& InAutoInfo);
	void RegistSequenceAutoProcess(const FWRAutoProcess& InUpdateProcess);

private:



private:
	FWRAutoProcess CurrentUpdateProcess;
	TQueue<FWRAutoProcess> BackupAutoProcess;




	//----------------------------- 스테이트 풀 -----------------------------
private:
	template <class T>
	T* GenerateAutoState(EWRAutoSystemState InStateType);

	WRAutoStateBase* GetAutoStateInstanceFromPool(EWRAutoSystemState InType, EWRAutoProcessType::en InProcessType);
	void ReturnToPool(WRAutoStateBase* InAuto);

private:
	TMap<EWRAutoSystemState, TDoubleLinkedList<WRAutoStateBase*>*> AutoStatePool;
	//----------------------------- 스테이트 풀 -----------------------------
};

template <class T>
T* CAutoSystemBase::GenerateAutoState(EWRAutoSystemState InStateType)
{
	WRAutoStateBase* ReturnPtr;
	if (AutoStatePool.Contains(InStateType))
	{
		TDoubleLinkedList<WRAutoStateBase*>* TempList = AutoStatePool[InStateType];
		if (TempList->Num() != 0 && TempList->GetHead())
		{
			ReturnPtr = TempList->GetHead()->GetValue();
		}
		else
		{
			ReturnPtr = new T();
		}

		AutoStatePool[InStateType]->RemoveNode(ReturnPtr);
	}
	else
	{
		ReturnPtr = new T();
	}

	return StaticCast<T*>(ReturnPtr);
}



class CAutoSystemManager : public CAutoSystemBase
{
public:
	CAutoSystemManager();
	virtual ~CAutoSystemManager();

public:
	virtual void Tick(float DeltaTime) override;
};