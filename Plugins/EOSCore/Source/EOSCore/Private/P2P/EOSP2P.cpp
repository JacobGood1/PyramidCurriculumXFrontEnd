/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "P2P/EOSP2P.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

UCoreP2P* UCoreP2P::s_P2PObject = nullptr;

UCoreP2P::UCoreP2P()
{
	s_P2PObject = this;
}

EOSResult UCoreP2P::EOSP2PSendPacket(UObject* worldContextObject, FEOSP2PSendPacketOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = {  };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_SendPacketOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.RemoteUserId = options.RemoteUserId;
		P2POptions.SocketId = &SocketId;
		P2POptions.Channel = static_cast<uint8_t>(options.Channel);
		P2POptions.DataLengthBytes = static_cast<uint32_t>(options.Data.Num());
		P2POptions.Data = options.Data.GetData();
		P2POptions.bAllowDelayedDelivery = options.bAllowDelayedDelivery;
		P2POptions.Reliability = static_cast<EOS_EPacketReliability>(options.Reliability);

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_SendPacket(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PGetNextReceivedPacketSize(UObject* worldContextObject, FEOSP2PGetNextReceivedPacketSizeOptions options, int32& outPacketSizeBytes)
{
	LogVerbose("");

	outPacketSizeBytes = 0;

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		uint8_t Channel = options.RequestedChannel;
		uint32_t Size = 0;
		EOS_P2P_GetNextReceivedPacketSizeOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.RequestedChannel = (options.RequestedChannel == -1 ? nullptr : &Channel);

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_GetNextReceivedPacketSize(GetP2PHandle(worldContextObject), &P2POptions, &Size));

		if (Result == EOSResult::EOS_Success)
		{
			outPacketSizeBytes = Size;
		}
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PReceivePacket(UObject* worldContextObject, FEOSP2PReceivePacketOptions options, FEOSProductUserId& outPeerId, FEOSP2PSocketId& outSocketId, int32& outChannel, TArray<uint8>& outData, int32& outBytesWritten)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		uint8_t OptionsChannel = options.RequestedChannel;

		EOS_P2P_ReceivePacketOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.MaxDataSizeBytes = static_cast<uint32_t>(options.MaxDataSizeBytes);
		P2POptions.RequestedChannel = (OptionsChannel == -1 ? nullptr : &OptionsChannel);

		EOS_ProductUserId PeerId;
		EOS_P2P_SocketId SocketId;
		uint8_t Channel;
		TArray<uint8> Data;
		Data.SetNum(options.MaxDataSizeBytes);
		uint32_t BytesWritten = 0;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_ReceivePacket(GetP2PHandle(worldContextObject), &P2POptions, &PeerId, &SocketId, &Channel, Data.GetData(), &BytesWritten));

		if (Result == EOSResult::EOS_Success)
		{
			Data.SetNum(BytesWritten);

			outPeerId = PeerId;
			outSocketId = SocketId;
			outChannel = Channel;
			outData = Data;
			outBytesWritten = BytesWritten;
		}
	}

	return Result;
}

static FOnP2PIncomingConnectionRequestCallback OnP2PIncomingConnectionRequestCallback;
FEOSNotificationId UCoreP2P::EOSP2PAddNotifyPeerConnectionRequest(UObject* worldContextObject, FEOSP2PAddNotifyPeerConnectionRequestOptions options, const FOnP2PIncomingConnectionRequestCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotifiationId;

	if (GetP2PHandle(worldContextObject))
	{
		OnP2PIncomingConnectionRequestCallback = callback;

		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_AddNotifyPeerConnectionRequestOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.SocketId = (options.SocketId.SocketName.Len() > 0 ? &SocketId : nullptr);

		NotifiationId = EOS_P2P_AddNotifyPeerConnectionRequest(GetP2PHandle(worldContextObject), &P2POptions, this, ([](const EOS_P2P_OnIncomingConnectionRequestInfo* data)
		{
			UCoreP2P* P2P = static_cast<UCoreP2P*>(data->ClientData);
			check(P2P);

			if (P2P)
			{
				OnP2PIncomingConnectionRequestCallback.ExecuteIfBound(*data, true);
			}

			LogVerbose("");
		}));
	}

	return NotifiationId;
}

void UCoreP2P::EOSP2PRemoveNotifyPeerConnectionRequest(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_RemoveNotifyPeerConnectionRequest(GetP2PHandle(worldContextObject), id);

		OnP2PIncomingConnectionRequestCallback.Clear();
		OnP2PIncomingConnectionRequestCallback.Unbind();
	}
}

FEOSNotificationId UCoreP2P::EOSP2PAddNotifyPeerConnectionClosed(UObject* worldContextObject, FEOSP2PAddNotifyPeerConnectionClosedOptions options)
{
	LogVerbose("");

	FEOSNotificationId NotifiationId;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_AddNotifyPeerConnectionClosedOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.SocketId = (options.SocketId.SocketName.Len() > 0 ? &SocketId : nullptr);

		NotifiationId = EOS_P2P_AddNotifyPeerConnectionClosed(GetP2PHandle(worldContextObject), &P2POptions, this, ([](const EOS_P2P_OnRemoteConnectionClosedInfo* data)
		{
			UCoreP2P* P2P = static_cast<UCoreP2P*>(data->ClientData);
			check(P2P);

			LogVerbose("");
		}));
	}

	return NotifiationId;
}

void UCoreP2P::EOSP2PRemoveNotifyPeerConnectionClosed(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_RemoveNotifyPeerConnectionClosed(GetP2PHandle(worldContextObject), id);
	}
}

EOSResult UCoreP2P::EOSP2PAcceptConnection(UObject* worldContextObject, FEOSP2PAcceptConnectionOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_AcceptConnectionOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.RemoteUserId = options.RemoteUserId;
		P2POptions.SocketId = &SocketId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_AcceptConnection(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PCloseConnection(UObject* worldContextObject, FEOSP2PCloseConnectionOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_CloseConnectionOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.RemoteUserId = options.RemoteUserId;
		P2POptions.SocketId = &SocketId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_CloseConnection(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PCloseConnections(UObject* worldContextObject, FEOSP2PCloseConnectionsOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;

		FCStringAnsi::Strncpy(SocketId.SocketName, TCHAR_TO_UTF8(*options.SocketId.SocketName), options.SocketId.SocketName.Len() + 1);

		EOS_P2P_CloseConnectionsOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.LocalUserId = options.LocalUserId;
		P2POptions.SocketId = &SocketId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_CloseConnections(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

static FOnP2PQueryNATTypeCompleteCallback EOSP2PQueryNATTypeCallback;
void UCoreP2P::EOSP2PQueryNATType(UObject* worldContextObject, const FOnP2PQueryNATTypeCompleteCallback& callback)
{
	LogVerbose("");

	if (GetP2PHandle(worldContextObject))
	{
		EOSP2PQueryNATTypeCallback = callback;
		EOS_P2P_QueryNATTypeOptions P2POptions = { };
		P2POptions.ApiVersion = EOS_P2P_QUERYNATTYPE_API_LATEST;

		EOS_P2P_QueryNATType(GetP2PHandle(worldContextObject), &P2POptions, this, ([](const EOS_P2P_OnQueryNATTypeCompleteInfo* data)
		{
			UCoreP2P* P2P = static_cast<UCoreP2P*>(data->ClientData);
			check(P2P);

			if (P2P)
			{
				EOSP2PQueryNATTypeCallback.ExecuteIfBound(*data, true);
			}
			LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));
		}));
	}
}

EOSResult UCoreP2P::EOSP2PGetNATType(UObject* worldContextObject, FEOSP2PGetNATTypeOptions options, EEOSENATType& outNATType)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_GetNATTypeOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;

		EOS_ENATType NAT = EOS_ENATType::EOS_NAT_Unknown;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_GetNATType(GetP2PHandle(worldContextObject), &P2POptions, &NAT));

		if (Result == EOSResult::EOS_Success)
		{
			outNATType = static_cast<EEOSENATType>(NAT);
		}
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PSetRelayControl(UObject* worldContextObject, FEOSP2PSetRelayControlOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SetRelayControlOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.RelayControl = static_cast<EOS_ERelayControl>(options.RelayControl);

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_SetRelayControl(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PGetRelayControl(UObject* worldContextObject, FEOSP2PGetRelayControlOptions options, EEOSERelayControl& outRelayControl)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_GetRelayControlOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;

		EOS_ERelayControl RelayControl = EOS_ERelayControl::EOS_RC_AllowRelays;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_GetRelayControl(GetP2PHandle(worldContextObject), &P2POptions, &RelayControl));

		if (Result == EOSResult::EOS_Success)
		{
			outRelayControl = static_cast<EEOSERelayControl>(RelayControl);
		}
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PSetPortRange(UObject* worldContextObject, FEOSP2PSetPortRangeOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_SetPortRangeOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;
		P2POptions.Port = static_cast<uint16_t>(options.Port);
		P2POptions.MaxAdditionalPortsToTry = static_cast<uint16_t>(options.MaxAdditionalPortsToTry);

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_SetPortRange(GetP2PHandle(worldContextObject), &P2POptions));
	}

	return Result;
}

EOSResult UCoreP2P::EOSP2PGetPortRange(UObject* worldContextObject, FEOSP2PGetPortRangeOptions options, int32& outPort, int32 outNumAdditionalPortsToTry)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetP2PHandle(worldContextObject))
	{
		EOS_P2P_GetPortRangeOptions P2POptions = { };
		P2POptions.ApiVersion = options.ApiVersion;

		uint16_t Port = 0;
		uint16_t AdditionalPorts = 0;

		Result = EOSHelpers::ToEOSCoreResult(EOS_P2P_GetPortRange(GetP2PHandle(worldContextObject), &P2POptions, &Port, &AdditionalPorts));

		if (Result == EOSResult::EOS_Success)
		{
			outPort = Port;
			outNumAdditionalPortsToTry = AdditionalPorts;
		}
	}

	return Result;
}
