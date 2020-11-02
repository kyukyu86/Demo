// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#pragma once

/**
 * 
 */
class WR_API WRTriggerUtility
{
public:
	WRTriggerUtility();
	~WRTriggerUtility();

	static bool IsIntersected_Sphere(const FVector& ActorPosition, const float ActorRadiusRange, const FVector& TriggerPosition, const float TriggerRange );
	static bool IsIntersected_Box(const FVector& ActorPosition, const float ActorRadiusRange, const FVector& TriggerPosition, const TArray<FVector>& vVectors, const float TriggerHeight);

	static bool InitFromString(FString& InString, FVector& InVector);
	static bool InitFromString(FString& InString, FRotator& InRotator);
	static bool InitFromString(FString& InString, FTransform& InTransform);

	static bool GetCommaLeftValueAndRightString(FString& String, float* Value);
	static const FVector GetColliderRandomPos(const FVector& Position, bool bRamdom = false, float fScale = 1.0f, int8 nMode = 0, float Angel = 0.f, float SizeX = 1.f);
};
