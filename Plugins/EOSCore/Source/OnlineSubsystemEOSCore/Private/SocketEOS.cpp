/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "SocketEOS.h"
#include "SocketSubsystemEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

#if WANTS_NP_LOGGING
#include "Windows/AllowWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_START
#include <Windows.h>
THIRD_PARTY_INCLUDES_END

const TCHAR* GetLogPrefix()
{
	static FString Prefix;
	if (Prefix.Len() == 0)
	{
		FParse::Value(FCommandLine::Get(), TEXT("LogPrefix="), Prefix);
		if (Prefix.Len() == 0)
		{
			Prefix = TEXT("Unknown");
		}
	}
	return *Prefix;
}

void NpLog(const TCHAR* Msg)
{
	static HWND EditWindow = NULL;
	// Get the edit window so we can send messages to it
	if (EditWindow == NULL)
	{
		HWND MainWindow = FindWindowW(NULL, L"Untitled - Notepad");
		if (MainWindow == NULL)
		{
			MainWindow = FindWindowW(NULL, L"*Untitled - Notepad");
		}
		if (MainWindow != NULL)
		{
			EditWindow = FindWindowExW(MainWindow, NULL, L"Edit", NULL);
		}
	}
	if (EditWindow != NULL)
	{
		SendMessageW(EditWindow, EM_REPLACESEL, TRUE, (LPARAM)Msg);
	}
}

#include "Windows/HideWindowsPlatformTypes.h"
#endif

FSocketEOS::FSocketEOS(FSocketSubsystemEOS& InSocketSubsystem, const FString& InSocketDescription)
	: FSocket(ESocketType::SOCKTYPE_Datagram, InSocketDescription, NAME_None)
	, bIsListening(false)
	, m_SocketSubsystem(InSocketSubsystem)
	, m_ConnectNotifyCallback(nullptr)
	, m_ConnectNotifyId(EOS_INVALID_NOTIFICATIONID)
	, m_ClosedNotifyCallback(nullptr)
	, m_ClosedNotifyId(EOS_INVALID_NOTIFICATIONID)
{}

FSocketEOS::~FSocketEOS()
{
	Close();

	if (m_LocalAddress.IsValid())
	{
		m_SocketSubsystem.UnbindChannel(m_LocalAddress);
		m_LocalAddress = FInternetAddrEOS();
	}
}

bool FSocketEOS::Shutdown(ESocketShutdownMode Mode)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::Close()
{
	check(IsInGameThread() && "p2p does not support multithreading");

	if (m_ConnectNotifyId != EOS_INVALID_NOTIFICATIONID)
	{
		EOS_P2P_RemoveNotifyPeerConnectionRequest(m_SocketSubsystem.GetP2PHandle(), m_ConnectNotifyId);
	}
	
	delete m_ConnectNotifyCallback;
	m_ConnectNotifyCallback = nullptr;
	
	if (m_ClosedNotifyId != EOS_INVALID_NOTIFICATIONID)
	{
		EOS_P2P_RemoveNotifyPeerConnectionClosed(m_SocketSubsystem.GetP2PHandle(), m_ClosedNotifyId);
	}
	
	delete m_ClosedNotifyCallback;
	m_ClosedNotifyCallback = nullptr;

	if (m_LocalAddress.IsValid())
	{
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		FCStringAnsi::Strcpy(SocketId.SocketName, m_LocalAddress.GetSocketName());

		EOS_P2P_CloseConnectionsOptions Options = { };
		Options.ApiVersion = EOS_P2P_CLOSECONNECTIONS_API_LATEST;
		Options.LocalUserId = m_SocketSubsystem.GetLocalUserId();
		Options.SocketId = &SocketId;

		EOS_EResult Result = EOS_P2P_CloseConnections(m_SocketSubsystem.GetP2PHandle(), &Options);

		LogVerbose("Closing socket (%s) with result (%s)", *m_LocalAddress.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
		NP_LOG(TEXT("[%s] - Closing socket (%s) with result (%s)\r\n"), GetLogPrefix(), *m_LocalAddress.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

		m_ClosedRemotes.Empty();
	}

	return true;
}

bool FSocketEOS::Bind(const FInternetAddr& Addr)
{
	check(IsInGameThread() && "p2p does not support multithreading");

	if (!Addr.IsValid())
	{
		LogWarning("Attempted to bind to invalid address. Address = (%s)", *Addr.ToString(true));
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EADDRNOTAVAIL);
		return false;
	}

	// Ensure we called Initialize so we know who we are
	if (m_LocalAddress.GetRemoteUserId() != nullptr)
	{
		LogWarning("Attempted to bind on a socket that was not initialized. Address = (%s)", *Addr.ToString(true));
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_NOTINITIALISED);
		return false;
	}

	// If we have a remote user id, we're already bound
	if (m_LocalAddress.GetRemoteUserId() != nullptr)
	{
		LogWarning("Attempted to bind a socket that was already bound. ExistingAddress = (%s) NewAddress = (%s)", *m_LocalAddress.ToString(true), *Addr.ToString(true));
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EADDRINUSE);
		return false;
	}

	const FInternetAddrEOS& EOSAddr = static_cast<const FInternetAddrEOS&>(Addr);
	if (!m_SocketSubsystem.BindChannel(EOSAddr))
	{
		LogWarning("Attempted to bind a socket to a port in use. NewAddress = (%s)", *Addr.ToString(true));
		// BindPort sets our LastSocketError
		return false;
	}

	EOS_ProductUserId LocalUserId = m_LocalAddress.GetLocalUserId();
	m_LocalAddress = EOSAddr;
	m_LocalAddress.SetLocalUserId(LocalUserId);

	LogVerbose("Successfully bound socket to address (%s)", *m_LocalAddress.ToString(true));
	NP_LOG(TEXT("[%s] - Successfully bound socket to address (%s)\r\n"), GetLogPrefix(), *m_LocalAddress.ToString(true));
	return true;
}

bool FSocketEOS::Connect(const FInternetAddr& Addr)
{
	/** Not supported - connectionless (UDP) only */
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::Listen(int32)
{
	check(IsInGameThread() && "p2p does not support multithreading");

	if (!m_LocalAddress.IsValid())
	{
		LogWarning("Attempted to listen without a bound address. Address = (%s)", *m_LocalAddress.ToString(true));
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EADDRINUSE);
		return false;
	}

	// Add listener for inbound connections
	EOS_P2P_SocketId SocketId = { };
	SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
	FCStringAnsi::Strcpy(SocketId.SocketName, m_LocalAddress.GetSocketName());

	EOS_P2P_AddNotifyPeerConnectionRequestOptions Options = { };
	Options.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	Options.SocketId = &SocketId;

	m_ConnectNotifyCallback = new FConnectNotifyCallback();
	m_ConnectNotifyCallback->m_CallbackLambda = [this](const EOS_P2P_OnIncomingConnectionRequestInfo* Info)
	{
		char PuidBuffer[64];
		int32 BufferLen = 64;
		if (EOS_ProductUserId_ToString(Info->RemoteUserId, PuidBuffer, &BufferLen) != EOS_EResult::EOS_Success)
		{
			PuidBuffer[0] = '\0';
		}
		FString RemoteUser(PuidBuffer);

		if (Info->LocalUserId == m_LocalAddress.GetLocalUserId() && FCStringAnsi::Stricmp(Info->SocketId->SocketName, m_LocalAddress.GetSocketName()) == 0)
		{
			// In case they disconnected and then reconnected, remove them from our closed list
			FInternetAddrEOS RemoteAddress(Info->RemoteUserId, Info->SocketId->SocketName, m_LocalAddress.GetChannel());
			RemoteAddress.SetLocalUserId(m_LocalAddress.GetLocalUserId());
			m_ClosedRemotes.Remove(RemoteAddress);

			EOS_P2P_SocketId SocketId = { };
			SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
			FCStringAnsi::Strcpy(SocketId.SocketName, Info->SocketId->SocketName);

			EOS_P2P_AcceptConnectionOptions Options = { };
			Options.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
			Options.LocalUserId = m_LocalAddress.GetLocalUserId();
			Options.RemoteUserId = Info->RemoteUserId;
			Options.SocketId = &SocketId;
			EOS_EResult AcceptResult = EOS_P2P_AcceptConnection(m_SocketSubsystem.GetP2PHandle(), &Options);
			if (AcceptResult == EOS_EResult::EOS_Success)
			{
				LogVerbose("Accepting connection request from (%s) on socket (%s)", *RemoteUser, UTF8_TO_TCHAR(Info->SocketId->SocketName));
				NP_LOG(TEXT("[%s] - Accepting connection request from (%s) on socket (%s)\r\n"), GetLogPrefix(), *RemoteUser, UTF8_TO_TCHAR(Info->SocketId->SocketName));
			}
			else
			{
				LogError("EOS_P2P_AcceptConnection from (%s) on socket (%s) failed with (%s)", *RemoteUser, UTF8_TO_TCHAR(Info->SocketId->SocketName), ANSI_TO_TCHAR(EOS_EResult_ToString(AcceptResult)));
				NP_LOG(TEXT("[%s] - EOS_P2P_AcceptConnection from (%s) on socket (%s) failed with (%s)\r\n"), GetLogPrefix(), *RemoteUser, UTF8_TO_TCHAR(Info->SocketId->SocketName), ANSI_TO_TCHAR(EOS_EResult_ToString(AcceptResult)));
			}
		}
		else
		{
			LogWarning("Ignoring connection request from (%s) on socket (%s)", *RemoteUser, UTF8_TO_TCHAR(Info->SocketId->SocketName));
		}
	};
	m_ConnectNotifyId = EOS_P2P_AddNotifyPeerConnectionRequest(m_SocketSubsystem.GetP2PHandle(), &Options, m_ConnectNotifyCallback, m_ConnectNotifyCallback->GetCallbackPtr());

	RegisterClosedNotification();

	bIsListening = true;

	return true;
}

bool FSocketEOS::WaitForPendingConnection(bool& bHasPendingConnection, const FTimespan& WaitTime)
{
	/** Not supported - connectionless (UDP) only */
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::HasPendingData(uint32& PendingDataSize)
{
	check(IsInGameThread() && "p2p does not support multithreading");

	PendingDataSize = 0;

	EOS_P2P_GetNextReceivedPacketSizeOptions Options = { };
	Options.ApiVersion = EOS_P2P_GETNEXTRECEIVEDPACKETSIZE_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	uint8 Channel = m_LocalAddress.GetChannel();
	Options.RequestedChannel = &Channel;

	EOS_EResult Result = EOS_P2P_GetNextReceivedPacketSize(m_SocketSubsystem.GetP2PHandle(), &Options, &PendingDataSize);
	if (Result != EOS_EResult::EOS_Success)
	{
		LogWarning("Unable to check for data on address (%s) result code = (%s)", *m_LocalAddress.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	return true;
}

FSocket* FSocketEOS::Accept(const FString& InSocketDescription)
{
	/** Not supported - connectionless (UDP) only */
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return nullptr;
}

FSocket* FSocketEOS::Accept(FInternetAddr& OutAddr, const FString& InSocketDescription)
{
	/** Not supported - connectionless (UDP) only */
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return nullptr;
}

bool FSocketEOS::SendTo(const uint8* Data, int32 Count, int32& OutBytesSent, const FInternetAddr& Destination)
{
	check(IsInGameThread() && "p2p does not support multithreading");

	OutBytesSent = 0;

	if (!Destination.IsValid())
	{
		LogWarning("Unable to send data, invalid destination address. DestinationAddress = (%s)", *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EADDRNOTAVAIL);
		return false;
	}

	if (Count > EOS_P2P_MAX_PACKET_SIZE)
	{
		LogWarning("Unable to send data, data over maximum size. Amount=[%d/%d] DestinationAddress = (%s)", Count, EOS_P2P_MAX_PACKET_SIZE, *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EMSGSIZE);
		return false;
	}

	if (Count < 0)
	{
		LogWarning("Unable to send data, data invalid. Amount=[%d/%d] DestinationAddress = (%s)", Count, EOS_P2P_MAX_PACKET_SIZE, *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	if (Data == nullptr && Count != 0)
	{
		LogWarning("Unable to send data, data invalid. DestinationAddress = (%s)", *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	if (!m_LocalAddress.IsValid())
	{
		LogWarning("Unable to send data, socket was not initialized. DestinationAddress = (%s)", *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_NOTINITIALISED);
		return false;
	}

	const FInternetAddrEOS& DestinationAddress = static_cast<const FInternetAddrEOS&>(Destination);
	if (m_LocalAddress == DestinationAddress)
	{
		LogWarning("Unable to send data, unable to send data to ourselves. DestinationAddress = (%s)", *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_ECONNREFUSED);
		return false;
	}

	// Check for sending to an address we explicitly closed
	if (WasClosed(DestinationAddress))
	{
		LogWarning("Unable to send data to closed connection. DestinationAddress = (%s)", *Destination.ToString(true));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_ECONNREFUSED);
		return false;
	}

	// Need to handle closures if we are a client and the server closes down on us
	RegisterClosedNotification();

	EOS_P2P_SocketId SocketId = { };
	SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
	FCStringAnsi::Strcpy(SocketId.SocketName, DestinationAddress.GetSocketName());

	EOS_P2P_SendPacketOptions Options = { };
	Options.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	Options.RemoteUserId = DestinationAddress.GetRemoteUserId();
	Options.SocketId = &SocketId;
	Options.Channel = DestinationAddress.GetChannel();
	Options.DataLengthBytes = Count;
	Options.Data = Data;
	EOS_EResult Result = EOS_P2P_SendPacket(m_SocketSubsystem.GetP2PHandle(), &Options);
	NP_LOG(TEXT("[%s] - EOS_P2P_SendPacket() to (%s) result code = (%s)\r\n"), GetLogPrefix(), *Destination.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
	if (Result != EOS_EResult::EOS_Success)
	{
		LogError("Unable to send data to (%s) result code = (%s)", *Destination.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}
	OutBytesSent = Count;

	return true;
}

bool FSocketEOS::Send(const uint8* Data, int32 Count, int32& BytesSent)
{
	/** Not supported - connectionless (UDP) only */
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	BytesSent = 0;
	return false;
}

bool FSocketEOS::RecvFrom(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddr& Source, ESocketReceiveFlags::Type Flags)
{
	check(IsInGameThread() && "p2p does not support multithreading");
	BytesRead = 0;

	if (BufferSize < 0)
	{
		LogError("Unable to receive data, receiving buffer was invalid. BufferSize = (%d)", BufferSize);

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	if (Flags != ESocketReceiveFlags::None)
	{
		// We do not support peaking / blocking until a packet comes
		LogError("Socket receive flags (%d) are not supported", int32(Flags));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
		return false;
	}

	EOS_P2P_ReceivePacketOptions Options = { };
	Options.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	Options.MaxDataSizeBytes = BufferSize;
	uint8 Channel = m_LocalAddress.GetChannel();
	Options.RequestedChannel = &Channel;

	EOS_ProductUserId RemoteUserId = nullptr;
	EOS_P2P_SocketId SocketId;
	
	EOS_EResult Result = EOS_P2P_ReceivePacket(m_SocketSubsystem.GetP2PHandle(), &Options, &RemoteUserId, &SocketId, &Channel, Data, (uint32*)&BytesRead);
	NP_LOG(TEXT("[%s] - EOS_P2P_ReceivePacket() for user (%s) and channel (%d) with result code = (%s)\r\n"), GetLogPrefix(), *MakeStringFromProductUserId(m_LocalAddress.GetLocalUserId()), Channel, ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
	if (Result == EOS_EResult::EOS_NotFound)
	{
		// No data to read
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EWOULDBLOCK);
		return false;
	}
	else if (Result != EOS_EResult::EOS_Success)
	{
		LogError("Unable to receive data result code = (%s)", ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	FInternetAddrEOS& SourceAddress = static_cast<FInternetAddrEOS&>(Source);
	SourceAddress.SetLocalUserId(m_LocalAddress.GetLocalUserId());
	SourceAddress.SetRemoteUserId(RemoteUserId);
	SourceAddress.SetSocketName(SocketId.SocketName);
	SourceAddress.SetChannel(Channel);

	NP_LOG(TEXT("[%s] - EOS_P2P_ReceivePacket() of size (%d) from (%s)\r\n"), GetLogPrefix(), BytesRead, *SourceAddress.ToString(true));

	return true;
}

bool FSocketEOS::Recv(uint8* Data, int32 BufferSize, int32& BytesRead, ESocketReceiveFlags::Type Flags)
{
	BytesRead = 0;
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::Wait(ESocketWaitConditions::Type Condition, FTimespan WaitTime)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

ESocketConnectionState FSocketEOS::GetConnectionState()
{
	return ESocketConnectionState::SCS_NotConnected;
}

void FSocketEOS::GetAddress(FInternetAddr& OutAddr)
{
	OutAddr = m_LocalAddress;
}

bool FSocketEOS::GetPeerAddress(FInternetAddr& OutAddr)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::SetNonBlocking(bool bIsNonBlocking)
{
	return true;
}

bool FSocketEOS::SetBroadcast(bool bAllowBroadcast)
{
	return true;
}

bool FSocketEOS::SetNoDelay(bool bIsNoDelay)
{
	return true;
}

bool FSocketEOS::JoinMulticastGroup(const FInternetAddr& GroupAddress)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::LeaveMulticastGroup(const FInternetAddr& GroupAddress)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::SetMulticastLoopback(bool bLoopback)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::SetMulticastTtl(uint8 TimeToLive)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::JoinMulticastGroup(const FInternetAddr& GroupAddress, const FInternetAddr& InterfaceAddress)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::LeaveMulticastGroup(const FInternetAddr& GroupAddress, const FInternetAddr& InterfaceAddress)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::SetMulticastInterface(const FInternetAddr& InterfaceAddress)
{
	m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EOPNOTSUPP);
	return false;
}

bool FSocketEOS::SetReuseAddr(bool bAllowReuse)
{
	return true;
}

bool FSocketEOS::SetLinger(bool bShouldLinger, int32 Timeout)
{
	return true;
}

bool FSocketEOS::SetRecvErr(bool bUseErrorQueue)
{
	return true;
}

bool FSocketEOS::SetSendBufferSize(int32 Size, int32& NewSize)
{
	return true;
}

bool FSocketEOS::SetReceiveBufferSize(int32 Size, int32& NewSize)
{
	return true;
}

int32 FSocketEOS::GetPortNo()
{
	return m_LocalAddress.GetChannel();
}

void FSocketEOS::SetLocalAddress(const FInternetAddrEOS& InLocalAddress)
{
	m_LocalAddress = InLocalAddress;
}

bool FSocketEOS::Close(const FInternetAddrEOS& RemoteAddress)
{
	check(IsInGameThread() && "p2p does not support multithreading");

	if (!RemoteAddress.IsValid())
	{
		LogError("Unable to close socket with remote address as it is invalid RemoteAddress = (%s)", *RemoteAddress.ToString(true));
		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EADDRNOTAVAIL);
		return false;
	}

	// So we don't reopen a connection by sending to it
	m_ClosedRemotes.Add(RemoteAddress);

	EOS_P2P_SocketId SocketId = { };
	SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
	FCStringAnsi::Strcpy(SocketId.SocketName, RemoteAddress.GetSocketName());

	EOS_P2P_CloseConnectionOptions Options = { };
	Options.ApiVersion = EOS_P2P_CLOSECONNECTION_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	Options.RemoteUserId = RemoteAddress.GetRemoteUserId();
	Options.SocketId = &SocketId;

	EOS_EResult Result = EOS_P2P_CloseConnection(m_SocketSubsystem.GetP2PHandle(), &Options);
	NP_LOG(TEXT("[%s] - EOS_P2P_CloseConnection() with remote address RemoteAddress (%s) result code (%s)\r\n"), GetLogPrefix(), *RemoteAddress.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
	if (Result != EOS_EResult::EOS_Success)
	{
		LogError("Unable to close socket with remote address RemoteAddress (%s) due to error (%s)", *RemoteAddress.ToString(true), ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

		m_SocketSubsystem.SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	return true;
}

void FSocketEOS::RegisterClosedNotification()
{
	if (m_ClosedNotifyId != EOS_INVALID_NOTIFICATIONID)
	{
		// Already listening for these events so ignore
		return;
	}
	
	EOS_P2P_SocketId SocketId = { };
	SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
	FCStringAnsi::Strcpy(SocketId.SocketName, m_LocalAddress.GetSocketName());

	EOS_P2P_AddNotifyPeerConnectionClosedOptions Options = { };
	Options.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST;
	Options.LocalUserId = m_LocalAddress.GetLocalUserId();
	Options.SocketId = &SocketId;

	m_ClosedNotifyCallback = new FClosedNotifyCallback();
	m_ClosedNotifyCallback->m_CallbackLambda = [this](const EOS_P2P_OnRemoteConnectionClosedInfo* Info)
	{
		// Add this connection to the list of closed ones
		FInternetAddrEOS RemoteAddress(Info->RemoteUserId, Info->SocketId->SocketName, m_LocalAddress.GetChannel());
		RemoteAddress.SetLocalUserId(m_LocalAddress.GetLocalUserId());
		m_ClosedRemotes.Add(RemoteAddress);
		NP_LOG(TEXT("[%s] - Close connection received for remote address (%s)\r\n"), GetLogPrefix(), *RemoteAddress.ToString(true));
	};

	m_ClosedNotifyId = EOS_P2P_AddNotifyPeerConnectionClosed(m_SocketSubsystem.GetP2PHandle(), &Options, m_ClosedNotifyCallback, m_ClosedNotifyCallback->GetCallbackPtr());
}
