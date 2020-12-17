/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "TitleStorage/EOSTitleStorage.h"
#include "eos_titlestorage.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

static size_t MaxChunkSizeTitleStorage = 4 * 4 * 4096;

UCoreTitleStorage* UCoreTitleStorage::s_TitleStorageObject = nullptr;
FTitleStorageTransferInProgress UCoreTitleStorage::CurrentRead;

UCoreTitleStorage::UCoreTitleStorage()
{
	s_TitleStorageObject = this;
}

void UCoreTitleStorage::EOSTitleStorageQueryFile(UObject* worldContextObject, const FEOSTitleStorageQueryFileOptions options, const FOnTitleStorageQueryFileCompleteCallback& callback)
{
	LogVerbose("");

	if (GetTitleStorageHandle(worldContextObject))
	{
		char FilenameAnsi[EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FilenameAnsi, TCHAR_TO_UTF8(*options.Filename), EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_TitleStorage_QueryFileOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;
		TitleStorageOptions.Filename = FilenameAnsi;

		FQueryFileOptionsCallback* CallbackObj = new FQueryFileOptionsCallback({ this, callback });
		EOS_TitleStorage_QueryFile(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, CallbackObj, Internal_OnTitleStorageQueryFileCallback);
	}
}

void UCoreTitleStorage::EOSTitleStorageQueryFileList(UObject* worldContextObject, const FEOSTitleStorageQueryFileListOptions options, const FOnTitleStorageQueryFileListCompleteCallback& callback)
{
	LogVerbose("");

	if (GetTitleStorageHandle(worldContextObject))
	{
		EOS_TitleStorage_QueryFileListOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;

		TArray<const char*> DataArray;

		for (int32 i = 0; i < options.ListOfTags.Num(); i++)
		{
			DataArray.Add(TCHAR_TO_UTF8(*options.ListOfTags[i]));
		}

		TitleStorageOptions.ListOfTagsCount = options.ListOfTags.Num();

		TitleStorageOptions.ListOfTags = DataArray.GetData();

		FQueryFileListOptionsCallback* CallbackObj = new FQueryFileListOptionsCallback({ this, callback });
		EOS_TitleStorage_QueryFileList(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, CallbackObj, Internal_OnTitleStorageQueryFileListCallback);
	}
}

EOSResult UCoreTitleStorage::EOSTitleStorageCopyFileMetadataByFilename(UObject* worldContextObject, const FEOSTitleStorageCopyFileMetadataByFilenameOptions options, FEOSTitleStorageFileMetadata& outMetadata)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		char FilenameAnsi[EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FilenameAnsi, TCHAR_TO_UTF8(*options.Filename), EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_TitleStorage_CopyFileMetadataByFilenameOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;
		TitleStorageOptions.Filename = FilenameAnsi;

		EOS_TitleStorage_FileMetadata* MetaData = NULL;
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorage_CopyFileMetadataByFilename(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, &MetaData));

		if (Result == EOSResult::EOS_Success)
		{
			outMetadata = *MetaData;
			EOS_TitleStorage_FileMetadata_Release(MetaData);
		}
	}

	return Result;
}

int32 UCoreTitleStorage::EOSTitleStorageGetFileMetadataCount(UObject* worldContextObject, const FEOSTitleStorageGetFileMetadataCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetTitleStorageHandle(worldContextObject))
	{
		EOS_TitleStorage_GetFileMetadataCountOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;

		Result = EOS_TitleStorage_GetFileMetadataCount(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions);
	}

	return Result;
}

EOSResult UCoreTitleStorage::EOSTitleStorageCopyFileMetadataAtIndex(UObject* worldContextObject, FEOSTitleStorageCopyFileMetadataAtIndexOptions options, FEOSTitleStorageFileMetadata& outMetadata)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		EOS_TitleStorage_CopyFileMetadataAtIndexOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;
		TitleStorageOptions.Index = static_cast<uint32_t>(options.Index);

		EOS_TitleStorage_FileMetadata* MetaData = NULL;
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorage_CopyFileMetadataAtIndex(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, &MetaData));

		if (Result == EOSResult::EOS_Success)
		{
			outMetadata = *MetaData;
			EOS_TitleStorage_FileMetadata_Release(MetaData);
		}
	}

	return Result;
}

FEOSTitleStorageFileTransferRequestHandle UCoreTitleStorage::EOSTitleStorageReadFile(UObject* worldContextObject, FEOSTitleStorageReadFileOptions options, const FOnTitleStorageReadFileCompleteCallback& callback)
{
	LogVerbose("");

	FEOSTitleStorageFileTransferRequestHandle Handle;

	if (GetTitleStorageHandle(worldContextObject))
	{
		char FilenameAnsi[EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES];
		FCStringAnsi::Strncpy(FilenameAnsi, TCHAR_TO_UTF8(*options.Filename), EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES);

		EOS_TitleStorage_ReadFileOptions TitleStorageOptions = { };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;
		TitleStorageOptions.Filename = FilenameAnsi;
		TitleStorageOptions.ReadChunkLengthBytes = static_cast<uint32_t>(MaxChunkSizeTitleStorage);
		TitleStorageOptions.ReadFileDataCallback = OnReadFileDataCallback;
		TitleStorageOptions.FileTransferProgressCallback = OnFileTransferProgressCallback;

		FReadFileCallback* CallbackObj = new FReadFileCallback({ this, callback });
		Handle = EOS_TitleStorage_ReadFile(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, CallbackObj, Internal_OnTitleStorageReadFileCallback);

		CurrentRead.LocalFile = options.SaveFileName;
		CurrentRead.CurrentTransferHandle = Handle;
	}

	return Handle;
}

EOSResult UCoreTitleStorage::EOSTitleStorageDeleteCache(UObject* worldContextObject, FEOSTitleStorageDeleteCacheOptions options, const FOnTitleStorageDeleteCacheCompleteCallback& callback)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		EOS_TitleStorage_DeleteCacheOptions TitleStorageOptions = { options.ApiVersion, options.LocalUserId };
		TitleStorageOptions.ApiVersion = options.ApiVersion;
		TitleStorageOptions.LocalUserId = options.LocalUserId;

		FDeleteCacheCallback* CallbackObj = new FDeleteCacheCallback({ this, callback });
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorage_DeleteCache(GetTitleStorageHandle(worldContextObject), &TitleStorageOptions, CallbackObj, Internal_OnTitleStorageDeleteCacheCallback));
	}

	return Result;
}

EOSResult UCoreTitleStorage::EOSTitleStorageFileTransferRequestGetFileRequestState(UObject* worldContextObject, FEOSTitleStorageFileTransferRequestHandle handle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorageFileTransferRequest_GetFileRequestState(handle));
	}

	return Result;
}

EOSResult UCoreTitleStorage::EOSTitleStorageFileTransferRequestGetFilename(UObject* worldContextObject, FEOSTitleStorageFileTransferRequestHandle handle, int32 filenameStringBufferSizeBytes, FString& outStringBuffer)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		char DataArray[EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES + 1];
		int32_t* Size = nullptr;
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorageFileTransferRequest_GetFilename(handle, (EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES + 1), DataArray, Size));

		if (Result == EOSResult::EOS_Success)
		{
			outStringBuffer = UTF8_TO_TCHAR(DataArray);
		}
	}

	return Result;
}

EOSResult UCoreTitleStorage::EOSTitleStorageFileTransferRequestCancelRequest(UObject* worldContextObject, FEOSTitleStorageFileTransferRequestHandle handle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetTitleStorageHandle(worldContextObject))
	{
		Result = EOSHelpers::ToEOSCoreResult(EOS_TitleStorageFileTransferRequest_CancelRequest(handle));
	}

	return Result;
}

void UCoreTitleStorage::Internal_OnTitleStorageQueryFileCallback(const EOS_TitleStorage_QueryFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryFileOptionsCallback* CallbackObj = static_cast<FQueryFileOptionsCallback*>(data->ClientData);
	check(CallbackObj);
	
	if (CallbackObj)
	{
		UCoreTitleStorage* TitleStorage = CallbackObj->TitleStorageObject;
		check(TitleStorage);

		if (TitleStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}

}

void UCoreTitleStorage::Internal_OnTitleStorageQueryFileListCallback(const EOS_TitleStorage_QueryFileListCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryFileListOptionsCallback* CallbackObj = static_cast<FQueryFileListOptionsCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		UCoreTitleStorage* TitleStorage = CallbackObj->TitleStorageObject;
		check(TitleStorage);

		if (TitleStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);

		}
		delete CallbackObj;
	}
}

void UCoreTitleStorage::Internal_OnTitleStorageDeleteCacheCallback(const EOS_TitleStorage_DeleteCacheCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDeleteCacheCallback* CallbackObj = static_cast<FDeleteCacheCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		UCoreTitleStorage* TitleStorage = CallbackObj->TitleStorageObject;
		check(TitleStorage);

		if (TitleStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);

			delete CallbackObj;
		}
	}
}

void UCoreTitleStorage::Internal_OnTitleStorageReadFileCallback(const EOS_TitleStorage_ReadFileCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FReadFileCallback* CallbackObj = static_cast<FReadFileCallback*>(data->ClientData);
	check(CallbackObj);

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

	CurrentRead.Reset();

	if (CallbackObj)
	{
		UCoreTitleStorage* TitleStorage = CallbackObj->TitleStorageObject;
		check(TitleStorage);

		if (TitleStorage)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);

			delete CallbackObj;
		}
	}
}

EOS_TitleStorage_EReadResult UCoreTitleStorage::OnReadFileDataCallback(const EOS_TitleStorage_ReadFileDataCallbackInfo* data)
{
	LogVerbose("");

	if (data)
	{
		return s_TitleStorageObject->ReceiveData(data->Filename, data->DataChunk, data->DataChunkLengthBytes, data->TotalFileSizeBytes);
	}
	else
	{
		return EOS_TitleStorage_EReadResult::EOS_TS_RR_FailRequest;
	}
}

void UCoreTitleStorage::OnFileTransferProgressCallback(const EOS_TitleStorage_FileTransferProgressCallbackInfo* data)
{
	LogVerbose("Transfered: %d/%d", data->BytesTransferred, data->TotalFileSizeBytes);
}

EOS_TitleStorage_EReadResult UCoreTitleStorage::ReceiveData(const FString& filename, const void* data, size_t numBytes, size_t totalSize)
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
				return EOS_TitleStorage_EReadResult::EOS_TS_RR_ContinueReading;
			}

			CurrentRead.Data.SetNum(totalSize);
		}

		if (CurrentRead.TotalSize - CurrentRead.CurrentIndex >= numBytes)
		{
			memcpy(static_cast<void*>(&CurrentRead.Data[CurrentRead.CurrentIndex]), data, numBytes);
			CurrentRead.CurrentIndex += numBytes;

			return EOS_TitleStorage_EReadResult::EOS_TS_RR_ContinueReading;
		}
		else
		{
			LogWarning("Could not receive data: too much of it!");

			return EOS_TitleStorage_EReadResult::EOS_TS_RR_FailRequest;
		}
	}
	else
	{
		LogWarning("Could not receive data: Data pointer is null!");

		return EOS_TitleStorage_EReadResult::EOS_TS_RR_FailRequest;
	}

	return EOS_TitleStorage_EReadResult::EOS_TS_RR_CancelRequest;
}
