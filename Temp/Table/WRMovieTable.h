#pragma once
#include "Base/WRTableRow.h"
#include "WRMovieTable.generated.h"

USTRUCT(BlueprintType)
struct FWRMovieTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("MovieData_Movie"); }
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//	TSoftObjectPtr<class UMediaSource> MediaSource;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//	TSoftObjectPtr<class USoundCue> MediaSound;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//	TSoftObjectPtr<class ULevelSequence> EventSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString MediaSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString MediaSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString EventSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int PanelID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float RunTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool Use3D;

};
