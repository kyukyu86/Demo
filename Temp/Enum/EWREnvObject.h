#pragma once
#include "ObjectMacros.h"

UENUM(BlueprintType)
enum class EWREnvSplineInfo : uint8
{
	None,
	PhysicsMesh,
	Body,
	ForwardLane,
	ReverseLane,
};

UENUM(BlueprintType)
enum class EWRDestructibleHitPositionType : uint8
{
	None = 0,
	Static,
	Dynamic,
	Max,
};

UENUM(BlueprintType)
enum class EWRDestructibleImpulseType : uint8
{
	None = 0,
	Radial,
	Direction,
	Max,
};


UENUM(BlueprintType)
enum class EWRSecretDialogPortalStateType : uint8
{
	None = 0,
	Seed,
	WillOpen,
	Opened,
	WillClose,
	Max,
};
