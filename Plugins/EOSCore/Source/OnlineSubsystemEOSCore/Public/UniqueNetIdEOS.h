/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"

#define EOS_ID_SEPARATOR TEXT("|")

class FUniqueNetIdEOS : public FUniqueNetIdString
{
public:
	FUniqueNetIdEOS() : FUniqueNetIdString() {}

	explicit FUniqueNetIdEOS(const FString& InUniqueNetId) : FUniqueNetIdString(InUniqueNetId)
	{
		ParseAccountIds();
	}

	explicit FUniqueNetIdEOS(FString&& InUniqueNetId) : FUniqueNetIdString(MoveTemp(InUniqueNetId))
	{
		ParseAccountIds();
	}

	explicit FUniqueNetIdEOS(const FUniqueNetId& Src) : FUniqueNetIdString(Src)
	{
		ParseAccountIds();
	}

public:
	friend uint32 GetTypeHash(const FUniqueNetIdEOS& A)
	{
		return ::GetTypeHash(A.UniqueNetIdStr);
	}

	/** global static instance of invalid (zero) id */
	static const TSharedRef<const FUniqueNetId>& EmptyId()
	{
		static const TSharedRef<const FUniqueNetId> EmptyId(MakeShared<FUniqueNetIdEOS>());
		return EmptyId;
	}

	virtual FName GetType() const override
	{
		static FName NAME_Eos(TEXT("EOS"));
		return NAME_Eos;
	}

public:
	FString m_EpicAccountIdStr;
	FString m_ProductUserIdStr;
public:
	void UpdateNetIdStr(const FString& InNetIdStr)
	{
		UniqueNetIdStr = InNetIdStr;
		ParseAccountIds();
	}

	void ParseAccountIds()
	{
		TArray<FString> AccountIds;
		UniqueNetIdStr.ParseIntoArray(AccountIds, EOS_ID_SEPARATOR, false);
		if (AccountIds.Num() > 0)
		{
			m_EpicAccountIdStr = AccountIds[0];
		}
		if (AccountIds.Num() > 1)
		{
			m_ProductUserIdStr = AccountIds[1];
		}
	}
};