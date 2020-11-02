// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentVoiceChatAudio.h"

#include "Sound/SoundSubmix.h"
#include "Manager/WRPathManager.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include <AudioDeviceManager.h>
#include "WRActorComponentVFX.h"
#include "Define/WRPathDefine.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCharacterManager.h"
#include "Table/WRPlayerTable.h"
#include "Table/Base/WRTableManager.h"
#include "Utility/WRUIUtility.h"
#include "Utility/WRActorUtility.h"
#include "Sound/SoundAttenuation.h"
#include "Engine/Engine.h"

void UWRActorComponentVoiceChatAudio::OnCreateComponent()
{
	Super::OnCreateComponent();

	Init();
}

void UWRActorComponentVoiceChatAudio::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	WRActorUtility::ClearTimer(PlayVoiceCaptureTimer);

	if (SoundAttenuation)
	{
		SoundAttenuation->RemoveFromRoot();
		SoundAttenuation = nullptr;
		//SoundAttenuation->ConditionalBeginDestroy();
	}
}

void UWRActorComponentVoiceChatAudio::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Tick_CheckToSpeakerIcon(DeltaTime);
	FlowCoolTimeForSpeakAni += DeltaTime;
}

void UWRActorComponentVoiceChatAudio::Init()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter)
	{
		ChatAudioComponet = NewObject<UAudioComponent>(OwnerCharacter);
		ChatAudioComponet->RegisterComponent();
		OwnerCharacter->AddOwnedComponent(ChatAudioComponet);
		OwnerCharacter->AddInstanceComponent(ChatAudioComponet);
		ChatAudioComponet->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);


		ChatAudioComponet->bAutoActivate = true;
		ChatAudioComponet->bAlwaysPlay = true;
		ChatAudioComponet->VolumeMultiplier = 1.f;

		//ChatAudioComponet->OnAudioFinishedNative.AddUObject(this, &UWRActorComponentVoiceChatAudio::ChatAudioFinished);
		//ChatAudioComponet->OnAudioPlaybackPercent.AddDynamic(this, &UWRActorComponentVoiceChatAudio::DelegeteCallback1);

		if (SoundAttenuation == nullptr)
		{
			SoundAttenuation = LoadObject<USoundAttenuation>(nullptr, TEXT("/Game/Asset/Sound/VoiceChatAttenuation.VoiceChatAttenuation"));
			if (SoundAttenuation)
				SoundAttenuation->AddToRoot();
		}

		ChatSoundWave = NewObject<USoundWaveProcedural>();
		ChatSoundWave->SetSampleRate(16000);
		ChatSoundWave->NumChannels = 1;
		ChatSoundWave->SoundGroup = SOUNDGROUP_Voice;
		//ChatSoundWave->Duration = INDEFINITELY_LOOPING_DURATION;
		ChatSoundWave->bLooping = false;
		ChatSoundWave->bProcedural = true;

#if PLATFORM_PS4
		ChatSoundWave->Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::PS4_Voice_Default_Volume);
#else //PLATFORM_PS4
		ChatSoundWave->Volume = WRConsoleManager::Get()->GetInt(EWRConsole::VoiceChat_DefaultVolume);
#endif //PLATFORM_PS4

		//ChatSoundWave->DecompressionType = EDecompressionType::DTYPE_Procedural;
		ChatSoundWave->AttenuationSettings = SoundAttenuation;

		ChatAudioComponet->SetSound(ChatSoundWave);

#if PLATFORM_PS4
#else //PLATFORM_PS4
		//float loopTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ChatAudioPlayLoopTime);
		//WRActorUtility::SetTimer(PlayVoiceCaptureTimer, FTimerDelegate::CreateUObject(this, &UWRActorComponentVoiceChatAudio::PlayVoiceAudio), loopTime, true);
#endif //PLATFORM_PS4
	}
}

void UWRActorComponentVoiceChatAudio::QueueChatAudio(TArray<uint8>& voiceData, const EWRAudioType InAudioType /* = EWRAudioType::None */, const bool IN InForce /* = false */)
{
	if (ChatSoundWave == nullptr || ChatAudioComponet == nullptr)
		return;

#if PLATFORM_PS4
	float Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::PS4_Voice_Default_Volume);
#else //PLATFORM_PS4
	float Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::VoiceChat_DefaultVolume);
#endif //PLATFORM_PS4
	float VolumeMultiplier = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_VoiceChatVolumeMultiplier);
	float PitchMultiplier = -1.f;

	switch (InAudioType)
	{
	case EWRAudioType::FocusChat:
	{
		Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_FocusTalkVolume);
		PlayToSpeakAnimation();	// [ 2020-8-11 : kyu ] StandMic용 애니가 따로 있음
	}
	break;
	case EWRAudioType::StandMic:
	{
		Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_MicVolume);
		VolumeMultiplier = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_MicVolumeMultiplier);
//		PitchMultiplier = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_MicPitchMultiplier);
	}
	break;
	case EWRAudioType::VideoChat:
	{
		Volume = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_VideoChatVolume);
		//VolumeMultiplier = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_VideoChatMultiplier);
	}
	break;
	default:
	{
		PlayToSpeakAnimation();	// [ 2020-8-11 : kyu ] StandMic용 애니가 따로 있음
	}
	break;
	}

	SetVolume(Volume);
	SetVolumeMultiplier(VolumeMultiplier);
	SetPitchMultiplier(PitchMultiplier);	


	// [ 2020-8-10 : kyu ] Submix 추가
	// [ 2020-8-21 : kyu ] Attenutate 개별적용
	SetupEffects(InAudioType, InForce);


	ChatSoundWave->QueueAudio(voiceData.GetData(), voiceData.Num());

#if PLATFORM_PS4
	PlayVoiceAudio();
#else
	PlayVoiceAudio();
#endif //PLATFORM_PS4

	// 연출
	//SpawnSpeakerIcon();
}

void UWRActorComponentVoiceChatAudio::PlayVoiceAudio()
{
	if (ChatAudioComponet == nullptr)
	{
		return;
	}

	if (!ChatAudioComponet->IsPlaying())
	{
		ChatAudioComponet->Play();
	}
}

void UWRActorComponentVoiceChatAudio::ChatAudioFinished(UAudioComponent* IN InAudioComponent)
{
	if (ChatSoundWave == nullptr || ChatAudioComponet == nullptr)
		return;

	//ChatSoundWave->ResetAudio();
}

void UWRActorComponentVoiceChatAudio::DelegeteCallback1(const class USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	FString WavFileName = FString::Printf(TEXT("UWRActorComponentVoiceChatAudio::DelegeteCallback - PlaybackPercent [%f]"), PlaybackPercent);
	GEngine->AddOnScreenDebugMessage(count++, 4.0f, FColor::Green, WavFileName);
}

void UWRActorComponentVoiceChatAudio::SetVolume(float IN InValue)
{
	ChatSoundWave->Volume = InValue;
}

void UWRActorComponentVoiceChatAudio::SetVolumeMultiplier(float IN InValue)
{
	if (InValue < 0.0f)
	{
		InValue = ChatSoundWave->GetVolumeMultiplier();
	}
	ChatAudioComponet->SetVolumeMultiplier(InValue);
}

void UWRActorComponentVoiceChatAudio::SetPitchMultiplier(float IN InValue)
{
	if (InValue < 0.0f)
	{
		InValue = ChatSoundWave->GetPitchMultiplier();
	}
	ChatAudioComponet->PitchMultiplier = InValue;
}

// void UWRActorComponentVoiceChatAudio::SetAttenuate(const bool IN InSet)
// {
// 	if (ChatSoundWave->AttenuationSettings)
// 	{
// 		if (ChatAudioComponet && ChatSoundWave->AttenuationSettings->Attenuation.bAttenuate != InSet)
// 		{
// 			ChatSoundWave->AttenuationSettings->Attenuation.bAttenuate = InSet;
// 			ChatAudioComponet->AdjustAttenuation(ChatSoundWave->AttenuationSettings->Attenuation);
// 		}
// 	}
// }

void UWRActorComponentVoiceChatAudio::Tick_CheckToSpeakerIcon(const float IN InDeltaTime)
{
	UWRActorComponentVFX* ActorComponentVFX = GetOwnerCharacter()->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX == nullptr)
		return;

	if (ActorComponentVFX->Contains(EWRVFXActor::SpeakerIcon) == false)
		return;

	FlowTimeForFX += InDeltaTime;
	if (FlowTimeForFX > WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_Icon_Time))
	{
		ActorComponentVFX->RemoveVFX(EWRVFXActor::SpeakerIcon);
	}
}

void UWRActorComponentVoiceChatAudio::SpawnSpeakerIcon()
{
	// 연출
	UWRActorComponentVFX* ActorComponentVFX = GetOwnerCharacter()->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX)
	{
		if (ActorComponentVFX->Contains(EWRVFXActor::SpeakerIcon) == false)
		{
			ActorComponentVFX->AddVFX(EWRVFXActor::SpeakerIcon, EFFECT_PATH_4_SPEAKER_ICON, GetOwnerCharacter(), GetOwnerCharacter());
			FlowTimeForFX = 0.f;
		}
	}
}

void UWRActorComponentVoiceChatAudio::PlayToSpeakAnimation()
{
	/*if (FlowCoolTimeForSpeakAni < WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SpeakAni_CoolTime))
		return;*/

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	if (OwnerCharacter->IsMe())
		return;

	if (OwnerCharacter->GetCurrentFSMType() != EWRFSM::Idle)
		return;

	if (OwnerCharacter->GetCharacterType() != EWRCharacterType::Player)
		return;

	const FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return;

	if (PlayerTable->VoiceChatAnis.Num() <= 0)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::VoiceChatMotion;
	//FSMInfo.JumpToSectionName = InAniID.ToString();

	int32 Count = 0;
	for (int32 i = 0; i < PlayerTable->VoiceChatAnis.Num(); ++i)
	{
		if(PlayerTable->VoiceChatAnis[i].IsNone())
			break;

		++Count;
	}

	int32 SectionNameIndex = FMath::Rand() % Count;
	FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(PlayerTable->VoiceChatAnis[SectionNameIndex]);

	OwnerCharacter->DetermineFSMSignal(FSMInfo);
	//FlowCoolTimeForSpeakAni = 0.f;
}

void UWRActorComponentVoiceChatAudio::SetupEffects(const EWRAudioType IN InAudioType, const bool IN InForce /*= false*/)
{
	EWRAudioType PrevAudioType = AudioType;
	if (InForce == false && PrevAudioType == InAudioType)
		return;

	AudioType = InAudioType;

	SetupSubmix(InAudioType);
	SetupAttenuate(InAudioType);

	// Replay
	ChatAudioComponet->Play();
}

void UWRActorComponentVoiceChatAudio::SetupSubmix(const EWRAudioType IN InAudioType)
{
	auto ReleaseSoundSubmixObjectLambda = [&]()
	{
		if (ChatSoundWave->SoundSubmixObject)
		{
			ChatSoundWave->SoundSubmixObject->RemoveFromRoot();
			ChatSoundWave->SoundSubmixObject = nullptr;

			ChatAudioComponet->bReverb = false;	// reverb off
		}
	};

	switch (InAudioType)
	{
	case EWRAudioType::Normal:
	{
		ReleaseSoundSubmixObjectLambda();
	}
	break;

	case EWRAudioType::StandMic:
	{
		ReleaseSoundSubmixObjectLambda();

		ChatAudioComponet->bReverb = true;	// reverb on

		ChatSoundWave->SoundSubmixObject = LoadObject<USoundSubmix>(nullptr, TEXT("/Game/Asset/Sound/Submix/StandMicSubmix.StandMicSubmix"));
		if (ChatSoundWave->SoundSubmixObject == nullptr)
		{
			ensureMsgf(false, TEXT("Sound Submix Load Failure"));
			return;
		}
		ChatSoundWave->SoundSubmixObject->AddToRoot();
	}
	break;

	case EWRAudioType::FocusChat:
	{
		ReleaseSoundSubmixObjectLambda();
	}
	break;
	}
}

void UWRActorComponentVoiceChatAudio::SetupAttenuate(const EWRAudioType IN InAudioType)
{
	if (ChatSoundWave->AttenuationSettings == nullptr)
		return;

	if (ChatAudioComponet == nullptr)
		return;

	bool bAttenuate = false;
	float FalloffDistance = 1000.f;
	float AttenuateShapeDistance = 300.f;

	switch (InAudioType)
	{
	case EWRAudioType::Normal:
	{
		bAttenuate = true;
		FalloffDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateDistNormal);
		AttenuateShapeDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateShapeNormal);
	}
	break;

	case EWRAudioType::FocusChat:
	{
		bAttenuate = false;
		FalloffDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateDistFocusTalk);
		AttenuateShapeDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateShapeFocusTalk);
	}
	break;

	case EWRAudioType::StandMic:
	{
		bAttenuate = true;
		FalloffDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateDistStandMic);
		AttenuateShapeDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_AttenuateShapeStandMic);
	}
	break;
	}

	ChatSoundWave->AttenuationSettings->Attenuation.bAttenuate = bAttenuate;
	ChatSoundWave->AttenuationSettings->Attenuation.FalloffDistance = FalloffDistance;
	ChatSoundWave->AttenuationSettings->Attenuation.AttenuationShapeExtents.X = AttenuateShapeDistance;
	ChatAudioComponet->AdjustAttenuation(ChatSoundWave->AttenuationSettings->Attenuation);
}
