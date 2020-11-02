#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include <Engine/StaticMesh.h>
#include "CustomAsset/WRLightValueAsset.h"
#include "WREquipmentLookTable.generated.h"

UENUM(BlueprintType)
enum class EWREquipmentLookType : uint8
{
	None,
	Weapon,
	Armor,
};

// USTRUCT(BlueprintType)
// struct FWRWeaponLightValues
// {
// 	GENERATED_USTRUCT_BODY()
// public:
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		float Intensity = 8.0f;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		FLinearColor Color = FLinearColor::White;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		float AttenuationRadius = 1000.0f;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		float SourceRadius = 1000.0f;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		bool bUseInverseSquaredFalloff = false;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		float LightFalloffExponent = 1.0f;
// 		
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		bool bUseTemperature = true;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		float Temperature = 6000.0f;
// 
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
// 		TAssetPtr<UMaterialInterface> LightFunctionMaterial;
// };

USTRUCT(BlueprintType)
struct FWREquipmentLookTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_EquipmentLook"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		EWREquipPart EquipPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		int32 BaseTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		int32 TopTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		int32 BottomTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta=(EditCondition="bEdit")*/)
		FString	StaticMesh;

	// [ 2020-1-7 : magedoga ] 3인칭 Body가 될수도, Weapon이 될수도 있음.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look")
		FString	SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look")
		FString	FirstPersonSkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		FName AttachSocket = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		TAssetPtr<UCurveFloat>	ChangeCurveSRC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		TAssetPtr<UCurveFloat>	ChangeCurveDEST;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		EWRAdditionalAbility	AdditionalAbility = EWRAdditionalAbility::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WR Equipment Look"/*, meta = (EditCondition = "bEdit")*/)
		TAssetPtr<UWRLightValueAsset>	LightValue;

};
