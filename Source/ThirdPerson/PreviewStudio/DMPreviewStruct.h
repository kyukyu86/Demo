
#pragma once

#include "CoreMinimal.h"
#include "DMPreviewStruct.generated.h"

DECLARE_DELEGATE(FDMPreviewExitDelegate)
DECLARE_DELEGATE_OneParam(FDMPreviewLoadCompletedDelegate, class AActor*)

UENUM(BlueprintType)
enum class EDMPreviewType : uint8
{
	Static,
	Skeletal,
	DefaultActor,
	CustomActor,
};

USTRUCT(BlueprintType)
struct FDMPreviewDataBase
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

struct FDMPreviewCustomActorData
{
public:
	// 각종 의상류 등

	void AddItem(const int32 IN InTID);
};

USTRUCT(BlueprintType)
struct FDMPreviewDataWidget : public FDMPreviewDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	void operator=(FDMPreviewDataBase& InDataBase)
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

struct FDMPreviewInfo
{
private:
	FDMPreviewDataWidget PreviewData; // Based Custom Info
	FDMPreviewCustomActorData CustomActorData;

public:
	EDMPreviewType Type;
	FString BPPath;
	FDMPreviewLoadCompletedDelegate PreviewLoadCompletedDelegate;
	FDMPreviewExitDelegate PreviewExitDelegate;
	class ADMPreviewStudio* PreviewStudio;
	class UAnimationAsset* Anim;
	class UDMUISlot_Preview* PreviewWidget;
	bool ManualVisible;
	bool AutoScaling;

	FDMPreviewInfo() :
		Type(EDMPreviewType::Static)
		, BPPath("")
		, PreviewExitDelegate(nullptr)
		, PreviewStudio(nullptr)
		, Anim(nullptr)
		, PreviewWidget(nullptr)
		, ManualVisible(false)
		, AutoScaling(true)
	{

	}

	FORCEINLINE FDMPreviewDataWidget& GetPreviewData() { return PreviewData; }
	FORCEINLINE FDMPreviewCustomActorData GetPreviewCustomActorData() const { return CustomActorData; }
	void SetPreviewWidget(class UDMUISlot_Preview* IN InWidget);
	void SetCustomActorData(const FDMPreviewCustomActorData& IN InData) { CustomActorData = InData; }
	void Release(const bool IN InParentDestroy = false);
	void ModifyBaseData(struct FDMPreviewDataBase& IN InPreviewTablData);
};
