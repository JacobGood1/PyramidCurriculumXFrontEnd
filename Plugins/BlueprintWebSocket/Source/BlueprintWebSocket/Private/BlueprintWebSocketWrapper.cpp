// Copyright Charmillot Clement 2020. All Rights Reserved.

#include "BlueprintWebSocketWrapper.h"
#include "BlueprintWebSocket.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

UBlueprintWebSocket* UBlueprintWebSocket::CreateWebSocket()
{
	return NewObject<UBlueprintWebSocket>();
}

UBlueprintWebSocket::UBlueprintWebSocket()
	: Super()
	, NativeSocket(nullptr)
{}

void UBlueprintWebSocket::Connect(const FString& Url, const FString& Protocol)
{
	if (IsConnected())
	{
		UE_LOG(LogBlueprintWebSocket, Warning, TEXT("Ignoring Connect(): the socket is already connected to a server."));
		return;
	}

	NativeSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol, Headers);

	NativeSocket->OnConnected()      .AddUObject(this, &UBlueprintWebSocket::OnConnected);
	NativeSocket->OnConnectionError().AddUObject(this, &UBlueprintWebSocket::OnConnectionError);
	NativeSocket->OnClosed()         .AddUObject(this, &UBlueprintWebSocket::OnClosed);

	if (OnMessageEvent.IsBound())
	{
		NativeSocket->OnMessage().AddUObject(this, &UBlueprintWebSocket::OnMessage);
	}

	if (OnRawMessageEvent.IsBound())
	{
		NativeSocket->OnRawMessage().AddUObject(this, &UBlueprintWebSocket::OnRawMessage);
	}
	
	NativeSocket->Connect();
}

void UBlueprintWebSocket::Close(const int32 Code, const FString& Reason)
{
	if (IsConnected())
	{
		NativeSocket->Close(Code, Reason);
	}
	else
	{
		UE_LOG(LogBlueprintWebSocket, Warning, TEXT("Close() called but the WebSocket is not connected."));
	}
}

void UBlueprintWebSocket::SetHeaders(const TMap<FString, FString>& InHeaders)
{
	for (const auto& Header : InHeaders)
	{
		Headers.Add(Header);
	}
}

void UBlueprintWebSocket::AddHeader(const FString& Header, const FString& Value)
{
	Headers.Add(Header, Value);
}

void UBlueprintWebSocket::RemoveHeader(const FString& HeaderToRemove)
{
	Headers.Remove(HeaderToRemove);
}

bool UBlueprintWebSocket::IsConnected() const
{
	return NativeSocket && NativeSocket->IsConnected();
}

void UBlueprintWebSocket::BranchIsConnected(EBlueprintWebSocketIsConnectedExecution& Branches)
{
	Branches = IsConnected() ? EBlueprintWebSocketIsConnectedExecution::Connected : EBlueprintWebSocketIsConnectedExecution::NotConnected;
}

void UBlueprintWebSocket::SendMessage(const FString& Message)
{
	if (IsConnected())
	{
		if (Message.Len() <= 0)
		{
			UE_LOG(LogBlueprintWebSocket, Warning, TEXT("SendMessage() called with an empty message."));
			return;
		}

		NativeSocket->Send(Message);
		OnMessageSent(Message);
	}
	else
	{
		UE_LOG(LogBlueprintWebSocket, Warning, TEXT("SendMessage() called but the WebSocket is not connected."));
	}
}

void UBlueprintWebSocket::SendRawMessage(const TArray<uint8> & Message, const bool bIsBinary)
{
	if (IsConnected())
	{
		if (Message.Num() <= 0)
		{
			UE_LOG(LogBlueprintWebSocket, Warning, TEXT("SendRawMessage() called with an empty message."));
			return;
		}

		NativeSocket->Send(Message.GetData(), sizeof(uint8) * Message.Num(), bIsBinary);
	}
	else
	{
		UE_LOG(LogBlueprintWebSocket, Warning, TEXT("SendRawMessage() called but the WebSocket is not connected."));
	}
}

void UBlueprintWebSocket::OnConnected()
{
	OnConnectedEvent.Broadcast();
}

void UBlueprintWebSocket::OnConnectionError(const FString& Error)
{
	OnConnectionErrorEvent.Broadcast(Error);
}

void UBlueprintWebSocket::OnClosed(int32 Status, const FString& Reason, bool bWasClean)
{
	OnCloseEvent.Broadcast(Status, Reason, bWasClean);
}

void UBlueprintWebSocket::OnMessage(const FString& Message)
{
	OnMessageEvent.Broadcast(Message);
}

void UBlueprintWebSocket::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	const TArray<uint8> ArrayData(reinterpret_cast<const uint8*>(Data), Size / sizeof(uint8));
	OnRawMessageEvent.Broadcast(ArrayData, static_cast<int64>(BytesRemaining));
}

void UBlueprintWebSocket::OnMessageSent(const FString& Message)
{
	OnMessageSentEvent.Broadcast(Message);
}
