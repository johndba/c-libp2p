#include <stdlib.h>
#include <string.h>

#include "libp2p/peer/peerstore.h"

struct PeerEntry* libp2p_peer_entry_new() {
	struct PeerEntry* out = (struct PeerEntry*)malloc(sizeof(struct PeerEntry));
	if (out != NULL) {
		out->peer = NULL;
	}
	return out;
}

void libp2p_peer_entry_free(struct PeerEntry* in) {
	if (in != NULL) {
		libp2p_peer_free(in->peer);
		free(in);
	}
}

struct PeerEntry* libp2p_peer_entry_copy(struct PeerEntry* in) {
	struct PeerEntry* out = libp2p_peer_entry_new();
	if (out != NULL) {
		out->peer = libp2p_peer_copy(in->peer);
		if (out->peer == NULL) {
			free(out);
			return NULL;
		}
	}
	return out;
}

/**
 * Creates a new empty peerstore
 * @returns an empty peerstore or NULL on error
 */
struct Peerstore* libp2p_peerstore_new() {
	struct Peerstore* out = (struct Peerstore*)malloc(sizeof(struct Peerstore));
	if (out != NULL) {
		out->head_entry = NULL;
	}
	return out;
}

/**
 * Deallocate resources used by the peerstore
 * @param in the struct to deallocate
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_peerstore_free(struct Peerstore* in) {
	if (in != NULL) {
		struct Libp2pLinkedList* current = in->head_entry;
		struct Libp2pLinkedList* next = NULL;
		while (current != NULL) {
			next = current->next;
			libp2p_peer_entry_free((struct PeerEntry*)current->item);
			current->item = NULL;
			libp2p_utils_linked_list_free(current);
			current = next;
		}
		free(in);
	}
	return 1;
}

/**
 * Add a Peer to the Peerstore
 * @param peerstore the peerstore to add the entry to
 * @param peer_entry the entry to add
 * @returns true(1) on success, otherwise false
 */
int libp2p_peerstore_add_peer_entry(struct Peerstore* peerstore, struct PeerEntry* peer_entry) {
	struct Libp2pLinkedList* new_item = libp2p_utils_linked_list_new();
	if (new_item == NULL)
		return 0;
	new_item->item = libp2p_peer_entry_copy(peer_entry);
	if (new_item->item == NULL) {
		libp2p_utils_linked_list_free(new_item);
		return 0;
	}
	if (peerstore->head_entry == NULL) {
		peerstore->head_entry = new_item;
		peerstore->last_entry = new_item;
	} else {
		peerstore->last_entry->next = new_item;
		peerstore->last_entry = new_item;
	}
	return 1;
}

/***
 * Add a peer to the peerstore
 * @param peerstore the peerstore to add the entry to
 * @param peer the peer to add (will be wrapped in PeerEntry struct)
 * @returns true(1) on success, otherwise false
 */
int libp2p_peerstore_add_peer(struct Peerstore* peerstore, struct Libp2pPeer* peer) {
	struct PeerEntry* peer_entry = libp2p_peer_entry_new();
	if (peer_entry == NULL) {
		return 0;
	}
	peer_entry->peer = libp2p_peer_copy(peer);
	if (peer_entry->peer == NULL)
		return 0;
	int retVal = libp2p_peerstore_add_peer_entry(peerstore, peer_entry);
	libp2p_peer_entry_free(peer_entry);
	return retVal;
}

/**
 * Retrieve a peer from the peerstore based on the peer id
 * @param peerstore the peerstore to search
 * @param peer_id the id to search for as a binary array
 * @param peer_id_size the size of the binary array
 * @returns the PeerEntry struct if found, otherwise NULL
 */
struct PeerEntry* libp2p_peerstore_get_peer_entry(struct Peerstore* peerstore, unsigned char* peer_id, size_t peer_id_size) {
	struct Libp2pLinkedList* current = peerstore->head_entry;
	while(current != NULL) {
		struct Libp2pPeer* peer = ((struct PeerEntry*)current->item)->peer;
		if (peer->id_size != peer_id_size)
			continue;
		if (memcmp(peer_id, peer->id, peer->id_size) == 0) {
			return (struct PeerEntry*)current->item;
		}
	}
	return NULL;
}

/**
 * Retrieve a peer from the peerstore based on the peer id
 * @param peerstore the peerstore to search
 * @param peer_id the id to search for as a binary array
 * @param peer_id_size the size of the binary array
 * @returns the Peer struct if found, otherwise NULL
 */
struct Libp2pPeer* libp2p_peerstore_get_peer(struct Peerstore* peerstore, unsigned char* peer_id, size_t peer_id_size) {
	struct PeerEntry* entry = libp2p_peerstore_get_peer_entry(peerstore, peer_id, peer_id_size);
	if (entry == NULL)
		return NULL;
	return entry->peer;
}
