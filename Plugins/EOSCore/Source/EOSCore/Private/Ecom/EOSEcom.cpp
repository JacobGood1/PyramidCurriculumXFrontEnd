/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Ecom/EOSEcom.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreEcom* UCoreEcom::s_EcomObject = nullptr;

UCoreEcom::UCoreEcom()
{
	s_EcomObject = this;
}

void UCoreEcom::EOSEcomQueryOwnership(UObject* worldContextObject, FEOSEcomQueryOwnershipOptions options, const FOnEcomQueryOwnershipCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		char CatalogNamespaceAnsi[512];
		FCStringAnsi::Strncpy(CatalogNamespaceAnsi, TCHAR_TO_UTF8(*options.CatalogNamespace), 512);

		EOS_Ecom_QueryOwnershipOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.CatalogItemIds = new EOS_Ecom_CatalogItemId[static_cast<uint32_t>(options.CatalogItemIds.Num())];
		EcomOptions.CatalogItemIdCount = static_cast<uint32_t>(options.CatalogItemIds.Num());
		EcomOptions.CatalogNamespace = CatalogNamespaceAnsi;

		for (int32 i = 0; i < options.CatalogItemIds.Num(); i++)
		{
			EcomOptions.CatalogItemIds[i] = options.CatalogItemIds[i];
		}

		FEcomQueryOwnershipCallback* CallbackObj = new FEcomQueryOwnershipCallback({ this, callback });
		EOS_Ecom_QueryOwnership(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnQueryOwnershipCallback);

		delete[] EcomOptions.CatalogItemIds;
	}
}

void UCoreEcom::EOSEcomQueryOwnershipToken(UObject* worldContextObject, FEOSEcomQueryOwnershipTokenOptions options, const FOnEcomQueryOwnershipTokenCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		char CatalogNamespaceAnsi[512];
		FCStringAnsi::Strncpy(CatalogNamespaceAnsi, TCHAR_TO_UTF8(*options.CatalogNamespace), 512);

		EOS_Ecom_QueryOwnershipTokenOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.CatalogItemIds = new EOS_Ecom_CatalogItemId[static_cast<uint32_t>(options.CatalogItemIds.Num())];
		EcomOptions.CatalogItemIdCount = static_cast<uint32_t>(options.CatalogItemIds.Num());
		EcomOptions.CatalogNamespace = CatalogNamespaceAnsi;


		for (int32 i = 0; i < options.CatalogItemIds.Num(); i++)
		{
			EcomOptions.CatalogItemIds[i] = options.CatalogItemIds[i];
		}

		FEcomQueryOwnershipTokenCallback* CallbackObj = new FEcomQueryOwnershipTokenCallback({ this, callback });
		EOS_Ecom_QueryOwnershipToken(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnQueryOwnershipTokenCallback);

		delete[] EcomOptions.CatalogItemIds;
	}
}

void UCoreEcom::EOSEcomQueryEntitlements(UObject* worldContextObject, FEOSEcomQueryEntitlementsOptions options, const FOnEcomQueryEntitlementsCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_QueryEntitlementsOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementNames = new EOS_Ecom_EntitlementName[options.EntitlementNames.Num()];
		EcomOptions.EntitlementNameCount = static_cast<uint32_t>(options.EntitlementNames.Num());
		EcomOptions.bIncludeRedeemed = options.bIncludeRedeemed;

		for (int32 i = 0; i < options.EntitlementNames.Num(); i++)
		{
			EcomOptions.EntitlementNames[i] = options.EntitlementNames[i];
		}

		FEcomQueryEntitlementsCallback* CallbackObj = new FEcomQueryEntitlementsCallback({ this, callback });
		EOS_Ecom_QueryEntitlements(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnQueryEntitlementsCallback);

		delete[] EcomOptions.EntitlementNames;
	}
}

void UCoreEcom::EOSEcomQueryOffers(UObject* worldContextObject, FEOSEcomQueryOffersOptions options, const FOnEcomQueryQueryOffersCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		char OverrideCatalogNamespaceAnsi[512];
		FCStringAnsi::Strncpy(OverrideCatalogNamespaceAnsi, TCHAR_TO_UTF8(*options.OverrideCatalogNamespace), 512);

		EOS_Ecom_QueryOffersOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OverrideCatalogNamespace = OverrideCatalogNamespaceAnsi;

		FEcomQueryOffersCallback* CallbackObj = new FEcomQueryOffersCallback({ this, callback });
		EOS_Ecom_QueryOffers(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnQueryOffersCallback);
	}
}

void UCoreEcom::EOSEcomCheckout(UObject* worldContextObject, FEOSEcomCheckoutOptions options, const FOnEcomQueryCheckoutCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		char OverrideCatalogNamespaceAnsi[512];
		FCStringAnsi::Strncpy(OverrideCatalogNamespaceAnsi, TCHAR_TO_UTF8(*options.OverrideCatalogNamespace), 512);

		EOS_Ecom_CheckoutOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OverrideCatalogNamespace = OverrideCatalogNamespaceAnsi;
		EcomOptions.EntryCount = static_cast<uint32_t>(options.Entries.Num());

		EOS_Ecom_CheckoutEntry* CheckoutEntries = new EOS_Ecom_CheckoutEntry[options.Entries.Num()];

		for (int32 i = 0; i < options.Entries.Num(); i++)
		{
			CheckoutEntries[i].ApiVersion = options.Entries[i].ApiVersion;
			CheckoutEntries[i].OfferId = options.Entries[i].OfferId;
		}

		EcomOptions.Entries = CheckoutEntries;

		FEcomCheckoutCallback* CallbackObj = new FEcomCheckoutCallback({ this, callback });
		EOS_Ecom_Checkout(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnCheckoutCallback);

		delete[] CheckoutEntries;
	}
}

void UCoreEcom::EOSEcomRedeemEntitlements(UObject* worldContextObject, FEOSEcomRedeemEntitlementsOptions options, const FOnEcomQueryRedeemEntitlementsCallback& callback)
{
	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_RedeemEntitlementsOptions EcomOptions = {  };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementIdCount = static_cast<uint32_t>(options.EntitlementIds.Num());

		EcomOptions.EntitlementIds = new EOS_Ecom_EntitlementId[options.EntitlementIds.Num()];

		for (int32 i = 0; i < options.EntitlementIds.Num(); i++)
		{
			EcomOptions.EntitlementIds[i] = options.EntitlementIds[i];
		}

		FEcomRedeemEntitlementsCallback* CallbackObj = new FEcomRedeemEntitlementsCallback({ this, callback });
		EOS_Ecom_RedeemEntitlements(GetEcomHandle(worldContextObject), &EcomOptions, CallbackObj, Internal_OnRedeemEntitlementsCallback);

		delete[] EcomOptions.EntitlementIds;
	}
}

int32 UCoreEcom::EOSEcomGetEntitlementsCount(UObject* worldContextObject, FEOSEcomGetEntitlementsCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetEntitlementsCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Ecom_GetEntitlementsCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetEntitlementsByNameCount(UObject* worldContextObject, FEOSEcomGetEntitlementsByNameCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetEntitlementsByNameCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementName = options.EntitlementName;

		Result = EOS_Ecom_GetEntitlementsByNameCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyEntitlementByIndex(UObject* worldContextObject, FEOSEcomCopyEntitlementByIndexOptions options, FEOSEcomEntitlement& outEntitlement)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyEntitlementByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementIndex = static_cast<uint32_t>(options.EntitlementIndex);

		EOS_Ecom_Entitlement* Entitlement = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyEntitlementByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &Entitlement));

		if (Result == EOSResult::EOS_Success)
		{
			outEntitlement = *Entitlement;
			EOS_Ecom_Entitlement_Release(Entitlement);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyEntitlementByNameAndIndex(UObject* worldContextObject, FEOSEcomCopyEntitlementByNameAndIndexOptions options, FEOSEcomEntitlement& outEntitlement)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyEntitlementByNameAndIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementName = options.EntitlementName;
		EcomOptions.Index = static_cast<uint32_t>(options.Index);

		EOS_Ecom_Entitlement* Entitlement = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyEntitlementByNameAndIndex(GetEcomHandle(worldContextObject), &EcomOptions, &Entitlement));

		if (Result == EOSResult::EOS_Success)
		{
			outEntitlement = *Entitlement;
			EOS_Ecom_Entitlement_Release(Entitlement);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyEntitlementById(UObject* worldContextObject, FEOSEcomCopyEntitlementByIdOptions options, FEOSEcomEntitlement& outEntitlement)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyEntitlementByIdOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.EntitlementId = options.EntitlementId;

		EOS_Ecom_Entitlement* Entitlement = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyEntitlementById(GetEcomHandle(worldContextObject), &EcomOptions, &Entitlement));

		if (Result == EOSResult::EOS_Success)
		{
			outEntitlement = *Entitlement;
			EOS_Ecom_Entitlement_Release(Entitlement);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetOfferCount(UObject* worldContextObject, FEOSEcomGetOfferCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetOfferCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Ecom_GetOfferCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyOfferByIndex(UObject* worldContextObject, FEOSEcomCopyOfferByIndexOptions options, FEOSEcomCatalogOffer& outOffer)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyOfferByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferIndex = static_cast<uint32_t>(options.OfferIndex);

		EOS_Ecom_CatalogOffer* CatalogOffer = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyOfferByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &CatalogOffer));

		if (Result == EOSResult::EOS_Success)
		{
			outOffer = *CatalogOffer;
			EOS_Ecom_CatalogOffer_Release(CatalogOffer);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyOfferById(UObject* worldContextObject, FEOSEcomCopyOfferByIdOptions options, FEOSEcomCatalogOffer& outOffer)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyOfferByIdOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferId = options.OfferId;

		EOS_Ecom_CatalogOffer* CatalogOffer = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyOfferById(GetEcomHandle(worldContextObject), &EcomOptions, &CatalogOffer));

		if (Result == EOSResult::EOS_Success)
		{
			outOffer = *CatalogOffer;
			EOS_Ecom_CatalogOffer_Release(CatalogOffer);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetOfferItemCount(UObject* worldContextObject, FEOSEcomGetOfferItemCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetOfferItemCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferId = options.OfferId;

		Result = EOS_Ecom_GetOfferItemCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyOfferItemByIndex(UObject* worldContextObject, FEOSEcomCopyOfferItemByIndexOptions options, FEOSEcomCatalogItem& outItem)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyOfferItemByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferId = options.OfferId;
		EcomOptions.ItemIndex = static_cast<uint32_t>(options.ItemIndex);

		EOS_Ecom_CatalogItem* CatalogItem = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyOfferItemByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &CatalogItem));

		if (Result == EOSResult::EOS_Success)
		{
			outItem = *CatalogItem;
			EOS_Ecom_CatalogItem_Release(CatalogItem);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyItemById(UObject* worldContextObject, FEOSEcomCopyItemByIdOptions options, FEOSEcomCatalogItem& outItem)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyItemByIdOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.ItemId = options.ItemId;

		EOS_Ecom_CatalogItem* CatalogItem = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyItemById(GetEcomHandle(worldContextObject), &EcomOptions, &CatalogItem));

		if (Result == EOSResult::EOS_Success)
		{
			outItem = *CatalogItem;
			EOS_Ecom_CatalogItem_Release(CatalogItem);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetOfferImageInfoCount(UObject* worldContextObject, FEOSEcomGetOfferImageInfoCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetOfferImageInfoCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferId = options.OfferId;

		Result = EOS_Ecom_GetOfferImageInfoCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyOfferImageInfoByIndex(UObject* worldContextObject, FEOSEcomCopyOfferImageInfoByIndexOptions options, FEOSEcomKeyImageInfo& outImageInfo)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyOfferImageInfoByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.OfferId = options.OfferId;
		EcomOptions.ImageInfoIndex = static_cast<uint32_t>(options.ImageInfoIndex);

		EOS_Ecom_KeyImageInfo* KeyImageInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyOfferImageInfoByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &KeyImageInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outImageInfo = *KeyImageInfo;
			EOS_Ecom_KeyImageInfo_Release(KeyImageInfo);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetItemImageInfoCount(UObject* worldContextObject, FEOSEcomGetItemImageInfoCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetItemImageInfoCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.ItemId = options.ItemId;

		Result = EOS_Ecom_GetItemImageInfoCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyItemImageInfoByIndex(UObject* worldContextObject, FEOSEcomCopyItemImageInfoByIndexOptions options, FEOSEcomKeyImageInfo& outImageInfo)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyItemImageInfoByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.ItemId = options.ItemId;
		EcomOptions.ImageInfoIndex = static_cast<uint32_t>(options.ImageInfoIndex);

		EOS_Ecom_KeyImageInfo* KeyImageInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyItemImageInfoByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &KeyImageInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outImageInfo = *KeyImageInfo;
			EOS_Ecom_KeyImageInfo_Release(KeyImageInfo);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetItemReleaseCount(UObject* worldContextObject, FEOSEcomGetItemReleaseCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetItemReleaseCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.ItemId = options.ItemId;

		Result = EOS_Ecom_GetItemReleaseCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyItemReleaseByIndex(UObject* worldContextObject, FEOSEcomCopyItemReleaseByIndexOptions options, FEOSEcomCatalogRelease& outRelease)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyItemReleaseByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.ItemId = options.ItemId;
		EcomOptions.ReleaseIndex = static_cast<uint32_t>(options.ReleaseIndex);


		EOS_Ecom_CatalogRelease* CatalogRelease = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyItemReleaseByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &CatalogRelease));

		if (Result == EOSResult::EOS_Success)
		{
			outRelease = *CatalogRelease;
			EOS_Ecom_CatalogRelease_Release(CatalogRelease);
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomGetTransactionCount(UObject* worldContextObject, FEOSEcomGetTransactionCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetTransactionCountOptions EcomOptions = {  };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Ecom_GetTransactionCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyTransactionByIndex(UObject* worldContextObject, FEOSEcomCopyTransactionByIndexOptions options, FEOSEcomHTransaction& outTransaction)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_CopyTransactionByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.TransactionIndex = static_cast<uint32_t>(options.TransactionIndex);

		EOS_Ecom_HTransaction Transaction;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyTransactionByIndex(GetEcomHandle(worldContextObject), &EcomOptions, &Transaction));

		if (Result == EOSResult::EOS_Success)
		{
			outTransaction = Transaction;
			EOS_Ecom_Transaction_Release(Transaction);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomCopyTransactionById(UObject* worldContextObject, FEOSEcomCopyTransactionByIdOptions options, FEOSEcomHTransaction& outTransaction)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		char TransactionIdAnsi[512];
		FCStringAnsi::Strncpy(TransactionIdAnsi, TCHAR_TO_UTF8(*options.TransactionId), 512);

		EOS_Ecom_CopyTransactionByIdOptions EcomOptions = {  };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;
		EcomOptions.TransactionId = TransactionIdAnsi;

		EOS_Ecom_HTransaction Transaction;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_CopyTransactionById(GetEcomHandle(worldContextObject), &EcomOptions, &Transaction));

		if (Result == EOSResult::EOS_Success)
		{
			outTransaction = Transaction;
			EOS_Ecom_Transaction_Release(Transaction);
		}
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomTransaction_GetTransactionId(UObject* worldContextObject, const FEOSEcomHTransaction& handle, FString& transactionId)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		char Buffer[4096];
		int32_t Size = 0;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_Transaction_GetTransactionId(handle, Buffer, &Size));

		if (Result == EOSResult::EOS_Success)
		{
			transactionId = Buffer;
		}
	}

	return Result;
}

int32 UCoreEcom::EOSEcomTransaction_GetEntitlementsCount(UObject* worldContextObject, const FEOSEcomHTransaction& handle, FEOSEcomTransactionGetEntitlementsCountOptions options)
{
	int32 Result = 0;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_GetEntitlementsCountOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Ecom_GetEntitlementsCount(GetEcomHandle(worldContextObject), &EcomOptions);
	}

	return Result;
}

EOSResult UCoreEcom::EOSEcomTransaction_CopyEntitlementByIndex(UObject* worldContextObject, const FEOSEcomHTransaction& handle, FEOSEcomTransactionCopyEntitlementByIndexOptions options, FEOSEcomEntitlement& outEntitlement)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetEcomHandle(worldContextObject))
	{
		EOS_Ecom_Transaction_CopyEntitlementByIndexOptions EcomOptions = { };
		EcomOptions.ApiVersion = options.ApiVersion;
		EcomOptions.EntitlementIndex = static_cast<uint32_t>(options.EntitlementIndex);

		EOS_Ecom_Entitlement* Entitlement = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Ecom_Transaction_CopyEntitlementByIndex(handle, &EcomOptions, &Entitlement));

		if (Result == EOSResult::EOS_Success)
		{
			outEntitlement = *Entitlement;
			EOS_Ecom_Entitlement_Release(Entitlement);
		}
	}

	return Result;
}


void UCoreEcom::Internal_OnQueryOwnershipCallback(const EOS_Ecom_QueryOwnershipCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomQueryOwnershipCallback* CallbackObj = static_cast<FEcomQueryOwnershipCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCoreEcom::Internal_OnQueryOwnershipTokenCallback(const EOS_Ecom_QueryOwnershipTokenCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomQueryOwnershipTokenCallback* CallbackObj = static_cast<FEcomQueryOwnershipTokenCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreEcom::Internal_OnQueryEntitlementsCallback(const EOS_Ecom_QueryEntitlementsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomQueryEntitlementsCallback* CallbackObj = static_cast<FEcomQueryEntitlementsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreEcom::Internal_OnQueryOffersCallback(const EOS_Ecom_QueryOffersCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomQueryOffersCallback* CallbackObj = static_cast<FEcomQueryOffersCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreEcom::Internal_OnCheckoutCallback(const EOS_Ecom_CheckoutCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomCheckoutCallback* CallbackObj = static_cast<FEcomCheckoutCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreEcom::Internal_OnRedeemEntitlementsCallback(const EOS_Ecom_RedeemEntitlementsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEcomRedeemEntitlementsCallback* CallbackObj = static_cast<FEcomRedeemEntitlementsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreEcom* Ecom = CallbackObj->EcomObject;
		check(Ecom);

		if (Ecom)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}
