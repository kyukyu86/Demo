#pragma once

#include "SingletonClass/WRSingleton.h"

#include "Enum/EWRMaterialParameterCollection.h"

#include "Actor/Animation/WRAnimInstance.h"

/**
*
*/

DECLARE_MULTICAST_DELEGATE(FWRComplete);

struct FWRMPC
{
	EWRMPC Key;

	FString Filename;
};

struct FWRCurve
{
	EWRMaterialParameter Key;

	FString Filename;
};

class WR_API WRMaterialParameterCollectionManager : public WRSingleton<WRMaterialParameterCollectionManager>
{
	CDECLARE_MANAGER(WRMaterialParameterCollectionManager);
	
private:
	WRMaterialParameterCollectionManager();
	virtual ~WRMaterialParameterCollectionManager();
	
public:
	virtual void OnInitialize() override;

	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

	virtual bool OnTick(float InDeltaTime) override;

public:
	void Spawn(const EWRMaterialParameter InKey, const float InDuration);
	void Remove(const EWRMaterialParameter InKey);

	FWRComplete& OnComplete(const EWRMaterialParameter& InKey);

public:
	// [ 2019-9-20 : animal14 ] 범용 함수
	void SetScalarParameterValue(const EWRMPC InKey, const FString& InParameterName, const float InParameterValue);
	void SetVectorParameterValue(const EWRMPC InKey, const FString& InParameterName, const FLinearColor& InParameterValue);

	// [ 2019-10-25 : hansang88 ] 
	float GetScalarParameterValue(const EWRMPC InKey, const FString& InParameterName);
	FLinearColor GetVectorParameterValue(const EWRMPC InKey, const FString& InParameterName);

	// [ 2019-9-20 : animal14 ] 전용 함수
	// [ 2019-10-7 : animal14 ] 타켓
	void SetLeft();
	void SetRight();
	void SetLeftRight(const float InParameterValue);
	void SetParrying(const bool InIsEnable);
	void SetEnpcAttack(const bool InIsEnable);

	void SetHit(const float InParameterValue);

	void SetLeftDodge(const float InParameterValue);
	void SetRightDodge(const float InParameterValue);

	void SetHP(const float InParameterValue);

	void SetDash(const float InParameterValue);
	void SetWhiteOut(const float InParameterValue);
	void SetRevive(const FLinearColor InParameterValue);
	void SetBlackOut(const float InParameterValue);
	void SetBreak(const float InParameterValue);
	void SetENPCToPCBlur(const float InParameterValue);
	void SetPCToENPCBlur(const float InParameterValue);
	void SetSpiderWeb(const float InParameterValue);
	void SetBlock(const float InParameterValue);

	// [ 2019-10-8 : animal14 ] 
	void SetImpactPoint(const FVector InParameterValue);

	// [ 2019-11-27 : animal14 ] 
	void SetRain(const float InParameterValue);
	void SetEmissive(const FLinearColor InParameterValue);
	void SetDrunk(const float InParameterValue);

	// [ 2019-10-7 : animal14 ] 
	void SetWindRotation(const float InParameterValue);
	void SetWindStrength(const float InParameterValue);
	void SetWindSpeed(const float InParameterValue);
	void SetWindMultiplier(const float InParameterValue);

	// [ 2019-11-1 : magedoga ] For direction of load level
	void SetEnableLoadLevelDirection(const float InValue);
	void SetMaxDistance(const float InValue);
	void SetGridWidth(const float InValue);
	void SetGridThickness(const float InValue);
	void SetGridFlow(const float InValue);
	void SetSceneFlow(const float InValue);
	void SetGridBoundThickness(const float InValue);
	void SetSceneBoundThickness(const float InValue);
	void SetBright(const float InValue);
	void SetGridColor(FLinearColor& InValue);
	void SetGridBoundColor(FLinearColor& InValue);
	void SetSceneBoundColor(FLinearColor& InValue);
	void SetEnableTransitionWhite(const bool bInEnable);
	void SetLocationForFoliageInteraction(const FName InParameterName ,const FVector InParameterValue);

	// [ 2020-1-28 : magedoga ] Select Character
	void SetEnableBlurInSelectCharacter(const bool bInEnable);
	void SetBlurAmountInSelectCharacter(const float InValue);

	// [ 2019-10-25 : hansang88 ] proto 문열릴때 처리
	void SetExposureTime(const float InEndTime);
	bool UpdateExposureTime(float InDeltaTime);
	void SetExposure(const float InParameterValue);

	// [ 2020-2-27 : magedoga ] Screen effect when enter & leave water volume
	void SetEnableWater(const bool bInEnable);
	void SetScreenWater(const float InValue);

private:
	void Register(const EWRMPC InKey, const FString& InFilename);
	void Register(const EWRMaterialParameter InKey, const FString& InFilename);

	void RegisterMPC(UObject* InObject, FString InString);
	void RegisterCurve(UObject* InObject, FString InString);

	void Unregister();

	void Process(float DeltaTime);

	void InitializeDodge();
	bool InitializeHP(const float InDuration);

	void SetDodge_Internal(float InRatio);

	void SetHP_Internal(float InRatio);

	void SetDash_Internal(float InRatio);
	void SetDashOut_Internal(float InRatio);
	void SetDashIn_Internal(float InRatio);
	void SetWhiteOut_Internal(float InRatio);
	void SetWhiteIn_Internal(float InRatio);
	void SetReviveOut_Internal(float InRatio);
	void SetReviveIn_Internal(float InRatio);
	void SetBlackOut_Internal(float InRatio);
	void SetBreak_Internal(float InRatio);
	void SetENPCToPCBlur_Internal(float InRatio);
	void SetPCToENPCBlur_Internal(float InRatio);
	void SetSpiderWeb_Internal(float InRatio);
	void SetBlock_Internal(float InRatio);

	template<class T>
	T* GetCastedCurve(const EWRMaterialParameter InKey);

private:
	TMap<EWRMPC, class UMaterialParameterCollection*> MPCs;

	TMap<EWRMaterialParameter, float> DeltaTimes;
	TMap<EWRMaterialParameter, float> Durations;
	TMap<EWRMaterialParameter, class UCurveBase*> Curves;
	TMap<EWRMaterialParameter, FWRComplete> Completes;

	TMap<FString, FWRMPC> TempMPCs;
	TMap<FString, FWRCurve> TempCurves;

	DECLARE_DELEGATE_OneParam(FWRTick, float);
	TArray<FWRTick> Sets;

	bool IsRecovery = false;

	int32 PreviousState = 0;
	int32 Difference = 1;

	float PreviousRemainHP = 1.0F;

	EWRCombatGimmickState CombatGimmickState;
	
private:
	// [ 2019-10-29 : kyu ] 일단 Exposure만 있는건가 싶어 이것만 별도 변수 뺌
	FDelegateHandle TickExposureHandle;

	float SetExposureElapsedTime = 0.f;
	float SetExposureEndTime = 0.f;
};

template<class T>
T* WRMaterialParameterCollectionManager::GetCastedCurve(const EWRMaterialParameter InKey)
{
	if (Curves.Contains(InKey) == false)
	{
		return nullptr;
	}
	return Cast<T>(Curves[InKey]);
}
