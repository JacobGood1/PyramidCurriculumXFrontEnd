// Copyright Charmillot Clement 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BlueprintWebSocketNodes.generated.h"

class UBlueprintWebSocket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWebSocketEvent, UBlueprintWebSocket*const, Socket, int32 const, StatusCode, const FString&, Message);

/**
 * Base class for asynchronous helper nodes
 **/
UCLASS(Abstract)
class UWebSocketConnectAsyncProxyBase : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    UWebSocketConnectAsyncProxyBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};
    
    /* Called once the WebSocket is connected to the server. */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnConnected;
    
    /* Called when the socket fail to connect to the server. */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnConnectionError;
    
    /* Called when the connection to the server has been closed. */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnClose;

    /* Called when the server has sent a message. */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnMessage;

    virtual void Activate();

protected:
    FORCEINLINE UBlueprintWebSocket* GetSocket() { return Socket; }

    void InitSocket(UBlueprintWebSocket* const InSocket, const FString & Url, const FString Protocol);

private:
    UFUNCTION()
    void OnConnectedInternal();
    UFUNCTION()
    void OnConnectionErrorInternal(const FString& Error);
    UFUNCTION()
    void OnCloseInternal(const int64 Status, const FString& Reason, const bool bWasClean);
    UFUNCTION()
    void OnMessageInternal(const FString& Message);

protected:
    UPROPERTY()
    UBlueprintWebSocket* Socket;

private:
    FString Url;
    FString Protocol;
};

/**
 *  Asynchronous node to easily handle connection and events.
 **/
UCLASS()
class UWebSocketConnectAsyncProxy final : public UWebSocketConnectAsyncProxyBase
{
    GENERATED_BODY()
public:
    UWebSocketConnectAsyncProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

    /**
     * Helper node to easily connect to server and handle connection events.
     * @param WebSocket The WebSocket used to connect.
     * @param Url       The URL of the server we want to connect to.
     * @param Protocol  The protocol to use. Should be "ws" or "wss".
     * @param Headers   The header used during the upgrade.
    */
    UFUNCTION(BlueprintCallable, Category=WebSocket, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Connect WebSocket to WebSocket server"))
    static UWebSocketConnectAsyncProxy* Connect(UBlueprintWebSocket* const WebSocket, const FString & Url, const FString & Protocol);

};


/**
 *  Asynchronous node to easily handle connection and events.
 **/
UCLASS()
class UWebSocketCreateConnectAsyncProxy final : public UWebSocketConnectAsyncProxyBase
{
    GENERATED_BODY()
public:
    UWebSocketCreateConnectAsyncProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

    /**
     * Helper node to easily connect to server and handle connection events.
     * @param Url       The URL of the server we want to connect to.
     * @param Protocol  The protocol to use. Should be "ws" or "wss".
     * @param Headers   The header used during the upgrade.
    */
    UFUNCTION(BlueprintCallable, Category=WebSocket, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers", DisplayName = "Connect to WebSocket server"))
    static UWebSocketCreateConnectAsyncProxy* Connect(const FString & Url, const FString & Protocol, const TMap<FString, FString> & Headers);

};

