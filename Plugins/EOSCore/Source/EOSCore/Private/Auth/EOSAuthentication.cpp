/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Auth/EOSAuthentication.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"
#include <TimerManager.h>

UCoreAuthentication* UCoreAuthentication::s_AuthObject = nullptr;

UCoreAuthentication::UCoreAuthentication()
{
	s_AuthObject = this;
}

void UCoreAuthentication::EOSAuthLogin(UObject* worldContextObject, FEOSAuthLoginOptions options, const FOnAuthLoginCallback& callback)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		char IdAnsi[512];
		char TokenAnsi[512];

		FCStringAnsi::Strncpy(IdAnsi, TCHAR_TO_UTF8(*options.Credentials.ID), 512);
		FCStringAnsi::Strncpy(TokenAnsi, TCHAR_TO_UTF8(*options.Credentials.Token), 512);

		EOS_Auth_Credentials Credentials = { };

		Credentials.ApiVersion = options.Credentials.ApiVersion;
		Credentials.Id = options.Credentials.ID.Len() > 0 ? IdAnsi : nullptr;
		Credentials.Token = options.Credentials.Token.Len() > 0 ? TokenAnsi : nullptr;
		Credentials.Type = static_cast<EOS_ELoginCredentialType>(options.Credentials.Type);
		Credentials.SystemAuthCredentialsOptions = options.Credentials.SystemAuthCredentialsOptions;
		Credentials.ExternalType = static_cast<EOS_EExternalCredentialType>(options.Credentials.ExternalType);

		if (options.Credentials.Type == EEOSLoginCredentialType::EOS_LCT_PersistentAuth)
		{
			Credentials.Id = NULL;
			Credentials.Token = NULL;
		}

		EOS_Auth_LoginOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;
		AuthOptions.Credentials = &Credentials;
		AuthOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_FriendsList | EOS_EAuthScopeFlags::EOS_AS_Presence;
		
		FAuthLoginCallback* Data = new FAuthLoginCallback({ this, callback });
		EOS_Auth_Login(GetAuthHandle(worldContextObject), &AuthOptions, Data, Internal_OnAuthLoginCallback);
	}
}

void UCoreAuthentication::EOSAuthLogout(UObject* worldContextObject, FEOSAuthLogoutOptions options, const FOnAuthLogoutCallback& callback)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		EOS_Auth_LogoutOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;
		AuthOptions.LocalUserId = options.LocalUserId;

		FAuthLogoutCallback* Data = new FAuthLogoutCallback({ this, callback });

		EOS_Auth_Logout(GetAuthHandle(worldContextObject), &AuthOptions, Data, Internal_OnAuthLogoutCallback);
	}
}

void UCoreAuthentication::EOSAuthLinkAccount(UObject* worldContextObject, FEOSAuthLinkAccountOptions options, const FOnAuthLinkAccountCallback& callback)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		EOS_Auth_LinkAccountOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;
		AuthOptions.LinkAccountFlags = static_cast<EOS_ELinkAccountFlags>(options.LinkAccountFlags);
		AuthOptions.ContinuanceToken = options.ContinuanceToken;
		AuthOptions.LocalUserId = options.LocalUserId;

		FAuthLinkAccountCallback* Data = new FAuthLinkAccountCallback({ this, callback });

		EOS_Auth_LinkAccount(GetAuthHandle(worldContextObject), &AuthOptions, Data, Internal_OnAuthOnLinkAccountCallback);
	}
}

void UCoreAuthentication::EOSAuthDeletePersistentAuth(UObject* worldContextObject, const FEOSAuthDeletePersistentAuthOptions& options, const FOnAuthDeletePersistentAuthCallback& callback)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		char RefreshTokenAnsi[512];
		FCStringAnsi::Strncpy(RefreshTokenAnsi, TCHAR_TO_UTF8(*options.RefreshToken), 512);

		EOS_Auth_DeletePersistentAuthOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;

		if (options.RefreshToken.Len() > 0)
		{
			AuthOptions.RefreshToken = RefreshTokenAnsi;
		}
		else
		{
			AuthOptions.RefreshToken = nullptr;
		}

		FAuthDeletePersistentAuthCallback* Data = new FAuthDeletePersistentAuthCallback({ this, callback });
		EOS_Auth_DeletePersistentAuth(GetAuthHandle(worldContextObject), &AuthOptions, Data, Internal_OnAuthDeletePersistentAuthCallback);
	}
}

void UCoreAuthentication::EOSAuthVerifyUserAuth(UObject* worldContextObject, const FEOSAuthVerifyUserAuthOptions& options, const FOnAuthVerifyUserAuthCallback& callback)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		char ClientIdAnsi[512];
		char AppAnsi[512];
		char ExpiresAtAnsi[512];
		char RefreshTokenAnsi[512];
		char RefreshExpiresAtAnsi[512];

		FCStringAnsi::Strncpy(ClientIdAnsi, TCHAR_TO_UTF8(*options.AuthToken.ClientId), 512);
		FCStringAnsi::Strncpy(AppAnsi, TCHAR_TO_UTF8(*options.AuthToken.App), 512);
		FCStringAnsi::Strncpy(ExpiresAtAnsi, TCHAR_TO_UTF8(*options.AuthToken.ExpiresAt), 512);
		FCStringAnsi::Strncpy(RefreshTokenAnsi, TCHAR_TO_UTF8(*options.AuthToken.RefreshToken), 512);
		FCStringAnsi::Strncpy(RefreshExpiresAtAnsi, TCHAR_TO_UTF8(*options.AuthToken.ExpiresAt), 512);

		EOS_Auth_Token AuthToken = { };
		AuthToken.ApiVersion = options.AuthToken.ApiVersion;
		AuthToken.App = AppAnsi;
		AuthToken.ClientId = ClientIdAnsi;
		AuthToken.AccountId = options.AuthToken.AccountId;
		AuthToken.AccessToken = static_cast<const char*>(options.AuthToken.AccessToken);
		AuthToken.ExpiresIn = FCString::Atod(*options.AuthToken.ExpiresIn);
		AuthToken.ExpiresAt = ExpiresAtAnsi;
		AuthToken.AuthType = static_cast<EOS_EAuthTokenType>(options.AuthToken.AuthType);
		AuthToken.RefreshExpiresIn = FCString::Atod(*options.AuthToken.RefreshExpiresIn);
		AuthToken.RefreshExpiresAt = RefreshExpiresAtAnsi;

		EOS_Auth_VerifyUserAuthOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;
		AuthOptions.AuthToken = &AuthToken;

		FAuthVerifyUserAuthCallback* Data = new FAuthVerifyUserAuthCallback({ this, callback });
		EOS_Auth_VerifyUserAuth(GetAuthHandle(worldContextObject), &AuthOptions, Data, Internal_OnAuthVerifyUserAuthCallback);
	}
}

int32 UCoreAuthentication::EOSAuthGetLoggedInAccountsCount(UObject* worldContextObject)
{
	LogVerbose("");

	int32 Value = 0;

	if (GetAuthHandle(worldContextObject))
	{
		Value = EOS_Auth_GetLoggedInAccountsCount(GetAuthHandle(worldContextObject));
	}

	return Value;
}

FEOSEpicAccountId UCoreAuthentication::EOSAuthGetLoggedInAccountByIndex(UObject* worldContextObject, int32 index)
{
	LogVerbose("");

	FEOSEpicAccountId Id;

	if (GetAuthHandle(worldContextObject))
	{
		Id = EOS_Auth_GetLoggedInAccountByIndex(GetAuthHandle(worldContextObject), index);
	}

	return Id;
}

EOSELoginStatus UCoreAuthentication::EOSAuthGetLoginStatus(UObject* worldContextObject, FEOSEpicAccountId localUserId)
{
	LogVerbose("");

	EOSELoginStatus Status = EOSELoginStatus::EOS_LS_NotLoggedIn;

	if (GetAuthHandle(worldContextObject))
	{
		Status = static_cast<EOSELoginStatus>(EOS_Auth_GetLoginStatus(GetAuthHandle(worldContextObject), localUserId));
	}

	return Status;
}

EOSResult UCoreAuthentication::EOSAuthCopyUserAuthToken(UObject* worldContextObject, FEOSAuthCopyUserAuthTokenOptions options, FEOSEpicAccountId localUserId, FEOSAuthToken& outUserAuthToken)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_TimedOut;

	if (GetAuthHandle(worldContextObject))
	{
		EOS_Auth_CopyUserAuthTokenOptions AuthOptions = { };
		AuthOptions.ApiVersion = options.ApiVersion;
		EOS_Auth_Token* AuthToken = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Auth_CopyUserAuthToken(GetAuthHandle(worldContextObject), &AuthOptions, localUserId, &AuthToken));

		if (Result == EOSResult::EOS_Success)
		{
			outUserAuthToken = *AuthToken;
		}

		EOS_Auth_Token_Release(AuthToken);
	}

	return Result;
}

struct FAuthLoginStatusChangedCallback
{
public:
	FAuthLoginStatusChangedCallback(UObject* worldContextObject, const FOnAuthLoginStatusChangedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FAuthLoginStatusChangedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnAuthLoginStatusChangedCallback Callback;
};

static TMap<FEOSNotificationId, FAuthLoginStatusChangedCallback*> s_AuthLoginStatusChangedCallbacks;

FEOSNotificationId UCoreAuthentication::EOSAuthAddNotifyLoginStatusChanged(UObject* worldContextObject, const FOnAuthLoginStatusChangedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetAuthHandle(worldContextObject))
	{
		EOS_Auth_AddNotifyLoginStatusChangedOptions AuthOptions = { EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST };

		FAuthLoginStatusChangedCallback* Data = new FAuthLoginStatusChangedCallback(worldContextObject, callback);
		Data->Callback = callback;

		Id = EOS_Auth_AddNotifyLoginStatusChanged(GetAuthHandle(worldContextObject), &AuthOptions, Data, ([](const EOS_Auth_LoginStatusChangedCallbackInfo* data)
		{
			FAuthLoginStatusChangedCallback* CallbackObj = static_cast<FAuthLoginStatusChangedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_AuthLoginStatusChangedCallbacks.Add(Id, Data);
	}

	return Id;
}

void UCoreAuthentication::EOSAuthRemoveNotifyLoginStatusChanged(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetAuthHandle(worldContextObject))
	{
		EOS_Auth_RemoveNotifyLoginStatusChanged(GetAuthHandle(worldContextObject), id);
		if (FAuthLoginStatusChangedCallback** Data = s_AuthLoginStatusChangedCallbacks.Find(id))
		{
			delete Data;
		}
		s_AuthLoginStatusChangedCallbacks.Remove(id);
	}
}

void UCoreAuthentication::Internal_OnAuthLoginCallback(const EOS_Auth_LoginCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FAuthLoginCallback* Data = static_cast<FAuthLoginCallback*>(data->ClientData);
	check(Data);
	if (Data)
	{
		UCoreAuthentication* Authentication = Data->AuthenticationObject;
		check(Authentication);

		if (Authentication)
		{
			Data->Callback.ExecuteIfBound(*data);
		}

		delete Data;
	}
}

void UCoreAuthentication::Internal_OnAuthLogoutCallback(const EOS_Auth_LogoutCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FAuthLogoutCallback* Data = static_cast<FAuthLogoutCallback*>(data->ClientData);
	check(Data);
	if (Data)
	{
		UCoreAuthentication* Authentication = Data->AuthenticationObject;
		check(Authentication);

		if (Authentication)
		{
			Data->Callback.ExecuteIfBound(*data);
		}

		delete Data;
	}
}

void UCoreAuthentication::Internal_OnAuthOnLinkAccountCallback(const EOS_Auth_LinkAccountCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FAuthLinkAccountCallback* Data = static_cast<FAuthLinkAccountCallback*>(data->ClientData);
	check(Data);
	if (Data)
	{
		UCoreAuthentication* Authentication = Data->AuthenticationObject;
		check(Authentication);

		if (Authentication)
		{
			Data->Callback.ExecuteIfBound(*data);
		}

		delete Data;
	}
}

void UCoreAuthentication::Internal_OnAuthDeletePersistentAuthCallback(const EOS_Auth_DeletePersistentAuthCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FAuthDeletePersistentAuthCallback* Data = static_cast<FAuthDeletePersistentAuthCallback*>(data->ClientData);
	check(Data);
	if (Data)
	{
		UCoreAuthentication* Authentication = Data->AuthenticationObject;
		check(Authentication);

		if (Authentication)
		{
			Data->Callback.ExecuteIfBound(*data);
		}

		delete Data;
	}
}

void UCoreAuthentication::Internal_OnAuthVerifyUserAuthCallback(const EOS_Auth_VerifyUserAuthCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FAuthVerifyUserAuthCallback* Data = static_cast<FAuthVerifyUserAuthCallback*>(data->ClientData);
	check(Data);
	if (Data)
	{
		UCoreAuthentication* Authentication = Data->AuthenticationObject;
		check(Authentication);

		if (Authentication)
		{
			Data->Callback.ExecuteIfBound(*data);
		}

		delete Data;
	}
}