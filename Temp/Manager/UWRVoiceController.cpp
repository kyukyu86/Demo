// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "UWRVoiceController.h"
#include "Core/WRGameInstance.h"
#include "WRConsoleManager.h"
#include <WebSockets/Public/WebSocketsModule.h>
#include <IWebSocket.h>
#include <AudioCapture/Public/AudioCaptureComponent.h>
#include <AudioDeviceManager.h>
#include <JsonReader.h>
#include <JsonObject.h>
#include <JsonSerializer.h>
#include "Table/WRVoiceRecognitionTable.h"
#include "Network/IMMessageSender.h"
#include "AudioDevice.h"

#if PLATFORM_WINDOWS
#include <time.h>
#pragma warning(disable:4996)
#endif

#include "WRInputSystemManager.h"
#include "WRCharacterManager.h"
#include "WRSFXManager.h"
#include "WRVoiceContentsManager.h"
#include "Component/Character/WRActorComponentVoiceChatAudio.h"

#include "Sound/SoundSubmix.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Serialization/JsonWriter.h"
#include "Misc/Base64.h"
#if USE_AUDIOMIXER_BP_LIBRARY
#include "AudioMixerBlueprintLibrary.h"
#endif // USE_AUDIOMIXER_BP_LIBRARY

#include "Sound/SoundWaveProcedural.h"
#include "Define/WRPathDefine.h"
#include "Component/Character/WRActorComponentVFX.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Components/AudioComponent.h"
#include "Utility/WRActorUtility.h"
#include "Component/Character/WRActorComponentMic.h"
#include "Actor/Voice/WRVoiceCommand.h"
#include "Actor/Voice/WRVoiceTranslation.h"

// For Cpp
DEFINE_LOG_CATEGORY(WRVoiceController)

#define BYTE_RATE	32000
#define VOICE_SOUNDWAVE_COUNT 5

FVoiceRecognition::FVoiceRecognition()
{

}

FVoiceRecognition::~FVoiceRecognition()
{
// 	if (Thread != nullptr)
// 	{
// 		delete Thread;
// 		Thread = nullptr;
// 	}
}

void FVoiceRecognition::StartThread(const FString& InUserOrder, bool bInFinish/* = false*/)
{
	if (Thread != nullptr)
		return;

	bFinish = bInFinish;

	UserOrder = InUserOrder;

	FString Text = FString::Printf(TEXT("StartThread UserOrder : %s"), *UserOrder);
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	Thread = FRunnableThread::Create(this, TEXT("WRVoiceRecognition"));
	check(Thread != nullptr);
}

#define WR_SORTED_ORDER

uint32 FVoiceRecognition::Run()
{
#ifdef WR_SORTED_ORDER
	// [ 2019-12-19 : animal14 ] 
	TArray<FString> UserOrders;
	TArray<TPair<int32, WRTableID>> SortedOrders;

	FString RemainOrder = UserOrder;
	int32 SeparateIndex = 0;

	FString Text = FString::Printf(TEXT("Thread Run"));
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	while (RemainOrder.FindChar(' ', SeparateIndex) == true)
	{
		UserOrders.Add(RemainOrder.Left(SeparateIndex));
		RemainOrder = RemainOrder.Right(RemainOrder.Len() - SeparateIndex - 1);
	}

	if (RemainOrder.Len() > 0)
	{
		UserOrders.Add(RemainOrder);
	}
	if (UserOrders.Num() == 0)
	{
		UserOrders.Add(UserOrder);
	}

	UDataTable* VoiceRevisionTable = WRTableManager::Get()->GetTable<FWRVoiceRevisionData>();
	if (VoiceRevisionTable == nullptr)
	{
		Text = FString::Printf(TEXT("[FVoiceRecognition::Run] VoiceRevisionTable == nullptr"));
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
		return 0;
	}

	UDataTable* VoiceCommandTable = WRTableManager::Get()->GetTable<FWRVoiceCommandData>();
	if (VoiceCommandTable == nullptr)
	{
		Text = FString::Printf(TEXT("[FVoiceRecognition::Run] VoiceCommandTable == nullptr"));
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
		return 0;
	}

	for (auto& Order : UserOrders)
	{
		const TMap<FName, uint8*>& VoiceRevisionData = VoiceRevisionTable->GetRowMap();
		for (auto& Data : VoiceRevisionData)
		{
			FWRVoiceRevisionData* RevisionData = reinterpret_cast<FWRVoiceRevisionData*>(Data.Value);
			if (RevisionData == nullptr)
			{
				continue;
			}

			FString orgOrder = Order;
			if (Order.ReplaceInline(*RevisionData->SimilarWord.ToString(), *RevisionData->CorrectionWord.ToString()) > 0)
			{
				Text = FString::Printf(TEXT("[ TID : %s ] [ orgOrder : %s ] [ SimilarWord : %s => CorrectionWord : %s ] [ reusltOrder : %s ]"),
					*Data.Key.ToString() ,*orgOrder, *RevisionData->SimilarWord.ToString(), *RevisionData->CorrectionWord.ToString(), *Order);
				WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
			}
		}

		const TMap<FName, uint8*>& VoiceCommandData = VoiceCommandTable->GetRowMap();
		for (auto& iter : VoiceCommandData)
		{
			FWRVoiceCommandData* CommandData = reinterpret_cast<FWRVoiceCommandData*>(iter.Value);
			if (CommandData == nullptr)
			{
				continue;
			}

			if (Order.Contains(CommandData->NounWord.ToString()))
			{
				if (CommandData->VerbWord == NAME_None || UserOrder.Contains(CommandData->VerbWord.ToString()))
				{
					int32 Key = Order.Find(CommandData->NounWord.ToString());
					EWRVoiceWordType Value = CommandData->WordType;
					int32 TID = FCString::Atoi(*(iter.Key.ToString()));
					TPair<int32, WRTableID> Temp(Key, TID);

					Text = FString::Printf(TEXT("NounWord : %s, WordType : %d"),
						*CommandData->NounWord.ToString(), (uint8)CommandData->WordType);
					WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

					SortedOrders.Add(Temp);
				}

			}
		}
	}

	if (SortedOrders.Num())
	{
		SortedOrders.Sort([](const TPair<int32, WRTableID>& A, const TPair<int32, WRTableID>& B)
		{
			return A.Key < B.Key;
		});
	}
	else
	{
		TPair<int32, WRTableID> Temp(0, 0);

		SortedOrders.Add(Temp);
	}

	for (auto& SortedOrder : SortedOrders)
	{
		if (SortedOrder.Value == 0)
			WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, 0);
		else
		{
			FWRVoiceCommandData* VoiceCommandData = WRTableManager::Get()->FindRow<FWRVoiceCommandData>(SortedOrder.Value);
			if (VoiceCommandData != nullptr)
				WRInputSystemManager::Get()->SetInputVoiceEvent(VoiceCommandData->WordType, SortedOrder.Value);
			else
				WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, 0);
		}

		return 0;
	}
#else // WR_SORTED_ORDER
	UserOrder.RemoveSpacesInline();
	UDataTable* VoiceRevisionTable = WRTableManager::Get()->GetTable<FWRVoiceRevisionData>();
	if (VoiceRevisionTable->IsValidLowLevel() == true)
	{
		const TMap<FName, uint8*>& RowMap = VoiceRevisionTable->GetRowMap();
		for (auto Iter = RowMap.CreateConstIterator(); Iter; ++Iter)
		{
			FWRVoiceRevisionData* RevisionData = reinterpret_cast<FWRVoiceRevisionData*>(Iter.Value());
			if (RevisionData != nullptr)
			{
				UserOrder.ReplaceInline(*RevisionData->SimilarWord.ToString(), *RevisionData->CorrectionWord.ToString());
			}
		}
	}

	UDataTable* VoiceCommandTable = WRTableManager::Get()->GetTable<FWRVoiceCommandData>();
	if (VoiceCommandTable->IsValidLowLevel() == true)
	{
		const TMap<FName, uint8*>& RowMap = VoiceCommandTable->GetRowMap();
		for (auto Iter = RowMap.CreateConstIterator(); Iter; ++Iter)
		{
			FWRVoiceCommandData* CommandData = reinterpret_cast<FWRVoiceCommandData*>(Iter.Value());
			if (CommandData != nullptr)
			{
				if (UserOrder.Contains(CommandData->NounWord.ToString()) == true)
				{
					if (CommandData->VerbWord == NAME_None
						|| UserOrder.Contains(CommandData->VerbWord.ToString()) == true)
					{
						WRInputSystemManager::Get()->SetInputVoiceEvent(CommandData->WordType);
						return 0;
					}
				}
			}
		}
	}

	if (bFinish == true)
	{
		WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::UNKNOWN);
	}
#endif // WR_SORTED_ORDER
	
	return 0;
}

void FVoiceRecognition::Exit()
{
	if (Thread != nullptr)
	{
		delete Thread;
		Thread = nullptr;
	}

	WRInputSystemManager::Get()->FinishedRecognition(this);
}

void UWRVoiceController::Init()
{
//	ScreenKey = 1;
//	LoopTimer = 0;
//	char* c_path = TCHAR_TO_UTF8(*FPaths::ProjectLogDir());
//#ifdef PLATFORM_WINDOWS
//	time_t timer;
//	struct tm *t;
//	timer = time(NULL); // ?ÑÏû¨ ?úÍ∞Å??Ï¥??®ÏúÑÎ°??ªÍ∏∞
//	t = localtime(&timer); // Ï¥??®ÏúÑ???úÍ∞Ñ??Î∂ÑÎ¶¨?òÏó¨ Íµ¨Ï°∞Ï≤¥Ïóê ?£Í∏∞
//
//	sprintf(VoiceLogFile, "%svoice_log_%2d%02d%02d-%02d%02d%02d.txt", c_path, t->tm_year - 100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
//#else
//	strcpy(VoiceLogFile, "%svoice_log.txt", c_path);
//#endif

	LoopTimer = 0;

	bReConnect = false;
	ServerID = 0;
	ServerUrl[0] = WRConsoleManager::Get()->GetString(EWRConsole::Network_Voice_WebSocketURL);
	ServerUrl[1] = WRConsoleManager::Get()->GetString(EWRConsole::Network_Voice_WebSocketURL2);
	SetWebSocket(ServerUrl[ServerID]);

	Http = &FHttpModule::Get();
	TranslationCnt = 0;

	GoogleSTTRequstCnt = 0;
	GoogleSTTRequstQueue.Empty();

#if UE_4_25
	VoiceCapture = FVoiceModule::Get().CreateVoiceCapture(DEFAULT_DEVICE_NAME, 16000, 1);
#else // UE_4_25
	VoiceCapture = FVoiceModule::Get().CreateVoiceCapture(DEFAULT_DEVICE_NAME, DEFAULT_VOICE_SAMPLE_RATE, DEFAULT_NUM_VOICE_CHANNELS);
#endif // UE_4_25
	if (!VoiceCapture)
	{
		WRInputSystemManager::Get()->DisplayAndSaveLog(L"### CreateVoiceCapture Fail ");
	}
	else
	{
		float MicThreshold = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_MicThreshold);
		UVOIPStatics::SetMicThreshold(MicThreshold);

		WRInputSystemManager::Get()->DisplayAndSaveLog(L"### CreateVoiceCapture Success ");
		WRInputSystemManager::Get()->DisplayAndSaveLog(L"### VoiceCapture Start ");
		VoiceCapture->Start();
	}


	TDoubleLinkedList<TPair<FTimerHandle, UAudioComponent*>>::TDoubleLinkedListNode* Node = StopVoiceTimerHandles.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FTimerHandle& Handle = Node->GetValue().Key;
		WRActorUtility::ClearTimer(Handle);
	}
	StopVoiceTimerHandles.Empty();

	AlwaysChatNoiseCheckTimer = 0;
}

void UWRVoiceController::SetWebSocket(FString& Url)
{
	if (WebSocket.IsValid() == true)
	{
		WebSocket->OnConnected().RemoveAll(this);
		WebSocket->OnConnectionError().RemoveAll(this);
		WebSocket->OnClosed().RemoveAll(this);
		WebSocket->OnRawMessage().RemoveAll(this);
	}
	FWebSocketsModule& WebSocketsModule = FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
	TArray<FString> Protocols;

	//Url = TEXT("ws://echo.websocket.org");
	WebSocket = WebSocketsModule.CreateWebSocket(Url, Protocols);

	WebSocket->OnConnected().AddUObject(this, &UWRVoiceController::OnWebsocketConnected);
	WebSocket->OnConnectionError().AddUObject(this, &UWRVoiceController::OnWebsocketConnectionError);
	WebSocket->OnClosed().AddUObject(this, &UWRVoiceController::OnWebsocketClosed);
	WebSocket->OnRawMessage().AddUObject(this, &UWRVoiceController::OnRawMessage);

	//FString log = TEXT("########## connect try : ") + Url + TEXT(" ##########");
	//LogAndDisplay(log);

	FString Text = TEXT("########## connect try : ") + Url + TEXT(" ##########");
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
}

void UWRVoiceController::WebSocketInit()
{
	if (WebSocket.IsValid() == false)
	{
		return;
	}
	if (bReConnect)	//?ºÎìúÎ∞±Ïù¥ ?ÜÎäî ?ÅÌÉú?êÏÑú conected ??Î∞îÎ°ú close?òÎäî Í≤ΩÏö∞. ?úÎ≤ÑÍ∞Ä ?§Î•∏ ?åÏÑ±?∏Ïãù???ôÏûë?òÍ≥† ?àÎã§.
	{
		bReConnect = false;
		//?úÎ≤Ñ Ï≤¥Ïù∏ÏßÄ.  
		#define NUM_SERVERS 2	//?úÎ≤Ñ ?ÑÏû¨2Í∞?
		ServerID++;
		ServerID %= NUM_SERVERS;
		SetWebSocket(ServerUrl[ServerID]);
		WebSocket->Connect();
	}

	if (WebSocket->IsConnected())
	{
		WebSocket->Close();
	}
	if (WebSocket->IsConnected() == false)
	{
		WebSocket->Connect();
	}
}

void UWRVoiceController::WebSocketRelease()
{
	if (WebSocket.IsValid() == true)
	{
		WebSocket->OnConnected().RemoveAll(this);
		WebSocket->OnConnectionError().RemoveAll(this);
		WebSocket->OnClosed().RemoveAll(this);
		WebSocket->OnRawMessage().RemoveAll(this);

		if (WebSocket->IsConnected())
		{
			WebSocket->Close();
		}
		WebSocket.Reset();
	}
}

void UWRVoiceController::OnShutdown()
{
	WebSocketRelease();

	for (int i = 0; i < VoiceRecognitionTask.Num(); ++i)
	{
		delete VoiceRecognitionTask[i];
	}

	VoiceRecognitionTask.Empty();
}

bool UWRVoiceController::OnTick(float InDeltaTime)
{
	LoopTimer += InDeltaTime;

	if (curVoiceProcessing && curVoiceProcessing->isFinish())
	{
		delete curVoiceProcessing;
		curVoiceProcessing = nullptr;
		VoiceProcessingQueue.Pop();
	}

	if (curVoiceProcessing == nullptr && VoiceProcessingQueue.Peek() != nullptr)
	{
		curVoiceProcessing = *(VoiceProcessingQueue.Peek());
	}

	VoiceCaptureTick(InDeltaTime);

	if (WebSocket.IsValid() == false)
	{
		return false;
	}

	if (WebSocket->IsConnected())
	{
		float feed_back_ftime = LoopTimer - FeedbackTimer;
		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::ResponseTime);

		if (feed_back_ftime < Duration)
		{
			return false;
		}

		WebSocket->Close();

		//for (auto& Task : VoiceRecognitionTask)
		//{
		//	Task->Stop();

		//	delete Task;
		//	Task = nullptr;
		//}
		//VoiceRecognitionTask.Empty();

		FString Text = FString::Printf(TEXT("##feedback time is over %.2f"), feed_back_ftime);
		//LogAndDisplay(Text);
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

		FeedbackTimer = LoopTimer;
	}
	return false;
}

void UWRVoiceController::StartRecording(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceEventDelegate callback)
{
	WRVoiceCommand* VoiceCommand = new WRVoiceCommand;
	VoiceCommand->NoiseSettingDelegate = FWRVoiceNoiseSettingDelegate::CreateUObject(this, &UWRVoiceController::SetNoiseVoulume);
	VoiceCommand->GoogleSTTRequstDelegate = FWRVoiceGoogleSTTRequstDelegate::CreateUObject(this, &UWRVoiceController::RequestGoogleSTT);
	VoiceCommand->Init(InVoiceContentType, subCommand, callback);
	VoiceProcessingQueue.Enqueue(VoiceCommand);
}

void UWRVoiceController::StartChatRecording()
{
	WRVoiceTranslation* VoiceTranslation = new WRVoiceTranslation;
	VoiceTranslation->GoogleSTTRequstDelegate = FWRVoiceGoogleSTTRequstDelegate::CreateUObject(this, &UWRVoiceController::RequestGoogleSTT);
	VoiceTranslation->Init();
	VoiceProcessingQueue.Enqueue(VoiceTranslation);
}

void UWRVoiceController::StopRecording()
{
	VoiceCaptureStop();
}

static int16_t fix_int16(int32_t num)
{
	if (num > INT16_MAX)
		return (INT16_MAX);
	if (num < INT16_MIN)
		return (INT16_MIN);
	return (int16_t)num;
}

//wav?åÏùº Î≥ÄÍ≤?
static void WaveNormalize(int16 *buf, int32 f_size, float normal_value)
{
	int32 i;
	int16 max_su = 0;
	for (i = 0; i < f_size; i++)
	{
		max_su = FMath::Max(FMath::Abs(buf[i]), max_su);
	}
	float max_float_su = max_su / (float)INT16_MAX;
	if (max_float_su <= 0.001f)
	{
		max_float_su = normal_value;
	}
	float multiply = normal_value / max_float_su;

	for (i = 0; i < f_size; i++)
	{
		buf[i] = fix_int16((int32_t)(buf[i] * multiply));
	}
}

FString UWRVoiceController::FileExists(int32 InIndex)
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/"));
	FString FileName = FString::Printf(TEXT("Recognize_%d"), InIndex);

	FString AbsoluteFilePath = FPaths::Combine(FilePath, FileName) + ".wav";

	return (IFileManager::Get().FileExists(*AbsoluteFilePath) == false) ? FileName : this->FileExists(++InIndex);
}



void UWRVoiceController::CancelRecording()
{
	VoiceCaptureStop(true);

}

void UWRVoiceController::SetImpossibleVoiceRecording(const bool IN bInImpossibleVoiceRecording)
{
	if (bImpossibleVoiceRecording == bInImpossibleVoiceRecording)
		return;

	bImpossibleVoiceRecording = bInImpossibleVoiceRecording;

	if (bImpossibleVoiceRecording)
	{
		// [SeungYeop 2020/07/22] : Ï≤?ûê Í∏∞Ï? Î≤àÏó≠???§Ïñ¥Í∞ÄÎ©¥ÏÑú Î≤àÏó≠?Ä ImpossibleÎ°?ÎßâÏ? ?äÎäê??
		//if (VoiceRecordingState != EWRVoiceRecordingStateType::NONE)
		if (bChatRecording == false && GetVoiceRecordingState() != EWRVoiceRecordingStateType::NONE)
		{
			// Í∞ïÏ†ú Ï§ëÏ?
			CancelRecording();
		}	
	}

	WRVoiceContentsManager::Get()->OnImpossibleVoiceRecordingChanged(GetVoiceRecordingState(), GetVoiceContentType(), bChatRecording, bImpossibleVoiceRecording);
}

void UWRVoiceController::FinishedRecognition(FVoiceRecognition* InFinishedTask)
{
	if (VoiceRecognitionTask.Contains(InFinishedTask) == true)
	{
		VoiceRecognitionTask.Remove(InFinishedTask);
		delete InFinishedTask;
	}
}

void UWRVoiceController::OnMatchVoiceEvent(EWRVoiceWordType InEventType)
{
	bFinish = true;
	WebSocket->Close();

	for (int i = 0; i < VoiceRecognitionTask.Num(); ++i)
	{
		VoiceRecognitionTask[i]->Stop();
	}

	float feed_back_ftime = LoopTimer - FeedbackTimer;

	//FString tmp = FString::Printf(TEXT("##match speech to text feedback time is %.2f, word is %d"), feed_back_ftime, InEventType);
	//LogAndDisplay(tmp);

	FString Text = FString::Printf(TEXT("##match speech to text feedback time is %.2f, word is %d"), feed_back_ftime, InEventType);
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
}

void UWRVoiceController::ChangeWavFormat()
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
		// BlockAlign = NumChannels * BitsPerSample / 8   ?∏Î¶¨??Î≤ÑÍ∑∏
		WaveHeader *Header = (WaveHeader*)VoiceData.GetData();
		Header->BlockAlign = 4;
	}
}


void UWRVoiceController::OnSubmixRecordedFileDone(const USoundWave* InSoundWave)
{
#if USE_AUDIOMIXER_BP_LIBRARY

	VoiceData.Reset();
	FString FileFullPath = WavFilePath + WavFileName + TEXT(".wav");
	if (FFileHelper::LoadFileToArray(VoiceData, *FileFullPath) == false)
	{
		UE_LOG(WRVoiceController, Display, TEXT("UWRVoiceController::OnSubmixRecordedFileDone - Load failed Wav [%s]"), *FileFullPath);
		return;
	}

	int32 NumOfVoiceData = VoiceData.Num();
	uint8 * Buffer = VoiceData.GetData();

	int32 bNormalizeOn = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_NormalizeOn);
	if (bNormalizeOn)	//waveÎ≥ºÎ•®Ï°∞Ï†à
	{
#define _WAVE_HEADER_SIZE	44
		int w_data_size = (NumOfVoiceData - _WAVE_HEADER_SIZE) / 2;
		WaveNormalize((int16*)&Buffer[_WAVE_HEADER_SIZE], w_data_size, WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_Normalize));
	}

	int32 bGoogleSTTOff = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_GoogleSTTOff);
	if (bGoogleSTTOff && !bChatRecording)
	{
		// ?πÏÜåÏº??∞Í≤∞ ???ÑÏÜ°
		WebSocketInit();
	}
	else
	{
		// Íµ¨Í? STT
		FString base64Str = FBase64::Encode(VoiceData);

		//FString encodeStr = BytesToString(VoiceData.GetData(), VoiceData.Num());

		//TArray<uint8> decodePCM;
		//FBase64::Decode(base64Str, decodePCM);

		//RequestGoogleSTT(base64Str);
	}

#else // USE_AUDIOMIXER_BP_LIBRARY
	WebSocketInit();
	//WavFileName = TEXT("Recognize_4");
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/"));
	FString AbsoluteFilePath = FPaths::Combine(FilePath, WavFileName) + TEXT(".wav");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	int64 FSize = PlatformFile.FileSize(*AbsoluteFilePath);
	IFileHandle* FileHandle = PlatformFile.OpenRead(*AbsoluteFilePath);
	if (FileHandle)
	{
		VoiceData.SetNumZeroed(FSize + 1);
		FileHandle->Read(VoiceData.GetData(), FSize);
		ChangeWavFormat();

		VoiceData[FSize] = 0;
		delete FileHandle;
	}
#endif // USE_AUDIOMIXER_BP_LIBRARY
}

//void UWRVoiceController::LogAndDisplay(FString& log)
//{
//	int32 bDisplay = WRConsoleManager::Get()->GetInt(EWRConsole::Network_Voice_DebugVoiceToText);
//	if (bDisplay)
//	{
//		GEngine->AddOnScreenDebugMessage(ScreenKey++, 2.0f, FColor::Green, log);
//	}
//
//	FILE *fp = fopen(VoiceLogFile, "a+");
//	if (fp)
//	{
//		fprintf(fp, "%s\n", TCHAR_TO_UTF8(*log));
//		fclose(fp);
//	}
//}

void UWRVoiceController::OnWebsocketConnected()
{
	bFinish = false;
	if (WebSocket.IsValid() == true && WebSocket->IsConnected() == true)
	{
		SetVoiceRecording(EWRVoiceRecordingStateType::VOICESENDING);
#if PLATFORM_WINDOWS
		time_t timer;
		struct tm *t;
		timer = time(NULL); // ?ÑÏû¨ ?úÍ∞Å??Ï¥??®ÏúÑÎ°??ªÍ∏∞
		t = localtime(&timer); // Ï¥??®ÏúÑ???úÍ∞Ñ??Î∂ÑÎ¶¨?òÏó¨ Íµ¨Ï°∞Ï≤¥Ïóê ?£Í∏∞

		TArray<FStringFormatArg> StringArgs;

		StringArgs.Add(t->tm_year - 100);
		StringArgs.Add(t->tm_mon + 1);
		StringArgs.Add(t->tm_mday);

		int IsPM = t->tm_hour / 12;
		int Hour = (t->tm_hour > 12) ? t->tm_hour - 12 : ((t->tm_hour == 12) ? t->tm_hour : t->tm_hour % 12);

		StringArgs.Add(Hour);
		StringArgs.Add(t->tm_min);

		FString Text = IsPM ? FString::Format(TEXT("Websocket CONNECTED and SEND 20{0}-{1}-{2} PM {3}:{4}"), StringArgs) : FString::Format(TEXT("Websocket CONNECTED and SEND 20{0}-{1}-{2} AM {3}:{4}"), StringArgs);
#else // PLATFORM_WINDOWS
		FString Text = TEXT("Websocket CONNECTED and SEND");
#endif // PLATFORM_WINDOWS
		//LogAndDisplay(Text);
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

		FeedbackTimer = LoopTimer;
		int32 spare_data = VoiceData.Num();
		uint8 * buf = VoiceData.GetData();

		/*
		int32 bNormalizeOn = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_NormalizeOn);
		if (bNormalizeOn)	//waveÎ≥ºÎ•®Ï°∞Ï†à
		{
#define _WAVE_HEADER_SIZE	44
			int w_data_size = (spare_data - _WAVE_HEADER_SIZE)/2;
			WaveNormalize((int16*)&buf[_WAVE_HEADER_SIZE], w_data_size, WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_Normalize));
		}
		*/

		for (; spare_data > 0; buf += (BYTE_RATE / 4), spare_data -= BYTE_RATE / 4)
		{
			int32 send_data = FMath::Min(spare_data, BYTE_RATE / 4);
			WebSocket->Send(buf, send_data, true);
		}

		//WebSocket->Send(VoiceData.GetData(), VoiceData.Num(), true);
		VoiceData.Empty();
	}
	else
	{
		SetVoiceRecording(EWRVoiceRecordingStateType::NONE);
	}
}

void UWRVoiceController::OnWebsocketConnectionError(const FString& Error)
{
	WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, INVALID_RECORD_TID);
	//WRInputSystemManager::Get()->InputVoiceEvent(INVALID_RECORD_TID);
	SetVoiceRecording(EWRVoiceRecordingStateType::NONE);

	//FString tmp = TEXT("Websocket ERROR : ") + Error;
	//LogAndDisplay(tmp);

	FString Text = TEXT("Websocket ERROR : ") + Error;
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	bReConnect = true;
} 

void UWRVoiceController::OnWebsocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	if (bFinish == false)
	{
		WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, INVALID_RECORD_TID);
		//WRInputSystemManager::Get()->InputVoiceEvent(INVALID_RECORD_TID);
	}

	//FString tmp = TEXT("Websocket CLOSED");
	//LogAndDisplay(tmp);

	FString Text = TEXT("Websocket CLOSED\n\n");
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	SetVoiceRecording(EWRVoiceRecordingStateType::NONE);
}

void UWRVoiceController::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	FString RecvMsg(Size, (char*)Data);
	
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(RecvMsg);
	TSharedPtr<FJsonObject> json_root = MakeShareable(new FJsonObject());

	FString str;
	const TArray<TSharedPtr<FJsonValue>>* str_array;
	if (FJsonSerializer::Deserialize(reader, json_root) && json_root.IsValid())
	{
		const TSharedPtr<FJsonObject>* json_result;
		bool ret = json_root->TryGetObjectField(TEXT("result"), json_result);
		if (ret)
		{
			str = (*json_result)->GetStringField(TEXT("final"));
			if (!str.Compare(TEXT("true")))
			{
				bFinish = true;
			}

			ret = (*json_result)->TryGetArrayField(TEXT("hypotheses"), str_array);
			if (ret)
			{
				TSharedPtr < FJsonObject > json_hypotheses = (*str_array)[0]->AsObject();
				str = json_hypotheses->GetStringField(TEXT("transcript"));
				
				UserOrder += str;
	
				//FString Msg = FString::Printf(TEXT("MESSAGE : %s"), *str);
				//LogAndDisplay(Msg);

				FString Text = FString::Printf(TEXT("MESSAGE : %s"), *str);
				WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

				FVoiceRecognition* Task = new FVoiceRecognition();
				VoiceRecognitionTask.Add(Task);
				Task->StartThread(UserOrder, bFinish);
			}
			
			if (bFinish)
			{
				float feed_back_ftime = LoopTimer - FeedbackTimer;

				//FString tmp = FString::Printf(TEXT("##final speech to text feedback time is %.2f"), feed_back_ftime);
				//LogAndDisplay(tmp);

				FString Text = FString::Printf(TEXT("##final speech to text feedback time is %.2f"), feed_back_ftime);
				WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

				//FVoiceRecognition* Task = new FVoiceRecognition();
				//VoiceRecognitionTask.Add(Task);
				//Task->StartThread(UserOrder, true);
				//VoiceRecognition();
				//WebSocket->Close();
			}

			//float feed_back_ftime = LoopTimer - FeedbackTimer;
			//float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::animal14);

			//if (feed_back_ftime > Duration)
			//{
			//	WebSocket->Close();

			//	for (auto& Task : VoiceRecognitionTask)
			//	{
			//		Task->Stop();

			//		delete Task;
			//		Task = nullptr;
			//	}
			//	VoiceRecognitionTask.Empty();

			//	FString Text = FString::Printf(TEXT("##feedback time is over %.2f"), feed_back_ftime);
			//	LogAndDisplay(Text);
			//}
		}
		else
		{
			bReConnect = true;	//?úÎ≤ÑÍ∞Ä busy?ÅÌÉú?¥Î©¥ ?¥Ï™Ω?ºÎ°ú ?®Îã§.
		}
	}
}

int32 UWRVoiceController::Translation(EWRLanguageType::en Src, EWRLanguageType::en Tar, FString Str)
{
	TranslationCnt++;
	if (Src == Tar)
	{
		WRInputSystemManager::Get()->OnResponseTranslation(TranslationCnt, Src, Tar, Str, Str);
		return TranslationCnt;
	}
	FString tbl[EWRLanguageType::en::MAX] = { L"ko",L"ja",L"zh",L"en" };

	FString url = "https://translation.googleapis.com/language/translate/v2?key=AIzaSyCTmxx_zAV2Y2dh-6RUDzOctcLh-wxMtmA";
	FString encoded = FGenericPlatformHttp::UrlEncode(Str);

	url = url + L"&q=" + encoded + L"&target=" + tbl[(int32)Tar];

	WRInputSystemManager::Get()->DisplayAndSaveLog(L"### google translation send: " + url);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request->SetURL(url);
	Request->SetVerb("GET");

	FTranslationEl el(TranslationCnt, Src, Tar, Str);
	TranslationQueue.Enqueue(el);

	Request->OnProcessRequestComplete().BindUObject(this, &UWRVoiceController::OnResponseTranslation);
	Request->ProcessRequest();

	return TranslationCnt;
}

void UWRVoiceController::OnResponseTranslation(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool q_ret = false;
	FTranslationEl el;
	if (TranslationQueue.Dequeue(el))
	{
		q_ret = true;
	}
	else
	{
		el.Identify = 0;
		el.Src = EWRLanguageType::en::MAX;
		el.Tar = EWRLanguageType::en::MAX;
		el.Str = L"Translation Queue Error";
	}

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	if (!Response || bWasSuccessful == false)
	{
		return;
	}

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		const TSharedPtr<FJsonObject>* json_result1;
		bool ret = JsonObject->TryGetObjectField(TEXT("data"), json_result1);
		if (ret)
		{
			const TArray<TSharedPtr<FJsonValue>>* json_array;
			ret = (*json_result1)->TryGetArrayField(TEXT("translations"), json_array);
			if (ret)
			{
				TSharedPtr < FJsonObject > translatedText = (*json_array)[0]->AsObject();
				FString translatedOrgStr = translatedText->GetStringField("translatedText");
				FString translatedStr = translatedOrgStr.Replace(L"&#39;",L"'");
				FString recieved = EWRLanguageType::GetStr(el.Src) + L" (Î≤àÏó≠)-> " + translatedStr;
//				GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, recieved);
				WRInputSystemManager::Get()->DisplayAndSaveLog(L"### translation response : " + recieved);
				WRInputSystemManager::Get()->OnResponseTranslation(el.Identify, el.Src, el.Tar, el.Str, translatedStr);
			}
			else
			{
				WRInputSystemManager::Get()->OnResponseTranslation(el.Identify, el.Src, el.Tar, el.Str, L"");
				WRInputSystemManager::Get()->DisplayAndSaveLog(L"### translation response translations error: " + Response->GetContentAsString());
			}
		}
		else
		{
			WRInputSystemManager::Get()->OnResponseTranslation(el.Identify, el.Src, el.Tar, el.Str, L"");
			WRInputSystemManager::Get()->DisplayAndSaveLog(L"### translation response data error: " + Response->GetContentAsString());
		}
	}
	else
	{
		WRInputSystemManager::Get()->OnResponseTranslation(el.Identify, el.Src, el.Tar, el.Str, L"");
		WRInputSystemManager::Get()->DisplayAndSaveLog(L"### translation response error: " + Response->GetContentAsString());
	}
}

EVoiceCaptureState::Type UWRVoiceController::GetVoiceData(TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& OutFinalVolume)
{
#if !PLATFORM_PS4
	EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(OutAvailableVoiceData);
	if (CaptureState == EVoiceCaptureState::Ok && OutAvailableVoiceData > 0)
	{
		uint32 VoiceCaptureReadBytes;

		OutVoiceBuffer.SetNumUninitialized(OutAvailableVoiceData);

		VoiceCapture->GetVoiceData(OutVoiceBuffer.GetData(), OutAvailableVoiceData, VoiceCaptureReadBytes);

		OutFinalVolume = CalculationVolume(OutVoiceBuffer, VoiceCaptureReadBytes);

		//UE_LOG(WRVoiceController, Display, TEXT("voice data volume : %f"), OutFinalVolume);
	}

	return CaptureState;
#else
 	EVoiceCaptureState::Type CaptureState = EVoiceCaptureState::NoData;
 	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
 	if (MyChar->IsValidLowLevel() == true)
 	{
 		IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(MyChar->GetWorld());
 		if (PS4Subsystem != nullptr)
 		{
 			IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
 
 			if (VoiceInt.IsValid() == true)
 			{
 	#if WR_ADD_VOICE_PORT
				uint8_t readBuffer[9192] = {0};
 				uint32 Length = 0;
 
 				if (VoiceInt->ReadLocalVoice(readBuffer, sizeof(readBuffer), Length))
 				{
 					VoiceInt->WriteVoice(0, readBuffer, Length);

					uint8 PCMBuffer[9192] = { 0 };
					uint32 PCMBufferSize = 0;
 					VoiceInt->GetVoicePCMData(PCMBuffer, PCMBufferSize);
 					if (PCMBufferSize > 0)
 					{
						OutVoiceBuffer.SetNumZeroed(PCMBufferSize);
						FMemory::Memcpy(OutVoiceBuffer.GetData(), PCMBuffer, PCMBufferSize);

						OutFinalVolume = CalculationVolume(OutVoiceBuffer, PCMBufferSize);
						OutAvailableVoiceData = PCMBufferSize;
 						CaptureState = EVoiceCaptureState::Ok;
 					}
 
 // 					FString VoiceBuffer = BytesToString(PCMBuffer, PCMBufferSize);
 // 					IMMessageSender::RequestVoiceChat(VoiceBuffer, PCMBufferSize, TargetAID);
 				}
 	#endif //WR_ADD_VOICE_PORT
 			}
 		}
 	}
 	return CaptureState;
//	return EVoiceCaptureState::NoData;
#endif
}

float UWRVoiceController::CalculationVolume(TArray<uint8>& voiceBuffer, uint32& readBytes)
{
	int16_t VoiceCaptureSample = 0;
	float VoiceCaptureTotalSquared = 0;

	for (uint32 i = 0; i < (readBytes / 2); i++)
	{
		VoiceCaptureSample = (voiceBuffer[i * 2 + 1] << 8) | voiceBuffer[i * 2];
		VoiceCaptureTotalSquared += ((float)VoiceCaptureSample * (float)VoiceCaptureSample);
	}

	float VoiceCaptureMeanSquare = (2 * (VoiceCaptureTotalSquared / voiceBuffer.Num()));
	float VoicecapturRms = FMath::Sqrt(VoiceCaptureMeanSquare);
	float OutFinalVolume = ((VoicecapturRms / 32768.0) * 200.f);

	return OutFinalVolume;
}

void UWRVoiceController::VoiceCaptureTick(float DeltaTime)
{
	TArray<uint8> VoiceCaptureBuffer;
	VoiceCaptureBuffer.Reset();
	uint32 VoiceCaptureReadBytes = 0;
	float VoiceCaptureFinalVolume = 0;
	EVoiceCaptureState::Type CaptureState;

#if !PLATFORM_PS4
	if (!VoiceCapture.IsValid())
		return;

	if (!VoiceCapture->IsCapturing())
		return;
#endif

	CaptureState = GetVoiceData(VoiceCaptureBuffer, VoiceCaptureReadBytes, VoiceCaptureFinalVolume);

#if !PLATFORM_PS4
		// ªÛΩ√√§∆√ ≥Î¿Ã¡Ó ∫º∑˝ √º≈©
		float noiseCheckTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AlwaysChatNoiseCheckTime);
		if (!bAlwaysChatNoiseCheck && AlwaysChatNoiseCheckTimer > noiseCheckTime)
		{
			AlwaysChatNoiseCheckTimer = 0;
			bAlwaysChatNoiseCheck = true;

			NoiseVoulumeList.Add(VoiceCaptureFinalVolume);
			SetNoiseVoulume(NoiseVoulumeList);
		}
		else if(!bAlwaysChatNoiseCheck)
		{
			AlwaysChatNoiseCheckTimer += DeltaTime;
			NoiseVoulumeList.Add(VoiceCaptureFinalVolume);
		}

		if (bAlwaysChatNoiseCheck)
		{
			/* Ï±ÑÌåÖ ?åÏÑ± ?∞Ïù¥??Î≥¥ÎÇ¥Í∏?*/
			SendVoiceChatData(VoiceCaptureBuffer, VoiceCaptureFinalVolume);
		}
#else

		SendVoiceChatData(VoiceCaptureBuffer, VoiceCaptureFinalVolume);
#endif

	if (curVoiceProcessing)
	{
		curVoiceProcessing->Tick(DeltaTime, CaptureState, VoiceCaptureBuffer, VoiceCaptureReadBytes, VoiceCaptureFinalVolume);
	}
}


void UWRVoiceController::SendVoiceChatData(TArray<uint8>& VoiceCaptureBuffer,float volume)
{	
	if (VoiceCaptureBuffer.Num() > 0)
	{
		//int w_data_size = VoiceCaptureBuffer.Num() / 2;
		//WaveTest((int16*)VoiceCaptureBuffer.GetData(), w_data_size);

		ChatVoiceQueue.Enqueue(VoiceCaptureBuffer);
	}

	if (SendingChatVoiceBuffer.Num() <= 0 && !ChatVoiceQueue.IsEmpty())
	{
		SendingChatVoiceBuffer.Empty();
		ChatVoiceQueue.Dequeue(SendingChatVoiceBuffer);
		SendingChatVoiceBufferIndex = 0;
	}
	
	if (SendingChatVoiceBuffer.Num() > 0)
	{
		TArray<uint8> sendBuf;
		uint32  sendSize;

		int sendBufferSize = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_SendChatBufferSize);
		if (SendingChatVoiceBuffer.Num() > sendBufferSize)
		{
			sendSize = sendBufferSize;
		}
		else
		{
			sendSize = SendingChatVoiceBuffer.Num();
		}

		sendBuf.AddZeroed(sendSize);
		FMemory::Memcpy(sendBuf.GetData(), SendingChatVoiceBuffer.GetData(), sendSize);
		SendingChatVoiceBuffer.RemoveAt(0, sendSize);

		FString encodeStr = BytesToString(sendBuf.GetData(), sendSize);
		OnReq_VoiceMessage(encodeStr);
		SendingChatVoiceBufferIndex = sendSize;

		// ?åÏä§?∏Ïö©
		//AID_t TargetAID = WRCharacterManager::Get()->GetMyAID();
		//OnInf_VoiceMessage(TargetAID, encodeStr, true, true, 0);	
	}
}

void UWRVoiceController::VoiceCaptureStop(bool bCancel)
{
	if (!curVoiceProcessing)
	{
		return;
	}

	curVoiceProcessing->End(bCancel);
}


void UWRVoiceController::OnReq_VoiceMessage(const FString IN voiceData)
{
	IMMessageSender::RequestVoiceMessage(voiceData);
}

void UWRVoiceController::OnInf_VoiceMessage(const AID_t IN playerAID, const FString IN voiceMsg)
{
	if (voiceMsg.IsEmpty())
		return;

	uint32 size = voiceMsg.Len();
	TArray<uint8> voiceData;
	if (size > 0)
	{
		voiceData.AddUninitialized(size);
		StringToBytes(voiceMsg, voiceData.GetData(), voiceMsg.Len());
	}

	AWRCharacterPlayer* Player = nullptr;
	if (WRVoiceContentsManager::Get()->GetVideoChatController().GetVideoChatPlayerAID() == playerAID)
	{
		Player = WRCharacterManager::Get()->GetMyCharacter();
	}
	else
	{
		Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(playerAID));
	}

	if (Player == nullptr)
		return;

	UWRActorComponentVoiceChatAudio* ChatAudioComponent = Player->GetComponent<UWRActorComponentVoiceChatAudio>();
	if (ChatAudioComponent)
	{
		// [ 2020-8-10 : kyu ] Determine Audio Type
		EWRAudioType AudioType = EWRAudioType::Normal;
				
		UWRActorComponentMic* MicComponent = Player->GetComponent<UWRActorComponentMic>();
		if (MicComponent && MicComponent->IsActivating())
		{
			AudioType = EWRAudioType::StandMic;

			int32 bMicNormalizeOn = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_MicNormalizeOn);
			if (bMicNormalizeOn)
			{
				int w_data_size = voiceData.Num() / 2;
				WaveNormalize((int16*)voiceData.GetData(), w_data_size, WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_Normalize));
			}
		}
		else if (WRVoiceContentsManager::Get()->GetVideoChatController().GetVideoChatPlayerAID() == playerAID)
		{
			AudioType = EWRAudioType::VideoChat;
		}
		else if (WRVoiceContentsManager::Get()->IsVisibleFocusTalkUI(playerAID))
		{
			AudioType = EWRAudioType::FocusChat;
		}

		ChatAudioComponent->QueueChatAudio(voiceData, AudioType);
	}
}

void UWRVoiceController::OnVoiceMessageFinished(UAudioComponent* IN InAudioComponent, const AID_t IN InPlayerAID)
{
	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InPlayerAID));
	if (Player == nullptr)
		return;

	UWRActorComponentVFX* ActorComponentVFX = Player->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX == nullptr)
		return;

	ActorComponentVFX->RemoveVFX(EWRVFXActor::SpeakerIcon);
}

void UWRVoiceController::StopVoiceSound(UAudioComponent* IN InAudioComponent)
{
	RemoveStopVoiceTimerHandle(InAudioComponent);
	if (InAudioComponent->IsValidLowLevel() == false)
		return;

	InAudioComponent->Stop();
}

void UWRVoiceController::RemoveStopVoiceTimerHandle(UAudioComponent* IN InAudioComponent)
{
	TDoubleLinkedList<TPair<FTimerHandle, UAudioComponent*>>::TDoubleLinkedListNode* Node = StopVoiceTimerHandles.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		UAudioComponent* AudioComponent= Node->GetValue().Value;
		if(AudioComponent != InAudioComponent)
			continue;

		StopVoiceTimerHandles.RemoveNode(Node);
		break;
	}
}
bool UWRVoiceController::IsCapturing()
{
	if (!VoiceCapture) 
		return false; 
	
	return VoiceCapture->IsCapturing();
}

bool UWRVoiceController::IsChatRecording()
{
	if (!curVoiceProcessing)
		return false;

	return curVoiceProcessing->GetVoiceRecordingUseType() == EWRVoiceRecordingUseType::TRANSLATION;
}

bool UWRVoiceController::IsVoiceRecording()
{
	if (IsChatRecording())
		return false;

	if (!curVoiceProcessing)
		return false;

	return curVoiceProcessing->GetVoiceRecordingState() != EWRVoiceRecordingStateType::NONE;
}

EWRVoiceContentType UWRVoiceController::GetVoiceContentType()
{
	if (!curVoiceProcessing)
		return EWRVoiceContentType::NONE;

	return curVoiceProcessing->GetVoiceContentType();
}

EWRVoiceRecordingStateType UWRVoiceController::GetVoiceRecordingState()
{
	if (!curVoiceProcessing)
		return EWRVoiceRecordingStateType::NONE;

	return curVoiceProcessing->GetVoiceRecordingState();
}

void UWRVoiceController::SetNoiseVoulume(TArray<float> volumes)
{
	if (volumes.Num() <= 0)
	{
		return;
	}

	float newNosizeVoulume = 0;
	float sum = 0;
	for (int i = 0; i < volumes.Num(); i++)
	{
		sum += volumes[i];
	}
	newNosizeVoulume = sum / volumes.Num();

	float NoiseVolumeScaling = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_NoiseVolumeScaling);
	newNosizeVoulume = newNosizeVoulume * NoiseVolumeScaling;
	if (newNosizeVoulume > 0 && NoiseVoulume <= 0)
	{
		NoiseVoulume = newNosizeVoulume;
		FString tmp2 = FString::Printf(TEXT("[ NoiseVoulume : %.2f ]"), NoiseVoulume);
		WRInputSystemManager::Get()->DisplayAndSaveLog(tmp2);
	}
	else if (newNosizeVoulume > 0)
	{
		NoiseVoulume = (newNosizeVoulume + NoiseVoulume) * 0.5;
		FString tmp2 = FString::Printf(TEXT("[ NoiseVoulume : %.2f ]"), NoiseVoulume);
		WRInputSystemManager::Get()->DisplayAndSaveLog(tmp2);
	}
}

void UWRVoiceController::SetVoiceRecording(const EWRVoiceRecordingStateType IN state)
{
	if (!curVoiceProcessing)
		return;

	curVoiceProcessing->SetVoiceRecording(state);
}

void UWRVoiceController::RequestGoogleSTT(FString pcmdata, int& outRequestCnt)
{
	GoogleSTTRequstCnt++;
	outRequestCnt = GoogleSTTRequstCnt;
	GoogleSTTRequstQueue.Enqueue(GoogleSTTRequstCnt);

	int32 LanguageType = WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType);
	FString tbl[EWRLanguageType::en::MAX + 1] = { L"ko-KR",L"ja-JP",L"zh",L"en-US" };

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	//JsonObject->SetStringField("uri", "gs://cloud-samples-tests/speech/brooklyn.flac");
	JsonObject->SetStringField("content", pcmdata);

	TSharedPtr<FJsonObject> JsonObject3 = MakeShareable(new FJsonObject);
	JsonObject3->SetStringField("encoding", "LINEAR16");
	JsonObject3->SetStringField("languageCode", tbl[LanguageType]);

	JsonObject3->SetStringField("sampleRateHertz", "16000");
	JsonObject3->SetStringField("audioChannelCount", "1");

	//JsonObject3->SetStringField("languageCode", "en-US");
	//JsonObject3->SetStringField("sampleRateHertz", "16000");
	//JsonObject3->SetStringField("encoding", "FLAC");
	//JsonObject3->SetStringField("languageCode", "en-US");
	//JsonObject3->SetStringField("sampleRateHertz", "16000");

	TSharedPtr<FJsonObject> JsonObject2 = MakeShareable(new FJsonObject);
	JsonObject2->SetObjectField("audio", JsonObject);
	JsonObject2->SetObjectField("config", JsonObject3);


	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject2.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> GoogleSTTRequest = Http->CreateRequest();

	GoogleSTTRequest->SetURL("https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCCc97xp4JTvRK9DL3EB8Op09aXKVT6Rc0");

	GoogleSTTRequest->SetVerb("POST");

	GoogleSTTRequest->SetHeader("Content-Type", TEXT("application/json"));
	//Request->SetHeader("Authorization", "Bearer ya29.c.KpQBwAc7C-nGfUm4p2CtCw49gqsNRI51iJFu-aFPoHSEF6bq4REMHUtVkkXdvPJaPRUeWvFPyjgcAoVNqBOgh9mODBB6KvqSt67qCGtarl5lLCaLOASlUI6fp6J7X5FiA69UQ-IgX7nbsOjAjGITVn6ZNing-uap58KI5KYupR-dXC99GfLeO5Y7Z4EfWJoDESOAnVOwXw");


	GoogleSTTRequest->OnProcessRequestComplete().BindUObject(this, &UWRVoiceController::OnResponseGoogleSTT);

	GoogleSTTRequest->SetContentAsString(OutputString);
	GoogleSTTRequest->ProcessRequest();

	FString tmp = FString::Printf(TEXT("##### Google STT Request #####"));
	WRInputSystemManager::Get()->DisplayAndSaveLog(tmp);

	FeedbackTimer = LoopTimer;
}

FString UWRVoiceController::GetGoogleSttTranscript(FHttpResponsePtr Response, bool bWasSuccessful)
{
	float feed_back_ftime = LoopTimer - FeedbackTimer;
	FString tmp = FString::Printf(TEXT("##### Google STT Response Time is %.2f #####"), feed_back_ftime);
	WRInputSystemManager::Get()->DisplayAndSaveLog(tmp);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedPtr<FJsonObject> JsonObject2;
	TSharedPtr<FJsonObject> JsonObject3;

	TArray<TSharedPtr<FJsonValue>> array;
	TArray<TSharedPtr<FJsonValue>> array2;

	FString transcript = "";

	if (!Response || bWasSuccessful == false)
	{
		return transcript;
	}

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		array = JsonObject->GetArrayField("results");
		if (array.Num() > 0)
		{
			TSharedPtr<FJsonValue> value = array[0];
			TSharedPtr<FJsonObject> json = value->AsObject();
			array2 = json->GetArrayField("alternatives");
			if (array2.Num() > 0)
			{
				value = array2[0];
				json = value->AsObject();
				transcript = json->GetStringField("transcript");
				FString confidence = json->GetStringField("confidence");

				FString tmp2 = FString::Printf(TEXT("transcript : %s  confidence : %s"), *transcript, *confidence);
				WRInputSystemManager::Get()->DisplayAndSaveLog(tmp2);
			}
		}
	}

	return transcript;
}

void UWRVoiceController::OnResponseGoogleSTT(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	int cnt = 0;
	GoogleSTTRequstQueue.Dequeue(cnt);

	if (curVoiceProcessing == nullptr)
	{
		return;
	}

	if (curVoiceProcessing->GetGoogleSTTRequstCnt() != cnt)
	{
		return;
	}

	FString transcript = GetGoogleSttTranscript(Response, bWasSuccessful);

	curVoiceProcessing->OnResponseGoogleSTT(transcript);
}