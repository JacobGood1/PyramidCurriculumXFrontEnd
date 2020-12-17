/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypesEOS.h"
#include "NboSerializer.h"

class FNboSerializeToBufferEOS : public FNboSerializeToBuffer
{
public:
	FNboSerializeToBufferEOS() 
		: FNboSerializeToBuffer(512)
	{ }

	/** Constructor specifying the size to use */
	FNboSerializeToBufferEOS(uint32 Size) 
		: FNboSerializeToBuffer(Size)
	{}

	/**
	 * Adds EOS session info to the buffer
	 */
 	friend inline FNboSerializeToBufferEOS& operator<<(FNboSerializeToBufferEOS& Ar, const FOnlineSessionInfoEOS& SessionInfo)
 	{
		check(SessionInfo.m_HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		Ar << SessionInfo.m_SessionId;
		Ar << *SessionInfo.m_HostAddr;
		return Ar;
 	}

	/**
	 * Adds EOS Unique Id to the buffer
	 */
	friend inline FNboSerializeToBufferEOS& operator<<(FNboSerializeToBufferEOS& Ar, const FUniqueNetIdEOS& UniqueId)
	{
		Ar << UniqueId.UniqueNetIdStr;
		Ar << UniqueId.m_EpicAccountIdStr;
		Ar << UniqueId.m_ProductUserIdStr;
		return Ar;
	}
};

/**
 * Class used to write data into packets for sending via system link
 */
class FNboSerializeFromBufferEOS : public FNboSerializeFromBuffer
{
public:
	/**
	 * Initializes the buffer, size, and zeros the read offset
	 */
	FNboSerializeFromBufferEOS(uint8* Packet,int32 Length) 
		: FNboSerializeFromBuffer(Packet,Length)
	{}

	/**
	 * Reads EOS session info from the buffer
	 */
 	friend inline FNboSerializeFromBufferEOS& operator>>(FNboSerializeFromBufferEOS& Ar, FOnlineSessionInfoEOS& SessionInfo)
 	{
		check(SessionInfo.m_HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		Ar >> SessionInfo.m_SessionId; 
		Ar >> *SessionInfo.m_HostAddr;
		return Ar;
 	}

	/**
	 * Reads EOS Unique Id from the buffer
	 */
	friend inline FNboSerializeFromBufferEOS& operator>>(FNboSerializeFromBufferEOS& Ar, FUniqueNetIdEOS& UniqueId)
	{
		Ar >> UniqueId.UniqueNetIdStr;
		Ar >> UniqueId.m_EpicAccountIdStr;
		Ar >> UniqueId.m_ProductUserIdStr;
		return Ar;
	}
};
