#include "WRVoiceBase.h"
#include <JsonReader.h>
#include <JsonObject.h>
#include <JsonSerializer.h>
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRInputSystemManager.h"
#include "LogMacros.h"
#include "Runtime/Online/HTTP/Public/Http.h"

//DEFINE_LOG_CATEGORY(WRVoiceBase)

WRVoiceBase::WRVoiceBase()
{
	WRVoiceBase::InitValue();
}

WRVoiceBase::~WRVoiceBase()
{

}

void WRVoiceBase::InitValue()
{
	VoiceRecordingTimer = 0;
	TotalVoiceCaptureBuffer.Empty();
	TotalVoiceCaptureReadBytes = 0;
	bWaitRecording = true;
	bNoiseCheck = true;
	bDelayTime = false;
	bImpossibleVoiceRecording = false;
	bChatRecording = false;

	VoiceContentType = EWRVoiceContentType::NONE;
	VoiceRecordingState = EWRVoiceRecordingStateType::NONE;

	GoogleSTTRequstCnt = 0;
	bFinish = false;
}

void WRVoiceBase::Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume)
{
	VoiceRecordingTimer += DeltaTime;

	if (bWaitRecording)
	{
		if (bNoiseCheck && VoiceRecordingTimer >= NoiseCheckTime)
		{
			bDelayTime = true;
			bNoiseCheck = false;
		}
		else if (bDelayTime && VoiceRecordingTimer >= VoiceContentDelayTime + NoiseCheckTime)
		{
			if (NoiseSettingDelegate.IsBound())
			{
				NoiseSettingDelegate.ExecuteIfBound(NoiseVoulumeList);
			}
			Begin();
			bWaitRecording = false;
		}
	}

	float SilenceHoldingInitTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SilenceHoldingInitTime);
	if (VoiceRecordingTimer > SilenceHoldingInitTime && SilenceHoldingTimer < 0)
	{
		SilenceHoldingTimer = 0;
		SilenceStartDateTime = FDateTime::Now().ToString();
	}

	if (CaptureState == EVoiceCaptureState::Ok && OutAvailableVoiceData > 0)
	{
		if (bNoiseCheck && VoiceRecordingUseType == EWRVoiceRecordingUseType::COMMAND)
		{
			FString tmp1 = FString::Printf(TEXT("#####VoiceCaptureFinalVolume : %.2f"), VoiceCaptureFinalVolume);
			WRInputSystemManager::Get()->DisplayAndSaveLog(tmp1);

			NoiseVoulumeList.Add(VoiceCaptureFinalVolume);
		}
		else if (!bWaitRecording)
		{
			//FString tmp2 = FString::Printf(TEXT("[ VoiceCaptureFinalVolume : %.2f ]"), VoiceCaptureFinalVolume);
			//WRInputSystemManager::Get()->DisplayAndSaveLog(tmp2);

			TotalVoiceCaptureBuffer.Append(OutVoiceBuffer);

			if (VoiceCaptureFinalVolume > NoiseVoulume)
			{
				TotalVoiceCaptureReadBytes += OutAvailableVoiceData;

				NoneVoiceBreakTimer = 0;

				// float SilenceHoldingInitVolume = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SilenceHoldingInitVolume);
				if (SilenceHoldingTimer > 0)
				{
					SilenceHoldingTimer = 0;
					SilenceStartDateTime = FDateTime::Now().ToString();
				}
			}
			else
			{
				NoneVoiceBreakTimer += DeltaTime;
			}
		}

	}
	else if (SilenceHoldingTimer >= 0 && CaptureState == EVoiceCaptureState::NoData)
	{
		SilenceHoldingTimer += DeltaTime;
		NoneVoiceBreakTimer += DeltaTime;
	}
}

void WRVoiceBase::SetVoiceRecording(const EWRVoiceRecordingStateType IN state)
{
	VoiceRecordingState = state;

	if (VoiceRecordingState == EWRVoiceRecordingStateType::NONE)
	{
		VoiceContentType = EWRVoiceContentType::NONE;
		RemoveWaveFile();
	}

	WRVoiceContentsManager::Get()->OnVoiceRecordingStateChanged(state, VoiceContentType, bChatRecording, bImpossibleVoiceRecording);
}

void WRVoiceBase::RemoveWaveFile()
{
	if (WRConsoleManager::Get()->GetBool(EWRConsole::EnableRemoveWav) == false)
	{
		return;
	}

#if USE_AUDIOMIXER_BP_LIBRARY
	FString AbsoluteFilePath = WavFilePath + WavFileName + TEXT(".wav");
#else // USE_AUDIOMIXER_BP_LIBRARY
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/"));
	FString AbsoluteFilePath = FPaths::Combine(FilePath, WavFileName) + TEXT(".wav");
#endif // USE_AUDIOMIXER_BP_LIBRARY

	if (IFileManager::Get().FileExists(*AbsoluteFilePath) == false)
	{
		return;
	}

	IFileManager::Get().Delete(*AbsoluteFilePath);
}

bool WRVoiceBase::ConvertVoiceCaptureBufferToWav()
{
	VoiceData.Reset();

	if (TotalVoiceCaptureBuffer.Num() <= 0)
	{
		return false;
	}

	FString Text = FString::Printf(TEXT("### SerializeWaveFile ### [ data size : %d ]"), TotalVoiceCaptureBuffer.Num());
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

#if UE_4_25
	SerializeWaveFile(VoiceData, TotalVoiceCaptureBuffer.GetData(), TotalVoiceCaptureBuffer.Num(), 1, 16000);
#else // UE_4_25
	SerializeWaveFile(VoiceData, TotalVoiceCaptureBuffer.GetData(), TotalVoiceCaptureBuffer.Num(), DEFAULT_NUM_VOICE_CHANNELS, DEFAULT_VOICE_SAMPLE_RATE);
#endif // UE_4_25

	ChangeWavFormat();

	ExportRecordingVoice();

	FString base64Str = FBase64::Encode(VoiceData);

	//FString encodeStr = BytesToString(VoiceData.GetData(), VoiceData.Num());

	RequestGoogleSTT(base64Str);

	return true;
}

FString WRVoiceBase::FileExists(int32 InIndex)
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/"));
	FString FileName = FString::Printf(TEXT("Recognize_%d"), InIndex);

	FString AbsoluteFilePath = FPaths::Combine(FilePath, FileName) + ".wav";

	return (IFileManager::Get().FileExists(*AbsoluteFilePath) == false) ? FileName : this->FileExists(++InIndex);
}


void WRVoiceBase::ExportRecordingVoice()
{
	bool isWavFileSave = WRConsoleManager::Get()->GetBool(EWRConsole::Voice_WavFileSave);
	if (!isWavFileSave)
		return;

	int32 spare_data = VoiceData.Num();
	uint8 * buf = VoiceData.GetData();

	WavFileName = FileExists(0);

	FString contentPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/"));
	FString wavpath = FPaths::Combine(contentPath, WavFileName) + TEXT(".wav");

	if (FFileHelper::SaveArrayToFile(VoiceData, *wavpath) == false)
	{
		//UE_LOG(WRVoiceBase, Display, TEXT("WRVoiceBase::ExportRecordingVoice - Save Failed."));
		return;
	}

	FString Text = FString::Printf(TEXT("### Save Wav File Name : %s.%s"), *WavFileName, TEXT("wav"));
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
}


void WRVoiceBase::ChangeWavFormat()
{
	class WaveHeader
	{
	public:
		char ChunkID[4];
		unsigned int ChunkSize;
		char Format[4];
		char SubChunk1ID[4];
		unsigned int SubChunk1Size;
		short AudioFormat;
		short NumChannels;
		unsigned int SampleRate;
		unsigned int ByteRate;
		short BlockAlign;
		short BitsPerSample;
		char SubChunk2ID[4];
		unsigned int SubChunk2Size;
	};

	if (VoiceData.Num() >= sizeof(WaveHeader))
	{
		// BlockAlign = NumChannels * BitsPerSample / 8   ?�리??버그
		WaveHeader *Header = (WaveHeader*)VoiceData.GetData();
		Header->BlockAlign = 4;
	}
}

void WRVoiceBase::RequestGoogleSTT(FString pcmdata)
{
	if (GoogleSTTRequstDelegate.IsBound())
	{
		GoogleSTTRequstDelegate.ExecuteIfBound(pcmdata, GoogleSTTRequstCnt);
	}
}