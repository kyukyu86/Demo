#include "WRTriggerActionNodeBase.h"
#include "WRTriggerActionProcessor.h"
#include "../Trigger/WRTriggerNodeBase.h"
#include "Utility/WRTriggerUtility.h"

WRTriggerActionNodeBase::WRTriggerActionNodeBase(TMap<FString, TSharedPtr<FJsonValue>> &InValues)//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType, TMap<FString, TSharedPtr<FJsonValue>>& InValues):
	//WRTriggerBaseNode(InUid, InNodeName, InNodeType)
{
	// [2017-1-9 : magedoga] Copy Values
	for (auto It = InValues.CreateIterator(); It; ++It)
	{
		FString Field = It.Key();

		TSharedPtr<FJsonValue> JValue;
		switch (It.Value()->Type)
		{
			case EJson::Boolean:
			{
				JValue = MakeShareable(new FJsonValueBoolean(It.Value()->AsBool()));
			}break;
			case EJson::Number:
			{
				JValue = MakeShareable(new FJsonValueNumber(It.Value()->AsNumber()));
			}break;
			case EJson::String:
			{
				JValue = MakeShareable(new FJsonValueString(It.Value()->AsString()));
			}break;
			case EJson::Array:
			{
				JValue = MakeShareable(new FJsonValueArray(It.Value()->AsArray()));
			}break;
			case EJson::Object:
			{
				JValue = MakeShareable(new FJsonValueObject(It.Value()->AsObject()));
			}break;
			default:
			{
				continue;
			}
		}

		Values.Add(Field, JValue);
	}
}

WRTriggerActionNodeBase::~WRTriggerActionNodeBase()
{
	delete ActionProcessor;
	ActionProcessor = nullptr;
	Values.Empty();
}




void WRTriggerActionNodeBase::ProcessAction(class WRTriggerNodeBase* Caller)
{
	if (ActionProcessor == nullptr)
	{
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerActionNodeBase] ActionProcessor is Null"));
		return;
	}

	ActionProcessor->ProcessAction(Caller);
}



void WRTriggerActionNodeBase::CreateActionProcessor()
{
	if (ActionProcessor != nullptr)
		return;

	// [2017-1-9 : magedoga] Set Processor
	ActionProcessor = WRTriggerActionProcessor::CreateLegalProcessor(this);
}

void WRTriggerActionNodeBase::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerBaseNode::ImportDataFromJson(InJsonObject);
}



TSharedPtr<FJsonValue> WRTriggerActionNodeBase::TryGetField(const FString& FieldName)
{
	const TSharedPtr<FJsonValue>* Field = Values.Find(FieldName);
	return (Field != nullptr && Field->IsValid()) ? *Field : TSharedPtr<FJsonValue>();
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, int32& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetNumber(OutNumber);
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, uint32& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetNumber(OutNumber);
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, int64& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetNumber(OutNumber);
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, double& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetNumber(OutNumber);
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, float& OutNumber)
{
	double TempNum;
	if (this->TryGetNumberField(FieldName, TempNum) == false)
		return false;

	OutNumber = (float)TempNum;
	return true;
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, FVector& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	FString Buffer;
	if (Field.IsValid())
	{
		if (Field->TryGetString(Buffer))
		{
			WRTriggerUtility::InitFromString(Buffer, OutNumber);
			return true;
		}
	}
	return false;
}

bool WRTriggerActionNodeBase::TryGetNumberField(const FString& FieldName, FRotator& OutNumber)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	FString Buffer;
	if (Field.IsValid())
	{
		if (Field->TryGetString(Buffer))
		{
			WRTriggerUtility::InitFromString(Buffer, OutNumber);
			return true;
		}
	}
	return false;
}

bool WRTriggerActionNodeBase::TryGetBoolField(const FString& FieldName, bool& OutBool)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetBool(OutBool);
}

bool WRTriggerActionNodeBase::TryGetStringField(const FString& FieldName, FString& OutString)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetString(OutString);
}



bool WRTriggerActionNodeBase::TryGetArrayValue(const FString& FieldName, const TArray< TSharedPtr<FJsonValue> >*& ArrayValue)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetArray(ArrayValue);
}

bool WRTriggerActionNodeBase::TryGetObjectValue(const FString& FieldName, const TSharedPtr<FJsonObject>*& ObjectValue)
{
	TSharedPtr<FJsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetObject(ObjectValue);
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<int32>& OutNumberArray)
{
	OutNumberArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		int32 Int32Temp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetNumber(Int32Temp))
				OutNumberArray.Add(Int32Temp);
		}
	}

	if (OutNumberArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<uint32>& OutNumberArray)
{
	OutNumberArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		uint32 Uint32Temp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetNumber(Uint32Temp))
				OutNumberArray.Add(Uint32Temp);
		}
	}

	if (OutNumberArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<int64>& OutNumberArray)
{
	OutNumberArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		int64 Int64Temp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetNumber(Int64Temp))
				OutNumberArray.Add(Int64Temp);
		}
	}

	if (OutNumberArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<double>& OutNumberArray)
{
	OutNumberArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		double DoubleTemp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetNumber(DoubleTemp))
				OutNumberArray.Add(DoubleTemp);
		}
	}

	if (OutNumberArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<float>& OutNumberArray)
{
	OutNumberArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		double DoubleTemp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetNumber(DoubleTemp))
				OutNumberArray.Add((float)DoubleTemp);
		}
	}

	if (OutNumberArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<bool>& OutBoolArray)
{
	OutBoolArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		bool BoolTemp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetBool(BoolTemp))
				OutBoolArray.Add(BoolTemp);
		}
	}

	if (OutBoolArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<FString>& OutStringArray)
{
	OutStringArray.Empty();

	const TArray< TSharedPtr<FJsonValue> >* TempValue;
	if (TryGetArrayValue(FieldName, TempValue) == true)
	{
		FString StrTemp;
		for (int i = 0; i < TempValue->Num(); ++i)
		{
			if ((*TempValue)[i]->TryGetString(StrTemp))
				OutStringArray.Add(StrTemp);
		}
	}

	if (OutStringArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<FVector>& OutVectorArray)
{
	OutVectorArray.Empty();
	TArray<FString> StrArrayTemp;
	TryGetArrayField(FieldName, StrArrayTemp);

	FVector VectorTemp;
	for (int i = 0; i < StrArrayTemp.Num(); ++i)
	{
		if(WRTriggerUtility::InitFromString(StrArrayTemp[i], VectorTemp))
			OutVectorArray.Add(VectorTemp);
	}

	if (OutVectorArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetArrayField(const FString& FieldName, TArray<FRotator>& OutRotatorArray)
{
	OutRotatorArray.Empty();
	TArray<FString> StrArrayTemp;
	TryGetArrayField(FieldName, StrArrayTemp);

	FRotator RotatorTemp;
	for (int i = 0; i < StrArrayTemp.Num(); ++i)
	{
		if (WRTriggerUtility::InitFromString(StrArrayTemp[i], RotatorTemp))
			OutRotatorArray.Add(RotatorTemp);
	}

	if (OutRotatorArray.Num() > 0)
		return true;
	else
		return false;
}

bool WRTriggerActionNodeBase::TryGetObjectStringField(const FString& FieldName, const FString& ObjectFieldName, FString& OutString)
{
	const TSharedPtr<FJsonObject>* TempObjectValue;
	if (TryGetObjectValue(FieldName, TempObjectValue) == true)
	{
		return (*TempObjectValue)->TryGetStringField(ObjectFieldName, OutString);
	}
	return false;
}

bool WRTriggerActionNodeBase::TryGetObjectNumberField(const FString& FieldName, const FString& ObjectFieldName, int32& OutNumber)
{
	const TSharedPtr<FJsonObject>* TempObjectValue;
	if (TryGetObjectValue(FieldName, TempObjectValue) == true)
	{
		return (*TempObjectValue)->TryGetNumberField(ObjectFieldName, OutNumber);
	}
	return false;
}

