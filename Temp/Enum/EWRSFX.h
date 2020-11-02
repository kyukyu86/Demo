#pragma once

UENUM(BlueprintType)
enum class EWRSFXSoundClass : uint8
{
	None = 0,
	SFX,
	BGM,
	Master,
	Max,
};

UENUM(BlueprintType)
enum class EWRSoundClassKind : uint8
{
	None = 0,
	SFX,
	BGM,
	Master,
	Max,
};

UENUM(BlueprintType)
enum class EWRSFXSpawnType : uint8
{
	None = 0,
	AtLocation,
	Attach,
	Sound2D,
	BGM,
	Max,
};

UENUM(BlueprintType)
enum class EWRSFX : uint8
{
	Entry = 0x0000,
	None = 0x0001,
	Option = 0x0002,
};

UENUM(BlueprintType)
enum class EWRSFXActor : uint8
{
	None = 0,
	AccDamage,
	TOD,
	Engine,
	Max,
};

UENUM(BlueprintType)
enum class EWRShoesMaterialType : uint8
{
	None = 0,
	Bare,
	Leather,
	Steel,
	Max,
};

//namespace EWRShoesMaterialKind
//{
//	UENUM(BlueprintType)
//		enum class en : uint8
//	{
//		None = 0,
//		Bare,
//		Leather,
//		Steel,
//		Max,
//	};
//
//	static FString GetStr(const EWRShoesMaterialKind::en IN ShoesMaterialKind)
//	{
//		switch (ShoesMaterialKind)
//		{
//		case EWRShoesMaterialKind::en::Bare:				return TEXT("Bare");
//		case EWRShoesMaterialKind::en::Leather:			    return TEXT("Leather");
//		case EWRShoesMaterialKind::en::Steel:				return TEXT("Steel");
//		default:								        return TEXT("");
//		}
//	}
//}