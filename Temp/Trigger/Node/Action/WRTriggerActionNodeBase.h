#pragma once
#include "../WRTriggerBaseNode.h"

class WR_API WRTriggerActionNodeBase : public WRTriggerBaseNode
{
public:
	WRTriggerActionNodeBase(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType, TMap<FString, */ TMap<FString, TSharedPtr<FJsonValue>>& InValues);
	~WRTriggerActionNodeBase();


	void ProcessAction(class WRTriggerNodeBase* Caller);
	void CreateActionProcessor();

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

	//====================================================================================================================
	// [2017-1-9 : magedoga] Field ±â¹Ý Value
public:
	TSharedPtr<FJsonValue> TryGetField(const FString& FieldName);

	bool TryGetNumberField(const FString& FieldName, int32& OutNumber);
	bool TryGetNumberField(const FString& FieldName, uint32& OutNumber);
	bool TryGetNumberField(const FString& FieldName, int64& OutNumber);
	bool TryGetNumberField(const FString& FieldName, double& OutNumber);
	bool TryGetNumberField(const FString& FieldName, float& OutNumber);

	bool TryGetNumberField(const FString& FieldName, FVector& OutNumber);
	bool TryGetNumberField(const FString& FieldName, FRotator& OutNumber);

	bool TryGetBoolField(const FString& FieldName, bool& OutBool);
	bool TryGetStringField(const FString& FieldName, FString& OutString);

	bool TryGetArrayValue(const FString& FieldName, const TArray< TSharedPtr<FJsonValue> >*& ArrayValue);
	bool TryGetObjectValue(const FString& FieldName, const TSharedPtr<FJsonObject>*& ObjectValue);

	bool TryGetArrayField(const FString& FieldName, TArray<int32>& OutNumberArray);
	bool TryGetArrayField(const FString& FieldName, TArray<uint32>& OutNumberArray);
	bool TryGetArrayField(const FString& FieldName, TArray<int64>& OutNumberArray);

	bool TryGetArrayField(const FString& FieldName, TArray<double>& OutNumberArray);
	bool TryGetArrayField(const FString& FieldName, TArray<float>& OutNumberArray);
	bool TryGetArrayField(const FString& FieldName, TArray<bool>& OutBoolArray);
	bool TryGetArrayField(const FString& FieldName, TArray<FString>& OutStringArray);

	bool TryGetArrayField(const FString& FieldName, TArray<FVector>& OutVectorArray);
	bool TryGetArrayField(const FString& FieldName, TArray<FRotator>& OutRotatorArray);

	bool TryGetObjectStringField(const FString& FieldName, const FString& ObjectFieldName, FString& OutString);
	bool TryGetObjectNumberField(const FString& FieldName, const FString& ObjectFieldName, int32& OutNumber);
	

private:
	TMap<FString, TSharedPtr<FJsonValue>>		Values;
	//====================================================================================================================


private:
	class WRTriggerActionProcessor*	ActionProcessor = nullptr;

};