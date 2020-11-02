#pragma once
#include "Base/WRTableRow.h"

#include "Network/Share/Define/Define_BluePrint.h"

#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"

#include "WRSocialTable.generated.h"


USTRUCT(BlueprintType)
struct FWRSocialAnimationTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SocialData_Animation"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		EWRCharacterType TargetCharacterType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		EWRSocialActionType SocialType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		int32 DanceGroupTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		int32 BaseMontageTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName MontageName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		int32 BaseSubMontageTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName SubMontageName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		int32 VoiceSubCommandTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName IconPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName TargetFXPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName OnlyTargetFXPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SocialData)
		FName OnlyTargetMessage;
};


USTRUCT(BlueprintType)
struct FWRDanceGroupTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SocialData_DanceGroup"); }
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceGroupData)
		TArray<int32> DanceTIDs;

};

USTRUCT(BlueprintType)
struct FWRDanceTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SocialData_Dance"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		EWRInput InputKey;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		int32 InputCallMinTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		int32 InputCallMaxTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		int32 InputLimiteTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		int32 BaseMontageTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName BaseMontageName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		int32 BaseSubMontageTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName BaseSubMontageName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName DanceKeyIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName StartSound;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)4
	//	TSoftClassPtr<class AWRVFX> StartFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName StartFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName StartImg;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DanceData)
		FName KeepImg;
};

USTRUCT(BlueprintType)
struct FWRSocialMenuSettingTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SocialData_Setting"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Setting)
		FName OpenUISoundPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Setting)
		FName CloseUISoundPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Setting)
		FName ChangeMenuSoundPath;
};