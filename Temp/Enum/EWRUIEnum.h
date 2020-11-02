// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#define DEF_COMPASS_WIDTH 2190
#define DEF_DISPLAY_COMPASS_DEGREE 120

#define MULTI_CREATABLE_WIDGET_START 100

// Defines all bitwise operators for enum classes so it can be (mostly) used as a regular flags enum
#define WRENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline CONSTEXPR Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline bool  operator& (Enum  Lhs, Enum Rhs) { return ((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline CONSTEXPR Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline CONSTEXPR bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline CONSTEXPR Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }
	






UENUM(BlueprintType)
enum class EWRUIPanelKind : uint8
{
	None,

	Main,
	Title,
	Loading,
	Menu,
	Status,
	Dialogue,
	Cheat,
	MonitorInLab,
	Tutorial,
	TutorialMini,
	QuestBoard,
	MultiTargetUI,
	CrossHair,
	DisplaySkillCoolTime,
	Parrying,
	Popup,
	SocialMenu,
	CharacterCreate,
	CharacterSelect,
	ClimbHandGuide,
	SelectDialogue,
	DanceMode,
	DanceModeBoard,
	CostumeChange,
	EquipmentChange,
	CustomText,
	Drink,
	DrinkAlert,
	BattleResultBoard,
	Revive,
	DungeonTimer,
	Notice,
	StyleNotice,
	InputIcon,
	DefaultVoiceCommand,
	Preview,
	Inventory,
	RecommendedVoiceCommand,
	HmdGuide,
	TargetLocationGuide,
	BoardingRequest,
	ChatRecordingState,
	ChatRecordingGaugeIcon,
	VideoChatUserList,
	VideoChatScreen,
	VideoChatScreen_NoCyber,
	SpawnSimulationGuide,
	SpawnLoading,
	EquipNavigator,
	NewCharacterCreate,			// 8월 빌드용 캐릭터 생성
	SubCharacterCreate,			// 캐릭터 생성 우측 상단 패널
	SubColorSelect,				// 캐릭터 생성용 Color 패널 하단 패널
	MoveSimulationGuide,
	HouseSelectPopup,

	IOPopup = MULTI_CREATABLE_WIDGET_START,
	SpeakDialogueIcon,		// 대화중 상태 아이콘
	AvailableDialogueIcon,	// 대화가능 상태 아이콘
	TargetInfo,
	TargetUI,
	CompassMark,
	ChatBubble,
	DrinkIcon,
	TardisTimer,
	VoiceChatSpeakIcon,
	Debug,
	TargetLocationSearched,
	RepeatSecretDialog,
	
	MAX_MULTI_CREATABLE_WIDGET_NUMBER,Q
};

static bool IsMultiCreatableUI(const EWRUIPanelKind IN InUIPanelKind)
{
 	if (MULTI_CREATABLE_WIDGET_START <= (uint8)InUIPanelKind && InUIPanelKind < EWRUIPanelKind::MAX_MULTI_CREATABLE_WIDGET_NUMBER)
 	{
 		return true;
 	}
 
 	return false;
}

UENUM(BlueprintType)
enum class EWRUISlotKind : uint8
{
	None = 0,
	Test,
	QuestBoard,
	QuestBoardList,
	QuestBoardList_TaskList,
	CompassMark,
	ScoreBoardList,
	InventoryItemInfo,
	InventoryPreview,
	FocusTalk,
	QuestMarkForTargetLocationGuide,
	PopupQuantity,
	QuantitySelector,
	Social_CircleSelectorElement,
};



 // [ 2020-05-22 : vuvle ] UI 관련 코드 정리로 인해 삭제. 무조건 3D 를 사용하여야 하기때문에 삭제시킨다.
/* enum EWRUISpaceType : uint8
{
	Space2D = 0,
	Space3D,
};
*/

// 구형
// namespace EWRUIWidgetCreationBitFlag
// {
// 	enum en : uint16
// 	{
// 		None											= 0x0000,
// 		Billboard										= 0x0001,
// 		ManualRedraw									= 0x0002,
// 		AttachToCharacter								= 0x0004,	// 고정된 트랜스폼으로 케릭터를 따라다님 (Fixed Transform)
// 		AttachToCharacterAndRotateToMe					= 0x0008,	// 케릭터를 따라다니며 Parent와의 Offset을 유지한 채 내 방향으로 회전함						X 
// 		AttachToCharacterAndRotateAxisZToMe				= 0x0010,	// 케릭터를 따라다니며 Parent와의 OffsetLocation을 유지한 채 내 방향으로 Z축 회전함			X
// 		AttachToCamera									= 0x0020,	// 상하 안따라옴
// 		AttachToCameraAll								= 0x0040,	// 모두 따라옴
// 		IgnoreDepth										= 0x0080,	// Z버퍼 무시 (맨위에 출력되게함, 3D위에 출력)
// 		SmoothMoving									= 0x0100,	// 카메라 따라다니는 UI가 카메라 방향 이동시 천천히 따라오도록 하는 기능					X
// 		WidgetInteractionable							= 0x0200,	// 위젯 인터렉션이 유효한지																	
// 		CreateStereoLayer								= 0x0400,   // StereoLayer 를 생성할건지에 대한 Flag
// 	};
// }

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWRUIWidgetCreationBitFlag : int32
{
	None,
	CreateStereoLayer					= 0x0001,
	CreateWidgetComponent				= 0x0002,

	UseFaceLock							= 0x0004,
	UserWorldLock						= 0x0008,
	UserTrackerLock						= 0x0010,

	Billboard							= 0x0020,
	ManualRedraw						= 0x0040,
	AttachToCharacter					= 0x0080,
	AttachToCamera						= 0x0100,
	AttachToCameraAll					= 0x0200,
	IgnoreDepth							= 0x0400,
	WidgetInteractionable				= 0x0800,

	SmoothMoving						= 0x1000,
	AttachToCharacterAndRotateToMe		= 0x2000,
	AttachToCharacterAndRotateAxisZToMe	= 0x4000,
};
WRENUM_CLASS_FLAGS(EWRUIWidgetCreationBitFlag);

UENUM(BlueprintType)
enum class EWRDialogueUIType : uint8
{
	None,
	CameraAttach,
	Fixed3DPosition,
};

UENUM(BlueprintType)
enum class EWRMenuCategory : uint8
{
	None,
	Main,
	Tutorial,
	Option,
	Cheat,
	Inven,
};

UENUM(BlueprintType)
enum class EWRInventoryMainSwitcher : uint8
{
	Category,
	Contents,
};

UENUM(BlueprintType)
enum class EWRInventorySlotGroup : uint8
{
	None,
	EquipList,
	Storage,
};

enum class EWRInventoryMiddleType : uint8
{
	EquipList,
	InnerCategory,
};

enum class EWRInventoryVehicleCategory : uint8
{
	Car,
};

enum class EWRInventoryConsumeCategory : uint8
{
	Material,
	Interior,
	Emote,
	Placement,
};

enum class EWRInventorySlotAction : uint8
{
	None,

	RegisterToSelectedRegistrationSlot,
	RegisterWhenStorageSelecting,

	UnRegister,

	Equip,
	UnEquip,

	SpawnVehicle,
	UnSpawnVehicle,

	SpawnInterior,
	UnSpawnInterior,		// 없음

	SpawnPlacement,
	UnSpawnPlacement,

	Use,

	// swap
	GotoStorage,
	GotoEquipList,

	// exception
	Exception_AlreadySameSlotRegistered,
	Exception_AlreadyOtherSlotRegistered,
	Exception_AlreadySameSlotEquipped,
	Exception_AlreadyOtherSlotEquipped,
};

enum class EPopupButtonFlag : uint8
{
	None = 0,
	OK = 1 << 0,
	OK_CANCEL = 1 << 1,
};
ENUM_CLASS_FLAGS(EPopupButtonFlag);

enum class EPopupOptionFlag : uint8
{
	None = 0,
	EditableText = 1 << 0,
	ProgressBar = 1 << 1,
	Slider = 1 << 2,
	Timer = 1 << 3,
	ForceOK = 1 << 4,
	Quantity = 1 << 5,
};
ENUM_CLASS_FLAGS(EPopupOptionFlag);

UENUM(BlueprintType)
enum class EWRInventorySoundType : uint8
{
	None,
	OpenInventory,
	CloseInventory,
	OpenContents,
	OpenItemInfo,
	CloseItemInfo,
	Equip,
	Unequip,
	Register,
	Unregister,
	Discard,
	FocusMoveSlot,
	FocusMoveMainCategory,
	FocusMoveInnerCategory,
	Circle,
	Cross,
	Alert,
	SelectCategory,
};