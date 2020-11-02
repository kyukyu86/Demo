// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#include "WRTriggerUtility.h"

#include "Kismet/KismetMathLibrary.h"

WRTriggerUtility::WRTriggerUtility()
{
}

WRTriggerUtility::~WRTriggerUtility()
{
}

bool WRTriggerUtility::IsIntersected_Sphere(const FVector& ActorPosition, const float ActorRadiusRange, const FVector& TriggerPosition, const float TriggerRange)
{
	FVector _vDiff = TriggerPosition - ActorPosition;

	float Distance = _vDiff.Size();
	if (Distance <= (TriggerRange + ActorRadiusRange))
		return true;

	return false;
}


bool WRTriggerUtility::IsIntersected_Box(const FVector& ActorPosition, const float ActorRadiusRange, const FVector& TriggerPosition, const TArray<FVector>& vVectors, const float TriggerHeight)
{
	FVector D = TriggerPosition - ActorPosition;

	// 높이 비교
	if ((TriggerHeight+ ActorRadiusRange) < UKismetMathLibrary::Abs(D.Z))
		return false;

	D.Z = 0.0f;

	/*
	if (m_bInitVectors)
	{
		if (1 > m_vVectors.Num())
		{
			for (int nIdx = 0; nIdx < 4; ++nIdx)
			{
				FVector tmp;
				m_vVectors.Push(tmp);
			}
		}

		m_vVectors[0] = m_vAxisY * Size.Y;
		m_vVectors[1] = m_vAxisY1 * ActorRadiusRange;
		m_vVectors[2] = m_vAxisX * Size.X;
		m_vVectors[3] = m_vAxisX1 * ActorRadiusRange;

#if UNITY_EDITOR
		//m_bInitVectors = false;
#elif UNITY_IPHONE || UNITY_ANDROID
		m_bInitVectors = false;
#endif
	}
	*/

	int nIdx = 0;
	int nIdx2 = 0;
	float sum = 0.0f;

	FVector vUnit;
	for (nIdx = 0; nIdx < 4; ++nIdx)
	{
		vUnit = vVectors[nIdx].GetSafeNormal();
		sum = 0.0f;

		for (nIdx2 = 0; nIdx2 < 4; ++nIdx2)
		{
			sum += UKismetMathLibrary::Abs(FVector::DotProduct(vVectors[nIdx2], vUnit));
		}

		if (sum < UKismetMathLibrary::Abs(FVector::DotProduct(D, vUnit)))
		{
			return false;
		}
	}

	return true;
}

bool WRTriggerUtility::InitFromString(FString& InString, FVector& InVector)
{
	InVector.X = InVector.Y = InVector.Z = 0;

	FString Temp = InString;
	float* VectorPtr[3] = { &(InVector.X), &(InVector.Y), &(InVector.Z) };
	for (int i = 0; i < 3; ++i)
	{
		if (i == 2)
		{
			*(VectorPtr[i]) = FCString::Atof(*Temp);
		}
		else
		{
			if (GetCommaLeftValueAndRightString(Temp, VectorPtr[i]) == false)
				return false;
		}
	}
	return true;
}

bool WRTriggerUtility::InitFromString(FString& InString, FRotator& InRotator)
{
	InRotator.Pitch = InRotator.Yaw = InRotator.Roll = 0;
	
	FString Temp = InString;
	float* RotatorPtr[3] = { &(InRotator.Pitch), &(InRotator.Yaw), &(InRotator.Roll) };
	for (int i = 0; i < 3; ++i)
	{
		if (i == 2)
		{
			*(RotatorPtr[i]) = FCString::Atof(*Temp);
		}
		else
		{
			if (GetCommaLeftValueAndRightString(Temp, RotatorPtr[i]) == false)
				return false;
		}
	}
	return true;
}

bool WRTriggerUtility::InitFromString(FString& InString, FTransform& InTransform)
{
	FVector Vec = FVector::ZeroVector;
	FRotator Rot = FRotator::ZeroRotator;
	FVector Scale = FVector::ZeroVector;

	FString Temp = InString;
	float* Ptr[9] = { &(Vec.X), &(Vec.Y), &(Vec.Z), &(Rot.Pitch), &(Rot.Yaw), &(Rot.Roll), &(Scale.X), &(Scale.Y), &(Scale.Z) };
	for (int i = 0; i < 9; ++i)
	{
		if (i == 8)
		{
			*(Ptr[i]) = FCString::Atof(*Temp);
		}
		else
		{
			if (GetCommaLeftValueAndRightString(Temp, Ptr[i]) == false)
				return false;
		}
	}
	InTransform.SetLocation(Vec);
	InTransform.SetRotation(Rot.Quaternion());
	InTransform.SetScale3D(Scale);
	return true;
}

bool WRTriggerUtility::GetCommaLeftValueAndRightString(FString& String, float* Value)
{
	int index;
	if (String.FindChar(',', index) == false)
	{
		return false;
	}

	*Value = FCString::Atof(*(String.Left(index)));
	String = String.Right(String.Len() - index - 1);
	return true;
}

const FVector WRTriggerUtility::GetColliderRandomPos(const FVector& Position, bool bRamdom /*= false*/, float fScale /*= 1.0f*/, int8 nMode /*= 0*/, float Angel /*= 0*/, float SizeX /*= 1.f*/)
{
	FVector _Position = Position;
	if (bRamdom)
	{
		FVector _Offet = FVector::ZeroVector;
		_Offet.Y = 0.0f;

		if (fScale < 1.0f)
			_Offet.X = SizeX*fScale*0.5f*nMode;
		else
			_Offet.X = fScale*0.5f*nMode;

		FRotator Angler;
		Angler.Pitch = 0.0f;
		Angler.Roll = 0.0f;
		Angler.Yaw = Angel;

		_Offet = Angler.RotateVector(_Offet);

		_Position += _Offet;
	}

	return _Position;
}