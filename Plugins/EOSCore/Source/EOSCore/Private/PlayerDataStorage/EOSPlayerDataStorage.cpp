/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "PlayerDataStorage/EOSPlayerDataStorage.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

static size_t MaxChunkSize = 4096;

UCorePlayerDataStorage* UCorePlayerDataStorage::s_PlayerDataStorageObject = nullptr;
FEOSHPlayerDataStorageFileTransferRequest UCorePlayerDataStorage::FileTransferHandleRead = nullptr;
FEOSHPlayerDataStorageFileTransferRequest UCorePlayerDataStorage::FileTransferHandleWrite = nullptr;
FTransferInProgress UCorePlayerDataStorage::CurrentRead;
FTransferInProgress UCorePlayerDataStorage::CurrentWrite;

UCorePlayerDataStorage::UCorePlayerDataStorage()
{
	s_PlayerDataStorageObject = this;
}

void UCorePlayerDataStorage::EOSPlayerDataStorageQueryFile(UObject* worldContextObject, FEOSPlayerDataStorageQueryFileOptions queryFileOptions, const FOnQueryFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char FileNameAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FileNameAnsi, TCHAR_TO_UTF8(*queryFileOptions.Filename), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_QueryFileOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = queryFileOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = queryFileOptions.LocalUserId;
		PlayerDataStorageOptions.Filename = FileNameAnsi;

		FFileCompleteCallback* CallbackObj = new FFileCompleteCallback({ this, callback });
		EOS_PlayerDataStorage_QueryFile(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, CallbackObj, Internal_OnPlayerDataStorageQueryFileCallback);
	}
}

void UCorePlayerDataStorage::EOSPlayerDataStorageQueryFileList(UObject* worldContextObject, FEOSPlayerDataStorageQueryFileListOptions queryFileListOptions, const FOnQueryFileListCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		EOS_PlayerDataStorage_QueryFileListOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = queryFileListOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = queryFileListOptions.LocalUserId;

		FFileListCompleteCallback* CallbackObj = new FFileListCompleteCallback({ this, callback });
		EOS_PlayerDataStorage_QueryFileList(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, CallbackObj, Internal_OnPlayerDataStorageQueryFileListCallback);
	}
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageCopyFileMetadataByFilename(UObject* worldContextObject, FEOSPlayerDataStorageCopyFileMetadataByFilenameOptions copyFileMetadataOptions, FEOSPlayerDataStorageFileMetadata& outMetadata)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char FileNameAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FileNameAnsi, TCHAR_TO_UTF8(*copyFileMetadataOptions.Filename), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_CopyFileMetadataByFilenameOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = copyFileMetadataOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = copyFileMetadataOptions.LocalUserId;
		PlayerDataStorageOptions.Filename = FileNameAnsi;

		EOS_PlayerDataStorage_FileMetadata* FileMetaData = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorage_CopyFileMetadataByFilename(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, &FileMetaData));

		if (Result == EOSResult::EOS_Success)
		{
			outMetadata = *FileMetaData;
		}
	}

	return Result;
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageGetFileMetadataCount(UObject* worldContextObject, FEOSPlayerDataStorageGetFileMetadataCountOptions getFileMetadataCountOptions, int32& outFileMetadataCount)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		EOS_PlayerDataStorage_GetFileMetadataCountOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = getFileMetadataCountOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = getFileMetadataCountOptions.LocalUserId;

		int32_t OutFileMetaDataCount = 0;

		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorage_GetFileMetadataCount(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, &OutFileMetaDataCount));

		if (Result == EOSResult::EOS_Success)
		{
			outFileMetadataCount = OutFileMetaDataCount;
		}
	}

	return Result;
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageCopyFileMetadataAtIndex(UObject* worldContextObject, FEOSPlayerDataStorageCopyFileMetadataAtIndexOptions copyFileMetadataOptions, FEOSPlayerDataStorageFileMetadata& outMetadata)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		EOS_PlayerDataStorage_CopyFileMetadataAtIndexOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = copyFileMetadataOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = copyFileMetadataOptions.LocalUserId;
		PlayerDataStorageOptions.Index = static_cast<uint32_t>(copyFileMetadataOptions.Index);

		EOS_PlayerDataStorage_FileMetadata* FileMetaData = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorage_CopyFileMetadataAtIndex(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, &FileMetaData));

		if (Result == EOSResult::EOS_Success)
		{
			outMetadata = *FileMetaData;
		}
	}

	return Result;
}

void UCorePlayerDataStorage::EOSPlayerDataStorageDuplicateFile(UObject* worldContextObject, FEOSPlayerDataStorageDuplicateFileOptions duplicateOptions, const FOnDuplicateFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char SourceAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		char DestinationAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];

		FCStringAnsi::Strncpy(SourceAnsi, TCHAR_TO_UTF8(*duplicateOptions.SourceFilename), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);
		FCStringAnsi::Strncpy(DestinationAnsi, TCHAR_TO_UTF8(*duplicateOptions.DestinationFilename), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_DuplicateFileOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = duplicateOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = duplicateOptions.LocalUserId;
		PlayerDataStorageOptions.DestinationFilename = DestinationAnsi;
		PlayerDataStorageOptions.SourceFilename = SourceAnsi;

		FDuplicateFileCompleteCallback* CallbackObj = new FDuplicateFileCompleteCallback({ this, callback });
		EOS_PlayerDataStorage_DuplicateFile(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, CallbackObj, Internal_OnPlayerDataStorageDuplicateFileCallback);
	}
}

void UCorePlayerDataStorage::EOSPlayerDataStorageDeleteFile(UObject* worldContextObject, FEOSPlayerDataStorageDeleteFileOptions deleteOptions, const FOnDeleteFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char FileNameAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FileNameAnsi, TCHAR_TO_UTF8(*deleteOptions.Filename), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_DeleteFileOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = deleteOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = deleteOptions.LocalUserId;
		PlayerDataStorageOptions.Filename = FileNameAnsi;

		FDeleteFileCompleteCallback* CallbackObj = new FDeleteFileCompleteCallback({ this, callback });
		EOS_PlayerDataStorage_DeleteFile(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, CallbackObj, Internal_OnPlayerDataStorageDeleteFileCallback);
	}
}

void UCorePlayerDataStorage::EOSPlayerDataStorageReadFile(UObject* worldContextObject, FEOSPlayerDataStorageReadFileOptions readOptions, const FOnReadFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char RemoteFileAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(RemoteFileAnsi, TCHAR_TO_UTF8(*readOptions.RemoteFile), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_ReadFileOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = readOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = readOptions.LocalUserId;
		PlayerDataStorageOptions.Filename = RemoteFileAnsi;
		PlayerDataStorageOptions.ReadChunkLengthBytes = static_cast<uint32_t>(MaxChunkSize);
		PlayerDataStorageOptions.ReadFileDataCallback = OnFileDataReceived;
		PlayerDataStorageOptions.FileTransferProgressCallback = OnFileTransferProgressUpdated;

		CurrentRead.LocalFile = readOptions.LocalFile;
		FReadFileCompleteCallback* CallbackObj = new FReadFileCompleteCallback({ this, callback });
		FileTransferHandleRead = EOS_PlayerDataStorage_ReadFile(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, CallbackObj, Internal_OnPlayerDataStorageReadFileCallback);
	}
}

void UCorePlayerDataStorage::EOSPlayerDataStorageWriteFile(UObject* worldContextObject, FEOSPlayerDataStorageWriteFileOptions writeOptions, const FOnWriteFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		if (!PlatformFile.FileExists(*writeOptions.LocalFile))
		{
			LogWarning("File does not exist!");

			return;
		}

		char RemoteFileAnsi[EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(RemoteFileAnsi, TCHAR_TO_UTF8(*writeOptions.RemoteFile), EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_PlayerDataStorage_WriteFileOptions PlayerDataStorageOptions = { };
		PlayerDataStorageOptions.ApiVersion = writeOptions.ApiVersion;
		PlayerDataStorageOptions.LocalUserId = writeOptions.LocalUserId;
		PlayerDataStorageOptions.Filename = RemoteFileAnsi;
		PlayerDataStorageOptions.ChunkLengthBytes = static_cast<uint32_t>(MaxChunkSize);
		PlayerDataStorageOptions.WriteFileDataCallback = OnFileDataSend;
		PlayerDataStorageOptions.FileTransferProgressCallback = OnFileTransferProgressUpdated;

		IFileHandle* Handle = PlatformFile.OpenRead(*writeOptions.LocalFile);

		if (Handle)
		{
			CurrentWrite.CurrentIndex = 0;

			TArray<uint8> Data;
			Data.SetNum(Handle->Size());

			CurrentWrite.Data.SetNum(Handle->Size());
			CurrentWrite.TotalSize = Handle->Size();
			Handle->Read(Data.GetData(), Handle->Size());

			memcpy(static_cast<void*>(&CurrentWrite.Data[0]), static_cast<const void*>(&Data[0]), Data.Num());
			delete Handle;
		}

		FWriteFileCompleteCallback* Data = new FWriteFileCompleteCallback({ this, callback });
		FileTransferHandleWrite = EOS_PlayerDataStorage_WriteFile(GetPlayerDataStorageHandle(worldContextObject), &PlayerDataStorageOptions, Data, Internal_OnPlayerDataStorageWriteFileCallback);
	}
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageDeleteCache(UObject* worldContextObject, FEOSPlayerDataStorageDeleteCacheOptions options, const FOnDeleteCacheCompleteCallback& callback)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		EOS_PlayerDataStorage_DeleteCacheOptions DeleteCacheOptions;
		DeleteCacheOptions.ApiVersion = options.ApiVersion;
		DeleteCacheOptions.LocalUserId = options.LocalUserId;

		FDeleteCacheCompleteCallback* CallbackObj = new FDeleteCacheCompleteCallback({ this, callback });

		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorage_DeleteCache(GetPlayerDataStorageHandle(worldContextObject), &DeleteCacheOptions, nullptr, Internal_OnPlayerDataStorageDeleteCacheCallback));
	}

	return Result;
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageFileTransferRequestGetFileRequestState(UObject* worldContextObject, const FEOSHPlayerDataStorageFileTransferRequest& handle)
{
	LogVerbose("");
	
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorageFileTransferRequest_GetFileRequestState(handle));
	}

	return Result;
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageFileTransferRequestGetFilename(UObject* worldContextObject, const FEOSHPlayerDataStorageFileTransferRequest& handle, int32 filenameStringBufferSizeBytes, FString& outStringBuffer)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		char Buffer[1024];
		int32 Size = 0;

		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorageFileTransferRequest_GetFilename(handle, filenameStringBufferSizeBytes, Buffer, &Size));

		if (Result == EOSResult::EOS_Success)
		{
			outStringBuffer = Buffer;
		}
	}

	return Result;
}

EOSResult UCorePlayerDataStorage::EOSPlayerDataStorageFileTransferRequestCancelRequest(UObject* worldContextObject, const FEOSHPlayerDataStorageFileTransferRequest& handle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		Result = EOSHelpers::ToEOSCoreResult(EOS_PlayerDataStorageFileTransferRequest_CancelRequest(handle));
	}

	return Result;
}

void UCorePlayerDataStorage::EOSPlayerDataStorageFileTransferRequestRelease(UObject* worldContextObject, const FEOSHPlayerDataStorageFileTransferRequest& handle)
{
	LogVerbose("");

	if (GetPlayerDataStorageHandle(worldContextObject))
	{
		EOS_PlayerDataStorageFileTransferRequest_Release(handle);
	}
}

EOS_PlayerDataStorage_EReadResult UCorePlayerDataStorage::OnFileDataReceived(const EOS_PlayerDataStorage_ReadFileDataCallbackInfo* data)
{
	LogVerbose("");

	if (data)
	{
		return s_PlayerDataStorageObject->ReceiveData(data->Filename, data->DataChunk, data->DataChunkLengthBytes, data->TotalFileSizeBytes);
	}
	else
	{
		return EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest;
	}
}

EOS_PlayerDataStorage_EReadResult UCorePlayerDataStorage::ReceiveData(const FString& filename, const void* data, size_t numBytes, size_t totalSize)
{
	LogVerbose("");

	if (data)
	{
		// initialize struct
		if (CurrentRead.CurrentIndex == 0 && CurrentRead.TotalSize == 0)
		{
			CurrentRead.TotalSize = totalSize;

			if (CurrentRead.TotalSize == 0)
			{
				return EOS_PlayerDataStorage_EReadResult::EOS_RR_ContinueReading;
			}

			CurrentRead.Data.SetNum(totalSize);
		}

		if (CurrentRead.TotalSize - CurrentRead.CurrentIndex >= numBytes)
		{
			memcpy(static_cast<void*>(&CurrentRead.Data[CurrentRead.CurrentIndex]), data, numBytes);
			CurrentRead.CurrentIndex += numBytes;

			return EOS_PlayerDataStorage_EReadResult::EOS_RR_ContinueReading;
		}
		else
		{
			LogWarning("Could not receive data: too much of it!");

			return EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest;
		}
	}
	else
	{
		LogWarning("Could not receive data: Data pointer is null!");

		return EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest;
	}

	return EOS_PlayerDataStorage_EReadResult::EOS_RR_CancelRequest;
}

void UCorePlayerDataStorage::OnProgress(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo* data)
{
	LogVerbose("");

	if (data)
	{
	}
}

EOS_PlayerDataStorage_EWriteResult UCorePlayerDataStorage::SendData(const FString& filename, void* data, uint32_t* bytesWritten)
{
	LogVerbose("");

	if (data && bytesWritten)
	{
		if (CurrentWrite.Done())
		{
			*bytesWritten = 0;

			return EOS_PlayerDataStorage_EWriteResult::EOS_WR_CompleteRequest;
		}

		size_t BytesToWrite = std::min(MaxChunkSize, CurrentWrite.TotalSize - CurrentWrite.CurrentIndex);

		if (BytesToWrite > 0)
		{
			memcpy(data, static_cast<const void*>(&CurrentWrite.Data[CurrentWrite.CurrentIndex]), BytesToWrite);
		}

		*bytesWritten = static_cast<uint32_t>(BytesToWrite);

		CurrentWrite.CurrentIndex += BytesToWrite;

		if (CurrentWrite.Done())
		{
			return EOS_PlayerDataStorage_EWriteResult::EOS_WR_CompleteRequest;
		}
		else
		{
			return EOS_PlayerDataStorage_EWriteResult::EOS_WR_ContinueWriting;
		}
	}
	else
	{
		LogError("Could not send data: pointer is null!");

		return EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest;
	}
}

EOS_PlayerDataStorage_EWriteResult UCorePlayerDataStorage::OnFileDataSend(const EOS_PlayerDataStorage_WriteFileDataCallbackInfo* data, void* outDataBuffer, uint32_t* outDataWritten)
{
	LogVerbose("");

	if (data)
	{
		return s_PlayerDataStorageObject->SendData(data->Filename, outDataBuffer, outDataWritten);
	}

	return EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest;
}

void UCorePlayerDataStorage::OnFileTransferProgressUpdated(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo* data)
{
	LogVerbose("");

	if (data)
	{
		if (data->TotalFileSizeBytes > 0)
		{
			LogVerbose("");
		}

		s_PlayerDataStorageObject->OnProgress(data);
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageQueryFileCallback(const EOS_PlayerDataStorage_QueryFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FFileCompleteCallback* CallbackObj = static_cast<FFileCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageQueryFileListCallback(const EOS_PlayerDataStorage_QueryFileListCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FFileListCompleteCallback* CallbackObj = static_cast<FFileListCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageDuplicateFileCallback(const EOS_PlayerDataStorage_DuplicateFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDuplicateFileCompleteCallback* CallbackObj = static_cast<FDuplicateFileCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageDeleteFileCallback(const EOS_PlayerDataStorage_DeleteFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDeleteFileCompleteCallback* CallbackObj = static_cast<FDeleteFileCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageReadFileCallback(const EOS_PlayerDataStorage_ReadFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	if (data->ResultCode == EOS_EResult::EOS_Success)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		IFileHandle* Handle = PlatformFile.OpenWrite(*CurrentRead.LocalFile);

		if (Handle)
		{
			if (Handle->Write(CurrentRead.Data.GetData(), CurrentRead.Data.Num()))
			{
				LogVerbose("");
			}

			delete Handle;
		}
	}
	
	EOS_PlayerDataStorageFileTransferRequest_CancelRequest(FileTransferHandleRead);
	CurrentRead.Reset();

	FReadFileCompleteCallback* CallbackObj = static_cast<FReadFileCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
	check(PlayerDataStorage);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageWriteFileCallback(const EOS_PlayerDataStorage_WriteFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FWriteFileCompleteCallback* CallbackObj = static_cast<FWriteFileCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{

			EOS_PlayerDataStorageFileTransferRequest_CancelRequest(FileTransferHandleWrite);
			CurrentWrite.Reset();

			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCorePlayerDataStorage::Internal_OnPlayerDataStorageDeleteCacheCallback(const EOS_PlayerDataStorage_DeleteCacheCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDeleteCacheCompleteCallback* CallbackObj = static_cast<FDeleteCacheCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCorePlayerDataStorage* PlayerDataStorage = CallbackObj->PlayerDataStorageObject;
		check(PlayerDataStorage);

		if (PlayerDataStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}
