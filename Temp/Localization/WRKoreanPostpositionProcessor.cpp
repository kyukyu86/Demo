// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRKoreanPostpositionProcessor.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/Regex.h"

WRKoreanPostpositionProcessor::WRKoreanPostpositionProcessor()
{
}

WRKoreanPostpositionProcessor::~WRKoreanPostpositionProcessor()
{
}

void WRKoreanPostpositionProcessor::ReplaceKoreanPostpositionToArrumentModifier(FString& TargetText)
{
	static const FString FirstPattern = TEXT("\\{([^}]*)([이으])([어에었로])([^{]*)\\}");
	//사용자가 이어, 이에, 이었, 으로 를 입력했을 경우
	MatchAndReplacePostposition(TargetText, FirstPattern);

	static const FString SecondPattern = TEXT("\\{([^}]*)([은는이가을를과와아야여예였로])([^{]*)\\}");
	// 그 이외의 조사를 입력했을 경우
	MatchAndReplacePostposition(TargetText, SecondPattern);
}

void WRKoreanPostpositionProcessor::MatchAndReplacePostposition(FString& TargetText, const FString& Pattern)
{
	FRegexPattern RegexPattern = FRegexPattern(Pattern);
	FRegexMatcher Matcher(RegexPattern, TargetText);

	TArray<WRKoreanPostpositionProcessor::PostpositionReplacer> ReplaceResult;

	while (Matcher.FindNext())
	{
		int MatchBegin = Matcher.GetMatchBeginning();
		int MatchEnd = Matcher.GetMatchEnding();

		FString PreFix = Matcher.GetCaptureGroup(1);
		FString PostPosition = Matcher.GetCaptureGroup(2);
		FString PostFix = Matcher.GetCaptureGroup(3);

		if ((PostPosition == TEXT("이") || PostPosition == TEXT("으")) &&
			(PostFix == TEXT("어") || PostFix == TEXT("에") || PostFix == TEXT("었") || PostFix == TEXT("로")))
		{
			PostPosition += PostFix;
			PostFix = Matcher.GetCaptureGroup(4);
		}

		ReplaceResult.Add(WRKoreanPostpositionProcessor::PostpositionReplacer(MatchBegin, MatchEnd, PreFix, PostPosition, PostFix));
	}

	for (int i = ReplaceResult.Num() - 1; i >= 0; i--)
	{
		ReplaceResult[i].ReplacePostpositionToArgumentModifier(TargetText);
	}
}

TMap<FString, TTuple<FString, FString>> WRKoreanPostpositionProcessor::PostpositionReplacer::KoreanPostpositionMap = WRKoreanPostpositionProcessor::PostpositionReplacer::InitializeMap();
TMap<FString, TTuple<FString, FString>> WRKoreanPostpositionProcessor::PostpositionReplacer::InitializeMap()
{
	TMap<FString, TTuple<FString, FString>> ReturnMap;

	ReturnMap.Add(FString(TEXT("은")), TTuple<FString, FString>(TEXT("은"), TEXT("는")));
	ReturnMap.Add(FString(TEXT("는")), TTuple<FString, FString>(TEXT("은"), TEXT("는")));

	ReturnMap.Add(FString(TEXT("이")), TTuple<FString, FString>(TEXT("이"), TEXT("가")));
	ReturnMap.Add(FString(TEXT("가")), TTuple<FString, FString>(TEXT("이"), TEXT("가")));

	ReturnMap.Add(FString(TEXT("을")), TTuple<FString, FString>(TEXT("을"), TEXT("를")));
	ReturnMap.Add(FString(TEXT("를")), TTuple<FString, FString>(TEXT("을"), TEXT("를")));

	ReturnMap.Add(FString(TEXT("과")), TTuple<FString, FString>(TEXT("과"), TEXT("와")));
	ReturnMap.Add(FString(TEXT("와")), TTuple<FString, FString>(TEXT("과"), TEXT("와")));

	ReturnMap.Add(FString(TEXT("아")), TTuple<FString, FString>(TEXT("아"), TEXT("야")));
	ReturnMap.Add(FString(TEXT("야")), TTuple<FString, FString>(TEXT("아"), TEXT("야")));

	ReturnMap.Add(FString(TEXT("이어")), TTuple<FString, FString>(TEXT("이어"), TEXT("여")));
	ReturnMap.Add(FString(TEXT("여")), TTuple<FString, FString>(TEXT("이어"), TEXT("여")));

	ReturnMap.Add(FString(TEXT("이에")), TTuple<FString, FString>(TEXT("이에"), TEXT("예")));
	ReturnMap.Add(FString(TEXT("예")), TTuple<FString, FString>(TEXT("이에"), TEXT("예")));

	ReturnMap.Add(FString(TEXT("이었")), TTuple<FString, FString>(TEXT("이었"), TEXT("였")));
	ReturnMap.Add(FString(TEXT("였")), TTuple<FString, FString>(TEXT("이었"), TEXT("였")));

	ReturnMap.Add(FString(TEXT("으로")), TTuple<FString, FString>(TEXT("으로"), TEXT("로")));
	ReturnMap.Add(FString(TEXT("로")), TTuple<FString, FString>(TEXT("으로"), TEXT("로")));

	return ReturnMap;
}

WRKoreanPostpositionProcessor::PostpositionReplacer::PostpositionReplacer(int _MatchBegin, int _MatchEnd, FString PreFix, FString Postposition, FString PostFix)
	:MatchBegin(_MatchBegin), MatchEnd(_MatchEnd)
{
	TTuple<FString, FString> TargetPostPostiionTuple = KoreanPostpositionMap[Postposition];
	FFormatNamedArguments Args;
	Args.Add("preFix", FText::FromString(PreFix));
	Args.Add("postFix", FText::FromString(PostFix));
	Args.Add("0", FText::FromString(TargetPostPostiionTuple.Get<0>()));
	Args.Add("1", FText::FromString(TargetPostPostiionTuple.Get<1>()));

	static FString FormatFString = TEXT("hpp({preFix}{0}{postFix}, {preFix}{1}{postFix})");
	FTextFormat FormatString = FTextFormat::FromString(FormatFString);
	KoreanArgemuntModifier = FText::Format(FormatString, Args);
}

void WRKoreanPostpositionProcessor::PostpositionReplacer::ReplacePostpositionToArgumentModifier(FString& TargetText)
{
	TargetText.RemoveAt(MatchBegin, MatchEnd - MatchBegin);
	TargetText.InsertAt(MatchBegin, KoreanArgemuntModifier.ToString());
	TargetText.InsertAt(MatchBegin, TEXT("|"));
}
