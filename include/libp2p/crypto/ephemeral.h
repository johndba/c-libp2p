#pragma once

#include <stdint.h>

/**
 * General helpers for ephemeral keys
 */

struct StretchedKey {
	char* iv;
	size_t iv_size;
	char* cipher_key;
	size_t cipher_size;
	unsigned char* mac_key;
	size_t mac_size;
};

struct EphemeralPublicKey {
	size_t num_bits;
	uint64_t x;
	uint64_t y;
	unsigned char* bytes; // a public key in bytes (the combination of X and Y)
	size_t bytes_size;
	unsigned char* shared_key;
	size_t shared_key_size;
};

struct EphemeralPrivateKey {
	size_t num_bits;
	uint64_t secret_key;
	struct EphemeralPublicKey* public_key;
};

/**
 * Generate a Ephemeral Public Key as well as a shared key
 * @param curve the curve to use (P-256, P-384, or P-521)
 * @param private_key where to store the private key
 * @reutrns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_ephemeral_keypair_generate(char* curve, struct EphemeralPrivateKey** private_key);

/***
 * Remove resources used by generation of ephemeral private key
 * @param in the key to destroy
 */
void libp2p_crypto_ephemeral_key_free( struct EphemeralPrivateKey* in);

/**
 * Routines to help with the StretchedKey struct
 */
struct StretchedKey* libp2p_crypto_ephemeral_stretched_key_new();
void libp2p_crypto_ephemeral_stretched_key_free(struct StretchedKey* in);
