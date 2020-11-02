// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enum/EWRInputSystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Network/Share/Define/Define_Language.h"
#include "Network/Share/Define/Define_Type.h"
#include "Engine/EngineTypes.h"
#include "Voice.h"
#include "Net/VoiceConfig.h"
#include "Actor/Voice/WRVoiceBase.h"
#include "UWRVoiceController.generated.h"

DECLARE_DELEGATE_OneParam(FWRVoiceEventDelegate, const WRTableID)


struct FWRSubCommandData
{
public:
	WRTableID	Tid;		
	FString		NounWord;
	FString		VerbWord;

	FWRSubCommandData()
	{
		Tid = 0;
		NounWord = "";
		VerbWord = "";
	}
};

class FVoiceRecognition : public FRunnable
{

public:
	FVoiceRecognition();
	~FVoiceRecognition();

	void StartThread(const FString& InUserOrder, bool bInFinish = false);
	virtual uint32 Run() override;
	virtual void Exit() override;

private:

	FString					UserOrder;
	bool					bFinish;
	class FRunnableThread*	Thread;

};

struct FTranslationEl
{
	int Identify;
	EWRLanguageType::en Src, Tar;
	FString Str;
	FTranslationEl() { Identify = 0; Src = EWRLanguageType::en::MAX; Tar = EWRLanguageType::en::MAX; }
	FTranslationEl(int idx, EWRLanguageType::en src_l, EWRLanguageType::en tar_l, FString& str_l)
	{
		Identify = idx;
		Src = src_l;
		Tar = tar_l;
		Str = str_l;
	}
};

struct FGSTTRequstEl
{
	bool bCancel;
	FGSTTRequstEl() { bCancel = false; }
};


struct FVoiceChatData
{
	TArray<uint8> Data;
	uint32 ReadBytes;
	FVoiceChatData() { Data.Reset(); ReadBytes = 0; }

};

class UAudioComponent;
UCLASS()
class UWRVoiceController : public UObject
{
	GENERATED_BODY()

	bool bReConnect;
	int ServerID;
	FString ServerUrl[2];
public:

	void Init();
	void SetWebSocket(FString& Url);
	void WebSocketInit();

	void WebSocketRelease();
	void OnShutdown();

	bool OnTick(float InDeltaTime);

	void StartRecording(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceEventDelegate callback);
	void StartChatRecording();
	void StopRecording();
	void CancelRecording();

	bool IsCapturing();
	bool IsVoiceRecording();
	bool IsChatRecording();
	EWRVoiceContentType GetVoiceContentType();
	EWRVoiceRecordingStateType GetVoiceRecordingState();
	bool IsImpossibleVoiceRecording() { return bImpossibleVoiceRecording; }
	void SetImpossibleVoiceRecording(const bool IN bInImpossibleVoiceRecording);
	void FinishedRecognition(FVoiceRecognition* InFinishedTask);

	void OnMatchVoiceEvent(EWRVoiceWordType InEventType);

	// [ 2019-11-19 : hansang88 ] wav 파일 형식이 조금씩 다름 현재 엔진에서 고정값 셋팅하여 해당 함수 통해 강제로 바꿔줌
	void ChangeWavFormat();

	UFUNCTION()
	void OnSubmixRecordedFileDone(const class USoundWave* InSoundWave);

	// [ 2019-11-7 : hansang88 ] WebSocket Test
	void OnWebsocketConnected();
	void OnWebsocketConnectionError(const FString& Error);
	void OnWebsocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

	//번역관련 API
	int32 Translation(EWRLanguageType::en Src, EWRLanguageType::en Tar, FString Str);
	void OnResponseTranslation(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void VoiceCaptureTick(float DeltaTime);
	void VoiceCaptureStop(bool bCancel = false);

	void OnReq_VoiceMessage(const FString IN voiceData);
	void OnInf_VoiceMessage(const AID_t IN playerAID, const FString IN voiceMsg);
	void OnVoiceMessageFinished(class UAudioComponent* IN InAudioComponent, const AID_t IN InPlayerAID);

	void StopVoiceSound(UAudioComponent* IN InAudioComponent);// [ 2020-05-11 SeungYeop ] 삭제 해야하는 코드
	void RemoveStopVoiceTimerHandle(UAudioComponent* IN InAudioComponent);

	void SendVoiceChatData(TArray<uint8>& VoiceCaptureBuffer, float volume);

	EVoiceCaptureState::Type GetVoiceData(TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& OutFinalVolume);
	float CalculationVolume(TArray<uint8>& voiceBuffer, uint32& readBytes);

	EWRVoiceRecordingUseType GetVoiceRecordingUseType() { return EWRVoiceRecordingUseType::COMMAND; }

	void SetNoiseVoulume(TArray<float> volumes);
	void SetVoiceRecording(const EWRVoiceRecordingStateType IN state);

	void RequestGoogleSTT(FString pcmdata, int& outRequestCnt);
	FString GetGoogleSttTranscript(FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseGoogleSTT(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	FString FileExists(int32 InIndex);
	// [ 2019-12-12 : odin ] WebSocket log and display
	//void LogAndDisplay(FString& log);

protected:
	TSharedPtr<class IWebSocket> WebSocket;
	TSharedPtr<class IVoiceCapture> VoiceCapture;

	float LoopTimer;
	bool bImpossibleVoiceRecording = false;
	bool bFinish = false;
	int TranslationCnt;
	TQueue<FTranslationEl> TranslationQueue;
	TArray<FVoiceRecognition*>	VoiceRecognitionTask;
	TArray<uint8> VoiceData;
	
	TDoubleLinkedList<TPair<FTimerHandle, UAudioComponent*>> StopVoiceTimerHandles;
	float						FeedbackTimer;
	TArray<float> NoiseVoulumeList;
#if USE_AUDIOMIXER_BP_LIBRARY
	FString						WavFilePath;
#endif // USE_AUDIOMIXER_BP_LIBRARY
	FString						WavFileName;
	FString						UserOrder;

	TQueue<WRVoiceBase*> VoiceProcessingQueue;
	WRVoiceBase* curVoiceProcessing;
	FHttpModule* Http;

	float NoiseVoulume;
	TQueue<TArray<uint8>> ChatVoiceQueue;
	TArray<uint8> SendingChatVoiceBuffer;
	TArray<uint8> MinimumChatVoiceBuffer;
	uint8 SendingChatVoiceBufferIndex;
	
	bool bAlwaysChatNoiseCheck;
	bool bChatRecording;

	float AlwaysChatNoiseCheckTimer;

	int GoogleSTTRequstCnt;
	TQueue<int> GoogleSTTRequstQueue;
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRVoiceController, Display, All);