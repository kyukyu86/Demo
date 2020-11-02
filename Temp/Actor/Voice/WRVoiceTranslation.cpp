#include "WRVoiceTranslation.h"

WRVoiceTranslation::WRVoiceTranslation()
{
}


WRVoiceTranslation::~WRVoiceTranslation()
{

}

void WRVoiceTranslation::Init()
{
	bChatRecording = true;

	VoiceContentDelayTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ChatRecordingDelayTime);
	NoiseCheckTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_NoiseVolumeCheckTime);

	FString Text = FString::Printf(TEXT("%s### Wait Chat Recording Enqueue ### %s"), LINE_TERMINATOR, *FDateTime::Now().ToString());
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	SetVoiceRecording(EWRVoiceRecordingStateType::DELAY);
	VoiceRecordingUseType = EWRVoiceRecordingUseType::TRANSLATION;
}

void WRVoiceTranslation::InitValue()
{
	NoneVoiceBreakTimer = 0;

	NoneChatVoiceDataCount = 0;

}

void WRVoiceTranslation::Begin()
{
	InitValue();

	FString Text = FString::Printf(TEXT("### StartChatRecording ### %s"), *FDateTime::Now().ToString());
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
}

void WRVoiceTranslation::Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume)
{
	WRVoiceBase::Tick(DeltaTime, CaptureState, OutVoiceBuffer, OutAvailableVoiceData, VoiceCaptureFinalVolume);

	// 말이 ?�으�?그동??말한 ?�이?��? stt 처리
	float NoneVoiceBreakTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_NoneVoiceBreakTime);
	float BreakVoiceDuration = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_BreakVoiceDuration);
	float ChatRecordingTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ChatRecordingTime);

	float samples_per_second = 44100;

	float duration = (float)TotalVoiceCaptureReadBytes / samples_per_second;


	if (NoneVoiceBreakTimer > NoneVoiceBreakTime && duration >= BreakVoiceDuration)
	{
		NoneChatVoiceDataCount = 0;
		BreakVoiceCaptureBuffer();
	}
	else if ((int)VoiceRecordingTimer >= ChatRecordingTime)
	{
		BreakVoiceCaptureBuffer();
	}

	int NoneChatVoiceStopCount = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_NoneChatVoiceStopCount);

	if (NoneChatVoiceDataCount >= NoneChatVoiceStopCount)
	{
		End();
		// ������ ������ ������ �κ�
		WRVoiceContentsManager::Get()->OnChatRecordingForciblyStoped();
	}
}

void WRVoiceTranslation::BreakVoiceCaptureBuffer()
{
	FString Text = FString::Printf(TEXT("### BreakVoiceCaptureBuffer ###"));
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	// ?�는부�?처리 - ?�엽???�업?�것
	WRVoiceContentsManager::Get()->OnVoiceChatSended();

	float BreakVoiceDuration = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_BreakVoiceDuration);
	float samples_per_second = 44100;
	float duration = (float)TotalVoiceCaptureReadBytes / samples_per_second;

	if (duration < BreakVoiceDuration)
	{
		NoneChatVoiceDataCount++;
		FString Text1 = FString::Printf(TEXT("### NoneChatVoiceDataCount [ count : %d ] ###"), NoneChatVoiceDataCount);
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text1);
	}
	else if (WRVoiceBase::ConvertVoiceCaptureBufferToWav())
	{
		NoneChatVoiceDataCount = 0;
	}

	InitVoiceRecordingValue();
}


void WRVoiceTranslation::InitVoiceRecordingValue()
{
	VoiceRecordingTimer = 0;

	SilenceHoldingTimer = -1.0f;

	TotalVoiceCaptureBuffer.Empty();

	TotalVoiceCaptureReadBytes = 0;

	NoneVoiceBreakTimer = 0;
}


void WRVoiceTranslation::End(bool bCancel)
{
	VoiceRecordingUseType = EWRVoiceRecordingUseType::NONE;

	if (bCancel)
	{
		FailedVoiceCapture();
		return;
	}

	if (!ConvertVoiceCaptureBufferToWav())
	{
		FailedVoiceCapture(EWRVoiceWordType::EmptyVoice);
		return;
	}

	SetVoiceRecording(EWRVoiceRecordingStateType::NONE);
	bChatRecording = false;

	bFinish = true;
}

void WRVoiceTranslation::FailedVoiceCapture(EWRVoiceWordType InEventType)
{
	SetVoiceRecording(EWRVoiceRecordingStateType::NONE);

	bChatRecording = false;

	bFinish = true;
}

void WRVoiceTranslation::OnResponseGoogleSTT(FString transcript)
{
	WRInputSystemManager::Get()->OnResponseGoogleSTT(transcript);
}
