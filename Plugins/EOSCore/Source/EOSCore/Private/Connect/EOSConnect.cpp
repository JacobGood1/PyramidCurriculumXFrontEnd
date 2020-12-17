/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Connect/EOSConnect.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreConnect* UCoreConnect::s_ConnectObject = nullptr;

UCoreConnect::UCoreConnect()
{
	s_ConnectObject = this;
}

void UCoreConnect::EOSConnectLogin(UObject* worldContextObject, FEOSConnectLoginOptions options, const FOnLoginCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		char DisplayNameAnsi[EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH];
		FCStringAnsi::Strncpy(DisplayNameAnsi, TCHAR_TO_UTF8(*options.UserLoginInfo.DisplayName), EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH);

		EOS_Connect_Credentials Credentials = { };
		Credentials.ApiVersion = options.Credentials.ApiVersion;
		Credentials.Token = static_cast<const char*>(options.Credentials.Token);
		Credentials.Type = static_cast<EOS_EExternalCredentialType>(options.Credentials.Type);

		EOS_Connect_UserLoginInfo UserLoginInfo = { };
		UserLoginInfo.ApiVersion = options.UserLoginInfo.ApiVersion;
		UserLoginInfo.DisplayName = DisplayNameAnsi;

		EOS_Connect_LoginOptions LoginOptions = { };
		LoginOptions.ApiVersion = options.ApiVersion;
		LoginOptions.Credentials = &Credentials;
		LoginOptions.UserLoginInfo = &UserLoginInfo;

		EEOSEExternalCredentialType Type = options.Credentials.Type;

		if (Type == EEOSEExternalCredentialType::EOS_ECT_APPLE_ID_TOKEN || Type == EEOSEExternalCredentialType::EOS_ECT_NINTENDO_ID_TOKEN || Type == EEOSEExternalCredentialType::EOS_ECT_NINTENDO_NSA_ID_TOKEN || Type == EEOSEExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN)
		{
			LoginOptions.UserLoginInfo = &UserLoginInfo;
		}
		else
		{
			LoginOptions.UserLoginInfo = nullptr;
		}

		FLoginCallback* CallbackObj = new FLoginCallback({ this, callback });
		EOS_Connect_Login(GetConnectHandle(worldContextObject), &LoginOptions, CallbackObj, Internal_OnConnectLoginCallback);
	}
}

void UCoreConnect::EOSConnectCreateUser(UObject* worldContextObject, FEOSConnectCreateUserOptions options, const FOnCreateUserCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_CreateUserOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.ContinuanceToken = options.ContinuanceToken;

		FCreateUserCallback* CallbackObj = new FCreateUserCallback({ this, callback });
		EOS_Connect_CreateUser(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectCreateUserCallback);
	}
}

void UCoreConnect::EOSConnectLinkAccount(UObject* worldContextObject, FEOSConnectLinkAccountOptions options, const FOnLinkAccountCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_LinkAccountOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.LocalUserId = options.LocalUserId;
		ConnectOptions.ContinuanceToken = options.ContinuanceToken;

		FLinkAccountCallback* CallbackObj = new FLinkAccountCallback({ this, callback });
		EOS_Connect_LinkAccount(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectLinkAccountCallback);
	}
}

void UCoreConnect::EOSConnectUnlinkAccount(UObject* worldContextObject, FEOSConnectUnlinkAccountOptions options, const FOnUnlinkAccountCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_UnlinkAccountOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.LocalUserId = options.LocalUserId;

		FUnlinkAccountCallback* CallbackObj = new FUnlinkAccountCallback({ this, callback });
		EOS_Connect_UnlinkAccount(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectUnlinkAccountCallback);
	}
}

void UCoreConnect::EOSConnectCreateDeviceId(UObject* worldContextObject, FEOSConnectCreateDeviceIdOptions options, const FOnCreateDeviceIdCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		char DeviceModelAnsi[EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH];
		FCStringAnsi::Strncpy(DeviceModelAnsi, TCHAR_TO_UTF8(*options.DeviceModel), EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH);

		EOS_Connect_CreateDeviceIdOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;

		if (options.DeviceModel.Len() > 0)
		{
			ConnectOptions.DeviceModel = DeviceModelAnsi;
		}
		else
		{
			ConnectOptions.DeviceModel = nullptr;
		}

		FCreateDeviceIdCallback* CallbackObj = new FCreateDeviceIdCallback({ this, callback });
		EOS_Connect_CreateDeviceId(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectCreateDeviceIdCallback);
	}
}

void UCoreConnect::EOSConnectDeleteDeviceId(UObject* worldContextObject, FEOSConnectDeleteDeviceIdOptions options, const FOnDeleteDeviceIdCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_DeleteDeviceIdOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;

		FDeleteDeviceIdCallback* CallbackObj = new FDeleteDeviceIdCallback({ this, callback });
		EOS_Connect_DeleteDeviceId(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectDeleteDeviceIdCallback);
	}
}

void UCoreConnect::EOSConnectTransferDeviceIdAccount(UObject* worldContextObject, FEOSConnectTransferDeviceIdAccountOptions options, const FOnTransferDeviceIdAccountCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_TransferDeviceIdAccountOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;

		FTransferDeviceIdAccountCallback* CallbackObj = new FTransferDeviceIdAccountCallback({ this, callback });
		EOS_Connect_TransferDeviceIdAccount(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectTransferDeviceIdAccount);
	}
}

void UCoreConnect::EOSConnectQueryExternalAccountMappings(UObject* worldContextObject, FEOSConnectQueryExternalAccountMappingsOptions options, const FOnQueryExternalAccountMappingsCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		FQueryExternalAccountMappingsOperation QueryExternalAccountMappingsOperation(options.ExternalAccountIds.Num(), options.LocalUserId, static_cast<EOS_EExternalAccountType>(options.AccountIdType));

		for (int32 i = 0; i < options.ExternalAccountIds.Num(); i++)
		{
			FCStringAnsi::Strncpy(const_cast<char*>(QueryExternalAccountMappingsOperation.PointerArray[i]), TCHAR_TO_UTF8(*options.ExternalAccountIds[i]), EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH);
		}

		FQueryExternalAccountMappingsCallback* CallbackObj = new FQueryExternalAccountMappingsCallback({ this, callback });
		EOS_Connect_QueryExternalAccountMappings(GetConnectHandle(worldContextObject), &QueryExternalAccountMappingsOperation, CallbackObj, Internal_OnConnectQueryExternalAccountMappingsCallback);
	}
}

void UCoreConnect::EOSConnectQueryProductUserIdMappings(UObject* worldContextObject, FEOSConnectQueryProductUserIdMappingsOptions options, const FOnQueryProductUserIdMappingsCallback& callback)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_QueryProductUserIdMappingsOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.LocalUserId = options.LocalUserId;
		ConnectOptions.AccountIdType_DEPRECATED = options.AccountIdType_DEPRECATED;
		ConnectOptions.ProductUserIds = new EOS_ProductUserId[options.ProductUserIdCount];
		ConnectOptions.ProductUserIdCount = static_cast<uint32_t>(options.ProductUserIdCount);

		for (int32 i = 0; i < options.ProductUserIdCount; i++)
		{
			ConnectOptions.ProductUserIds[i] = options.ProductUserIds[i];
		}

		FQueryProductUserIdMappingsCallback* CallbackObj = new FQueryProductUserIdMappingsCallback({ this, callback });
		EOS_Connect_QueryProductUserIdMappings(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, Internal_OnConnectQueryProductUserIdMappingsCallback);
		delete[] ConnectOptions.ProductUserIds;
	}
}

FEOSProductUserId UCoreConnect::EOSConnectGetExternalAccountMapping(UObject* worldContextObject, FEOSConnectGetExternalAccountMappingsOptions options)
{
	LogVerbose("");

	FEOSProductUserId ProductUserId;

	if (GetConnectHandle(worldContextObject))
	{
		char TargetExternalUserIdAnsi[512];
		FCStringAnsi::Strncpy(TargetExternalUserIdAnsi, TCHAR_TO_UTF8(*options.TargetExternalUserId), 512);

		EOS_Connect_GetExternalAccountMappingsOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.LocalUserId = options.LocalUserId;
		ConnectOptions.AccountIdType = static_cast<EOS_EExternalAccountType>(options.AccountIdType);

		if (options.TargetExternalUserId.Len() > 0)
		{
			ConnectOptions.TargetExternalUserId = TargetExternalUserIdAnsi;
		}
		else
		{
			ConnectOptions.TargetExternalUserId = nullptr;
		}

		ProductUserId = EOS_Connect_GetExternalAccountMapping(GetConnectHandle(worldContextObject), &ConnectOptions);
	}

	return ProductUserId;
}

EOSResult UCoreConnect::EOSConnectGetProductUserIdMapping(UObject* worldContextObject, FEOSConnectGetProductUserIdMappingOptions options, FString& outString)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_UnrecognizedResponse;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_GetProductUserIdMappingOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.LocalUserId = options.LocalUserId;
		ConnectOptions.AccountIdType = static_cast<EOS_EExternalAccountType>(options.AccountIdType);
		ConnectOptions.TargetProductUserId = options.TargetProductUserId;

		char Buffer[EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH];
		int32_t BufferSize = EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Connect_GetProductUserIdMapping(GetConnectHandle(worldContextObject), &ConnectOptions, Buffer, &BufferSize));

		if (Result == EOSResult::EOS_Success)
		{
			outString = Buffer;
		}
	}

	return Result;
}

int32 UCoreConnect::EOSConnectGetProductUserExternalAccountCount(UObject* worldContextObject, FEOSConnectGetProductUserExternalAccountCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_GetProductUserExternalAccountCountOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.TargetUserId = options.TargetUserId;

		Result = EOS_Connect_GetProductUserExternalAccountCount(GetConnectHandle(worldContextObject), &ConnectOptions);
	}

	return Result;
}

EOSResult UCoreConnect::EOSConnectCopyProductUserExternalAccountByIndex(UObject* worldContextObject, FEOSConnectCopyProductUserExternalAccountByIndexOptions options, FEOSConnectExternalAccountInfo& outExternalAccountInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_UnrecognizedResponse;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_CopyProductUserExternalAccountByIndexOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.TargetUserId = options.TargetUserId;
		ConnectOptions.ExternalAccountInfoIndex = static_cast<uint32_t>(options.ExternalAccountInfoIndex);

		EOS_Connect_ExternalAccountInfo* AccountInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Connect_CopyProductUserExternalAccountByIndex(GetConnectHandle(worldContextObject), &ConnectOptions, &AccountInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalAccountInfo = *AccountInfo;
		}

		EOS_Connect_ExternalAccountInfo_Release(AccountInfo);
	}

	return Result;
}

EOSResult UCoreConnect::EOSConnectCopyProductUserExternalAccountByAccountType(UObject* worldContextObject, FEOSConnectCopyProductUserExternalAccountByAccountTypeOptions options, FEOSConnectExternalAccountInfo& outExternalAccountInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_UnrecognizedResponse;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_CopyProductUserExternalAccountByAccountTypeOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.TargetUserId = options.TargetUserId;
		ConnectOptions.AccountIdType = static_cast<EOS_EExternalAccountType>(options.AccountIdType);

		EOS_Connect_ExternalAccountInfo* AccountInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Connect_CopyProductUserExternalAccountByAccountType(GetConnectHandle(worldContextObject), &ConnectOptions, &AccountInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalAccountInfo = *AccountInfo;
		}

		EOS_Connect_ExternalAccountInfo_Release(AccountInfo);
	}

	return Result;
}

EOSResult UCoreConnect::EOSConnectCopyProductUserExternalAccountByAccountId(UObject* worldContextObject, FEOSConnectCopyProductUserExternalAccountByAccountIdOptions options, FEOSConnectExternalAccountInfo& outExternalAccountInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_UnrecognizedResponse;

	if (GetConnectHandle(worldContextObject))
	{
		char AccountIdAnsi[512];
		FCStringAnsi::Strncpy(AccountIdAnsi, TCHAR_TO_UTF8(*options.AccountId), 512);

		EOS_Connect_CopyProductUserExternalAccountByAccountIdOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.TargetUserId = options.TargetUserId;
		ConnectOptions.AccountId = AccountIdAnsi;

		EOS_Connect_ExternalAccountInfo* AccountInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Connect_CopyProductUserExternalAccountByAccountId(GetConnectHandle(worldContextObject), &ConnectOptions, &AccountInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalAccountInfo = *AccountInfo;
		}

		EOS_Connect_ExternalAccountInfo_Release(AccountInfo);
	}

	return Result;
}

EOSResult UCoreConnect::EOSConnectCopyProductUserInfo(UObject* worldContextObject, FEOSConnectCopyProductUserInfoOptions options, FEOSConnectExternalAccountInfo& outExternalAccountInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_UnrecognizedResponse;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_CopyProductUserInfoOptions ConnectOptions = { };
		ConnectOptions.ApiVersion = options.ApiVersion;
		ConnectOptions.TargetUserId = options.TargetUserId;

		EOS_Connect_ExternalAccountInfo* AccountInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Connect_CopyProductUserInfo(GetConnectHandle(worldContextObject), &ConnectOptions, &AccountInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outExternalAccountInfo = *AccountInfo;
		}

		EOS_Connect_ExternalAccountInfo_Release(AccountInfo);
	}

	return Result;
}

int32 UCoreConnect::EOSConnectGetLoggedInUsersCount(UObject* worldContextObject)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetConnectHandle(worldContextObject))
	{
		Result = EOS_Connect_GetLoggedInUsersCount(GetConnectHandle(worldContextObject));
	}

	return Result;
}

FEOSProductUserId UCoreConnect::EOSConnectGetLoggedInUserByIndex(UObject* worldContextObject, int32 index)
{
	LogVerbose("");

	FEOSProductUserId Result;

	if (GetConnectHandle(worldContextObject))
	{
		Result = EOS_Connect_GetLoggedInUserByIndex(GetConnectHandle(worldContextObject), index);
	}

	return Result;
}

EEOSELoginStatus UCoreConnect::EOSConnectGetLoginStatus(UObject* worldContextObject, FEOSProductUserId localUserId)
{
	LogVerbose("");

	EEOSELoginStatus Result = EEOSELoginStatus::EOS_LS_NotLoggedIn;

	if (GetConnectHandle(worldContextObject))
	{
		Result = static_cast<EEOSELoginStatus>(EOS_Connect_GetLoginStatus(GetConnectHandle(worldContextObject), localUserId));
	}

	return Result;
}

struct FAuthExpirationCallback
{
public:
	FAuthExpirationCallback(UObject* worldContextObject, const FOnAuthExpirationCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FAuthExpirationCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnAuthExpirationCallback Callback;
};

static TMap<FEOSNotificationId, FAuthExpirationCallback*> s_AuthExpirationCallbacks;

FEOSNotificationId UCoreConnect::EOSConnectAddNotifyAuthExpiration(UObject* worldContextObject, const FOnAuthExpirationCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_AddNotifyAuthExpirationOptions ConnectOptions = { EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST };

		FAuthExpirationCallback* CallbackObj = new FAuthExpirationCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Connect_AddNotifyAuthExpiration(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, ([](const EOS_Connect_AuthExpirationCallbackInfo* data)
		{
			FAuthExpirationCallback* CallbackObj = static_cast<FAuthExpirationCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));
		
		s_AuthExpirationCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreConnect::EOSConnectRemoveNotifyAuthExpiration(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_RemoveNotifyAuthExpiration(GetConnectHandle(worldContextObject), id);

		if (FAuthExpirationCallback** CallbackObj = s_AuthExpirationCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_AuthExpirationCallbacks.Remove(id);
	}
}

struct FLoginStatusChangedCallback
{
public:
	FLoginStatusChangedCallback(UObject* worldContextObject, const FOnLoginStatusChangedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{}
	~FLoginStatusChangedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLoginStatusChangedCallback Callback;
};

static TMap<FEOSNotificationId, FLoginStatusChangedCallback*> s_LoginStatusChangedCallbacks;

FEOSNotificationId UCoreConnect::EOSConnectAddNotifyLoginStatusChanged(UObject* worldContextObject, const FOnLoginStatusChangedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_AddNotifyLoginStatusChangedOptions ConnectOptions = { EOS_CONNECT_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST };

		FLoginStatusChangedCallback* CallbackObj = new FLoginStatusChangedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Connect_AddNotifyLoginStatusChanged(GetConnectHandle(worldContextObject), &ConnectOptions, CallbackObj, ([](const EOS_Connect_LoginStatusChangedCallbackInfo* data)
		{
			FLoginStatusChangedCallback* CallbackObj = static_cast<FLoginStatusChangedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_LoginStatusChangedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreConnect::EOSConnectRemoveNotifyLoginStatusChanged(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetConnectHandle(worldContextObject))
	{
		EOS_Connect_RemoveNotifyLoginStatusChanged(GetConnectHandle(worldContextObject), id);
		if (FLoginStatusChangedCallback** CallbackObj = s_LoginStatusChangedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LoginStatusChangedCallbacks.Remove(id);
	}
}

void UCoreConnect::Internal_OnConnectLoginCallback(const EOS_Connect_LoginCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLoginCallback* CallbackObj = static_cast<FLoginCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectCreateUserCallback(const EOS_Connect_CreateUserCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FCreateUserCallback* CallbackObj = static_cast<FCreateUserCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectLinkAccountCallback(const EOS_Connect_LinkAccountCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLinkAccountCallback* CallbackObj = static_cast<FLinkAccountCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectUnlinkAccountCallback(const EOS_Connect_UnlinkAccountCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUnlinkAccountCallback* CallbackObj = static_cast<FUnlinkAccountCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectCreateDeviceIdCallback(const EOS_Connect_CreateDeviceIdCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FCreateDeviceIdCallback* CallbackObj = static_cast<FCreateDeviceIdCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectDeleteDeviceIdCallback(const EOS_Connect_DeleteDeviceIdCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDeleteDeviceIdCallback* CallbackObj = static_cast<FDeleteDeviceIdCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{

			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectTransferDeviceIdAccount(const EOS_Connect_TransferDeviceIdAccountCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FTransferDeviceIdAccountCallback* CallbackObj = static_cast<FTransferDeviceIdAccountCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectQueryExternalAccountMappingsCallback(const EOS_Connect_QueryExternalAccountMappingsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryExternalAccountMappingsCallback* CallbackObj = static_cast<FQueryExternalAccountMappingsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{

			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreConnect::Internal_OnConnectQueryProductUserIdMappingsCallback(const EOS_Connect_QueryProductUserIdMappingsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryProductUserIdMappingsCallback* CallbackObj = static_cast<FQueryProductUserIdMappingsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreConnect* Connect = CallbackObj->ConnectObject;
		check(Connect);

		if (Connect)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}