// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Define/WRTypeDefine.h"
#include "Network/Share/Define/Define_Actor.h"
#include "WRPreviewStruct.generated.h"

DECLARE_DELEGATE(FWRPreviewExitDelegate)
DECLARE_DELEGATE_OneParam(FWRPreviewLoadCompletedDelegate, class AActor*)


UENUM(BlueprintType)
enum class EWRPreviewType : uint8
{
	Static,
	Skeletal,
	DefaultActor,
	CustomActor,
};

USTRUCT(BlueprintType)
struct FWRPreviewDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseRotate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseZoom = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomMax = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ZoomSocket = FName("None");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ActorLocationOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator ActorRotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ActorRotationValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector CameraOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CameraOffsetPivotSocket = FName("None");
};

USTRUCT(BlueprintType)
struct FWRPreviewDataWidget : public FWRPreviewDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	void operator=(FWRPreviewDataBase& InDataBase)
	{
		UseRotate = InDataBase.UseRotate;
		UseZoom = InDataBase.UseZoom;
		ZoomMax = InDataBase.ZoomMax;
		ZoomSocket = InDataBase.ZoomSocket;
		ActorLocationOffset = InDataBase.ActorLocationOffset;
		ActorRotationOffset = InDataBase.ActorRotationOffset;
		ActorRotationValue = InDataBase.ActorRotationValue;
		CameraOffset = InDataBase.CameraOffset;
		CameraOffsetPivotSocket = InDataBase.CameraOffsetPivotSocket;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D SceneSize = FVector2D(1024.f, 1024.f);

};

struct FWRPreviewCustomActorData
{
public:
	WRTableID PresetTID = INVALID_TABLE_TID;
	WRTableID LeftWeaponTID = INVALID_TABLE_TID;
	WRTableID RightWeaponTID = INVALID_TABLE_TID;
	WRTableID TemporaryTID = INVALID_TABLE_TID;
	TArray<WRTableID> CostumeList;
	stCustomizeInfo CustomizeInfo;
	
	void AddItem(const WRTableID IN InItemTID);
};

struct FWRPreviewInfo
{
private:
	FWRPreviewDataWidget PreviewData; // Based Custom Info
	FWRPreviewCustomActorData CustomActorData;

public:
	EWRPreviewType Type;
	FString BPPath;
	FWRPreviewLoadCompletedDelegate PreviewLoadCompletedDelegate;
	FWRPreviewExitDelegate PreviewExitDelegate;
	class AWRPreviewStudio* PreviewStudio;
	class UAnimationAsset* Anim;
	class UWRUISlot_Preview* PreviewWidget;
	bool ManualVisible;
	bool AutoScaling;

	FWRPreviewInfo() :
		Type(EWRPreviewType::Static)
		, BPPath("")
		, PreviewExitDelegate(nullptr)
		, PreviewStudio(nullptr)
		, Anim(nullptr)
		, PreviewWidget(nullptr)
		, ManualVisible(false)
		, AutoScaling(true)
	{

	}

	FORCEINLINE FWRPreviewDataWidget& GetPreviewData() { return PreviewData; }
	FORCEINLINE FWRPreviewCustomActorData GetPreviewCustomActorData() const { return CustomActorData; }
	void SetPreviewWidget(class UWRUISlot_Preview* IN InWidget);
	void SetCustomActorData(const FWRPreviewCustomActorData& IN InData) { CustomActorData = InData; }
	void Release(const bool IN InParentDestroy = false);
	void ModifyBaseData(struct FWRPreviewDataBase& IN InPreviewTablData);
};