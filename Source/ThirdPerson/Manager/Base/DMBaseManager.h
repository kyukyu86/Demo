// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template<class TManager>
class THIRDPERSON_API DMBaseManager
{
public:
	FORCEINLINE DMBaseManager() {}
	FORCEINLINE ~DMBaseManager() {}

protected:
	static TManager* Instance;

public:
	static TManager* Get()
	{
		if (Instance == nullptr)
			Instance = new TManager;
		return Instance;
	}
	static void Release()
	{
		delete Instance;
	}

};

template <class TManager> TManager* DMBaseManager<TManager>::Instance = nullptr;
