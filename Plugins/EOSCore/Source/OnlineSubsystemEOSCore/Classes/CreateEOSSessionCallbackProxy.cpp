/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "CreateEOSSessionCallbackProxy.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSessionEOS.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

//////////////////////////////////////////////////////////////////////////
// UCreateEOSSessionCallbackProxy

UCreateEOSSessionCallbackProxy::UCreateEOSSessionCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_CreateCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateCompleted))
	, m_StartCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartCompleted))
	, m_NumPublicConnections(1)
{}

UCreateEOSSessionCallbackProxy* UCreateEOSSessionCallbackProxy::Create_EOS_Session(UObject* worldContextObject, FString sessionName, int32 publicConnections)
{
	UCreateEOSSessionCallbackProxy* Proxy = NewObject<UCreateEOSSessionCallbackProxy>();
	Proxy->m_NumPublicConnections = publicConnections;
	Proxy->m_WorldContextObject = worldContextObject;
	Proxy->m_SessionName = sessionName;
	return Proxy;
}

void UCreateEOSSessionCallbackProxy::Activate()
{
	FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(m_WorldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));

	if (Subsystem && Subsystem->m_SessionInterfacePtr)
	{
		m_CreateCompleteDelegateHandle = Subsystem->m_SessionInterfacePtr->AddOnCreateSessionCompleteDelegate_Handle(m_CreateCompleteDelegate);

		FOnlineSessionSettings Settings;
		Settings.NumPublicConnections = m_NumPublicConnections;
		Settings.bShouldAdvertise = true;
		Settings.bAllowJoinInProgress = true;
		Settings.bIsLANMatch = false;
		Settings.bUsesPresence = true;
		Settings.bAllowJoinViaPresence = true;

		Settings.Settings.Add("SESSION_NAME", *m_SessionName);

		Subsystem->m_SessionInterfacePtr->CreateSession(0, NAME_GameSession, Settings);
		
		return;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
	}

	OnFailure.Broadcast();
}

void UCreateEOSSessionCallbackProxy::OnCreateCompleted(FName SessionName, bool bWasSuccessful)
{
	FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(m_WorldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));

	if (Subsystem && Subsystem->m_SessionInterfacePtr)
	{
		Subsystem->m_SessionInterfacePtr->ClearOnCreateSessionCompleteDelegate_Handle(m_CreateCompleteDelegateHandle);

		if (bWasSuccessful)
		{
			m_StartCompleteDelegateHandle = Subsystem->m_SessionInterfacePtr->AddOnStartSessionCompleteDelegate_Handle(m_StartCompleteDelegate);
			Subsystem->m_SessionInterfacePtr->StartSession(NAME_GameSession);

			return;
		}
	}

	if (!bWasSuccessful)
	{
		OnFailure.Broadcast();
	}
}

void UCreateEOSSessionCallbackProxy::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(m_WorldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM));

	if (Subsystem && Subsystem->m_SessionInterfacePtr)
	{
		Subsystem->m_SessionInterfacePtr->ClearOnStartSessionCompleteDelegate_Handle(m_StartCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
}
