// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Base/WRTableRow.h"

#include "Enum/EWRMap.h"
#include "Network/Share/Define/Define_Map.h"
#include "WRMapTable.generated.h"


// [2020-03-25 vuvle] 일단 기획에 맞춰서 임의적으로 START 값을 나눔 10000*TypeValue 로 하고 싶지만 Field Type 의 문제 발생 소지가 많아 현재는 강제로 값을 넣음. ProtoType 이후 수정 필요.
#define DEF_STANDARD_VALUE_MAP_TID_BANDWIDTH	10000
#define DEF_MAP_TYPE_FIELD_START				1
#define DEF_MAP_TYPE_LAB_START					10000
#define DEF_MAP_TYPE_TOWN_START					20000
#define DEF_MAP_TYPE_SCENARIO_START				40000
#define DEF_MAP_TYPE_STAGE_START				50000
#define DEF_MAP_TYPE_BATTLE_FIELD_START			60000
#define DEF_MAP_TYPE_END						70000


USTRUCT()
struct FWRMapTableBase : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	virtual EWRMapType::en GetMapType() { return EWRMapType::Field; }
	static EWRMapType::en GetMapType(WRTableID IN InMapTID) { 
		if (DEF_MAP_TYPE_FIELD_START <= InMapTID && InMapTID < DEF_MAP_TYPE_LAB_START) return EWRMapType::Field;
		else if (DEF_MAP_TYPE_LAB_START <= InMapTID && InMapTID < DEF_MAP_TYPE_TOWN_START) return EWRMapType::Lab;
		else if (DEF_MAP_TYPE_TOWN_START <= InMapTID && InMapTID < DEF_MAP_TYPE_SCENARIO_START) return EWRMapType::Town;
		else if (DEF_MAP_TYPE_SCENARIO_START <= InMapTID && InMapTID < DEF_MAP_TYPE_STAGE_START) return EWRMapType::Scenario;
		else if (DEF_MAP_TYPE_STAGE_START <= InMapTID && InMapTID < DEF_MAP_TYPE_BATTLE_FIELD_START) return EWRMapType::Stage;
		else if (DEF_MAP_TYPE_BATTLE_FIELD_START <= InMapTID && InMapTID < DEF_MAP_TYPE_END) return EWRMapType::BattleField;
		else return EWRMapType::None;
	}
	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Field; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		FName NameLocalkey;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
// 		EWRMapType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 MaxUserNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 ViewRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapTable")
		TAssetPtr<UWorld>	SceneLevelFile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 StartPosTid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 ExitPosTid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 RevivePosTid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 ShowRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 ShowNametagRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		FName LevelName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Change Map Direction")
		//bool Departureeffect = false;
		TAssetPtr<class UMaterialInterface>	StreamingLevelEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Change Map Direction")
		TAssetPtr<class USoundCue>	StreamingLevelEffectSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Change Map Direction")
	TAssetPtr<class UWRLoadLevelDirectionAsset>	Arrivaleffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Change Map Direction")
	TAssetPtr<class UWRLoadLevelDirectionAsset> DepartureEffectToMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Change Map Direction")
	TSoftObjectPtr<class UWRPreLoadAsset> PreLoadData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	bool UseTODControl = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	float DayCycle = 0.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	float Raining = 0.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	float Thundering = 0.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	float Lightning = 0.0F;	
};

USTRUCT(BlueprintType)
struct FWRMapLapTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MapData_Lab"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::Lab; }
	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Lab; }
	
};

USTRUCT(BlueprintType)
struct FWRMapFieldTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MapData_Field"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::Field; }
};

USTRUCT(BlueprintType)
struct FWRMapScenarioTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MapData_Scenario"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::Scenario; }
};

USTRUCT(BlueprintType)
struct FWRMapStageTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MapData_Stage"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::Stage; }
};

USTRUCT(BlueprintType)
struct FWRMapTownTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MapData_Town"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::Town; }
	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Town; }
};

USTRUCT(BlueprintType)
struct FWRMapBattleFieldTable : public FWRMapTableBase
{
	GENERATED_USTRUCT_BODY()

	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
	// 		EWRMapType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 StartPos2Tid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 StartPos3Tid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapTable")
		int32 StartPos4Tid;

public:
	static FString GetFileName() { return TEXT("MapData_BattleField"); }
	virtual EWRMapType::en GetMapType() override { return EWRMapType::BattleField; }
	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Field; }
};