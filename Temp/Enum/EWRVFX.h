#pragma once

UENUM(BlueprintType)
enum class EWRVFX : uint8
{
	Entry = 0x0000,
	None = 0x0001,
	Component = 0x0002,
	NoneComponent = 0x0003 UMETA(DisplayName = "None | Component"),
	Async = 0x0004,
	NoneAsync = 0x0005 UMETA(DisplayName = "None | Async"),
	ComponentAsync = 0x0006 UMETA(DisplayName = "Component | Async"),
	NoneComponentAsync = 0x0007 UMETA(DisplayName = "None | Component | Async"),
	Max = 0x0008,
};

UENUM(BlueprintType)
enum class EWRVFXComponent : uint8
{
	Attach = 0,
	Move,
	Sound,
	Spawn,
	Laser,
	Collision,
	FX,
	Spline,
	Mesh,
	Max,
};

UENUM(BlueprintType)
enum class EWRVFXCollisionResult : uint8
{
	Destroy = 0,
	Attach,
	Stop,
	//Propagation,
	Max,
};

UENUM(BlueprintType)
enum class EWRVFXAttach : uint8
{
	None = 0,
	Spawner,
	Target,
	Parent,
	Max,
};

UENUM(BlueprintType)
enum class EWRVFXMove : uint8
{
	None = 0,
	Spawner,
	SpawnerToTarget,
	TraceTarget,
	Max,
};

UENUM(BlueprintType)
enum class EWRVFXActor : uint8
{
	None = 0,
	TOD,
	Target,
	MoveQuestTask, // ÀÌµ¿ Äù½ºÆ®
	UnderWater,
	SpeakerIcon,
	Max,
};

UENUM(BlueprintType)
enum class EWRVFXParameter : uint8
{
	Scalar = 0,
	Vector,
};

UENUM(BlueprintType)
enum class EWRVFXFade : uint8
{
	None = 0,
	In,
	Out,
	Max
};

UENUM(BlueprintType)
enum class EWRVFXComponentToAttach : uint8
{
	None = 0,
	Camera,
	Max
};