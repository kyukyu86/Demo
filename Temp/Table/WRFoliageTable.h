#pragma once
#include "Base/WRTableRow.h"
#include "Sound/SoundCue.h"
#include <Particles/ParticleSystem.h>
#include "Actor/EnvActor/Foliage/WRDynamicFoliage.h"
#include "WRFoliageTable.generated.h"


USTRUCT(BlueprintType)
struct FWRFoliageTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("EnvData_Foliage"); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		int32 FoliageTID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		int32 CuttedFoliageTID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		FString CutEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		FString ThroughSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		FString CuttingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		FString DynamicFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float MaxGlow =-1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float IncreaseGlowSize = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float AttenuationRatio = -1.f;
};
