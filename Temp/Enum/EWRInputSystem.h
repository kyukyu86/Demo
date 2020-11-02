#pragma once

namespace EWRHMDTick
{
	enum Flag : uint8
	{
		None = 0x00,

		Targetting	= 0x01,
		HMDInfo		= 0x02,

		All = 0xFF,
	};
}

enum EWRPadMotionType : uint8
{
	PULL,
	PUSH,
	MAX,
};

UENUM(BlueprintType)
enum class EWRVoiceWordType : uint8
{
	NONE,
	EmptyVoice,
	Unknown,
	WakeUp,			
	Heal,
	QuestInfo,// Quest_Info
	Tutorial,
	Teleport,
	Summon,
	Option,
	Trigger,
	Dialogue,
	ChatRecord,
	StopVoiceRecord,
	Inventory,
	VideoChat,
	VideoChatShortcutNumber,
	VideoChatNickname,
	TestWordTypeA,
	TestWordTypeB,
	TestWordTypeC,
	TestWordTypeD,
	TestWordTypeE,
	TestWordTypeF,
	TestWordTypeG,
	TestWordTypeH,
	TestWordTypeI,
	TestWordTypeJ,
};

// À½¼ºÀ» »ç¿ëÇÑ ÄÁÅÙÃ÷
enum class EWRVoiceContentType : uint8
{
	NONE,
	Default,
	Menu,
	SocialMenu,
	SelectDialogue,
	VideoChatUserList,
	VideoChatScreen,
};

enum EWRVoiceRecordingStateType : uint8
{
	NONE,
	DELAY,
	RECORDING,
	VOICESENDING,
};

enum class EWRVoiceRecordingUseType : uint8
{
	NONE,
	COMMAND,
	TRANSLATION,
};