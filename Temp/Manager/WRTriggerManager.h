#pragma once
#include "CoreMinimal.h"

#include "SingletonClass/WRSingleton.h"
#include "../Enum/EWRTriggerEnum.h"

DECLARE_LOG_CATEGORY_EXTERN(WRTriggerLog, Display, All);

struct FWRTriggerNodeCallData
{
public:
	FWRTriggerNodeCallData()
	{
		this->Clear();
	}

	FWRTriggerNodeCallData(const FString& InTriggerSystemName, const int32& InTriggerUid, EWRTriggerNodeType::en InTransitionType) :
		TriggerSystemName(InTriggerSystemName),
		TriggerUid(InTriggerUid),
		TransitionType(InTransitionType) {}

	void Clear()
	{
		TriggerSystemName.Empty();
		TriggerUid = -1;
		TransitionType = EWRTriggerNodeType::None;
	}

	bool IsValid()
	{
		return TriggerSystemName.IsEmpty() == false;
	}


	FString	TriggerSystemName;
	int32 TriggerUid;
	EWRTriggerNodeType::en TransitionType;
};

class WR_API WRTriggerManager : public WRSingleton<WRTriggerManager>
{

public:
	FORCEINLINE WRTriggerManager() {}
	FORCEINLINE virtual ~WRTriggerManager() {}

protected:
	// Begin IMInstance Interface
	virtual bool OnTick(float InDeltaTime) override;
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;
	virtual void OnRenderStart() override;
	// End IMInstance Interface
	

public:
	void RemoveAll();

	//bool CallTriggerNode(const FWRTriggerNodeCallData& CallData);

	/**
	* [2017-1-4 : magedoga]
	*  @ FileName : Path + Name
	*/
	bool FindTriggerFiles(const FString InFileName, OUT TArray<FString>& FoundFiles);
	bool LoadTrigger(const FString& InFileName, bool bBuild = true);
	void RemoveFollowerID(int32 nActorID);
	void SetCollisionActor(class AWRCharacter* Actor);
	void RemoveCollisionActor(class AWRCharacter* IN InActor);

	const TArray<class WRTriggerActionNodeBase*>* GetSpawnActionNodes(const FString& InName);
	const TArray<class WRTriggerNodeBase*>* GetTriggerNodes(const FString& InName);
	const TArray<class WRTriggerActionNodeBase*>* GetActionNodes(const FString& InName);
	const TArray<class WRTriggerTransitionNodeBase*>* GetTransitionNodes(const FString& InName);
	const TArray<class WRTriggerSpecialNodeBase*>* GetSpecialNodes(const FString& InName);
	const TMap<int32, class WRTriggerNode_MainAreaBox*>* GetMainAreaTriggerNodes(const FString& InName);

	const TArray<class WRTriggerActionNodeBase*>* GetSpawnActionNodes();
	const TArray<class WRTriggerNodeBase*>* GetTriggerNodes();
	const TArray<class WRTriggerActionNodeBase*>* GetActionNodes();
	const TArray<class WRTriggerTransitionNodeBase*>* GetTransitionNodes();
	const TArray<class WRTriggerSpecialNodeBase*>* GetSpecialNodes();
	const TMap<int32, class WRTriggerNode_MainAreaBox*>* GetMainAreaTriggerNodes();

	class WRTriggerBaseNode* FindSpawnActionFromCurrentMapTrigger(int32 TriggerID, EWRTriggerNodeType::en SpawnActionType);

	void RunActiveTrigger(int32 TriggerID);
	void RunDisableTrigger(int32 TriggerID);
		
	void OnResultTriggerBoxEvent(int32 TriggerID, uint32 TransitionType);
	void OnResurrectionRevive();		// [ 2018-6-5 : hansang88 ] 즉시 부활시 처리
	void OnReconnect();					// [ 2018-6-23 : hansang88 ] 서버 재연결시 호출

	void SetStartTriggerRun(bool StartTrigger) { bStartTriggerRun = StartTrigger; }
		
	class WRTriggerBaseNode* FindTriggerNodeFromCurrentMapTrigger(int32 TriggerID);

	void SetUsedTriggerList(const std::list<TID_t>& UsedTriggerTids);
	void LoadUsedTriggerData();
	void SaveUsedTriggerData(int32 TriggerSaveNum);
	bool IsUsedTriggerData(int32 TriggerSaveNum);

	void OnPlayStreamingMovieFinished();
	void AddPlayStreamingPostTriggerIDs(TArray<int32> InPlayStreamingPostTriggerIDs) { PlayStreamingPostTriggerIDs = InPlayStreamingPostTriggerIDs; }

	bool CheckCondition(WRTriggerNodeBase* TriggerNode);
private:
	class WRTriggerSystem* AddTriggerSystem(const TCHAR* InName = nullptr);
	void RemoveTriggerSystem(const TCHAR* InName);


private:
	bool						bStartTriggerRun = false;
	float						bStartElapsedTime = 0.f;

	TMap<FString, class WRTriggerSystem*>	TriggerSystems;
	TMap<int32, TArray<int32>>				UsedTriggerIDs;
	TArray<int32>							PlayStreamingPostTriggerIDs;
};