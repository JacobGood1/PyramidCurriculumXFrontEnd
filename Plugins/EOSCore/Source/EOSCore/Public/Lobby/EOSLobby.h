/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
 official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "EOSCoreModule.h"
#include "eos_lobby_types.h"
#include "Lobby/EOSLobbyTypes.h"
#include "EOSLobby.generated.h"

/**
 * The Lobby Interface is used to manage lobbies that provide a persistent connection between users and
 * notifications of data sharing/updates.  Lobbies may also be found by advertising and searching with the backend service.
 * All Lobby Interface calls take a handle of type EOS_HLobby as the first parameter.
 * This handle can be retrieved from a EOS_HPlatform handle by using the EOS_Platform_GetLobbyInterface function.
 *
 * NOTE: At this time, this feature is only available for products that are part of the Epic Games store.
 *
 * @see EOS_Platform_GetLobbyInterface
 */

UCLASS()
class EOSCORE_API UCoreLobby : public UEOSCoreSubsystem
{
	GENERATED_BODY()
public:
    UCoreLobby();
public:
    /**
     * The Lobby Interface is used to manage lobbies that provide a persistent connection between users and
     * notifications of data sharing/updates.  Lobbies may also be found by advertising and searching with the backend service.
     * All Lobby Interface calls take a handle of type EOS_HLobby as the first parameter.
     * This handle can be retrieved from a EOS_HPlatform handle by using the EOS_Platform_GetLobbyInterface function.
     *
     * NOTE: At this time, this feature is only available for products that are part of the Epic Games store.
     *
     * @see EOS_Platform_GetLobbyInterface
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOSCore|Interfaces")
		static UCoreLobby* GetLobby() { return s_LobbyObject; }
public:
    /**
     * Creates a lobby and adds the user to the lobby membership.  There is no data associated with the lobby at the start and can be added vis EOS_Lobby_UpdateLobbyModification
     *
     * @param Options Required fields for the creation of a lobby such as a user count and its starting advertised state
     * @param callback A callback that is fired when the create operation completes, either successfully or in error
     *
     * @return EOS_Success if the creation completes successfully
     *         EOS_InvalidParameters if any of the options are incorrect
     *         EOS_LimitExceeded if the number of allowed lobbies is exceeded
     */
    UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyCreateLobby(UObject* worldContextObject, FEOSLobbyCreateLobbyOptions options, const FOnLobbyCreateLobbyCallback& callback);

	/**
	 * Destroy a lobby given a lobby id
	 *
	 * @param Options Structure containing information about the lobby to be destroyed
	 * @param callback A callback that is fired when the destroy operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the destroy completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_AlreadyPending if the lobby is already marked for destroy
	 *         EOS_NotFound if the lobby to be destroyed does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyDestroyLobby(UObject* worldContextObject, FEOSLobbyDestroyLobbyOptions options, const FOnLobbyDestroyLobbyCallback& callback);

	/**
	 * Join a lobby, creating a local instance under a given lobby id.  Backend will validate various conditions to make sure it is possible to join the lobby.
	 *
	 * @param Options Structure containing information about the lobby to be joined
	 * @param callback A callback that is fired when the join operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the destroy completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyJoinLobby(UObject* worldContextObject, FEOSLobbyJoinLobbyOptions options, const FOnLobbyJoinLobbyCallback& callback);

	/**
	 * Leave a lobby given a lobby id
	 *
	 * @param Options Structure containing information about the lobby to be left
	 * @param callback A callback that is fired when the leave operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the leave completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_AlreadyPending if the lobby is already marked for leave
	 *         EOS_NotFound if a lobby to be left does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyLeaveLobby(UObject* worldContextObject, FEOSLobbyLeaveLobbyOptions options, const FOnLobbyLeaveLobbyCallback& callback);

	/**
	 * Creates a lobby modification handle (UObject* worldContextObject, FEOSHLobbyModification). The lobby modification handle is used to modify an existing lobby and can be applied with EOS_Lobby_UpdateLobby.
	 * The FEOSHLobbyModification must be released by calling EOSLobbyModificationRelease once it is no longer needed.
	 *
	 * @param Options Required fields such as lobby id
	 * @param OutLobbyModificationHandle Pointer to a Lobby Modification Handle only set if successful
	 * @return EOS_Success if we successfully created the Lobby Modification Handle pointed at in OutLobbyModificationHandle, or an error result if the input data was invalid
	 *		   EOS_InvalidParameters if any of the options are incorrect
	 *
	 * @see EOSLobbyModificationRelease
	 * @see EOS_Lobby_UpdateLobby
	 * @see FEOSHLobbyModification
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyUpdateLobbyModification(UObject* worldContextObject, FEOSLobbyUpdateLobbyModificationOptions options, FEOSHLobbyModification& outLobbyModificationHandle);

	/**
	 * Update a lobby given a lobby modification handle created via EOS_Lobby_UpdateLobbyModification
	 *
	 * @param Options Structure containing information about the lobby to be updated
	 * @param callback A callback that is fired when the update operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the update completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_Lobby_NotOwner if the lobby modification contains modifications that are only allowed by the owner
	 *         EOS_NotFound if the lobby to update does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyUpdateLobby(UObject* worldContextObject, FEOSLobbyUpdateLobbyOptions options, const FOnLobbyUpdateLobbyCallback& callback);

	/**
	 * Promote an existing member of the lobby to owner, allowing them to make lobby data modifications
	 *
	 * @param Options Structure containing information about the lobby and member to be promoted
	 * @param callback A callback that is fired when the promotion operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the promote completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_Lobby_NotOwner if the calling user is not the owner of the lobby
	 *         EOS_NotFound if the lobby of interest does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyPromoteMember(UObject* worldContextObject, FEOSLobbyPromoteMemberOptions options, const FOnLobbyPromoteMemberCallback& callback);

	/**
	 * Kick an existing member from the lobby
	 *
	 * @param Options Structure containing information about the lobby and member to be kicked
	 * @param callback A callback that is fired when the kick operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the kick completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_Lobby_NotOwner if the calling user is not the owner of the lobby
	 *         EOS_NotFound if a lobby of interest does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyKickMember(UObject* worldContextObject, FEOSLobbyKickMemberOptions options, const FOnLobbyKickMemberCallback& callback);

	/**
	 * Register to receive notifications when a lobby owner updates the attributes associated with the lobby.
	 * @note must call RemoveNotifyLobbyUpdateReceived to remove the notification
	 *
	 * @param Options Structure containing information about the request.
	 * @param Notification A callback that is fired when a a notification is received.
	 *
	 * @return handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSNotificationId EOSLobbyAddNotifyLobbyUpdateReceived(UObject* worldContextObject, const FOnLobbyUpdateReceivedCallback& callback);

	/**
	 * Unregister from receiving notifications when a lobby changes its data.
	 *
	 * @param id Handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static void EOSLobbyRemoveNotifyLobbyUpdateReceived(UObject* worldContextObject, FEOSNotificationId id);

	/**
	 * Register to receive notifications when a lobby member updates the attributes associated with themselves inside the lobby.
	 * @note must call RemoveNotifyLobbyMemberUpdateReceived to remove the notification
	 *
	 * @param Options Structure containing information about the request.
	 * @param Notification A callback that is fired when a a notification is received.
	 *
	 * @return handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSNotificationId EOSLobbyAddNotifyLobbyMemberUpdateReceived(UObject* worldContextObject, const FOnLobbyMemberUpdateReceivedCallback& callback);

	/**
	 * Unregister from receiving notifications when lobby members change their data.
	 *
	 * @param id Handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static void EOSLobbyRemoveNotifyLobbyMemberUpdateReceived(UObject* worldContextObject, FEOSNotificationId id);

	/**
	 * Register to receive notifications about the changing status of lobby members.
	 * @note must call RemoveNotifyLobbyMemberStatusReceived to remove the notification
	 *
	 * @param Options Structure containing information about the request.
	 * @param Notification A callback that is fired when a a notification is received.
	 *
	 * @return handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSNotificationId EOSLobbyAddNotifyLobbyMemberStatusReceived(UObject* worldContextObject, const FOnLobbyMemberStatusReceivedCallback& callback);

	/**
	 * Unregister from receiving notifications when lobby members status change.
	 *
	 * @param id Handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static void EOSLobbyRemoveNotifyLobbyMemberStatusReceived(UObject* worldContextObject, FEOSNotificationId id);

	/**
	 * Send an invite to another user.  User must be a member of the lobby or else the call will fail
	 *
	 * @param Options Structure containing information about the lobby and user to invite

	 * @param callback A callback that is fired when the send invite operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the send invite completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_NotFound if the lobby to send the invite from does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbySendInvite(UObject* worldContextObject, FEOSLobbySendInviteOptions options, const FOnLobbySendInviteCallback& callback);

	/**
	 * Reject an invite from another user.
	 *
	 * @param Options Structure containing information about the invite to reject
	 * @param callback A callback that is fired when the reject invite operation completes, either successfully or in error
	 *
	 * @return EOS_Success if the invite rejection completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_NotFound if the invite does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyRejectInvite(UObject* worldContextObject, FEOSLobbyRejectInviteOptions options, const FOnLobbyRejectInviteCallback& callback);

	/**
	 * Retrieve all existing invites for a single user
	 *
	 * @param Options Structure containing information about the invites to query
	 * @param callback A callback that is fired when the query invites operation completes, either successfully or in error
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbyQueryInvites(UObject* worldContextObject, FEOSLobbyQueryInvitesOptions options, const FOnLobbyQueryInvitesCallback& callback);

	/**
	 * Get the number of known invites for a given user
	 *
	 * @param Options the Options associated with retrieving the current invite count
	 *
	 * @return number of known invites for a given user or 0 if there is an error
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static int32 EOSLobbyGetInviteCount(UObject* worldContextObject, FEOSLobbyGetInviteCountOptions options);

	/**
	 * Retrieve an invite id from a list of active invites for a given user
	 *
	 * @param Options Structure containing the input parameters
	 *
	 * @return EOS_Success if the input is valid and an invite id was returned
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_NotFound if the invite doesn't exist
	 *
	 * @see EOS_Lobby_GetInviteCount
	 * @see EOS_Lobby_CopyLobbyDetailsHandleByInviteId
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyGetInviteIdByIndex(UObject* worldContextObject, FEOSLobbyGetInviteIdByIndexOptions options, FLobbyInviteId& inviteId);

	/**
	 * Create a lobby search handle.  This handle may be modified to include various search parameters.
	 * Searching is possible in three methods, all mutually exclusive
	 * - set the lobby id to find a specific lobby
	 * - set the target user id to find a specific user
	 * - set lobby parameters to find an array of lobbies that match the search criteria (not available yet)
	 *
	 * @param Options Structure containing required parameters such as the maximum number of search results
	 * @param OutLobbySearchHandle The new search handle or null if there was an error creating the search handle
	 *
	 * @return EOS_Success if the search creation completes successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyCreateLobbySearch(UObject* worldContextObject, FEOSLobbyCreateLobbySearchOptions options, FEOSHLobbySearch& outLobbySearchHandle);

	/**
	 * Register to receive notifications about lobby invites sent to local users.
	 * @note must call RemoveNotifyLobbyInviteReceived to remove the notification
	 *
	 * @param Options Structure containing information about the request.
	 * @param Notification A callback that is fired when a a notification is received.
	 *
	 * @return handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSNotificationId EOSLobbyAddNotifyLobbyInviteReceived(UObject* worldContextObject, const FOnLobbyInviteReceivedCallback& callback);

	/**
	 * Unregister from receiving notifications when a user receives a lobby invitation.
	 *
	 * @param id Handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static void EOSLobbyRemoveNotifyLobbyInviteReceived(UObject* worldContextObject, FEOSNotificationId id);

	/**
	 * Register to receive notifications about lobby invites accepted by local user via the overlay.
	 * @note must call RemoveNotifyLobbyInviteAccepted to remove the notification
	 *
	 * @param Options Structure containing information about the request.
	 * @param ClientData Arbitrary data that is passed back to you in the CompletionDelegate.
	 * @param Notification A callback that is fired when a a notification is received.
	 *
	 * @return handle representing the registered callback
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSNotificationId EOSLobbyAddNotifyLobbyInviteAccepted(UObject* worldContextObject, FEOSLobbyAddNotifyLobbyInviteAcceptedOptions options, const FOnLobbyInviteAcceptedCallback& callback);

	/**
	* Unregister from receiving notifications when a user accepts a lobby invitation via the overlay.
	*
	* @param InId Handle representing the registered callback
	*/
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static void EOSLobbyRemoveNotifyLobbyInviteAccepted(UObject* worldContextObject, FEOSNotificationId id);

	/**
	  * EOS_Lobby_CopyLobbyDetailsHandleByInviteId is used to immediately retrieve a handle to the lobby information from after notification of an invite
	  * If the call returns an EOS_Success result, the out parameter, OutLobbyDetailsHandle, must be passed to EOSLobbyDetailsRelease to release the memory associated with it.
	  *
	  * @param Options Structure containing the input parameters
	  * @param OutLobbyDetailsHandle out parameter used to receive the lobby handle
	  *
	  * @return EOS_Success if the information is available and passed out in OutLobbyDetailsHandle
	  *         EOS_InvalidParameters if you pass an invalid invite id or a null pointer for the out parameter
	  *         EOS_IncompatibleVersion if the API version passed in is incorrect
	  *         EOS_NotFound If the invite id cannot be found
	  *
	  * @see EOS_Lobby_CopyLobbyDetailsHandleByInviteIdOptions
	  * @see EOSLobbyDetailsRelease
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyCopyLobbyDetailsHandleByInviteId(UObject* worldContextObject, FEOSLobbyCopyLobbyDetailsHandleByInviteIdOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle);

	/**
	 * Create a handle to an existing lobby.
	 * If the call returns an EOS_Success result, the out parameter, OutLobbyDetailsHandle, must be passed to EOSLobbyDetailsRelease to release the memory associated with it.
	 *
	 * @param Options Structure containing information about the lobby to retrieve
	 * @param OutLobbyDetailsHandle The new active lobby handle or null if there was an error
	 *
	 * @return EOS_Success if the lobby handle was created successfully
	 *         EOS_InvalidParameters if any of the options are incorrect
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *         EOS_NotFound if the lobby doesn't exist
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyCopyLobbyDetailsHandle(UObject* worldContextObject, FEOSLobbyCopyLobbyDetailsHandleOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle);

	/**
	 * To modify lobbies or the lobby member data, you must call EOS_Lobby_UpdateLobbyModification to create a Lobby Modification handle. To modify that handle, call
	 * FEOSHLobbyModification methods. Once you are finished, call EOS_Lobby_UpdateLobby with your handle. You must then release your Lobby Modification
	 * handle by calling EOSLobbyModificationRelease.
	 */

	 /**
	  * Set the permissions associated with this lobby.
	  * The permissions range from "public" to "invite only" and are described by EOS_ELobbyPermissionLevel
	  *
	  * @param Options Options associated with the permission level of the lobby
	  *
	  * @return EOS_Success if setting this parameter was successful
	  *         EOS_IncompatibleVersion if the API version passed in is incorrect
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationSetPermissionLevel(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationSetPermissionLevelOptions options);

	/**
	 * Set the maximum number of members allowed in this lobby.
	 * When updating the lobby, it is not possible to reduce this number below the current number of existing members
	 *
	 * @param Options Options associated with max number of members in this lobby
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationSetMaxMembers(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationSetMaxMembersOptions options);

	/**
	 * Associate an attribute with this lobby
	 * An attribute is something may be public or private with the lobby.
	 * If public, it can be queried for in a search, otherwise the data remains known only to lobby members
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationAddAttributeOptions options);

	/**
	 * Associate an attribute with this lobby
	 * An attribute is something may be public or private with the lobby.
	 * If public, it can be queried for in a search, otherwise the data remains known only to lobby members
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddAttributeBool(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, bool bValue, EEOSELobbyAttributeVisibility visibility);
	/**
	 * Associate an attribute with this lobby
	 * An attribute is something may be public or private with the lobby.
	 * If public, it can be queried for in a search, otherwise the data remains known only to lobby members
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddAttributeDouble(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);
	/**
	* Associate an attribute with this lobby
	* An attribute is something may be public or private with the lobby.
	* If public, it can be queried for in a search, otherwise the data remains known only to lobby members
	*
	* @param Options Options to set the attribute and its visibility state
	*
	* @return EOS_Success if setting this parameter was successful
	*		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	*         EOS_IncompatibleVersion if the API version passed in is incorrect
	*/
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddAttributeInt64(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);

	/**
	  * Associate an attribute with this lobby
	  * An attribute is something may be public or private with the lobby.
	  * If public, it can be queried for in a search, otherwise the data remains known only to lobby members
	  *
	  * @param Options Options to set the attribute and its visibility state
	  *
	  * @return EOS_Success if setting this parameter was successful
	  *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	  *         EOS_IncompatibleVersion if the API version passed in is incorrect
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddAttributeString(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);

	/**
	 * Remove an attribute associated with the lobby
	 *
	 * @param Options Specify the key of the attribute to remove
	 *
	 * @return EOS_Success if removing this parameter was successful
	 *		   EOS_InvalidParameters if the key is null or empty
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationRemoveAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationRemoveAttributeOptions options);

	/**
	 * Associate an attribute with a member of the lobby
	 * Lobby member data is always private to the lobby
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	static EOSResult EOSLobbyModificationAddMemberAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationAddMemberAttributeOptions options);

	/**
	 * Associate an attribute with a member of the lobby
	 * Lobby member data is always private to the lobby
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddMemberAttributeBool(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, bool bValue, EEOSELobbyAttributeVisibility visibility);

	/**
	  * Associate an attribute with a member of the lobby
	  * Lobby member data is always private to the lobby
	  *
	  * @param Options Options to set the attribute and its visibility state
	  *
	  * @return EOS_Success if setting this parameter was successful
	  *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	  *         EOS_IncompatibleVersion if the API version passed in is incorrect
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddMemberAttributeDouble(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);
	/**
	* Associate an attribute with a member of the lobby
	* Lobby member data is always private to the lobby
	*
	* @param Options Options to set the attribute and its visibility state
	*
	* @return EOS_Success if setting this parameter was successful
	*		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	*         EOS_IncompatibleVersion if the API version passed in is incorrect
	*/
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddMemberAttributeInt64(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);

	/**
	 * Associate an attribute with a member of the lobby
	 * Lobby member data is always private to the lobby
	 *
	 * @param Options Options to set the attribute and its visibility state
	 *
	 * @return EOS_Success if setting this parameter was successful
	 *		   EOS_InvalidParameters if the attribute is missing information or otherwise invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationAddMemberAttributeString(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility);

	/**
	 * Remove an attribute associated with of member of the lobby
	 *
	 * @param Options Specify the key of the member attribute to remove
	 *
	 * @return EOS_Success if removing this parameter was successful
	 *		   EOS_InvalidParameters if the key is null or empty
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyModificationRemoveMemberAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationRemoveMemberAttributeOptions options);

	/**
	 * A "read only" representation of an existing lobby that games interact with externally.
	 * Both the lobby and lobby search interfaces interface use this common class for lobby management and search results
	 */

	 /**
	  * Get the product user id of the current owner for a given lobby
	  *
	  * @param Options Structure containing the input parameters
	  *
	  * @return the product user id for the lobby owner or null if the input parameters are invalid
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSProductUserId EOSLobbyDetailsGetLobbyOwner(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetLobbyOwnerOptions options);

	/**
	 * EOSLobbyDetailsCopyInfo is used to immediately retrieve a copy of lobby information from a given source such as a existing lobby or a search result.
	 * If the call returns an EOS_Success result, the out parameter, OutLobbyDetailsInfo, must be passed to EOSLobbyDetailsInfo_Release to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutLobbyDetailsInfo Out parameter used to receive the EOSLobbyDetailsInfo structure.
	 *
	 * @return EOS_Success if the information is available and passed out in OutLobbyDetailsInfo
	 *         EOS_InvalidParameters if you pass a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOSLobbyDetailsInfo
	 * @see EOSLobbyDetailsCopyInfoOptions
	 * @see EOSLobbyDetailsInfo_Release
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static  EOSResult EOSLobbyDetailsCopyInfo(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyInfoOptions options, FEOSLobbyDetailsInfo& outLobbyDetailsInfo);

	/**
	 * Get the number of attributes associated with this lobby
	 *
	 * @param Options the Options associated with retrieving the attribute count
	 *
	 * @return number of attributes on the lobby or 0 if there is an error
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static int32 EOSLobbyDetailsGetAttributeCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetAttributeCountOptions options);

	/**
	 * EOSLobbyDetailsCopyAttributeByIndex is used to immediately retrieve a copy of a lobby attribute from a given source such as a existing lobby or a search result.
	 * If the call returns an EOS_Success result, the out parameter, OutAttribute, must be passed to EOS_Lobby_Attribute_Release to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutAttribute Out parameter used to receive the EOS_Lobby_Attribute structure.
	 *
	 * @return EOS_Success if the information is available and passed out in OutAttribute
	 *         EOS_InvalidParameters if you pass a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOS_Lobby_Attribute
	 * @see EOSLobbyDetailsCopyAttributeByIndexOptions
	 * @see EOS_Lobby_Attribute_Release
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyDetailsCopyAttributeByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyAttributeByIndexOptions options, FEOSLobbyAttribute& outAttribute);

	/**
	 * EOSLobbyDetailsCopyAttributeByKey is used to immediately retrieve a copy of a lobby attribute from a given source such as a existing lobby or a search result.
	 * If the call returns an EOS_Success result, the out parameter, OutAttribute, must be passed to EOS_Lobby_Attribute_Release to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutAttribute Out parameter used to receive the EOS_Lobby_Attribute structure.
	 *
	 * @return EOS_Success if the information is available and passed out in OutAttribute
	 *         EOS_InvalidParameters if you pass a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOS_Lobby_Attribute
	 * @see EOSLobbyDetailsCopyAttributeByKeyOptions
	 * @see EOS_Lobby_Attribute_Release
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyDetailsCopyAttributeByKey(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyAttributeByKeyOptions options, FEOSLobbyAttribute& outAttribute);

	/**
	 * Get the number of members associated with this lobby
	 *
	 * @param Options the Options associated with retrieving the member count
	 *
	 * @return number of members in the existing lobby or 0 if there is an error
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static int32 EOSLobbyDetailsGetMemberCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberCountOptions options);

	/**
	 * EOSLobbyDetailsGetMemberByIndex is used to immediately retrieve individual members registered with a lobby.
	 *
	 * @param Options Structure containing the input parameters
	 *
	 * @return the product user id for the registered member at a given index or null if that index is invalid
	 *
	 * @see EOSLobbyDetailsGetMemberCount
	 * @see EOSLobbyDetailsGetMemberByIndexOptions
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static FEOSProductUserId EOSLobbyDetailsGetMemberByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberByIndexOptions options);

	/**
	 * EOSLobbyDetailsGetMemberAttributeCount is used to immediately retrieve the attribute count for members in a lobby.
	 *
	 * @param Options Structure containing the input parameters
	 *
	 * @return the number of attributes associated with a given lobby member or 0 if that member is invalid
	 *
	 * @see EOSLobbyDetailsGetMemberCount
	 * @see EOSLobbyDetailsGetMemberAttributeCountOptions
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static int32 EOSLobbyDetailsGetMemberAttributeCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberAttributeCountOptions options);

	/**
	 * EOSLobbyDetailsCopyMemberAttributeByIndex is used to immediately retrieve a copy of a lobby member attribute from an existing lobby.
	 * If the call returns an EOS_Success result, the out parameter, OutAttribute, must be passed to EOS_Lobby_Attribute_Release to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutAttribute Out parameter used to receive the EOS_Lobby_Attribute structure.
	 *
	 * @return EOS_Success if the information is available and passed out in OutAttribute
	 *         EOS_InvalidParameters if you pass a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOS_Lobby_Attribute
	 * @see EOSLobbyDetailsCopyMemberAttributeByIndexOptions
	 * @see EOS_Lobby_Attribute_Release
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyDetailsCopyMemberAttributeByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyMemberAttributeByIndexOptions options, FEOSLobbyAttribute& outAttribute);

	/**
	 * EOSLobbyDetailsCopyMemberAttributeByKey is used to immediately retrieve a copy of a lobby member attribute from an existing lobby.
	 * If the call returns an EOS_Success result, the out parameter, OutAttribute, must be passed to EOS_Lobby_Attribute_Release to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutAttribute Out parameter used to receive the EOS_Lobby_Attribute structure.
	 *
	 * @return EOS_Success if the information is available and passed out in OutAttribute
	 *         EOS_InvalidParameters if you pass a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOS_Lobby_Attribute
	 * @see EOSLobbyDetailsCopyMemberAttributeByKeyOptions
	 * @see EOS_Lobby_Attribute_Release
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbyDetailsCopyMemberAttributeByKey(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyMemberAttributeByKeyOptions options, FEOSLobbyAttribute& outAttribute);

	/**
	 * Class responsible for the creation, setup, and execution of a search query.
	 * Search parameters are defined, the query is executed and the search results are returned within this object
	 */

	 /**
	  * Find lobbies matching the search criteria setup via this lobby search handle.
	  * When the operation completes, this handle will have the search results that can be parsed
	  *
	  * @param Options Structure containing information about the search criteria to use
	  * @param callback A callback that is fired when the search operation completes, either successfully or in error
	  *
	  * @return EOS_Success if the find operation completes successfully
	  *         EOS_NotFound if searching for an individual lobby by lobby id or target user id returns no results
	  *         EOS_InvalidParameters if any of the options are incorrect
	  */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		void EOSLobbySearchFind(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchFindOptions options, const FOnLobbyFindCallback& callback);

	/**
	 * Set a lobby id to find and will return at most one search result.  Setting TargetUserId or SearchParameters will result in EOSLobbySearchFind failing
	 *
	 * @param Options A specific lobby id for which to search
	 *
	 * @return EOS_Success if setting this lobby id was successful
	 *         EOS_InvalidParameters if the lobby id is invalid or null
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetLobbyId(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetLobbyIdOptions options);

	/**
	 * Set a target user id to find.  Setting LobbyId or SearchParameters will result in EOSLobbySearchFind failing
	 * NOTE: a search result will only be found if this user is in a public lobby
	 *
	 * @param Options a specific target user id to find
	 *
	 * @return EOS_Success if setting this target user id was successful
	 *         EOS_InvalidParameters if the target user id is invalid or null
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetTargetUserId(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetTargetUserIdOptions options);



	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetParameterBool(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, bool bValue, EEOSEComparisonOp comparisonOp);
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetParameterDouble(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp);
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetParameterInt64(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp);
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetParameterString(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp);

	/** NYI */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static  EOSResult EOSLobbySearchRemoveParameter(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchRemoveParameterOptions options);

	/**
	 * Set the maximum number of search results to return in the query, can't be more than EOS_LOBBY_MAX_SEARCH_RESULTS
	 *
	 * @param Options maximum number of search results to return in the query
	 *
	 * @return EOS_Success if setting the max results was successful
	 *         EOS_InvalidParameters if the number of results requested is invalid
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchSetMaxResults(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetMaxResultsOptions options);

	/**
	 * Get the number of search results found by the search parameters in this search
	 *
	 * @param Options Options associated with the search count
	 *
	 * @return return the number of search results found by the query or 0 if search is not complete
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static  int32 EOSLobbySearchGetSearchResultCount(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchGetSearchResultCountOptions options);

	/**
	 * EOSLobbySearchCopySearchResultByIndex is used to immediately retrieve a handle to the lobby information from a given search result.
	 * If the call returns an EOS_Success result, the out parameter, OutLobbyDetailsHandle, must be passed to EOSLobbyDetailsRelease to release the memory associated with it.
	 *
	 * @param Options Structure containing the input parameters
	 * @param OutLobbyDetailsHandle out parameter used to receive the lobby details handle
	 *
	 * @return EOS_Success if the information is available and passed out in OutLobbyDetailsHandle
	 *         EOS_InvalidParameters if you pass an invalid index or a null pointer for the out parameter
	 *         EOS_IncompatibleVersion if the API version passed in is incorrect
	 *
	 * @see EOSLobbySearchCopySearchResultByIndexOptions
	 * @see EOSLobbyDetailsRelease
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Lobby", meta = (WorldContext = "worldContextObject"))
		static EOSResult EOSLobbySearchCopySearchResultByIndex(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchCopySearchResultByIndexOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle);
private:
    static EOSResult EOSLobbySearchSetParameter(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetParameterOptions options);
private:
    static void Internal_OnLobbyCreateLobbyCallback(const EOS_Lobby_CreateLobbyCallbackInfo* data);
    static void Internal_OnLobbyDestroyLobbyCallback(const EOS_Lobby_DestroyLobbyCallbackInfo* data);
    static void Internal_OnLobbyJoinLobbyCallback(const EOS_Lobby_JoinLobbyCallbackInfo* data);
    static void Internal_OnLobbyLeaveLobbyCallback(const EOS_Lobby_LeaveLobbyCallbackInfo* data);
    static void Internal_OnLobbyUpdateLobbyCallback(const EOS_Lobby_UpdateLobbyCallbackInfo* data);
    static void Internal_OnLobbyPromoteMemberCallback(const EOS_Lobby_PromoteMemberCallbackInfo* data);
    static void Internal_OnLobbyKickMemberCallback(const EOS_Lobby_KickMemberCallbackInfo* data);
    static void Internal_OnLobbySendInviteCallback(const EOS_Lobby_SendInviteCallbackInfo* data);
    static void Internal_OnLobbyRejectInviteCallback(const EOS_Lobby_RejectInviteCallbackInfo* data);
    static void Internal_OnLobbyQueryInvitesCallback(const EOS_Lobby_QueryInvitesCallbackInfo* data);
    static void Internal_OnLobbyFindCallback(const EOS_LobbySearch_FindCallbackInfo* data);
private:
    static UCoreLobby* s_LobbyObject;
};