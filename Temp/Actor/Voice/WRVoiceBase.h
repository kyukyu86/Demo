// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

DECLARE_DELEGATE_OneParam(FWRVoiceNoiseSettingDelegate, TArray<float> )
DECLARE_DELEGATE_TwoParams(FWRVoiceGoogleSTTRequstDelegate, FString, int& )


class WRVoiceBase
{
public:
	WRVoiceBase();
	virtual ~WRVoiceBase();

public:
	virtual void InitValue();
	virtual void Begin() {}
	virtual void Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume);
	virtual void End(bool bCancel = false) {}
	virtual void FailedVoiceCapture(EWRVoiceWordType InEventType = EWRVoiceWordType::Unknown) {}
	virtual void RequestGoogleSTT(FString pcmdata);
	virtual void OnResponseGoogleSTT(FString transcript) {}

	bool isFinish() { return bFinish; }
	void ChangeWavFormat();
	bool ConvertVoiceCaptureBufferToWav();
	void SetVoiceRecording(const EWRVoiceRecordingStateType IN state);
	void RemoveWaveFile();
	
	void ExportRecordingVoice();
	FString FileExists(int32 InIndex);

	EWRVoiceRecordingStateType GetVoiceRecordingState() { return VoiceRecordingState; }
	EWRVoiceContentType GetVoiceContentType() { return VoiceContentType; }
	EWRVoiceRecordingUseType GetVoiceRecordingUseType() { return VoiceRecordingUseType; }

	int GetGoogleSTTRequstCnt() { return GoogleSTTRequstCnt; }

protected:
	TArray<uint8> VoiceData;
	EWRVoiceContentType VoiceContentType;
	EWRVoiceRecordingStateType VoiceRecordingState;
	TArray<uint8> TotalVoiceCaptureBuffer;
	uint32 TotalVoiceCaptureReadBytes;

#if USE_AUDIOMIXER_BP_LIBRARY
	FString						WavFilePath;
#endif // USE_AUDIOMIXER_BP_LIBRARY
	FString						WavFileName;

	FString SilenceStartDateTime;

	EWRVoiceRecordingUseType VoiceRecordingUseType = EWRVoiceRecordingUseType::NONE;

	float VoiceRecordingTimer;
	float VoiceContentDelayTimer;
	float VoiceContentDelayTime;
	float NoiseCheckTime;
	float SilenceHoldingTimer;
	float SilenceHoldingTime;
	float NoiseVoulume;
	bool bPrepare;
	bool bWaitRecording;
	bool bNoiseCheck;
	bool bDelayTime;
	bool bImpossibleVoiceRecording;
	bool bChatRecording;
	float LoopTimer;
	float FeedbackTimer;
	float NoneVoiceBreakTimer;

	int GoogleSTTRequstCnt;

	TArray<float> NoiseVoulumeList;

	bool bFinish;

public:
	FWRVoiceNoiseSettingDelegate NoiseSettingDelegate;
	FWRVoiceGoogleSTTRequstDelegate GoogleSTTRequstDelegate;
};
