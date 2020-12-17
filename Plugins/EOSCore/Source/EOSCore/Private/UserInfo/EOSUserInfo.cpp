/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "UserInfo/EOSUserInfo.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreUserInfo* UCoreUserInfo::s_UserInfoObject = nullptr;

UCoreUserInfo::UCoreUserInfo()
{
	s_UserInfoObject = this;
}

void UCoreUserInfo::EOSUserInfoQueryUserInfo(UObject* worldContextObject, FEOSUserInfoQueryUserInfoOptions options, const FOnQueryUserInfoCallback& callback)
{
	LogVerbose("");

	if (GetUserInfoHandle(worldContextObject))
	{
		EOS_UserInfo_QueryUserInfoOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;

		FQueryUserInfoCallback* CallbackObj = new FQueryUserInfoCallback({ this, callback });
		EOS_UserInfo_QueryUserInfo(GetUserInfoHandle(worldContextObject), &UserInfoOptions, CallbackObj, Internal_OnUserInfoQueryUserInfoCallback);
	}
}

void UCoreUserInfo::EOSUserInfoQueryUserInfoByDisplayName(UObject* worldContextObject, FEOSUserInfoQueryUserInfoByDisplayNameOptions options, const FOnQueryUserInfoByDisplayNameCallback& callback)
{
	LogVerbose("");

	if (GetUserInfoHandle(worldContextObject))
	{
		char DisplayNameAnsi[EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH];
		FCStringAnsi::Strncpy(DisplayNameAnsi, TCHAR_TO_UTF8(*options.DisplayName), EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH);

		EOS_UserInfo_QueryUserInfoByDisplayNameOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.DisplayName = DisplayNameAnsi;
		
		FQueryUserInfoByDisplayNameCallback* CallbackObj = new FQueryUserInfoByDisplayNameCallback({ this, callback });
		EOS_UserInfo_QueryUserInfoByDisplayName(GetUserInfoHandle(worldContextObject), &UserInfoOptions, CallbackObj, Internal_OnUserInfoQueryUserInfoByDisplayNameCallback);
	}
}

void UCoreUserInfo::EOSUserInfoQueryUserInfoByExternalAccount(UObject* worldContextObject, FEOSUserInfoQueryUserInfoByExternalAccountOptions options, const FOnQueryUserInfoByExternalAccountCallback& callback)
{
	LogVerbose("");

	if (GetUserInfoHandle(worldContextObject))
	{
		char ExternalAccountId[128];
		FCStringAnsi::Strncpy(ExternalAccountId, TCHAR_TO_UTF8(*options.ExternalAccountId), 128);

		EOS_UserInfo_QueryUserInfoByExternalAccountOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.ExternalAccountId = ExternalAccountId;
		UserInfoOptions.AccountType = static_cast<EOS_EExternalAccountType>(options.AccountType);
		
		FQueryUserInfoByExternalAccountCallback* CallbackObj = new FQueryUserInfoByExternalAccountCallback({ this, callback });
		EOS_UserInfo_QueryUserInfoByExternalAccount(GetUserInfoHandle(worldContextObject), &UserInfoOptions, CallbackObj, Internal_OnUserInfoQueryUserInfoByExternalAccountCallback);
	}
}

EOSResult UCoreUserInfo::EOSUserInfoCopyUserInfo(UObject* worldContextObject, FEOSUserInfoCopyUserInfoOptions options, FEOSUserInfo& outUserInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUserInfoHandle(worldContextObject))
	{
		EOS_UserInfo_CopyUserInfoOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;

		EOS_UserInfo* UserInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_UserInfo_CopyUserInfo(GetUserInfoHandle(worldContextObject), &UserInfoOptions, &UserInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outUserInfo = *UserInfo;
		}
		EOS_UserInfo_Release(UserInfo);
	}

	return Result;
}

int32 UCoreUserInfo::EOSUserInfoGetExternalUserInfoCount(UObject* worldContextObject, FEOSUserInfoGetExternalUserInfoCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetUserInfoHandle(worldContextObject))
	{
		EOS_UserInfo_GetExternalUserInfoCountOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;

		Result = EOS_UserInfo_GetExternalUserInfoCount(GetUserInfoHandle(worldContextObject), &UserInfoOptions);
	}

	return Result;
}

EOSResult UCoreUserInfo::EOSUserInfoCopyExternalUserInfoByIndex(UObject* worldContextObject, FEOSUserInfoCopyExternalUserInfoByIndexOptions options, FEOSUserInfoExternalUserInfo& outExternalUserInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUserInfoHandle(worldContextObject))
	{
		EOS_UserInfo_CopyExternalUserInfoByIndexOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;
		UserInfoOptions.Index = static_cast<uint32_t>(options.Index);
		
		EOS_UserInfo_ExternalUserInfo* ExternalUserInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_UserInfo_CopyExternalUserInfoByIndex(GetUserInfoHandle(worldContextObject), &UserInfoOptions, &ExternalUserInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalUserInfo = *ExternalUserInfo;
			EOS_UserInfo_ExternalUserInfo_Release(ExternalUserInfo);
		}
	}

	return Result;
}

EOSResult UCoreUserInfo::EOSUserInfoCopyExternalUserInfoByAccountType(UObject* worldContextObject, FEOSUserInfoCopyExternalUserInfoByAccountTypeOptions options, FEOSUserInfoExternalUserInfo& outExternalUserInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUserInfoHandle(worldContextObject))
	{
		EOS_UserInfo_CopyExternalUserInfoByAccountTypeOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;
		UserInfoOptions.AccountType = static_cast<EOS_EExternalAccountType>(options.AccountType);
		
		EOS_UserInfo_ExternalUserInfo* ExternalUserInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_UserInfo_CopyExternalUserInfoByAccountType(GetUserInfoHandle(worldContextObject), &UserInfoOptions, &ExternalUserInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalUserInfo = *ExternalUserInfo;
			EOS_UserInfo_ExternalUserInfo_Release(ExternalUserInfo);
		}
	}

	return Result;
}

EOSResult UCoreUserInfo::EOSUserInfoCopyExternalUserInfoByAccountId(UObject* worldContextObject, FEOSUserInfoCopyExternalUserInfoByAccountIdOptions options, FEOSUserInfoExternalUserInfo& outExternalUserInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUserInfoHandle(worldContextObject))
	{
		char AccountIdAnsi[256];
		FCStringAnsi::Strncpy(AccountIdAnsi, TCHAR_TO_UTF8(*options.AccountId), 256);

		EOS_UserInfo_CopyExternalUserInfoByAccountIdOptions UserInfoOptions = { };
		UserInfoOptions.ApiVersion = options.ApiVersion;
		UserInfoOptions.LocalUserId = options.LocalUserId;
		UserInfoOptions.TargetUserId = options.TargetUserId;
		UserInfoOptions.AccountId = AccountIdAnsi;
		
		EOS_UserInfo_ExternalUserInfo* ExternalUserInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_UserInfo_CopyExternalUserInfoByAccountId(GetUserInfoHandle(worldContextObject), &UserInfoOptions, &ExternalUserInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalUserInfo = *ExternalUserInfo;
			EOS_UserInfo_ExternalUserInfo_Release(ExternalUserInfo);
		}

	}

	return Result;
}

void UCoreUserInfo::Internal_OnUserInfoQueryUserInfoCallback(const EOS_UserInfo_QueryUserInfoCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryUserInfoCallback* CallbackObj = static_cast<FQueryUserInfoCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreUserInfo* UserInfo = CallbackObj->UserInfoObject;
	check(UserInfo);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCoreUserInfo::Internal_OnUserInfoQueryUserInfoByDisplayNameCallback(const EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryUserInfoByDisplayNameCallback* CallbackObj = static_cast<FQueryUserInfoByDisplayNameCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreUserInfo* UserInfo = CallbackObj->UserInfoObject;
	check(UserInfo);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCoreUserInfo::Internal_OnUserInfoQueryUserInfoByExternalAccountCallback(const EOS_UserInfo_QueryUserInfoByExternalAccountCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryUserInfoByExternalAccountCallback* CallbackObj = static_cast<FQueryUserInfoByExternalAccountCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreUserInfo* UserInfo = CallbackObj->UserInfoObject;
	check(UserInfo);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}
