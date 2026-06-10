// SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
// SPDX-License-Identifier: Apache-2.0

/* This file is a reference template for implementation of the
 * PSA Certified Crypto API v1.5
 */

#ifndef PSA_CRYPTO_H
#define PSA_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief The major version of this implementation of the Crypto API.
 */
#define PSA_CRYPTO_API_VERSION_MAJOR 1

/**
 * @brief The minor version of this implementation of the Crypto API.
 */
#define PSA_CRYPTO_API_VERSION_MINOR 5

/**
 * @brief Library initialization.
 */
psa_status_t psa_crypto_init(void);

/**
 * @brief A status code that indicates that there is not enough entropy to
 *        generate random data needed for the requested action.
 */
#define PSA_ERROR_INSUFFICIENT_ENTROPY ((psa_status_t)-148)

/**
 * @brief A status code that indicates that the decrypted padding is incorrect.
 */
#define PSA_ERROR_INVALID_PADDING ((psa_status_t)-150)

/**
 * @brief Key identifier.
 */
typedef uint32_t psa_key_id_t;

/**
 * @brief The type of an object containing key attributes.
 */
typedef /* implementation-defined type */ psa_key_attributes_t;

/**
 * @brief This macro returns a suitable initializer for a key attribute object
 *        of type psa_key_attributes_t.
 */
#define PSA_KEY_ATTRIBUTES_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a key attribute object.
 */
psa_key_attributes_t psa_key_attributes_init(void);

/**
 * @brief Retrieve the attributes of a key.
 *
 * @param key        Identifier of the key to query.
 * @param attributes On entry, *attributes must be in a valid state.
 */
psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t * attributes);

/**
 * @brief Reset a key attribute object to a freshly initialized state.
 *
 * @param attributes The attribute object to reset.
 */
void psa_reset_key_attributes(psa_key_attributes_t * attributes);

/**
 * @brief Encoding of a key type.
 */
typedef uint16_t psa_key_type_t;

/**
 * @brief An invalid key type value.
 */
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)

/**
 * @brief Whether a key type is an unstructured array of bytes.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) /* specification-defined value */

/**
 * @brief Whether a key type is asymmetric: either a key pair or a public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) /* specification-defined value */

/**
 * @brief Whether a key type is the public part of a key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) /* specification-defined value */

/**
 * @brief Whether a key type is a key pair containing a private part and a
 *        public part.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_KEY_PAIR(type) /* specification-defined value */

/**
 * @brief The type of identifiers of an elliptic curve family.
 */
typedef uint8_t psa_ecc_family_t;

/**
 * @brief SEC Koblitz curves over prime fields.
 */
#define PSA_ECC_FAMILY_SECP_K1 ((psa_ecc_family_t) 0x17)

/**
 * @brief SEC random curves over prime fields.
 */
#define PSA_ECC_FAMILY_SECP_R1 ((psa_ecc_family_t) 0x12)

/**
 * @brief This family of curves is weak and deprecated.
 */
#define PSA_ECC_FAMILY_SECP_R2 ((psa_ecc_family_t) 0x1b)

/**
 * @brief SEC Koblitz curves over binary fields.
 */
#define PSA_ECC_FAMILY_SECT_K1 ((psa_ecc_family_t) 0x27)

/**
 * @brief SEC random curves over binary fields.
 */
#define PSA_ECC_FAMILY_SECT_R1 ((psa_ecc_family_t) 0x22)

/**
 * @brief SEC additional random curves over binary fields.
 */
#define PSA_ECC_FAMILY_SECT_R2 ((psa_ecc_family_t) 0x2b)

/**
 * @brief Brainpool P random curves.
 */
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 ((psa_ecc_family_t) 0x30)

/**
 * @brief Curve used primarily in France and elsewhere in Europe.
 */
#define PSA_ECC_FAMILY_FRP ((psa_ecc_family_t) 0x33)

/**
 * @brief Montgomery curves.
 */
#define PSA_ECC_FAMILY_MONTGOMERY ((psa_ecc_family_t) 0x41)

/**
 * @brief Twisted Edwards curves.
 */
#define PSA_ECC_FAMILY_TWISTED_EDWARDS ((psa_ecc_family_t) 0x42)

/**
 * @brief The type of identifiers of a finite field Diffie-Hellman group family.
 */
typedef uint8_t psa_dh_family_t;

/**
 * @brief Finite field Diffie-Hellman groups defined for TLS in RFC 7919.
 */
#define PSA_DH_FAMILY_RFC7919 ((psa_dh_family_t) 0x03)

/**
 * @brief Finite field Diffie-Hellman groups defined for IKE2 in RFC 3526.
 */
#define PSA_DH_FAMILY_RFC3526 ((psa_dh_family_t) 0x05)

/**
 * @brief Declare the type of a key.
 *
 * @param attributes The attribute object to write to.
 * @param type       The key type to write.
 */
void psa_set_key_type(psa_key_attributes_t * attributes,
                      psa_key_type_t type);

/**
 * @brief Retrieve the key type from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The key type stored in the attribute object.
 */
psa_key_type_t psa_get_key_type(const psa_key_attributes_t * attributes);

/**
 * @brief Retrieve the key size from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The key size stored in the attribute object, in bits.
 */
size_t psa_get_key_bits(const psa_key_attributes_t * attributes);

/**
 * @brief Declare the size of a key.
 *
 * @param attributes The attribute object to write to.
 * @param bits       The key size in bits.
 */
void psa_set_key_bits(psa_key_attributes_t * attributes,
                      size_t bits);

/**
 * @brief Raw data.
 */
#define PSA_KEY_TYPE_RAW_DATA ((psa_key_type_t)0x1001)

/**
 * @brief A secret for key derivation.
 */
#define PSA_KEY_TYPE_DERIVE ((psa_key_type_t)0x1200)

/**
 * @brief A low-entropy secret for password hashing or key derivation.
 */
#define PSA_KEY_TYPE_PASSWORD ((psa_key_type_t)0x1203)

/**
 * @brief A secret value that can be used to verify a password hash.
 */
#define PSA_KEY_TYPE_PASSWORD_HASH ((psa_key_type_t)0x1205)

/**
 * @brief A secret value that can be used when computing a password hash.
 */
#define PSA_KEY_TYPE_PEPPER ((psa_key_type_t)0x1206)

/**
 * @brief HMAC key.
 */
#define PSA_KEY_TYPE_HMAC ((psa_key_type_t)0x1100)

/**
 * @brief Key for a cipher, AEAD or MAC algorithm based on the AES block cipher.
 */
#define PSA_KEY_TYPE_AES ((psa_key_type_t)0x2400)

/**
 * @brief Key for a cipher, AEAD or MAC algorithm based on the ARIA block
 *        cipher.
 */
#define PSA_KEY_TYPE_ARIA ((psa_key_type_t)0x2406)

/**
 * @brief Key for a cipher or MAC algorithm based on DES or 3DES (Triple-DES).
 */
#define PSA_KEY_TYPE_DES ((psa_key_type_t)0x2301)

/**
 * @brief Key for a cipher, AEAD or MAC algorithm based on the Camellia block
 *        cipher.
 */
#define PSA_KEY_TYPE_CAMELLIA ((psa_key_type_t)0x2403)

/**
 * @brief Key for a cipher, AEAD or MAC algorithm based on the SM4 block cipher.
 */
#define PSA_KEY_TYPE_SM4 ((psa_key_type_t)0x2405)

/**
 * @brief Key for the ARC4 stream cipher.
 */
#define PSA_KEY_TYPE_ARC4 ((psa_key_type_t)0x2002)

/**
 * @brief Key for the ChaCha20 stream cipher or the ChaCha20-Poly1305 AEAD
 *        algorithm.
 */
#define PSA_KEY_TYPE_CHACHA20 ((psa_key_type_t)0x2004)

/**
 * @brief Key for the XChaCha20 stream cipher or the XChaCha20-Poly1305 AEAD
 *        algorithm.
 */
#define PSA_KEY_TYPE_XCHACHA20 ((psa_key_type_t)0x2007)

/**
 * @brief Key for the Ascon-AEAD128 AEAD algorithm.
 */
#define PSA_KEY_TYPE_ASCON ((psa_key_type_t)0x2008)

/**
 * @brief Key for a BLAKE2 MAC algorithm.
 */
#define PSA_KEY_TYPE_BLAKE2 ((psa_key_type_t)0x1103)

/**
 * @brief WPA3-SAE password token using elliptic curves.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_WPA3_SAE_ECC(curve) /* specification-defined value */

/**
 * @brief WPA3-SAE password token using finite fields.
 *
 * @param group A value of type psa_dh_family_t that identifies the finite field
 *              Diffie-Hellman family to be used.
 */
#define PSA_KEY_TYPE_WPA3_SAE_DH(group) /* specification-defined value */

/**
 * @brief Whether a key type is a WPA3-SAE password token using elliptic curves.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_WPA3_SAE_ECC(type) /* specification-defined value */

/**
 * @brief Extract the curve family from a WPA3-SAE password token using elliptic
 *        curves.
 *
 * @param type A WPA3-SAE password token using elliptic curve key type: a value
 *             of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_WPA3_SAE_ECC(type) is true.
 *
 * @return The elliptic curve family id, if type is a supported WPA3-SAE
 *         password token using elliptic curve key.
 */
#define PSA_KEY_TYPE_WPA3_SAE_ECC_GET_FAMILY(type) \
    /* specification-defined value */

/**
 * @brief Whether a key type is a WPA3-SAE password token using elliptic curves.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_WPA3_SAE_DH(type) /* specification-defined value */

/**
 * @brief Extract the finite field group family from a WPA3-SAE password token
 *        using finite fields.
 *
 * @param type A WPA3-SAE password token using finite fields key type: a value
 *             of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_WPA3_SAE_DH(type) is true.
 *
 * @return The finite field group family id, if type is a supported WPA3-SAE
 *         password token using finite fields key.
 */
#define PSA_KEY_TYPE_WPA3_SAE_DH_GET_FAMILY(type) \
    /* specification-defined value */

/**
 * @brief RSA key pair: both the private and public key.
 */
#define PSA_KEY_TYPE_RSA_KEY_PAIR ((psa_key_type_t)0x7001)

/**
 * @brief RSA public key.
 */
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY ((psa_key_type_t)0x4001)

/**
 * @brief Whether a key type is an RSA key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_RSA(type) /* specification-defined value */

/**
 * @brief Elliptic curve key pair: both the private and public key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the ECC curve
 *              family to be used.
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve) /* specification-defined value */

/**
 * @brief Elliptic curve public key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the ECC curve
 *              family to be used.
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve) /* specification-defined value */

/**
 * @brief Whether a key type is an elliptic curve key, either a key pair or a
 *        public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC(type) /* specification-defined value */

/**
 * @brief Whether a key type is an elliptic curve key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) /* specification-defined value */

/**
 * @brief Whether a key type is an elliptic curve public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) /* specification-defined value */

/**
 * @brief Extract the curve family from an elliptic curve key type.
 *
 * @param type An elliptic curve key type: a value of type psa_key_type_t such
 *             that PSA_KEY_TYPE_IS_ECC(type) is true.
 *
 * @return The elliptic curve family id, if type is a supported elliptic curve
 *         key.
 */
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief Finite field Diffie-Hellman key pair: both the private key and public
 *        key.
 *
 * @param group A value of type psa_dh_family_t that identifies the finite field
 *              Diffie-Hellman group family to be used.
 */
#define PSA_KEY_TYPE_DH_KEY_PAIR(group) /* specification-defined value */

/**
 * @brief Finite field Diffie-Hellman public key.
 *
 * @param group A value of type psa_dh_family_t that identifies the finite field
 *              Diffie-Hellman group family to be used.
 */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group) /* specification-defined value */

/**
 * @brief Whether a key type is a finite field Diffie-Hellman key, either a key
 *        pair or a public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH(type) /* specification-defined value */

/**
 * @brief Whether a key type is a finite field Diffie-Hellman key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type) /* specification-defined value */

/**
 * @brief Whether a key type is a finite field Diffie-Hellman public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) /* specification-defined value */

/**
 * @brief Extract the group family from a finite field Diffie-Hellman key type.
 *
 * @param type A finite field Diffie-Hellman key type: a value of type
 *             psa_key_type_t such that PSA_KEY_TYPE_IS_DH(type) is true.
 *
 * @return The finite field Diffie-Hellman group family id, if type is a
 *         supported finite field Diffie-Hellman key.
 */
#define PSA_KEY_TYPE_DH_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief Leighton-Micali Signatures (LMS) public key.
 */
#define PSA_KEY_TYPE_LMS_PUBLIC_KEY ((psa_key_type_t)0x4007)

/**
 * @brief Hierarchical Signature Scheme (HSS) public key.
 */
#define PSA_KEY_TYPE_HSS_PUBLIC_KEY ((psa_key_type_t)0x4008)

/**
 * @brief eXtended Merkle Signature Scheme (XMSS) public key.
 */
#define PSA_KEY_TYPE_XMSS_PUBLIC_KEY ((psa_key_type_t)0x400B)

/**
 * @brief Multi-tree eXtended Merkle Signature Scheme (XMSS^MT) public key.
 */
#define PSA_KEY_TYPE_XMSS_MT_PUBLIC_KEY ((psa_key_type_t)0x400D)

/**
 * @brief ML-DSA key pair: both the private and public key.
 */
#define PSA_KEY_TYPE_ML_DSA_KEY_PAIR ((psa_key_type_t)0x7002)

/**
 * @brief ML-DSA public key.
 */
#define PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY ((psa_key_type_t)0x4002)

/**
 * @brief Whether a key type is an ML-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ML_DSA(type) /* specification-defined value */

/**
 * @brief The type of identifiers of a Stateless hash-based DSA parameter set.
 */
typedef uint8_t psa_slh_dsa_family_t;

/**
 * @brief SLH-DSA key pair: both the private key and public key.
 *
 * @param set A value of type psa_slh_dsa_family_t that identifies the SLH-DSA
 *            parameter-set family to be used.
 */
#define PSA_KEY_TYPE_SLH_DSA_KEY_PAIR(set) /* specification-defined value */

/**
 * @brief SLH-DSA public key.
 *
 * @param set A value of type psa_slh_dsa_family_t that identifies the SLH-DSA
 *            parameter-set family to be used.
 */
#define PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY(set) /* specification-defined value */

/**
 * @brief SLH-DSA family for the SLH-DSA-SHA2-NNNs parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHA2_S ((psa_slh_dsa_family_t) 0x02)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHA2-NNNf parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHA2_F ((psa_slh_dsa_family_t) 0x04)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHAKE-NNNs parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHAKE_S ((psa_slh_dsa_family_t) 0x0b)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHAKE-NNNf parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHAKE_F ((psa_slh_dsa_family_t) 0x0d)

/**
 * @brief Whether a key type is an SLH-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA(type) /* specification-defined value */

/**
 * @brief Whether a key type is an SLH-DSA key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA_KEY_PAIR(type) \
    /* specification-defined value */

/**
 * @brief Whether a key type is an SLH-DSA public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA_PUBLIC_KEY(type) \
    /* specification-defined value */

/**
 * @brief Extract the parameter-set family from an SLH-DSA key type.
 *
 * @param type An SLH-DSA key type: a value of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_SLH_DSA(type) is true.
 *
 * @return The SLH-DSA parameter-set family id, if type is a supported SLH-DSA
 *         key.
 */
#define PSA_KEY_TYPE_SLH_DSA_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief ML-KEM key pair: both the decapsulation and encapsulation key.
 */
#define PSA_KEY_TYPE_ML_KEM_KEY_PAIR ((psa_key_type_t)0x7004)

/**
 * @brief ML-KEM public (encapsulation) key.
 */
#define PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY ((psa_key_type_t)0x4004)

/**
 * @brief Whether a key type is an ML-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ML_KEM(type) /* specification-defined value */

/**
 * @brief SPAKE2+ key pair: both the prover and verifier key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(curve) /* specification-defined value */

/**
 * @brief SPAKE2+ public key: the verifier key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(curve) \
    /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P(type) /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P_KEY_PAIR(type) \
    /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P_PUBLIC_KEY(type) \
    /* specification-defined value */

/**
 * @brief Extract the curve family from a SPAKE2+ key type.
 *
 * @param type A SPAKE2+ key type: a value of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_SPAKE2P(type) is true.
 *
 * @return The elliptic curve family id, if type is a supported SPAKE2+ key.
 */
#define PSA_KEY_TYPE_SPAKE2P_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief The key-pair type corresponding to a public-key type.
 *
 * @param type A public-key type or key-pair type.
 *
 * @return The corresponding key-pair type.
 */
#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
    /* specification-defined value */

/**
 * @brief The public-key type corresponding to a key-pair type.
 *
 * @param type A public-key type or key-pair type.
 *
 * @return The corresponding public-key type.
 */
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
    /* specification-defined value */

/**
 * @brief Encoding of key lifetimes.
 */
typedef uint32_t psa_key_lifetime_t;

/**
 * @brief Encoding of key persistence levels.
 */
typedef uint8_t psa_key_persistence_t;

/**
 * @brief Encoding of key location indicators.
 */
typedef uint32_t psa_key_location_t;

/**
 * @brief The default lifetime for volatile keys.
 */
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t) 0x00000000)

/**
 * @brief The default lifetime for persistent keys.
 */
#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t) 0x00000001)

/**
 * @brief The persistence level of volatile keys.
 */
#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t) 0x00)

/**
 * @brief The default persistence level for persistent keys.
 */
#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t) 0x01)

/**
 * @brief A persistence level indicating that a key is never destroyed.
 */
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t) 0xff)

/**
 * @brief The local storage area for persistent keys.
 */
#define PSA_KEY_LOCATION_LOCAL_STORAGE ((psa_key_location_t) 0x000000)

/**
 * @brief The default secure element storage area for persistent keys.
 */
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t) 0x000001)

/**
 * @brief Set the lifetime of a key, for a persistent key or a non-default
 *        location.
 *
 * @param attributes The attribute object to write to.
 * @param lifetime   The lifetime for the key.
 */
void psa_set_key_lifetime(psa_key_attributes_t * attributes,
                          psa_key_lifetime_t lifetime);

/**
 * @brief Retrieve the lifetime from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The lifetime value stored in the attribute object.
 */
psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t * attributes);

/**
 * @brief Extract the persistence level from a key lifetime.
 *
 * @param lifetime The lifetime value to query: a value of type
 *                 psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t) ((lifetime) & 0x000000ff))

/**
 * @brief Extract the location indicator from a key lifetime.
 *
 * @param lifetime The lifetime value to query: a value of type
 *                 psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t) ((lifetime) >> 8))

/**
 * @brief Whether a key lifetime indicates that the key is volatile.
 *
 * @param lifetime The lifetime value to query: a value of type
 *                 psa_key_lifetime_t.
 *
 * @return 1 if the key is volatile, otherwise 0.
 */
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)

/**
 * @brief Construct a lifetime from a persistence level and a location.
 *
 * @param persistence The persistence level: a value of type
 *                    psa_key_persistence_t.
 * @param location    The location indicator: a value of type
 *                    psa_key_location_t.
 *
 * @return The constructed lifetime value.
 */
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))

/**
 * @brief The null key identifier.
 */
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)

/**
 * @brief The minimum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)

/**
 * @brief The maximum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)

/**
 * @brief The maximum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)

/**
 * @brief Declare a key as persistent and set its key identifier.
 *
 * @param attributes The attribute object to write to.
 * @param id         The persistent identifier for the key.
 */
void psa_set_key_id(psa_key_attributes_t * attributes,
                    psa_key_id_t id);

/**
 * @brief Retrieve the key identifier from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The persistent identifier stored in the attribute object.
 */
psa_key_id_t psa_get_key_id(const psa_key_attributes_t * attributes);

/**
 * @brief Encoding of a cryptographic algorithm.
 */
typedef uint32_t psa_algorithm_t;

/**
 * @brief Declare the permitted-algorithm policy for a key.
 *
 * @param attributes The attribute object to write to.
 * @param alg        The permitted algorithm to write.
 */
void psa_set_key_algorithm(psa_key_attributes_t * attributes,
                           psa_algorithm_t alg);

/**
 * @brief Retrieve the permitted-algorithm policy from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The algorithm stored in the attribute object.
 */
psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t * attributes);

/**
 * @brief Encoding of permitted usage on a key.
 */
typedef uint32_t psa_key_usage_t;

/**
 * @brief Permission to export the key.
 */
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)

/**
 * @brief Permission to copy the key.
 */
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)

/**
 * @brief Permission for the implementation to cache the key.
 */
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)

/**
 * @brief Permission to encrypt a message, or perform key encapsulation, with
 *        the key.
 */
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)

/**
 * @brief Permission to decrypt a message, or perform key decapsulation, with
 *        the key.
 */
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)

/**
 * @brief Permission to sign a message with the key.
 */
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)

/**
 * @brief Permission to verify a message signature with the key.
 */
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)

/**
 * @brief Permission to sign a message hash with the key.
 */
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)

/**
 * @brief Permission to verify a message hash with the key.
 */
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)

/**
 * @brief Permission to derive other keys or produce a password hash from this
 *        key.
 */
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)

/**
 * @brief Permission to verify the result of a key derivation, including
 *        password hashing.
 */
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)

/**
 * @brief Used in the psa_check_key_usage() function to determine if the key can
 *        be used in the public key role in a key-agreement or a PAKE operation.
 */
#define PSA_KEY_USAGE_DERIVE_PUBLIC ((psa_key_usage_t)0x00000080)

/**
 * @brief Permission to wrap another key with the key.
 */
#define PSA_KEY_USAGE_WRAP ((psa_key_usage_t)0x00010000)

/**
 * @brief Permission to unwrap another key with the key.
 */
#define PSA_KEY_USAGE_UNWRAP ((psa_key_usage_t)0x00020000)

/**
 * @brief Declare usage flags for a key.
 *
 * @param attributes  The attribute object to write to.
 * @param usage_flags The usage flags to write.
 */
void psa_set_key_usage_flags(psa_key_attributes_t * attributes,
                             psa_key_usage_t usage_flags);

/**
 * @brief Retrieve the usage flags from key attributes.
 *
 * @param attributes The key attribute object to query.
 *
 * @return The usage flags stored in the attribute object.
 */
psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t * attributes);

/**
 * @brief Query the capability of a key.
 *
 * @param key   Identifier of the key to check.
 * @param alg   An algorithm identifier: a value of type psa_algorithm_t.
 * @param usage A single PSA_KEY_USAGE_xxx flag.
 */
psa_status_t psa_check_key_usage(psa_key_id_t key,
                                 psa_algorithm_t alg,
                                 psa_key_usage_t usage);

/**
 * @brief Import a key in binary format.
 *
 * @param attributes  The attributes for the new key.
 * @param data        Buffer containing the key data.
 * @param data_length Size of the data buffer in bytes.
 * @param key         On success, an identifier for the newly created key.
 */
psa_status_t psa_import_key(const psa_key_attributes_t * attributes,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key);

/**
 * @brief Custom production parameters for key generation or key derivation.
 */
typedef struct psa_custom_key_parameters_t {
    /// @brief Flags to control the key production process.
    uint32_t flags;
} psa_custom_key_parameters_t;

/**
 * @brief The default production parameters for key generation or key
 *        derivation.
 */
#define PSA_CUSTOM_KEY_PARAMETERS_INIT { 0 }

/**
 * @brief Generate a key or key pair.
 *
 * @param attributes The attributes for the new key.
 * @param key        On success, an identifier for the newly created key.
 */
psa_status_t psa_generate_key(const psa_key_attributes_t * attributes,
                              psa_key_id_t * key);

/**
 * @brief Generate a key or key pair using custom production parameters.
 *
 * @param attributes         The attributes for the new key.
 * @param custom             Customized production parameters for the key
 *                           generation.
 * @param custom_data        A buffer containing additional variable-sized
 *                           production parameters.
 * @param custom_data_length Length of custom_data in bytes.
 * @param key                On success, an identifier for the newly created
 *                           key.
 */
psa_status_t psa_generate_key_custom(const psa_key_attributes_t * attributes,
                                     const psa_custom_key_parameters_t * custom,
                                     const uint8_t * custom_data,
                                     size_t custom_data_length,
                                     psa_key_id_t * key);

/**
 * @brief Make a copy of a key.
 *
 * @param source_key The key to copy.
 * @param attributes The attributes for the new key.
 * @param target_key On success, an identifier for the newly created key.
 */
psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t * attributes,
                          psa_key_id_t * target_key);

/**
 * @brief Register implementation-provided key material with a volatile key
 *        identifier and key policy.
 *
 * @param attributes   The attributes for the key to be registered.
 * @param label        Buffer containing a label that identifies the
 *                     implementation-provided key to be registered.
 * @param label_length Size of the label buffer in bytes.
 * @param key          On success, an identifier for the newly created key.
 */
psa_status_t psa_attach_key(const psa_key_attributes_t * attributes,
                            const uint8_t * label,
                            size_t label_length,
                            psa_key_id_t * key);

/**
 * @brief Destroy or unregister a key.
 *
 * @param key Identifier of the key to erase.
 */
psa_status_t psa_destroy_key(psa_key_id_t key);

/**
 * @brief Remove non-essential copies of key material from memory.
 *
 * @param key Identifier of the key to purge.
 */
psa_status_t psa_purge_key(psa_key_id_t key);

/**
 * @brief Export a key in binary format.
 *
 * @param key         Identifier of the key to export.
 * @param data        Buffer where the key data is to be written.
 * @param data_size   Size of the data buffer in bytes.
 * @param data_length On success, the number of bytes that make up the key data.
 */
psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t * data,
                            size_t data_size,
                            size_t * data_length);

/**
 * @brief Export a public key or the public part of a key pair in binary format.
 *
 * @param key         Identifier of the key to export.
 * @param data        Buffer where the key data is to be written.
 * @param data_size   Size of the data buffer in bytes.
 * @param data_length On success, the number of bytes that make up the key data.
 */
psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t * data,
                                   size_t data_size,
                                   size_t * data_length);

/**
 * @brief Sufficient output buffer size for psa_export_key().
 *
 * @param key_type A supported key type.
 * @param key_bits The size of the key in bits.
 *
 * @return If the parameters are valid and supported, return a buffer size in
 *         bytes that guarantees that psa_export_key() or
 *         psa_export_public_key() will not fail with
 *         PSA_ERROR_BUFFER_TOO_SMALL.
 */
#define PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */

/**
 * @brief Sufficient output buffer size for psa_export_public_key().
 *
 * @param key_type A public-key or key-pair key type.
 * @param key_bits The size of the key in bits.
 *
 * @return If the parameters are valid and supported, return a buffer size in
 *         bytes that guarantees that psa_export_public_key() will not fail with
 *         PSA_ERROR_BUFFER_TOO_SMALL.
 */
#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */

/**
 * @brief Sufficient buffer size for exporting any asymmetric key pair.
 */
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE /* implementation-defined value */

/**
 * @brief Sufficient buffer size for exporting any asymmetric public key.
 */
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE /* implementation-defined value */

/**
 * @brief Sufficient buffer size for exporting any asymmetric key pair or public
 *        key.
 */
#define PSA_EXPORT_ASYMMETRIC_KEY_MAX_SIZE /* implementation-defined value */

/**
 * @brief An invalid algorithm identifier value.
 */
#define PSA_ALG_NONE ((psa_algorithm_t)0)

/**
 * @brief Whether the specified algorithm is a hash algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hash algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an XOF algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an XOF algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_XOF(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a MAC algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a MAC algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_MAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a symmetric cipher algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a symmetric cipher algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_CIPHER(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an authenticated encryption with
 *        associated data (AEAD) algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an AEAD algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_AEAD(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a key wrapping algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-wrapping algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_KEY_WRAP(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a key-derivation algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-derivation algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_KEY_DERIVATION(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an asymmetric signature algorithm,
 *        also known as public-key signature algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an asymmetric signature algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_SIGN(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an asymmetric encryption algorithm,
 *        also known as public-key encryption algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an asymmetric encryption algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a key-agreement algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-agreement algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_KEY_AGREEMENT(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a password-authenticated key
 *        exchange.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a password-authenticated key exchange (PAKE) algorithm, 0
 *         otherwise.
 */
#define PSA_ALG_IS_PAKE(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a key-encapsulation algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-encapsulation algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_KEY_ENCAPSULATION(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm encoding is a wildcard.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a wildcard algorithm encoding.
 */
#define PSA_ALG_IS_WILDCARD(alg) /* specification-defined value */

/**
 * @brief Get the hash used by a composite algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return The underlying hash algorithm if alg is a composite algorithm that
 *         uses a hash algorithm.
 */
#define PSA_ALG_GET_HASH(alg) /* specification-defined value */

/**
 * @brief The MD2 message-digest algorithm.
 */
#define PSA_ALG_MD2 ((psa_algorithm_t)0x02000001)

/**
 * @brief The MD4 message-digest algorithm.
 */
#define PSA_ALG_MD4 ((psa_algorithm_t)0x02000002)

/**
 * @brief The MD5 message-digest algorithm.
 */
#define PSA_ALG_MD5 ((psa_algorithm_t)0x02000003)

/**
 * @brief The RIPEMD-160 message-digest algorithm.
 */
#define PSA_ALG_RIPEMD160 ((psa_algorithm_t)0x02000004)

/**
 * @brief The Zigbee 1.0 hash function based on a Matyas-Meyer-Oseas (MMO)
 *        construction using AES-128.
 */
#define PSA_ALG_AES_MMO_ZIGBEE ((psa_algorithm_t)0x02000007)

/**
 * @brief The SHA-1 message-digest algorithm.
 */
#define PSA_ALG_SHA_1 ((psa_algorithm_t)0x02000005)

/**
 * @brief The SHA-224 message-digest algorithm.
 */
#define PSA_ALG_SHA_224 ((psa_algorithm_t)0x02000008)

/**
 * @brief The SHA-256 message-digest algorithm.
 */
#define PSA_ALG_SHA_256 ((psa_algorithm_t)0x02000009)

/**
 * @brief The SHA-384 message-digest algorithm.
 */
#define PSA_ALG_SHA_384 ((psa_algorithm_t)0x0200000a)

/**
 * @brief The SHA-512 message-digest algorithm.
 */
#define PSA_ALG_SHA_512 ((psa_algorithm_t)0x0200000b)

/**
 * @brief The SHA-512/224 message-digest algorithm.
 */
#define PSA_ALG_SHA_512_224 ((psa_algorithm_t)0x0200000c)

/**
 * @brief The SHA-512/256 message-digest algorithm.
 */
#define PSA_ALG_SHA_512_256 ((psa_algorithm_t)0x0200000d)

/**
 * @brief The SHA-256/192 message digest algorithm.
 */
#define PSA_ALG_SHA_256_192 ((psa_algorithm_t)0x0200000E)

/**
 * @brief The SHA3-224 message-digest algorithm.
 */
#define PSA_ALG_SHA3_224 ((psa_algorithm_t)0x02000010)

/**
 * @brief The SHA3-256 message-digest algorithm.
 */
#define PSA_ALG_SHA3_256 ((psa_algorithm_t)0x02000011)

/**
 * @brief The SHA3-384 message-digest algorithm.
 */
#define PSA_ALG_SHA3_384 ((psa_algorithm_t)0x02000012)

/**
 * @brief The SHA3-512 message-digest algorithm.
 */
#define PSA_ALG_SHA3_512 ((psa_algorithm_t)0x02000013)

/**
 * @brief The SHAKE256/512 message digest algorithm.
 */
#define PSA_ALG_SHAKE256_512 ((psa_algorithm_t)0x02000015)

/**
 * @brief The SHAKE128/256 message digest algorithm.
 */
#define PSA_ALG_SHAKE128_256 ((psa_algorithm_t)0x02000016)

/**
 * @brief The SHAKE256/192 message digest algorithm.
 */
#define PSA_ALG_SHAKE256_192 ((psa_algorithm_t)0x02000017)

/**
 * @brief The SHAKE256/256 message digest algorithm.
 */
#define PSA_ALG_SHAKE256_256 ((psa_algorithm_t)0x02000018)

/**
 * @brief The SM3 message-digest algorithm.
 */
#define PSA_ALG_SM3 ((psa_algorithm_t)0x02000014)

/**
 * @brief The Ascon-Hash256 message-digest algorithm.
 */
#define PSA_ALG_ASCON_HASH256 ((psa_algorithm_t)0x02000019)

/**
 * @brief The BLAKE2s cryptographic hash with 256 bits of output (BLAKE2s-256).
 */
#define PSA_ALG_BLAKE2S_HASH256 ((psa_algorithm_t)0x0200001C)

/**
 * @brief The BLAKE2sp cryptographic hash with 256 bits of output
 *        (BLAKE2sp-256).
 */
#define PSA_ALG_BLAKE2SP_HASH256 ((psa_algorithm_t)0x0200001D)

/**
 * @brief The BLAKE2b cryptographic hash with 512 bits of output (BLAKE2b-512).
 */
#define PSA_ALG_BLAKE2B_HASH512 ((psa_algorithm_t)0x0200001E)

/**
 * @brief The BLAKE2bp cryptographic hash with 512 bits of output
 *        (BLAKE2bp-512).
 */
#define PSA_ALG_BLAKE2BP_HASH512 ((psa_algorithm_t)0x0200001F)

/**
 * @brief Whether the specified algorithm is a BLAKE2 hash algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a BLAKE2 hash algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_BLAKE2_HASH(alg) /* specification-defined value */

/**
 * @brief Calculate the hash (digest) of a message.
 *
 * @param alg          The hash algorithm to compute: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_HASH(alg) is true.
 * @param input        Buffer containing the message to hash.
 * @param input_length Size of the input buffer in bytes.
 * @param hash         Buffer where the hash is to be written.
 * @param hash_size    Size of the hash buffer in bytes.
 * @param hash_length  On success, the number of bytes that make up the hash
 *                     value.
 */
psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * hash,
                              size_t hash_size,
                              size_t * hash_length);

/**
 * @brief Calculate the hash (digest) of a message and compare it with a
 *        reference value.
 *
 * @param alg          The hash algorithm to compute: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_HASH(alg) is true.
 * @param input        Buffer containing the message to hash.
 * @param input_length Size of the input buffer in bytes.
 * @param hash         Buffer containing the expected hash value.
 * @param hash_length  Size of the hash buffer in bytes.
 */
psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              const uint8_t * hash,
                              size_t hash_length);

/**
 * @brief The type of the state object for multi-part hash operations.
 */
typedef /* implementation-defined type */ psa_hash_operation_t;

/**
 * @brief This macro returns a suitable initializer for a hash operation object
 *        of type psa_hash_operation_t.
 */
#define PSA_HASH_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a hash operation object.
 */
psa_hash_operation_t psa_hash_operation_init(void);

/**
 * @brief Set up a multi-part hash operation.
 *
 * @param operation The operation object to set up.
 * @param alg       The hash algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_HASH(alg) is true.
 */
psa_status_t psa_hash_setup(psa_hash_operation_t * operation,
                            psa_algorithm_t alg);

/**
 * @brief Add a message fragment to a multi-part hash operation.
 *
 * @param operation    Active hash operation.
 * @param input        Buffer containing the message fragment to hash.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_hash_update(psa_hash_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length);

/**
 * @brief Finish the calculation of the hash of a message.
 *
 * @param operation   Active hash operation.
 * @param hash        Buffer where the hash is to be written.
 * @param hash_size   Size of the hash buffer in bytes.
 * @param hash_length On success, the number of bytes that make up the hash
 *                    value.
 */
psa_status_t psa_hash_finish(psa_hash_operation_t * operation,
                             uint8_t * hash,
                             size_t hash_size,
                             size_t * hash_length);

/**
 * @brief Finish the calculation of the hash of a message and compare it with an
 *        expected value.
 *
 * @param operation   Active hash operation.
 * @param hash        Buffer containing the expected hash value.
 * @param hash_length Size of the hash buffer in bytes.
 */
psa_status_t psa_hash_verify(psa_hash_operation_t * operation,
                             const uint8_t * hash,
                             size_t hash_length);

/**
 * @brief Abort a hash operation.
 *
 * @param operation Initialized hash operation.
 */
psa_status_t psa_hash_abort(psa_hash_operation_t * operation);

/**
 * @brief Halt the hash operation and extract the intermediate state of the hash
 *        computation.
 *
 * @param operation         Active hash operation.
 * @param hash_state        Buffer where the hash suspend state is to be
 *                          written.
 * @param hash_state_size   Size of the hash_state buffer in bytes.
 * @param hash_state_length On success, the number of bytes that make up the
 *                          hash suspend state.
 */
psa_status_t psa_hash_suspend(psa_hash_operation_t * operation,
                              uint8_t * hash_state,
                              size_t hash_state_size,
                              size_t * hash_state_length);

/**
 * @brief Set up a multi-part hash operation using the hash suspend state from a
 *        previously suspended hash operation.
 *
 * @param operation         The operation object to set up.
 * @param hash_state        A buffer containing the suspended hash state which
 *                          is to be resumed.
 * @param hash_state_length Length of hash_state in bytes.
 */
psa_status_t psa_hash_resume(psa_hash_operation_t * operation,
                             const uint8_t * hash_state,
                             size_t hash_state_length);

/**
 * @brief Clone a hash operation.
 *
 * @param source_operation The active hash operation to clone.
 * @param target_operation The operation object to set up.
 */
psa_status_t psa_hash_clone(const psa_hash_operation_t * source_operation,
                            psa_hash_operation_t * target_operation);

/**
 * @brief The size of the output of psa_hash_compute() and psa_hash_finish(), in
 *        bytes.
 *
 * @param alg A hash algorithm or an HMAC algorithm: a value of type
 *            psa_algorithm_t such that (PSA_ALG_IS_HASH(alg) ||
 *            PSA_ALG_IS_HMAC(alg)) is true.
 *
 * @return The hash length for the specified hash algorithm.
 */
#define PSA_HASH_LENGTH(alg) /* implementation-defined value */

/**
 * @brief Maximum size of a hash.
 */
#define PSA_HASH_MAX_SIZE /* implementation-defined value */

/**
 * @brief A sufficient hash suspend state buffer size for psa_hash_suspend(), in
 *        bytes.
 *
 * @param alg A hash algorithm: a value of type psa_algorithm_t such that
 *            PSA_ALG_IS_HASH(alg) is true.
 *
 * @return A sufficient output size for the algorithm.
 */
#define PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) /* specification-defined value */

/**
 * @brief A sufficient hash suspend state buffer size for psa_hash_suspend(),
 *        for any supported hash algorithms.
 */
#define PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief The size of the algorithm field that is part of the output of
 *        psa_hash_suspend(), in bytes.
 */
#define PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH ((size_t)4)

/**
 * @brief The size of the input-length field that is part of the output of
 *        psa_hash_suspend(), in bytes.
 *
 * @param alg A hash algorithm: a value of type psa_algorithm_t such that
 *            PSA_ALG_IS_HASH(alg) is true.
 *
 * @return The size, in bytes, of the input-length field of the hash suspend
 *         state for the specified hash algorithm.
 */
#define PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) \
    /* specification-defined value */

/**
 * @brief The size of the hash-state field that is part of the output of
 *        psa_hash_suspend(), in bytes.
 *
 * @param alg A hash algorithm: a value of type psa_algorithm_t such that
 *            PSA_ALG_IS_HASH(alg) is true.
 *
 * @return The size, in bytes, of the hash-state field of the hash suspend state
 *         for the specified hash algorithm.
 */
#define PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) \
    /* specification-defined value */

/**
 * @brief The input block size of a hash algorithm, in bytes.
 *
 * @param alg A hash algorithm: a value of type psa_algorithm_t such that
 *            PSA_ALG_IS_HASH(alg) is true.
 *
 * @return The block size in bytes for the specified hash algorithm.
 */
#define PSA_HASH_BLOCK_LENGTH(alg) /* implementation-defined value */

/**
 * @brief The SHAKE128 XOF algorithm.
 */
#define PSA_ALG_SHAKE128 ((psa_algorithm_t)0x0D000100)

/**
 * @brief The SHAKE256 XOF algorithm.
 */
#define PSA_ALG_SHAKE256 ((psa_algorithm_t)0x0D000200)

/**
 * @brief The Ascon-XOF128 XOF algorithm.
 */
#define PSA_ALG_ASCON_XOF128 ((psa_algorithm_t)0x0D000300)

/**
 * @brief The Ascon-CXOF128 XOF algorithm, with context.
 */
#define PSA_ALG_ASCON_CXOF128 ((psa_algorithm_t)0x0D008300)

/**
 * @brief The type of the state object for multi-part XOF operations.
 */
typedef /* implementation-defined type */ psa_xof_operation_t;

/**
 * @brief This macro returns a suitable initializer for an XOF operation object
 *        of type psa_xof_operation_t.
 */
#define PSA_XOF_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for an XOF operation object.
 */
psa_xof_operation_t psa_xof_operation_init(void);

/**
 * @brief Set up an XOF operation.
 *
 * @param operation The operation object to set up.
 * @param alg       The XOF algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_XOF(alg) is true.
 */
psa_status_t psa_xof_setup(psa_xof_operation_t * operation,
                           psa_algorithm_t alg);

/**
 * @brief Provide a context for a multi-part XOF operation.
 *
 * @param operation      Active XOF operation.
 * @param context        Buffer containing the context value.
 * @param context_length Size of the context buffer in bytes.
 */
psa_status_t psa_xof_set_context(psa_xof_operation_t * operation,
                                 const uint8_t * context,
                                 size_t context_length);

/**
 * @brief Add input to a multi-part XOF operation.
 *
 * @param operation    Active XOF operation.
 * @param input        Buffer containing the input fragment.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_xof_update(psa_xof_operation_t * operation,
                            const uint8_t * input,
                            size_t input_length);

/**
 * @brief Extract data from an XOF operation.
 *
 * @param operation     Active XOF operation.
 * @param output        Buffer where the output will be written.
 * @param output_length Number of bytes to output.
 */
psa_status_t psa_xof_output(psa_xof_operation_t * operation,
                            uint8_t * output,
                            size_t output_length);

/**
 * @brief Abort an XOF operation.
 *
 * @param operation Initialized XOF operation.
 */
psa_status_t psa_xof_abort(psa_xof_operation_t * operation);

/**
 * @brief Whether the specified XOF algorithm has a context parameter.
 *
 * @param alg An XOF algorithm identifier: a value of type psa_algorithm_t such
 *            that PSA_ALG_IS_XOF(alg) is true.
 *
 * @return 1 if alg is an XOF algorithm that has a context parameter.
 */
#define PSA_ALG_XOF_HAS_CONTEXT(alg) /* specification-defined value */

/**
 * @brief Macro to build an HMAC message-authentication-code algorithm from an
 *        underlying hash algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HMAC algorithm.
 */
#define PSA_ALG_HMAC(hash_alg) /* specification-defined value */

/**
 * @brief The CBC-MAC message-authentication-code algorithm, constructed over a
 *        block cipher.
 */
#define PSA_ALG_CBC_MAC ((psa_algorithm_t)0x03c00100)

/**
 * @brief The CMAC message-authentication-code algorithm, constructed over a
 *        block cipher.
 */
#define PSA_ALG_CMAC ((psa_algorithm_t)0x03c00200)

/**
 * @brief Macro to build an BLAKE2 message-authentication-code algorithm from a
 *        BLAKE2 hash algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_BLAKE2_HASH(hash_alg) is true.
 *
 * @return The corresponding BLAKE2 MAC algorithm.
 */
#define PSA_ALG_BLAKE2_MAC(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build a truncated MAC algorithm.
 *
 * @param mac_alg    A MAC algorithm: a value of type psa_algorithm_t such that
 *                   PSA_ALG_IS_MAC(mac_alg) is true.
 * @param mac_length Desired length of the truncated MAC in bytes.
 *
 * @return The corresponding MAC algorithm with the specified length.
 */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length) \
    /* specification-defined value */

/**
 * @brief Macro to construct the MAC algorithm with an untruncated MAC, from a
 *        truncated MAC algorithm.
 *
 * @param mac_alg A MAC algorithm: a value of type psa_algorithm_t such that
 *                PSA_ALG_IS_MAC(mac_alg) is true.
 *
 * @return The corresponding MAC algorithm with an untruncated MAC.
 */
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg) /* specification-defined value */

/**
 * @brief Macro to build a MAC minimum-MAC-length wildcard algorithm.
 *
 * @param mac_alg        A MAC algorithm: a value of type psa_algorithm_t such
 *                       that PSA_ALG_IS_MAC(alg) is true.
 * @param min_mac_length Desired minimum length of the message authentication
 *                       code in bytes.
 *
 * @return The corresponding MAC wildcard algorithm with the specified minimum
 *         MAC length.
 */
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length) \
    /* specification-defined value */

/**
 * @brief Calculate the message authentication code (MAC) of a message.
 *
 * @param key          Identifier of the key to use for the operation.
 * @param alg          The MAC algorithm to compute: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_MAC(alg) is true.
 * @param input        Buffer containing the input message.
 * @param input_length Size of the input buffer in bytes.
 * @param mac          Buffer where the MAC value is to be written.
 * @param mac_size     Size of the mac buffer in bytes.
 * @param mac_length   On success, the number of bytes that make up the MAC
 *                     value.
 */
psa_status_t psa_mac_compute(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * mac,
                             size_t mac_size,
                             size_t * mac_length);

/**
 * @brief Calculate the MAC of a message and compare it with a reference value.
 *
 * @param key          Identifier of the key to use for the operation.
 * @param alg          The MAC algorithm to compute: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_MAC(alg) is true.
 * @param input        Buffer containing the input message.
 * @param input_length Size of the input buffer in bytes.
 * @param mac          Buffer containing the expected MAC value.
 * @param mac_length   Size of the mac buffer in bytes.
 */
psa_status_t psa_mac_verify(psa_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t * input,
                            size_t input_length,
                            const uint8_t * mac,
                            size_t mac_length);

/**
 * @brief The type of the state object for multi-part MAC operations.
 */
typedef /* implementation-defined type */ psa_mac_operation_t;

/**
 * @brief This macro returns a suitable initializer for a MAC operation object
 *        of type psa_mac_operation_t.
 */
#define PSA_MAC_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a MAC operation object.
 */
psa_mac_operation_t psa_mac_operation_init(void);

/**
 * @brief Set up a multi-part MAC calculation operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The MAC algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_MAC(alg) is true.
 */
psa_status_t psa_mac_sign_setup(psa_mac_operation_t * operation,
                                psa_key_id_t key,
                                psa_algorithm_t alg);

/**
 * @brief Set up a multi-part MAC verification operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The MAC algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_MAC(alg) is true.
 */
psa_status_t psa_mac_verify_setup(psa_mac_operation_t * operation,
                                  psa_key_id_t key,
                                  psa_algorithm_t alg);

/**
 * @brief Add a message fragment to a multi-part MAC operation.
 *
 * @param operation    Active MAC operation.
 * @param input        Buffer containing the message fragment to add to the MAC
 *                     calculation.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_mac_update(psa_mac_operation_t * operation,
                            const uint8_t * input,
                            size_t input_length);

/**
 * @brief Finish the calculation of the MAC of a message.
 *
 * @param operation  Active MAC operation.
 * @param mac        Buffer where the MAC value is to be written.
 * @param mac_size   Size of the mac buffer in bytes.
 * @param mac_length On success, the number of bytes that make up the MAC value.
 */
psa_status_t psa_mac_sign_finish(psa_mac_operation_t * operation,
                                 uint8_t * mac,
                                 size_t mac_size,
                                 size_t * mac_length);

/**
 * @brief Finish the calculation of the MAC of a message and compare it with an
 *        expected value.
 *
 * @param operation  Active MAC operation.
 * @param mac        Buffer containing the expected MAC value.
 * @param mac_length Size of the mac buffer in bytes.
 */
psa_status_t psa_mac_verify_finish(psa_mac_operation_t * operation,
                                   const uint8_t * mac,
                                   size_t mac_length);

/**
 * @brief Abort a MAC operation.
 *
 * @param operation Initialized MAC operation.
 */
psa_status_t psa_mac_abort(psa_mac_operation_t * operation);

/**
 * @brief Whether the specified algorithm is an HMAC algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an HMAC algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HMAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a BLAKE2 MAC algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a BLAKE2 MAC algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_BLAKE2_MAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a MAC algorithm based on a block
 *        cipher.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a MAC algorithm based on a block cipher, 0 otherwise.
 */
#define PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) /* specification-defined value */

/**
 * @brief The size of the output of psa_mac_compute() and psa_mac_sign_finish(),
 *        in bytes.
 *
 * @param key_type The type of the MAC key.
 * @param key_bits The size of the MAC key in bits.
 * @param alg      A MAC algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_MAC(alg) is true.
 *
 * @return The MAC length for the specified algorithm with the specified key
 *         parameters.
 */
#define PSA_MAC_LENGTH(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient buffer size for storing the MAC output by
 *        psa_mac_verify() and psa_mac_verify_finish(), for any of the supported
 *        key types and MAC algorithms.
 */
#define PSA_MAC_MAX_SIZE /* implementation-defined value */

/**
 * @brief The stream cipher mode of a stream cipher algorithm.
 */
#define PSA_ALG_STREAM_CIPHER ((psa_algorithm_t)0x04800100)

/**
 * @brief A stream cipher built using the Counter (CTR) mode of a block cipher.
 */
#define PSA_ALG_CTR ((psa_algorithm_t)0x04c01000)

/**
 * @brief The CCM* cipher mode without authentication.
 */
#define PSA_ALG_CCM_STAR_NO_TAG ((psa_algorithm_t)0x04c01300)

/**
 * @brief A stream cipher built using the Cipher Feedback (CFB) mode of a block
 *        cipher.
 */
#define PSA_ALG_CFB ((psa_algorithm_t)0x04c01100)

/**
 * @brief A stream cipher built using the Output Feedback (OFB) mode of a block
 *        cipher.
 */
#define PSA_ALG_OFB ((psa_algorithm_t)0x04c01200)

/**
 * @brief The XEX with Ciphertext Stealing (XTS) cipher mode of a block cipher.
 */
#define PSA_ALG_XTS ((psa_algorithm_t)0x0440ff00)

/**
 * @brief The Electronic Codebook (ECB) mode of a block cipher, with no padding.
 */
#define PSA_ALG_ECB_NO_PADDING ((psa_algorithm_t)0x04404400)

/**
 * @brief The Cipher Block Chaining (CBC) mode of a block cipher, with no
 *        padding.
 */
#define PSA_ALG_CBC_NO_PADDING ((psa_algorithm_t)0x04404000)

/**
 * @brief The Cipher Block Chaining (CBC) mode of a block cipher, with PKCS#7
 *        padding.
 */
#define PSA_ALG_CBC_PKCS7 ((psa_algorithm_t)0x04404100)

/**
 * @brief Encrypt a message using a symmetric cipher.
 *
 * @param key           Identifier of the key to use for the operation.
 * @param alg           The cipher algorithm to compute: a value of type
 *                      psa_algorithm_t such that PSA_ALG_IS_CIPHER(alg) is
 *                      true.
 * @param input         Buffer containing the message to encrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param output        Buffer where the output is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the output.
 */
psa_status_t psa_cipher_encrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);

/**
 * @brief Decrypt a message using a symmetric cipher.
 *
 * @param key           Identifier of the key to use for the operation.
 * @param alg           The cipher algorithm to compute: a value of type
 *                      psa_algorithm_t such that PSA_ALG_IS_CIPHER(alg) is
 *                      true.
 * @param input         Buffer containing the message to decrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param output        Buffer where the plaintext is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the output.
 */
psa_status_t psa_cipher_decrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);

/**
 * @brief The type of the state object for multi-part cipher operations.
 */
typedef /* implementation-defined type */ psa_cipher_operation_t;

/**
 * @brief This macro returns a suitable initializer for a cipher operation
 *        object of type psa_cipher_operation_t.
 */
#define PSA_CIPHER_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a cipher operation object.
 */
psa_cipher_operation_t psa_cipher_operation_init(void);

/**
 * @brief Set the key for a multi-part symmetric encryption operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The cipher algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_CIPHER(alg) is true.
 */
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief Set the key for a multi-part symmetric decryption operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The cipher algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_CIPHER(alg) is true.
 */
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief Generate an initialization vector (IV) for a symmetric encryption
 *        operation.
 *
 * @param operation Active cipher operation.
 * @param iv        Buffer where the generated IV is to be written.
 * @param iv_size   Size of the iv buffer in bytes.
 * @param iv_length On success, the number of bytes of the generated IV.
 */
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t * operation,
                                    uint8_t * iv,
                                    size_t iv_size,
                                    size_t * iv_length);

/**
 * @brief Set the initialization vector (IV) for a symmetric encryption or
 *        decryption operation.
 *
 * @param operation Active cipher operation.
 * @param iv        Buffer containing the IV to use.
 * @param iv_length Size of the IV in bytes.
 */
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t * operation,
                               const uint8_t * iv,
                               size_t iv_length);

/**
 * @brief Encrypt or decrypt a message fragment in an active cipher operation.
 *
 * @param operation     Active cipher operation.
 * @param input         Buffer containing the message fragment to encrypt or
 *                      decrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param output        Buffer where the output is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the
 *                      returned output.
 */
psa_status_t psa_cipher_update(psa_cipher_operation_t * operation,
                               const uint8_t * input,
                               size_t input_length,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);

/**
 * @brief Finish encrypting or decrypting a message in a cipher operation.
 *
 * @param operation     Active cipher operation.
 * @param output        Buffer where the last part of the output is to be
 *                      written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the
 *                      returned output.
 */
psa_status_t psa_cipher_finish(psa_cipher_operation_t * operation,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);

/**
 * @brief Abort a cipher operation.
 *
 * @param operation Initialized cipher operation.
 */
psa_status_t psa_cipher_abort(psa_cipher_operation_t * operation);

/**
 * @brief Whether the specified algorithm is a stream cipher.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a stream cipher algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_STREAM_CIPHER(alg) /* specification-defined value */

/**
 * @brief A wildcard algorithm that permits the use of the key with CCM* as both
 *        an AEAD and an unauthenticated cipher algorithm.
 */
#define PSA_ALG_CCM_STAR_ANY_TAG ((psa_algorithm_t)0x04c09300)

/**
 * @brief A sufficient output buffer size for psa_cipher_encrypt(), in bytes.
 *
 * @param key_type     A symmetric key type that is compatible with algorithm
 *                     alg.
 * @param alg          A cipher algorithm: a value of type psa_algorithm_t such
 *                     that PSA_ALG_IS_CIPHER(alg) is true.
 * @param input_length Size of the input in bytes.
 *
 * @return A sufficient output size for the specified key type and algorithm.
 */
#define PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_encrypt(), for any of
 *        the supported key types and cipher algorithms.
 *
 * @param input_length Size of the input in bytes.
 */
#define PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_decrypt(), in bytes.
 *
 * @param key_type     A symmetric key type that is compatible with algorithm
 *                     alg.
 * @param alg          A cipher algorithm: a value of type psa_algorithm_t such
 *                     that PSA_ALG_IS_CIPHER(alg) is true.
 * @param input_length Size of the input in bytes.
 *
 * @return A sufficient output size for the specified key type and algorithm.
 */
#define PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_decrypt(), for any of
 *        the supported key types and cipher algorithms.
 *
 * @param input_length Size of the input in bytes.
 */
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */

/**
 * @brief The default IV size for a cipher algorithm, in bytes.
 *
 * @param key_type A symmetric key type that is compatible with algorithm alg.
 * @param alg      A cipher algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_CIPHER(alg) is true.
 *
 * @return The default IV size for the specified key type and algorithm.
 */
#define PSA_CIPHER_IV_LENGTH(key_type, alg) /* implementation-defined value */

/**
 * @brief A sufficient buffer size for storing the IV generated by
 *        psa_cipher_generate_iv(), for any of the supported key types and
 *        cipher algorithms.
 */
#define PSA_CIPHER_IV_MAX_SIZE /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_update(), in bytes.
 *
 * @param key_type     A symmetric key type that is compatible with algorithm
 *                     alg.
 * @param alg          A cipher algorithm: a value of type psa_algorithm_t such
 *                     that PSA_ALG_IS_CIPHER(alg) is true.
 * @param input_length Size of the input in bytes.
 *
 * @return A sufficient output size for the specified key type and algorithm.
 */
#define PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_update(), for any of
 *        the supported key types and cipher algorithms.
 *
 * @param input_length Size of the input in bytes.
 */
#define PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_finish().
 *
 * @param key_type A symmetric key type that is compatible with algorithm alg.
 * @param alg      A cipher algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_CIPHER(alg) is true.
 *
 * @return A sufficient output size for the specified key type and algorithm.
 */
#define PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_cipher_finish(), for any of
 *        the supported key types and cipher algorithms.
 */
#define PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief The block size of a block cipher.
 *
 * @param type A cipher key type: a value of type psa_key_type_t.
 *
 * @return The block size for a block cipher, or 1 for a stream cipher.
 */
#define PSA_BLOCK_CIPHER_BLOCK_LENGTH(type) /* specification-defined value */

/**
 * @brief The maximum block size of a block cipher supported by the
 *        implementation.
 */
#define PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE /* implementation-defined value */

/**
 * @brief The Counter with CBC-MAC (CCM) authenticated encryption algorithm.
 */
#define PSA_ALG_CCM ((psa_algorithm_t)0x05500100)

/**
 * @brief The Galois/Counter Mode (GCM) authenticated encryption algorithm.
 */
#define PSA_ALG_GCM ((psa_algorithm_t)0x05500200)

/**
 * @brief The ChaCha20-Poly1305 AEAD algorithm.
 */
#define PSA_ALG_CHACHA20_POLY1305 ((psa_algorithm_t)0x05100500)

/**
 * @brief The XChaCha20-Poly1305 AEAD algorithm.
 */
#define PSA_ALG_XCHACHA20_POLY1305 ((psa_algorithm_t)0x05100600)

/**
 * @brief The Ascon-AEAD128 AEAD algorithm.
 */
#define PSA_ALG_ASCON_AEAD128 ((psa_algorithm_t)0x05100700)

/**
 * @brief Macro to build a AEAD algorithm with a shortened tag.
 *
 * @param aead_alg   An AEAD algorithm: a value of type psa_algorithm_t such
 *                   that PSA_ALG_IS_AEAD(aead_alg) is true.
 * @param tag_length Desired length of the authentication tag in bytes.
 *
 * @return The corresponding AEAD algorithm with the specified tag length.
 */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
    /* specification-defined value */

/**
 * @brief An AEAD algorithm with the default tag length.
 *
 * @param aead_alg An AEAD algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_AEAD(aead_alg) is true.
 *
 * @return The corresponding AEAD algorithm with the default tag length for that
 *         algorithm.
 */
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
    /* specification-defined value */

/**
 * @brief Macro to build an AEAD minimum-tag-length wildcard algorithm.
 *
 * @param aead_alg       An AEAD algorithm: a value of type psa_algorithm_t such
 *                       that PSA_ALG_IS_AEAD(aead_alg) is true.
 * @param min_tag_length Desired minimum length of the authentication tag in
 *                       bytes.
 *
 * @return The corresponding AEAD wildcard algorithm with the specified minimum
 *         tag length.
 */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    /* specification-defined value */

/**
 * @brief Process an authenticated encryption operation.
 *
 * @param key                    Identifier of the key to use for the operation.
 * @param alg                    The AEAD algorithm to compute: a value of type
 *                               psa_algorithm_t such that PSA_ALG_IS_AEAD(alg)
 *                               is true.
 * @param nonce                  Nonce or IV to use.
 * @param nonce_length           Size of the nonce buffer in bytes.
 * @param additional_data        Additional data that will be authenticated but
 *                               not encrypted.
 * @param additional_data_length Size of additional_data in bytes.
 * @param plaintext              Data that will be authenticated and encrypted.
 * @param plaintext_length       Size of plaintext in bytes.
 * @param ciphertext             Output buffer for the authenticated and
 *                               encrypted data.
 * @param ciphertext_size        Size of the ciphertext buffer in bytes.
 * @param ciphertext_length      On success, the size of the output in the
 *                               ciphertext buffer.
 */
psa_status_t psa_aead_encrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * nonce,
                              size_t nonce_length,
                              const uint8_t * additional_data,
                              size_t additional_data_length,
                              const uint8_t * plaintext,
                              size_t plaintext_length,
                              uint8_t * ciphertext,
                              size_t ciphertext_size,
                              size_t * ciphertext_length);

/**
 * @brief Process an authenticated decryption operation.
 *
 * @param key                    Identifier of the key to use for the operation.
 * @param alg                    The AEAD algorithm to compute: a value of type
 *                               psa_algorithm_t such that PSA_ALG_IS_AEAD(alg)
 *                               is true.
 * @param nonce                  Nonce or IV to use.
 * @param nonce_length           Size of the nonce buffer in bytes.
 * @param additional_data        Additional data that has been authenticated but
 *                               not encrypted.
 * @param additional_data_length Size of additional_data in bytes.
 * @param ciphertext             Data that has been authenticated and encrypted.
 * @param ciphertext_length      Size of ciphertext in bytes.
 * @param plaintext              Output buffer for the decrypted data.
 * @param plaintext_size         Size of the plaintext buffer in bytes.
 * @param plaintext_length       On success, the size of the output in the
 *                               plaintext buffer.
 */
psa_status_t psa_aead_decrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * nonce,
                              size_t nonce_length,
                              const uint8_t * additional_data,
                              size_t additional_data_length,
                              const uint8_t * ciphertext,
                              size_t ciphertext_length,
                              uint8_t * plaintext,
                              size_t plaintext_size,
                              size_t * plaintext_length);

/**
 * @brief The type of the state object for multi-part AEAD operations.
 */
typedef /* implementation-defined type */ psa_aead_operation_t;

/**
 * @brief This macro returns a suitable initializer for an AEAD operation object
 *        of type psa_aead_operation_t.
 */
#define PSA_AEAD_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for an AEAD operation object.
 */
psa_aead_operation_t psa_aead_operation_init(void);

/**
 * @brief Set the key for a multi-part authenticated encryption operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The AEAD algorithm: a value of type psa_algorithm_t such
 *                  that PSA_ALG_IS_AEAD(alg) is true.
 */
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);

/**
 * @brief Set the key for a multi-part authenticated decryption operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       The AEAD algorithm to compute: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_AEAD(alg) is true.
 */
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);

/**
 * @brief Declare the lengths of the message and additional data for AEAD.
 *
 * @param operation        Active AEAD operation.
 * @param ad_length        Size of the non-encrypted additional authenticated
 *                         data in bytes.
 * @param plaintext_length Size of the plaintext to encrypt in bytes.
 */
psa_status_t psa_aead_set_lengths(psa_aead_operation_t * operation,
                                  size_t ad_length,
                                  size_t plaintext_length);

/**
 * @brief Generate a random nonce for an authenticated encryption operation.
 *
 * @param operation    Active AEAD operation.
 * @param nonce        Buffer where the generated nonce is to be written.
 * @param nonce_size   Size of the nonce buffer in bytes.
 * @param nonce_length On success, the number of bytes of the generated nonce.
 */
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t * operation,
                                     uint8_t * nonce,
                                     size_t nonce_size,
                                     size_t * nonce_length);

/**
 * @brief Set the nonce for an authenticated encryption or decryption operation.
 *
 * @param operation    Active AEAD operation.
 * @param nonce        Buffer containing the nonce to use.
 * @param nonce_length Size of the nonce in bytes.
 */
psa_status_t psa_aead_set_nonce(psa_aead_operation_t * operation,
                                const uint8_t * nonce,
                                size_t nonce_length);

/**
 * @brief Pass additional data to an active AEAD operation.
 *
 * @param operation    Active AEAD operation.
 * @param input        Buffer containing the fragment of additional data.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_aead_update_ad(psa_aead_operation_t * operation,
                                const uint8_t * input,
                                size_t input_length);

/**
 * @brief Encrypt or decrypt a message fragment in an active AEAD operation.
 *
 * @param operation     Active AEAD operation.
 * @param input         Buffer containing the message fragment to encrypt or
 *                      decrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param output        Buffer where the output is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the
 *                      returned output.
 */
psa_status_t psa_aead_update(psa_aead_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * output,
                             size_t output_size,
                             size_t * output_length);

/**
 * @brief Finish encrypting a message in an AEAD operation.
 *
 * @param operation         Active AEAD operation.
 * @param ciphertext        Buffer where the last part of the ciphertext is to
 *                          be written.
 * @param ciphertext_size   Size of the ciphertext buffer in bytes.
 * @param ciphertext_length On success, the number of bytes of returned
 *                          ciphertext.
 * @param tag               Buffer where the authentication tag is to be
 *                          written.
 * @param tag_size          Size of the tag buffer in bytes.
 * @param tag_length        On success, the number of bytes that make up the
 *                          returned tag.
 */
psa_status_t psa_aead_finish(psa_aead_operation_t * operation,
                             uint8_t * ciphertext,
                             size_t ciphertext_size,
                             size_t * ciphertext_length,
                             uint8_t * tag,
                             size_t tag_size,
                             size_t * tag_length);

/**
 * @brief Finish authenticating and decrypting a message in an AEAD operation.
 *
 * @param operation        Active AEAD operation.
 * @param plaintext        Buffer where the last part of the plaintext is to be
 *                         written.
 * @param plaintext_size   Size of the plaintext buffer in bytes.
 * @param plaintext_length On success, the number of bytes of returned
 *                         plaintext.
 * @param tag              Buffer containing the expected authentication tag.
 * @param tag_length       Size of the tag buffer in bytes.
 */
psa_status_t psa_aead_verify(psa_aead_operation_t * operation,
                             uint8_t * plaintext,
                             size_t plaintext_size,
                             size_t * plaintext_length,
                             const uint8_t * tag,
                             size_t tag_length);

/**
 * @brief Abort an AEAD operation.
 *
 * @param operation Initialized AEAD operation.
 */
psa_status_t psa_aead_abort(psa_aead_operation_t * operation);

/**
 * @brief Whether the specified algorithm is an AEAD mode on a block cipher.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an AEAD algorithm which is an AEAD mode based on a block
 *         cipher, 0 otherwise.
 */
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) /* specification-defined value */

/**
 * @brief A sufficient ciphertext buffer size for psa_aead_encrypt(), in bytes.
 *
 * @param key_type         A symmetric key type that is compatible with
 *                         algorithm alg.
 * @param alg              An AEAD algorithm: a value of type psa_algorithm_t
 *                         such that PSA_ALG_IS_AEAD(alg) is true.
 * @param plaintext_length Size of the plaintext in bytes.
 *
 * @return The AEAD ciphertext size for the specified key type and algorithm.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient ciphertext buffer size for psa_aead_encrypt(), for any of
 *        the supported key types and AEAD algorithms.
 *
 * @param plaintext_length Size of the plaintext in bytes.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient plaintext buffer size for psa_aead_decrypt(), in bytes.
 *
 * @param key_type          A symmetric key type that is compatible with
 *                          algorithm alg.
 * @param alg               An AEAD algorithm: a value of type psa_algorithm_t
 *                          such that PSA_ALG_IS_AEAD(alg) is true.
 * @param ciphertext_length Size of the ciphertext in bytes.
 *
 * @return The AEAD plaintext size for the specified key type and algorithm.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient plaintext buffer size for psa_aead_decrypt(), for any of
 *        the supported key types and AEAD algorithms.
 *
 * @param ciphertext_length Size of the ciphertext in bytes.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length) \
    /* implementation-defined value */

/**
 * @brief The default nonce size for an AEAD algorithm, in bytes.
 *
 * @param key_type A symmetric key type that is compatible with algorithm alg.
 * @param alg      An AEAD algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_AEAD(alg) is true.
 *
 * @return The default nonce size for the specified key type and algorithm.
 */
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) /* implementation-defined value */

/**
 * @brief A sufficient buffer size for storing the nonce generated by
 *        psa_aead_generate_nonce(), for any of the supported key types and AEAD
 *        algorithms.
 */
#define PSA_AEAD_NONCE_MAX_SIZE /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_aead_update().
 *
 * @param key_type     A symmetric key type that is compatible with algorithm
 *                     alg.
 * @param alg          An AEAD algorithm: a value of type psa_algorithm_t such
 *                     that PSA_ALG_IS_AEAD(alg) is true.
 * @param input_length Size of the input in bytes.
 *
 * @return A sufficient output buffer size for the specified key type and
 *         algorithm.
 */
#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_aead_update(), for any of the
 *        supported key types and AEAD algorithms.
 *
 * @param input_length Size of the input in bytes.
 */
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */

/**
 * @brief A sufficient ciphertext buffer size for psa_aead_finish().
 *
 * @param key_type A symmetric key type that is compatible with algorithm alg.
 * @param alg      An AEAD algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_AEAD(alg) is true.
 *
 * @return A sufficient ciphertext buffer size for the specified key type and
 *         algorithm.
 */
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient ciphertext buffer size for psa_aead_finish(), for any of
 *        the supported key types and AEAD algorithms.
 */
#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief The length of a tag for an AEAD algorithm, in bytes.
 *
 * @param key_type The type of the AEAD key.
 * @param key_bits The size of the AEAD key in bits.
 * @param alg      An AEAD algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_AEAD(alg) is true.
 *
 * @return The tag length for the specified algorithm and key.
 */
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient buffer size for storing the tag output by
 *        psa_aead_finish(), for any of the supported key types and AEAD
 *        algorithms.
 */
#define PSA_AEAD_TAG_MAX_SIZE /* implementation-defined value */

/**
 * @brief A sufficient plaintext buffer size for psa_aead_verify(), in bytes.
 *
 * @param key_type A symmetric key type that is compatible with algorithm alg.
 * @param alg      An AEAD algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_AEAD(alg) is true.
 *
 * @return A sufficient plaintext buffer size for the specified key type and
 *         algorithm.
 */
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient plaintext buffer size for psa_aead_verify(), for any of
 *        the supported key types and AEAD algorithms.
 */
#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief A key-wrapping algorithm based on the NIST Key Wrap (KW) mode of a
 *        block cipher.
 */
#define PSA_ALG_KW ((psa_algorithm_t)0x0B400100)

/**
 * @brief A key-wrapping algorithm based on the NIST Key Wrap with Padding (KWP)
 *        mode of a block cipher.
 */
#define PSA_ALG_KWP ((psa_algorithm_t)0x0BC00200)

/**
 * @brief Unwrap and import a key using a specified wrapping key.
 *
 * @param attributes   The attributes for the new key.
 * @param wrapping_key Identifier of the key to use for the unwrapping
 *                     operation.
 * @param alg          The key-wrapping algorithm: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_KEY_WRAP(alg) is
 *                     true.
 * @param data         Buffer containing the wrapped key data.
 * @param data_length  Size of the data buffer in bytes.
 * @param key          On success, an identifier for the newly created key.
 */
psa_status_t psa_unwrap_key(const psa_key_attributes_t * attributes,
                            psa_key_id_t wrapping_key,
                            psa_algorithm_t alg,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key);

/**
 * @brief Wrap and export a key using a specified wrapping key.
 *
 * @param wrapping_key Identifier of the key to use for the wrapping operation.
 * @param alg          The key-wrapping algorithm: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_KEY_WRAP(alg) is
 *                     true.
 * @param key          Identifier of the key to wrap.
 * @param data         Buffer where the wrapped key data is to be written.
 * @param data_size    Size of the data buffer in bytes.
 * @param data_length  On success, the number of bytes that make up the wrapped
 *                     key data.
 */
psa_status_t psa_wrap_key(psa_key_id_t wrapping_key,
                          psa_algorithm_t alg,
                          psa_key_id_t key,
                          uint8_t * data,
                          size_t data_size,
                          size_t * data_length);

/**
 * @brief Sufficient output buffer size for psa_wrap_key().
 *
 * @param wrap_key_type A supported key-wrapping key type.
 * @param alg           A supported key-wrapping algorithm.
 * @param key_type      A supported key type.
 * @param key_bits      The size of the key in bits.
 *
 * @return If the parameters are valid and supported, return a buffer size in
 *         bytes that guarantees that psa_wrap_key() will not fail with
 *         PSA_ERROR_BUFFER_TOO_SMALL.
 */
#define PSA_WRAP_KEY_OUTPUT_SIZE(wrap_key_type, alg, key_type, key_bits) \
    /* implementation-defined value */

/**
 * @brief Sufficient buffer size for wrapping any asymmetric key pair.
 */
#define PSA_WRAP_KEY_PAIR_MAX_SIZE /* implementation-defined value */

/**
 * @brief Macro to build an HKDF algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HKDF algorithm.
 */
#define PSA_ALG_HKDF(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build an HKDF-Extract algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HKDF-Extract algorithm.
 */
#define PSA_ALG_HKDF_EXTRACT(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build an HKDF-Expand algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HKDF-Expand algorithm.
 */
#define PSA_ALG_HKDF_EXPAND(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build a NIST SP 800-108 conformant, counter-mode KDF
 *        algorithm based on HMAC.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding key-derivation algorithm.
 */
#define PSA_ALG_SP800_108_COUNTER_HMAC(hash_alg) \
    /* specification-defined value */

/**
 * @brief Macro to build a NIST SP 800-108 conformant, counter-mode KDF
 *        algorithm based on CMAC.
 */
#define PSA_ALG_SP800_108_COUNTER_CMAC ((psa_algorithm_t)0x08000800)

/**
 * @brief Macro to build a TLS-1.2 PRF algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding TLS-1.2 PRF algorithm.
 */
#define PSA_ALG_TLS12_PRF(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build a TLS-1.2 PSK-to-MasterSecret algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding TLS-1.2 PSK to MS algorithm.
 */
#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) /* specification-defined value */

/**
 * @brief The TLS 1.2 ECJPAKE-to-PMS key-derivation algorithm.
 */
#define PSA_ALG_TLS12_ECJPAKE_TO_PMS ((psa_algorithm_t)0x08000609)

/**
 * @brief The WPA3-SAE hash-to-element password token key-derivation algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 */
#define PSA_ALG_WPA3_SAE_H2E(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build a PBKDF2-HMAC password-hashing or key-stretching
 *        algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding PBKDF2-HMAC-XXX algorithm.
 */
#define PSA_ALG_PBKDF2_HMAC(hash_alg) /* specification-defined value */

/**
 * @brief The PBKDF2-AES-CMAC-PRF-128 password-hashing or key-stretching
 *        algorithm.
 */
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)

/**
 * @brief Encoding of the step of a key derivation.
 */
typedef uint16_t psa_key_derivation_step_t;

/**
 * @brief A high-entropy secret input for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_SECRET /* implementation-defined value */

/**
 * @brief A high-entropy additional secret input for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_OTHER_SECRET \
    /* implementation-defined value */

/**
 * @brief A low-entropy secret input for password hashing or key stretching.
 */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD /* implementation-defined value */

/**
 * @brief A label for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_LABEL /* implementation-defined value */

/**
 * @brief A context for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_CONTEXT /* implementation-defined value */

/**
 * @brief A salt for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_SALT /* implementation-defined value */

/**
 * @brief An information string for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_INFO /* implementation-defined value */

/**
 * @brief A seed for key derivation.
 */
#define PSA_KEY_DERIVATION_INPUT_SEED /* implementation-defined value */

/**
 * @brief A cost parameter for password hashing or key stretching.
 */
#define PSA_KEY_DERIVATION_INPUT_COST /* implementation-defined value */

/**
 * @brief The type of the state object for key-derivation operations.
 */
typedef /* implementation-defined type */ psa_key_derivation_operation_t;

/**
 * @brief This macro returns a suitable initializer for a key-derivation
 *        operation object of type psa_key_derivation_operation_t.
 */
#define PSA_KEY_DERIVATION_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a key-derivation operation object.
 */
psa_key_derivation_operation_t psa_key_derivation_operation_init(void);

/**
 * @brief Set up a key-derivation operation.
 *
 * @param operation The key-derivation operation object to set up.
 * @param alg       The algorithm to compute.
 */
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t * operation,
                                      psa_algorithm_t alg);

/**
 * @brief Retrieve the current capacity of a key-derivation operation.
 *
 * @param operation The operation to query.
 * @param capacity  On success, the capacity of the operation.
 */
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t * operation,
                                             size_t * capacity);

/**
 * @brief Set the maximum capacity of a key-derivation operation.
 *
 * @param operation The key-derivation operation object to modify.
 * @param capacity  The new capacity of the operation.
 */
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t * operation,
                                             size_t capacity);

/**
 * @brief Provide an input for key derivation or key agreement.
 *
 * @param operation   The key-derivation operation object to use.
 * @param step        Which step the input data is for.
 * @param data        Input data to use.
 * @param data_length Size of the data buffer in bytes.
 */
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t * operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t * data,
                                            size_t data_length);

/**
 * @brief Provide a numeric input for key derivation or key agreement.
 *
 * @param operation The key-derivation operation object to use.
 * @param step      Which step the input data is for.
 * @param value     The value of the numeric input.
 */
psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              uint64_t value);

/**
 * @brief Provide an input for key derivation in the form of a key.
 *
 * @param operation The key-derivation operation object to use.
 * @param step      Which step the input data is for.
 * @param key       Identifier of the key.
 */
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t * operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key);

/**
 * @brief Read some data from a key-derivation operation.
 *
 * @param operation     The key-derivation operation object to read from.
 * @param output        Buffer where the output will be written.
 * @param output_length Number of bytes to output.
 */
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t * operation,
                                             uint8_t * output,
                                             size_t output_length);

/**
 * @brief Derive a key from an ongoing key-derivation operation.
 *
 * @param attributes The attributes for the new key.
 * @param operation  The key-derivation operation object to read from.
 * @param key        On success, an identifier for the newly created key.
 */
psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t * attributes,
                                           psa_key_derivation_operation_t * operation,
                                           psa_key_id_t * key);

/**
 * @brief Derive a key from an ongoing key-derivation operation with custom
 *        production parameters.
 *
 * @param attributes         The attributes for the new key.
 * @param operation          The key-derivation operation object to read from.
 * @param custom             Customized production parameters for the key
 *                           derivation.
 * @param custom_data        A buffer containing additional variable-sized
 *                           production parameters.
 * @param custom_data_length Length of custom_data in bytes.
 * @param key                On success, an identifier for the newly created
 *                           key.
 */
psa_status_t psa_key_derivation_output_key_custom(const psa_key_attributes_t * attributes,
                                                  psa_key_derivation_operation_t * operation,
                                                  const psa_custom_key_parameters_t * custom,
                                                  const uint8_t * custom_data,
                                                  size_t custom_data_length,
                                                  psa_key_id_t * key);

/**
 * @brief Compare output data from a key-derivation operation to an expected
 *        value.
 *
 * @param operation       The key-derivation operation object to read from.
 * @param expected_output Buffer containing the expected derivation output.
 * @param output_length   Length of the expected output.
 */
psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t * operation,
                                             const uint8_t * expected_output,
                                             size_t output_length);

/**
 * @brief Compare output data from a key-derivation operation to an expected
 *        value stored in a key.
 *
 * @param operation The key-derivation operation object to read from.
 * @param expected  A key of type PSA_KEY_TYPE_PASSWORD_HASH containing the
 *                  expected output.
 */
psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t * operation,
                                           psa_key_id_t expected);

/**
 * @brief Abort a key-derivation operation.
 *
 * @param operation The operation to abort.
 */
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t * operation);

/**
 * @brief Whether the specified algorithm is a key-stretching or password-
 *        hashing algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-stretching or password-hashing algorithm, 0
 *         otherwise.
 */
#define PSA_ALG_IS_KEY_DERIVATION_STRETCHING(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an HKDF algorithm
 *        (PSA_ALG_HKDF(hash_alg)).
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an HKDF algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HKDF(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an HKDF-Extract algorithm
 *        (PSA_ALG_HKDF_EXTRACT(hash_alg)).
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an HKDF-Extract algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HKDF_EXTRACT(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an HKDF-Expand algorithm
 *        (PSA_ALG_HKDF_EXPAND(hash_alg)).
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an HKDF-Expand algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HKDF_EXPAND(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a key-derivation algorithm
 *        constructed using PSA_ALG_SP800_108_COUNTER_HMAC(hash_alg).
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a key-derivation algorithm constructed using
 *         PSA_ALG_SP800_108_COUNTER_HMAC(), 0 otherwise.
 */
#define PSA_ALG_IS_SP800_108_COUNTER_HMAC(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a TLS-1.2 PRF algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a TLS-1.2 PRF algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_TLS12_PRF(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a TLS-1.2 PSK to MS algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a TLS-1.2 PSK to MS algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a PBKDF2-HMAC algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a PBKDF2-HMAC algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_PBKDF2_HMAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a WPA3-SAE hash-to-element key-
 *        derivation algorithmAdded in version 1.4.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a WPA3-SAE hash-to-element algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_WPA3_SAE_H2E(alg) /* specification-defined value */

/**
 * @brief Use the maximum possible capacity for a key-derivation operation.
 */
#define PSA_KEY_DERIVATION_UNLIMITED_CAPACITY \
    /* implementation-defined value */

/**
 * @brief This macro returns the maximum supported length of the PSK for the
 *        TLS-1.2 PSK-to-MS key derivation.
 */
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE /* implementation-defined value */

/**
 * @brief The size of the output from the TLS 1.2 ECJPAKE-to-PMS key-derivation
 *        algorithm, in bytes.
 */
#define PSA_TLS12_ECJPAKE_TO_PMS_OUTPUT_SIZE 32

/**
 * @brief The RSA PKCS#1 v1.5 message signature scheme, with hashing.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding RSA PKCS#1 v1.5 signature algorithm.
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg) /* specification-defined value */

/**
 * @brief The raw RSA PKCS#1 v1.5 signature algorithm, without hashing.
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW ((psa_algorithm_t) 0x06000200)

/**
 * @brief The RSA PSS message signature scheme, with hashing.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding RSA PSS signature algorithm.
 */
#define PSA_ALG_RSA_PSS(hash_alg) /* specification-defined value */

/**
 * @brief The RSA PSS message signature scheme, with hashing.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding RSA PSS signature algorithm.
 */
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an RSA PKCS#1 v1.5 signature
 *        algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an RSA PKCS#1 v1.5 signature algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an RSA PSS signature algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an RSA PSS signature algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_RSA_PSS(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an RSA PSS signature algorithm that
 *        permits any salt length.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an RSA PSS signature algorithm that permits any salt
 *         length, 0 otherwise.
 */
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an RSA PSS signature algorithm that
 *        requires the standard salt length.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an RSA PSS signature algorithm that requires the standard
 *         salt length, 0 otherwise.
 */
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) /* specification-defined value */

/**
 * @brief The randomized ECDSA signature scheme, with hashing.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding randomized ECDSA signature algorithm.
 */
#define PSA_ALG_ECDSA(hash_alg) /* specification-defined value */

/**
 * @brief The randomized ECDSA signature scheme, without hashing.
 */
#define PSA_ALG_ECDSA_ANY ((psa_algorithm_t) 0x06000600)

/**
 * @brief Deterministic ECDSA signature scheme, with hashing.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding deterministic ECDSA signature algorithm.
 */
#define PSA_ALG_DETERMINISTIC_ECDSA(hash_alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is ECDSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an ECDSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_ECDSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is deterministic ECDSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a deterministic ECDSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is randomized ECDSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a randomized ECDSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_RANDOMIZED_ECDSA(alg) /* specification-defined value */

/**
 * @brief Edwards-curve digital signature algorithm without pre-hashing
 *        (PureEdDSA), with zero-length context.
 */
#define PSA_ALG_PURE_EDDSA ((psa_algorithm_t) 0x06000800)

/**
 * @brief Edwards-curve digital signature algorithm without pre-hashing
 *        (PureEdDSA), with a context.
 */
#define PSA_ALG_EDDSA_CTX ((psa_algorithm_t) 0x06000A00)

/**
 * @brief Edwards-curve digital signature algorithm with pre-hashing
 *        (HashEdDSA), using the Edwards25519 curve.
 */
#define PSA_ALG_ED25519PH ((psa_algorithm_t) 0x0600090B)

/**
 * @brief Edwards-curve digital signature algorithm with pre-hashing
 *        (HashEdDSA), using the Edwards448 curve.
 */
#define PSA_ALG_ED448PH ((psa_algorithm_t) 0x06000915)

/**
 * @brief Whether the specified algorithm is HashEdDSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a HashEdDSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_EDDSA(alg) /* specification-defined value */

/**
 * @brief Stateless hash-based digital signature algorithm without pre-hashing
 *        (SLH-DSA).
 */
#define PSA_ALG_SLH_DSA ((psa_algorithm_t) 0x06004000)

/**
 * @brief Deterministic stateless hash-based digital signature algorithm without
 *        pre-hashing (SLH-DSA).
 */
#define PSA_ALG_DETERMINISTIC_SLH_DSA ((psa_algorithm_t) 0x06004100)

/**
 * @brief Stateless hash-based digital signature algorithm with pre-hashing
 *        (HashSLH-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HashSLH-DSA signature algorithm, using hash_alg to
 *         pre-hash the message.
 */
#define PSA_ALG_HASH_SLH_DSA(hash_alg) /* specification-defined value */

/**
 * @brief Deterministic stateless hash-based digital signature algorithm with
 *        pre-hashing (HashSLH-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding deterministic HashSLH-DSA signature algorithm,
 *         using hash_alg to pre-hash the message.
 */
#define PSA_ALG_DETERMINISTIC_HASH_SLH_DSA(hash_alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is SLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an SLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is deterministic HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a deterministic HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_DETERMINISTIC_HASH_SLH_DSA(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is hedged HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hedged HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HEDGED_HASH_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Module lattice-based digital signature algorithm without pre-hashing
 *        (ML-DSA).
 */
#define PSA_ALG_ML_DSA ((psa_algorithm_t) 0x06004400)

/**
 * @brief Deterministic module lattice-based digital signature algorithm without
 *        pre-hashing (ML-DSA).
 */
#define PSA_ALG_DETERMINISTIC_ML_DSA ((psa_algorithm_t) 0x06004500)

/**
 * @brief Module lattice-based digital signature algorithm with pre-hashing
 *        (HashML-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HashML-DSA signature algorithm, using hash_alg to
 *         pre-hash the message.
 */
#define PSA_ALG_HASH_ML_DSA(hash_alg) /* specification-defined value */

/**
 * @brief Deterministic module lattice-based digital signature algorithm with
 *        pre-hashing (HashML-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding deterministic HashML-DSA signature algorithm, using
 *         hash_alg to pre-hash the message.
 */
#define PSA_ALG_DETERMINISTIC_HASH_ML_DSA(hash_alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is ML-DSA, without pre-hashing.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a pure ML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_ML_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_ML_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is deterministic HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a deterministic HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is hedged HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hedged HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HEDGED_HASH_ML_DSA(alg) /* specification-defined value */

/**
 * @brief Leighton-Micali Signatures (LMS) signature algorithm.
 */
#define PSA_ALG_LMS ((psa_algorithm_t) 0x06004800)

/**
 * @brief Hierarchical Signature Scheme (HSS) signature algorithm.
 */
#define PSA_ALG_HSS ((psa_algorithm_t) 0x06004900)

/**
 * @brief eXtended Merkle Signature Scheme (XMSS) signature algorithm.
 */
#define PSA_ALG_XMSS ((psa_algorithm_t) 0x06004A00)

/**
 * @brief Multi-tree eXtended Merkle Signature Scheme (XMSS^MT) signature
 *        algorithm.
 */
#define PSA_ALG_XMSS_MT ((psa_algorithm_t) 0x06004B00)

/**
 * @brief Sign a message with a private key.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm: a value of type
 *                         psa_algorithm_t such that
 *                         PSA_ALG_IS_SIGN_MESSAGE(alg) is true.
 * @param input            The input message to sign.
 * @param input_length     Size of the input buffer in bytes.
 * @param signature        Buffer where the signature is to be written.
 * @param signature_size   Size of the signature buffer in bytes.
 * @param signature_length On success, the number of bytes that make up the
 *                         returned signature value.
 */
psa_status_t psa_sign_message(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * signature,
                              size_t signature_size,
                              size_t * signature_length);

/**
 * @brief Sign a message with a private key using a supplied context.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm: a value of type
 *                         psa_algorithm_t such that
 *                         PSA_ALG_IS_SIGN_MESSAGE(alg) is true.
 * @param input            The input message to sign.
 * @param input_length     Size of the input buffer in bytes.
 * @param context          The context to use for this signature.
 * @param context_length   Size of the context buffer in bytes.
 * @param signature        Buffer where the signature is to be written.
 * @param signature_size   Size of the signature buffer in bytes.
 * @param signature_length On success, the number of bytes that make up the
 *                         returned signature value.
 */
psa_status_t psa_sign_message_with_context(psa_key_id_t key,
                                           psa_algorithm_t alg,
                                           const uint8_t * input,
                                           size_t input_length,
                                           const uint8_t * context,
                                           size_t context_length,
                                           uint8_t * signature,
                                           size_t signature_size,
                                           size_t * signature_length);

/**
 * @brief Verify the signature of a message with a public key.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm: a value of type
 *                         psa_algorithm_t such that
 *                         PSA_ALG_IS_SIGN_MESSAGE(alg) is true.
 * @param input            The message whose signature is to be verified.
 * @param input_length     Size of the input buffer in bytes.
 * @param signature        Buffer containing the signature to verify.
 * @param signature_length Size of the signature buffer in bytes.
 */
psa_status_t psa_verify_message(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                const uint8_t * signature,
                                size_t signature_length);

/**
 * @brief Verify the signature of a message with a public key and a supplied
 *        context.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm: a value of type
 *                         psa_algorithm_t such that
 *                         PSA_ALG_IS_SIGN_MESSAGE(alg) is true.
 * @param input            The message whose signature is to be verified.
 * @param input_length     Size of the input buffer in bytes.
 * @param context          The context to use for this signature.
 * @param context_length   Size of the context buffer in bytes.
 * @param signature        Buffer containing the signature to verify.
 * @param signature_length Size of the signature buffer in bytes.
 */
psa_status_t psa_verify_message_with_context(psa_key_id_t key,
                                             psa_algorithm_t alg,
                                             const uint8_t * input,
                                             size_t input_length,
                                             const uint8_t * context,
                                             size_t context_length,
                                             const uint8_t * signature,
                                             size_t signature_length);

/**
 * @brief Sign a pre-computed hash with a private key.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm that separates the
 *                         hash and sign operations: a value of type
 *                         psa_algorithm_t such that PSA_ALG_IS_SIGN_HASH(alg)
 *                         is true.
 * @param hash             The input to sign.
 * @param hash_length      Size of the hash buffer in bytes.
 * @param signature        Buffer where the signature is to be written.
 * @param signature_size   Size of the signature buffer in bytes.
 * @param signature_length On success, the number of bytes that make up the
 *                         returned signature value.
 */
psa_status_t psa_sign_hash(psa_key_id_t key,
                           psa_algorithm_t alg,
                           const uint8_t * hash,
                           size_t hash_length,
                           uint8_t * signature,
                           size_t signature_size,
                           size_t * signature_length);

/**
 * @brief Sign a pre-computed hash with a private key and a supplied context.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm that separates the
 *                         hash and sign operations: a value of type
 *                         psa_algorithm_t such that PSA_ALG_IS_SIGN_HASH(alg)
 *                         is true.
 * @param hash             The input to sign.
 * @param hash_length      Size of the hash buffer in bytes.
 * @param context          The context to use for this signature.
 * @param context_length   Size of the context buffer in bytes.
 * @param signature        Buffer where the signature is to be written.
 * @param signature_size   Size of the signature buffer in bytes.
 * @param signature_length On success, the number of bytes that make up the
 *                         returned signature value.
 */
psa_status_t psa_sign_hash_with_context(psa_key_id_t key,
                                        psa_algorithm_t alg,
                                        const uint8_t * hash,
                                        size_t hash_length,
                                        const uint8_t * context,
                                        size_t context_length,
                                        uint8_t * signature,
                                        size_t signature_size,
                                        size_t * signature_length);

/**
 * @brief Verify the signature of a hash or short message using a public key.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm that separates the
 *                         hash and sign operations: a value of type
 *                         psa_algorithm_t such that PSA_ALG_IS_SIGN_HASH(alg)
 *                         is true.
 * @param hash             The input whose signature is to be verified.
 * @param hash_length      Size of the hash buffer in bytes.
 * @param signature        Buffer containing the signature to verify.
 * @param signature_length Size of the signature buffer in bytes.
 */
psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * hash,
                             size_t hash_length,
                             const uint8_t * signature,
                             size_t signature_length);

/**
 * @brief Verify the signature of a hash or short message using a public key and
 *        a supplied context.
 *
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm that separates the
 *                         hash and sign operations: a value of type
 *                         psa_algorithm_t such that PSA_ALG_IS_SIGN_HASH(alg)
 *                         is true.
 * @param hash             The input whose signature is to be verified.
 * @param hash_length      Size of the hash buffer in bytes.
 * @param context          The context to use for this signature.
 * @param context_length   Size of the context buffer in bytes.
 * @param signature        Buffer containing the signature to verify.
 * @param signature_length Size of the signature buffer in bytes.
 */
psa_status_t psa_verify_hash_with_context(psa_key_id_t key,
                                          psa_algorithm_t alg,
                                          const uint8_t * hash,
                                          size_t hash_length,
                                          const uint8_t * context,
                                          size_t context_length,
                                          const uint8_t * signature,
                                          size_t signature_length);

/**
 * @brief The type of the state object for multi-part sign operations.
 */
typedef /* implementation-defined type */ psa_sign_operation_t;

/**
 * @brief This macro returns a suitable initializer for a sign operation object
 *        of type psa_sign_operation_t.
 */
#define PSA_SIGN_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a sign operation object.
 */
psa_sign_operation_t psa_sign_operation_init(void);

/**
 * @brief Set up a multi-part sign operation.
 *
 * @param operation The operation object to set up.
 * @param key       Identifier of the key to use for the operation.
 * @param alg       An asymmetric signature algorithm: a value of type
 *                  psa_algorithm_t such that PSA_ALG_IS_SIGN_MESSAGE(alg) is
 *                  true.
 */
psa_status_t psa_sign_setup(psa_sign_operation_t * operation,
                            psa_key_id_t key,
                            psa_algorithm_t alg);

/**
 * @brief Provide a context for a multi-part sign operation.
 *
 * @param operation      Active sign operation.
 * @param context        Buffer containing the context value.
 * @param context_length Size of the context buffer in bytes.
 */
psa_status_t psa_sign_set_context(psa_sign_operation_t * operation,
                                  const uint8_t * context,
                                  size_t context_length);

/**
 * @brief Add a message fragment to a multi-part sign operation.
 *
 * @param operation    Active sign operation.
 * @param input        Buffer containing the message fragment to sign.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_sign_update(psa_sign_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length);

/**
 * @brief Finish the calculation of an asymmetric signature of a message.
 *
 * @param operation        Active sign operation.
 * @param signature        Buffer where the signature is to be written.
 * @param signature_size   Size of the signature buffer in bytes.
 * @param signature_length On success, the number of bytes that make up the
 *                         returned signature value.
 */
psa_status_t psa_sign_finish(psa_sign_operation_t * operation,
                             uint8_t * signature,
                             size_t signature_size,
                             size_t * signature_length);

/**
 * @brief Abort a sign operation.
 *
 * @param operation Initialized sign operation.
 */
psa_status_t psa_sign_abort(psa_sign_operation_t * operation);

/**
 * @brief The type of the state object for multi-part asymmetric signature
 *        verification operations.
 */
typedef /* implementation-defined type */ psa_verify_operation_t;

/**
 * @brief This macro returns a suitable initializer for a verify operation
 *        object of type psa_verify_operation_t.
 */
#define PSA_VERIFY_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a verify operation object.
 */
psa_verify_operation_t psa_verify_operation_init(void);

/**
 * @brief Set up a multi-part verify operation.
 *
 * @param operation        The operation object to set up.
 * @param key              Identifier of the key to use for the operation.
 * @param alg              An asymmetric signature algorithm: a value of type
 *                         psa_algorithm_t such that
 *                         PSA_ALG_IS_SIGN_MESSAGE(alg) is true.
 * @param signature        Buffer containing the signature to verify.
 * @param signature_length Size of the signature buffer in bytes.
 */
psa_status_t psa_verify_setup(psa_verify_operation_t * operation,
                              psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * signature,
                              size_t signature_length);

/**
 * @brief Provide a context for a multi-part verify operation.
 *
 * @param operation      Active verify operation.
 * @param context        Buffer containing the context value.
 * @param context_length Size of the context buffer in bytes.
 */
psa_status_t psa_verify_set_context(psa_verify_operation_t * operation,
                                    const uint8_t * context,
                                    size_t context_length);

/**
 * @brief Add a message fragment to a multi-part verify operation.
 *
 * @param operation    Active verify operation.
 * @param input        Buffer containing the message fragment to verify.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_verify_update(psa_verify_operation_t * operation,
                               const uint8_t * input,
                               size_t input_length);

/**
 * @brief Report the validity of the message signature.
 *
 * @param operation Active verify operation.
 */
psa_status_t psa_verify_finish(psa_verify_operation_t * operation);

/**
 * @brief Abort a verify operation.
 *
 * @param operation Initialized verify operation.
 */
psa_status_t psa_verify_abort(psa_verify_operation_t * operation);

/**
 * @brief Whether the specified algorithm is a signature algorithm that can be
 *        used with message signature and verification functions, and with sign
 *        and verify multi-part operations.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a signature algorithm that can be used to sign a message.
 */
#define PSA_ALG_IS_SIGN_MESSAGE(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a signature algorithm that can be
 *        used with hash signature and verification functions.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a signature algorithm that can be used to sign a hash.
 */
#define PSA_ALG_IS_SIGN_HASH(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a hash-and-sign algorithm that
 *        signs exactly the hash value.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hash-and-sign algorithm that signs exactly the hash
 *         value, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_AND_SIGN(alg) /* specification-defined value */

/**
 * @brief Whether the specified signature algorithm can be used with a non-zero-
 *        length context.
 *
 * @param alg A signature algorithm identifier: a value of type psa_algorithm_t
 *            such that PSA_ALG_IS_SIGN(alg) is true.
 *
 * @return 1 if alg is a signature algorithm that can be used with a non-zero-
 *         length context.
 */
#define PSA_ALG_SIGN_SUPPORTS_CONTEXT(alg) /* implementation-defined value */

/**
 * @brief When setting a hash-and-sign algorithm in a key policy, permit any
 *        hash algorithm.
 */
#define PSA_ALG_ANY_HASH ((psa_algorithm_t)0x020000ff)

/**
 * @brief Sufficient signature buffer size for psa_sign_message() and
 *        psa_sign_hash().
 *
 * @param key_type An asymmetric key type.
 * @param key_bits The size of the key in bits.
 * @param alg      The signature algorithm.
 *
 * @return A sufficient signature buffer size for the specified asymmetric
 *         signature algorithm and key parameters.
 */
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient signature buffer size for psa_sign_message() and
 *        psa_sign_hash(), for any of the supported key types and asymmetric
 *        signature algorithms.
 */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */

/**
 * @brief The RSA PKCS#1 v1.5 asymmetric encryption algorithm.
 */
#define PSA_ALG_RSA_PKCS1V15_CRYPT ((psa_algorithm_t)0x07000200)

/**
 * @brief The RSA OAEP asymmetric encryption algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding RSA OAEP encryption algorithm.
 */
#define PSA_ALG_RSA_OAEP(hash_alg) /* specification-defined value */

/**
 * @brief Encrypt a short message with a public key.
 *
 * @param key           Identifier of the key to use for the operation.
 * @param alg           The asymmetric encryption algorithm to compute: a value
 *                      of type psa_algorithm_t such that
 *                      PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) is true.
 * @param input         The message to encrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param salt          A salt or label, if supported by the encryption
 *                      algorithm.
 * @param salt_length   Size of the salt buffer in bytes.
 * @param output        Buffer where the encrypted message is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the
 *                      returned output.
 */
psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);

/**
 * @brief Decrypt a short message with a private key.
 *
 * @param key           Identifier of the key to use for the operation.
 * @param alg           The asymmetric encryption algorithm to compute: a value
 *                      of type psa_algorithm_t such that
 *                      PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) is true.
 * @param input         The message to decrypt.
 * @param input_length  Size of the input buffer in bytes.
 * @param salt          A salt or label, if supported by the encryption
 *                      algorithm.
 * @param salt_length   Size of the salt buffer in bytes.
 * @param output        Buffer where the decrypted message is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes that make up the
 *                      returned output.
 */
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);

/**
 * @brief Whether the specified algorithm is an RSA OAEP encryption algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an RSA OAEP algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_RSA_OAEP(alg) /* specification-defined value */

/**
 * @brief Sufficient output buffer size for psa_asymmetric_encrypt().
 *
 * @param key_type An asymmetric key type, either a key pair or a public key.
 * @param key_bits The size of the key in bits.
 * @param alg      An asymmetric encryption algorithm: a value of type
 *                 psa_algorithm_t such that
 *                 PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) is true.
 *
 * @return A sufficient output buffer size for the specified asymmetric
 *         encryption algorithm and key parameters.
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_asymmetric_encrypt(), for any
 *        of the supported key types and asymmetric encryption algorithms.
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */

/**
 * @brief Sufficient output buffer size for psa_asymmetric_decrypt().
 *
 * @param key_type An asymmetric key type, either a key pair or a public key.
 * @param key_bits The size of the key in bits.
 * @param alg      An asymmetric encryption algorithm: a value of type
 *                 psa_algorithm_t such that
 *                 PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) is true.
 *
 * @return A sufficient output buffer size for the specified asymmetric
 *         encryption algorithm and key parameters.
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief A sufficient output buffer size for psa_asymmetric_decrypt(), for any
 *        of the supported key types and asymmetric encryption algorithms.
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */

/**
 * @brief The finite field Diffie-Hellman (DH) key-agreement algorithm.
 */
#define PSA_ALG_FFDH ((psa_algorithm_t)0x09010000)

/**
 * @brief The elliptic curve Diffie-Hellman (ECDH) key-agreement algorithm.
 */
#define PSA_ALG_ECDH ((psa_algorithm_t)0x09020000)

/**
 * @brief Macro to build a combined algorithm that chains a key agreement with a
 *        key derivation.
 *
 * @param ka_alg  A key-agreement algorithm: a value of type psa_algorithm_t
 *                such that PSA_ALG_IS_KEY_AGREEMENT(ka_alg) is true.
 * @param kdf_alg A key-derivation algorithm: a value of type psa_algorithm_t
 *                such that PSA_ALG_IS_KEY_DERIVATION(kdf_alg) is true.
 *
 * @return The corresponding key-agreement and key-derivation algorithm.
 */
#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg) \
    /* specification-defined value */

/**
 * @brief Perform a key agreement and return the shared secret as a derivation
 *        key.
 *
 * @param private_key     Identifier of the private key to use.
 * @param peer_key        Public key of the peer.
 * @param peer_key_length Size of peer_key in bytes.
 * @param alg             The standalone key-agreement algorithm to compute: a
 *                        value of type psa_algorithm_t such that
 *                        PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg) is true.
 * @param attributes      The attributes for the new key.
 * @param key             On success, an identifier for the newly created key.
 */
psa_status_t psa_key_agreement(psa_key_id_t private_key,
                               const uint8_t * peer_key,
                               size_t peer_key_length,
                               psa_algorithm_t alg,
                               const psa_key_attributes_t * attributes,
                               psa_key_id_t * key);

/**
 * @brief Perform a key agreement and return the shared secret.
 *
 * @param alg             The standalone key-agreement algorithm to compute: a
 *                        value of type psa_algorithm_t such that
 *                        PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg) is true.
 * @param private_key     Identifier of the private key to use.
 * @param peer_key        Public key of the peer.
 * @param peer_key_length Size of peer_key in bytes.
 * @param output          Buffer where the shared secret is to be written.
 * @param output_size     Size of the output buffer in bytes.
 * @param output_length   On success, the number of bytes that make up the
 *                        returned output.
 */
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t * peer_key,
                                   size_t peer_key_length,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_length);

/**
 * @brief Perform a key agreement and use the shared secret as input to a key
 *        derivation.
 *
 * @param operation       The key-derivation operation object to use.
 * @param step            Which step the input data is for.
 * @param private_key     Identifier of the private key to use.
 * @param peer_key        Public key of the peer.
 * @param peer_key_length Size of peer_key in bytes.
 */
psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t * peer_key,
                                              size_t peer_key_length);

/**
 * @brief Get the standalone key-agreement algorithm from a combined key-
 *        agreement and key-derivation algorithm.
 *
 * @param alg A key-agreement algorithm: a value of type psa_algorithm_t such
 *            that PSA_ALG_IS_KEY_AGREEMENT(alg) is true.
 *
 * @return The underlying standalone key-agreement algorithm if alg is a key-
 *         agreement algorithm.
 */
#define PSA_ALG_KEY_AGREEMENT_GET_BASE(alg) /* specification-defined value */

/**
 * @brief Get the key-derivation algorithm used in a combined key-agreement and
 *        key-derivation algorithm.
 *
 * @param alg A key-agreement algorithm: a value of type psa_algorithm_t such
 *            that PSA_ALG_IS_KEY_AGREEMENT(alg) is true.
 *
 * @return The underlying key-derivation algorithm if alg is a key-agreement
 *         algorithm.
 */
#define PSA_ALG_KEY_AGREEMENT_GET_KDF(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a standalone key-agreement
 *        algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a standalone key-agreement algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a standalone key-agreement
 *        algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 */
#define PSA_ALG_IS_RAW_KEY_AGREEMENT(alg) \
    PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)

/**
 * @brief Whether the specified algorithm is a finite field Diffie-Hellman
 *        algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a finite field Diffie-Hellman algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_FFDH(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is an elliptic curve Diffie-Hellman
 *        algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an elliptic curve Diffie-Hellman algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_ECDH(alg) /* specification-defined value */

/**
 * @brief Sufficient output buffer size for psa_raw_key_agreement().
 *
 * @param key_type A supported key type.
 * @param key_bits The size of the key in bits.
 *
 * @return A sufficient output buffer size for the specified key type and size.
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */

/**
 * @brief Sufficient output buffer size for psa_raw_key_agreement(), for any of
 *        the supported key types and key-agreement algorithms.
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */

/**
 * @brief The Elliptic Curve Integrated Encryption Scheme (ECIES).
 */
#define PSA_ALG_ECIES_SEC1 ((psa_algorithm_t)0x0c000100)

/**
 * @brief Module Lattice-based key-encapsulation mechanism (ML-KEM).
 */
#define PSA_ALG_ML_KEM ((psa_algorithm_t)0x0c000200)

/**
 * @brief Use a public key to generate a new shared secret key and associated
 *        ciphertext.
 *
 * @param key               Identifier of the key to use for the encapsulation.
 * @param alg               The key-encapsulation algorithm to use: a value of
 *                          type psa_algorithm_t such that
 *                          PSA_ALG_IS_KEY_ENCAPSULATION(alg) is true.
 * @param attributes        The attributes for the output key.
 * @param output_key        On success, an identifier for the newly created
 *                          shared secret key.
 * @param ciphertext        Buffer where the ciphertext output is to be written.
 * @param ciphertext_size   Size of the ciphertext buffer in bytes.
 * @param ciphertext_length On success, the number of bytes that make up the
 *                          ciphertext value.
 */
psa_status_t psa_encapsulate(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const psa_key_attributes_t * attributes,
                             psa_key_id_t * output_key,
                             uint8_t * ciphertext,
                             size_t ciphertext_size,
                             size_t * ciphertext_length);

/**
 * @brief Use a private key to decapsulate a shared secret key from a
 *        ciphertext.
 *
 * @param key               Identifier of the key to use for the decapsulation.
 * @param alg               The key-encapsulation algorithm to use: a value of
 *                          type psa_algorithm_t such that
 *                          PSA_ALG_IS_KEY_ENCAPSULATION(alg) is true.
 * @param ciphertext        The ciphertext received from the other participant.
 * @param ciphertext_length Size of the ciphertext buffer in bytes.
 * @param attributes        The attributes for the output key.
 * @param output_key        On success, an identifier for the newly created
 *                          shared secret key.
 */
psa_status_t psa_decapsulate(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * ciphertext,
                             size_t ciphertext_length,
                             const psa_key_attributes_t * attributes,
                             psa_key_id_t * output_key);

/**
 * @brief Sufficient ciphertext buffer size for psa_encapsulate(), in bytes.
 *
 * @param key_type A key type that is compatible with algorithm alg.
 * @param key_bits The size of the key in bits.
 * @param alg      A key-encapsulation algorithm: a value of type
 *                 psa_algorithm_t such that PSA_ALG_IS_KEY_ENCAPSULATION(alg)
 *                 is true.
 *
 * @return A sufficient ciphertext buffer size for the specified algorithm, key
 *         type, and size.
 */
#define PSA_ENCAPSULATE_CIPHERTEXT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */

/**
 * @brief Sufficient ciphertext buffer size for psa_encapsulate(), for any of
 *        the supported key types and key-encapsulation algorithms.
 */
#define PSA_ENCAPSULATE_CIPHERTEXT_MAX_SIZE /* implementation-defined value */

/**
 * @brief Encoding of the primitive associated with the PAKE.
 */
typedef uint32_t psa_pake_primitive_t;

/**
 * @brief Encoding of the type of the PAKE's primitive.
 */
typedef uint8_t psa_pake_primitive_type_t;

/**
 * @brief The PAKE primitive type indicating the use of elliptic curves.
 */
#define PSA_PAKE_PRIMITIVE_TYPE_ECC ((psa_pake_primitive_type_t)0x01)

/**
 * @brief The PAKE primitive type indicating the use of a finite field Diffie-
 *        Hellman group.
 */
#define PSA_PAKE_PRIMITIVE_TYPE_DH ((psa_pake_primitive_type_t)0x02)

/**
 * @brief Encoding of the family of the primitive associated with the PAKE.
 */
typedef uint8_t psa_pake_family_t;

/**
 * @brief Construct a PAKE primitive from type, family and bit-size.
 *
 * @param pake_type   The type of the primitive: a value of type
 *                    psa_pake_primitive_type_t.
 * @param pake_family The family of the primitive.
 * @param pake_bits   The bit-size of the primitive: a value of type size_t.
 *
 * @return The constructed primitive value.
 */
#define PSA_PAKE_PRIMITIVE(pake_type, pake_family, pake_bits) \
    /* specification-defined value */

/**
 * @brief Extract the PAKE primitive type from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive type, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_TYPE(pake_primitive) \
    /* specification-defined value */

/**
 * @brief Extract the family from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive family, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_FAMILY(pake_primitive) \
    /* specification-defined value */

/**
 * @brief Extract the bit-size from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive bit-size, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_BITS(pake_primitive) \
    /* specification-defined value */

/**
 * @brief The type of an object describing a PAKE cipher suite.
 */
typedef /* implementation-defined type */ psa_pake_cipher_suite_t;

/**
 * @brief This macro returns a suitable initializer for a PAKE cipher suite
 *        object of type psa_pake_cipher_suite_t.
 */
#define PSA_PAKE_CIPHER_SUITE_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a PAKE cipher suite object.
 */
psa_pake_cipher_suite_t psa_pake_cipher_suite_init(void);

/**
 * @brief Retrieve the PAKE algorithm from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return The PAKE algorithm stored in the cipher suite object.
 */
psa_algorithm_t psa_pake_cs_get_algorithm(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the PAKE algorithm for the cipher suite.
 *
 * @param cipher_suite The cipher suite object to write to.
 * @param alg          The PAKE algorithm to write: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_PAKE(alg) is true.
 */
void psa_pake_cs_set_algorithm(psa_pake_cipher_suite_t* cipher_suite,
                               psa_algorithm_t alg);

/**
 * @brief Retrieve the primitive from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return The primitive stored in the cipher suite object.
 */
psa_pake_primitive_t psa_pake_cs_get_primitive(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the primitive for a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to write to.
 * @param primitive    The PAKE primitive to write: a value of type
 *                     psa_pake_primitive_t.
 */
void psa_pake_cs_set_primitive(psa_pake_cipher_suite_t* cipher_suite,
                               psa_pake_primitive_t primitive);

/**
 * @brief A key confirmation value that indicates an confirmed key in a PAKE
 *        cipher suite.
 */
#define PSA_PAKE_CONFIRMED_KEY 0

/**
 * @brief A key confirmation value that indicates an unconfirmed key in a PAKE
 *        cipher suite.
 */
#define PSA_PAKE_UNCONFIRMED_KEY 1

/**
 * @brief Retrieve the key confirmation from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return A key confirmation value: either PSA_PAKE_CONFIRMED_KEY or
 *         PSA_PAKE_UNCONFIRMED_KEY.
 */
uint32_t psa_pake_cs_get_key_confirmation(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the key confirmation from a PAKE cipher suite.
 *
 * @param cipher_suite     The cipher suite object to write to.
 * @param key_confirmation The key confirmation value to write: either
 *                         PSA_PAKE_CONFIRMED_KEY or PSA_PAKE_UNCONFIRMED_KEY.
 */
void psa_pake_cs_set_key_confirmation(psa_pake_cipher_suite_t* cipher_suite,
                                      uint32_t key_confirmation);

/**
 * @brief Encoding of the application role in a PAKE algorithm.
 */
typedef uint8_t psa_pake_role_t;

/**
 * @brief A value to indicate no role in a PAKE algorithm.
 */
#define PSA_PAKE_ROLE_NONE ((psa_pake_role_t)0x00)

/**
 * @brief The first peer in a balanced PAKE.
 */
#define PSA_PAKE_ROLE_FIRST ((psa_pake_role_t)0x01)

/**
 * @brief The second peer in a balanced PAKE.
 */
#define PSA_PAKE_ROLE_SECOND ((psa_pake_role_t)0x02)

/**
 * @brief The client in an augmented PAKE.
 */
#define PSA_PAKE_ROLE_CLIENT ((psa_pake_role_t)0x11)

/**
 * @brief The server in an augmented PAKE.
 */
#define PSA_PAKE_ROLE_SERVER ((psa_pake_role_t)0x12)

/**
 * @brief Encoding of input and output steps for a PAKE algorithm.
 */
typedef uint8_t psa_pake_step_t;

/**
 * @brief A key share being sent to or received from a PAKE participant.
 */
#define PSA_PAKE_STEP_KEY_SHARE ((psa_pake_step_t)0x01)

/**
 * @brief A Schnorr NIZKP public key being sent to or received from a PAKE
 *        participant.
 */
#define PSA_PAKE_STEP_ZK_PUBLIC ((psa_pake_step_t)0x02)

/**
 * @brief A Schnorr NIZKP proof being sent to or received from a PAKE
 *        participant.
 */
#define PSA_PAKE_STEP_ZK_PROOF ((psa_pake_step_t)0x03)

/**
 * @brief A key confirmation value being sent to or received from a PAKE
 *        participant.
 */
#define PSA_PAKE_STEP_CONFIRM ((psa_pake_step_t)0x04)

/**
 * @brief A salt value used for deriving shared secrets within a PAKE operation.
 */
#define PSA_PAKE_STEP_SALT ((psa_pake_step_t)0x05)

/**
 * @brief A commitment value being sent to or received from a PAKE participant.
 */
#define PSA_PAKE_STEP_COMMIT ((psa_pake_step_t)0x06)

/**
 * @brief A counter used as part of key confirmation.
 */
#define PSA_PAKE_STEP_CONFIRM_COUNT ((psa_pake_step_t)0x07)

/**
 * @brief A key identifier value from a PAKE operation.
 */
#define PSA_PAKE_STEP_KEY_ID ((psa_pake_step_t)0x08)

/**
 * @brief The type of the state object for PAKE operations.
 */
typedef /* implementation-defined type */ psa_pake_operation_t;

/**
 * @brief This macro returns a suitable initializer for a PAKE operation object
 *        of type psa_pake_operation_t.
 */
#define PSA_PAKE_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a PAKE operation object.
 */
psa_pake_operation_t psa_pake_operation_init(void);

/**
 * @brief Setup a password-authenticated key exchange.
 *
 * @param operation    The operation object to set up.
 * @param password_key Identifier of the key holding the password or a value
 *                     derived from the password.
 * @param cipher_suite The cipher suite to use.
 */
psa_status_t psa_pake_setup(psa_pake_operation_t * operation,
                            psa_key_id_t password_key,
                            const psa_pake_cipher_suite_t * cipher_suite);

/**
 * @brief Set the application role for a password-authenticated key exchange.
 *
 * @param operation Active PAKE operation.
 * @param role      A value of type psa_pake_role_t indicating the application
 *                  role in the PAKE algorithm.
 */
psa_status_t psa_pake_set_role(psa_pake_operation_t * operation,
                               psa_pake_role_t role);

/**
 * @brief Set the user ID for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param user_id     The user ID to authenticate with.
 * @param user_id_len Size of the user_id buffer in bytes.
 */
psa_status_t psa_pake_set_user(psa_pake_operation_t * operation,
                               const uint8_t * user_id,
                               size_t user_id_len);

/**
 * @brief Set the peer ID for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param peer_id     The peer's ID to authenticate.
 * @param peer_id_len Size of the peer_id buffer in bytes.
 */
psa_status_t psa_pake_set_peer(psa_pake_operation_t * operation,
                               const uint8_t * peer_id,
                               size_t peer_id_len);

/**
 * @brief Set the context data for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param context     The peer's ID to authenticate.
 * @param context_len Size of the context buffer in bytes.
 */
psa_status_t psa_pake_set_context(psa_pake_operation_t * operation,
                                  const uint8_t * context,
                                  size_t context_len);

/**
 * @brief Get output for a step of a password-authenticated key exchange.
 *
 * @param operation     Active PAKE operation.
 * @param step          The step of the algorithm for which the output is
 *                      requested.
 * @param output        Buffer where the output is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes of the returned output.
 */
psa_status_t psa_pake_output(psa_pake_operation_t * operation,
                             psa_pake_step_t step,
                             uint8_t * output,
                             size_t output_size,
                             size_t * output_length);

/**
 * @brief Provide input for a step of a password-authenticated key exchange.
 *
 * @param operation    Active PAKE operation.
 * @param step         The step for which the input is provided.
 * @param input        Buffer containing the input.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_pake_input(psa_pake_operation_t * operation,
                            psa_pake_step_t step,
                            const uint8_t * input,
                            size_t input_length);

/**
 * @brief Extract the shared secret from the PAKE as a key.
 *
 * @param operation  Active PAKE operation.
 * @param attributes The attributes for the new key.
 * @param key        On success, an identifier for the newly created key.
 */
psa_status_t psa_pake_get_shared_key(psa_pake_operation_t * operation,
                                     const psa_key_attributes_t * attributes,
                                     psa_key_id_t * key);

/**
 * @brief Abort a PAKE operation.
 *
 * @param operation Initialized PAKE operation.
 */
psa_status_t psa_pake_abort(psa_pake_operation_t * operation);

/**
 * @brief Sufficient output buffer size for psa_pake_output(), in bytes.
 *
 * @param alg         A PAKE algorithm: a value of type psa_algorithm_t such
 *                    that PSA_ALG_IS_PAKE(alg) is true.
 * @param primitive   A primitive of type psa_pake_primitive_t that is
 *                    compatible with algorithm alg.
 * @param output_step A value of type psa_pake_step_t that is valid for the
 *                    algorithm alg.
 *
 * @return A sufficient output buffer size for the specified PAKE algorithm,
 *         primitive, and output step.
 */
#define PSA_PAKE_OUTPUT_SIZE(alg, primitive, output_step) \
    /* implementation-defined value */

/**
 * @brief Sufficient output buffer size for psa_pake_output() for any of the
 *        supported PAKE algorithms, primitives and output steps.
 */
#define PSA_PAKE_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief Sufficient buffer size for inputs to psa_pake_input().
 *
 * @param alg        A PAKE algorithm: a value of type psa_algorithm_t such that
 *                   PSA_ALG_IS_PAKE(alg) is true.
 * @param primitive  A primitive of type psa_pake_primitive_t that is compatible
 *                   with algorithm alg.
 * @param input_step A value of type psa_pake_step_t that is valid for the
 *                   algorithm alg.
 *
 * @return A sufficient buffer size for the specified PAKE algorithm, primitive,
 *         and input step.
 */
#define PSA_PAKE_INPUT_SIZE(alg, primitive, input_step) \
    /* implementation-defined value */

/**
 * @brief Sufficient buffer size for inputs to psa_pake_input() for any of the
 *        supported PAKE algorithms, primitives and input steps.
 */
#define PSA_PAKE_INPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief Macro to build the Password-authenticated key exchange by juggling
 *        (J-PAKE) algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A J-PAKE algorithm, parameterized by a specific hash.
 */
#define PSA_ALG_JPAKE(hash_alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a J-PAKE algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a J-PAKE algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_JPAKE(alg) /* specification-defined value */

/**
 * @brief Macro to build the SPAKE2+ algorithm, using HMAC-based key
 *        confirmation.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A SPAKE2+ algorithm, using HMAC for key confirmation, parameterized
 *         by a specific hash.
 */
#define PSA_ALG_SPAKE2P_HMAC(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build the SPAKE2+ algorithm, using CMAC-based key
 *        confirmation.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A SPAKE2+ algorithm, using CMAC for key confirmation, parameterized
 *         by a specific hash.
 */
#define PSA_ALG_SPAKE2P_CMAC(hash_alg) /* specification-defined value */

/**
 * @brief The SPAKE2+ algorithm, as used by the Matter v1 specification.
 */
#define PSA_ALG_SPAKE2P_MATTER ((psa_algorithm_t)0x0A000609)

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm that uses a
 *        HMAC-based key confirmation.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm that uses a HMAC-based key
 *         confirmation, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P_HMAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm that uses a
 *        CMAC-based key confirmation.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm that uses a CMAC-based key
 *         confirmation, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P_CMAC(alg) /* specification-defined value */

/**
 * @brief Macro to build the WPA3-SAE algorithm, with fixed-sized PMK output
 *        key.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A WPA3-SAE algorithm, for the Looping or Hash-to-element variants,
 *         parameterized by a specific hash.
 */
#define PSA_ALG_WPA3_SAE_FIXED(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build the WPA3-SAE algorithm, with group-dependent size of
 *        the PMK output key.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A WPA3-SAE algorithm, for the group-dependent-hash variant,
 *         parameterized by a specific hash.
 */
#define PSA_ALG_WPA3_SAE_GDH(hash_alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a WPA3-SAE algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a WPA3-SAE algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_WPA3_SAE(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a WPA3-SAE algorithm with a fixed-
 *        sized output key.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a WPA3-SAE algorithm with a fixed-sized output key, 0
 *         otherwise.
 */
#define PSA_ALG_IS_WPA3_SAE_FIXED(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a WPA3-SAE algorithm with a group-
 *        dependent size for the output key.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a WPA3-SAE algorithm with a group-dependent size for the
 *         output key, 0 otherwise.
 */
#define PSA_ALG_IS_WPA3_SAE_GDH(alg) /* specification-defined value */

/**
 * @brief A wildcard algorithm for WPA3-SAE password keys and password token
 *        keys.
 */
#define PSA_ALG_WPA3_SAE_ANY ((psa_algorithm_t)0x0a0088ff)

/**
 * @brief Generate random bytes.
 *
 * @param output      Output buffer for the generated data.
 * @param output_size Number of bytes to generate and output.
 */
psa_status_t psa_generate_random(uint8_t * output,
                                 size_t output_size);

#ifdef __cplusplus
}
#endif

#endif // PSA_CRYPTO_H
