/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Core/EOSCoreLibrary.h"
#include "EOSCoreModule.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "EOSCoreLogging.h"
#include <GameFramework/PlayerController.h>
#include <Engine/LocalPlayer.h>
#include "UniqueNetIdEOS.h"

FDelegateHandle UEOSCoreLibrary::s_LoginCompleteDelegate;

bool UEOSCoreLibrary::EOS_Initialized(UObject* worldContextObject)
{
	FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));
	
	if (Subsystem)
	{
		return Subsystem->m_EOSPlatformHandle != nullptr;
	}

	return false;
}

bool UEOSCoreLibrary::UpdateUniqueNetIdFromOSS(APlayerController* playerController)
{
	LogVerbose("");

	bool bResult = false;

	if (playerController)
	{
		if (playerController->PlayerState)
		{
			FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(playerController, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));

			if (Subsystem && Subsystem->GetIdentityInterface())
			{
				int32 ControllerId = playerController->GetLocalPlayer()->GetControllerId();

				LogVerbose("PlayerId: %d", ControllerId);

				if (ControllerId != INDEX_NONE)
				{
					if (TSharedPtr<const FUniqueNetId> NetId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(ControllerId))
					{
						playerController->PlayerState->SetUniqueId(NetId);
						playerController->GetLocalPlayer()->SetCachedUniqueNetId(NetId);

						bResult = true;
					}
					else
					{
						LogError("No NetId found for player %d", ControllerId);
					}
				}
				else
				{
					LogError("Invalid Player Controller Id %d", ControllerId);
				}
			}
			else
			{
				LogError("No EOSCore subsystem found || Invalid identity interface");
			}
		}
		else
		{
			LogError("Invalid player state");
		}
	}
	else
	{
		LogError("Invalid player controller");
	}

	return bResult;
}

bool UEOSCoreLibrary::Login(UObject* worldContextObject, APlayerController* playerController, FString loginId, FString password, ELoginType authType, ECredentialsType credentialsType, FString additionalData)
{
	if (!worldContextObject)
	{
		LogError("No world context object found!");
		return false;
	}

	if (!playerController)
	{
		LogWarning("No player controller specified, will be unable to update NetId");
	}

	FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));
	check(Subsystem);

	if (!Subsystem)
	{
		LogError("EOS Subsystem not found");
		return false;
	}

	if (!Subsystem->GetIdentityInterface())
	{
		LogError("Subsystem IdentityInterface not valid");
		return false;
	}

	int32 ControllerId = 0;

	if (playerController)
	{
		ControllerId = playerController->GetLocalPlayer()->GetControllerId();

		// clear any old binds
		Subsystem->GetIdentityInterface()->ClearOnLoginCompleteDelegate_Handle(ControllerId, s_LoginCompleteDelegate);

		s_LoginCompleteDelegate= Subsystem->GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(ControllerId, FOnLoginCompleteDelegate::CreateLambda([worldContextObject, playerController, ControllerId](int32 Unused, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorStr)
		{
			FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));

			check(Subsystem);

			if (Subsystem)
			{
				if (bWasSuccessful)
				{
					if (playerController && playerController->PlayerState)
					{
						playerController->PlayerState->SetUniqueId(MakeShared<FUniqueNetIdEOS>(UserId));
						playerController->GetLocalPlayer()->SetCachedUniqueNetId(UserId.AsShared());
					}
				}

				Subsystem->GetIdentityInterface()->ClearOnLoginCompleteDelegate_Handle(ControllerId, s_LoginCompleteDelegate);
			}
			else
			{
				LogError("Invalid subsystem, this delegate should've been cleared!");
			}
			
		}));
	}

	FString Type = FString::Printf(TEXT("%d|%d|%s"), static_cast<int32>(authType), static_cast<int32>(credentialsType), *additionalData);

	FOnlineAccountCredentials Creds(Type, loginId, password);

	return Subsystem->GetIdentityInterface()->Login(ControllerId, Creds);
}

FDateTime UEOSCoreLibrary::FromUnixTimestamp(FString timestamp)
{
	return FDateTime::FromUnixTimestamp(FCString::Atoi64(*timestamp));
}

void UEOSCoreLibrary::BreakLobbyIdStruct(FEOSLobbyId id, FString& outId)
{
	outId = id.ToString();
}

FEOSLobbyId UEOSCoreLibrary::MakeLobbyIdStruct(FString lobbyId)
{
	return FEOSLobbyId(lobbyId);
}

void UEOSCoreLibrary::BreakAccessTokenStruct(FAccessToken token, FString& outToken)
{
	outToken = token.ToString();
}

FAccessToken UEOSCoreLibrary::MakeAccessTokenStruct(FString token)
{
	return FAccessToken(token);
}

void UEOSCoreLibrary::BreakLobbyInviteIdStruct(FLobbyInviteId inviteId, FString& outinviteId)
{
	outinviteId = inviteId.ToString();
}

FLobbyInviteId UEOSCoreLibrary::MakeLobbyInviteIdStruct(FString inviteId)
{
	return FLobbyInviteId(TCHAR_TO_UTF8(*inviteId));
}

void UEOSCoreLibrary::BreakUIEventIdStruct(FEOSUIEventId eventId, FString& outEventId)
{
	outEventId = LexToString(eventId.EventId);
}

bool UEOSCoreLibrary::IsLobbyIdValid(FEOSLobbyId id)
{
	return id.ToString().Len() > 0;
}

void UEOSCoreLibrary::EOS_Success(EOSResult status, ESuccessFail& result)
{
	if (status == EOSResult::EOS_Success)
		result = ESuccessFail::Success;
		else
		result = ESuccessFail::Fail;
}

bool UEOSCoreLibrary::IsProductUserIdIdenticalWith(FEOSProductUserId a, FEOSProductUserId b)
{
	if (EOSProductUserIdIsValid(a) && EOSProductUserIdIsValid(b))
	{
		FString aString;
		FString bString;

		EOSProductUserIdToString(a, aString);
		EOSProductUserIdToString(b, bString);

		return aString == bString;
	}
	else
	{
		return false;
	}
}

bool UEOSCoreLibrary::IsEpicAccountIdIdenticalWith(FEOSEpicAccountId a, FEOSEpicAccountId b)
{
	if (EOSEpicAccountIdIsValid(a) && EOSEpicAccountIdIsValid(b))
	{
		FString aString;
		FString bString;

		EOSEpicAccountIdToString(a, aString);
		EOSEpicAccountIdToString(b, bString);

		return aString == bString;
	}
	else
	{
		return false;
	}
}

bool UEOSCoreLibrary::IsLobbyIdIdenticalWith(FEOSLobbyId a, FEOSLobbyId b)
{
	return a.ToString() == b.ToString();
}

bool UEOSCoreLibrary::GetSessionAttributeBool(const FEOSSessionsAttributeData& data)
{
	return data.AttributeData.Value.AsBool>0;
}

FString UEOSCoreLibrary::GetSessionAttributeInt64(const FEOSSessionsAttributeData& data)
{
	return LexToString(data.AttributeData.Value.AsInt64);
}

FString UEOSCoreLibrary::GetSessionAttributeDouble(const FEOSSessionsAttributeData& data)
{
	return LexToString(data.AttributeData.Value.AsDouble);
}

FString UEOSCoreLibrary::GetSessionAttributeString(const FEOSSessionsAttributeData& data)
{
	return data.AttributeData.Value.AsUtf8;
}

bool UEOSCoreLibrary::GetLobbyAttributeBool(const FEOSLobbyAttributeData& data)
{
	return data.AttributeData.Value.AsBool>0;
}

FString UEOSCoreLibrary::GetLobbyAttributeInt64(const FEOSLobbyAttributeData& data)
{
	return LexToString(data.AttributeData.Value.AsInt64);
}

FString UEOSCoreLibrary::GetLobbyAttributeDouble(const FEOSLobbyAttributeData& data)
{
	return LexToString(data.AttributeData.Value.AsDouble);
}

FString UEOSCoreLibrary::GetLobbyAttributeString(const FEOSLobbyAttributeData& data)
{
	return data.AttributeData.Value.AsUtf8;
}

FBucketId UEOSCoreLibrary::MakeBucketId(FString string)
{
	return FBucketId(TCHAR_TO_UTF8(*string));
}

void UEOSCoreLibrary::BreakBucketId(FBucketId bucket, FString& string)
{
	string = bucket.ToString();
}

FSessionInviteId UEOSCoreLibrary::MakeSessionsInviteId(FString string)
{
	return FSessionInviteId(TCHAR_TO_UTF8(*string));
}

void UEOSCoreLibrary::BreakSessionsInviteId(FSessionInviteId id, FString& string)
{
	string = id.ToString();
}

FString UEOSCoreLibrary::EOSEResultToString(EOSResult result)
{
	return EOS_EResult_ToString(static_cast<EOS_EResult>(result));
}

bool UEOSCoreLibrary::EOSEResultIsOperationComplete(EOSResult result)
{
	return EOS_EResult_IsOperationComplete(static_cast<EOS_EResult>(result)) > 0;
}

EOSResult UEOSCoreLibrary::EOSByteArrayToString(const TArray<uint8> array, FString& outString)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	TArray<char> Buffer;
	Buffer.SetNum(2048);
	uint32_t BufferSize = 2048;

	Result = EOSHelpers::ToEOSCoreResult(EOS_ByteArray_ToString(array.GetData(), array.Num(), Buffer.GetData(), &BufferSize));
	
	if (Result == EOSResult::EOS_Success)
	{
		outString = UTF8_TO_TCHAR(Buffer.GetData());
	}

	return Result;
}

bool UEOSCoreLibrary::EOSEpicAccountIdIsValid(FEOSEpicAccountId id)
{
	if (id.ToString().Len() > 0)
		return EOS_EpicAccountId_IsValid(id) != EOS_FALSE;
	
	return false;
}

EOSResult UEOSCoreLibrary::EOSEpicAccountIdToString(FEOSEpicAccountId id, FString& string)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (EOSEpicAccountIdIsValid(id))
	{
		char Buffer[EOS_EPICACCOUNTID_MAX_LENGTH+1];
		int32_t BufferLen = EOS_EPICACCOUNTID_MAX_LENGTH+1;
		Result = EOSHelpers::ToEOSCoreResult(EOS_EpicAccountId_ToString(id, Buffer, &BufferLen));
		if (Result == EOSResult::EOS_Success)
		{
			string = Buffer;
		}
	}

	return Result;
}

FEOSEpicAccountId UEOSCoreLibrary::EOSEpicAccountIdFromString(FString string)
{
	return EOS_EpicAccountId_FromString(TCHAR_TO_UTF8(*string));
}

bool UEOSCoreLibrary::EOSProductUserIdIsValid(FEOSProductUserId id)
{
	if (id.ToString().Len() > 0)
		return EOS_ProductUserId_IsValid(id) != EOS_FALSE;

	return false;
}

EOSResult UEOSCoreLibrary::EOSProductUserIdToString(FEOSProductUserId id, FString& string)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (EOSProductUserIdIsValid(id))
	{
		char Buffer[EOS_EPICACCOUNTID_MAX_LENGTH+1];
		int32_t OutBuffer = EOS_EPICACCOUNTID_MAX_LENGTH+1;
		Result = EOSHelpers::ToEOSCoreResult(EOS_ProductUserId_ToString(id, Buffer, &OutBuffer));
		if (Result == EOSResult::EOS_Success)
		{
			string = Buffer;
		}
	}
	
	return Result;
}

FEOSProductUserId UEOSCoreLibrary::EOSProductUserIdFromString(FString string)
{
	return EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*string));
}

TArray<uint8> UEOSCoreLibrary::CoreStringToByte(FString string)
{
	TArray<uint8> Arr;
	Arr.SetNumUninitialized(string.Len());

	StringToBytes(string, Arr.GetData(), Arr.Num());

	return Arr;
}

FString UEOSCoreLibrary::CoreBytesToString(const TArray<uint8>& data)
{
	return BytesToString(data.GetData(), data.Num());
}

FEOSProductUserId UEOSCoreLibrary::GetCurrentProductId(UObject* worldContextObject, int32 userIndex /* = 0 */)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
		return EOS_Connect_GetLoggedInUserByIndex(UEOSCoreSubsystem::GetConnectHandle(worldContextObject), userIndex);

	return FEOSProductUserId();
}

FEOSEpicAccountId UEOSCoreLibrary::GetCurrentAccountId(UObject* worldContextObject, int32 userIndex /* = 0 */)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
		return EOS_Auth_GetLoggedInAccountByIndex(UEOSCoreSubsystem::GetAuthHandle(worldContextObject), userIndex);

	return FEOSEpicAccountId();
}

EOSResult UEOSCoreLibrary::EOSContinuanceTokenToString(FContinuanceToken token, FString& string)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	char Buffer[1024];
	int32_t OutBuffer = 1024;
	Result = EOSHelpers::ToEOSCoreResult(EOS_ContinuanceToken_ToString(token.Token, Buffer, &OutBuffer));
	if (Result == EOSResult::EOS_Success)
	{
		string = UTF8_TO_TCHAR(Buffer);
	}

	return Result;
}
