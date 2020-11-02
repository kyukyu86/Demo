#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enum/EWRUIEnum.h"
#include <Components/StereoLayerComponent.h>
#include "WRWidgetTable.generated.h"

USTRUCT(BlueprintType)
struct FWRWidgetTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
		EWRUIPanelKind PanelKind = EWRUIPanelKind::None;

	UPROPERTY(EditAnywhere)
		FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FRotator Rotation = FRotator(0.f, 180.f, 0.f);

	UPROPERTY(EditAnywhere)
		FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere)
		FVector2D WidgetComponentPivot = FVector2D(0.5f, 0.5f);

	UPROPERTY(EditAnywhere)
		float OffsetScaleForFixedSize = 0.f;

	// StereoLayer
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EWRUIWidgetCreationBitFlag"))
		int32 CreationTypeBits;



	/*
	UPROPERTY(EditAnywhere)
		FVector SL_CylinderScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "1"))
		uint32 SL_SupportsDepth = 0;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "1"))
		uint32 SL_Priority = 0;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "1"))
		float SL_CylinderRadius = 0;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "1"))
		float SL_CylinderOverlayArc = 0;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "1"))
		int32 SL_CylinderHeight = 0;
	
	UPROPERTY(EditAnywhere)
		FVector StereoLayerLocation = Location;

	UPROPERTY(EditAnywhere)
		FRotator StereoLayerRotation = Rotation;

	UPROPERTY(EditAnywhere)
		FVector2D StereoRenderSize;
	*/
};