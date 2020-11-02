#include "WRMaterialParameterCollectionManager.h"
#include "WRPathManager.h"
#include "WRCharacterManager.h"
#include "WRConsoleManager.h"
#include "WRAsyncLoadUObjectManager.h"

#include "Materials/MaterialParameterCollection.h"

#include "Kismet/KismetMaterialLibrary.h"

#include "Ticker.h"

#include "Actor/Character/WRCharacterMine.h"

#include "Component/Character/WRActorComponentSFX.h"

#include "Define/WRPathDefine.h"

WRMaterialParameterCollectionManager::WRMaterialParameterCollectionManager()
{

}

WRMaterialParameterCollectionManager::~WRMaterialParameterCollectionManager()
{

}

void WRMaterialParameterCollectionManager::OnInitialize()
{
	this->Register(EWRMPC::Character, TEXT("/Game/Asset/Character/00_GlobalShader/Parameter/Character_ParameterCollection.Character_ParameterCollection"));
	this->Register(EWRMPC::UI, TEXT("/Game/Asset/BG/02_GlobalShader/PostProcess/PP_TargetParameterCollection.PP_TargetParameterCollection"));
	this->Register(EWRMPC::Foliage, TEXT("/Game/Asset/BG/02_GlobalShader/Parameter/GrassGlobalParams.GrassGlobalParams"));
	this->Register(EWRMPC::LevelLoad, TEXT("/Game/Asset/BG/02_GlobalShader/PostProcess/PP_LoadLevelParameterCollection.PP_LoadLevelParameterCollection"));
	//this->Register(EWRMPC::Monitor, TEXT("/Game/Asset/BG/04_Portotype/Lobby/M_Base/LobbyLabParameterCollection.LobbyLabParameterCollection"));
	this->Register(EWRMPC::Movie, TEXT("/Game/Asset/BG/02_GlobalShader/Parameter/MovieParameterCollection.MovieParameterCollection"));

	this->Register(EWRMaterialParameter::WhiteOut, TEXT("/Game/Asset/BG/02_GlobalShader/PostProcess/WhiteOut_Curve.WhiteOut_Curve"));

	// [ 2019-10-15 : animal14 ] 바인딩
	Sets.SetNum((int32)EWRMaterialParameter::Max);
	Sets[(int32)EWRMaterialParameter::Dodge].BindRaw(this, &WRMaterialParameterCollectionManager::SetDodge_Internal);
	Sets[(int32)EWRMaterialParameter::Dash].BindRaw(this, &WRMaterialParameterCollectionManager::SetDash_Internal);
	Sets[(int32)EWRMaterialParameter::DashOut].BindRaw(this, &WRMaterialParameterCollectionManager::SetDashOut_Internal);
	Sets[(int32)EWRMaterialParameter::DashIn].BindRaw(this, &WRMaterialParameterCollectionManager::SetDashIn_Internal);
	Sets[(int32)EWRMaterialParameter::HP].BindRaw(this, &WRMaterialParameterCollectionManager::SetHP_Internal);
	Sets[(int32)EWRMaterialParameter::WhiteOut].BindRaw(this, &WRMaterialParameterCollectionManager::SetWhiteOut_Internal);
	Sets[(int32)EWRMaterialParameter::WhiteIn].BindRaw(this, &WRMaterialParameterCollectionManager::SetWhiteIn_Internal);
	Sets[(int32)EWRMaterialParameter::ReviveOut].BindRaw(this, &WRMaterialParameterCollectionManager::SetReviveOut_Internal);
	Sets[(int32)EWRMaterialParameter::ReviveIn].BindRaw(this, &WRMaterialParameterCollectionManager::SetReviveIn_Internal);
	Sets[(int32)EWRMaterialParameter::BlackOut].BindRaw(this, &WRMaterialParameterCollectionManager::SetBlackOut_Internal);
	Sets[(int32)EWRMaterialParameter::Break].BindRaw(this, &WRMaterialParameterCollectionManager::SetBreak_Internal);
	Sets[(int32)EWRMaterialParameter::ENPCToPCBlur].BindRaw(this, &WRMaterialParameterCollectionManager::SetENPCToPCBlur_Internal);
	Sets[(int32)EWRMaterialParameter::PCToENPCBlur].BindRaw(this, &WRMaterialParameterCollectionManager::SetPCToENPCBlur_Internal);
	Sets[(int32)EWRMaterialParameter::SpiderWeb].BindRaw(this, &WRMaterialParameterCollectionManager::SetSpiderWeb_Internal);
	Sets[(int32)EWRMaterialParameter::Block].BindRaw(this, &WRMaterialParameterCollectionManager::SetBlock_Internal);
}

void WRMaterialParameterCollectionManager::OnShutdown()
{
	for (auto& TempMPC : TempMPCs)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(TempMPC.Value.Filename);

		FString Text = FString::Format(TEXT("MPC {0} was not removed"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRMaterialParameterCollectionManager::OnShutdown()"));
#else // UE_EDITOR
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(TempMPC.Key);
#endif // UE_EDITOR
	}

	for (auto& TempCurve : TempCurves)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(TempCurve.Value.Filename);

		FString Text = FString::Format(TEXT("Curve {0} was not removed"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRMaterialParameterCollectionManager::OnShutdown()"));
#else // UE_EDITOR
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(TempCurve.Key);
#endif // UE_EDITOR
	}
	
	this->Unregister();

	if (TickExposureHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickExposureHandle);
		TickExposureHandle.Reset();
	}
}

void WRMaterialParameterCollectionManager::OnLoadLevelStart()
{

}

void WRMaterialParameterCollectionManager::OnLoadLevelComplete()
{

}

void WRMaterialParameterCollectionManager::OnRenderStart()
{

}

bool WRMaterialParameterCollectionManager::OnTick(float InDeltaTime)
{
	this->Process(InDeltaTime);

	return true;
}

void WRMaterialParameterCollectionManager::Spawn(const EWRMaterialParameter InKey, const float InDuration)
{
	if (InKey == EWRMaterialParameter::Dodge)
	{
		this->InitializeDodge();
	}
	if (InKey == EWRMaterialParameter::HP)
	{
		bool bEnable = WRConsoleManager::Get()->GetInt(EWRConsole::PostProcess_EnableHP) == 1;
		if (bEnable == false)
		{
			return;
		}

		bool IsSkipping = this->InitializeHP(InDuration);
		if (IsSkipping)
		{
			return;
		}
	}

	DeltaTimes.Add(InKey, 0.0F);
	Durations.Add(InKey, InDuration);
}

void WRMaterialParameterCollectionManager::Remove(const EWRMaterialParameter InKey)
{
	if (DeltaTimes.Contains(InKey) == false || Durations.Contains(InKey) == false)
	{
		return;
	}

	DeltaTimes.Remove(InKey);
	Durations.Remove(InKey);

	// [ 2019-10-30 : animal14 ] 완료 콜백
	if (Completes.Contains(InKey) == false)
	{
		return;
	}
	Completes[InKey].Broadcast();
}

FWRComplete& WRMaterialParameterCollectionManager::OnComplete(const EWRMaterialParameter& InKey)
{
	if (Completes.Contains(InKey) == false)
	{
		FWRComplete Complete;

		Completes.Add(InKey, Complete);
	}
	return Completes[InKey];
}

void WRMaterialParameterCollectionManager::SetScalarParameterValue(const EWRMPC InKey, const FString& InParameterName, const float InParameterValue)
{
	if (MPCs.Contains(InKey) == false)
	{
		return;
	}

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	UKismetMaterialLibrary::SetScalarParameterValue(World, MPCs[InKey], *InParameterName, InParameterValue);
}

void WRMaterialParameterCollectionManager::SetVectorParameterValue(const EWRMPC InKey, const FString& InParameterName, const FLinearColor& InParameterValue)
{
	if (MPCs.Contains(InKey) == false)
	{
		return;
	}

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	UKismetMaterialLibrary::SetVectorParameterValue(World, MPCs[InKey], *InParameterName, InParameterValue);
}

float WRMaterialParameterCollectionManager::GetScalarParameterValue(const EWRMPC InKey, const FString& InParameterName)
{
	if (MPCs.Contains(InKey) == false)
	{
		return 0;
	}

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return 0;
	}
	return UKismetMaterialLibrary::GetScalarParameterValue(World, MPCs[InKey], *InParameterName);
}

FLinearColor WRMaterialParameterCollectionManager::GetVectorParameterValue(const EWRMPC InKey, const FString& InParameterName)
{
	FLinearColor returnValue(0, 0, 0, 0);
	if (MPCs.Contains(InKey) == false)
	{
		return returnValue;
	}

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return returnValue;
	}

	returnValue = UKismetMaterialLibrary::GetVectorParameterValue(World, MPCs[InKey], *InParameterName);

	return returnValue;
}

void WRMaterialParameterCollectionManager::SetLeft()
{
	this->SetScalarParameterValue(EWRMPC::UI, "LeftRight", 0.0F);
}

void WRMaterialParameterCollectionManager::SetRight()
{
	this->SetScalarParameterValue(EWRMPC::UI, "LeftRight", 1.0F);
}

void WRMaterialParameterCollectionManager::SetLeftRight(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "LeftRight", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetParrying(const bool InIsEnable)
{
	float ParameterValue = InIsEnable ? 1.0F : 0.0F;

	this->SetScalarParameterValue(EWRMPC::UI, "Parrying", ParameterValue);
}

void WRMaterialParameterCollectionManager::SetEnpcAttack(const bool InIsEnable)
{
	float ParameterValue = InIsEnable ? 1.0F : 0.0F;

	this->SetScalarParameterValue(EWRMPC::UI, "EnpcAttack", ParameterValue);
}

void WRMaterialParameterCollectionManager::SetHit(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Hit_Red", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetLeftDodge(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Left_Evasion_Blue", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetRightDodge(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Right_Evasion_Blue", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetHP(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "PCHitBloodMain", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetDash(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Dash", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetWhiteOut(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Whiteout", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetRevive(const FLinearColor InParameterValue)
{
	this->SetVectorParameterValue(EWRMPC::UI, "BlackOut_Vector", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetBlackOut(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "BlackOut", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetBreak(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "GuardBreak", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetENPCToPCBlur(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "PCBlur", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetPCToENPCBlur(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "PCAttackBlur", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetSpiderWeb(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "SpiderWeb", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetBlock(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "ShieldGroggy", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetImpactPoint(const FVector InParameterValue)
{
	this->SetVectorParameterValue(EWRMPC::Character, "HitPartVector", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetRain(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::Character, "RainMain", InParameterValue);
	this->SetScalarParameterValue(EWRMPC::Foliage, "RainMain", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetEmissive(const FLinearColor InParameterValue)
{
	this->SetVectorParameterValue(EWRMPC::Foliage, "TODEmissive", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetDrunk(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "AlcoholIntensity", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetWindRotation(const float InParameterValue)
{
	// [ 2019-10-7 : animal14 ] 1의 값은 360도 회전한 값
	float ParameterValue = FMath::Clamp(InParameterValue, 0.0F, 1.0F);

	this->SetScalarParameterValue(EWRMPC::Foliage, "WindTextureRotation", ParameterValue);
}

void WRMaterialParameterCollectionManager::SetWindStrength(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::Foliage, "WindStrength", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetWindSpeed(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::Foliage, "WindSpeed", InParameterValue);
}

void WRMaterialParameterCollectionManager::SetWindMultiplier(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::Foliage, "DirWindMultiplier", InParameterValue);
}

//====================================================================================
// [ 2019-11-1 : magedoga ] Load Level PP

void WRMaterialParameterCollectionManager::SetEnableLoadLevelDirection(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "Enable", InValue);
}

void WRMaterialParameterCollectionManager::SetMaxDistance(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "MaxDistance", InValue);
}

void WRMaterialParameterCollectionManager::SetGridWidth(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "GridWidth", InValue);
}

void WRMaterialParameterCollectionManager::SetGridThickness(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "GridThickness", InValue);
}

void WRMaterialParameterCollectionManager::SetGridFlow(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "GridFlow", InValue);
}

void WRMaterialParameterCollectionManager::SetSceneFlow(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "SceneFlow", InValue);
}

void WRMaterialParameterCollectionManager::SetGridBoundThickness(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "GridBoundThickness", InValue);
}

void WRMaterialParameterCollectionManager::SetSceneBoundThickness(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "SceneBoundThickness", InValue);
}

void WRMaterialParameterCollectionManager::SetBright(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "Bright", InValue);
}

void WRMaterialParameterCollectionManager::SetGridColor(FLinearColor& InValue)
{
	this->SetVectorParameterValue(EWRMPC::LevelLoad, "GridColor", InValue);
}

void WRMaterialParameterCollectionManager::SetGridBoundColor(FLinearColor& InValue)
{
	this->SetVectorParameterValue(EWRMPC::LevelLoad, "GridBoundColor", InValue);
}

void WRMaterialParameterCollectionManager::SetSceneBoundColor(FLinearColor& InValue)
{
	this->SetVectorParameterValue(EWRMPC::LevelLoad, "SceneBoundColor", InValue);
}

void WRMaterialParameterCollectionManager::SetEnableTransitionWhite(const bool bInEnable)
{
	this->SetScalarParameterValue(EWRMPC::LevelLoad, "EnableTransitionWhite", bInEnable == true ? 1.0f : 0.0f);
}

void WRMaterialParameterCollectionManager::SetLocationForFoliageInteraction(const FName InParameterName, const FVector InParameterValue)
{
	this->SetVectorParameterValue(EWRMPC::Foliage, InParameterName.ToString(), InParameterValue);
}

void WRMaterialParameterCollectionManager::SetEnableBlurInSelectCharacter(const bool bInEnable)
{
	this->SetScalarParameterValue(EWRMPC::UI, "EnableBlurInSelectCharacter", bInEnable == true ? 1.0f : 0.0f);
}

void WRMaterialParameterCollectionManager::SetBlurAmountInSelectCharacter(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "EnableBlurInSelectCharacter", InValue);
}

void WRMaterialParameterCollectionManager::SetExposureTime(const float InParameterGoalValue)
{
	SetExposureElapsedTime = 0.f;
	SetExposureEndTime = InParameterGoalValue;
	//	SetCollectionEndTime = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::UI, "Exposure_Time");

	if (TickExposureHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickExposureHandle);
		TickExposureHandle.Reset();
	}

	TickExposureHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRMaterialParameterCollectionManager::UpdateExposureTime));
}

bool WRMaterialParameterCollectionManager::UpdateExposureTime(float InDeltaTime)
{
	if (SetExposureEndTime == 0.f)
		return true;

	SetExposureElapsedTime += InDeltaTime;

	float  Value = FMath::Lerp(1.f, 0.f, FMath::Clamp(SetExposureElapsedTime, 0.f, SetExposureEndTime) / SetExposureEndTime);
	SetExposure(Value);

	if (SetExposureElapsedTime >= SetExposureEndTime)
	{
		if (TickExposureHandle.IsValid() == true)
		{
			FTicker::GetCoreTicker().RemoveTicker(TickExposureHandle);
			TickExposureHandle.Reset();
			SetExposureEndTime = 0.f;
		}
	}
	return true;
}

void WRMaterialParameterCollectionManager::SetExposure(const float InParameterValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "Exposure", InParameterValue);
}


//====================================================================================
// [ 2020-2-27 : magedoga ] Water
void WRMaterialParameterCollectionManager::SetEnableWater(const bool bInEnable)
{
	this->SetScalarParameterValue(EWRMPC::UI, "OceanIn", bInEnable == true ? 1.0f : 0.0f);
}

void WRMaterialParameterCollectionManager::SetScreenWater(const float InValue)
{
	this->SetScalarParameterValue(EWRMPC::UI, "OceanOutWater", InValue);
}
//====================================================================================

void WRMaterialParameterCollectionManager::Register(const EWRMPC InKey, const FString& InFilename)
{
	FWRMPC Value;
	Value.Key = InKey;
	Value.Filename = InFilename;

	FString Key = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(Value.Filename, FWRCompleteAsyncLoad::CreateRaw(this, &WRMaterialParameterCollectionManager::RegisterMPC));
	TempMPCs.Add(Key, Value);
}

void WRMaterialParameterCollectionManager::Register(const EWRMaterialParameter InKey, const FString& InFilename)
{
	FWRCurve Value;
	Value.Key = InKey;
	Value.Filename = InFilename;

	FString Key = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(Value.Filename, FWRCompleteAsyncLoad::CreateRaw(this, &WRMaterialParameterCollectionManager::RegisterCurve));
	TempCurves.Add(Key, Value);
}

void WRMaterialParameterCollectionManager::RegisterMPC(UObject* InObject, FString InKey)
{
	if (TempMPCs.Contains(InKey) == false)
	{
		return;
	}

	UMaterialParameterCollection* MPC = Cast<UMaterialParameterCollection>(InObject);
	if (MPC == nullptr)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(TempMPCs[InKey].Filename);

		FString Text = FString::Format(TEXT("MPC {0} is nullptr"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRMaterialParameterCollectionManager::Register()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	MPC->AddToRoot();

	MPCs.Add(TempMPCs[InKey].Key, MPC);

	TempMPCs.Remove(InKey);
}

void WRMaterialParameterCollectionManager::RegisterCurve(UObject* InObject, FString InKey)
{
	if (TempCurves.Contains(InKey) == false)
	{
		return;
	}

	UCurveBase* CurveBase = Cast<UCurveBase>(InObject);
	if (CurveBase == nullptr)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(TempCurves[InKey].Filename);

		FString Text = FString::Format(TEXT("CurveBase {0} is nullptr"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRActorComponentManageMaterial::Register()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	CurveBase->AddToRoot();

	Curves.Add(TempCurves[InKey].Key, CurveBase);

	TempCurves.Remove(InKey);
}


void WRMaterialParameterCollectionManager::Unregister()
{
	for (auto& MPC : MPCs)
	{
		MPC.Value->RemoveFromRoot();
		MPC.Value = nullptr;
	}
	MPCs.Empty();

	for (auto& Curve : Curves)
	{
		Curve.Value->RemoveFromRoot();
		Curve.Value = nullptr;
	}
	Curves.Empty();
}

void WRMaterialParameterCollectionManager::Process(float DeltaTime)
{
	for (int32 i = 0; i < (int32)EWRMaterialParameter::Max; ++i)
	{
		EWRMaterialParameter Key = (EWRMaterialParameter)i;

		if (DeltaTimes.Contains(Key) == false || Durations.Contains(Key) == false)
		{
			continue;
		}

		float Duration = Durations[Key];
		if (0.0F > Duration - FLT_EPSILON)
		{
			// [ 2019-10-15 : animal14 ] 실행
			Sets[i].ExecuteIfBound(1.0F);
			this->Remove(Key);
			continue;
		}

		DeltaTimes[Key] += DeltaTime;

		float AccDeltaTime = DeltaTimes[Key];
		if (AccDeltaTime > Duration + DeltaTime)
		{
			this->Remove(Key);
			continue;
		}

		float Ratio = FMath::Clamp(AccDeltaTime / Duration, 0.0F, 1.0F);

		// [ 2019-10-15 : animal14 ] 실행
		Sets[i].ExecuteIfBound(1.0F - Ratio);
	}
}

void WRMaterialParameterCollectionManager::InitializeDodge()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(MyCharacter->GetTargetAID());
	if (Target == nullptr)
	{
		return;
	}

	UWRAnimInstance* AnimInstance = Target->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	if (AnimInstance->IsIncludedCombatGimmickState(EWRCombatGimmickState::AvoidingLeft))
	{
		CombatGimmickState = EWRCombatGimmickState::AvoidingLeft;
	}
	if (AnimInstance->IsIncludedCombatGimmickState(EWRCombatGimmickState::AvoidingRight))
	{
		CombatGimmickState = EWRCombatGimmickState::AvoidingRight;
	}
}

bool WRMaterialParameterCollectionManager::InitializeHP(const float InDuration)
{
	bool IsSkipping = false;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return IsSkipping;
	}
	
	UWRActorComponentSFX* ActorComponentSFX = MyCharacter->GetComponent<UWRActorComponentSFX>();
	if (ActorComponentSFX == nullptr)
	{
		return IsSkipping;
	}

	int32 State = 0;

	float RemainHP = (float)MyCharacter->GetHP() / (float)MyCharacter->GetMaxHP();
	if (RemainHP > 0.5F)
	{
		State = 0;

		if (PreviousState != State)
		{
			ActorComponentSFX->RemoveSFX(EWRSFXActor::AccDamage, InDuration);
		}
	}
	else if (RemainHP <= 0.5F && RemainHP > 0.3F)
	{
		State = 1;

		if (PreviousState != State)
		{
			ActorComponentSFX->ChangeSFX(EWRSFXActor::AccDamage, SOUND_PATH_4_ACCUMULATE_DAMAGE_S, MyCharacter->GetRootComponent(), InDuration);
		}
	}
	else if (RemainHP <= 0.3F && RemainHP > 0.1F)
	{
		State = 2;

		if (PreviousState != State)
		{
			ActorComponentSFX->ChangeSFX(EWRSFXActor::AccDamage, SOUND_PATH_4_ACCUMULATE_DAMAGE_M, MyCharacter->GetRootComponent(), InDuration);
		}
	}
	else
	{
		State = 3;

		if (PreviousState != State)
		{
			ActorComponentSFX->ChangeSFX(EWRSFXActor::AccDamage, SOUND_PATH_4_ACCUMULATE_DAMAGE_F, MyCharacter->GetRootComponent(), InDuration);
		}
	}

	IsSkipping = (PreviousState == State) ? true : false;
	IsRecovery = (RemainHP - PreviousRemainHP > 0.0F) ? true : false;

	// [ 2019-11-15 : animal14 ] 
	PreviousRemainHP = RemainHP;
	Difference = FMath::Abs(PreviousState - State);
	PreviousState = State;

	return IsSkipping;
}

void WRMaterialParameterCollectionManager::SetDodge_Internal(float InRatio)
{
	if (CombatGimmickState == EWRCombatGimmickState::AvoidingLeft)
	{
		this->SetLeftDodge(InRatio);
	}
	if (CombatGimmickState == EWRCombatGimmickState::AvoidingRight)
	{
		this->SetRightDodge(InRatio);
	}
}

void WRMaterialParameterCollectionManager::SetHP_Internal(float InRatio)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	float RemainHP = (float)MyCharacter->GetHP() / (float)MyCharacter->GetMaxHP();
	if (RemainHP > 0.5F)
	{
		if (IsRecovery)
		{
			this->SetHP(0.0F + (InRatio * Difference));
		}
		else
		{
		}
	}
	else if (RemainHP <= 0.5F && RemainHP > 0.3F)
	{
		IsRecovery ? this->SetHP(1.0F + (InRatio * Difference)) : this->SetHP(1.0F - (InRatio * Difference));
	}
	else if (RemainHP <= 0.3F && RemainHP > 0.1F)
	{
		IsRecovery ? this->SetHP(2.0F + (InRatio * Difference)) : this->SetHP(2.0F - (InRatio * Difference));
	}
	else
	{
		if (IsRecovery)
		{
		}
		else
		{
			this->SetHP(3.0F - (InRatio * Difference));
		}
	}
}

void WRMaterialParameterCollectionManager::SetDash_Internal(float InRatio)
{
	this->SetDash(InRatio);
}

void WRMaterialParameterCollectionManager::SetDashOut_Internal(float InRatio)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	float Ratio = WRConsoleManager::Get()->GetFloat(EWRConsole::Dash_Ratio);
	if (MyCharacter->MoveForwardValue == 0.0F && MyCharacter->MoveRightValue == 0.0F)
	{
		this->SetBlackOut(FMath::Lerp(0.0F, Ratio, 1.0F - InRatio));
	}
	else
	{
		this->SetBlackOut(FMath::Lerp(WRConsoleManager::Get()->GetFloat(EWRConsole::BlackOutIn_Ratio), Ratio, 1.0F - InRatio));
	}
}

void WRMaterialParameterCollectionManager::SetDashIn_Internal(float InRatio)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	float Ratio = WRConsoleManager::Get()->GetFloat(EWRConsole::Dash_Ratio);
	if (MyCharacter->MoveForwardValue == 0.0F && MyCharacter->MoveRightValue == 0.0F)
	{
		this->SetBlackOut(FMath::Lerp(0.0F, Ratio, InRatio));
	}
	else
	{
		this->SetBlackOut(FMath::Lerp(WRConsoleManager::Get()->GetFloat(EWRConsole::BlackOutIn_Ratio), Ratio, InRatio));
	}
}

void WRMaterialParameterCollectionManager::SetWhiteOut_Internal(float InRatio)
{
	UCurveFloat* CurveFloat = this->GetCastedCurve<UCurveFloat>(EWRMaterialParameter::WhiteOut);
	if (CurveFloat == nullptr)
	{
		return;
	}

	float Ratio = CurveFloat->GetFloatValue((1.0F - InRatio) * 3.0F);

	this->SetWhiteOut(Ratio);
}

void WRMaterialParameterCollectionManager::SetWhiteIn_Internal(float InRatio)
{
	UCurveFloat* CurveFloat = this->GetCastedCurve<UCurveFloat>(EWRMaterialParameter::WhiteOut);
	if (CurveFloat == nullptr)
	{
		return;
	}

	float Ratio = CurveFloat->GetFloatValue(InRatio * 3.0F);

	this->SetWhiteOut(Ratio);
}

void WRMaterialParameterCollectionManager::SetReviveOut_Internal(float InRatio)
{
	FLinearColor Parameter = this->GetVectorParameterValue(EWRMPC::UI, "BlackOut_Vector");
	Parameter.B = FMath::Clamp(1.0F - InRatio, 0.0F, 1.0F);

	this->SetRevive(Parameter);
}

void WRMaterialParameterCollectionManager::SetReviveIn_Internal(float InRatio)
{
	FLinearColor Parameter = this->GetVectorParameterValue(EWRMPC::UI, "BlackOut_Vector");
	Parameter.B = FMath::Clamp(InRatio, 0.0F, 1.0F);

	this->SetRevive(Parameter);
}

void WRMaterialParameterCollectionManager::SetBlackOut_Internal(float InRatio)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	float Ratio = WRConsoleManager::Get()->GetFloat(EWRConsole::BlackOutIn_Ratio);
	if (MyCharacter->MoveForwardValue == 0.0F && MyCharacter->MoveRightValue == 0.0F)
	{
		this->SetBlackOut(FMath::Lerp(0.0F, Ratio, InRatio));
	}
	else
	{
		this->SetBlackOut(FMath::Lerp(0.0F, Ratio, 1.0F - InRatio));
	}
}

void WRMaterialParameterCollectionManager::SetBreak_Internal(float InRatio)
{
	this->SetBreak(InRatio);
}

void WRMaterialParameterCollectionManager::SetENPCToPCBlur_Internal(float InRatio)
{
	this->SetENPCToPCBlur(InRatio);
}

void WRMaterialParameterCollectionManager::SetPCToENPCBlur_Internal(float InRatio)
{
	this->SetPCToENPCBlur(InRatio);
}

void WRMaterialParameterCollectionManager::SetSpiderWeb_Internal(float InRatio)
{
	this->SetSpiderWeb(InRatio);
}

void WRMaterialParameterCollectionManager::SetBlock_Internal(float InRatio)
{
	this->SetBlock(InRatio);
}