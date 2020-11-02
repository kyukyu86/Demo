// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"


#include "WRActorComponentVoiceChatAudio.generated.h"


enum class EWRAudioType : uint8
{
	None,

	Normal,
	StandMic,
	FocusChat,
	VideoChat,
};

/**
 *		for Player
 */
class USoundAttenuation;
UCLASS()
class WR_API UWRActorComponentVoiceChatAudio : public UWRActorComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class USoundWaveProcedural* ChatSoundWave;

	UPROPERTY()
	class UAudioComponent* ChatAudioComponet;

	UPROPERTY()
	USoundAttenuation* SoundAttenuation;

	bool IsRegist;

	UPROPERTY()
	FTimerHandle PlayVoiceCaptureTimer;

	TArray<uint8> TestVoiceData;
	uint8 count;

	// 연출 관련 변수
	float FlowTimeForFX = 0.f;;
	float FlowCoolTimeForSpeakAni = 0.f;

	// [ 2020-8-10 : kyu ] SubMix Type
	EWRAudioType AudioType = EWRAudioType::None;

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Init();
	void QueueChatAudio(TArray<uint8>& voiceData, const EWRAudioType InAudioType = EWRAudioType::None, const bool IN InForce = false);
	void ChatAudioFinished(UAudioComponent* IN InAudioComponent);
	void PlayVoiceAudio();

	UFUNCTION()
	void DelegeteCallback1(const class USoundWave* PlayingSoundWave, const float PlaybackPercent);

	void SetVolume(float IN InValue);
	void SetVolumeMultiplier(float IN InValue);
	void SetPitchMultiplier(float IN InValue);

private:
	void Tick_CheckToSpeakerIcon(const float IN InDeltaTime);
	void SpawnSpeakerIcon();
	void PlayToSpeakAnimation();

	// [ 2020-8-10 : kyu ] Submix 추가
	// [ 2020-8-21 : kyu ] Attenutate 개별적용
	void SetupEffects(const EWRAudioType IN InAudioType, const bool IN InForce = false);
	void SetupSubmix(const EWRAudioType IN InAudioType);
	void SetupAttenuate(const EWRAudioType IN InAudioType);
};