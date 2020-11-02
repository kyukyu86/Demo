#pragma once
#include "WRVoiceBase.h"

DECLARE_DELEGATE_OneParam(FWRVoiceCommandDelegate, const WRTableID)

class WRVoiceCommand : public WRVoiceBase
{
public:
	WRVoiceCommand();
	virtual ~WRVoiceCommand();

public:
	virtual void InitValue() override;
	void Init(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceCommandDelegate callback);
	virtual void Begin() override;
	virtual void Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume) override;
	virtual void End(bool bCancel = false) override;
	virtual void FailedVoiceCapture(EWRVoiceWordType InEventType = EWRVoiceWordType::Unknown) override;
	virtual void RequestGoogleSTT(FString pcmdata) override;
	virtual void OnResponseGoogleSTT(FString transcript) override;

	void VoiceRecognition(FString transcript);
public:
	TArray<FWRSubCommandData> SubCommandList;
	FWRVoiceCommandDelegate VoiceEventCallback;
};
