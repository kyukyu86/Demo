
#pragma once

#include "CoreMinimal.h"
#include "DMPreviewStruct.generated.h"

DECLARE_DELEGATE(FDMPreviewExitDelegate)


UENUM(BlueprintType)
enum class EDMPreviewType : uint8
{
	Static,
	Skeletal,
	DefaultActor,
	CustomActor,
};

USTRUCT(BlueprintType)
struct FDMPreviewBaseInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseRotate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseZoom = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomMax = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ZoomSocket = FName("None");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D SceneSize = FVector2D(1024.f, 1024.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator ActorRotate = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector CameraOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CameraOffsetPivotSocket = FName("None");
};

struct FDMPreviewInfo
{
private:
	FDMPreviewBaseInfo BaseInfo; // Based Custom Info

public:
	EDMPreviewType Type;
	FString BPPath;
	FDMPreviewExitDelegate PreviewExitDelegate;
	class ADMPreviewStudio* PreviewStudio;
	class UAnimationAsset* Anim;
	class UDMUISlot_Preview* PreviewWidget;

	FDMPreviewInfo() :
		Type(EDMPreviewType::Static)
		, BPPath("")
		, PreviewExitDelegate(nullptr)
		, PreviewStudio(nullptr)
		, Anim(nullptr)
		, PreviewWidget(nullptr)
	{

	}

	FORCEINLINE FDMPreviewBaseInfo& GetPreviewBaseInfo() { return BaseInfo; }
	void SetPreviewWidget(class UDMUISlot_Preview* IN InWidget);
	void Release(const bool IN InParentDestroy = false);
};
