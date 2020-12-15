// Copyright Charmillot Clement 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintWebSocketWrapper.generated.h"

class IWebSocket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE            (FWebSocketConnectedEvent                                                                                   );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketConnectionErrorEvent, const FString&,       ConnectionError                                      );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWebSocketClosedEvent,          int64,                StatusCode,   const FString&, Reason, bool, bWasClean);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketMessageEvent,         const FString&,       MessageString                                        );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FWebSocketRawMessageEvent,      const TArray<uint8>&, Data,         int32,          BytesRemaining         );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketMessageSentEvent,     const FString&,       MessageString                                        );

/**
 *	A socket's state. Used to create multi-pins nodes.
 **/
UENUM(BlueprintType)
enum class EBlueprintWebSocketIsConnectedExecution : uint8
{
	Connected,
	NotConnected
};


/** 
 *    A wrapper around the engine's native WebSocket.
 **/
UCLASS(BlueprintType, Blueprintable)
class BLUEPRINTWEBSOCKET_API UBlueprintWebSocket : public UObject
{
	GENERATED_BODY()

public:
	UBlueprintWebSocket();

	/**
	 * Create a WebSocket object to connect to a WebSocket server.
	 * @return A new WebSocket.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = WebSocket)
	static UPARAM(DisplayName = "New WebSocket") UBlueprintWebSocket* CreateWebSocket();

	/**
	 * Connect to the WebSocket server with the specified protocol.
	 * @param Url	   The server's URL.
	 * @param Protocol The protocol to use for connection. "ws" or "wss".
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void Connect(const FString& Url, const FString& Protocol);

	/**
	 * Set the specified headers for the connection.
	 * If the socket already have headers, it will keep the old headers
	 * and override the duplicated headers with the new values.
	 * @param InHeaders The header used during connection upgrade.
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void SetHeaders(const TMap<FString, FString>& InHeaders);

	/**
	 * Add the specifed key / value to the headers.
	 * @param Header The name of the header to add.
	 * @param Value  The value of the header.
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void AddHeader(const FString & Header, const FString& Value);

	/**
	 * Remove the specified header from the list of headers.
	 * @param HeaderToRemove The name of the header we want to remove
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void RemoveHeader(const FString & HeaderToRemove);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = WebSocket)
	/**
	 * Returns whether this socket is currently connected to a server.
	 * @return If we're connected to a server.
	*/
	UPARAM(DisplayName="Is Connected") bool IsConnected() const;

	UFUNCTION(BlueprintCallable, Category = WebSocket, meta = (ExpandEnumAsExecs = Branches))
	void BranchIsConnected(EBlueprintWebSocketIsConnectedExecution& Branches);
	
	/**
	 * Sends the specified message to the WebSocket server.
	 * @param Message The data to send.
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void SendMessage(const FString& Message);

	/**
	 * Sends the specified binary message to the WebSocket server.
	 * @param Message   The data to send.
	 * @param bIsBinary If this is a binary message or not.
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void SendRawMessage(const TArray<uint8> & Message, const bool bIsBinary = false);

	/**
	 * Close the connection with the WebSocket server.
	 * @param Code	 The code to send to the server.
	 * @param Reason The reason to send to the server.
	*/
	UFUNCTION(BlueprintCallable, Category = WebSocket)
	void Close(const int32 Code = 1000, const FString& Reason = TEXT(""));

	/* This event is called when the WebSocket successfully connect to the server. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketConnectedEvent OnConnectedEvent;

	/* This event is called when the WebSocket fail to connect to the server. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketConnectionErrorEvent OnConnectionErrorEvent;

	/* This event is called when the connection is closed. The connection can be closed by the client or the server. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketClosedEvent OnCloseEvent;

	/* This event is called when the server sent a string message. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketMessageEvent OnMessageEvent;

	/* This event is called when the server sent a binary message. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketRawMessageEvent OnRawMessageEvent;

	/* This event is called just after a message has been sent to the server. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = WebSocket)
	FWebSocketMessageSentEvent OnMessageSentEvent;

private:
	UFUNCTION() 
	void OnConnected();

	UFUNCTION()
	void OnConnectionError(const FString & Error);

	UFUNCTION()
	void OnClosed(int32 Status, const FString& Reason, bool bWasClean);

	UFUNCTION()
	void OnMessage(const FString& Message);

	void OnMessageSent(const FString& Message);

	void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

private:
	TMap<FString, FString> Headers;

	TSharedPtr<IWebSocket> NativeSocket;
};
