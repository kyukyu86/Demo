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
	NewCharacterCreate,			// 8�� ����� ĳ���� ����
	SubCharacterCreate,			// ĳ���� ���� ���� ��� �г�
	SubColorSelect,				// ĳ���� ������ Color �г� �ϴ� �г�
	MoveSimulationGuide,
	HouseSelectPopup,

	IOPopup = MULTI_CREATABLE_WIDGET_START,
	SpeakDialogueIcon,		// ��ȭ�� ���� ������
	AvailableDialogueIcon,	// ��ȭ���� ���� ������
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



 // [ 2020-05-22 : vuvle ] UI ���� �ڵ� ������ ���� ����. ������ 3D �� ����Ͽ��� �ϱ⶧���� ������Ų��.
/* enum EWRUISpaceType : uint8
{
	Space2D = 0,
	Space3D,
};
*/

// ����
// namespace EWRUIWidgetCreationBitFlag
// {
// 	enum en : uint16
// 	{
// 		None											= 0x0000,
// 		Billboard										= 0x0001,
// 		ManualRedraw									= 0x0002,
// 		AttachToCharacter								= 0x0004,	// ������ Ʈ���������� �ɸ��͸� ����ٴ� (Fixed Transform)
// 		AttachToCharacterAndRotateToMe					= 0x0008,	// �ɸ��͸� ����ٴϸ� Parent���� Offset�� ������ ä �� �������� ȸ����						X 
// 		AttachToCharacterAndRotateAxisZToMe				= 0x0010,	// �ɸ��͸� ����ٴϸ� Parent���� OffsetLocation�� ������ ä �� �������� Z�� ȸ����			X
// 		AttachToCamera									= 0x0020,	// ���� �ȵ����
// 		AttachToCameraAll								= 0x0040,	// ��� �����
// 		IgnoreDepth										= 0x0080,	// Z���� ���� (������ ��µǰ���, 3D���� ���)
// 		SmoothMoving									= 0x0100,	// ī�޶� ����ٴϴ� UI�� ī�޶� ���� �̵��� õõ�� ��������� �ϴ� ���					X
// 		WidgetInteractionable							= 0x0200,	// ���� ���ͷ����� ��ȿ����																	
// 		CreateStereoLayer								= 0x0400,   // StereoLayer �� �����Ұ����� ���� Flag
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
	UnSpawnInterior,		// ����

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