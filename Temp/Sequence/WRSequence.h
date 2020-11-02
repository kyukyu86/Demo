// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <NoExportTypes.h>
#include <MovieSceneSequencePlayer.h>
#include "WRSequence.generated.h"

class ALevelSequenceActor;

UCLASS()
class WR_API UWRSequence : public UObject
{
	GENERATED_BODY()
public:
	
	UWRSequence();
	~UWRSequence();

public:
	void Init(const FString& InPath, const FMovieSceneSequencePlaybackSettings& InSettings = FMovieSceneSequencePlaybackSettings());
	void Clear();
	bool PlaySequence();
	void StopSequence();

	UFUNCTION(BlueprintCallable)
	void OnPlay();

	UFUNCTION(BlueprintCallable)
	void OnStop();

	UFUNCTION(BlueprintCallable)
	void OnFinished();

	UFUNCTION(BlueprintCallable)
	void OnPause();

	void RunActiveTrigger();

	FString					Path;
	TArray<int32>			PostTriggerIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ALevelSequenceActor* SequenceActor = nullptr;

};