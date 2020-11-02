// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Define/WRTypeDefine.h"
#include "WRTableRow.generated.h"

USTRUCT(BlueprintType)
struct FWRTableRow : public FTableRowBase
{
	friend class WRTableManager;

	GENERATED_USTRUCT_BODY()
public:
	FWRTableRow() {};
	virtual ~FWRTableRow() {};

#if WITH_EDITOR
	virtual void CheckValidData() {}
#endif // WITH_EDITOR

	FName GetRowName() const { return RowName; }
	WRTableID GetTableID() const
	{
		return FCString::Atoi(*GetRowName().ToString());
	}
	
	virtual void GenerateCustomData() {};
private:

	FName RowName;
};