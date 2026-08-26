// SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
// SPDX-License-Identifier: Apache-2.0

typedef /* implementation-defined type */ psa_aead_operation_t;
typedef uint32_t psa_algorithm_t;
typedef /* implementation-defined type */ psa_cipher_operation_t;
typedef uint32_t psa_crypto_subsystem_t;
typedef uint8_t psa_dh_family_t;
typedef uint8_t psa_ecc_family_t;
typedef /* implementation-defined type */ psa_hash_operation_t;
typedef /* implementation-defined type */ psa_key_attributes_t;
typedef /* implementation-defined type */ psa_key_derivation_operation_t;
typedef uint16_t psa_key_derivation_step_t;
typedef uint32_t psa_key_id_t;
typedef uint32_t psa_key_lifetime_t;
typedef uint32_t psa_key_location_t;
typedef uint8_t psa_key_persistence_t;
typedef uint16_t psa_key_type_t;
typedef uint32_t psa_key_usage_t;
typedef /* implementation-defined type */ psa_mac_operation_t;
typedef /* implementation-defined type */ psa_pake_cipher_suite_t;
typedef uint8_t psa_pake_family_t;
typedef /* implementation-defined type */ psa_pake_operation_t;
typedef uint32_t psa_pake_primitive_t;
typedef uint8_t psa_pake_primitive_type_t;
typedef uint8_t psa_pake_role_t;
typedef uint8_t psa_pake_step_t;
typedef /* implementation-defined type */ psa_sign_operation_t;
typedef uint8_t psa_slh_dsa_family_t;
typedef /* implementation-defined type */ psa_verify_operation_t;
typedef /* implementation-defined type */ psa_xof_operation_t;
typedef struct psa_custom_key_parameters_t {
    uint32_t flags;
} psa_custom_key_parameters_t;
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length) \
    /* implementation-defined value */
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
    /* implementation-defined value */
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length) \
    /* implementation-defined value */
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
    /* implementation-defined value */
#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) /* implementation-defined value */
#define PSA_AEAD_NONCE_MAX_SIZE /* implementation-defined value */
#define PSA_AEAD_OPERATION_INIT /* implementation-defined value */
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_AEAD_TAG_MAX_SIZE /* implementation-defined value */
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */
#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */
#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE /* implementation-defined value */
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    /* specification-defined value */
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
    /* specification-defined value */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
    /* specification-defined value */
#define PSA_ALG_AES_MMO_ZIGBEE ((psa_algorithm_t)0x02000007)
#define PSA_ALG_ANY_HASH ((psa_algorithm_t)0x020000ff)
#define PSA_ALG_ASCON_AEAD128 ((psa_algorithm_t)0x05100700)
#define PSA_ALG_ASCON_CXOF128 ((psa_algorithm_t)0x0D008300)
#define PSA_ALG_ASCON_HASH256 ((psa_algorithm_t)0x02000019)
#define PSA_ALG_ASCON_XOF128 ((psa_algorithm_t)0x0D000300)
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length) \
    /* specification-defined value */
#define PSA_ALG_BLAKE2BP_HASH512 ((psa_algorithm_t)0x0200001F)
#define PSA_ALG_BLAKE2B_HASH512 ((psa_algorithm_t)0x0200001E)
#define PSA_ALG_BLAKE2SP_HASH256 ((psa_algorithm_t)0x0200001D)
#define PSA_ALG_BLAKE2S_HASH256 ((psa_algorithm_t)0x0200001C)
#define PSA_ALG_BLAKE2_MAC(hash_alg) /* specification-defined value */
#define PSA_ALG_CBC_MAC ((psa_algorithm_t)0x03c00100)
#define PSA_ALG_CBC_NO_PADDING ((psa_algorithm_t)0x04404000)
#define PSA_ALG_CBC_PKCS7 ((psa_algorithm_t)0x04404100)
#define PSA_ALG_CCM ((psa_algorithm_t)0x05500100)
#define PSA_ALG_CCM_STAR_ANY_TAG ((psa_algorithm_t)0x04c09300)
#define PSA_ALG_CCM_STAR_NO_TAG ((psa_algorithm_t)0x04c01300)
#define PSA_ALG_CFB ((psa_algorithm_t)0x04c01100)
#define PSA_ALG_CHACHA20_POLY1305 ((psa_algorithm_t)0x05100500)
#define PSA_ALG_CMAC ((psa_algorithm_t)0x03c00200)
#define PSA_ALG_CTR ((psa_algorithm_t)0x04c01000)
#define PSA_ALG_DETERMINISTIC_ECDSA(hash_alg) /* specification-defined value */
#define PSA_ALG_DETERMINISTIC_HASH_ML_DSA(hash_alg) \
    /* specification-defined value */
#define PSA_ALG_DETERMINISTIC_HASH_SLH_DSA(hash_alg) \
    /* specification-defined value */
#define PSA_ALG_DETERMINISTIC_ML_DSA ((psa_algorithm_t) 0x06004500)
#define PSA_ALG_DETERMINISTIC_SLH_DSA ((psa_algorithm_t) 0x06004100)
#define PSA_ALG_ECB_NO_PADDING ((psa_algorithm_t)0x04404400)
#define PSA_ALG_ECDH ((psa_algorithm_t)0x09020000)
#define PSA_ALG_ECDSA(hash_alg) /* specification-defined value */
#define PSA_ALG_ECDSA_ANY ((psa_algorithm_t) 0x06000600)
#define PSA_ALG_ECIES_SEC1 ((psa_algorithm_t)0x0c000100)
#define PSA_ALG_ED25519PH ((psa_algorithm_t) 0x0600090B)
#define PSA_ALG_ED448PH ((psa_algorithm_t) 0x06000915)
#define PSA_ALG_EDDSA_CTX ((psa_algorithm_t) 0x06000A00)
#define PSA_ALG_FFDH ((psa_algorithm_t)0x09010000)
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg) /* specification-defined value */
#define PSA_ALG_GCM ((psa_algorithm_t)0x05500200)
#define PSA_ALG_GET_HASH(alg) /* specification-defined value */
#define PSA_ALG_HASH_ML_DSA(hash_alg) /* specification-defined value */
#define PSA_ALG_HASH_SLH_DSA(hash_alg) /* specification-defined value */
#define PSA_ALG_HKDF(hash_alg) /* specification-defined value */
#define PSA_ALG_HKDF_EXPAND(hash_alg) /* specification-defined value */
#define PSA_ALG_HKDF_EXTRACT(hash_alg) /* specification-defined value */
#define PSA_ALG_HMAC(hash_alg) /* specification-defined value */
#define PSA_ALG_HSS ((psa_algorithm_t) 0x06004900)
#define PSA_ALG_IS_AEAD(alg) /* specification-defined value */
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) /* specification-defined value */
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) /* specification-defined value */
#define PSA_ALG_IS_BLAKE2_HASH(alg) /* specification-defined value */
#define PSA_ALG_IS_BLAKE2_MAC(alg) /* specification-defined value */
#define PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) /* specification-defined value */
#define PSA_ALG_IS_CIPHER(alg) /* specification-defined value */
#define PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) /* specification-defined value */
#define PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA(alg) \
    /* specification-defined value */
#define PSA_ALG_IS_DETERMINISTIC_HASH_SLH_DSA(alg) \
    /* specification-defined value */
#define PSA_ALG_IS_ECDH(alg) /* specification-defined value */
#define PSA_ALG_IS_ECDSA(alg) /* specification-defined value */
#define PSA_ALG_IS_FFDH(alg) /* specification-defined value */
#define PSA_ALG_IS_HASH(alg) /* specification-defined value */
#define PSA_ALG_IS_HASH_AND_SIGN(alg) /* specification-defined value */
#define PSA_ALG_IS_HASH_EDDSA(alg) /* specification-defined value */
#define PSA_ALG_IS_HASH_ML_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_HASH_SLH_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_HEDGED_HASH_ML_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_HEDGED_HASH_SLH_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_HKDF(alg) /* specification-defined value */
#define PSA_ALG_IS_HKDF_EXPAND(alg) /* specification-defined value */
#define PSA_ALG_IS_HKDF_EXTRACT(alg) /* specification-defined value */
#define PSA_ALG_IS_HMAC(alg) /* specification-defined value */
#define PSA_ALG_IS_JPAKE(alg) /* specification-defined value */
#define PSA_ALG_IS_KEY_AGREEMENT(alg) /* specification-defined value */
#define PSA_ALG_IS_KEY_DERIVATION(alg) /* specification-defined value */
#define PSA_ALG_IS_KEY_DERIVATION_STRETCHING(alg) \
    /* specification-defined value */
#define PSA_ALG_IS_KEY_ENCAPSULATION(alg) /* specification-defined value */
#define PSA_ALG_IS_KEY_WRAP(alg) /* specification-defined value */
#define PSA_ALG_IS_MAC(alg) /* specification-defined value */
#define PSA_ALG_IS_ML_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_PAKE(alg) /* specification-defined value */
#define PSA_ALG_IS_PBKDF2_HMAC(alg) /* specification-defined value */
#define PSA_ALG_IS_RANDOMIZED_ECDSA(alg) /* specification-defined value */
#define PSA_ALG_IS_RAW_KEY_AGREEMENT(alg) \
    PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)
#define PSA_ALG_IS_RSA_OAEP(alg) /* specification-defined value */
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) /* specification-defined value */
#define PSA_ALG_IS_RSA_PSS(alg) /* specification-defined value */
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg) /* specification-defined value */
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) /* specification-defined value */
#define PSA_ALG_IS_SIGN(alg) /* specification-defined value */
#define PSA_ALG_IS_SIGN_HASH(alg) /* specification-defined value */
#define PSA_ALG_IS_SIGN_MESSAGE(alg) /* specification-defined value */
#define PSA_ALG_IS_SLH_DSA(alg) /* specification-defined value */
#define PSA_ALG_IS_SP800_108_COUNTER_HMAC(alg) \
    /* specification-defined value */
#define PSA_ALG_IS_SPAKE2P(alg) /* specification-defined value */
#define PSA_ALG_IS_SPAKE2P_CMAC(alg) /* specification-defined value */
#define PSA_ALG_IS_SPAKE2P_HMAC(alg) /* specification-defined value */
#define PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg) \
    /* specification-defined value */
#define PSA_ALG_IS_STREAM_CIPHER(alg) /* specification-defined value */
#define PSA_ALG_IS_TLS12_PRF(alg) /* specification-defined value */
#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) /* specification-defined value */
#define PSA_ALG_IS_WILDCARD(alg) /* specification-defined value */
#define PSA_ALG_IS_WPA3_SAE(alg) /* specification-defined value */
#define PSA_ALG_IS_WPA3_SAE_FIXED(alg) /* specification-defined value */
#define PSA_ALG_IS_WPA3_SAE_GDH(alg) /* specification-defined value */
#define PSA_ALG_IS_WPA3_SAE_H2E(alg) /* specification-defined value */
#define PSA_ALG_IS_XOF(alg) /* specification-defined value */
#define PSA_ALG_JPAKE(hash_alg) /* specification-defined value */
#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg) \
    /* specification-defined value */
#define PSA_ALG_KEY_AGREEMENT_GET_BASE(alg) /* specification-defined value */
#define PSA_ALG_KEY_AGREEMENT_GET_KDF(alg) /* specification-defined value */
#define PSA_ALG_KW ((psa_algorithm_t)0x0B400100)
#define PSA_ALG_KWP ((psa_algorithm_t)0x0BC00200)
#define PSA_ALG_LMS ((psa_algorithm_t) 0x06004800)
#define PSA_ALG_MD2 ((psa_algorithm_t)0x02000001)
#define PSA_ALG_MD4 ((psa_algorithm_t)0x02000002)
#define PSA_ALG_MD5 ((psa_algorithm_t)0x02000003)
#define PSA_ALG_ML_DSA ((psa_algorithm_t) 0x06004400)
#define PSA_ALG_ML_KEM ((psa_algorithm_t)0x0c000200)
#define PSA_ALG_NONE ((psa_algorithm_t)0)
#define PSA_ALG_OFB ((psa_algorithm_t)0x04c01200)
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)
#define PSA_ALG_PBKDF2_HMAC(hash_alg) /* specification-defined value */
#define PSA_ALG_PURE_EDDSA ((psa_algorithm_t) 0x06000800)
#define PSA_ALG_RIPEMD160 ((psa_algorithm_t)0x02000004)
#define PSA_ALG_RSA_OAEP(hash_alg) /* specification-defined value */
#define PSA_ALG_RSA_PKCS1V15_CRYPT ((psa_algorithm_t)0x07000200)
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg) /* specification-defined value */
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW ((psa_algorithm_t) 0x06000200)
#define PSA_ALG_RSA_PSS(hash_alg) /* specification-defined value */
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg) /* specification-defined value */
#define PSA_ALG_SHA3_224 ((psa_algorithm_t)0x02000010)
#define PSA_ALG_SHA3_256 ((psa_algorithm_t)0x02000011)
#define PSA_ALG_SHA3_384 ((psa_algorithm_t)0x02000012)
#define PSA_ALG_SHA3_512 ((psa_algorithm_t)0x02000013)
#define PSA_ALG_SHAKE128 ((psa_algorithm_t)0x0D000100)
#define PSA_ALG_SHAKE128_256 ((psa_algorithm_t)0x02000016)
#define PSA_ALG_SHAKE256 ((psa_algorithm_t)0x0D000200)
#define PSA_ALG_SHAKE256_192 ((psa_algorithm_t)0x02000017)
#define PSA_ALG_SHAKE256_256 ((psa_algorithm_t)0x02000018)
#define PSA_ALG_SHAKE256_512 ((psa_algorithm_t)0x02000015)
#define PSA_ALG_SHA_1 ((psa_algorithm_t)0x02000005)
#define PSA_ALG_SHA_224 ((psa_algorithm_t)0x02000008)
#define PSA_ALG_SHA_256 ((psa_algorithm_t)0x02000009)
#define PSA_ALG_SHA_256_192 ((psa_algorithm_t)0x0200000E)
#define PSA_ALG_SHA_384 ((psa_algorithm_t)0x0200000a)
#define PSA_ALG_SHA_512 ((psa_algorithm_t)0x0200000b)
#define PSA_ALG_SHA_512_224 ((psa_algorithm_t)0x0200000c)
#define PSA_ALG_SHA_512_256 ((psa_algorithm_t)0x0200000d)
#define PSA_ALG_SIGN_SUPPORTS_CONTEXT(alg) /* implementation-defined value */
#define PSA_ALG_SLH_DSA ((psa_algorithm_t) 0x06004000)
#define PSA_ALG_SM3 ((psa_algorithm_t)0x02000014)
#define PSA_ALG_SP800_108_COUNTER_CMAC ((psa_algorithm_t)0x08000800)
#define PSA_ALG_SP800_108_COUNTER_HMAC(hash_alg) \
    /* specification-defined value */
#define PSA_ALG_SPAKE2P_CMAC(hash_alg) /* specification-defined value */
#define PSA_ALG_SPAKE2P_HMAC(hash_alg) /* specification-defined value */
#define PSA_ALG_SPAKE2P_MATTER ((psa_algorithm_t)0x0A000609)
#define PSA_ALG_STREAM_CIPHER ((psa_algorithm_t)0x04800100)
#define PSA_ALG_TLS12_ECJPAKE_TO_PMS ((psa_algorithm_t)0x08000609)
#define PSA_ALG_TLS12_PRF(hash_alg) /* specification-defined value */
#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) /* specification-defined value */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length) \
    /* specification-defined value */
#define PSA_ALG_WPA3_SAE_ANY ((psa_algorithm_t)0x0a0088ff)
#define PSA_ALG_WPA3_SAE_FIXED(hash_alg) /* specification-defined value */
#define PSA_ALG_WPA3_SAE_GDH(hash_alg) /* specification-defined value */
#define PSA_ALG_WPA3_SAE_H2E(hash_alg) /* specification-defined value */
#define PSA_ALG_XCHACHA20_POLY1305 ((psa_algorithm_t)0x05100600)
#define PSA_ALG_XMSS ((psa_algorithm_t) 0x06004A00)
#define PSA_ALG_XMSS_MT ((psa_algorithm_t) 0x06004B00)
#define PSA_ALG_XOF_HAS_CONTEXT(alg) /* specification-defined value */
#define PSA_ALG_XTS ((psa_algorithm_t)0x0440ff00)
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_BLOCK_CIPHER_BLOCK_LENGTH(type) /* specification-defined value */
#define PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE /* implementation-defined value */
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */
#define PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */
#define PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */
#define PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */
#define PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */
#define PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg) \
    /* implementation-defined value */
#define PSA_CIPHER_IV_LENGTH(key_type, alg) /* implementation-defined value */
#define PSA_CIPHER_IV_MAX_SIZE /* implementation-defined value */
#define PSA_CIPHER_OPERATION_INIT /* implementation-defined value */
#define PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length) \
    /* implementation-defined value */
#define PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
    /* implementation-defined value */
#define PSA_CRYPTO_ALL_SUBSYSTEMS /* implementation-defined value */
#define PSA_CRYPTO_API_VERSION_MAJOR 1
#define PSA_CRYPTO_API_VERSION_MINOR 5
#define PSA_CRYPTO_SUBSYSTEM_ACCELERATORS /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_BUILTIN_KEYS /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_COMMUNICATION /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_KEYS /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_RANDOM /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS /* implementation-defined value */
#define PSA_CRYPTO_SUBSYSTEM_STORAGE /* implementation-defined value */
#define PSA_CUSTOM_KEY_PARAMETERS_INIT { 0 }
#define PSA_DH_FAMILY_RFC3526 ((psa_dh_family_t) 0x05)
#define PSA_DH_FAMILY_RFC7919 ((psa_dh_family_t) 0x03)
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 ((psa_ecc_family_t) 0x30)
#define PSA_ECC_FAMILY_FRP ((psa_ecc_family_t) 0x33)
#define PSA_ECC_FAMILY_MONTGOMERY ((psa_ecc_family_t) 0x41)
#define PSA_ECC_FAMILY_SECP_K1 ((psa_ecc_family_t) 0x17)
#define PSA_ECC_FAMILY_SECP_R1 ((psa_ecc_family_t) 0x12)
#define PSA_ECC_FAMILY_SECP_R2 ((psa_ecc_family_t) 0x1b)
#define PSA_ECC_FAMILY_SECT_K1 ((psa_ecc_family_t) 0x27)
#define PSA_ECC_FAMILY_SECT_R1 ((psa_ecc_family_t) 0x22)
#define PSA_ECC_FAMILY_SECT_R2 ((psa_ecc_family_t) 0x2b)
#define PSA_ECC_FAMILY_TWISTED_EDWARDS ((psa_ecc_family_t) 0x42)
#define PSA_ENCAPSULATE_CIPHERTEXT_MAX_SIZE /* implementation-defined value */
#define PSA_ENCAPSULATE_CIPHERTEXT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_ERROR_INSUFFICIENT_ENTROPY ((psa_status_t)-148)
#define PSA_ERROR_INVALID_PADDING ((psa_status_t)-150)
#define PSA_EXPORT_ASYMMETRIC_KEY_MAX_SIZE /* implementation-defined value */
#define PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE /* implementation-defined value */
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE /* implementation-defined value */
#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */
#define PSA_HASH_BLOCK_LENGTH(alg) /* implementation-defined value */
#define PSA_HASH_LENGTH(alg) /* implementation-defined value */
#define PSA_HASH_MAX_SIZE /* implementation-defined value */
#define PSA_HASH_OPERATION_INIT /* implementation-defined value */
#define PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH ((size_t)4)
#define PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) \
    /* specification-defined value */
#define PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) \
    /* specification-defined value */
#define PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE /* implementation-defined value */
#define PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) /* specification-defined value */
#define PSA_KEY_ATTRIBUTES_INIT /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_CONTEXT /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_COST /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_INFO /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_LABEL /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_OTHER_SECRET \
    /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_SALT /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_SECRET /* implementation-defined value */
#define PSA_KEY_DERIVATION_INPUT_SEED /* implementation-defined value */
#define PSA_KEY_DERIVATION_OPERATION_INIT /* implementation-defined value */
#define PSA_KEY_DERIVATION_UNLIMITED_CAPACITY \
    /* implementation-defined value */
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t) ((lifetime) >> 8))
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t) ((lifetime) & 0x000000ff))
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)
#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t) 0x00000001)
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t) 0x00000000)
#define PSA_KEY_LOCATION_LOCAL_STORAGE ((psa_key_location_t) 0x000000)
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t) 0x000001)
#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t) 0x01)
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t) 0xff)
#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t) 0x00)
#define PSA_KEY_TYPE_AES ((psa_key_type_t)0x2400)
#define PSA_KEY_TYPE_ARC4 ((psa_key_type_t)0x2002)
#define PSA_KEY_TYPE_ARIA ((psa_key_type_t)0x2406)
#define PSA_KEY_TYPE_ASCON ((psa_key_type_t)0x2008)
#define PSA_KEY_TYPE_BLAKE2 ((psa_key_type_t)0x1103)
#define PSA_KEY_TYPE_CAMELLIA ((psa_key_type_t)0x2403)
#define PSA_KEY_TYPE_CHACHA20 ((psa_key_type_t)0x2004)
#define PSA_KEY_TYPE_DERIVE ((psa_key_type_t)0x1200)
#define PSA_KEY_TYPE_DES ((psa_key_type_t)0x2301)
#define PSA_KEY_TYPE_DH_GET_FAMILY(type) /* specification-defined value */
#define PSA_KEY_TYPE_DH_KEY_PAIR(group) /* specification-defined value */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group) /* specification-defined value */
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) /* specification-defined value */
#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve) /* specification-defined value */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve) /* specification-defined value */
#define PSA_KEY_TYPE_HMAC ((psa_key_type_t)0x1100)
#define PSA_KEY_TYPE_HSS_PUBLIC_KEY ((psa_key_type_t)0x4008)
#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_DH(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_ECC(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_KEY_PAIR(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_ML_DSA(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_ML_KEM(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_RSA(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_SLH_DSA(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_SLH_DSA_KEY_PAIR(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_IS_SLH_DSA_PUBLIC_KEY(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_IS_SPAKE2P(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_SPAKE2P_KEY_PAIR(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_IS_SPAKE2P_PUBLIC_KEY(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_WPA3_SAE_DH(type) /* specification-defined value */
#define PSA_KEY_TYPE_IS_WPA3_SAE_ECC(type) /* specification-defined value */
#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_LMS_PUBLIC_KEY ((psa_key_type_t)0x4007)
#define PSA_KEY_TYPE_ML_DSA_KEY_PAIR ((psa_key_type_t)0x7002)
#define PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY ((psa_key_type_t)0x4002)
#define PSA_KEY_TYPE_ML_KEM_KEY_PAIR ((psa_key_type_t)0x7004)
#define PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY ((psa_key_type_t)0x4004)
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)
#define PSA_KEY_TYPE_PASSWORD ((psa_key_type_t)0x1203)
#define PSA_KEY_TYPE_PASSWORD_HASH ((psa_key_type_t)0x1205)
#define PSA_KEY_TYPE_PEPPER ((psa_key_type_t)0x1206)
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_RAW_DATA ((psa_key_type_t)0x1001)
#define PSA_KEY_TYPE_RSA_KEY_PAIR ((psa_key_type_t)0x7001)
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY ((psa_key_type_t)0x4001)
#define PSA_KEY_TYPE_SLH_DSA_GET_FAMILY(type) /* specification-defined value */
#define PSA_KEY_TYPE_SLH_DSA_KEY_PAIR(set) /* specification-defined value */
#define PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY(set) /* specification-defined value */
#define PSA_KEY_TYPE_SM4 ((psa_key_type_t)0x2405)
#define PSA_KEY_TYPE_SPAKE2P_GET_FAMILY(type) /* specification-defined value */
#define PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(curve) /* specification-defined value */
#define PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(curve) \
    /* specification-defined value */
#define PSA_KEY_TYPE_WPA3_SAE_DH(group) /* specification-defined value */
#define PSA_KEY_TYPE_WPA3_SAE_DH_GET_FAMILY(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_WPA3_SAE_ECC(curve) /* specification-defined value */
#define PSA_KEY_TYPE_WPA3_SAE_ECC_GET_FAMILY(type) \
    /* specification-defined value */
#define PSA_KEY_TYPE_XCHACHA20 ((psa_key_type_t)0x2007)
#define PSA_KEY_TYPE_XMSS_MT_PUBLIC_KEY ((psa_key_type_t)0x400D)
#define PSA_KEY_TYPE_XMSS_PUBLIC_KEY ((psa_key_type_t)0x400B)
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)
#define PSA_KEY_USAGE_DERIVE_PUBLIC ((psa_key_usage_t)0x00000080)
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)
#define PSA_KEY_USAGE_UNWRAP ((psa_key_usage_t)0x00020000)
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)
#define PSA_KEY_USAGE_WRAP ((psa_key_usage_t)0x00010000)
#define PSA_MAC_LENGTH(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_MAC_MAX_SIZE /* implementation-defined value */
#define PSA_MAC_OPERATION_INIT /* implementation-defined value */
#define PSA_PAKE_CIPHER_SUITE_INIT /* implementation-defined value */
#define PSA_PAKE_CONFIRMED_KEY 0
#define PSA_PAKE_INPUT_MAX_SIZE /* implementation-defined value */
#define PSA_PAKE_INPUT_SIZE(alg, primitive, input_step) \
    /* implementation-defined value */
#define PSA_PAKE_OPERATION_INIT /* implementation-defined value */
#define PSA_PAKE_OUTPUT_MAX_SIZE /* implementation-defined value */
#define PSA_PAKE_OUTPUT_SIZE(alg, primitive, output_step) \
    /* implementation-defined value */
#define PSA_PAKE_PRIMITIVE(pake_type, pake_family, pake_bits) \
    /* specification-defined value */
#define PSA_PAKE_PRIMITIVE_GET_BITS(pake_primitive) \
    /* specification-defined value */
#define PSA_PAKE_PRIMITIVE_GET_FAMILY(pake_primitive) \
    /* specification-defined value */
#define PSA_PAKE_PRIMITIVE_GET_TYPE(pake_primitive) \
    /* specification-defined value */
#define PSA_PAKE_PRIMITIVE_TYPE_DH ((psa_pake_primitive_type_t)0x02)
#define PSA_PAKE_PRIMITIVE_TYPE_ECC ((psa_pake_primitive_type_t)0x01)
#define PSA_PAKE_ROLE_CLIENT ((psa_pake_role_t)0x11)
#define PSA_PAKE_ROLE_FIRST ((psa_pake_role_t)0x01)
#define PSA_PAKE_ROLE_NONE ((psa_pake_role_t)0x00)
#define PSA_PAKE_ROLE_SECOND ((psa_pake_role_t)0x02)
#define PSA_PAKE_ROLE_SERVER ((psa_pake_role_t)0x12)
#define PSA_PAKE_STEP_COMMIT ((psa_pake_step_t)0x06)
#define PSA_PAKE_STEP_CONFIRM ((psa_pake_step_t)0x04)
#define PSA_PAKE_STEP_CONFIRM_COUNT ((psa_pake_step_t)0x07)
#define PSA_PAKE_STEP_KEY_ID ((psa_pake_step_t)0x08)
#define PSA_PAKE_STEP_KEY_SHARE ((psa_pake_step_t)0x01)
#define PSA_PAKE_STEP_SALT ((psa_pake_step_t)0x05)
#define PSA_PAKE_STEP_ZK_PROOF ((psa_pake_step_t)0x03)
#define PSA_PAKE_STEP_ZK_PUBLIC ((psa_pake_step_t)0x02)
#define PSA_PAKE_UNCONFIRMED_KEY 1
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE \
    /* implementation-defined value */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */
#define PSA_SIGN_OPERATION_INIT /* implementation-defined value */
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg) \
    /* implementation-defined value */
#define PSA_SLH_DSA_FAMILY_SHA2_F ((psa_slh_dsa_family_t) 0x04)
#define PSA_SLH_DSA_FAMILY_SHA2_S ((psa_slh_dsa_family_t) 0x02)
#define PSA_SLH_DSA_FAMILY_SHAKE_F ((psa_slh_dsa_family_t) 0x0d)
#define PSA_SLH_DSA_FAMILY_SHAKE_S ((psa_slh_dsa_family_t) 0x0b)
#define PSA_TLS12_ECJPAKE_TO_PMS_OUTPUT_SIZE 32
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE /* implementation-defined value */
#define PSA_VERIFY_OPERATION_INIT /* implementation-defined value */
#define PSA_WRAP_KEY_OUTPUT_SIZE(wrap_key_type, alg, key_type, key_bits) \
    /* implementation-defined value */
#define PSA_WRAP_KEY_PAIR_MAX_SIZE /* implementation-defined value */
#define PSA_XOF_OPERATION_INIT /* implementation-defined value */
psa_status_t psa_aead_abort(psa_aead_operation_t * operation);
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
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);
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
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);
psa_status_t psa_aead_finish(psa_aead_operation_t * operation,
                             uint8_t * ciphertext,
                             size_t ciphertext_size,
                             size_t * ciphertext_length,
                             uint8_t * tag,
                             size_t tag_size,
                             size_t * tag_length);
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t * operation,
                                     uint8_t * nonce,
                                     size_t nonce_size,
                                     size_t * nonce_length);
psa_aead_operation_t psa_aead_operation_init(void);
psa_status_t psa_aead_set_lengths(psa_aead_operation_t * operation,
                                  size_t ad_length,
                                  size_t plaintext_length);
psa_status_t psa_aead_set_nonce(psa_aead_operation_t * operation,
                                const uint8_t * nonce,
                                size_t nonce_length);
psa_status_t psa_aead_update(psa_aead_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * output,
                             size_t output_size,
                             size_t * output_length);
psa_status_t psa_aead_update_ad(psa_aead_operation_t * operation,
                                const uint8_t * input,
                                size_t input_length);
psa_status_t psa_aead_verify(psa_aead_operation_t * operation,
                             uint8_t * plaintext,
                             size_t plaintext_size,
                             size_t * plaintext_length,
                             const uint8_t * tag,
                             size_t tag_length);
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);
psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);
psa_status_t psa_attach_key(const psa_key_attributes_t * attributes,
                            const uint8_t * label,
                            size_t label_length,
                            psa_key_id_t * key);
psa_status_t psa_check_key_usage(psa_key_id_t key,
                                 psa_algorithm_t alg,
                                 psa_key_usage_t usage);
psa_status_t psa_cipher_abort(psa_cipher_operation_t * operation);
psa_status_t psa_cipher_decrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);
psa_status_t psa_cipher_encrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);
psa_status_t psa_cipher_finish(psa_cipher_operation_t * operation,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t * operation,
                                    uint8_t * iv,
                                    size_t iv_size,
                                    size_t * iv_length);
psa_cipher_operation_t psa_cipher_operation_init(void);
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t * operation,
                               const uint8_t * iv,
                               size_t iv_length);
psa_status_t psa_cipher_update(psa_cipher_operation_t * operation,
                               const uint8_t * input,
                               size_t input_length,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);
psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t * attributes,
                          psa_key_id_t * target_key);
psa_status_t psa_crypto_init(void);
psa_status_t psa_crypto_init_subsystem(psa_crypto_subsystem_t subsystem);
psa_status_t psa_decapsulate(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * ciphertext,
                             size_t ciphertext_length,
                             const psa_key_attributes_t * attributes,
                             psa_key_id_t * output_key);
psa_status_t psa_destroy_key(psa_key_id_t key);
psa_status_t psa_encapsulate(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const psa_key_attributes_t * attributes,
                             psa_key_id_t * output_key,
                             uint8_t * ciphertext,
                             size_t ciphertext_size,
                             size_t * ciphertext_length);
psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t * data,
                            size_t data_size,
                            size_t * data_length);
psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t * data,
                                   size_t data_size,
                                   size_t * data_length);
psa_status_t psa_generate_key(const psa_key_attributes_t * attributes,
                              psa_key_id_t * key);
psa_status_t psa_generate_key_custom(const psa_key_attributes_t * attributes,
                                     const psa_custom_key_parameters_t * custom,
                                     const uint8_t * custom_data,
                                     size_t custom_data_length,
                                     psa_key_id_t * key);
psa_status_t psa_generate_random(uint8_t * output,
                                 size_t output_size);
psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t * attributes);
psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t * attributes);
size_t psa_get_key_bits(const psa_key_attributes_t * attributes);
psa_key_id_t psa_get_key_id(const psa_key_attributes_t * attributes);
psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t * attributes);
psa_key_type_t psa_get_key_type(const psa_key_attributes_t * attributes);
psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t * attributes);
psa_status_t psa_hash_abort(psa_hash_operation_t * operation);
psa_status_t psa_hash_clone(const psa_hash_operation_t * source_operation,
                            psa_hash_operation_t * target_operation);
psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              const uint8_t * hash,
                              size_t hash_length);
psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * hash,
                              size_t hash_size,
                              size_t * hash_length);
psa_status_t psa_hash_finish(psa_hash_operation_t * operation,
                             uint8_t * hash,
                             size_t hash_size,
                             size_t * hash_length);
psa_hash_operation_t psa_hash_operation_init(void);
psa_status_t psa_hash_resume(psa_hash_operation_t * operation,
                             const uint8_t * hash_state,
                             size_t hash_state_length);
psa_status_t psa_hash_setup(psa_hash_operation_t * operation,
                            psa_algorithm_t alg);
psa_status_t psa_hash_suspend(psa_hash_operation_t * operation,
                              uint8_t * hash_state,
                              size_t hash_state_size,
                              size_t * hash_state_length);
psa_status_t psa_hash_update(psa_hash_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length);
psa_status_t psa_hash_verify(psa_hash_operation_t * operation,
                             const uint8_t * hash,
                             size_t hash_length);
psa_status_t psa_import_key(const psa_key_attributes_t * attributes,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key);
psa_status_t psa_key_agreement(psa_key_id_t private_key,
                               const uint8_t * peer_key,
                               size_t peer_key_length,
                               psa_algorithm_t alg,
                               const psa_key_attributes_t * attributes,
                               psa_key_id_t * key);
psa_key_attributes_t psa_key_attributes_init(void);
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t * operation);
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t * operation,
                                             size_t * capacity);
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t * operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t * data,
                                            size_t data_length);
psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              uint64_t value);
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t * operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key);
psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t * peer_key,
                                              size_t peer_key_length);
psa_key_derivation_operation_t psa_key_derivation_operation_init(void);
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t * operation,
                                             uint8_t * output,
                                             size_t output_length);
psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t * attributes,
                                           psa_key_derivation_operation_t * operation,
                                           psa_key_id_t * key);
psa_status_t psa_key_derivation_output_key_custom(const psa_key_attributes_t * attributes,
                                                  psa_key_derivation_operation_t * operation,
                                                  const psa_custom_key_parameters_t * custom,
                                                  const uint8_t * custom_data,
                                                  size_t custom_data_length,
                                                  psa_key_id_t * key);
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t * operation,
                                             size_t capacity);
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t * operation,
                                      psa_algorithm_t alg);
psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t * operation,
                                             const uint8_t * expected_output,
                                             size_t output_length);
psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t * operation,
                                           psa_key_id_t expected);
psa_status_t psa_mac_abort(psa_mac_operation_t * operation);
psa_status_t psa_mac_compute(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * mac,
                             size_t mac_size,
                             size_t * mac_length);
psa_mac_operation_t psa_mac_operation_init(void);
psa_status_t psa_mac_sign_finish(psa_mac_operation_t * operation,
                                 uint8_t * mac,
                                 size_t mac_size,
                                 size_t * mac_length);
psa_status_t psa_mac_sign_setup(psa_mac_operation_t * operation,
                                psa_key_id_t key,
                                psa_algorithm_t alg);
psa_status_t psa_mac_update(psa_mac_operation_t * operation,
                            const uint8_t * input,
                            size_t input_length);
psa_status_t psa_mac_verify(psa_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t * input,
                            size_t input_length,
                            const uint8_t * mac,
                            size_t mac_length);
psa_status_t psa_mac_verify_finish(psa_mac_operation_t * operation,
                                   const uint8_t * mac,
                                   size_t mac_length);
psa_status_t psa_mac_verify_setup(psa_mac_operation_t * operation,
                                  psa_key_id_t key,
                                  psa_algorithm_t alg);
psa_status_t psa_pake_abort(psa_pake_operation_t * operation);
psa_pake_cipher_suite_t psa_pake_cipher_suite_init(void);
psa_algorithm_t psa_pake_cs_get_algorithm(const psa_pake_cipher_suite_t* cipher_suite);
uint32_t psa_pake_cs_get_key_confirmation(const psa_pake_cipher_suite_t* cipher_suite);
psa_pake_primitive_t psa_pake_cs_get_primitive(const psa_pake_cipher_suite_t* cipher_suite);
void psa_pake_cs_set_algorithm(psa_pake_cipher_suite_t* cipher_suite,
                               psa_algorithm_t alg);
void psa_pake_cs_set_key_confirmation(psa_pake_cipher_suite_t* cipher_suite,
                                      uint32_t key_confirmation);
void psa_pake_cs_set_primitive(psa_pake_cipher_suite_t* cipher_suite,
                               psa_pake_primitive_t primitive);
psa_status_t psa_pake_get_shared_key(psa_pake_operation_t * operation,
                                     const psa_key_attributes_t * attributes,
                                     psa_key_id_t * key);
psa_status_t psa_pake_input(psa_pake_operation_t * operation,
                            psa_pake_step_t step,
                            const uint8_t * input,
                            size_t input_length);
psa_pake_operation_t psa_pake_operation_init(void);
psa_status_t psa_pake_output(psa_pake_operation_t * operation,
                             psa_pake_step_t step,
                             uint8_t * output,
                             size_t output_size,
                             size_t * output_length);
psa_status_t psa_pake_set_context(psa_pake_operation_t * operation,
                                  const uint8_t * context,
                                  size_t context_len);
psa_status_t psa_pake_set_peer(psa_pake_operation_t * operation,
                               const uint8_t * peer_id,
                               size_t peer_id_len);
psa_status_t psa_pake_set_role(psa_pake_operation_t * operation,
                               psa_pake_role_t role);
psa_status_t psa_pake_set_user(psa_pake_operation_t * operation,
                               const uint8_t * user_id,
                               size_t user_id_len);
psa_status_t psa_pake_setup(psa_pake_operation_t * operation,
                            psa_key_id_t password_key,
                            const psa_pake_cipher_suite_t * cipher_suite);
psa_status_t psa_purge_key(psa_key_id_t key);
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t * peer_key,
                                   size_t peer_key_length,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_length);
void psa_reset_key_attributes(psa_key_attributes_t * attributes);
void psa_set_key_algorithm(psa_key_attributes_t * attributes,
                           psa_algorithm_t alg);
void psa_set_key_bits(psa_key_attributes_t * attributes,
                      size_t bits);
void psa_set_key_id(psa_key_attributes_t * attributes,
                    psa_key_id_t id);
void psa_set_key_lifetime(psa_key_attributes_t * attributes,
                          psa_key_lifetime_t lifetime);
void psa_set_key_type(psa_key_attributes_t * attributes,
                      psa_key_type_t type);
void psa_set_key_usage_flags(psa_key_attributes_t * attributes,
                             psa_key_usage_t usage_flags);
psa_status_t psa_sign_abort(psa_sign_operation_t * operation);
psa_status_t psa_sign_finish(psa_sign_operation_t * operation,
                             uint8_t * signature,
                             size_t signature_size,
                             size_t * signature_length);
psa_status_t psa_sign_hash(psa_key_id_t key,
                           psa_algorithm_t alg,
                           const uint8_t * hash,
                           size_t hash_length,
                           uint8_t * signature,
                           size_t signature_size,
                           size_t * signature_length);
psa_status_t psa_sign_hash_with_context(psa_key_id_t key,
                                        psa_algorithm_t alg,
                                        const uint8_t * hash,
                                        size_t hash_length,
                                        const uint8_t * context,
                                        size_t context_length,
                                        uint8_t * signature,
                                        size_t signature_size,
                                        size_t * signature_length);
psa_status_t psa_sign_message(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * signature,
                              size_t signature_size,
                              size_t * signature_length);
psa_status_t psa_sign_message_with_context(psa_key_id_t key,
                                           psa_algorithm_t alg,
                                           const uint8_t * input,
                                           size_t input_length,
                                           const uint8_t * context,
                                           size_t context_length,
                                           uint8_t * signature,
                                           size_t signature_size,
                                           size_t * signature_length);
psa_sign_operation_t psa_sign_operation_init(void);
psa_status_t psa_sign_set_context(psa_sign_operation_t * operation,
                                  const uint8_t * context,
                                  size_t context_length);
psa_status_t psa_sign_setup(psa_sign_operation_t * operation,
                            psa_key_id_t key,
                            psa_algorithm_t alg);
psa_status_t psa_sign_update(psa_sign_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length);
psa_status_t psa_unwrap_key(const psa_key_attributes_t * attributes,
                            psa_key_id_t wrapping_key,
                            psa_algorithm_t alg,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key);
psa_status_t psa_verify_abort(psa_verify_operation_t * operation);
psa_status_t psa_verify_finish(psa_verify_operation_t * operation);
psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * hash,
                             size_t hash_length,
                             const uint8_t * signature,
                             size_t signature_length);
psa_status_t psa_verify_hash_with_context(psa_key_id_t key,
                                          psa_algorithm_t alg,
                                          const uint8_t * hash,
                                          size_t hash_length,
                                          const uint8_t * context,
                                          size_t context_length,
                                          const uint8_t * signature,
                                          size_t signature_length);
psa_status_t psa_verify_message(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                const uint8_t * signature,
                                size_t signature_length);
psa_status_t psa_verify_message_with_context(psa_key_id_t key,
                                             psa_algorithm_t alg,
                                             const uint8_t * input,
                                             size_t input_length,
                                             const uint8_t * context,
                                             size_t context_length,
                                             const uint8_t * signature,
                                             size_t signature_length);
psa_verify_operation_t psa_verify_operation_init(void);
psa_status_t psa_verify_set_context(psa_verify_operation_t * operation,
                                    const uint8_t * context,
                                    size_t context_length);
psa_status_t psa_verify_setup(psa_verify_operation_t * operation,
                              psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * signature,
                              size_t signature_length);
psa_status_t psa_verify_update(psa_verify_operation_t * operation,
                               const uint8_t * input,
                               size_t input_length);
psa_status_t psa_wrap_key(psa_key_id_t wrapping_key,
                          psa_algorithm_t alg,
                          psa_key_id_t key,
                          uint8_t * data,
                          size_t data_size,
                          size_t * data_length);
psa_status_t psa_xof_abort(psa_xof_operation_t * operation);
psa_xof_operation_t psa_xof_operation_init(void);
psa_status_t psa_xof_output(psa_xof_operation_t * operation,
                            uint8_t * output,
                            size_t output_length);
psa_status_t psa_xof_set_context(psa_xof_operation_t * operation,
                                 const uint8_t * context,
                                 size_t context_length);
psa_status_t psa_xof_setup(psa_xof_operation_t * operation,
                           psa_algorithm_t alg);
psa_status_t psa_xof_update(psa_xof_operation_t * operation,
                            const uint8_t * input,
                            size_t input_length);
