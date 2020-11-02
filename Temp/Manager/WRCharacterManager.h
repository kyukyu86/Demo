// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SingletonClass/WRSingleton.h"
#include "Core/WRGameInstance.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Enum/EWRCharacter.h"
#include "Actor/Character/WRCharacter.h"
#include "Network/WRProtocolHeaders.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRPathManager.h"
#include "../Enum/EWRPath.h"
#include "WRMapManager.h"
#include "Table/WRCharacterPresetTable.h"
#include "Network/Share/Define/Define_Actor.h"


// For Header
DECLARE_LOG_CATEGORY_EXTERN(CharacterManager, Display, All);

DECLARE_MULTICAST_DELEGATE(FWRMyCharacterStartMoveDelegate);
DECLARE_MULTICAST_DELEGATE(FWRMyCharacterStopMoveDelegate);
DECLARE_MULTICAST_DELEGATE(FWRMyCharacterTargetChangedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FWRMyCharacterSpawnedDelegate, AWRCharacterMine*);

struct FWRCharacterSpawnParams
{
public:
	FWRCharacterSpawnParams() 
	{

	}
	FWRCharacterSpawnParams(const AID_t& InActorID, const WRTableID& InCharacterTID, const FString& InBPPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale = FVector::OneVector, int32 InTriggerSpawnID = 0, EWRChatType::en InChatType = EWRChatType::en::Normal)
		: ActorID(InActorID)
		, CharacterTableID(InCharacterTID)
		, BPPath(InBPPath)
		, Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
		, TriggerSpawnID(InTriggerSpawnID)
		, ChatType(InChatType)
	{}

	AID_t ActorID;
	WRTableID CharacterTableID;
	FString BPPath;
	FVector Location;
	FRotator Rotation;
	FVector Scale;
	int32	TriggerSpawnID;
	EWRChatType::en ChatType;
};

struct FWRSkeletalMeshMergeTask
{
public:
	FWRSkeletalMeshMergeTask() {
		MeshesToMerge.Empty();
	}

	void Clear() {
		IsCompleted = false;
		MeshesToMerge.Empty();
		Skeleton = nullptr;
		PhysicsAsset = nullptr;

		for (auto& Object : Objects)
		{
			if (Object.Value == nullptr)
			{
				WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(Object.Key);
			}
			else
			{
				Object.Value->RemoveFromRoot();
			}
		}
		Objects.Empty();
	}

public:
	bool IsCompleted = false;

	TArray<USkeletalMesh*> MeshesToMerge;

	USkeleton* Skeleton = nullptr;

	UPhysicsAsset* PhysicsAsset = nullptr;

	TMap<FString, UObject*> Objects;
};

struct FWRReservedInvinsibleInfo
{
	FWRReservedInvinsibleInfo(const bool InIsVisible, const bool InIsUseFade)
		: IsVisible(InIsVisible)
		, IsUseFade(InIsUseFade)
	{}

	bool IsVisible;
	bool IsUseFade;
};
/**
*
*/
class AWRCharacterNPC;
class AActor;
struct FWRNPCBaseTable;
class WR_API WRCharacterManager : public WRSingleton<WRCharacterManager>
{
	CDECLARE_MANAGER(WRCharacterManager);
	
private:
	WRCharacterManager();
	virtual ~WRCharacterManager();
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual void EndLoadLevelProcess() override;
	
	virtual bool OnTick(float InDeltaTime) override;

	template <class TCharacterType>
	TCharacterType* Spawn(const FWRCharacterSpawnParams& InParam, bool bInIsMe = false, bool bInIgnoreCheckMapLoading = false);
	void OnCompleteSpawn(const FWRCharacterSpawnParams& InParam, class AWRCharacter* InSpawnedChar, bool bInIsMe);
	
	//void Remove(AID_t InAID);
	//void Remove(class AWRCharacter* InCharacter);
	void Remove(AID_t InAID, bool IsDisplaying = false);
	void Remove(class AWRCharacter* InCharacter, bool IsDisplaying = false);

	//====================================================================================
	// Tick Process
	void TickSpawn(float DeltaTime);
private:
	void TickDistanceCulling(float DeltaTime);

	void SetDistanceCulling(const bool IN bInCulling, class AWRCharacter* InCharacter);
public:
	void OnPostSpawned(FWRCharacterSpawnParams& SpawnParam, AWRCharacter* InActor, const bool IN InModifyZ = true);

	//player
	void SpawnMe();
	void DetermineSpawnPlayer(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform);
	void SpawnOnBoardingPlayerWhenVehicleSpawnCompleted(AID_t InAID);
	void SpawnPlayer(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform);
	void LoadPlayerMontage(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform);
	void LoadPlayerWeaponSkillMontage(WRTableID InWeaponTID);
	void DetermineOnSpawnPlayerComplete(UObject* InActor, FString InHashKey);
	void OnSpawnPlayerComplete(UObject* InActor, FString InHashKey);

	bool RemovePlayerInfo(AID_t InAID);
	void ModifyPlayerLocation(AID_t InAID, FVector InLocation);
	void ModifyPlayerOffsetTransformInVehicle(AID_t InAID, FVector InOffset, float InAngle);
	void ModifyOwnedVehicleAID(AID_t InAID, AID_t InVehicleAID);
	void ModifyBoardingVehicleInfo(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex);

	//npc
	void SpawnNpc(const pt_Game_Actor::Inf_NpcAppear_G2U& InInform, bool bImmediately = false);
	void LoadNpcMontage(const FWRNPCBaseTable* InNpcBaseTableNpcBaseTable);
	void OnSpawnNpcCompleted(UObject* InActor, FString InHashKey);

	//Asnyc시 삭제
//	bool SpawnNpc(const pt_Game_Actor::Inf_NpcAppear_G2U& InInform);
	//============


	void RemoveNpcInfo(AID_t InAID);
	bool ModifyNpcAIMoveToPosition(const pt_Game_User::Inf_ActorMoveToPos_G2U& InInform);
	void ModifyHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform);

	//IO
	void SpawnIO(const pt_Game_Actor::Inf_IOAppear_G2U& InInform);
	void OnSpawnIOCompleted(UObject* InActor, FString InHashKey);

	//SO
	void SpawnSO(const pt_Game_Actor::Inf_SOAppear_G2U& InInform);

	//AP
	void SpawnAP(const pt_Game_Actor::Inf_APAppear_G2U& InInform);
	void OnSpawnAPComplete(UObject* InActor, FString InHashKey);
	void OnChangeAPState(const pt_Game_Actor::Inf_ChangeAPState_G2U& InInform);

	//Preset
	AWRCharacter* SpawnPresetCharacters(const WRTableID IN InTableID);
	void ChangeMyPreset(const WRTableID IN InTableID, const bool bUsingHologram = false);
	void ChangePreset(const AID_t IN InAID, const WRTableID IN InTableID, const bool IN bUsingHologram = false);
	FString GetPresetBPPath(const FWRCharacterPresetTable* IN InPresetTable);
	FString GetMyCharacterBPPath();

	//====================================================================================
	// [ 2019-7-23 : magedoga ] Find functions
	
	// [ 2019-8-27 : magedoga ] by Index
	class AWRCharacter* FindbyIndex(const int32 InIndex);
	template <class TCharacterType>
	TCharacterType* FindbyIndex_Casted(const int32 InIndex);

	// [ 2019-8-27 : magedoga ] by AID
	class AWRCharacter* FindbyAID(AID_t InAID);
	class AWRCharacter* FindByTID(const WRTableID IN InCharacterTID);
	void FindAllByTID(const WRTableID IN InCharacterTID, TDoubleLinkedList<class AWRCharacter*>& OUT OutCharacterList);
	template <class TCharacterType>
	TCharacterType* FindbyAID_Casted(AID_t InAID);

	class AWRCharacter* FindCharacterBySpawnTid(const int32 InSpawnTid);
	const FWRNPCBaseTable* FindNpcBaseTable(const WRTableID IN InTableID);
	class AWRCharacterIO* FindIOByResultAttachID(const int32 InID);
	void FindByNpcJobType(const EWRNpcJobType IN InNpcJobType, TDoubleLinkedList<AWRCharacterNPC*>& OUT OutNpcCharacter);
	void GetActorListBasedOnFactionRelation(class AWRCharacter* InFinder, EWRFactionRelation InRelation, TArray<class AWRCharacter*>& InoutResult);
	void GetNearByCharacters(TDoubleLinkedList<class AWRCharacter*>& OUT OutList, const float IN InDist);
	void GetActorListInnerSector(TDoubleLinkedList<class AWRCharacter*>& OUT OutList, const FTransform IN InStandardTF, const float IN InMin, const float IN InMax, const float IN InAngle);
	void GetCharactersByType(EWRCharacterType InType, TArray<AWRCharacter*>& OutResult);
	void GetAllActor(TArray<AActor*>& InoutActor);

	//====================================================================================
	AID_t GenerateStandaloneAUID();
	FORCEINLINE AID_t GetMyAID() { return MyAUID; }
	class AWRCharacterMine* GetMyCharacter();
	bool IsMyCharacter(class AWRCharacter* InChar) const;
	const class AWRCharacter* GetMyTarget();
	bool IsMoving();

	//====================================================================================
	// [ 2019-10-1 : kyu ] 무기 관련 추가
	bool IsEquippedHook();
	bool IsHookConnected();
	bool IsHookableRange(class AWRCharacter* IN InChar);
	bool IsCableTarget(class AWRCharacter* IN InChar);
	bool IsCableTargetPart(class AWRCharacter* IN InChar, const EWRCollisionType IN InPart);
	class AWRCharacter* GetCableTarget();
	EWRAdditionalAbility GetEquippedSubWeaponAbility();
	// [ 2020-3-13 : kyu ] Abandoned Thing
	void SpawnAbandonedThing(FString& IN InObjectPath, class AWRCharacter* IN InChar, const WRTableID IN InItemTID);
	// [ 2019-9-26 : kyu ] for Debug & Test
	void OnChangedConsole_IOCollisionShow(const bool IN InShow);
	// [ 2019-10-7 : kyu ] Hook
	void CutOffMyHookChain(const bool bInImmediately = false, const bool IN InIdle = false);
	void HookCutAllRelatedTarget(AWRCharacter* IN InTarget, const bool IN InByMe);
	// [ 2020-3-17 : kyu ] Cheers
	void SetMyCheersCondition(const EWRCheersConditionType IN InConditionType);
	EWRCheersConditionType GetMyCheersCondition();
	void SetCheersCondition(const AID_t IN InAID , const EWRCheersConditionType IN InConditionType);
	EWRCheersConditionType GetCheersCondition(const AID_t IN InAID);


	// Dead Character
	const TArray<AWRCharacter*>& GetDeadCharacters() { return DeadCharacters; }
	void AddDeadCharacter(AWRCharacter* DeadCharacter) { DeadCharacters.Add(DeadCharacter); }
	void RemoveDeadCharacter(AWRCharacter* InChar) { DeadCharacters.Remove(InChar); }

	// Item
	void OnAddItemReceived();
	void OnUpdateItemCountReceived();
	void OnDeleteItemReceived();

private:

	// [ 2019-7-17 : magedoga ] InPath는 "/Game/Asset/Blueprint/Character/" 하위부터. 
	template <class TCharacterType>
	TCharacterType* Spawn_Internal(const FString& InBPPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale = FVector::OneVector);

	void Remove_Internal(class AWRCharacter* InCharacter, bool bInRemoveFromContainer = true);
	void RemoveAll();

	void SetTickInterval_Internal(float InInterval, AWRCharacter* InCharacter);

private:

	// [ 2019-8-27 : magedoga ] Custom Key Func
	struct FWRCharacterKeyFunc : BaseKeyFuncs<class AWRCharacter*, AID_t>
	{
		static FORCEINLINE KeyInitType GetSetKey(class AWRCharacter* Element)
		{
			return Element->GetActorID();
		}

		static FORCEINLINE bool Matches(AID_t A, AID_t B)
		{
			return A == B;
		}

		static FORCEINLINE uint32 GetKeyHash(AID_t InKey) 
		{
			return GetTypeHash(InKey);
		}
	};

	TArray<AWRCharacter*>							DeadCharacters;
	TSet<class AWRCharacter*, FWRCharacterKeyFunc>	Characters;
	TMap<int32, AWRCharacter*>	TriggerSpawnTidCharacters;
	AID_t ActorUID4StandAlone = INITIALIZE_STANDALONE_AUID;

	AID_t MyAUID = 0;

	//Spawn Actor
	TMap<FString, AID_t> AsyncSpawnActorKey;

	TMap<AID_t, pt_Game_Actor::Inf_PlayerAppear_G2U> PlayereOnBoardingVehicle;	//차량이 소환될때까지 딜레이 되는 케릭터 정보
	TMap<AID_t, pt_Game_Actor::Inf_PlayerAppear_G2U> PlayerAppearInfo2Spawn;
	TMap<AID_t, pt_Game_Actor::Inf_NpcAppear_G2U> NpcAppearanceInfo2Spawn;
	
	TMap <AID_t, pt_Game_Actor::Inf_IOAppear_G2U> IOAppearanceInfo2Spawn;
	TMap <AID_t, pt_Game_Actor::Inf_SOAppear_G2U> SOAppearanceInfo2Spawn;
	TMap <AID_t, pt_Game_Actor::Inf_APAppear_G2U> APAppearanceInfo2Spawn;

public:
	void CancelAsync(const AID_t InAID);

public:
	void MergeMesh(AActor* InActor, EWREquipPart InPart);
	void MergeMeshAsynchronously(AActor* InActor, EWREquipPart InPart);

	void AsyncLoadResource(AActor* InCharacter);
	void OnAsyncLoadComplete(UObject* InAsset, FString InHash);

private:
	TMap<AActor*, FWRSkeletalMeshMergeTask> Tasks;

private:
	float CullingRefreshTime = 0.0F;

	//====================================================================================
		// [ 2020-4-6 : firefirefox ] MyCharacter Event
	// mycharacter 쪽에 하고 싶으나 아직 myCHaracterMyanager가 없고 AWRCharacterMine에서 하려니 프리셋 변경 시 캐릭터 재생성 이슈 때문에 여기서 처리함
public:
	FDelegateHandle AddMyCharacterStartMoveDelegate(const FSimpleDelegate& IN InDelegateToNotify);
	FDelegateHandle AddMyCharacterStopMoveDelegate(const FSimpleDelegate& IN InDelegateToNotify);
	FDelegateHandle AddMyCharacterTargetChangedDelegate(const FSimpleDelegate& IN InDelegateToNotify);
	void RemoveMyCharacterStartMoveDelegate(const FDelegateHandle& IN InDelegateHandle);
	void RemoveMyCharacterStopMoveDelegate(const FDelegateHandle& IN InDelegateHandle);
	void RemoveMyCharacterTargetChangedDelegate(const FDelegateHandle& IN InDelegateHandle);
	FWRMyCharacterSpawnedDelegate& GetMyCharacterSpawnedDelegate() { return MyCharacterSpawnedDelegate; }
	
	void NotifyMyCharacterStartMoveDelegate();
	void NotifyMyCharacterStopMoveDelegate();
	void NotifyMyCharacterTargetChangedDelegate();
private:
	FWRMyCharacterStartMoveDelegate StartMoveDelegate;
	FWRMyCharacterStopMoveDelegate StopMoveDelegate;
	FWRMyCharacterTargetChangedDelegate TargetChangedDelegate;
	FWRMyCharacterSpawnedDelegate MyCharacterSpawnedDelegate;
	//====================================================================================

//====================================================================================
// [ 2020-7-17: EZ ] MyCharacter Event
// 비밀대화 관련 렌더 컨트롤
	TMap<AID_t, FWRReservedInvinsibleInfo> ReservedInvisibles;
public:
	void SetVisibleAll(const TArray<AID_t>& IgnoreAIDList, const bool InIsVisible);
	void SetVisibleByList(const TArray<AID_t>& InAIDList, const bool InIsVisible);

	void SetWRTargettingCollisionEnableAll(const ECollisionEnabled::Type InType, const TArray<AID_t>& InIgnoreList);
	void SetWRTargettingCollisionEnableByList(const TArray<AID_t>& InAIDList, const ECollisionEnabled::Type InType);

	void AddVisibilityBitByAIDList(const TArray<AID_t>& InAIDList, const EWRVisibility InBit);
	void RemoveVisibilityBitByAIDList(const TArray<AID_t>& InAIDList, const EWRVisibility InBit);
//====================================================================================

	// [ 2020-8-20 : magedoga ] Display URO Debug
	void SetEnableDisplayURODebug(const bool bInEnable);

	// [ 2020-9-14 : kyu ] 죄송
	void ResetVRRotateOrientation();

};

template <class TCharacterType>
TCharacterType* WRCharacterManager::Spawn(const FWRCharacterSpawnParams& InParam, bool bInIsMe /* = false */, bool bInIgnoreCheckMapLoading /* = false */)
{
	if (bInIgnoreCheckMapLoading == false)
	{
		if (WRMapManager::IsValid() == false || (WRMapManager::IsValid() && WRMapManager::Get()->IsLoading()))
		{
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Is Loading : Spawn"), TEXT("WRCharacterManager::Spawn"));
			return nullptr;
		}
	}

	TCharacterType* SpawnedChar = Spawn_Internal<TCharacterType>(InParam.BPPath, InParam.Location, InParam.Rotation, InParam.Scale);
	if (SpawnedChar)
	{
		OnCompleteSpawn(InParam, SpawnedChar, bInIsMe);
		Characters.Add(SpawnedChar);

		if(InParam.TriggerSpawnID != 0 && TriggerSpawnTidCharacters.Contains(InParam.TriggerSpawnID) == false)
			TriggerSpawnTidCharacters.Add(InParam.TriggerSpawnID, SpawnedChar);
	}

	//if(bInIsMe)
		//WRUIManager::Get()->OnStereoLayerInitialize();

	return SpawnedChar;
	//return Spawn_Internal<TCharacterType>(InParam.BPPath, InParam.Location, InParam.Rotation);
}

template <class TCharacterType>
TCharacterType* WRCharacterManager::FindbyIndex_Casted(const int32 InIndex)
{
	return Cast<TCharacterType>(this->FindbyIndex(InIndex));
}

template <class TCharacterType>
TCharacterType* WRCharacterManager::FindbyAID_Casted(AID_t InAID)
{
	return Cast<TCharacterType>(this->FindbyAID(InAID));
}

template <class TCharacterType>
TCharacterType* WRCharacterManager::Spawn_Internal(const FString& InBPPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale /* = FVector::OneVector */)
{
	// [ 2019-7-17 : magedoga ] Get World Context Object
	UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();

	//FString BPFullPath = FPackageName::IsValidObjectPath(InBPPath) ? InBPPath : WRPathManager::Get()->GetPath(EWRPath::Blueprint_Character) + InBPPath;
	FString BPFullPath = FPackageName::IsValidObjectPath(InBPPath) ? InBPPath : WRPathManager::Get()->GetPath(EWRPath::Character) + InBPPath;

	if (FPackageName::DoesPackageExist(BPFullPath) == false)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(BPFullPath);

		FString Text = FString::Format(TEXT("Does Not Package Exist {0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRCharacterManager::Spawn_Internal()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return nullptr;
	}
	
	// [ 2019-7-17 : magedoga ] Get Character Blueprint Path
	//FString CharacterBPPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_Character);
	// [ 2019-7-17 : magedoga ] Combine Path
	//FString BPFullPath = CharacterBPPath + InBPPath;

	TCharacterType* SpawnedCharacter = nullptr;
	// [ 2019-7-17 : magedoga ] 추후 타입도 추가되면 타입에 맞게 생성하는 코드 추가하면 됨.
	SpawnedCharacter = WRActorSpawnUtility::Spawn<TCharacterType>(GameInstance, BPFullPath, InLocation, InRotation, InScale);
	//====================================================================================
//	// [ 2019-7-17 : magedoga ] Type EX
// 	if (ECCharacterType::Player == InType)
// 	{
// 		SpawnedCharacter = WRActorSpawnUtility::Spawn<ACCharacterPlayer>(GameInstance, BPFullPath, InLocation, InRotation, InScale);
// 	}
	//====================================================================================

	if (SpawnedCharacter == nullptr)
	{
		UE_LOG(CharacterManager, Display, TEXT("WRCharacterManager::Spawn - Spawn failed. Please check your Blueprint Path."));
		return nullptr;
	}

	//SpawnedCharacter->AddToRoot();
	SpawnedCharacter->OnPostSpawn();

	return SpawnedCharacter;
}

// template <class TCharacterType>
// AWRCharacter* WRCharacterManager::Spawn(const FCCharacterSpawnParams& InParam)
// {
// 	return this->Spawn_Internal<TCharacterType>(InParam.BPPath, InParam.Location, InParam.Rotation, InParam.Scale);
// }
// 
// template <class TCharacterType>
// AWRCharacter* WRCharacterManager::Spawn_Internal(const FString& InBPPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale /*= FVector::OneVector */)
// {
// 	// [ 2019-7-17 : magedoga ] Get World Context Object
// 	UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
// 	// [ 2019-7-17 : magedoga ] Get Character Blueprint Path
// 	//FString CharacterBPPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_Character);
// 	// [ 2019-7-17 : magedoga ] Combine Path
// 	//FString BPFullPath = CharacterBPPath + InBPPath;
// 
// 	AWRCharacter* SpawnedCharacter = nullptr;
// 	// [ 2019-7-17 : magedoga ] 추후 타입도 추가되면 타입에 맞게 생성하는 코드 추가하면 됨.
// 	SpawnedCharacter = WRActorSpawnUtility::Spawn<TCharacterType>(GameInstance, BPFullPath, InLocation, InRotation, InScale);
// 	//====================================================================================
// //	// [ 2019-7-17 : magedoga ] Type EX
// // 	if (ECCharacterType::Player == InType)
// // 	{
// // 		SpawnedCharacter = WRActorSpawnUtility::Spawn<ACCharacterPlayer>(GameInstance, BPFullPath, InLocation, InRotation, InScale);
// // 	}
// 	//====================================================================================
// 
// 	if (SpawnedCharacter == nullptr)
// 	{
// 		UE_LOG(CharacterManager, Display, TEXT("WRCharacterManager::Spawn - Spawn failed. Please check your Blueprint Path."));
// 		return nullptr;
// 	}
// 
// 	SpawnedCharacter->AddToRoot();
// 	Characters.Add(SpawnedCharacter);
// 
// 	SpawnedCharacter->OnPostSpawn();
// 
// 
// 	return SpawnedCharacter;
// }




DECLARE_STATS_GROUP(TEXT("WRCharacterManager"), STATGROUP_CCharacterManager, STATCAT_Advanced);