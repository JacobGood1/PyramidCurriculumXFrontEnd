/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "OnlineSessionEOS.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSubsystemTypesEOS.h"
#include "OnlineIdentityEOS.h"
#include "NboSerializerEOS.h"
#include "InternetAddrEOS.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

/** This is the game name plus version in ansi done once for optimization */
char BucketIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];

FString MakeStringFromAttributeValue(const EOS_Sessions_AttributeData* Attribute)
{
	switch (Attribute->ValueType)
	{
		case EOS_ESessionAttributeType::EOS_SAT_Int64:
		{
			int32 Value = Attribute->Value.AsInt64;
			return FString::Printf(TEXT("%d"), Value);
		}
		case EOS_ESessionAttributeType::EOS_SAT_Double:
		{
			double Value = Attribute->Value.AsDouble;
			return FString::Printf(TEXT("%f"), Value);
		}
		case EOS_ESessionAttributeType::EOS_SAT_String:
		{
			FString Value = Attribute->Value.AsUtf8;
			return FString::Printf(TEXT("%s"), *Value);
		}
	}
	return TEXT("");
}

bool IsSessionSettingTypeSupported(EOnlineKeyValuePairDataType::Type InType)
{
	switch (InType)
	{
		case EOnlineKeyValuePairDataType::Int32:
		case EOnlineKeyValuePairDataType::UInt32:
		case EOnlineKeyValuePairDataType::Int64:
		case EOnlineKeyValuePairDataType::Double:
		case EOnlineKeyValuePairDataType::String:
		case EOnlineKeyValuePairDataType::Float:
		case EOnlineKeyValuePairDataType::Bool:
		{
			return true;
		}
	}
	return false;
}

EOS_EOnlineComparisonOp ToEOSSearchOp(EOnlineComparisonOp::Type Op)
{
	switch (Op)
	{
		case EOnlineComparisonOp::Equals:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_EQUAL;
		}
		case EOnlineComparisonOp::NotEquals:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_NOTEQUAL;
		}
		case EOnlineComparisonOp::GreaterThan:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_GREATERTHAN;
		}
		case EOnlineComparisonOp::GreaterThanEquals:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_GREATERTHANOREQUAL;
		}
		case EOnlineComparisonOp::LessThan:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_LESSTHAN;
		}
		case EOnlineComparisonOp::LessThanEquals:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_LESSTHANOREQUAL;
		}
		case EOnlineComparisonOp::Near:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_DISTANCE;
		}
		case EOnlineComparisonOp::In:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_ANYOF;
		}
		case EOnlineComparisonOp::NotIn:
		{
			return EOS_EOnlineComparisonOp::EOS_OCO_NOTANYOF;
		}
	}
	return EOS_EOnlineComparisonOp::EOS_OCO_EQUAL;
}

struct FAttributeOptions :
	public EOS_Sessions_AttributeData
{
	char KeyAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];
	char ValueAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];

	FAttributeOptions(const char* InKey, const char* InValue) :
		EOS_Sessions_AttributeData()
	{
		ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
		ValueType = EOS_ESessionAttributeType::EOS_SAT_String;
		Value.AsUtf8 = ValueAnsi;
		Key = KeyAnsi;
		FCStringAnsi::Strncpy(KeyAnsi, InKey, EOS_OSS_STRING_BUFFER_LENGTH + 1);
		FCStringAnsi::Strncpy(ValueAnsi, InValue, EOS_OSS_STRING_BUFFER_LENGTH + 1);
	}

	FAttributeOptions(const char* InKey, bool InValue) :
		EOS_Sessions_AttributeData()
	{
		ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
		ValueType = EOS_ESessionAttributeType::EOS_SAT_Boolean;
		Value.AsBool = InValue ? EOS_TRUE : EOS_FALSE;
		Key = KeyAnsi;
		FCStringAnsi::Strncpy(KeyAnsi, InKey, EOS_OSS_STRING_BUFFER_LENGTH + 1);
	}

	FAttributeOptions(const char* InKey, float InValue) :
		EOS_Sessions_AttributeData()
	{
		ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
		ValueType = EOS_ESessionAttributeType::EOS_SAT_Double;
		Value.AsDouble = InValue;
		Key = KeyAnsi;
		FCStringAnsi::Strncpy(KeyAnsi, InKey, EOS_OSS_STRING_BUFFER_LENGTH + 1);
	}

	FAttributeOptions(const char* InKey, int32 InValue) :
		EOS_Sessions_AttributeData()
	{
		ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
		ValueType = EOS_ESessionAttributeType::EOS_SAT_Int64;
		Value.AsInt64 = InValue;
		Key = KeyAnsi;
		FCStringAnsi::Strncpy(KeyAnsi, InKey, EOS_OSS_STRING_BUFFER_LENGTH + 1);
	}

	FAttributeOptions(const char* InKey, const FVariantData& InValue) :
		EOS_Sessions_AttributeData()
	{
		ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;

		switch (InValue.GetType())
		{
			case EOnlineKeyValuePairDataType::Int32:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Int64;
				int32 RawValue = 0;
				InValue.GetValue(RawValue);
				Value.AsInt64 = RawValue;
				break;
			}
			case EOnlineKeyValuePairDataType::UInt32:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Int64;
				uint32 RawValue = 0;
				InValue.GetValue(RawValue);
				Value.AsInt64 = RawValue;
				break;
			}
			case EOnlineKeyValuePairDataType::Int64:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Int64;
				int64 RawValue = 0;
				InValue.GetValue(RawValue);
				Value.AsInt64 = RawValue;
				break;
			}
			case EOnlineKeyValuePairDataType::Bool:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Boolean;
				bool RawValue = false;
				InValue.GetValue(RawValue);
				Value.AsBool = RawValue ? EOS_TRUE : EOS_FALSE;
				break;
			}
			case EOnlineKeyValuePairDataType::Double:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Double;
				double RawValue = 0.0;
				InValue.GetValue(RawValue);
				Value.AsDouble = RawValue;
				break;
			}
			case EOnlineKeyValuePairDataType::Float:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_Double;
				float RawValue = 0.0;
				InValue.GetValue(RawValue);
				Value.AsDouble = RawValue;
				break;
			}
			case EOnlineKeyValuePairDataType::String:
			{
				ValueType = EOS_ESessionAttributeType::EOS_SAT_String;
				Value.AsUtf8 = ValueAnsi;
				Key = KeyAnsi;
				FString OutString;
				InValue.GetValue(OutString);
				FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*OutString), EOS_OSS_STRING_BUFFER_LENGTH + 1);
				break;
			}
		}
		Key = KeyAnsi;
		FCStringAnsi::Strncpy(KeyAnsi, InKey, EOS_OSS_STRING_BUFFER_LENGTH + 1);
	}
};

FOnlineSessionInfoEOS::FOnlineSessionInfoEOS()
	: m_HostAddr(nullptr)
	, m_SessionId(TEXT("INVALID"))
	, m_SessionHandle(nullptr)
	, bIsFromClone(false)
{
}

FOnlineSessionInfoEOS::FOnlineSessionInfoEOS(const FString& InHostIp, const FString& InSessionId, EOS_HSessionDetails InSessionHandle)
	: FOnlineSessionInfo()
	, m_SessionId(InSessionId)
	, m_SessionHandle(InSessionHandle)
	, bIsFromClone(false)
{
	if (InHostIp.StartsWith(EOS_CONNECTION_URL_PREFIX, ESearchCase::IgnoreCase))
	{
		m_HostAddr = ISocketSubsystem::Get(EOS_SUBSYSTEM)->GetAddressFromString(InHostIp);
		m_EOSAddress = InHostIp;
	}
	else
	{
		m_HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetAddressFromString(InHostIp);
		m_HostAddr->SetPort(FURL::UrlConfig.DefaultPort);
	}
}

FOnlineSessionInfoEOS::~FOnlineSessionInfoEOS()
{
	if (m_SessionHandle != nullptr && !bIsFromClone)
	{
		EOS_SessionDetails_Release(m_SessionHandle);
	}
}

void FOnlineSessionInfoEOS::InitLAN(FOnlineSubsystemEOS* Subsystem)
{
	// Read the IP from the system
	bool bCanBindAll;
	m_HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);

	// The below is a workaround for systems that set hostname to a distinct address from 127.0.0.1 on a loopback interface.
	// See e.g. https://www.debian.org/doc/manuals/debian-reference/ch05.en.html#_the_hostname_resolution
	// and http://serverfault.com/questions/363095/why-does-my-hostname-appear-with-the-address-127-0-1-1-rather-than-127-0-0-1-in
	// Since we bind to 0.0.0.0, we won't answer on 127.0.1.1, so we need to advertise ourselves as 127.0.0.1 for any other loopback address we may have.
	uint32 HostIp = 0;
	m_HostAddr->GetIp(HostIp); // will return in host order
	// if this address is on loopback interface, advertise it as 127.0.0.1
	if ((HostIp & 0xff000000) == 0x7f000000)
	{
		m_HostAddr->SetIp(0x7f000001);	// 127.0.0.1
	}

	// Now set the port that was configured
	m_HostAddr->SetPort(GetPortFromNetDriver(Subsystem->GetInstanceName()));

	FGuid OwnerGuid;
	FPlatformMisc::CreateGuid(OwnerGuid);
	m_SessionId = FUniqueNetIdEOS(OwnerGuid.ToString());
}

typedef TEOSGlobalCallback<EOS_Sessions_OnSessionInviteReceivedCallback, EOS_Sessions_SessionInviteReceivedCallbackInfo> FSessionInviteReceivedCallback;
typedef TEOSGlobalCallback<EOS_Sessions_OnSessionInviteAcceptedCallback, EOS_Sessions_SessionInviteAcceptedCallbackInfo> FSessionInviteAcceptedCallback;


void FOnlineSessionEOS::Init(const char* InBucketId)
{
	FCStringAnsi::Strncpy(BucketIdAnsi, InBucketId, EOS_OSS_STRING_BUFFER_LENGTH + 1);

	bIsDedicatedServer = IsRunningDedicatedServer();
	
	bIsUsingP2PSockets = false;
	GConfig->GetBool(TEXT("/Script/OnlineSubsystemEOS.NetDriverEOS"), TEXT("bIsUsingP2PSockets"), bIsUsingP2PSockets, GEngineIni);

	/* Listen for Session Invites */
	{
		FSessionInviteAcceptedCallback* CallbackObj = new FSessionInviteAcceptedCallback();
		m_SessionInviteAcceptedCallback = CallbackObj;
		CallbackObj->m_CallbackLambda = [this](const EOS_Sessions_SessionInviteAcceptedCallbackInfo* data)
		{
			UE_LOG_ONLINE_SESSION(Log, TEXT("Invite accepted"));

			EOS_HSessionDetails m_SessionDetailsHandle = NULL;
			EOS_Sessions_CopySessionHandleByInviteIdOptions m_SessionHandleByInviteIdOptions = { EOS_SESSIONS_COPYSESSIONHANDLEBYINVITEID_API_LATEST, data->InviteId };

			EOS_EResult CopySessionHandleResult = EOS_Sessions_CopySessionHandleByInviteId(m_Subsystem->m_SessionsHandle, &m_SessionHandleByInviteIdOptions, &m_SessionDetailsHandle);

			if (CopySessionHandleResult == EOS_EResult::EOS_Success)
			{
				EOS_SessionDetails_Info* m_SessionInfo = new EOS_SessionDetails_Info;
				EOS_SessionDetails_CopyInfoOptions m_CopyInfoOptions = { EOS_SESSIONDETAILS_COPYINFO_API_LATEST };

				EOS_EResult CopySessionDetailsResult = EOS_SessionDetails_CopyInfo(m_SessionDetailsHandle, &m_CopyInfoOptions, &m_SessionInfo);

				if (CopySessionHandleResult == EOS_EResult::EOS_Success)
				{
					FOnlineSessionSearchResult SearchResult;
					SearchResult.PingInMs = -1;
					SearchResult.Session.SessionInfo = MakeShareable(new FOnlineSessionInfoEOS(m_SessionInfo->HostAddress, m_SessionInfo->SessionId, m_SessionDetailsHandle));

					CopyAttributes(m_SessionDetailsHandle, SearchResult.Session);
					TriggerOnSessionUserInviteAcceptedDelegates(true, 0, m_Subsystem->GetIdentityInterface()->GetUniquePlayerId(0), SearchResult);
				}
				else
				{
					UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionDetails_CopyInfo() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(CopySessionDetailsResult)));
				}
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_CopySessionHandleByInviteId() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(CopySessionHandleResult)));
			}
		};

		EOS_Sessions_AddNotifySessionInviteAcceptedOptions Options = { };
		Options.ApiVersion = EOS_SESSIONS_ADDNOTIFYSESSIONINVITEACCEPTED_API_LATEST;
		m_SessionInviteAcceptedId = EOS_Sessions_AddNotifySessionInviteAccepted(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());
	}
	FOnStartSessionCompleteDelegate StartSessionDelegate = FOnStartSessionCompleteDelegate::CreateLambda([this](FName sessionName, bool bWasSuccessful)
	{
		FNamedOnlineSession* Session = GetNamedSession(sessionName);

		if (Session)
		{
			this->UpdatePresenceJoinInfo(0, Session);
		}
	});

	AddOnStartSessionCompleteDelegate_Handle(StartSessionDelegate);

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateLambda([this](FName sessionName, EOnJoinSessionCompleteResult::Type result)
	{
		FNamedOnlineSession* Session = GetNamedSession(sessionName);

		if (Session)
		{
			this->UpdatePresenceJoinInfo(0, Session);
		}
	});

	AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateLambda([this](FName sessionName, bool)
	{
		FNamedOnlineSession* Session = GetNamedSession(sessionName);

		if (Session)
		{
			this->UpdatePresenceJoinInfo(0, Session);
		}
	});

	AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
}

bool FOnlineSessionEOS::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
	uint32 Result = ONLINE_FAIL;

	// Check for an existing session
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session == nullptr)
	{
		if (bIsDedicatedServer || m_Subsystem->m_IdentityInterfacePtr->GetLoginStatus(HostingPlayerNum) >= ELoginStatus::UsingLocalProfile)
		{
			// Create a new session and deep copy the game settings
			Session = AddNamedSession(SessionName, NewSessionSettings);
			check(Session);

			Session->SessionState = EOnlineSessionState::Creating;
			Session->OwningUserId = m_Subsystem->m_IdentityInterfacePtr->GetUniquePlayerId(HostingPlayerNum);

			if (const auto SessionSetting = NewSessionSettings.Settings.Find("SESSION_NAME"))
			{
				FString NewSessionName;
				SessionSetting->Data.GetValue(NewSessionName);
				Session->OwningUserName = NewSessionName;
			}

			if (Session->OwningUserName.Len() == 0)
			{
				Session->OwningUserName = m_Subsystem->m_IdentityInterfacePtr->GetPlayerNickname(HostingPlayerNum);
			}

			if (bIsDedicatedServer || (Session->OwningUserId.IsValid() && Session->OwningUserId->IsValid()))
			{
				Session->NumOpenPrivateConnections = NewSessionSettings.NumPrivateConnections;
				Session->NumOpenPublicConnections = NewSessionSettings.NumPublicConnections;
				Session->HostingPlayerNum = HostingPlayerNum;
				Session->SessionSettings.BuildUniqueId = GetBuildUniqueId();
				Session->SessionSettings.bIsDedicated = IsRunningDedicatedServer();
				Session->SessionSettings.bAntiCheatProtected = false;
				Session->SessionSettings.bUsesStats = false;

				if (!NewSessionSettings.bIsLANMatch)
				{
					Result = CreateEOSSession(HostingPlayerNum, Session);
				}
				else
				{
					Result = CreateLANSession(HostingPlayerNum, Session);
				}
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Warning, TEXT("Cannot create session '%s': invalid user (%d)."), *SessionName.ToString(), HostingPlayerNum);
			}

			if (Result != ONLINE_IO_PENDING)
			{
				// Set the game state as pending (not started)
				Session->SessionState = EOnlineSessionState::Pending;

				if (Result != ONLINE_SUCCESS)
				{
					// Clean up the session info so we don't get into a confused state
					RemoveNamedSession(SessionName);
				}
				else
				{
					RegisterLocalPlayers(Session);
				}
			}
		}
		else
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Cannot create session '%s': user not logged in (%d)."), *SessionName.ToString(), HostingPlayerNum);
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Cannot create session '%s': session already exists."), *SessionName.ToString());
	}

	if (Result != ONLINE_IO_PENDING)
	{
		TriggerOnCreateSessionCompleteDelegates(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
	}

	return Result == ONLINE_IO_PENDING || Result == ONLINE_SUCCESS;
}

bool FOnlineSessionEOS::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
	return CreateSession(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(HostingPlayerId), SessionName, NewSessionSettings);
}

uint32 FOnlineSessionEOS::CreateLANSession(int32 HostingPlayerNum, FNamedOnlineSession* Session)
{
	check(Session);
	uint32 Result = ONLINE_SUCCESS;

	// Setup the host session info
	FOnlineSessionInfoEOS* NewSessionInfo = new FOnlineSessionInfoEOS();
	NewSessionInfo->InitLAN(m_Subsystem);
	Session->SessionInfo = MakeShareable(NewSessionInfo);

	// Don't create a the beacon if advertising is off
	if (Session->SessionSettings.bShouldAdvertise)
	{
		if (!m_LANSession.IsValid())
		{
			m_LANSession = MakeShareable(new FLANSession());
		}

		FOnValidQueryPacketDelegate QueryPacketDelegate = FOnValidQueryPacketDelegate::CreateRaw(this, &FOnlineSessionEOS::OnValidQueryPacketReceived);
		if (!m_LANSession->Host(QueryPacketDelegate))
		{
			Result = ONLINE_FAIL;
		}
	}

	return Result;
}

void FOnlineSessionEOS::SetPermissionLevel(EOS_HSessionModification SessionModHandle, FNamedOnlineSession* Session)
{
	EOS_SessionModification_SetPermissionLevelOptions Options = { };
	Options.ApiVersion = EOS_SESSIONMODIFICATION_SETPERMISSIONLEVEL_API_LATEST;
	if (Session->SessionSettings.NumPublicConnections > 0)
	{
		Options.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised;
	}
	else if (Session->SessionSettings.bAllowJoinViaPresence)
	{
		Options.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_JoinViaPresence;
	}
	else
	{
		Options.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_InviteOnly;
	}

	UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_SessionModification_SetPermissionLevel() set to (%d)"), (int32)Options.PermissionLevel);

	EOS_EResult ResultCode = EOS_SessionModification_SetPermissionLevel(SessionModHandle, &Options);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionModification_SetPermissionLevel() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
	}
}

void FOnlineSessionEOS::SetJoinInProgress(EOS_HSessionModification SessionModHandle, FNamedOnlineSession* Session)
{
	EOS_SessionModification_SetJoinInProgressAllowedOptions Options = { };
	Options.ApiVersion = EOS_SESSIONMODIFICATION_SETJOININPROGRESSALLOWED_API_LATEST;
	Options.bAllowJoinInProgress = Session->SessionSettings.bAllowJoinInProgress ? EOS_TRUE : EOS_FALSE;

	UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_SessionModification_SetJoinInProgressAllowed() set to (%d)"), Options.bAllowJoinInProgress);

	EOS_EResult ResultCode = EOS_SessionModification_SetJoinInProgressAllowed(SessionModHandle, &Options);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionModification_SetJoinInProgressAllowed() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
	}
}

void FOnlineSessionEOS::AddAttribute(EOS_HSessionModification SessionModHandle, const EOS_Sessions_AttributeData* Attribute)
{
	EOS_SessionModification_AddAttributeOptions Options = { };
	Options.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
	Options.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
	Options.SessionAttribute = Attribute;

	UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_SessionModification_AddAttribute() named (%s) with value (%s)"), *FString(Attribute->Key), *MakeStringFromAttributeValue(Attribute));

	EOS_EResult ResultCode = EOS_SessionModification_AddAttribute(SessionModHandle, &Options);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionModification_AddAttribute() failed for attribute name (%s) with EOS result code (%s)"), *FString(Attribute->Key), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
	}
}

void FOnlineSessionEOS::SetAttributes(EOS_HSessionModification SessionModHandle, FNamedOnlineSession* Session)
{
	FAttributeOptions Opt1("NumPrivateConnections", Session->SessionSettings.NumPrivateConnections);
	AddAttribute(SessionModHandle, &Opt1);

	FAttributeOptions Opt2("NumPublicConnections", Session->SessionSettings.NumPublicConnections);
	AddAttribute(SessionModHandle, &Opt2);

	if (Session->OwningUserName.IsEmpty())
	{
		FString OwningPlayerName(TEXT("[DEDICATED] - "));

		FString UserName = FPlatformProcess::UserName();

		if (UserName.IsEmpty())
		{
			FString ComputerName = FPlatformProcess::ComputerName();
			OwningPlayerName += ComputerName;
		}
		else
		{
			OwningPlayerName += UserName;
		}

		Session->OwningUserName = OwningPlayerName;
	}

	FAttributeOptions Opt3("OwningPlayerName", TCHAR_TO_UTF8(*Session->OwningUserName));
	AddAttribute(SessionModHandle, &Opt3);

	FString NetId = Session->OwningUserId->ToString();
	FAttributeOptions Opt4("OwningNetId", TCHAR_TO_UTF8(*NetId));
	AddAttribute(SessionModHandle, &Opt4);

	FAttributeOptions Opt5("bAntiCheatProtected", Session->SessionSettings.bAntiCheatProtected);
	AddAttribute(SessionModHandle, &Opt5);

	FAttributeOptions Opt6("bUsesStats", Session->SessionSettings.bUsesStats);
	AddAttribute(SessionModHandle, &Opt6);

	FAttributeOptions Opt7("bIsDedicated", Session->SessionSettings.bIsDedicated);
	AddAttribute(SessionModHandle, &Opt7);

	FAttributeOptions Opt8("BuildUniqueId", Session->SessionSettings.BuildUniqueId);
	AddAttribute(SessionModHandle, &Opt8);

	if (auto SessionSetting = Session->SessionSettings.Settings.Find("SESSION_NAME"))
	{
		FString CustomSessionName;
		SessionSetting->Data.GetValue(CustomSessionName);

		if (CustomSessionName.Len() > 0)
		{
			FAttributeOptions Opt9("SESSION_NAME", TCHAR_TO_UTF8(*CustomSessionName));
			AddAttribute(SessionModHandle, &Opt9);
		}
	}

	// Add all of the session settings
	for (FSessionSettings::TConstIterator It(Session->SessionSettings.Settings); It; ++It)
	{
		const FName KeyName = It.Key();
		const FOnlineSessionSetting& Setting = It.Value();

		// Skip unsupported types or non session advertised settings
		if (Setting.AdvertisementType < EOnlineDataAdvertisementType::ViaOnlineService || !IsSessionSettingTypeSupported(Setting.Data.GetType()))
		{
			continue;
		}

		FString SettingName(TEXT("FOSS=") + KeyName.ToString());
		FAttributeOptions Attribute(TCHAR_TO_UTF8(*SettingName), Setting.Data);

		AddAttribute(SessionModHandle, &Attribute);
	}
}

struct FBeginMetricsOptions :
	public EOS_Metrics_BeginPlayerSessionOptions
{
	char DisplayNameAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];
	char ServerIpAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];
	char SessionIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];
	char ExternalIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];

	FBeginMetricsOptions() :
		EOS_Metrics_BeginPlayerSessionOptions()
	{
		ApiVersion = EOS_METRICS_BEGINPLAYERSESSION_API_LATEST;
		GameSessionId = SessionIdAnsi;
		DisplayName = DisplayNameAnsi;
		ServerIp = ServerIpAnsi;
		AccountId.External = ExternalIdAnsi;
	}
};

void FOnlineSessionEOS::BeginSessionAnalytics(FNamedOnlineSession* Session)
{
	if (m_Subsystem)
	{
		int32 LocalUserNum = m_Subsystem->m_IdentityInterfacePtr->GetDefaultLocalUser();

		if (EOS_EpicAccountId_IsValid(m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(LocalUserNum)))
		{
			FOnlineUserPtr LocalUser = m_Subsystem->m_IdentityInterfacePtr->GetLocalOnlineUser(LocalUserNum);

			if (LocalUser.IsValid())
			{
				TSharedPtr<const FOnlineSessionInfoEOS> SessionInfoEOS = StaticCastSharedPtr<const FOnlineSessionInfoEOS>(Session->SessionInfo);

				FBeginMetricsOptions Options;
				FCStringAnsi::Strncpy(Options.ServerIpAnsi, TCHAR_TO_UTF8(*SessionInfoEOS->m_HostAddr->ToString(false)), EOS_OSS_STRING_BUFFER_LENGTH + 1);
				FString DisplayName = LocalUser->GetDisplayName();
				FCStringAnsi::Strncpy(Options.DisplayNameAnsi, TCHAR_TO_UTF8(*DisplayName), EOS_OSS_STRING_BUFFER_LENGTH + 1);
				Options.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_Epic;
				Options.AccountId.Epic = m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(LocalUserNum);

				EOS_EResult Result = EOS_Metrics_BeginPlayerSession(m_Subsystem->m_MetricsHandle, &Options);

				if (Result != EOS_EResult::EOS_Success)
				{
					UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Metrics_BeginPlayerSession() returned EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
				}
			}
		}
	}
}

void FOnlineSessionEOS::UpdatePresenceJoinInfo(int32 localUserNum, FNamedOnlineSession* session)
{
	UE_LOG_ONLINE_SESSION(Log, TEXT("UpdatePresenceJoinInfo() (%d) %s"), localUserNum, *session->SessionName.ToString());

	if (m_Subsystem && EOS_EpicAccountId_IsValid(m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(localUserNum)))
	{
		EOS_Presence_CreatePresenceModificationOptions CreatePresenceOptions = { };
		CreatePresenceOptions.ApiVersion = EOS_PRESENCE_CREATEPRESENCEMODIFICATION_API_LATEST;
		CreatePresenceOptions.LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(localUserNum);
		
		EOS_HPresenceModification PresenceHandle = NULL;
		EOS_Presence_CreatePresenceModification(m_Subsystem->m_PresenceHandle, &CreatePresenceOptions, &PresenceHandle);

		TSharedPtr<const FOnlineSessionInfoEOS> SessionInfoEOS = StaticCastSharedPtr<const FOnlineSessionInfoEOS>(session->SessionInfo);

		char JoinInfoAnsi[256];
		FCStringAnsi::Strncpy(JoinInfoAnsi, TCHAR_TO_UTF8(*SessionInfoEOS->m_HostAddr->ToString(true)), (SessionInfoEOS->m_HostAddr->ToString(true).Len() +1));

		EOS_PresenceModification_SetJoinInfoOptions m_SetJoinInfoOptions = { };
		m_SetJoinInfoOptions.ApiVersion = EOS_PRESENCEMODIFICATION_SETJOININFO_API_LATEST;
		m_SetJoinInfoOptions.JoinInfo = JoinInfoAnsi;

		EOS_PresenceModification_SetJoinInfo(PresenceHandle, &m_SetJoinInfoOptions);

		EOS_Presence_SetPresenceOptions SetPresenceOptions = { EOS_PRESENCE_SETPRESENCE_API_LATEST, m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(localUserNum), PresenceHandle };

		char RichTextAnsi[256];
		FCStringAnsi::Strncpy(RichTextAnsi, TCHAR_TO_UTF8(*session->SessionName.ToString()), (session->SessionName.ToString().Len() + 1));

		EOS_PresenceModification_SetRawRichTextOptions RichTextOptions = { };
		RichTextOptions.ApiVersion = EOS_PRESENCEMODIFICATION_SETRAWRICHTEXT_API_LATEST;
		RichTextOptions.RichText = RichTextAnsi;

		EOS_PresenceModification_SetRawRichText(PresenceHandle, &RichTextOptions);
		EOS_Presence_SetPresence(m_Subsystem->m_PresenceHandle, &SetPresenceOptions, nullptr, ([](const EOS_Presence_SetPresenceCallbackInfo* callbackData)
		{
			UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_Presence_SetPresence() %s"), *FString(EOS_EResult_ToString(callbackData->ResultCode)));
		}));
	}
}

template<typename BaseStruct>
struct TNamedSessionOptions :
	public BaseStruct
{
	char SessionNameAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];

	TNamedSessionOptions(const char* InSessionNameAnsi)
		: BaseStruct()
	{
		FCStringAnsi::Strncpy(SessionNameAnsi, InSessionNameAnsi, EOS_OSS_STRING_BUFFER_LENGTH + 1);
		this->SessionName = SessionNameAnsi;
	}
};

struct FSessionCreateOptions :
	public TNamedSessionOptions<EOS_Sessions_CreateSessionModificationOptions>
{
	FSessionCreateOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_CreateSessionModificationOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_CREATESESSIONMODIFICATION_API_LATEST;
		BucketId = BucketIdAnsi;
	}
};

uint32 FOnlineSessionEOS::CreateEOSSession(int32 HostingPlayerNum, FNamedOnlineSession* Session)
{
	check(Session != nullptr);

	EOS_HSessionModification SessionModHandle = nullptr;

	FSessionCreateOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));
	Options.MaxPlayers = Session->SessionSettings.NumPrivateConnections + Session->SessionSettings.NumPublicConnections;
	Options.LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId(HostingPlayerNum);
	Options.bPresenceEnabled = (Session->SessionSettings.bUsesPresence ||
		Session->SessionSettings.bAllowJoinViaPresence ||
		Session->SessionSettings.bAllowJoinViaPresenceFriendsOnly ||
		Session->SessionSettings.bAllowInvites) ? EOS_TRUE : EOS_FALSE;

	EOS_EResult ResultCode = EOS_Sessions_CreateSessionModification(m_Subsystem->m_SessionsHandle, &Options, &SessionModHandle);

	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_CreateSessionModification() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
		return ONLINE_FAIL;
	}

	Session->SessionState = EOnlineSessionState::Creating;

	FString HostAddr;
	// If we are not a dedicated server and are using p2p sockets, then we need to add a custom URL for connecting
	if (!bIsDedicatedServer && bIsUsingP2PSockets)
	{
		FInternetAddrEOS TempAddr(MakeStringFromProductUserId(Options.LocalUserId), Session->SessionName.ToString(), FURL::UrlConfig.DefaultPort);
		HostAddr = TempAddr.ToString(true);
		char HostAddrAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];
		FCStringAnsi::Strncpy(HostAddrAnsi, TCHAR_TO_UTF8(*HostAddr), EOS_OSS_STRING_BUFFER_LENGTH + 1);

		EOS_SessionModification_SetHostAddressOptions HostOptions = { };
		HostOptions.ApiVersion = EOS_SESSIONMODIFICATION_SETHOSTADDRESS_API_LATEST;
		// Expect URLs to look like "EOS:PUID:SocketName:Channel" and channel can be optional
		HostOptions.HostAddress = HostAddrAnsi;
		EOS_EResult HostResult = EOS_SessionModification_SetHostAddress(SessionModHandle, &HostOptions);
		UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_SessionModification_SetHostAddress(%s) returned (%s)"), *HostAddr, ANSI_TO_TCHAR(EOS_EResult_ToString(HostResult)));
	}
	else
	{
		// This is basically ignored
		HostAddr = TEXT("127.0.0.1");
	}
	Session->SessionInfo = MakeShareable(new FOnlineSessionInfoEOS(HostAddr, Session->SessionName.ToString(), nullptr));

	FUpdateSessionCallback* CallbackObj = new FUpdateSessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_UpdateSessionCallbackInfo* Data)
	{
		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_Sessions_OutOfSync;

		if (bWasSuccessful)
		{
			Session->SessionState = EOnlineSessionState::Pending;
			BeginSessionAnalytics(Session);
			RegisterLocalPlayers(Session);
		}
		else
		{
			Session->SessionState = EOnlineSessionState::NoSession;
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_UpdateSession() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}

		TriggerOnCreateSessionCompleteDelegates(Session->SessionName, bWasSuccessful);
	};



	return SharedSessionUpdate(SessionModHandle, Session, CallbackObj);
}

uint32 FOnlineSessionEOS::SharedSessionUpdate(EOS_HSessionModification SessionModHandle, FNamedOnlineSession* Session, FUpdateSessionCallback* Callback)
{
	// Set invite flags
	SetPermissionLevel(SessionModHandle, Session);
	// Set JIP flag
	SetJoinInProgress(SessionModHandle, Session);
	// Add any attributes for filtering by searchers
	SetAttributes(SessionModHandle, Session);

	// Commit the session changes
	EOS_Sessions_UpdateSessionOptions CreateOptions = { };
	CreateOptions.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
	CreateOptions.SessionModificationHandle = SessionModHandle;
	EOS_Sessions_UpdateSession(m_Subsystem->m_SessionsHandle, &CreateOptions, Callback, Callback->GetCallbackPtr());

	EOS_SessionModification_Release(SessionModHandle);

	return ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::StartSession(FName SessionName)
{
	uint32 Result = ONLINE_FAIL;
	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		// Can't start a match multiple times
		if (Session->SessionState == EOnlineSessionState::Pending ||
			Session->SessionState == EOnlineSessionState::Ended)
		{
			if (!Session->SessionSettings.bIsLANMatch)
			{
				Result = StartEOSSession(Session);
			}
			else
			{
				// If this lan match has join in progress disabled, shut down the beacon
				if (!Session->SessionSettings.bAllowJoinInProgress)
				{
					m_LANSession->StopLANSession();
				}
				Result = ONLINE_SUCCESS;
				Session->SessionState = EOnlineSessionState::InProgress;
			}
		}
		else
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't start an online session (%s) in state %s"),
				*SessionName.ToString(),
				EOnlineSessionState::ToString(Session->SessionState));
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't start an online game for session (%s) that hasn't been created"), *SessionName.ToString());
	}

	if (Result != ONLINE_IO_PENDING)
	{
		// Just trigger the delegate
		TriggerOnStartSessionCompleteDelegates(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
	}

	return Result == ONLINE_SUCCESS || Result == ONLINE_IO_PENDING;
}

struct FSessionStartOptions :
	public TNamedSessionOptions<EOS_Sessions_StartSessionOptions>
{
	FSessionStartOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_StartSessionOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_STARTSESSION_API_LATEST;
	}
};

typedef TEOSCallback<EOS_Sessions_OnStartSessionCallback, EOS_Sessions_StartSessionCallbackInfo> FStartSessionCallback;

uint32 FOnlineSessionEOS::StartEOSSession(FNamedOnlineSession* Session)
{
	Session->SessionState = EOnlineSessionState::Starting;

	FSessionStartOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));
	FStartSessionCallback* CallbackObj = new FStartSessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_StartSessionCallbackInfo* Data)
	{
		Session->SessionState = EOnlineSessionState::InProgress;

		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (!bWasSuccessful)
		{
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_StartSession() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		TriggerOnStartSessionCompleteDelegates(Session->SessionName, bWasSuccessful);
	};

	EOS_Sessions_StartSession(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData)
{
	int32 Result = ONLINE_FAIL;

	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		Session->SessionSettings = UpdatedSessionSettings;

		if (!Session->SessionSettings.bIsLANMatch)
		{
			Result = UpdateEOSSession(Session, UpdatedSessionSettings);
		}
		else
		{
			Result = ONLINE_SUCCESS;
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No session (%s) found for update!"), *SessionName.ToString());
	}

	if (Result != ONLINE_IO_PENDING)
	{
		TriggerOnUpdateSessionCompleteDelegates(SessionName, Result == ONLINE_SUCCESS);
	}

	return Result == ONLINE_SUCCESS || Result == ONLINE_IO_PENDING;
}

struct FSessionUpdateOptions :
	public TNamedSessionOptions<EOS_Sessions_UpdateSessionModificationOptions>
{
	FSessionUpdateOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_UpdateSessionModificationOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_UPDATESESSIONMODIFICATION_API_LATEST;
	}
};

uint32 FOnlineSessionEOS::UpdateEOSSession(FNamedOnlineSession* Session, FOnlineSessionSettings& UpdatedSessionSettings)
{
	if (Session->SessionState == EOnlineSessionState::Creating)
	{
		return ONLINE_IO_PENDING;
	}

	EOS_HSessionModification SessionModHandle = NULL;
	FSessionUpdateOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));

	EOS_EResult ResultCode = EOS_Sessions_UpdateSessionModification(m_Subsystem->m_SessionsHandle, &Options, &SessionModHandle);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_UpdateSessionModification() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
		return ONLINE_FAIL;
	}

	FUpdateSessionCallback* CallbackObj = new FUpdateSessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_UpdateSessionCallbackInfo* Data)
	{
		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_Sessions_OutOfSync;
		if (!bWasSuccessful)
		{
			Session->SessionState = EOnlineSessionState::NoSession;
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_UpdateSession() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		TriggerOnUpdateSessionCompleteDelegates(Session->SessionName, bWasSuccessful);
	};

	return SharedSessionUpdate(SessionModHandle, Session, CallbackObj);
}

bool FOnlineSessionEOS::EndSession(FName SessionName)
{
	uint32 Result = ONLINE_FAIL;

	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		// Can't end a match that isn't in progress
		if (Session->SessionState == EOnlineSessionState::InProgress)
		{
			if (!Session->SessionSettings.bIsLANMatch)
			{
				Result = EndEOSSession(Session);
			}
			else
			{
				// If the session should be advertised and the lan beacon was destroyed, recreate
				if (Session->SessionSettings.bShouldAdvertise &&
					!m_LANSession.IsValid() &&
					m_LANSession->LanBeacon == nullptr &&
					m_Subsystem->IsServer())
				{
					// Recreate the beacon
					Result = CreateLANSession(Session->HostingPlayerNum, Session);
				}
				else
				{
					Result = ONLINE_SUCCESS;
				}
			}
		}
		else
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't end session (%s) in state %s"),
				*SessionName.ToString(),
				EOnlineSessionState::ToString(Session->SessionState));
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't end an online game for session (%s) that hasn't been created"),
			*SessionName.ToString());
	}

	if (Result != ONLINE_IO_PENDING)
	{
		if (Session)
		{
			Session->SessionState = EOnlineSessionState::Ended;
		}

		TriggerOnEndSessionCompleteDelegates(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
	}

	return Result == ONLINE_SUCCESS || Result == ONLINE_IO_PENDING;
}

struct FSessionEndOptions :
	public TNamedSessionOptions<EOS_Sessions_EndSessionOptions>
{
	FSessionEndOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_EndSessionOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_ENDSESSION_API_LATEST;
	}
};

typedef TEOSCallback<EOS_Sessions_OnEndSessionCallback, EOS_Sessions_EndSessionCallbackInfo> FEndSessionCallback;

uint32 FOnlineSessionEOS::EndEOSSession(FNamedOnlineSession* Session)
{
	// Only called from EndSession/DestroySession and presumes only in InProgress state
	check(Session && Session->SessionState == EOnlineSessionState::InProgress);

	Session->SessionState = EOnlineSessionState::Ending;

	FSessionEndOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));
	FEndSessionCallback* CallbackObj = new FEndSessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_EndSessionCallbackInfo* Data)
	{
		Session->SessionState = EOnlineSessionState::Ended;

		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (!bWasSuccessful)
		{
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_EndSession() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		TriggerOnEndSessionCompleteDelegates(Session->SessionName, bWasSuccessful);
	};

	EOS_Sessions_EndSession(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate)
{
	uint32 Result = ONLINE_FAIL;

	// Find the session in question
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		if (Session->SessionState != EOnlineSessionState::Destroying)
		{
			if (!Session->SessionSettings.bIsLANMatch)
			{
				if (Session->SessionState == EOnlineSessionState::InProgress)
				{
					// Enqueue all the end session tasks first
					Result = EndEOSSession(Session);
				}

				Result = DestroyEOSSession(Session, CompletionDelegate);
			}
			else
			{
				if (m_LANSession.IsValid())
				{
					// Tear down the LAN beacon
					m_LANSession->StopLANSession();
					m_LANSession = nullptr;
				}

				Result = ONLINE_SUCCESS;
			}

			if (Result != ONLINE_IO_PENDING)
			{
				// The session info is no longer needed
				RemoveNamedSession(Session->SessionName);
				CompletionDelegate.ExecuteIfBound(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
				TriggerOnDestroySessionCompleteDelegates(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
			}
		}
		else
		{
			// Purposefully skip the delegate call as one should already be in flight
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Already in process of destroying session (%s)"), *SessionName.ToString());
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't destroy a null online session (%s)"), *SessionName.ToString());
		CompletionDelegate.ExecuteIfBound(SessionName, false);
		TriggerOnDestroySessionCompleteDelegates(SessionName, false);
	}

	return Result == ONLINE_SUCCESS || Result == ONLINE_IO_PENDING;
}

struct FEndMetricsOptions :
	public EOS_Metrics_EndPlayerSessionOptions
{
	char ExternalIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH + 1];

	FEndMetricsOptions() :
		EOS_Metrics_EndPlayerSessionOptions()
	{
		ApiVersion = EOS_METRICS_ENDPLAYERSESSION_API_LATEST;
		AccountId.External = ExternalIdAnsi;
	}
};

void FOnlineSessionEOS::EndSessionAnalytics()
{
	if (m_Subsystem)
	{
		int32 LocalUserNum = m_Subsystem->m_IdentityInterfacePtr->GetDefaultLocalUser();

		if (EOS_EpicAccountId_IsValid(m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(LocalUserNum)))
		{
			FOnlineUserPtr LocalUser = m_Subsystem->m_IdentityInterfacePtr->GetLocalOnlineUser(LocalUserNum);
			if (LocalUser.IsValid())
			{
				FEndMetricsOptions Options;
				Options.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_Epic;
				Options.AccountId.Epic = m_Subsystem->m_IdentityInterfacePtr->GetLocalEpicAccountId(LocalUserNum);

				EOS_EResult Result = EOS_Metrics_EndPlayerSession(m_Subsystem->m_MetricsHandle, &Options);
				if (Result != EOS_EResult::EOS_Success)
				{
					UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Metrics_EndPlayerSession() returned EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
				}
			}
		}
	}
}

struct FSessionDestroyOptions :
	public TNamedSessionOptions<EOS_Sessions_DestroySessionOptions>
{
	FSessionDestroyOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_DestroySessionOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_DESTROYSESSION_API_LATEST;
	}
};

typedef TEOSCallback<EOS_Sessions_OnDestroySessionCallback, EOS_Sessions_DestroySessionCallbackInfo> FDestroySessionCallback;

uint32 FOnlineSessionEOS::DestroyEOSSession(FNamedOnlineSession* Session, const FOnDestroySessionCompleteDelegate& CompletionDelegate)
{
	Session->SessionState = EOnlineSessionState::Destroying;

	FSessionDestroyOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));
	FDestroySessionCallback* CallbackObj = new FDestroySessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_DestroySessionCallbackInfo* Data)
	{
		EndSessionAnalytics();

		Session->SessionState = EOnlineSessionState::NoSession;
		FName SessionName = Session->SessionName;

		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (!bWasSuccessful)
		{
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_DestroySession() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		RemoveNamedSession(SessionName);
		TriggerOnDestroySessionCompleteDelegates(SessionName, bWasSuccessful);
	};

	EOS_Sessions_DestroySession(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId)
{
	return IsPlayerInSessionImpl(this, SessionName, UniqueId);
}

bool FOnlineSessionEOS::StartMatchmaking(const TArray< TSharedRef<const FUniqueNetId> >& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("StartMatchmaking is not supported on this platform. Use FindSessions or FindSessionById."));
	TriggerOnMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionEOS::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("CancelMatchmaking is not supported on this platform. Use CancelFindSessions."));
	TriggerOnCancelMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionEOS::CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("CancelMatchmaking is not supported on this platform. Use CancelFindSessions."));
	TriggerOnCancelMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionEOS::FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	uint32 Return = ONLINE_FAIL;

	// Don't start another search while one is in progress
	if (!m_CurrentSessionSearch.IsValid() || SearchSettings->SearchState != EOnlineAsyncTaskState::InProgress)
	{
		// LAN searching uses this as an approximation for ping so make sure to set it
		m_SessionSearchStartInSeconds = FPlatformTime::Seconds();

		// Free up previous results
		SearchSettings->SearchResults.Empty();
		// Copy the search pointer so we can keep it around
		m_CurrentSessionSearch = SearchSettings;

		// Check if its a LAN query
		if (SearchSettings->bIsLanQuery == false)
		{
			Return = FindEOSSession(SearchingPlayerNum, SearchSettings);
		}
		else
		{
			Return = FindLANSession();
		}

		if (Return == ONLINE_IO_PENDING)
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Ignoring game search request while another search is pending"));
		Return = ONLINE_IO_PENDING;
	}

	return Return == ONLINE_SUCCESS || Return == ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	// This function doesn't use the SearchingPlayerNum parameter, so passing in anything is fine.

	return FindSessions(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(SearchingPlayerId), SearchSettings);
}

bool FOnlineSessionEOS::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegates)
{
	FOnlineSessionSearchResult EmptyResult;
	CompletionDelegates.ExecuteIfBound(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(SearchingUserId), false, EmptyResult);
	return true;
}

void FOnlineSessionEOS::AddSearchAttribute(EOS_HSessionSearch SearchHandle, const EOS_Sessions_AttributeData* Attribute, EOS_EOnlineComparisonOp ComparisonOp)
{
	EOS_SessionSearch_SetParameterOptions Options = { };
	Options.ApiVersion = EOS_SESSIONSEARCH_SETPARAMETER_API_LATEST;
	Options.Parameter = Attribute;
	Options.ComparisonOp = ComparisonOp;

	EOS_EResult ResultCode = EOS_SessionSearch_SetParameter(SearchHandle, &Options);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionSearch_SetParameter() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
	}
}

void FOnlineSessionEOS::CopySearchResult(EOS_HSessionDetails SessionHandle, EOS_SessionDetails_Info* SessionInfo, FOnlineSession& OutSession)
{
	OutSession.SessionSettings.bAllowJoinInProgress = SessionInfo->Settings->bAllowJoinInProgress == EOS_TRUE;
	switch (SessionInfo->Settings->PermissionLevel)
	{
	case EOS_EOnlineSessionPermissionLevel::EOS_OSPF_InviteOnly:
	{
		OutSession.SessionSettings.bAllowInvites = true;
		OutSession.SessionSettings.bUsesPresence = 0;
		OutSession.SessionSettings.bAllowJoinViaPresence = 0;
		OutSession.SessionSettings.NumPrivateConnections = SessionInfo->Settings->NumPublicConnections;
		OutSession.NumOpenPrivateConnections = SessionInfo->NumOpenPublicConnections;
		break;
	}
	case EOS_EOnlineSessionPermissionLevel::EOS_OSPF_JoinViaPresence:
	{
		OutSession.SessionSettings.bAllowInvites = true;
		OutSession.SessionSettings.bUsesPresence = true;
		OutSession.SessionSettings.bAllowJoinViaPresence = true;
		OutSession.SessionSettings.NumPrivateConnections = SessionInfo->Settings->NumPublicConnections;
		OutSession.NumOpenPrivateConnections = SessionInfo->NumOpenPublicConnections;
		break;
	}
	case EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised:
	{
		OutSession.SessionSettings.bAllowInvites = true;
		OutSession.SessionSettings.bUsesPresence = true;
		OutSession.SessionSettings.bAllowJoinViaPresence = true;
		OutSession.SessionSettings.NumPublicConnections = SessionInfo->Settings->NumPublicConnections;
		OutSession.NumOpenPublicConnections = SessionInfo->NumOpenPublicConnections;
		break;
	}
	}

	CopyAttributes(SessionHandle, OutSession);
}

void FOnlineSessionEOS::CopyAttributes(EOS_HSessionDetails SessionHandle, FOnlineSession& OutSession)
{
	EOS_SessionDetails_GetSessionAttributeCountOptions CountOptions = { };
	CountOptions.ApiVersion = EOS_SESSIONDETAILS_GETSESSIONATTRIBUTECOUNT_API_LATEST;
	int32 Count = EOS_SessionDetails_GetSessionAttributeCount(SessionHandle, &CountOptions);

	for (int32 Index = 0; Index < Count; Index++)
	{
		EOS_SessionDetails_CopySessionAttributeByIndexOptions AttrOptions = { };
		AttrOptions.ApiVersion = EOS_SESSIONDETAILS_COPYSESSIONATTRIBUTEBYINDEX_API_LATEST;
		AttrOptions.AttrIndex = Index;

		EOS_SessionDetails_Attribute* Attribute = NULL;
		EOS_EResult ResultCode = EOS_SessionDetails_CopySessionAttributeByIndex(SessionHandle, &AttrOptions, &Attribute);
		if (ResultCode == EOS_EResult::EOS_Success)
		{
			FString Key = Attribute->Data->Key;
			if (Key == TEXT("NumPublicConnections"))
			{
				OutSession.SessionSettings.NumPublicConnections = Attribute->Data->Value.AsInt64;
			}
			else if (Key == TEXT("NumPrivateConnections"))
			{
				OutSession.SessionSettings.NumPrivateConnections = Attribute->Data->Value.AsInt64;
			}
			else if (Key == TEXT("bAntiCheatProtected"))
			{
				OutSession.SessionSettings.bAntiCheatProtected = Attribute->Data->Value.AsBool == EOS_TRUE;
			}
			else if (Key == TEXT("bUsesStats"))
			{
				OutSession.SessionSettings.bUsesStats = Attribute->Data->Value.AsBool == EOS_TRUE;
			}
			else if (Key == TEXT("bIsDedicated"))
			{
				OutSession.SessionSettings.bIsDedicated = Attribute->Data->Value.AsBool == EOS_TRUE;
			}
			else if (Key == TEXT("BuildUniqueId"))
			{
				OutSession.SessionSettings.BuildUniqueId = Attribute->Data->Value.AsInt64;
			}
			else if (Key == TEXT("OwningPlayerName"))
			{
				OutSession.OwningUserName = ANSI_TO_TCHAR(Attribute->Data->Value.AsUtf8);
			}
			else if (Key == TEXT("OwningNetId"))
			{
				OutSession.OwningUserId = MakeShareable(new FUniqueNetIdEOS(ANSI_TO_TCHAR(Attribute->Data->Value.AsUtf8)));
			}
			else if (Key.StartsWith(TEXT("FOSS=")))
			{
				FString KeyName;
				FParse::Value(*Key, TEXT("FOSS="), KeyName);
				if (KeyName.Len() > 0)
				{
					FOnlineSessionSetting Setting;
					switch (Attribute->Data->ValueType)
					{
					case EOS_ESessionAttributeType::EOS_SAT_Boolean:
					{
						Setting.Data.SetValue(Attribute->Data->Value.AsBool == EOS_TRUE);
						break;
					}
					case EOS_ESessionAttributeType::EOS_SAT_Int64:
					{
						Setting.Data.SetValue(static_cast<int64>(Attribute->Data->Value.AsInt64));
						break;
					}
					case EOS_ESessionAttributeType::EOS_SAT_Double:
					{
						Setting.Data.SetValue(Attribute->Data->Value.AsDouble);
						break;
					}
					case EOS_ESessionAttributeType::EOS_SAT_String:
					{
						Setting.Data.SetValue(ANSI_TO_TCHAR(Attribute->Data->Value.AsUtf8));
						break;
					}
					}
					OutSession.SessionSettings.Settings.Add(FName(KeyName), Setting);
				}
			}
		}

		EOS_SessionDetails_Attribute_Release(Attribute);
	}
}

void FOnlineSessionEOS::AddSearchResult(EOS_HSessionDetails SessionHandle, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	EOS_SessionDetails_Info* SessionInfo = nullptr;
	EOS_SessionDetails_CopyInfoOptions CopyOptions = { };
	CopyOptions.ApiVersion = EOS_SESSIONDETAILS_COPYINFO_API_LATEST;
	EOS_EResult CopyResult = EOS_SessionDetails_CopyInfo(SessionHandle, &CopyOptions, &SessionInfo);
	if (CopyResult == EOS_EResult::EOS_Success)
	{
		int32 Position = SearchSettings->SearchResults.AddZeroed();
		FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[Position];
		// This will set the host address and port
		SearchResult.Session.SessionInfo = MakeShareable(new FOnlineSessionInfoEOS(SessionInfo->HostAddress, SessionInfo->SessionId, SessionHandle));

		CopySearchResult(SessionHandle, SessionInfo, SearchResult.Session);

		EOS_SessionDetails_Info_Release(SessionInfo);
	}
}

typedef TEOSCallback<EOS_SessionSearch_OnFindCallback, EOS_SessionSearch_FindCallbackInfo> FFindSessionsCallback;

uint32 FOnlineSessionEOS::FindEOSSession(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	EOS_HSessionSearch SearchHandle = nullptr;
	EOS_Sessions_CreateSessionSearchOptions HandleOptions = { };
	HandleOptions.ApiVersion = EOS_SESSIONS_CREATESESSIONSEARCH_API_LATEST;
	HandleOptions.MaxSearchResults = FMath::Clamp(SearchSettings->MaxSearchResults, 0, EOS_SESSIONS_MAX_SEARCH_RESULTS);

	EOS_EResult ResultCode = EOS_Sessions_CreateSessionSearch(m_Subsystem->m_SessionsHandle, &HandleOptions, &SearchHandle);
	if (ResultCode != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_CreateSessionSearch() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(ResultCode)));
		return ONLINE_FAIL;
	}
	// Store our search handle for use/cleanup later
	m_CurrentSearchHandle = MakeShareable(new FSessionSearchEOS(SearchHandle));

	FAttributeOptions Opt1("NumPublicConnections", 1);
	AddSearchAttribute(SearchHandle, &Opt1, EOS_EOnlineComparisonOp::EOS_OCO_GREATERTHANOREQUAL);

	FAttributeOptions Opt2(EOS_SESSIONS_SEARCH_BUCKET_ID, BucketIdAnsi);
	AddSearchAttribute(SearchHandle, &Opt2, EOS_EOnlineComparisonOp::EOS_OCO_EQUAL);

	// Add the search settings
	for (FSearchParams::TConstIterator It(SearchSettings->QuerySettings.SearchParams); It; ++It)
	{
		const FName Key = It.Key();
		const FOnlineSessionSearchParam& SearchParam = It.Value();

		if (!IsSessionSettingTypeSupported(SearchParam.Data.GetType()))
		{
			continue;
		}

		if (Key == SETTING_MAPNAME)
		{
			// TODO: FIX SEARCH

			continue;
		}

		if (Key == SEARCH_DEDICATED_ONLY)
		{
			// TODO: FIX DEDICATED SEARCH

			continue;
		}

		if (Key == SEARCH_EMPTY_SERVERS_ONLY)
		{
			// TODO: FIX SEARCH

			continue;
		}
		if (Key == SEARCH_SECURE_SERVERS_ONLY)
		{
			// TODO: FIX SEARCH

			continue;
		}
		if (Key == SEARCH_PRESENCE)
		{
			// TODO: FIX SEARCH

			continue;
		}

		/*QuerySettings.Set(SETTING_MAPNAME, FString(), EOnlineComparisonOp::Equals);
		QuerySettings.Set(SEARCH_DEDICATED_ONLY, false, EOnlineComparisonOp::Equals);
		QuerySettings.Set(SEARCH_EMPTY_SERVERS_ONLY, false, EOnlineComparisonOp::Equals);
		QuerySettings.Set(SEARCH_SECURE_SERVERS_ONLY, false, EOnlineComparisonOp::Equals);*/


		UE_LOG_ONLINE_SESSION(Log, TEXT("Adding search param named (%s), (%s)"), *Key.ToString(), *SearchParam.ToString());

		FString ParamName(TEXT("FOSS=") + Key.ToString());
		FAttributeOptions Attribute(TCHAR_TO_UTF8(*ParamName), SearchParam.Data);

		AddSearchAttribute(SearchHandle, &Attribute, ToEOSSearchOp(SearchParam.ComparisonOp));
	}

	FFindSessionsCallback* CallbackObj = new FFindSessionsCallback();
	CallbackObj->m_CallbackLambda = [this, SearchSettings](const EOS_SessionSearch_FindCallbackInfo* Data)
	{
		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (bWasSuccessful)
		{
			EOS_SessionSearch_GetSearchResultCountOptions SearchResultOptions = { };
			SearchResultOptions.ApiVersion = EOS_SESSIONSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
			int32 NumSearchResults = EOS_SessionSearch_GetSearchResultCount(m_CurrentSearchHandle->SearchHandle, &SearchResultOptions);

			EOS_SessionSearch_CopySearchResultByIndexOptions IndexOptions = { };
			IndexOptions.ApiVersion = EOS_SESSIONSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
			for (int32 Index = 0; Index < NumSearchResults; Index++)
			{
				EOS_HSessionDetails SessionHandle = nullptr;
				IndexOptions.SessionIndex = Index;
				EOS_EResult Result = EOS_SessionSearch_CopySearchResultByIndex(m_CurrentSearchHandle->SearchHandle, &IndexOptions, &SessionHandle);
				if (Result == EOS_EResult::EOS_Success)
				{
					AddSearchResult(SessionHandle, SearchSettings);
				}
			}
			SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
		}
		else
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_SessionSearch_Find() failed with EOS result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		TriggerOnFindSessionsCompleteDelegates(bWasSuccessful);
	};

	SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;

	// Execute the search
	EOS_SessionSearch_FindOptions Options = { };
	Options.ApiVersion = EOS_SESSIONSEARCH_FIND_API_LATEST;
	Options.LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId(SearchingPlayerNum);
	EOS_SessionSearch_Find(SearchHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return ONLINE_IO_PENDING;
}

uint32 FOnlineSessionEOS::FindLANSession()
{
	uint32 Return = ONLINE_FAIL;

	if (!m_LANSession.IsValid())
	{
		m_LANSession = MakeShareable(new FLANSession());
	}

	// Recreate the unique identifier for this client
	GenerateNonce((uint8*)&m_LANSession->LanNonce, 8);

	FOnValidResponsePacketDelegate ResponseDelegate = FOnValidResponsePacketDelegate::CreateRaw(this, &FOnlineSessionEOS::OnValidResponsePacketReceived);
	FOnSearchingTimeoutDelegate TimeoutDelegate = FOnSearchingTimeoutDelegate::CreateRaw(this, &FOnlineSessionEOS::OnLANSearchTimeout);

	FNboSerializeToBufferEOS Packet(LAN_BEACON_MAX_PACKET_SIZE);
	m_LANSession->CreateClientQueryPacket(Packet, m_LANSession->LanNonce);
	if (m_LANSession->Search(Packet, ResponseDelegate, TimeoutDelegate))
	{
		Return = ONLINE_IO_PENDING;
	}

	if (Return == ONLINE_FAIL)
	{
		m_CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Failed;

		// Just trigger the delegate as having failed
		TriggerOnFindSessionsCompleteDelegates(false);
	}

	return Return;
}

bool FOnlineSessionEOS::CancelFindSessions()
{
	uint32 Return = ONLINE_FAIL;
	if (m_CurrentSessionSearch.IsValid() && m_CurrentSessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		// Make sure it's the right type
		if (m_CurrentSessionSearch->bIsLanQuery)
		{
			check(m_LANSession);
			Return = ONLINE_SUCCESS;
			m_LANSession->StopLANSession();
			m_CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Failed;
			m_CurrentSessionSearch = nullptr;
		}
		else
		{
			Return = ONLINE_SUCCESS;
			// NULLing out the object will prevent the async event from adding the results
			m_CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Failed;
			m_CurrentSessionSearch = nullptr;
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't cancel a search that isn't in progress"));
	}

	if (Return != ONLINE_IO_PENDING)
	{
		TriggerOnCancelFindSessionsCompleteDelegates(true);
	}

	return Return == ONLINE_SUCCESS || Return == ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::JoinSession(int32 PlayerNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
	uint32 Return = ONLINE_FAIL;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	// Don't join a session if already in one or hosting one
	if (Session == nullptr)
	{
		// Create a named session from the search result data
		Session = AddNamedSession(SessionName, DesiredSession.Session);
		Session->HostingPlayerNum = PlayerNum;

		// Create Internet or LAN match
		if (!Session->SessionSettings.bIsLANMatch)
		{
			if (DesiredSession.Session.SessionInfo.IsValid())
			{
				TSharedPtr<const FOnlineSessionInfoEOS> SearchSessionInfo = StaticCastSharedPtr<const FOnlineSessionInfoEOS>(DesiredSession.Session.SessionInfo);

				FOnlineSessionInfoEOS* NewSessionInfo = new FOnlineSessionInfoEOS(*SearchSessionInfo);
				Session->SessionInfo = MakeShareable(NewSessionInfo);

				Return = JoinEOSSession(PlayerNum, Session, &DesiredSession.Session);
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Warning, TEXT("Invalid session info on search result"), *SessionName.ToString());
			}
		}
		else
		{
			FOnlineSessionInfoEOS* NewSessionInfo = new FOnlineSessionInfoEOS();
			Session->SessionInfo = MakeShareable(NewSessionInfo);

			Return = JoinLANSession(PlayerNum, Session, &DesiredSession.Session);
		}

		if (Return != ONLINE_IO_PENDING)
		{
			if (Return != ONLINE_SUCCESS)
			{
				// Clean up the session info so we don't get into a confused state
				RemoveNamedSession(SessionName);
			}
			else
			{
				RegisterLocalPlayers(Session);
			}
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Session (%s) already exists, can't join twice"), *SessionName.ToString());
	}

	if (Return != ONLINE_IO_PENDING)
	{
		// Just trigger the delegate as having failed
		TriggerOnJoinSessionCompleteDelegates(SessionName, Return == ONLINE_SUCCESS ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
	}

	return Return == ONLINE_SUCCESS || Return == ONLINE_IO_PENDING;
}

bool FOnlineSessionEOS::JoinSession(const FUniqueNetId& SearchingUserId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
	return JoinSession(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(SearchingUserId), SessionName, DesiredSession);
}

struct FJoinSessionOptions :
	public TNamedSessionOptions<EOS_Sessions_JoinSessionOptions>
{
	FJoinSessionOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_JoinSessionOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_JOINSESSION_API_LATEST;
	}
};

typedef TEOSCallback<EOS_Sessions_OnJoinSessionCallback, EOS_Sessions_JoinSessionCallbackInfo> FJoinSessionCallback;

uint32 FOnlineSessionEOS::JoinEOSSession(int32 PlayerNum, FNamedOnlineSession* Session, const FOnlineSession* SearchSession)
{
	if (!Session->SessionInfo.IsValid())
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("Session (%s) has invalid session info"), *Session->SessionName.ToString());
		return ONLINE_FAIL;
	}
	EOS_ProductUserId ProductUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId(PlayerNum);
	if (ProductUserId == nullptr)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("Session (%s) invalid user id (%d)"), *Session->SessionName.ToString(), PlayerNum);
		return ONLINE_FAIL;
	}
	TSharedPtr<FOnlineSessionInfoEOS> EOSSessionInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);
	if (!EOSSessionInfo->m_SessionId.IsValid())
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("Session (%s) has invalid session id"), *Session->SessionName.ToString());
		return ONLINE_FAIL;
	}

	// Copy the session info over
	TSharedPtr<const FOnlineSessionInfoEOS> SearchSessionInfo = StaticCastSharedPtr<const FOnlineSessionInfoEOS>(SearchSession->SessionInfo);
	EOSSessionInfo->m_HostAddr = SearchSessionInfo->m_HostAddr->Clone();

	Session->SessionState = EOnlineSessionState::Pending;

	FJoinSessionCallback* CallbackObj = new FJoinSessionCallback();
	CallbackObj->m_CallbackLambda = [this, Session](const EOS_Sessions_JoinSessionCallbackInfo* Data)
	{
		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (bWasSuccessful)
		{
			BeginSessionAnalytics(Session);
		}
		else
		{
			UE_LOG_ONLINE_SESSION(Error, TEXT("EOS_Sessions_JoinSession() failed for session (%s) with EOS result code (%s)"), *Session->SessionName.ToString(), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
		TriggerOnJoinSessionCompleteDelegates(Session->SessionName, bWasSuccessful ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
	};

	FJoinSessionOptions Options(TCHAR_TO_UTF8(*Session->SessionName.ToString()));
	Options.LocalUserId = ProductUserId;
	Options.SessionHandle = EOSSessionInfo->m_SessionHandle;
	EOS_Sessions_JoinSession(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return ONLINE_IO_PENDING;
}

uint32 FOnlineSessionEOS::JoinLANSession(int32 PlayerNum, FNamedOnlineSession* Session, const FOnlineSession* SearchSession)
{
	uint32 Result = ONLINE_FAIL;
	Session->SessionState = EOnlineSessionState::Pending;

	if (Session->SessionInfo.IsValid())
	{
		// Copy the session info over
		TSharedPtr<const FOnlineSessionInfoEOS> SearchSessionInfo = StaticCastSharedPtr<const FOnlineSessionInfoEOS>(SearchSession->SessionInfo);
		TSharedPtr<FOnlineSessionInfoEOS> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);
		SessionInfo->m_HostAddr = SearchSessionInfo->m_HostAddr->Clone();
		Result = ONLINE_SUCCESS;
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Session (%s) has invalid session info"), *Session->SessionName.ToString());
	}

	return Result;
}

bool FOnlineSessionEOS::FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in EOS subsystem
	TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(LocalUserNum, false, EmptySearchResult);
	return false;
};

bool FOnlineSessionEOS::FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in EOS subsystem
	TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(LocalUserId), false, EmptySearchResult);
	return false;
}

bool FOnlineSessionEOS::FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& FriendList)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in EOS subsystem
	TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(m_Subsystem->m_IdentityInterfacePtr->GetLocalUserNumFromUniqueNetId(LocalUserId), false, EmptySearchResult);
	return false;
}

struct FSendSessionInviteOptions :
	public TNamedSessionOptions<EOS_Sessions_SendInviteOptions>
{
	FSendSessionInviteOptions(const char* InSessionNameAnsi) :
		TNamedSessionOptions<EOS_Sessions_SendInviteOptions>(InSessionNameAnsi)
	{
		ApiVersion = EOS_SESSIONS_SENDINVITE_API_LATEST;
	}
};

typedef TEOSCallback<EOS_Sessions_OnSendInviteCallback, EOS_Sessions_SendInviteCallbackInfo> FSendSessionInviteCallback;

bool FOnlineSessionEOS::SendSessionInvite(FName SessionName, EOS_ProductUserId SenderId, EOS_ProductUserId ReceiverId)
{
	FSendSessionInviteOptions Options(TCHAR_TO_UTF8(*SessionName.ToString()));
	Options.LocalUserId = SenderId;
	Options.TargetUserId = ReceiverId;

	FSendSessionInviteCallback* CallbackObj = new FSendSessionInviteCallback();
	CallbackObj->m_CallbackLambda = [this, SessionName](const EOS_Sessions_SendInviteCallbackInfo* Data)
	{
		bool bWasSuccessful = Data->ResultCode == EOS_EResult::EOS_Success;
		if (!bWasSuccessful)
		{
			UE_LOG_ONLINE_SESSION(Error, TEXT("SendSessionInvite() failed for session (%s) with EOS result code (%s)"), *SessionName.ToString(), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		}
	};

	EOS_Sessions_SendInvite(m_Subsystem->m_SessionsHandle, &Options, CallbackObj, CallbackObj->GetCallbackPtr());

	return true;
}

bool FOnlineSessionEOS::SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend)
{
	EOS_ProductUserId LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId(LocalUserNum);
	if (LocalUserId == nullptr)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("SendSessionInviteToFriend() failed due to user (%d) being not logged in"), (int32)LocalUserNum);
		return false;
	}
	EOS_ProductUserId TargetUserId = m_Subsystem->m_IdentityInterfacePtr->GetProductUserId(Friend);
	if (TargetUserId == nullptr)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("SendSessionInviteToFriend() failed due to target user (%s) having not played this game"), *Friend.ToDebugString());
		return false;
	}

	return SendSessionInvite(SessionName, LocalUserId, TargetUserId);
};

bool FOnlineSessionEOS::SendSessionInviteToFriend(const FUniqueNetId& LocalNetId, FName SessionName, const FUniqueNetId& Friend)
{
	EOS_ProductUserId LocalUserId = m_Subsystem->m_IdentityInterfacePtr->GetProductUserId(LocalNetId);
	if (LocalUserId == nullptr)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("SendSessionInviteToFriend() failed due to user (%s) being not logged in"), *LocalNetId.ToDebugString());
		return false;
	}
	EOS_ProductUserId TargetUserId = m_Subsystem->m_IdentityInterfacePtr->GetProductUserId(Friend);
	if (TargetUserId == nullptr)
	{
		UE_LOG_ONLINE_SESSION(Error, TEXT("SendSessionInviteToFriend() failed due to target user (%s) having not played this game"), *Friend.ToDebugString());
		return false;
	}

	return SendSessionInvite(SessionName, LocalUserId, TargetUserId);
}

bool FOnlineSessionEOS::SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends)
{
	for (TSharedRef<const FUniqueNetId> NetId : Friends)
	{
		if (SendSessionInviteToFriend(LocalUserNum, SessionName, *NetId) == false)
		{
			return false;
		}
	}
	return true;
};

bool FOnlineSessionEOS::SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends)
{
	for (TSharedRef<const FUniqueNetId> NetId : Friends)
	{
		if (SendSessionInviteToFriend(LocalUserId, SessionName, *NetId) == false)
		{
			return false;
		}
	}
	return true;
}

bool FOnlineSessionEOS::PingSearchResults(const FOnlineSessionSearchResult& SearchResult)
{
	return false;
}

/** Get a resolved connection string from a session info */
static bool GetConnectStringFromSessionInfo(TSharedPtr<FOnlineSessionInfoEOS>& SessionInfo, FString& ConnectInfo, int32 PortOverride = 0)
{
	if (!SessionInfo.IsValid() || !SessionInfo->m_HostAddr.IsValid())
	{
		return false;
	}

	if (PortOverride != 0)
	{
		ConnectInfo = FString::Printf(TEXT("%s:%d"), *SessionInfo->m_HostAddr->ToString(false), PortOverride);
	}
	else if (SessionInfo->m_EOSAddress.Len() > 0)
	{
		ConnectInfo = SessionInfo->m_EOSAddress;
	}
	else
	{
		ConnectInfo = SessionInfo->m_HostAddr->ToString(true);
	}

	return true;
}

bool FOnlineSessionEOS::GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType)
{
	bool bSuccess = false;
	// Find the session
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session != nullptr)
	{
		TSharedPtr<FOnlineSessionInfoEOS> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);
		if (PortType == NAME_BeaconPort)
		{
			int32 BeaconListenPort = GetBeaconPortFromSessionSettings(Session->SessionSettings);
			bSuccess = GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo, BeaconListenPort);
		}
		else if (PortType == NAME_GamePort)
		{
			bSuccess = GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo);
		}

		if (!bSuccess)
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Invalid session info for session %s in GetResolvedConnectString()"), *SessionName.ToString());
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning,
			TEXT("Unknown session name (%s) specified to GetResolvedConnectString()"),
			*SessionName.ToString());
	}

	return bSuccess;
}

bool FOnlineSessionEOS::GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo)
{
	bool bSuccess = false;
	if (SearchResult.Session.SessionInfo.IsValid())
	{
		TSharedPtr<FOnlineSessionInfoEOS> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(SearchResult.Session.SessionInfo);

		if (PortType == NAME_BeaconPort)
		{
			int32 BeaconListenPort = GetBeaconPortFromSessionSettings(SearchResult.Session.SessionSettings);
			bSuccess = GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo, BeaconListenPort);

		}
		else if (PortType == NAME_GamePort)
		{
			bSuccess = GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo);
		}
	}

	if (!bSuccess || ConnectInfo.IsEmpty())
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Invalid session info in search result to GetResolvedConnectString()"));
	}

	return bSuccess;
}

FOnlineSessionSettings* FOnlineSessionEOS::GetSessionSettings(FName SessionName)
{
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		return &Session->SessionSettings;
	}
	return nullptr;
}

void FOnlineSessionEOS::RegisterLocalPlayers(FNamedOnlineSession* Session)
{
}

bool FOnlineSessionEOS::RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited)
{
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShareable(new FUniqueNetIdEOS(PlayerId)));
	return RegisterPlayers(SessionName, Players, bWasInvited);
}

bool FOnlineSessionEOS::RegisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players, bool bWasInvited)
{
	bool bSuccess = false;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	TArray<EOS_ProductUserId> m_PlayersToRegister;
	if (Session)
	{
		bSuccess = true;

		for (int32 PlayerIdx = 0; PlayerIdx < Players.Num(); PlayerIdx++)
		{
			const TSharedRef<const FUniqueNetId>& PlayerId = Players[PlayerIdx];

			FUniqueNetIdMatcher PlayerMatch(*PlayerId);
			if (Session->RegisteredPlayers.IndexOfByPredicate(PlayerMatch) == INDEX_NONE)
			{
				Session->RegisteredPlayers.Add(PlayerId);

				TSharedPtr<FUniqueNetIdEOS> m_NetId = MakeShareable(new FUniqueNetIdEOS(Players[PlayerIdx]->ToString()));
				m_PlayersToRegister.Add(EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*m_NetId->m_ProductUserIdStr)));

				// update number of open connections
				if (Session->NumOpenPublicConnections > 0)
				{
					Session->NumOpenPublicConnections--;
				}
				else if (Session->NumOpenPrivateConnections > 0)
				{
					Session->NumOpenPrivateConnections--;
				}
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Log, TEXT("Player %s already registered in session %s"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No game present to join for session (%s)"), *SessionName.ToString());
	}

	auto m_Callback = [](const EOS_Sessions_RegisterPlayersCallbackInfo* Data)
	{
		UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_Sessions_RegisterPlayers() Code: (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
	};

	EOS_Sessions_RegisterPlayersOptions m_RegisterPlayersOptions = { EOS_SESSIONS_REGISTERPLAYERS_API_LATEST, TCHAR_TO_UTF8(*SessionName.ToString()), m_PlayersToRegister.GetData(), static_cast<uint32_t>(m_PlayersToRegister.Num()) };
	EOS_Sessions_RegisterPlayers(m_Subsystem->m_SessionsHandle, &m_RegisterPlayersOptions, nullptr, m_Callback);

	TriggerOnRegisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return bSuccess;
}

bool FOnlineSessionEOS::UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId)
{
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShareable(new FUniqueNetIdEOS(PlayerId)));
	return UnregisterPlayers(SessionName, Players);
}

bool FOnlineSessionEOS::UnregisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players)
{
	bool bSuccess = true;

	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	TArray<EOS_ProductUserId> m_PlayersToRegister;

	if (Session)
	{
		for (int32 PlayerIdx = 0; PlayerIdx < Players.Num(); PlayerIdx++)
		{
			const TSharedRef<const FUniqueNetId>& PlayerId = Players[PlayerIdx];

			FUniqueNetIdMatcher PlayerMatch(*PlayerId);
			int32 RegistrantIndex = Session->RegisteredPlayers.IndexOfByPredicate(PlayerMatch);
			if (RegistrantIndex != INDEX_NONE)
			{
				Session->RegisteredPlayers.RemoveAtSwap(RegistrantIndex);
				TSharedRef<FUniqueNetIdEOS> m_NetId = MakeShareable(new FUniqueNetIdEOS(Players[PlayerIdx]->ToString()));

				m_PlayersToRegister.Add(EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*m_NetId->m_ProductUserIdStr)));

				// update number of open connections
				if (Session->NumOpenPublicConnections < Session->SessionSettings.NumPublicConnections)
				{
					Session->NumOpenPublicConnections++;
				}
				else if (Session->NumOpenPrivateConnections < Session->SessionSettings.NumPrivateConnections)
				{
					Session->NumOpenPrivateConnections++;
				}
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Warning, TEXT("Player %s is not part of session (%s)"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No game present to leave for session (%s)"), *SessionName.ToString());
		bSuccess = false;
	}

	auto m_Callback = [](const EOS_Sessions_UnregisterPlayersCallbackInfo* Data)
	{
		UE_LOG_ONLINE_SESSION(Log, TEXT("EOS_Sessions_UnregisterPlayers() Code: (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
	};

	EOS_Sessions_UnregisterPlayersOptions m_RegisterPlayersOptions = { EOS_SESSIONS_REGISTERPLAYERS_API_LATEST, TCHAR_TO_UTF8(*SessionName.ToString()), m_PlayersToRegister.GetData(), static_cast<uint32_t>(m_PlayersToRegister.Num()) };
	EOS_Sessions_UnregisterPlayers(m_Subsystem->m_SessionsHandle, &m_RegisterPlayersOptions, nullptr, m_Callback);

	TriggerOnUnregisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return bSuccess;
}

void FOnlineSessionEOS::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);
	TickLanTasks(DeltaTime);
}

void FOnlineSessionEOS::TickLanTasks(float DeltaTime)
{
	if (m_LANSession.IsValid() &&
		m_LANSession->GetBeaconState() > ELanBeaconState::NotUsingLanBeacon)
	{
		m_LANSession->Tick(DeltaTime);
	}
}

void FOnlineSessionEOS::AppendSessionToPacket(FNboSerializeToBufferEOS& Packet, FOnlineSession* Session)
{
	/** Owner of the session */
	Packet << *StaticCastSharedPtr<const FUniqueNetIdEOS>(Session->OwningUserId)
		<< Session->OwningUserName
		<< Session->NumOpenPrivateConnections
		<< Session->NumOpenPublicConnections;

	// Try to get the actual port the netdriver is using
	SetPortFromNetDriver(*m_Subsystem, Session->SessionInfo);

	// Write host info (host addr, session id, and key)
	Packet << *StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);

	// Now append per game settings
	AppendSessionSettingsToPacket(Packet, &Session->SessionSettings);
}

void FOnlineSessionEOS::AppendSessionSettingsToPacket(FNboSerializeToBufferEOS& Packet, FOnlineSessionSettings* SessionSettings)
{
#if DEBUG_LAN_BEACON
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Sending session settings to client"));
#endif 

	// Members of the session settings class
	Packet << SessionSettings->NumPublicConnections
		<< SessionSettings->NumPrivateConnections
		<< (uint8)SessionSettings->bShouldAdvertise
		<< (uint8)SessionSettings->bIsLANMatch
		<< (uint8)SessionSettings->bIsDedicated
		<< (uint8)SessionSettings->bUsesStats
		<< (uint8)SessionSettings->bAllowJoinInProgress
		<< (uint8)SessionSettings->bAllowInvites
		<< (uint8)SessionSettings->bUsesPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresenceFriendsOnly
		<< (uint8)SessionSettings->bAntiCheatProtected
		<< SessionSettings->BuildUniqueId;

	// First count number of advertised keys
	int32 NumAdvertisedProperties = 0;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			NumAdvertisedProperties++;
		}
	}

	// Add count of advertised keys and the data
	Packet << (int32)NumAdvertisedProperties;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			Packet << It.Key();
			Packet << Setting;
#if DEBUG_LAN_BEACON
			UE_LOG_ONLINE_SESSION(Verbose, TEXT("%s"), *Setting.ToString());
#endif
		}
	}
}

void FOnlineSessionEOS::OnValidQueryPacketReceived(uint8* PacketData, int32 PacketLength, uint64 ClientNonce)
{
	// Iterate through all registered sessions and respond for each LAN match
	FScopeLock ScopeLock(&m_SessionLock);
	for (int32 SessionIndex = 0; SessionIndex < m_Sessions.Num(); SessionIndex++)
	{
		FNamedOnlineSession* Session = &m_Sessions[SessionIndex];

		// Don't respond to query if the session is not a joinable LAN match.
		if (Session != nullptr)
		{
			const FOnlineSessionSettings& Settings = Session->SessionSettings;

			const bool bIsMatchInProgress = Session->SessionState == EOnlineSessionState::InProgress;

			const bool bIsMatchJoinable = Settings.bIsLANMatch &&
				(!bIsMatchInProgress || Settings.bAllowJoinInProgress) &&
				Settings.NumPublicConnections > 0;

			if (bIsMatchJoinable)
			{
				FNboSerializeToBufferEOS Packet(LAN_BEACON_MAX_PACKET_SIZE);
				// Create the basic header before appending additional information
				m_LANSession->CreateHostResponsePacket(Packet, ClientNonce);

				// Add all the session details
				AppendSessionToPacket(Packet, Session);

				// Broadcast this response so the client can see us
				m_LANSession->BroadcastPacket(Packet, Packet.GetByteCount());
			}
		}
	}
}

void FOnlineSessionEOS::ReadSessionFromPacket(FNboSerializeFromBufferEOS& Packet, FOnlineSession* Session)
{
#if DEBUG_LAN_BEACON
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Reading session information from server"));
#endif

	/** Owner of the session */
	FUniqueNetIdEOS* UniqueId = new FUniqueNetIdEOS;
	Packet >> *UniqueId
		>> Session->OwningUserName
		>> Session->NumOpenPrivateConnections
		>> Session->NumOpenPublicConnections;

	Session->OwningUserId = MakeShareable(UniqueId);

	// Allocate and read the connection data
	FOnlineSessionInfoEOS* EOSSessionInfo = new FOnlineSessionInfoEOS();
	EOSSessionInfo->m_HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Packet >> *EOSSessionInfo;
	Session->SessionInfo = MakeShareable(EOSSessionInfo);

	// Read any per object data using the server object
	ReadSettingsFromPacket(Packet, Session->SessionSettings);
}

void FOnlineSessionEOS::ReadSettingsFromPacket(FNboSerializeFromBufferEOS& Packet, FOnlineSessionSettings& SessionSettings)
{
#if DEBUG_LAN_BEACON
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Reading game settings from server"));
#endif

	// Clear out any old settings
	SessionSettings.Settings.Empty();

	// Members of the session settings class
	Packet >> SessionSettings.NumPublicConnections
		>> SessionSettings.NumPrivateConnections;
	uint8 Read = 0;
	// Read all the bools as bytes
	Packet >> Read;
	SessionSettings.bShouldAdvertise = !!Read;
	Packet >> Read;
	SessionSettings.bIsLANMatch = !!Read;
	Packet >> Read;
	SessionSettings.bIsDedicated = !!Read;
	Packet >> Read;
	SessionSettings.bUsesStats = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinInProgress = !!Read;
	Packet >> Read;
	SessionSettings.bAllowInvites = !!Read;
	Packet >> Read;
	SessionSettings.bUsesPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = !!Read;
	Packet >> Read;
	SessionSettings.bAntiCheatProtected = !!Read;

	// BuildId
	Packet >> SessionSettings.BuildUniqueId;

	// Now read the contexts and properties from the settings class
	int32 NumAdvertisedProperties = 0;
	// First, read the number of advertised properties involved, so we can presize the array
	Packet >> NumAdvertisedProperties;
	if (Packet.HasOverflow() == false)
	{
		FName Key;
		// Now read each context individually
		for (int32 Index = 0;
			Index < NumAdvertisedProperties && Packet.HasOverflow() == false;
			Index++)
		{
			FOnlineSessionSetting Setting;
			Packet >> Key;
			Packet >> Setting;
			SessionSettings.Set(Key, Setting);

#if DEBUG_LAN_BEACON
			UE_LOG_ONLINE_SESSION(Verbose, TEXT("%s"), *Setting->ToString());
#endif
		}
	}

	// If there was an overflow, treat the string settings/properties as broken
	if (Packet.HasOverflow())
	{
		SessionSettings.Settings.Empty();
		UE_LOG_ONLINE_SESSION(Verbose, TEXT("Packet overflow detected in ReadGameSettingsFromPacket()"));
	}
}

void FOnlineSessionEOS::OnValidResponsePacketReceived(uint8* PacketData, int32 PacketLength)
{
	// Create an object that we'll copy the data to
	FOnlineSessionSettings NewServer;
	if (m_CurrentSessionSearch.IsValid())
	{
		// Add space in the search results array
		FOnlineSessionSearchResult* NewResult = new (m_CurrentSessionSearch->SearchResults) FOnlineSessionSearchResult();
		// this is not a correct ping, but better than nothing
		NewResult->PingInMs = static_cast<int32>((FPlatformTime::Seconds() - m_SessionSearchStartInSeconds) * 1000);

		FOnlineSession* NewSession = &NewResult->Session;

		// Prepare to read data from the packet
		FNboSerializeFromBufferEOS Packet(PacketData, PacketLength);

		ReadSessionFromPacket(Packet, NewSession);

		// NOTE: we don't notify until the timeout happens
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Failed to create new online game settings object"));
	}
}

void FOnlineSessionEOS::OnLANSearchTimeout()
{
	// See if there were any sessions that were marked as hosting before the search started
	bool bWasHosting = false;

	{
		FScopeLock ScopeLock(&m_SessionLock);
		for (int32 SessionIdx = 0; SessionIdx < m_Sessions.Num(); SessionIdx++)
		{
			FNamedOnlineSession& Session = m_Sessions[SessionIdx];
			if (Session.SessionSettings.bShouldAdvertise &&
				Session.SessionSettings.bIsLANMatch &&
				m_Subsystem->IsServer())
			{
				bWasHosting = true;
				break;
			}
		}
	}

	if (bWasHosting)
	{
		FOnValidQueryPacketDelegate QueryPacketDelegate = FOnValidQueryPacketDelegate::CreateRaw(this, &FOnlineSessionEOS::OnValidQueryPacketReceived);
		// Maintain lan beacon if there was a session that was marked as hosting
		if (m_LANSession->Host(QueryPacketDelegate))
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Failed to restart hosted LAN session after search completion"));
		}
	}
	else
	{
		// Stop future timeouts since we aren't searching any more
		m_LANSession->StopLANSession();
	}

	if (m_CurrentSessionSearch.IsValid())
	{
		if (m_CurrentSessionSearch->SearchResults.Num() > 0)
		{
			// Allow game code to sort the servers
			m_CurrentSessionSearch->SortSearchResults();
		}

		m_CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Done;

		m_CurrentSessionSearch = nullptr;
	}

	// Trigger the delegate as complete
	TriggerOnFindSessionsCompleteDelegates(true);
}

int32 FOnlineSessionEOS::GetNumSessions()
{
	FScopeLock ScopeLock(&m_SessionLock);
	return m_Sessions.Num();
}

void FOnlineSessionEOS::DumpSessionState()
{
	FScopeLock ScopeLock(&m_SessionLock);

	for (int32 SessionIdx = 0; SessionIdx < m_Sessions.Num(); SessionIdx++)
	{
		DumpNamedSession(&m_Sessions[SessionIdx]);
	}
}

void FOnlineSessionEOS::RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(PlayerId, EOnJoinSessionCompleteResult::Success);
}

void FOnlineSessionEOS::UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(PlayerId, true);
}

void FOnlineSessionEOS::SetPortFromNetDriver(const FOnlineSubsystemEOS& Subsystem, const TSharedPtr<FOnlineSessionInfo>& SessionInfo)
{
	auto NetDriverPort = GetPortFromNetDriver(Subsystem.GetInstanceName());
	auto SessionInfoEOS = StaticCastSharedPtr<FOnlineSessionInfoEOS>(SessionInfo);
	if (SessionInfoEOS.IsValid() && SessionInfoEOS->m_HostAddr.IsValid())
	{
		SessionInfoEOS->m_HostAddr->SetPort(NetDriverPort);
	}
}

bool FOnlineSessionEOS::IsHost(const FNamedOnlineSession& Session) const
{
	if (m_Subsystem->IsDedicated())
	{
		return true;
	}

	TSharedPtr<const FUniqueNetId> UserId = m_Subsystem->m_IdentityInterfacePtr->GetUniquePlayerId(Session.HostingPlayerNum);
	return (UserId.IsValid() && (*UserId == *Session.OwningUserId));
}

TSharedPtr<const FUniqueNetId> FOnlineSessionEOS::CreateSessionIdFromString(const FString& SessionIdStr)
{
	TSharedPtr<const FUniqueNetId> SessionId;
	if (!SessionIdStr.IsEmpty())
	{
		SessionId = MakeShared<FUniqueNetIdEOS>(SessionIdStr);
	}
	return SessionId;
}
