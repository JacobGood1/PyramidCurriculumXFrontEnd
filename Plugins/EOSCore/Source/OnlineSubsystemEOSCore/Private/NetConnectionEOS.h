/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "IpConnection.h"
#include "NetConnectionEOS.generated.h"

UCLASS(Transient, Config=Engine)
class UNetConnectionEOS	: public UIpConnection
{
	GENERATED_BODY()

public:
	explicit UNetConnectionEOS(const FObjectInitializer& ObjectInitializer);

//~ Begin NetConnection Interface
	virtual void InitLocalConnection(UNetDriver* InDriver, FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override;
	virtual void InitRemoteConnection(UNetDriver* InDriver, FSocket* InSocket, const FURL& InURL, const FInternetAddr& InRemoteAddr, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override;
	virtual void CleanUp() override;
//~ End NetConnection Interface

public:
	void DestroyEOSConnection();
public:
	bool bIsPassthrough;
	bool bHasP2PSession;
};
