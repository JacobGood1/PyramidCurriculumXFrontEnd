/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Chat/EOSChat.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreChat* UCoreChat::ChatObject = nullptr;

struct FChatMessageListener
{
	EOS_NotificationId ListenId;
	FOnChatMessageReceivedCallback Callback;
	EOS_P2PHandle* P2PHandle;
	FEOSProductUserId UserProductId;
};

static TArray<FChatMessageListener*> s_ChatListeners;

UCoreChat::UCoreChat()
{
	ChatObject = this;
}

void UCoreChat::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCoreChat::Deinitialize()
{
	for (FChatMessageListener* Element : s_ChatListeners)
	{
		if (Element->P2PHandle != nullptr)
			EOS_P2P_RemoveNotifyPeerConnectionRequest(Element->P2PHandle, Element->ListenId);
	}

	s_ChatListeners.Empty();

	Super::Deinitialize();
}

bool UCoreChat::Tick(float deltaTime)
{
	for (FChatMessageListener* Element : s_ChatListeners)
	{
		if (Element->P2PHandle == nullptr) continue;

		if (EOS_ProductUserId_IsValid(Element->UserProductId))
		{
			uint32_t NextPacketSize = 0;
			uint8_t Channel = (uint8)EEOSChannelType::DATA_CHAT;

			EOS_P2P_GetNextReceivedPacketSizeOptions GetPacketSizeOptions = { EOS_P2P_GETNEXTRECEIVEDPACKETSIZE_API_LATEST, Element->UserProductId, &Channel };
			EOS_P2P_GetNextReceivedPacketSize(Element->P2PHandle, &GetPacketSizeOptions, &NextPacketSize);

			if (NextPacketSize > 0)
			{
				EOS_P2P_ReceivePacketOptions GetPacketOptions = { EOS_P2P_RECEIVEPACKET_API_LATEST, Element->UserProductId, NextPacketSize, &Channel };

				EOS_ProductUserId OutPeerId;
				EOS_P2P_SocketId OutSocketId;
				uint8_t OutChannel = 0;
				TArray<uint8> DataArray;
				DataArray.SetNum(NextPacketSize);
				uint32_t OutBytesWritten = 0;

				if (EOS_P2P_ReceivePacket(Element->P2PHandle, &GetPacketOptions, &OutPeerId, &OutSocketId, &OutChannel, DataArray.GetData(), &OutBytesWritten) == EOS_EResult::EOS_Success)
				{
					LogVerbose("Received chat message of %d bytes", OutBytesWritten);

					FString ChatMessage = BytesToString(DataArray.GetData(), OutBytesWritten);
					Element->Callback.ExecuteIfBound(OutPeerId, Element->UserProductId, ChatMessage);
				}
			}
		}
	}

	return true;
}

void UCoreChat::SendChatMessage(UObject* worldContextObject, FEOSProductUserId localUserId, FEOSProductUserId target, FString message, EEOSEPacketReliability reliability /* = EEOSEPacketReliability::EOS_PR_UnreliableUnordered */)
{
	TArray<uint8> DataArray;
	DataArray.SetNum(message.Len());
	StringToBytes(message, DataArray.GetData(), DataArray.Num());

	EOS_P2P_SocketId SocketId = { };
	SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
	FCStringAnsi::Strncpy(SocketId.SocketName, "CHAT", 5);

	EOS_P2P_SendPacketOptions SendPacketOptions = { };
	SendPacketOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
	SendPacketOptions.LocalUserId = localUserId;
	SendPacketOptions.RemoteUserId = target;
	SendPacketOptions.SocketId = &SocketId;
	SendPacketOptions.Channel = (uint8)EEOSChannelType::DATA_CHAT;
	SendPacketOptions.DataLengthBytes = DataArray.Num();
	SendPacketOptions.Data = DataArray.GetData();
	SendPacketOptions.bAllowDelayedDelivery = true;
	SendPacketOptions.Reliability = static_cast<EOS_EPacketReliability>(reliability);

	if (EOS_P2P_SendPacket(GetP2PHandle(worldContextObject), &SendPacketOptions) == EOS_EResult::EOS_Success)
	{
		LogVerbose("Sent chat message of %i bytes to %s", message.Len(), *target.ToString());
	}
}

void UCoreChat::ListenForChatMessages(UObject* worldContextObject, FEOSProductUserId localUserId, const FOnChatMessageReceivedCallback& callback)
{
	if (!EOS_ProductUserId_IsValid(localUserId))
	{
		LogError("Invalid local user id!");
		return;
	}

	for (FChatMessageListener* Element : s_ChatListeners)
	{
		if (localUserId.ToString() == Element->UserProductId.ToString())
		{
			LogVerbose("Listener already exists");
			return;
		}
	}

	if (FOnlineSubsystemEOS* m_Subsystem = GetSubsystem(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		FCStringAnsi::Strncpy(SocketId.SocketName, "CHAT", 5);

		EOS_P2P_AddNotifyPeerConnectionRequestOptions AddNotifyPeerConnectionOptions = { };
		AddNotifyPeerConnectionOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
		AddNotifyPeerConnectionOptions.LocalUserId = localUserId;
		AddNotifyPeerConnectionOptions.SocketId = &SocketId;

		auto m_Callback = [](const EOS_P2P_OnIncomingConnectionRequestInfo* data)
		{
			LogVerbose("");

			EOS_P2P_AcceptConnectionOptions AddNotifyPeerConnectionOptions = { };
			AddNotifyPeerConnectionOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
			AddNotifyPeerConnectionOptions.LocalUserId = data->LocalUserId;
			AddNotifyPeerConnectionOptions.RemoteUserId = data->RemoteUserId;
			AddNotifyPeerConnectionOptions.SocketId = data->SocketId;

			EOS_P2P_AcceptConnection(GetP2PHandle(static_cast<UObject*>(data->ClientData)), &AddNotifyPeerConnectionOptions);
		};

		FChatMessageListener* ListenerObj = new FChatMessageListener;
		ListenerObj->P2PHandle = m_Subsystem->m_P2PHandle;
		ListenerObj->Callback = callback;
		ListenerObj->ListenId = EOS_P2P_AddNotifyPeerConnectionRequest(GetP2PHandle(worldContextObject), &AddNotifyPeerConnectionOptions, worldContextObject, m_Callback);
		ListenerObj->UserProductId = localUserId;

		s_ChatListeners.Add(ListenerObj);

		LogVerbose("Listening for Chat Messages");
	}
	else
	{
		LogError("Cannot listen for chat messages, subsystem is not valid.");
	}
}

void UCoreChat::ClearAllChatListeners()
{
	for (FChatMessageListener* Element : s_ChatListeners)
	{
		if (Element->P2PHandle == nullptr) continue;

		EOS_P2P_RemoveNotifyPeerConnectionRequest(Element->P2PHandle, Element->ListenId);
	}

	s_ChatListeners.Empty();
}
