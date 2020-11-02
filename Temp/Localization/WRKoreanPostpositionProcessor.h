// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Tuple.h"

/**
 * 
 */

class WR_API WRKoreanPostpositionProcessor
{
private:
	static void MatchAndReplacePostposition(FString& TargetText, const FString& Pattern);
	WRKoreanPostpositionProcessor();
	~WRKoreanPostpositionProcessor();

public:
	static void ReplaceKoreanPostpositionToArrumentModifier(FString& TargetText);

	class PostpositionReplacer
	{
	private:
		static TMap<FString, TTuple<FString, FString>> InitializeMap();
		int MatchBegin;
		int MatchEnd;
		FText KoreanArgemuntModifier;

	public:
		PostpositionReplacer(int _MatchBegin, int _MatchEnd, FString _PreFix, FString _Postposition, FString _PostFix);
		void ReplacePostpositionToArgumentModifier(FString& TargetText);
		static TMap<FString, TTuple<FString, FString>> KoreanPostpositionMap;
	};
};
