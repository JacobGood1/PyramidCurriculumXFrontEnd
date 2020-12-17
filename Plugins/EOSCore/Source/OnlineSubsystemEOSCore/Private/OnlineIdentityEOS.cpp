/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "OnlineIdentityEOS.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

typedef TEOSCallback<EOS_Connect_OnLoginCallback, EOS_Connect_LoginCallbackInfo> FConnectLoginCallback;

FOnlineIdentityEOS::FOnlineIdentityEOS(FOnlineSubsystemEOS* InSubsystem)
	: m_Subsystem(InSubsystem)
	, m_DefaultLocalUser(-1)
	, bInitialized(false)
	, LoginNotificationId(0)
	, LoginNotificationCallback(nullptr)
{
}

FOnlineIdentityEOS::~FOnlineIdentityEOS()
{
}

void FOnlineIdentityEOS::ConnectLoginStatusChanged(const EOS_Connect_LoginStatusChangedCallbackInfo* data)
{
	if (data->CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn)
	{
		if (ProductUserIdToUserNumMap.Contains(data->LocalUserId))
		{
			int32 LocalUserNum = ProductUserIdToUserNumMap[data->LocalUserId];
			FUniqueNetIdEOSPtr UserNetId = UserNumToNetIdMap[LocalUserNum];
			TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::LoggedIn, ELoginStatus::NotLoggedIn, *UserNetId);

			// Need to remove the local user
			RemoveLocalUser(LocalUserNum);

			if (LocalUserNumToConnectLoginNotifcationMap.Contains(LocalUserNum))
			{
				FNotificationIdCallbackPair* NotificationPair = LocalUserNumToConnectLoginNotifcationMap[LocalUserNum];
				LocalUserNumToConnectLoginNotifcationMap.Remove(LocalUserNum);

				EOS_Connect_RemoveNotifyAuthExpiration(m_Subsystem->m_ConnectHandle, NotificationPair->NotificationId);

				delete NotificationPair;
			}
		}
	}
	else
	{
		FullLoginCallback(0);
	}
}

void FOnlineIdentityEOS::LoginStatusChanged(const EOS_Auth_LoginStatusChangedCallbackInfo* Data)
{
	if (Data->CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn)
	{
		if (AccountIdToUserNumMap.Contains(Data->LocalUserId))
		{
			int32 LocalUserNum = AccountIdToUserNumMap[Data->LocalUserId];
			FUniqueNetIdEOSPtr UserNetId = UserNumToNetIdMap[LocalUserNum];
			TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::LoggedIn, ELoginStatus::NotLoggedIn, *UserNetId);
			// Need to remove the local user
			RemoveLocalUser(LocalUserNum);

			// Clean up user based notifies if we have no logged in users
			if (UserNumToNetIdMap.Num() == 0)
			{
				if (LoginNotificationId > 0)
				{
					// Remove the callback
					EOS_Auth_RemoveNotifyLoginStatusChanged(m_Subsystem->m_AuthHandle, LoginNotificationId);
					delete LoginNotificationCallback;
					LoginNotificationCallback = nullptr;
					LoginNotificationId = 0;
				}
				// Remove the per user connect login notification
				if (LocalUserNumToConnectLoginNotifcationMap.Contains(LocalUserNum))
				{
					FNotificationIdCallbackPair* NotificationPair = LocalUserNumToConnectLoginNotifcationMap[LocalUserNum];
					LocalUserNumToConnectLoginNotifcationMap.Remove(LocalUserNum);

					EOS_Connect_RemoveNotifyAuthExpiration(m_Subsystem->m_ConnectHandle, NotificationPair->NotificationId);

					delete NotificationPair;
				}
			}
		}
	}
	else
	{
		if (!AccountIdToUserNumMap.Contains(Data->LocalUserId))
		{
			ConnectLogin(AccountIdToUserNumMap.Num());
		}
	}
}

typedef TEOSCallback<EOS_Auth_OnLoginCallback, EOS_Auth_LoginCallbackInfo> FLoginCallback;
typedef TEOSCallback<EOS_Connect_OnCreateUserCallback, EOS_Connect_CreateUserCallbackInfo> FCreateUserCallback;
typedef TEOSGlobalCallback<EOS_Connect_OnAuthExpirationCallback, EOS_Connect_AuthExpirationCallbackInfo> FRefreshAuthCallback;
typedef TEOSGlobalCallback<EOS_Auth_OnLoginStatusChangedCallback, EOS_Auth_LoginStatusChangedCallbackInfo> FLoginStatusChangedCallback;
typedef TEOSGlobalCallback<EOS_Connect_OnLoginStatusChangedCallback, EOS_Connect_LoginStatusChangedCallbackInfo> FConnectLoginStatusChangedCallback;

struct FAuthCredentials : public EOS_Auth_Credentials
{
	FAuthCredentials()
		: EOS_Auth_Credentials()
	{
		ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
		Id = IdAnsi;
		Token = TokenAnsi;
	}
	char IdAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char TokenAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
};

bool FOnlineIdentityEOS::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	LogVerbose("Login: %s", *AccountCredentials.ToDebugString());

	if (GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
	{
		UE_LOG_ONLINE(Warning, TEXT("User (%d) already logged in."), LocalUserNum);
		TriggerOnLoginCompleteDelegates(LocalUserNum, false, *GetLocalUniqueNetIdEOS(LocalUserNum), FString(TEXT("Already logged in")));
		return true;
	}

	EOS_Auth_LoginOptions LoginOptions = { };
	LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
	LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_FriendsList | EOS_EAuthScopeFlags::EOS_AS_Presence;

	FAuthCredentials Credentials;
	LoginOptions.Credentials = &Credentials;

	const TCHAR* Delims[1] = { TEXT("|") };
	TArray<FString> Values;
	AccountCredentials.Type.ParseIntoArray(Values, Delims, 1, false);
	
	FString LoginType = TEXT("0");
	FString LoginCredentialsType = TEXT("0");
	FString LoginData;

	if (Values.IsValidIndex(0))
	{
		LoginType = Values[0];
	}

	if (Values.IsValidIndex(1))
	{
		LoginCredentialsType = Values[1];
	}

	if (Values.IsValidIndex(2))
	{
		LoginData = Values[2];
	}

	// LoginTypes
	// 0 = Password
	// 1 = ExchangeCode
	// 2 = PersistentAuth
	// 3 = DeviceCode
	// 4 = Developer
	// 5 = RefreshToken
	// 6 = AccountPortal
	// 7 = ExternalAuth

	// CredentialTypes
	// 0 = Epic
	// 1 = STEAM_APP_TICKET
	// 2 = PSN_ID_TOKEN
	// 3 = XBL_XSTS_TOKEN
	// 4 = DISCORD_ACCESS_TOKEN
	// 5 = GOG_SESSION_TICKET
	// 6 = NINTENDO_ID_TOKEN
	// 7 = NINTENDO_NSA_ID_TOKEN
	// 8 = UPLAY_ACCESS_TOKEN
	// 9 = OPENID_ACCESS_TOKEN
	// 10 = DEVICEID_ACCESS_TOKEN
	// 11 = APPLE_ID_TOKEN

	if (LoginType == TEXT("0")) // password
	{
		LogVerbose("Logging in using Password Authentication");

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Password;
	}
	else if (LoginType == TEXT("1")) // exchange code from epic launcher
	{
		LogVerbose("Logging in using ExhcangeCode Authentication");

		FCStringAnsi::Strncpy(Credentials.TokenAnsi, TCHAR_TO_UTF8(*AccountCredentials.Token), EOS_OSS_STRING_BUFFER_LENGTH);
		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
	}
	else if (LoginType == TEXT("2")) // persistent auth
	{
		LogVerbose("Logging in using Persistent Authentication");

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;
		Credentials.Id = nullptr;
		Credentials.Token = nullptr;
	}
	else if (LoginType == TEXT("4")) // dev tool
	{
		LogVerbose("Logging in using DevTool Authentication");

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
		FCStringAnsi::Strncpy(Credentials.IdAnsi, TCHAR_TO_UTF8(*AccountCredentials.Id), EOS_OSS_STRING_BUFFER_LENGTH);
		FCStringAnsi::Strncpy(Credentials.TokenAnsi, TCHAR_TO_UTF8(*AccountCredentials.Token), EOS_OSS_STRING_BUFFER_LENGTH);
	}
	else if (LoginType == TEXT("6")) // account portal
	{
		LogVerbose("Logging in using Account Portal Authentication");

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
		Credentials.Id = nullptr;
		Credentials.Token = nullptr;
	}
	else if (LoginType == TEXT("7"))
	{
		LogVerbose("Logging in using External Authentication");

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExternalAuth;
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("Unable to Login() user (%d) due to missing auth parameters"), LocalUserNum);
		TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdEOS(), FString(TEXT("Missing auth parameters")));
		return false;
	}

	// Perform the auth call
	if (LoginCredentialsType == TEXT("0"))
	{
		LogVerbose("Logging in using Epic Credentials Type");

		FLoginCallback* CallbackObj = new FLoginCallback();
		CallbackObj->m_CallbackLambda = [this, LocalUserNum](const EOS_Auth_LoginCallbackInfo* Data)
		{
			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				// Continue the login process by getting the product user id
				ConnectLogin(LocalUserNum);
			}
			else
			{
				FString ErrorString = FString::Printf(TEXT("Login(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
				UE_LOG_ONLINE(Warning, TEXT("%s"), *ErrorString);
				TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdEOS(), ErrorString);
			}
		};

		EOS_Auth_Login(m_Subsystem->m_AuthHandle, &LoginOptions, (void*)CallbackObj, CallbackObj->GetCallbackPtr());
	}
	else if (LoginCredentialsType == TEXT("1"))
	{
		LogVerbose("Logging in using SteamAppTicket");

		if (LoginData.IsEmpty())
		{
			LogError("No SteamAppTicket was provided");
			return false;
		}

		LogVerbose("Using SteamAppTicket: %s", *LoginData);

		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExternalAuth;
		Credentials.ExternalType = EOS_EExternalCredentialType::EOS_ECT_STEAM_APP_TICKET;
		FCStringAnsi::Strncpy(Credentials.TokenAnsi, TCHAR_TO_UTF8(*LoginData), EOS_OSS_STRING_BUFFER_LENGTH);

		FLoginCallback* CallbackObj = new FLoginCallback();
		CallbackObj->m_CallbackLambda = [this, LocalUserNum](const EOS_Auth_LoginCallbackInfo* Data)
		{
			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				// Continue the login process by getting the product user id
				ConnectLogin(LocalUserNum);
			}
			else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
			{

			}
			else
			{
				FString ErrorString = FString::Printf(TEXT("Login(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
				UE_LOG_ONLINE(Warning, TEXT("%s"), *ErrorString);
				TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdEOS(), ErrorString);
			}
		};

		EOS_Auth_Login(m_Subsystem->m_AuthHandle, &LoginOptions, (void*)CallbackObj, CallbackObj->GetCallbackPtr());

		return false;

	}
	else if (LoginCredentialsType == TEXT("2"))
	{
		LogError("Logging in using PSN ID Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("3"))
	{
		LogError("Logging in using Xbox XSTS Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("4"))
	{
		LogError("Logging in using Discord Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("5"))
	{
		LogError("Logging in using GOG Session Ticket - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("6"))
	{
		LogError("Logging in using Nintento ID Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("7"))
	{
		LogError("Logging in using Nintendo NSA Id Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("8"))
	{
		LogError("Logging in using Uplay Access Token - Not yet supported");
		return false;
	}
	else if (LoginCredentialsType == TEXT("9"))
	{
		LogError("Logging in using Open ID Access Token - Not yet supported");
		return false;
	}
	// login with device id
	else if (LoginCredentialsType == TEXT("10"))
	{
		LogVerbose("Logging in using Local Device Id");

		FConnectLoginCallback* CallbackObj = new FConnectLoginCallback();

		EOS_Connect_Credentials DeviceIdCredentials = { };
		DeviceIdCredentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		DeviceIdCredentials.Token = nullptr;
		DeviceIdCredentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;

		char DisplayNameAnsi[EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH];
		FCStringAnsi::Strncpy(DisplayNameAnsi, TCHAR_TO_UTF8(*AccountCredentials.Id), EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH);

		EOS_Connect_UserLoginInfo DeviceIdUserLoginInfo = { };
		DeviceIdUserLoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
		DeviceIdUserLoginInfo.DisplayName = DisplayNameAnsi;

		EOS_Connect_LoginOptions DeviceIdOptions = { };
		DeviceIdOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		DeviceIdOptions.Credentials = &DeviceIdCredentials;
		DeviceIdOptions.UserLoginInfo = &DeviceIdUserLoginInfo;

		CallbackObj->m_CallbackLambda = [LocalUserNum, AccountCredentials, this](const EOS_Connect_LoginCallbackInfo* Data)
		{
			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				UE_LOG_ONLINE(Warning, TEXT("Successfully logged in local device"));
				AddLocalUser(LocalUserNum, nullptr, Data->LocalUserId);

				FUniqueNetIdEOSPtr UserNetId = GetLocalUniqueNetIdEOS(LocalUserNum);
				check(UserNetId.IsValid());

				TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserNetId, FString());
				TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::NotLoggedIn, ELoginStatus::LoggedIn, *UserNetId);
			}
			else if (Data->ResultCode == EOS_EResult::EOS_NotFound)
			{
				CreateDeviceId(LocalUserNum, AccountCredentials, true);
			}
			else
			{
				FString ErrorString = FString::Printf(TEXT("Login(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

				UE_LOG_ONLINE(Warning, TEXT("%s"), *ErrorString);

				TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdEOS(), ErrorString);
			}
		};

		EOS_Connect_Login(m_Subsystem->m_ConnectHandle, &DeviceIdOptions, CallbackObj, CallbackObj->GetCallbackPtr());
	}
	else if (LoginCredentialsType == TEXT("11"))
	{
		LogError("Logging in using Apple ID Token - Not yet supported");
		return false;
	}

	return true;
}


void FOnlineIdentityEOS::Tick(float deltaTime)
{
	if (!bInitialized)
	{
		if (m_Subsystem)
		{
			/*
						{
							FLoginStatusChangedCallback* CallbackObj = new FLoginStatusChangedCallback();
							LoginNotificationCallback = CallbackObj;
							CallbackObj->m_CallbackLambda = [this](const EOS_Auth_LoginStatusChangedCallbackInfo* Data)
							{
								LoginStatusChanged(Data);
							};

							EOS_Auth_AddNotifyLoginStatusChangedOptions Options = { EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST };
							LoginNotificationId = EOS_Auth_AddNotifyLoginStatusChanged(m_Subsystem->m_AuthHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
						}*/

			{
				EOS_Connect_AddNotifyLoginStatusChangedOptions ConnectLoginStatusChangedOptions = { EOS_CONNECT_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST };

				FConnectLoginStatusChangedCallback* CallbackObj = new FConnectLoginStatusChangedCallback();
				LoginNotificationCallback = CallbackObj;
				CallbackObj->m_CallbackLambda = [this](const EOS_Connect_LoginStatusChangedCallbackInfo* Data)
				{
					ConnectLoginStatusChanged(Data);
				};

				ConnectLoginNotificationId = EOS_Connect_AddNotifyLoginStatusChanged(m_Subsystem->m_ConnectHandle, &ConnectLoginStatusChangedOptions, CallbackObj, CallbackObj->GetCallbackPtr());

				bInitialized = true;
			}
		}
	}
}

void FOnlineIdentityEOS::ConnectLogin(int32 LocalUserNum)
{
	EOS_Auth_Token* AuthToken = nullptr;
	EOS_Auth_CopyUserAuthTokenOptions CopyOptions = { };
	CopyOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

	EOS_EpicAccountId AccountId = EOS_Auth_GetLoggedInAccountByIndex(m_Subsystem->m_AuthHandle, LocalUserNum);
	EOS_EResult CopyResult = EOS_Auth_CopyUserAuthToken(m_Subsystem->m_AuthHandle, &CopyOptions, AccountId, &AuthToken);

	if (CopyResult == EOS_EResult::EOS_Success)
	{
		EOS_Connect_Credentials Credentials = { };
		Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC;
		Credentials.Token = AuthToken->AccessToken;

		EOS_Connect_LoginOptions Options = { };
		Options.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		Options.Credentials = &Credentials;

		FConnectLoginCallback* CallbackObj = new FConnectLoginCallback();
		CallbackObj->m_CallbackLambda = [LocalUserNum, AccountId, this](const EOS_Connect_LoginCallbackInfo* Data)
		{
			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				// We have an account mapping, skip to final login
				FullLoginCallback(LocalUserNum);
			}
			else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
			{
				// We need to create the mapping for this user using the continuation token
				CreateConnectedLogin(LocalUserNum, AccountId, Data->ContinuanceToken);
			}
			else
			{
				UE_LOG_ONLINE(Error, TEXT("ConnectLogin(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
				Logout(LocalUserNum);
			}
		};

		EOS_Connect_Login(m_Subsystem->m_ConnectHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

		EOS_Auth_Token_Release(AuthToken);
	}
	else
	{
		UE_LOG_ONLINE(Error, TEXT("ConnectLogin(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
		Logout(LocalUserNum);
	}
}

void FOnlineIdentityEOS::RefreshConnectLogin(int32 LocalUserNum)
{
	if (GetAuthLoggedInStatus(LocalUserNum) == EOS_ELoginStatus::EOS_LS_LoggedIn)
	{
		UE_LOG_ONLINE(Error, TEXT("Can't refresh ConnectLogin(%d) since (%d) is already logged in"), LocalUserNum, LocalUserNum);
		return;
	}

	EOS_EpicAccountId AccountId = GetLocalEpicAccountId(LocalUserNum);
	EOS_Auth_Token* AuthToken = nullptr;
	EOS_Auth_CopyUserAuthTokenOptions CopyOptions = { };
	CopyOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

	EOS_EResult CopyResult = EOS_Auth_CopyUserAuthToken(m_Subsystem->m_AuthHandle, &CopyOptions, AccountId, &AuthToken);
	if (CopyResult == EOS_EResult::EOS_Success)
	{
		EOS_Connect_Credentials Credentials = { };
		Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC;
		Credentials.Token = AuthToken->AccessToken;

		EOS_Connect_LoginOptions Options = { };
		Options.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		Options.Credentials = &Credentials;

		FConnectLoginCallback* CallbackObj = new FConnectLoginCallback();
		CallbackObj->m_CallbackLambda = [LocalUserNum, AccountId, this](const EOS_Connect_LoginCallbackInfo* Data)
		{
			if (Data->ResultCode != EOS_EResult::EOS_Success)
			{
				UE_LOG_ONLINE(Error, TEXT("Failed to refresh ConnectLogin(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
				Logout(LocalUserNum);
			}
		};
		EOS_Connect_Login(m_Subsystem->m_ConnectHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

		EOS_Auth_Token_Release(AuthToken);
	}
	else
	{
		UE_LOG_ONLINE(Error, TEXT("Failed to refresh ConnectLogin(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
		Logout(LocalUserNum);
	}
}


void FOnlineIdentityEOS::CreateConnectedLogin(int32 LocalUserNum, EOS_EpicAccountId AccountId, EOS_ContinuanceToken Token)
{
	EOS_Connect_CreateUserOptions Options = { };
	Options.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
	Options.ContinuanceToken = Token;

	FCreateUserCallback* CallbackObj = new FCreateUserCallback();
	CallbackObj->m_CallbackLambda = [LocalUserNum, AccountId, this](const EOS_Connect_CreateUserCallbackInfo* Data)
	{
		if (Data->ResultCode == EOS_EResult::EOS_Success)
		{
			// We have an account mapping, skip to final login
			FullLoginCallback(LocalUserNum);
		}
		else
		{
			FString ErrorString = FString::Printf(TEXT("Login(%d) failed with EOS result code (%s)"), LocalUserNum, ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
			TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdEOS(), ErrorString);
		}
	};

	EOS_Connect_CreateUser(m_Subsystem->m_ConnectHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
}

struct FCreateDeviceIdCallbackOSS
{
	FOnlineIdentityEOS* m_IdentityPtr;
	int32 m_LocalUserNum;
	FOnlineAccountCredentials m_Credentials;
	bool bInvokeLoginOnSuccess;
};

void FOnlineIdentityEOS::CreateDeviceId(int32 localUserNum, const FOnlineAccountCredentials& credentials, bool bInvokeLoginOnSuccess /* = false */)
{
	FCreateDeviceIdCallbackOSS* CreateDeviceIdCallbackObj = new FCreateDeviceIdCallbackOSS;
	CreateDeviceIdCallbackObj->m_Credentials = credentials;
	CreateDeviceIdCallbackObj->m_IdentityPtr = this;
	CreateDeviceIdCallbackObj->m_LocalUserNum = localUserNum;
	CreateDeviceIdCallbackObj->bInvokeLoginOnSuccess = bInvokeLoginOnSuccess;

	auto m_CreateDeviceIdCallback = [](const EOS_Connect_CreateDeviceIdCallbackInfo* data)
	{
		FCreateDeviceIdCallbackOSS* CallbackObj = static_cast<FCreateDeviceIdCallbackOSS*>(data->ClientData);
		check(CallbackObj);

		if (CallbackObj)
		{
			FString ErrorString = FString::Printf(TEXT("EOS_Connect_CreateDeviceId EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(data->ResultCode)));

			UE_LOG_ONLINE(Warning, TEXT("%s"), *ErrorString);

			if (data->ResultCode == EOS_EResult::EOS_Success && CallbackObj->bInvokeLoginOnSuccess)
			{
				CallbackObj->m_IdentityPtr->Login(CallbackObj->m_LocalUserNum, CallbackObj->m_Credentials);
			}
		}
	};

	char DeviceModelAnsi[EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH];
	FCStringAnsi::Strncpy(DeviceModelAnsi, TCHAR_TO_UTF8(*credentials.Id), EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH);

	EOS_Connect_CreateDeviceIdOptions CreateDeviceIdOptions = { EOS_CONNECT_CREATEDEVICEID_API_LATEST, "EOSCore" };
	CreateDeviceIdOptions.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;
	CreateDeviceIdOptions.DeviceModel = DeviceModelAnsi;

	EOS_Connect_CreateDeviceId(m_Subsystem->m_ConnectHandle, &CreateDeviceIdOptions, CreateDeviceIdCallbackObj, m_CreateDeviceIdCallback);
}

void FOnlineIdentityEOS::FullLoginCallback(int32 LocalUserNum)
{
	EOS_EpicAccountId AccountId = EOS_Auth_GetLoggedInAccountByIndex(m_Subsystem->m_AuthHandle, LocalUserNum);
	EOS_ProductUserId ProductUserId = EOS_Connect_GetLoggedInUserByIndex(m_Subsystem->m_ConnectHandle, LocalUserNum);

	if (EOS_EpicAccountId_IsValid(AccountId))
	{
		if (LoginNotificationId == 0)
		{
			FLoginStatusChangedCallback* CallbackObj = new FLoginStatusChangedCallback();
			LoginNotificationCallback = CallbackObj;
			CallbackObj->m_CallbackLambda = [this](const EOS_Auth_LoginStatusChangedCallbackInfo* Data)
			{
				LoginStatusChanged(Data);
			};

			EOS_Auth_AddNotifyLoginStatusChangedOptions Options = { };
			Options.ApiVersion = EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST;
			LoginNotificationId = EOS_Auth_AddNotifyLoginStatusChanged(m_Subsystem->m_AuthHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
		}
	}

	if (!LocalUserNumToConnectLoginNotifcationMap.Contains(LocalUserNum))
	{
		FNotificationIdCallbackPair* NotificationPair = new FNotificationIdCallbackPair();
		LocalUserNumToConnectLoginNotifcationMap.Add(LocalUserNum, NotificationPair);

		FRefreshAuthCallback* CallbackObj = new FRefreshAuthCallback();
		NotificationPair->Callback = CallbackObj;

		CallbackObj->m_CallbackLambda = [LocalUserNum, this](const EOS_Connect_AuthExpirationCallbackInfo* Data)
		{
			RefreshConnectLogin(LocalUserNum);
		};

		EOS_Connect_AddNotifyAuthExpirationOptions Options = { };
		Options.ApiVersion = EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST;
		NotificationPair->NotificationId = EOS_Connect_AddNotifyAuthExpiration(m_Subsystem->m_ConnectHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
	}

	AddLocalUser(LocalUserNum, AccountId, ProductUserId);
	FUniqueNetIdEOSPtr UserNetId = GetLocalUniqueNetIdEOS(LocalUserNum);
	check(UserNetId.IsValid());

	TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserNetId, FString());
	TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::NotLoggedIn, ELoginStatus::LoggedIn, *UserNetId);
}

typedef TEOSCallback<EOS_Auth_OnLogoutCallback, EOS_Auth_LogoutCallbackInfo> FLogoutCallback;

bool FOnlineIdentityEOS::Logout(int32 LocalUserNum)
{
	FUniqueNetIdEOSPtr UserId = GetLocalUniqueNetIdEOS(LocalUserNum);
	if (!UserId.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("No logged in user found for LocalUserNum=%d."),
			LocalUserNum);
		TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
		return false;
	}

	FLogoutCallback* CallbackObj = new FLogoutCallback();
	CallbackObj->m_CallbackLambda = [LocalUserNum, this](const EOS_Auth_LogoutCallbackInfo* Data)
	{
		if (Data->ResultCode == EOS_EResult::EOS_Success)
		{
			RemoveLocalUser(LocalUserNum);

			TriggerOnLogoutCompleteDelegates(LocalUserNum, true);
		}
		else
		{
			TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
		}
	};

	EOS_Auth_LogoutOptions LogoutOptions = { };
	LogoutOptions.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
	LogoutOptions.LocalUserId = StringToAccountIdMap[UserId->UniqueNetIdStr];

	EOS_Auth_Logout(m_Subsystem->m_AuthHandle, &LogoutOptions, CallbackObj, CallbackObj->GetCallbackPtr());

	return true;
}

bool FOnlineIdentityEOS::AutoLogin(int32 LocalUserNum)
{
	FString LoginId;
	FString Token;
	FString AuthType;

	FParse::Value(FCommandLine::Get(), TEXT("AUTH_LOGIN="), LoginId);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TOKEN="), Token);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);

	int32 Type = 0;
	int32 CredentialsType = 0;

	if (AuthType.Contains("password"))
	{
		Type = 0;
	}
	else if (AuthType.Contains("exchangecode"))
	{
		Type = 1;
	}
	else if (AuthType.Contains("persistentauth") || AuthType.Contains("autologin"))
	{
		Type = 2;
	}
	else if (AuthType.Contains("devicecode"))
	{
		Type = 3;
	}
	else if (AuthType.Contains("developer") || AuthType.Contains("devtool"))
	{
		Type = 4;
	}
	else if (AuthType.Contains("refreshtoken"))
	{
		Type = 5;
	}
	else if (AuthType.Contains("portal"))
	{
		Type = 6;
	}
	else if (AuthType.Contains("external"))
	{
		Type = 7;
	}
	else if (AuthType.Contains("deviceid"))
	{
		Type = 0;
		CredentialsType = 10;

		if (IsRunningDedicatedServer() && LoginId.Len() == 0)
		{
			LoginId = "DedicatedServer";
		}
	}

	AuthType = FString::Printf(TEXT("%d|%d"), Type, CredentialsType);

	if (AuthType.IsEmpty())
	{
		UE_LOG_ONLINE(Warning, TEXT("Unable to AutoLogin user (%d) due to missing auth command line args"), LocalUserNum);
		return false;
	}

	FOnlineAccountCredentials Creds(AuthType, LoginId, Token);
	return Login(LocalUserNum, Creds);
}

void FOnlineIdentityEOS::AddLocalUser(int32 LocalUserNum, EOS_EpicAccountId EpicAccountId, EOS_ProductUserId UserId)
{
	// Set the default user to the first one that logs in
	if (m_DefaultLocalUser == -1)
	{
		m_DefaultLocalUser = LocalUserNum;
	}

	const FString& NetId = MakeNetIdStringFromIds(EpicAccountId, UserId);
	FUniqueNetIdEOSRef UserNetId(new FUniqueNetIdEOS(NetId));
	FUserOnlineAccountEOSRef UserAccountRef(new FUserOnlineAccountEOS(UserNetId));

	UserNumToNetIdMap.Add(LocalUserNum, UserNetId);
	AccountIdToUserNumMap.Add(EpicAccountId, LocalUserNum);
	NetIdStringToOnlineUserMap.Add(*NetId, UserAccountRef);
	StringToUserAccountMap.Add(NetId, UserAccountRef);
	AccountIdToStringMap.Add(EpicAccountId, NetId);
	ProductUserIdToStringMap.Add(UserId, *NetId);
	StringToAccountIdMap.Add(NetId, EpicAccountId);
	EpicAccountIdToAttributeAccessMap.Add(EpicAccountId, UserAccountRef);
	ProductUserIdToUserNumMap.Add(UserId, LocalUserNum);
	StringToProductUserIdMap.Add(NetId, UserId);

	// Get auth token info
	EOS_Auth_Token* AuthToken = nullptr;
	EOS_Auth_CopyUserAuthTokenOptions Options = { };
	Options.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

	EOS_EResult CopyResult = EOS_Auth_CopyUserAuthToken(m_Subsystem->m_AuthHandle, &Options, EpicAccountId, &AuthToken);
	if (CopyResult == EOS_EResult::EOS_Success)
	{
		UserAccountRef->SetAuthAttribute(AUTH_ATTR_ID_TOKEN, AuthToken->AccessToken);
		EOS_Auth_Token_Release(AuthToken);

		UpdateUserInfo(UserAccountRef, EpicAccountId, EpicAccountId);
	}
}

void FOnlineIdentityEOS::UpdateUserInfo(IAttributeAccessInterfaceRef AttributeAccessRef, EOS_EpicAccountId LocalId, EOS_EpicAccountId AccountId)
{
	EOS_UserInfo_CopyUserInfoOptions Options = { };
	Options.ApiVersion = EOS_USERINFO_COPYUSERINFO_API_LATEST;
	Options.LocalUserId = LocalId;
	Options.TargetUserId = AccountId;

	EOS_UserInfo* UserInfo = nullptr;

	EOS_EResult CopyResult = EOS_UserInfo_CopyUserInfo(m_Subsystem->m_UserInfoHandle, &Options, &UserInfo);
	if (CopyResult == EOS_EResult::EOS_Success)
	{
		AttributeAccessRef->SetInternalAttribute(USER_ATTR_DISPLAY_NAME, UserInfo->DisplayName);
		AttributeAccessRef->SetInternalAttribute(USER_ATTR_COUNTRY, UserInfo->Country);
		AttributeAccessRef->SetInternalAttribute(USER_ATTR_LANG, UserInfo->PreferredLanguage);
		EOS_UserInfo_Release(UserInfo);
	}
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityEOS::GetUserAccount(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> Result;

	FUniqueNetIdEOS EOSID(UserId);
	const FUserOnlineAccountEOSRef* FoundUserAccount = StringToUserAccountMap.Find(EOSID.UniqueNetIdStr);
	if (FoundUserAccount != nullptr)
	{
		return *FoundUserAccount;
	}
	return nullptr;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityEOS::GetAllUserAccounts() const
{
	TArray<TSharedPtr<FUserOnlineAccount>> Result;

	for (TMap<FString, FUserOnlineAccountEOSRef>::TConstIterator It(StringToUserAccountMap); It; ++It)
	{
		Result.Add(It.Value());
	}
	return Result;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityEOS::GetUniquePlayerId(int32 LocalUserNum) const
{
	return GetLocalUniqueNetIdEOS(LocalUserNum);
}

int32 FOnlineIdentityEOS::GetLocalUserNumFromUniqueNetId(const FUniqueNetId& NetId) const
{
	FUniqueNetIdEOS EosId(NetId);
	if (StringToAccountIdMap.Contains(EosId.UniqueNetIdStr))
	{
		EOS_EpicAccountId AccountId = StringToAccountIdMap[EosId.UniqueNetIdStr];
		if (AccountIdToUserNumMap.Contains(AccountId))
		{
			return AccountIdToUserNumMap[AccountId];
		}
	}
	// Use the default user if we can't find the person that they want
	return m_DefaultLocalUser;
}

FUniqueNetIdEOSPtr FOnlineIdentityEOS::GetLocalUniqueNetIdEOS(int32 LocalUserNum) const
{
	const FUniqueNetIdEOSPtr* FoundId = UserNumToNetIdMap.Find(LocalUserNum);
	if (FoundId != nullptr)
	{
		return *FoundId;
	}
	return nullptr;
}

FUniqueNetIdEOSPtr FOnlineIdentityEOS::GetLocalUniqueNetIdEOS(EOS_ProductUserId UserId) const
{
	if (ProductUserIdToUserNumMap.Contains(UserId))
	{
		return GetLocalUniqueNetIdEOS(ProductUserIdToUserNumMap[UserId]);
	}
	return nullptr;
}

FUniqueNetIdEOSPtr FOnlineIdentityEOS::GetLocalUniqueNetIdEOS(EOS_EpicAccountId AccountId) const
{
	if (AccountIdToUserNumMap.Contains(AccountId))
	{
		return GetLocalUniqueNetIdEOS(AccountIdToUserNumMap[AccountId]);
	}
	return nullptr;
}


EOS_ELoginStatus FOnlineIdentityEOS::GetAuthLoggedInStatus(int32 userId) const
{
	EOS_EpicAccountId AccountId = EOS_Auth_GetLoggedInAccountByIndex(m_Subsystem->m_AuthHandle, userId);

	return EOS_Auth_GetLoginStatus(m_Subsystem->m_AuthHandle, AccountId);
}


EOS_ELoginStatus FOnlineIdentityEOS::GetConnectLoggedInStatus(int32 userId) const
{
	EOS_ProductUserId AccountId = EOS_Connect_GetLoggedInUserByIndex(m_Subsystem->m_ConnectHandle, userId);

	return EOS_Connect_GetLoginStatus(m_Subsystem->m_ConnectHandle, AccountId);
}

EOS_EpicAccountId FOnlineIdentityEOS::GetLocalEpicAccountId(int32 LocalUserNum) const
{
	EOS_EpicAccountId AccountId = EOS_Auth_GetLoggedInAccountByIndex(m_Subsystem->m_AuthHandle, LocalUserNum);

	return AccountId;
}

EOS_EpicAccountId FOnlineIdentityEOS::GetLocalEpicAccountId() const
{
	return GetLocalEpicAccountId(m_DefaultLocalUser);
}

EOS_ProductUserId FOnlineIdentityEOS::GetLocalProductUserId(int32 LocalUserNum) const
{
	EOS_ProductUserId ProductId = EOS_Connect_GetLoggedInUserByIndex(m_Subsystem->m_ConnectHandle, LocalUserNum);

	return ProductId;
}

EOS_ProductUserId FOnlineIdentityEOS::GetLocalProductUserId() const
{
	return GetLocalProductUserId(m_DefaultLocalUser);
}

EOS_EpicAccountId FOnlineIdentityEOS::GetLocalEpicAccountId(EOS_ProductUserId UserId) const
{
	if (ProductUserIdToUserNumMap.Contains(UserId))
	{
		return GetLocalEpicAccountId(ProductUserIdToUserNumMap[UserId]);
	}
	return nullptr;
}

EOS_ProductUserId FOnlineIdentityEOS::GetLocalProductUserId(EOS_EpicAccountId AccountId) const
{
	if (AccountIdToUserNumMap.Contains(AccountId))
	{
		return GetLocalProductUserId(AccountIdToUserNumMap[AccountId]);
	}
	return nullptr;
}

EOS_EpicAccountId FOnlineIdentityEOS::GetEpicAccountId(const FUniqueNetId& NetId) const
{
	FUniqueNetIdEOS EOSId(NetId);

	if (StringToAccountIdMap.Contains(EOSId.UniqueNetIdStr))
	{
		return StringToAccountIdMap[EOSId.UniqueNetIdStr];
	}
	return nullptr;
}

EOS_ProductUserId FOnlineIdentityEOS::GetProductUserId(const FUniqueNetId& NetId) const
{
	FUniqueNetIdEOS EOSId(NetId);

	if (StringToProductUserIdMap.Contains(EOSId.UniqueNetIdStr))
	{
		return StringToProductUserIdMap[EOSId.UniqueNetIdStr];
	}
	return nullptr;
}

FOnlineUserPtr FOnlineIdentityEOS::GetLocalOnlineUser(int32 LocalUserNum) const
{
	FOnlineUserPtr OnlineUser;
	if (UserNumToNetIdMap.Contains(LocalUserNum))
	{
		const FUniqueNetIdEOSPtr NetId = UserNumToNetIdMap.FindRef(LocalUserNum);
		if (NetIdStringToOnlineUserMap.Contains(*NetId->UniqueNetIdStr))
		{
			OnlineUser = NetIdStringToOnlineUserMap.FindRef(*NetId->UniqueNetIdStr);
		}
	}
	return OnlineUser;
}

FOnlineUserPtr FOnlineIdentityEOS::GetOnlineUser(EOS_ProductUserId UserId) const
{
	FOnlineUserPtr OnlineUser;
	if (ProductUserIdToStringMap.Contains(UserId))
	{
		const FString& NetId = ProductUserIdToStringMap.FindRef(UserId);
		if (NetIdStringToOnlineUserMap.Contains(*NetId))
		{
			OnlineUser = NetIdStringToOnlineUserMap.FindRef(*NetId);
		}
	}
	return OnlineUser;
}

FOnlineUserPtr FOnlineIdentityEOS::GetOnlineUser(EOS_EpicAccountId AccountId) const
{
	FOnlineUserPtr OnlineUser;
	if (AccountIdToStringMap.Contains(AccountId))
	{
		const FString& NetId = AccountIdToStringMap.FindRef(AccountId);
		if (NetIdStringToOnlineUserMap.Contains(*NetId))
		{
			OnlineUser = NetIdStringToOnlineUserMap.FindRef(*NetId);
		}
	}
	return OnlineUser;
}

void FOnlineIdentityEOS::RemoveLocalUser(int32 LocalUserNum)
{
	const FUniqueNetIdEOSPtr* FoundId = UserNumToNetIdMap.Find(LocalUserNum);
	if (FoundId != nullptr)
	{
		const FString& NetId = (*FoundId)->UniqueNetIdStr;
		EOS_EpicAccountId AccountId = StringToAccountIdMap[NetId];
		AccountIdToStringMap.Remove(AccountId);
		AccountIdToUserNumMap.Remove(AccountId);
		NetIdStringToOnlineUserMap.Remove(NetId);
		StringToAccountIdMap.Remove(NetId);
		StringToUserAccountMap.Remove(NetId);
		UserNumToNetIdMap.Remove(LocalUserNum);
	}
	// Reset this for the next user login
	if (LocalUserNum == m_DefaultLocalUser)
	{
		m_DefaultLocalUser = -1;
	}
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityEOS::CreateUniquePlayerId(uint8* Bytes, int32 Size)
{
	if (Bytes != nullptr && Size > 0)
	{
		FString StrId(Size, (TCHAR*)Bytes);
		return MakeShareable(new FUniqueNetIdEOS(StrId));
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityEOS::CreateUniquePlayerId(const FString& Str)
{
	return MakeShareable(new FUniqueNetIdEOS(Str));
}

ELoginStatus::Type FOnlineIdentityEOS::GetLoginStatus(int32 LocalUserNum) const
{
	FUniqueNetIdEOSPtr UserId = GetLocalUniqueNetIdEOS(LocalUserNum);
	if (UserId.IsValid())
	{
		return GetLoginStatus(*UserId);
	}
	return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityEOS::GetLoginStatus(const FUniqueNetIdEOS& UserId) const
{
	if (!StringToAccountIdMap.Contains(UserId.UniqueNetIdStr))
	{
		return ELoginStatus::NotLoggedIn;
	}

	// are we using a local account?
	if (UserId.m_EpicAccountIdStr == "INVALID")
	{
		EOS_ProductUserId ProductUserId = EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*UserId.m_ProductUserIdStr));

		if (EOS_ProductUserId_IsValid(ProductUserId))
		{
			EOS_ELoginStatus ConnectLoginStatus = EOS_Connect_GetLoginStatus(m_Subsystem->m_ConnectHandle, ProductUserId);

			switch (ConnectLoginStatus)
			{
			case EOS_ELoginStatus::EOS_LS_LoggedIn:
			{
				return ELoginStatus::LoggedIn;
			}
			case EOS_ELoginStatus::EOS_LS_UsingLocalProfile:
			{
				return ELoginStatus::UsingLocalProfile;
			}
			}
		}
	}

	EOS_EpicAccountId AccountId = StringToAccountIdMap[UserId.UniqueNetIdStr];
	if (AccountId == nullptr)
	{
		return ELoginStatus::NotLoggedIn;
	}

	EOS_ELoginStatus LoginStatus = EOS_Auth_GetLoginStatus(m_Subsystem->m_AuthHandle, AccountId);
	switch (LoginStatus)
	{
	case EOS_ELoginStatus::EOS_LS_LoggedIn:
	{
		return ELoginStatus::LoggedIn;
	}
	case EOS_ELoginStatus::EOS_LS_UsingLocalProfile:
	{
		return ELoginStatus::UsingLocalProfile;
	}
	}
	return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityEOS::GetLoginStatus(const FUniqueNetId& UserId) const
{
	FUniqueNetIdEOS EosId(UserId);
	return GetLoginStatus(EosId);
}

FString FOnlineIdentityEOS::GetPlayerNickname(int32 LocalUserNum) const
{
	FUniqueNetIdEOSPtr UserId = GetLocalUniqueNetIdEOS(LocalUserNum);
	if (UserId.IsValid())
	{
		TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(*UserId);
		if (UserAccount.IsValid())
		{
			return UserAccount->GetDisplayName();
		}
	}
	return FString();
}

FString FOnlineIdentityEOS::GetPlayerNickname(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(UserId);
	if (UserAccount.IsValid())
	{
		return UserAccount->GetDisplayName();
	}
	return FString();
}

FString FOnlineIdentityEOS::GetAuthToken(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(*UserId);
		if (UserAccount.IsValid())
		{
			return UserAccount->GetAccessToken();
		}
	}
	return FString();
}

void FOnlineIdentityEOS::RevokeAuthToken(const FUniqueNetId& LocalUserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(LocalUserId, FOnlineError(EOnlineErrorResult::NotImplemented));
}

FPlatformUserId FOnlineIdentityEOS::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	return GetLocalUserNumFromUniqueNetId(UniqueNetId);
}

void FOnlineIdentityEOS::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(UserId, Privilege, (uint32)EPrivilegeResults::NoFailures);
}

FString FOnlineIdentityEOS::GetAuthType() const
{
	return TEXT("epic");
}

typedef TEOSCallback<EOS_UserInfo_OnQueryUserInfoCallback, EOS_UserInfo_QueryUserInfoCallbackInfo> FReadUserInfoCallback;

void FOnlineIdentityEOS::ReadUserInfo(EOS_EpicAccountId EpicAccountId)
{
	FReadUserInfoCallback* CallbackObj = new FReadUserInfoCallback();
	CallbackObj->m_CallbackLambda = [this](const EOS_UserInfo_QueryUserInfoCallbackInfo* Data)
	{
		if (Data->ResultCode == EOS_EResult::EOS_Success)
		{
			IAttributeAccessInterfaceRef AttributeAccessRef = EpicAccountIdToAttributeAccessMap[Data->TargetUserId];
			UpdateUserInfo(AttributeAccessRef, Data->LocalUserId, Data->TargetUserId);
		}
	};

	EOS_UserInfo_QueryUserInfoOptions Options = { };
	Options.ApiVersion = EOS_USERINFO_QUERYUSERINFO_API_LATEST;
	Options.LocalUserId = GetLocalEpicAccountId(m_DefaultLocalUser);
	Options.TargetUserId = EpicAccountId;
	EOS_UserInfo_QueryUserInfo(m_Subsystem->m_UserInfoHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
}

struct FQueryByDisplayNameOptions :
	public EOS_UserInfo_QueryUserInfoByDisplayNameOptions
{
	FQueryByDisplayNameOptions() :
		EOS_UserInfo_QueryUserInfoByDisplayNameOptions()
	{
		ApiVersion = EOS_USERINFO_QUERYUSERINFOBYDISPLAYNAME_API_LATEST;
		DisplayName = DisplayNameAnsi;
	}
	char DisplayNameAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
};

typedef TEOSCallback<EOS_UserInfo_OnQueryUserInfoByDisplayNameCallback, EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo> FQueryInfoByNameCallback;
