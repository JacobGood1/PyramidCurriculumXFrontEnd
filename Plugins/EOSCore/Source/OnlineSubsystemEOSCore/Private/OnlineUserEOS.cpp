/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "OnlineUserEOS.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineIdentityEOS.h"
#include "OnlineSubsystemEOSPrivatePCH.h"


FOnlineUserEOS::FOnlineUserEOS(FOnlineSubsystemEOS* InSubsystem)
	: m_Subsystem(InSubsystem)
{
}

bool FOnlineUserEOS::QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>>& UserIds)
{
	return false;
}

bool FOnlineUserEOS::GetAllUserInfo(int32 LocalUserNum, TArray<TSharedRef<FOnlineUser>>& OutUsers)
{
	OutUsers.Reset();
	// Get remote users
	for (TMap<FString, FOnlineUserPtr>::TConstIterator It(m_Subsystem->m_IdentityInterfacePtr->NetIdStringToOnlineUserMap); It; ++It)
	{
		if (It.Value().IsValid())
		{
			OutUsers.Add(It.Value().ToSharedRef());
		}
	}
	// Get local users
	for (TMap<FString, FUserOnlineAccountEOSRef>::TConstIterator It(m_Subsystem->m_IdentityInterfacePtr->StringToUserAccountMap); It; ++It)
	{
		OutUsers.Add(It.Value());
	}
	return true;
}

TSharedPtr<FOnlineUser> FOnlineUserEOS::GetUserInfo(int32 LocalUserNum, const class FUniqueNetId& UserId)
{
	TSharedPtr<FOnlineUser> OnlineUser;
	FUniqueNetIdEOS EOSID(UserId);
	if (m_Subsystem->m_IdentityInterfacePtr->NetIdStringToOnlineUserMap.Contains(EOSID.UniqueNetIdStr))
	{
		OnlineUser = m_Subsystem->m_IdentityInterfacePtr->NetIdStringToOnlineUserMap[EOSID.UniqueNetIdStr];
	}
	return OnlineUser;
}

bool FOnlineUserEOS::QueryUserIdMapping(const FUniqueNetId& UserId, const FString& DisplayNameOrEmail, const FOnQueryUserMappingComplete& Delegate)
{
	return false;
}

bool FOnlineUserEOS::QueryExternalIdMappings(const FUniqueNetId& UserId, const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, const FOnQueryExternalIdMappingsComplete& Delegate)
{
	FUniqueNetIdEOS EOSID(UserId);
	if (!m_Subsystem->m_IdentityInterfacePtr->StringToProductUserIdMap.Contains(EOSID.UniqueNetIdStr))
	{
		Delegate.ExecuteIfBound(false, UserId, QueryOptions, ExternalIds, FString::Printf(TEXT("User (%s) is not logged in, so can't query external account ids"), *EOSID.UniqueNetIdStr));
		return false;
	}
	int32 LocalUserNum = m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(UserId);

	EOS_ProductUserId LocalUserId = m_Subsystem->m_IdentityInterfacePtr->StringToProductUserIdMap[EOSID.UniqueNetIdStr];
	const int32 NumBatches = (ExternalIds.Num() / EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_MAX_ACCOUNT_IDS) + 1;
	int32 QueryStart = 0;
	// Process queries in batches since there's a max that can be done at once
	for (int32 BatchCount = 0; BatchCount < NumBatches; BatchCount++)
	{
		const uint32 AmountToProcess = FMath::Min(ExternalIds.Num() - QueryStart, EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_MAX_ACCOUNT_IDS);
		TArray<FString> BatchIds;
		BatchIds.Empty(AmountToProcess);
		FQueryByStringIdsOptions Options(AmountToProcess, LocalUserId);
		// Build an options up per batch
		for (uint32 ProcessedCount = 0; ProcessedCount < AmountToProcess; ProcessedCount++, QueryStart++)
		{
			FCStringAnsi::Strncpy(Options.PointerArray[ProcessedCount], TCHAR_TO_UTF8(*ExternalIds[ProcessedCount]), EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH + 1);
			BatchIds.Add(ExternalIds[ProcessedCount]);
		}
		FQueryByStringIdsCallback* CallbackObj = new FQueryByStringIdsCallback();
		CallbackObj->m_CallbackLambda = [LocalUserNum, QueryOptions, BatchIds, this, Delegate](const EOS_Connect_QueryExternalAccountMappingsCallbackInfo* Data)
		{
			EOS_EResult Result = Data->ResultCode;
			if (m_Subsystem->m_IdentityInterfacePtr->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
			{
				// Handle the user logging out while a read is in progress
				Result = EOS_EResult::EOS_InvalidUser;
			}

			FString ErrorString;
			FUniqueNetIdEOS EOSID;
			if (Result == EOS_EResult::EOS_Success)
			{
				EOSID = *m_Subsystem->m_IdentityInterfacePtr->UserNumToNetIdMap[LocalUserNum];

				FGetAccountMappingOptions Options;
				Options.LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId(m_Subsystem->m_IdentityInterfacePtr->m_DefaultLocalUser);
				// Get the product id for each epic account passed in
				for (const FString& StringId : BatchIds)
				{
					FCStringAnsi::Strncpy(Options.AccountId, TCHAR_TO_UTF8(*StringId), EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH + 1);
					EOS_ProductUserId ProductUserId = EOS_Connect_GetExternalAccountMapping(m_Subsystem->m_ConnectHandle, &Options);
					if (EOS_ProductUserId_IsValid(ProductUserId) == EOS_TRUE)
					{
						EOS_EpicAccountId AccountId = EOS_EpicAccountId_FromString(Options.AccountId);
					}
				}
			}
			else
			{
				ErrorString = FString::Printf(TEXT("EOS_Connect_QueryExternalAccountMappings() failed with result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
			}
			Delegate.ExecuteIfBound(false, EOSID, QueryOptions, BatchIds, ErrorString);
		};

		EOS_Connect_QueryExternalAccountMappings(m_Subsystem->m_ConnectHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
	}
	return true;
}

void FOnlineUserEOS::GetExternalIdMappings(const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, TArray<TSharedPtr<const FUniqueNetId>>& OutIds)
{
	OutIds.Reset();
	for (const FString& AccountIdStr : ExternalIds)
	{
		OutIds.Add(GetExternalIdMapping(QueryOptions, AccountIdStr));
	}
}

TSharedPtr<const FUniqueNetId> FOnlineUserEOS::GetExternalIdMapping(const FExternalIdQueryOptions& QueryOptions, const FString& ExternalId)
{
	TSharedPtr<const FUniqueNetId> NetId;
	EOS_EpicAccountId AccountId = EOS_EpicAccountId_FromString(TCHAR_TO_UTF8(*ExternalId));
	if (EOS_EpicAccountId_IsValid(AccountId) == EOS_TRUE && m_Subsystem->m_IdentityInterfacePtr->AccountIdToStringMap.Contains(AccountId))
	{
		const FString& NetIdStr = m_Subsystem->m_IdentityInterfacePtr->AccountIdToStringMap[AccountId];
		NetId = m_Subsystem->m_IdentityInterfacePtr->NetIdStringToOnlineUserMap[NetIdStr]->GetUserId();
	}
	return NetId;
}