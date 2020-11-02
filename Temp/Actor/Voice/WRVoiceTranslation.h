#pragma once
#include "WRVoiceBase.h"

class WRVoiceTranslation : public WRVoiceBase
{
public:
	WRVoiceTranslation();
	virtual ~WRVoiceTranslation();

public:
	virtual void InitValue() override;
	void Init();
	virtual void Begin() override;
	virtual void Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume) override;
	virtual void End(bool bCancel = false) override;
	virtual void OnResponseGoogleSTT(FString transcript);
	virtual void FailedVoiceCapture(EWRVoiceWordType InEventType = EWRVoiceWordType::Unknown) override;

	void BreakVoiceCaptureBuffer();
	void InitVoiceRecordingValue();
protected:
	int NoneChatVoiceDataCount;
};
