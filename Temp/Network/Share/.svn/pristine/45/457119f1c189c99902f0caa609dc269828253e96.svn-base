//******************************************************************************
//	FileName: 	Define_Language.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	언어 관련
//******************************************************************************



#pragma once


//-----------------------------------------------------
//	이름 생성 규칙 (캐릭터, 펠로우, 길드, 인사말)
//-----------------------------------------------------
#define MIN_NAME_LEN				2
#define MAX_NAME_LEN				10
#define MAX_GREETING_COMMNET_LEN	20


//-----------------------------------------------------
// 국가별 코드
//-----------------------------------------------------
struct EWRLanguageType
{
	enum en : uint8
	{
		Korean,
		Japanese,
		Chinese,
		English,
		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Korean, L"Korean");
		InsertMap(Japanese, L"Japanese");
		InsertMap(Chinese, L"Chinese");
		InsertMap(English, L"English");
	}

	// 국가별 코드와 맞는가?
	static bool IsMatchLanguage(wchar_t code, EWRLanguageType::en InCharCode)
	{
		switch (InCharCode)
		{
		case EWRLanguageType::Korean:	{	if (IsKorean(code))		return true;	}	break;
		case EWRLanguageType::Japanese: {	if (IsJapanese(code))	return true;	}	break;
		case EWRLanguageType::Chinese:	{	if (IsChinease(code))	return true;	}	break;
		case EWRLanguageType::MAX:		{							return true;	}	break;
		}

		return false;
	}

	static bool GetGoogleLanguageStr(EWRLanguageType::en eLangType, std::wstring& strLang)
	{
		switch (eLangType)
		{
		case EWRLanguageType::Korean:	strLang = L"ko";	break;
		case EWRLanguageType::Japanese:	strLang = L"ja";	break;
		case EWRLanguageType::Chinese:	strLang = L"zh";	break;
		case EWRLanguageType::English:	strLang = L"en";	break;
		default:
			return false;
		}

		return true;
	}
	// 한글인가?
	static bool IsKorean(wchar_t code)
	{
		// Hangul Jamo			0x1100 ~0x11FF (10진수 : 4352 ~4607)
		if (code >= 0x1100 && code <= 0x11FF)
			return true;
		// Hangul Syllables		0xAC00 ~0xD7AF (10진수 : 44032 ~55215)
		else if (code >= 0xAC00 && code <= 0xD7AF)
			return true;
		return false;
	}

	// 일본어인가?
	static bool IsJapanese(wchar_t code)
	{
		// Hiragana				0x3040 ~0x309F (10진수 : 12368 ~12447)
		if (code >= 0x3040 && code <= 0x309F)
			return true;
		// Katakana				0x30A0 ~0x30FF (10진수 : 12448 ~12543)
		else if (code >= 0x30A0 && code <= 0x30FF)
			return true;
		// CJK Ideograph			0x4E00 ~0x9FFF (10진수 : 19968 ~40959)
		else if (code >= 0x4E00 && code <= 0x9FFF)
			return true;
		return false;
	}

	// 중국어인가?
	static bool IsChinease(wchar_t code)
	{
		// CJK Ideograph			0x4E00 ~0x9FFF (10진수 : 19968 ~40959)
		if (code >= 0x4E00 && code <= 0x9FFF)
			return true;
		return false;
	}
};


//-----------------------------------------------------
//	이름 검증 결과 
//-----------------------------------------------------
struct EWRCheckNameResult
{
	enum en
	{
		Success,				// 성공
		FailedBlank,			// 공백문자포함
		FailedOddWord,			// 허용되지 않는 문자(특수문자 등)
		FailedAllNumber,		// 모든 문자가 숫자
		FailedInvalidLength,	// 허용되지 않는 문자길이
		FailedProhibitWord,	// 금지어가 포함(일단은 클라에서만 사용중. 서버에서 이 Enum값 사용 시 괄호 내 내용은 지워주세요)
		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Success,				L"Success");
		InsertMap(FailedBlank,			L"FailedBlank");
		InsertMap(FailedOddWord,		L"FailedOddWord");
		InsertMap(FailedAllNumber,		L"FailedAllNumber");
		InsertMap(FailedInvalidLength,	L"FailedInvalidLength");
		InsertMap(FailedProhibitWord, L"FailedProhibitWord");
	}

	// 언어에 따른 체크 적용
	static EWRCheckNameResult::en CheckNameByLanguage(
		const wchar_t * strName,							// 입력된 문자열

		EWRLanguageType::en enCKLanguage = EWRLanguageType::MAX,	// 국가별 문자만 허락 여부
		bool bEngUpper = true,								// 영어 대문자 가능 여부
		bool bEngLower = true,								// 영어 소문자 가능 여부
		bool bNumber = true,								// 숫자 가능 여부
		bool bBlank = true,									// 빈칸 여부 체크
		unsigned int nMinLength = MIN_NAME_LEN,				// 글자 길이 최소값
		unsigned int nMaxLength = MAX_NAME_LEN,				// 글자 길이 최대값

		std::wstring* pErrorString = nullptr				// 오류 문자열
	)
	{
		size_t namelen = wcslen(strName);

		// 최소길이 체크
		if (0 < nMinLength)
		{
			if (namelen < nMinLength)
				return EWRCheckNameResult::FailedInvalidLength;
		}

		// 최대길이 체크
		if (0 < nMaxLength)
		{
			if (namelen > nMaxLength)
				return EWRCheckNameResult::FailedInvalidLength;
		}

		bool bAllNumber = true;

		for( size_t i = 0; i < namelen; i++ )
		{
			wchar_t code = strName[i];

			/*
				UniCode (http://www.unicode.org/)
				UniCode Chart (http://www.unicode.org/charts/index.html)

				Basic Latin				0x0000 ~ 0x007F
				Basic Latin 숫자		0x0030 ~ 0x0039 (10진수 : 48 ~ 57 )
				Basic Latin 영문 대문자	0x0041 ~ 0x005A (10진수 : 65 ~ 90)
				Basic Latin 영문 소문자	0x0061 ~ 0x007A (10진수 : 97 ~ 122)

				Hangul Jamo				0x1100 ~ 0x11FF (10진수 : 4352 ~ 4607)
				Hangul Syllables		0xAC00 ~ 0xD7AF (10진수 : 44032 ~ 55215)

				Hiragana				0x3040 ~ 0x309F (10진수 : 12368 ~ 12447)
				Katakana				0x30A0 ~ 0x30FF (10진수 : 12448 ~ 12543)

				CJK Ideograph			0x4E00 ~ 0x9FFF (10진수 : 19968 ~ 40959)
				Space	공백			0x0020 (10진수 : 32)
			*/

			{
				if( code >= 0x0030 && code <= 0x0039 )
				{
					// 숫자
					if(bNumber)
						continue;
				}
				else if( code >= 0x0041 && code <= 0x005A )
				{
					bAllNumber = false;

					// 대문자
					if(bEngUpper)
						continue;
				}
				else if( code >= 0x0061 && code <= 0x007A )
				{
					bAllNumber = false;

					// 소문자
					if(bEngLower)
						continue;
				}
				else if (code == 0x0020)
				{
					// 공백
					if (bBlank)
						return EWRCheckNameResult::FailedBlank;
					else
						continue;
				}
				else
				{
					if (EWRLanguageType::IsMatchLanguage(code, enCKLanguage))
					{
						bAllNumber = false;
						continue;
					}
				}

				if(pErrorString)
					*pErrorString = code;

				return EWRCheckNameResult::FailedOddWord;
			}
		}

		// 숫자로만 이루어진 이름인지 여부 체크
		if (bAllNumber)
		{
			return EWRCheckNameResult::FailedAllNumber;
		}

		return EWRCheckNameResult::Success;
	}

	// 기존 함수는 한글체크로 변경
	static EWRCheckNameResult::en CheckName(const wchar_t * strName, bool bCKEnUpper=true, bool bCKEnLower=true, bool bCKLanguage=true, bool bCKNumber=true, std::wstring* pErrorString=nullptr)
	{
		return CheckNameByLanguage(strName, EWRLanguageType::Korean, bCKEnUpper, bCKEnLower, bCKNumber, true, MIN_NAME_LEN, MAX_NAME_LEN, pErrorString);
	}
};

