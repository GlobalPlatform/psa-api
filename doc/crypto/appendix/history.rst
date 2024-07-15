.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

Changes to the API
==================

.. _changes:

Document change history
-----------------------

This section provides the detailed changes made between published version of the document.

Changes between *1.4.1* and *1.5.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added support for BLAKE2:

    -   BLAKE2s and BLAKE2sp cryptographic hashes, `PSA_ALG_BLAKE2S_HASH256` and `PSA_ALG_BLAKE2SP_HASH256`.
    -   BLAKE2b and BLAKE2bp cryptographic hashes, `PSA_ALG_BLAKE2B_HASH512` and `PSA_ALG_BLAKE2BP_HASH512`.
    -   A BLAKE2 MAC based on a BLAKE2 hash, `PSA_ALG_BLAKE2_MAC()`.

*   Provide multi-part operations for asymmetric signatures.
    This enables many message-signature algorithms to be used for fragmented messages.
    See :secref:`multi-part-signature`.

*   Added support for partial initialization of the implementation. See :secref:`library-init`.

Relaxations
~~~~~~~~~~~


Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~


Other changes
~~~~~~~~~~~~~

*   Integrated the PQC algorithms and key types from :cite-title:`PSA-PQC`.

    This provides support for LMS, HSS, XMSS, |XMSS^MT|, ML-DSA, SLH-DSA, and ML-KEM.

    -   For LMS and HSS, see :secref:`lms-keys` and :secref:`lms-algorithms`.
    -   For XMSS and |XMSS^MT|, see :secref:`xmss-keys` and :secref:`xmss-algorithms`.
    -   For ML-DSA, see :secref:`ml-dsa-keys` and :secref:`ml-dsa-algorithms`.
    -   For SLH-DSA, see :secref:`slh-dsa-keys` and :secref:`slh-dsa-algorithms`.
    -   For ML-KEM, see :secref:`ml-kem-keys` and :secref:`ml-kem-algorithms`.
    -   Additional hash algorithms: `PSA_ALG_SHAKE128_256`, `PSA_ALG_SHAKE256_192`, `PSA_ALG_SHAKE256_256`, and `PSA_ALG_SHA_256_192`.

Changes between *1.4.0* and *1.4.1*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Clarified that when a hash algorithm is unspecified and not recommended with HMAC, this also applies to algorithms based on HMAC.
    This affects `PSA_ALG_ASCON_HASH256` and `PSA_ALG_SHAKE256_512`.
*   Noted that some hash algorithms are unspecified and not recommended with `PSA_ALG_RSA_PKCS1V15_SIGN` due to the lack of a standard OID.
    This affects `PSA_ALG_AES_MMO_ZIGBEE`, `PSA_ALG_ASCON_HASH256` and `PSA_ALG_SHAKE256_512`.

Changes between *1.3.2* and *1.4.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added `psa_attach_key()` to register existing key material as a volatile key within the implementation.
*   Added `psa_check_key_usage()` to query a key's capabilities.
*   Add support for extendable-output functions (XOF).
    See :secref:`xof`.
*   Added support for key wrapping using key-wrapping algorithms.
    See :secref:`key-wrapping`.
*   Added support for context parameters in signature algorithms:

    -   `psa_sign_message_with_context()`
    -   `psa_verify_message_with_context()`
    -   `psa_sign_hash_with_context()`
    -   `psa_verify_hash_with_context()`

    See :secref:`sign`.
*   Added PureEdDSA algorithms with non-zero context.
    See :secref:`eddsa-sign-algorithms` and `PSA_ALG_EDDSA_CTX`.
*   Added support for the WPA3-SAE PAKE:

    -   Add `PSA_KEY_TYPE_WPA3_SAE_ECC` and `PSA_KEY_TYPE_WPA3_SAE_DH` key types for WPA3-SAE password tokens.
    -   Added the `PSA_ALG_WPA3_SAE_H2E()` KDF for generating a WPA3-SAE password token from a password.
    -   Added WPA3-SAE PAKE algorithms, `PSA_ALG_WPA3_SAE_FIXED()` and `PSA_ALG_WPA3_SAE_GDH()`.
    -   Added finite field Diffie-Hellman family `PSA_DH_FAMILY_RFC3526`, which provides cyclic groups used for WPA3-SAE.
    -   Added wildcard key policy `PSA_ALG_WPA3_SAE_ANY` to permit password and password token keys to be used in any WPA3-SAE cipher suite.

    See :secref:`pake-wpa3-sae`.
*   Add support for the Ascon family of light-weight algorithms:

    -   `PSA_ALG_ASCON_AEAD128`
    -   `PSA_ALG_ASCON_HASH256`
    -   `PSA_ALG_ASCON_XOF128`
    -   `PSA_ALG_ASCON_CXOF128`

Relaxations
~~~~~~~~~~~

*   Relaxed the permitted-key policy requirements for ECDSA verification, to be consistent with those for ML-DSA and SLH-DSA.
    When verifying a signature, the `PSA_ALG_ECDSA` and `PSA_ALG_DETERMINISTIC_ECDSA` are considered equivalent when checking the key's permitted-algorithm policy.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Corrected the example implementation of `PSA_ALG_IS_SIGN_HASH()` in :secref:`appendix-specdef-values`, to exclude PureEdDSA.
*   Clarified the use of hash algorithms with `PSA_ALG_HMAC`.

Other changes
~~~~~~~~~~~~~

*   Reorganized the chapter on key types.
    See :secref:`key-types`.

Changes between *1.3.1* and *1.3.2*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Other changes
~~~~~~~~~~~~~

*   Updated introduction to reflect GlobalPlatform assuming the governance of the PSA Certified evaluation scheme.

Changes between *1.3.0* and *1.3.1*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Clarify the way a 'volatile key' is designated, based on a persistence level of `PSA_KEY_PERSISTENCE_VOLATILE`, to ensure that this is consistent throughout the specification. See :secref:`key-lifetimes`.
*   Corrected the type of the key id parameter to `psa_generate_key_custom()` and `psa_key_derivation_output_key_custom()`.
*   Added missing 'Added in version' information to key derivation macros.

Changes between *1.2.1* and *1.3.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added `PSA_EXPORT_ASYMMETRIC_KEY_MAX_SIZE` to evaluate the export buffer size for any asymmetric key pair or public key.
*   Add extended key-generation and key-derivation functions, `psa_generate_key_custom()` and `psa_key_derivation_output_key_custom()`, that accept additional parameters to control the key creation process.
*   Define a key production parameter to select a non-default exponent for RSA key generation.
*   Reworked the allocation of bits in the encoding of asymmetric keys, to increase the scope for additional asymmetric key types:

    -   Bit 7 was previously an unused indicator for :sc:`implementation defined` family values, and is now allocated to the ASYM-TYPE.
    -   ASYM-TYPE 0 is now a category for non-parameterized asymmetric keys, of which RSA is one specific type.

    This has no effect on any currently allocated key type values, but affects the correct implementation of macros used to manipulate asymmetric key types.

    See :secref:`asymmetric-key-encoding` and :secref:`appendix-specdef-key-values`.
*   Added key-encapsulation functions, `psa_encapsulate()` and `psa_decapsulate()`.

    -   Added `PSA_ALG_ECIES_SEC1` as a key-encapsulation algorithm that implements the key agreement steps of ECIES.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Clarified the documentation of key attributes in key creation functions.
*   Clarified the constraint on `psa_key_derivation_output_key()` for algorithms that have a `PSA_KEY_DERIVATION_INPUT_PASSWORD` input step.
*   Removed the redundant key input constraints on `psa_key_derivation_verify_bytes()` and `psa_key_derivation_verify_key()`. These match the policy already checked in `psa_key_derivation_input_key()`.
*   Documented the use of context parameters in J-PAKE and SPAKE2+ PAKE operations.
    See :secref:`jpake-operation` and :secref:`spake2p-operation`.
*   Clarified asymmetric signature support by categorizing the different types of signature algorithm.

Other changes
~~~~~~~~~~~~~

*   Integrated the PAKE Extension with the main specification for the |API|.
*   Moved the documentation of key formats and key-derivation procedures to sub-sections within each key type.
*   Clarified the flexibility for an implementation to return either :code:`PSA_ERROR_NOT_SUPPORTED` or :code:`PSA_ERROR_INVALID_ARGUMENT` when provided with unsupported algorithm identifier or key parameters.
*   Added API version information to APIs that have been added or changed since version 1.0 of the |API|.

Changes between *1.2.0* and *1.2.1*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Fix the example implementation of `PSA_ALG_KEY_AGREEMENT_GET_BASE()` and `PSA_ALG_KEY_AGREEMENT_GET_KDF()` in :secref:`appendix-specdef-values`, to give correct results for key agreements combined with PBKDF2.
*   Remove the dependency on the underlying hash algorithm in definition of HMAC keys, and their behavior on import and export.
    Transferred the responsibility for truncating over-sized HMAC keys to the application.
    See `PSA_KEY_TYPE_HMAC`.
*   Rewrite the description of `PSA_ALG_CTR`, to clarify how to use the API to set the appropriate IV for different application use cases.

Changes between *1.1.2* and *1.2.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added `psa_key_agreement()` for standalone key agreement that outputs to a new key object. Also added `PSA_ALG_IS_STANDALONE_KEY_AGREEMENT()` as a synonym for `PSA_ALG_IS_RAW_KEY_AGREEMENT()`.

*   Added support for the XChaCha20 cipher and XChaCha20-Poly1305 AEAD algorithms. See `PSA_KEY_TYPE_XCHACHA20` and `PSA_ALG_XCHACHA20_POLY1305`.
*   Added support for :cite-title:`ZIGBEE` cryptographic algorithms. See `PSA_ALG_AES_MMO_ZIGBEE` and `PSA_ALG_CCM_STAR_NO_TAG`.
*   Defined key-derivation algorithms based on the Counter mode recommendations in :cite-title:`SP800-108`. See `PSA_ALG_SP800_108_COUNTER_HMAC()` and `PSA_ALG_SP800_108_COUNTER_CMAC`.
*   Added support for TLS 1.2 ECJPAKE-to-PMS key-derivation. See `PSA_ALG_TLS12_ECJPAKE_TO_PMS`.

*   Changed the policy for `psa_key_derivation_verify_bytes()` and `psa_key_derivation_verify_key()`, so that these functions are also permitted when an input key has the `PSA_KEY_USAGE_DERIVE` usage flag.
*   Removed the special treatment of :code:`PSA_ERROR_INVALID_SIGNATURE` for key-derivation operations. A verification failure in `psa_key_derivation_verify_bytes()` and `psa_key_derivation_verify_key()` now puts the operation into an error state.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Clarified the behavior of a key-derivation operation when there is insufficient capacity for a call to `psa_key_derivation_output_bytes()`, `psa_key_derivation_output_key()`, `psa_key_derivation_verify_bytes()`, or `psa_key_derivation_verify_key()`.
*   Reserved the value ``0`` for most enum-like integral types.
*   Changed terminology for clarification: a 'raw key agreement' algorithm is now a 'standalone key agreement', and a 'full key agreement' is a 'combined key agreement'.


Changes between *1.1.1* and *1.1.2*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Clarified the requirements on the ``hash`` parameter in the `psa_sign_hash()` and `psa_verify_hash()` functions.
*   Explicitly described the handling of input and output in `psa_cipher_update()`, consistent with the documentation of `psa_aead_update()`.
*   Clarified the behavior of operation objects following a call to a setup function. Provided a diagram to illustrate :ref:`multi-part operation states <multi-part-operations>`.
*   Clarified the key policy requirement for `PSA_ALG_ECDSA_ANY`.
*   Clarified `PSA_KEY_USAGE_EXPORT`: "it permits moving a key outside of its current security boundary". This improves understanding of why it is not only required for `psa_export_key()`, but can also be required for `psa_copy_key()` in some situations.

Other changes
~~~~~~~~~~~~~

*   Moved the documentation of supported key import/export formats to a separate section of the specification.

Changes between *1.1.0* and *1.1.1*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Extended `PSA_ALG_TLS12_PSK_TO_MS` to support TLS cipher suites that mix a key exchange with a pre-shared key.
*   Added a new key-derivation input step `PSA_KEY_DERIVATION_INPUT_OTHER_SECRET`.
*   Added new algorithm families `PSA_ALG_HKDF_EXTRACT` and `PSA_ALG_HKDF_EXPAND` for protocols that require the two parts of HKDF separately.

Other changes
~~~~~~~~~~~~~

*   Relicensed the document under Attribution-ShareAlike 4.0 International with a patent license derived from Apache License 2.0. See :secref:`license`.
*   Adopted a standard set of Adversarial models for the Security Risk Assessment. See :secref:`adversarial-models`.

Changes between *1.0.1* and *1.1.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Relaxation when a raw key agreement is used as a key's permitted-algorithm policy. This now also permits the key agreement to be combined with any key-derivation algorithm. See `PSA_ALG_FFDH` and `PSA_ALG_ECDH`.

*   Provide wildcard permitted-algorithm polices for MAC and AEAD that can specify a minimum MAC or tag length. The following elements are added to the API:

    -   `PSA_ALG_AT_LEAST_THIS_LENGTH_MAC()`
    -   `PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG()`

*   Added support for password-hashing and key-stretching algorithms, as key-derivation operations.

    -   Added key types `PSA_KEY_TYPE_PASSWORD`, `PSA_KEY_TYPE_PASSWORD_HASH` and `PSA_KEY_TYPE_PEPPER`, to support use of these new types of algorithm.
    -   Add key-derivation input steps `PSA_KEY_DERIVATION_INPUT_PASSWORD` and `PSA_KEY_DERIVATION_INPUT_COST`.
    -   Added `psa_key_derivation_input_integer()` to support numerical inputs to a key-derivation operation.
    -   Added functions `psa_key_derivation_verify_bytes()` and `psa_key_derivation_verify_key()` to compare derivation output data within the cryptoprocessor.
    -   Added usage flag `PSA_KEY_USAGE_VERIFY_DERIVATION` for using keys with the new verification functions.
    -   Modified the description of existing key-derivation APIs to enable the use of key-derivation functionality.

*   Added algorithms `PSA_ALG_PBKDF2_HMAC()` and `PSA_ALG_PBKDF2_AES_CMAC_PRF_128` to implement the PBKDF2 password-hashing algorithm.

*   Add support for twisted Edwards Elliptic curve keys, and the associated EdDSA signature algorithms. The following elements are added to the API:

    -   `PSA_ECC_FAMILY_TWISTED_EDWARDS`
    -   `PSA_ALG_PURE_EDDSA`
    -   `PSA_ALG_ED25519PH`
    -   `PSA_ALG_ED448PH`
    -   `PSA_ALG_SHAKE256_512`
    -   `PSA_ALG_IS_HASH_EDDSA()`

*   Added an identifier for `PSA_KEY_TYPE_ARIA`.

*   Added `PSA_ALG_RSA_PSS_ANY_SALT()`, which creates the same signatures as `PSA_ALG_RSA_PSS()`, but permits any salt length when verifying a signature. Also added the helper macros `PSA_ALG_IS_RSA_PSS_ANY_SALT()` and `PSA_ALG_IS_RSA_PSS_STANDARD_SALT()`, and extended `PSA_ALG_IS_RSA_PSS()` to detect both variants of the RSA-PSS algorithm.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Described the use of header files and the general API conventions. See :secref:`library-conventions`.

*   Added details for SHA-512/224 to the hash suspend state. See :secref:`hash-suspend-state`.

*   Removed ambiguities from support macros that provide buffer sizes, and improved consistency of parameter domain definition.

*   Clarified the length of salt used for creating `PSA_ALG_RSA_PSS()` signatures, and that verification requires the same length of salt in the signature.

*   Documented the use of :code:`PSA_ERROR_INVALID_ARGUMENT` when the input data to an operation exceeds the limit specified by the algorithm.

*   Clarified how the `PSA_ALG_RSA_OAEP()` algorithm uses the hash algorithm parameter.

*   Fixed error in `psa_key_derivation_setup()` documentation: combined key-agreement and key-derivation algorithms are valid for the |API|.

*   Added and clarified documentation for error conditions across the API.

*   Clarified the distinction between `PSA_ALG_IS_HASH_AND_SIGN()` and `PSA_ALG_IS_SIGN_HASH()`.

*   Clarified the behavior of `PSA_ALG_IS_HASH_AND_SIGN()` with a wildcard algorithm policy parameter.

*   Documented the use of `PSA_ALG_RSA_PKCS1V15_SIGN_RAW` with the :code:`PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH)` wildcard policy.

*   Clarified the way that `PSA_ALG_CCM` determines the value of the CCM configuration parameter *L*. Clarified that nonces generated by `psa_aead_generate_nonce()` can be shorter than the default nonce length provided by `PSA_AEAD_NONCE_LENGTH()`.

Other changes
~~~~~~~~~~~~~

*   Add new appendix describing the encoding of algorithm identifiers and key types. See :secref:`appendix-encodings`.

*   Migrated cryptographic operation summaries to the start of the appropriate operation section, and out of the :secref:`functionality-overview`.

*   Included a Security Risk Assessment for the |API|.


Changes between *1.0.0* and *1.0.1*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added subtypes `psa_key_persistence_t` and `psa_key_location_t` for key lifetimes, and defined standard values for these attributes.

*   Added identifiers for `PSA_ALG_SM3` and `PSA_KEY_TYPE_SM4`.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*   Provided citation references for all cryptographic algorithms in the specification.

*   Provided precise key size information for all key types.

*   Permitted implementations to store and export long HMAC keys in hashed form.

*   Provided details for initialization vectors in all unauthenticated cipher algorithms.

*   Provided details for nonces in all AEAD algorithms.

*   Clarified the input steps for HKDF.

*   Provided details of signature algorithms, include requirements when using with `psa_sign_hash()` and `psa_verify_hash()`.

*   Provided details of key-agreement algorithms, and how to use them.

*   Aligned terminology relating to key policies, to clarify the combination of the usage flags and permitted algorithm in the policy.

*   Clarified the use of the individual key attributes for all of the key creation functions.

*   Restructured the description for `psa_key_derivation_output_key()`, to clarify the handling of the excess bits in ECC key generation when needing a string of bits whose length is not a multiple of ``8``.

*   Referenced the correct buffer size macros for `psa_export_key()`.

*   Removed the use of the :code:`PSA_ERROR_DOES_NOT_EXIST` error.

*   Clarified concurrency rules.

*   Document that `psa_key_derivation_output_key()` does not return :code:`PSA_ERROR_NOT_PERMITTED` if the secret input is the result of a key agreement. This matches what was already documented for `PSA_KEY_DERIVATION_INPUT_SECRET`.

*   Relax the requirement to use the defined key-derivation methods in `psa_key_derivation_output_key()`: implementation-specific KDF algorithms can use implementation-defined methods to derive the key material.

*   Clarify the requirements for implementations that support concurrent execution of API calls.

Other changes
~~~~~~~~~~~~~

*   Provided a glossary of terms.

*   Provided a table of references.

*   Restructured the :secref:`key-management` chapter.

    -   Moved individual attribute types, values and accessor functions into their own sections.
    -   Placed permitted algorithms and usage flags into :secref:`key-policy`.
    -   Moved most introductory material from the :secref:`functionality-overview` into the relevant API sections.


Changes between *1.0 beta 3* and *1.0.0*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added `PSA_CRYPTO_API_VERSION_MAJOR` and `PSA_CRYPTO_API_VERSION_MINOR` to report the |API| version.

*   Removed ``PSA_ALG_GMAC`` algorithm identifier.

*   Removed internal implementation macros from the API specification:

    -   ``PSA_AEAD_TAG_LENGTH_OFFSET``
    -   ``PSA_ALG_AEAD_FROM_BLOCK_FLAG``
    -   ``PSA_ALG_AEAD_TAG_LENGTH_MASK``
    -   ``PSA__ALG_AEAD_WITH_DEFAULT_TAG_LENGTH__CASE``
    -   ``PSA_ALG_CATEGORY_AEAD``
    -   ``PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION``
    -   ``PSA_ALG_CATEGORY_CIPHER``
    -   ``PSA_ALG_CATEGORY_HASH``
    -   ``PSA_ALG_CATEGORY_KEY_AGREEMENT``
    -   ``PSA_ALG_CATEGORY_KEY_DERIVATION``
    -   ``PSA_ALG_CATEGORY_MAC``
    -   ``PSA_ALG_CATEGORY_MASK``
    -   ``PSA_ALG_CATEGORY_SIGN``
    -   ``PSA_ALG_CIPHER_FROM_BLOCK_FLAG``
    -   ``PSA_ALG_CIPHER_MAC_BASE``
    -   ``PSA_ALG_CIPHER_STREAM_FLAG``
    -   ``PSA_ALG_DETERMINISTIC_ECDSA_BASE``
    -   ``PSA_ALG_ECDSA_BASE``
    -   ``PSA_ALG_ECDSA_IS_DETERMINISTIC``
    -   ``PSA_ALG_HASH_MASK``
    -   ``PSA_ALG_HKDF_BASE``
    -   ``PSA_ALG_HMAC_BASE``
    -   ``PSA_ALG_IS_KEY_DERIVATION_OR_AGREEMENT``
    -   ``PSA_ALG_IS_VENDOR_DEFINED``
    -   ``PSA_ALG_KEY_AGREEMENT_MASK``
    -   ``PSA_ALG_KEY_DERIVATION_MASK``
    -   ``PSA_ALG_MAC_SUBCATEGORY_MASK``
    -   ``PSA_ALG_MAC_TRUNCATION_MASK``
    -   ``PSA_ALG_RSA_OAEP_BASE``
    -   ``PSA_ALG_RSA_PKCS1V15_SIGN_BASE``
    -   ``PSA_ALG_RSA_PSS_BASE``
    -   ``PSA_ALG_TLS12_PRF_BASE``
    -   ``PSA_ALG_TLS12_PSK_TO_MS_BASE``
    -   ``PSA_ALG_VENDOR_FLAG``
    -   ``PSA_BITS_TO_BYTES``
    -   ``PSA_BYTES_TO_BITS``
    -   ``PSA_ECDSA_SIGNATURE_SIZE``
    -   ``PSA_HMAC_MAX_HASH_BLOCK_SIZE``
    -   ``PSA_KEY_EXPORT_ASN1_INTEGER_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_DSA_KEY_PAIR_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_DSA_PUBLIC_KEY_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_ECC_KEY_PAIR_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_RSA_KEY_PAIR_MAX_SIZE``
    -   ``PSA_KEY_EXPORT_RSA_PUBLIC_KEY_MAX_SIZE``
    -   ``PSA_KEY_TYPE_CATEGORY_FLAG_PAIR``
    -   ``PSA_KEY_TYPE_CATEGORY_KEY_PAIR``
    -   ``PSA_KEY_TYPE_CATEGORY_MASK``
    -   ``PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY``
    -   ``PSA_KEY_TYPE_CATEGORY_RAW``
    -   ``PSA_KEY_TYPE_CATEGORY_SYMMETRIC``
    -   ``PSA_KEY_TYPE_DH_GROUP_MASK``
    -   ``PSA_KEY_TYPE_DH_KEY_PAIR_BASE``
    -   ``PSA_KEY_TYPE_DH_PUBLIC_KEY_BASE``
    -   ``PSA_KEY_TYPE_ECC_CURVE_MASK``
    -   ``PSA_KEY_TYPE_ECC_KEY_PAIR_BASE``
    -   ``PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE``
    -   ``PSA_KEY_TYPE_IS_VENDOR_DEFINED``
    -   ``PSA_KEY_TYPE_VENDOR_FLAG``
    -   ``PSA_MAC_TRUNCATED_LENGTH``
    -   ``PSA_MAC_TRUNCATION_OFFSET``
    -   ``PSA_ROUND_UP_TO_MULTIPLE``
    -   ``PSA_RSA_MINIMUM_PADDING_SIZE``
    -   ``PSA_VENDOR_ECC_MAX_CURVE_BITS``
    -   ``PSA_VENDOR_RSA_MAX_KEY_BITS``

*   Remove the definition of implementation-defined macros from the specification, and clarified the implementation requirements for these macros in :secref:`implementation-specific-macro`.

    -   Macros with implementation-defined values are indicated by ``/* implementation-defined value */`` in the API prototype.
        The implementation must provide the implementation.

    -   Macros for algorithm and key type construction and inspection have specification-defined values.
        This is indicated by ``/* specification-defined value */`` in the API prototype.
        Example definitions of these macros is provided in :secref:`appendix-specdef-values`.

*   Changed the semantics of multi-part operations.

    -   Formalize the standard pattern for multi-part operations.
    -   Require all errors to result in an error state, requiring a call to ``psa_xxx_abort()`` to reset the object.
    -   Define behavior in illegal and impossible operation states, and for copying and reusing operation objects.

    Although the API signatures have not changed, this change requires modifications to application flows that handle error conditions in multi-part operations.

*   Merge the key identifier and key handle concepts in the API.

    -   Replaced all references to key handles with key identifiers, or something similar.
    -   Replaced all uses of ``psa_key_handle_t`` with `psa_key_id_t` in the API, and removes the ``psa_key_handle_t`` type.
    -   Removed ``psa_open_key`` and ``psa_close_key``.
    -   Added `PSA_KEY_ID_NULL` for the never valid zero key identifier.
    -   Document rules related to destroying keys whilst in use.
    -   Added the `PSA_KEY_USAGE_CACHE` usage flag and the related `psa_purge_key()` API.
    -   Added clarification about caching keys to non-volatile memory.

*   Renamed ``PSA_ALG_TLS12_PSK_TO_MS_MAX_PSK_LEN`` to `PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE`.

*   Relax definition of implementation-defined types.

    -   This is indicated in the specification by ``/* implementation-defined type */`` in the type definition.
    -   The specification only defines the name of implementation-defined types, and does not require that the implementation is a C struct.

*   Zero-length keys are not permitted. Attempting to create one will now result in an error.

*   Relax the constraints on inputs to key derivation:

    -   `psa_key_derivation_input_bytes()` can be used for secret input steps. This is necessary if a zero-length input is required by the application.
    -   `psa_key_derivation_input_key()` can be used for non-secret input steps.

*   Multi-part cipher operations now require that the IV is passed using `psa_cipher_set_iv()`, the option to provide this as part of the input to `psa_cipher_update()` has been removed.

    The format of the output from `psa_cipher_encrypt()`, and input to `psa_cipher_decrypt()`, is documented.

*   Support macros to calculate the size of output buffers, IVs and nonces.

    -   Macros to calculate a key and/or algorithm specific result are provided for all output buffers. The new macros are:

        *   `PSA_AEAD_NONCE_LENGTH()`
        *   `PSA_CIPHER_ENCRYPT_OUTPUT_SIZE()`
        *   `PSA_CIPHER_DECRYPT_OUTPUT_SIZE()`
        *   `PSA_CIPHER_UPDATE_OUTPUT_SIZE()`
        *   `PSA_CIPHER_FINISH_OUTPUT_SIZE()`
        *   `PSA_CIPHER_IV_LENGTH()`
        *   `PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE()`
        *   `PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE()`

    -   Macros that evaluate to a maximum type-independent buffer size are provided. The new macros are:

        *   `PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE()`
        *   `PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE()`
        *   `PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE()`
        *   `PSA_AEAD_FINISH_OUTPUT_MAX_SIZE`
        *   `PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE`
        *   `PSA_AEAD_NONCE_MAX_SIZE`
        *   `PSA_AEAD_TAG_MAX_SIZE`
        *   `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE`
        *   `PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE`
        *   `PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE()`
        *   `PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE()`
        *   `PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE()`
        *   `PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE`
        *   `PSA_CIPHER_IV_MAX_SIZE`
        *   `PSA_EXPORT_KEY_PAIR_MAX_SIZE`
        *   `PSA_EXPORT_PUBLIC_KEY_MAX_SIZE`
        *   `PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE`

    -   AEAD output buffer size macros are now parameterized on the key type as well as the algorithm:

        *   `PSA_AEAD_ENCRYPT_OUTPUT_SIZE()`
        *   `PSA_AEAD_DECRYPT_OUTPUT_SIZE()`
        *   `PSA_AEAD_UPDATE_OUTPUT_SIZE()`
        *   `PSA_AEAD_FINISH_OUTPUT_SIZE()`
        *   `PSA_AEAD_TAG_LENGTH()`
        *   `PSA_AEAD_VERIFY_OUTPUT_SIZE()`

    -   Some existing macros have been renamed to ensure that the name of the support macros are consistent. The following macros have been renamed:

        *   ``PSA_ALG_AEAD_WITH_DEFAULT_TAG_LENGTH()`` → `PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG()`
        *   ``PSA_ALG_AEAD_WITH_TAG_LENGTH()`` → `PSA_ALG_AEAD_WITH_SHORTENED_TAG()`
        *   ``PSA_KEY_EXPORT_MAX_SIZE()`` → `PSA_EXPORT_KEY_OUTPUT_SIZE()`
        *   ``PSA_HASH_SIZE()`` → `PSA_HASH_LENGTH()`
        *   ``PSA_MAC_FINAL_SIZE()`` → `PSA_MAC_LENGTH()`
        *   ``PSA_BLOCK_CIPHER_BLOCK_SIZE()`` → `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`
        *   ``PSA_MAX_BLOCK_CIPHER_BLOCK_SIZE`` → `PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE`

    -   Documentation of the macros and of related APIs has been updated to reference the related API elements.

*   Provide hash-and-sign operations as well as sign-the-hash operations. The API for asymmetric signature has been changed to clarify the use of the new functions.

    -   The existing asymmetric signature API has been renamed to clarify that this is for signing a hash that is already computed:

        *   ``PSA_KEY_USAGE_SIGN`` → `PSA_KEY_USAGE_SIGN_HASH`
        *   ``PSA_KEY_USAGE_VERIFY`` → `PSA_KEY_USAGE_VERIFY_HASH`
        *   ``psa_asymmetric_sign()`` → `psa_sign_hash()`
        *   ``psa_asymmetric_verify()`` → `psa_verify_hash()`

    -   New APIs added to provide the complete message signing operation:

        *   `PSA_KEY_USAGE_SIGN_MESSAGE`
        *   `PSA_KEY_USAGE_VERIFY_MESSAGE`
        *   `psa_sign_message()`
        *   `psa_verify_message()`

    -   New Support macros to identify which algorithms can be used in which signing API:

        *   `PSA_ALG_IS_SIGN_HASH()`
        *   `PSA_ALG_IS_SIGN_MESSAGE()`

    -   Renamed support macros that apply to both signing APIs:

        *   ``PSA_ASYMMETRIC_SIGN_OUTPUT_SIZE()`` → `PSA_SIGN_OUTPUT_SIZE()`
        *   ``PSA_ASYMMETRIC_SIGNATURE_MAX_SIZE`` → `PSA_SIGNATURE_MAX_SIZE`

    -   The usage flag values have been changed, including for `PSA_KEY_USAGE_DERIVE`.

*   Restructure `psa_key_type_t` and reassign all key type values.

    -   `psa_key_type_t` changes from 32-bit to 16-bit integer.
    -   Reassigned the key type categories.
    -   Add a parity bit to the key type to ensure that valid key type values differ by at least 2 bits.
    -   16-bit elliptic curve ids (``psa_ecc_curve_t``) replaced by 8-bit ECC curve family ids (`psa_ecc_family_t`).
        16-bit  Diffie-Hellman group ids (``psa_dh_group_t``) replaced by 8-bit DH group family ids (`psa_dh_family_t`).

        *   These ids are no longer related to the IANA Group Registry specification.
        *   The new key type values do not encode the key size for ECC curves or DH groups. The key bit size from the key attributes identify a specific ECC curve or DH group within the family.

    -   The following macros have been removed:

        *   ``PSA_DH_GROUP_FFDHE2048``
        *   ``PSA_DH_GROUP_FFDHE3072``
        *   ``PSA_DH_GROUP_FFDHE4096``
        *   ``PSA_DH_GROUP_FFDHE6144``
        *   ``PSA_DH_GROUP_FFDHE8192``
        *   ``PSA_ECC_CURVE_BITS``
        *   ``PSA_ECC_CURVE_BRAINPOOL_P256R1``
        *   ``PSA_ECC_CURVE_BRAINPOOL_P384R1``
        *   ``PSA_ECC_CURVE_BRAINPOOL_P512R1``
        *   ``PSA_ECC_CURVE_CURVE25519``
        *   ``PSA_ECC_CURVE_CURVE448``
        *   ``PSA_ECC_CURVE_SECP160K1``
        *   ``PSA_ECC_CURVE_SECP160R1``
        *   ``PSA_ECC_CURVE_SECP160R2``
        *   ``PSA_ECC_CURVE_SECP192K1``
        *   ``PSA_ECC_CURVE_SECP192R1``
        *   ``PSA_ECC_CURVE_SECP224K1``
        *   ``PSA_ECC_CURVE_SECP224R1``
        *   ``PSA_ECC_CURVE_SECP256K1``
        *   ``PSA_ECC_CURVE_SECP256R1``
        *   ``PSA_ECC_CURVE_SECP384R1``
        *   ``PSA_ECC_CURVE_SECP521R1``
        *   ``PSA_ECC_CURVE_SECT163K1``
        *   ``PSA_ECC_CURVE_SECT163R1``
        *   ``PSA_ECC_CURVE_SECT163R2``
        *   ``PSA_ECC_CURVE_SECT193R1``
        *   ``PSA_ECC_CURVE_SECT193R2``
        *   ``PSA_ECC_CURVE_SECT233K1``
        *   ``PSA_ECC_CURVE_SECT233R1``
        *   ``PSA_ECC_CURVE_SECT239K1``
        *   ``PSA_ECC_CURVE_SECT283K1``
        *   ``PSA_ECC_CURVE_SECT283R1``
        *   ``PSA_ECC_CURVE_SECT409K1``
        *   ``PSA_ECC_CURVE_SECT409R1``
        *   ``PSA_ECC_CURVE_SECT571K1``
        *   ``PSA_ECC_CURVE_SECT571R1``
        *   ``PSA_KEY_TYPE_GET_CURVE``
        *   ``PSA_KEY_TYPE_GET_GROUP``

    -   The following macros have been added:

        *   `PSA_DH_FAMILY_RFC7919`
        *   `PSA_ECC_FAMILY_BRAINPOOL_P_R1`
        *   `PSA_ECC_FAMILY_SECP_K1`
        *   `PSA_ECC_FAMILY_SECP_R1`
        *   `PSA_ECC_FAMILY_SECP_R2`
        *   `PSA_ECC_FAMILY_SECT_K1`
        *   `PSA_ECC_FAMILY_SECT_R1`
        *   `PSA_ECC_FAMILY_SECT_R2`
        *   `PSA_ECC_FAMILY_MONTGOMERY`
        *   `PSA_KEY_TYPE_DH_GET_FAMILY`
        *   `PSA_KEY_TYPE_ECC_GET_FAMILY`

    -   The following macros have new values:

        *   `PSA_KEY_TYPE_AES`
        *   `PSA_KEY_TYPE_ARC4`
        *   `PSA_KEY_TYPE_CAMELLIA`
        *   `PSA_KEY_TYPE_CHACHA20`
        *   `PSA_KEY_TYPE_DERIVE`
        *   `PSA_KEY_TYPE_DES`
        *   `PSA_KEY_TYPE_HMAC`
        *   `PSA_KEY_TYPE_NONE`
        *   `PSA_KEY_TYPE_RAW_DATA`
        *   `PSA_KEY_TYPE_RSA_KEY_PAIR`
        *   `PSA_KEY_TYPE_RSA_PUBLIC_KEY`

    -   The following macros with specification-defined values have new example implementations:

        *   `PSA_BLOCK_CIPHER_BLOCK_LENGTH`
        *   `PSA_KEY_TYPE_DH_KEY_PAIR`
        *   `PSA_KEY_TYPE_DH_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_ECC_KEY_PAIR`
        *   `PSA_KEY_TYPE_ECC_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_IS_ASYMMETRIC`
        *   `PSA_KEY_TYPE_IS_DH`
        *   `PSA_KEY_TYPE_IS_DH_KEY_PAIR`
        *   `PSA_KEY_TYPE_IS_DH_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_IS_ECC`
        *   `PSA_KEY_TYPE_IS_ECC_KEY_PAIR`
        *   `PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_IS_KEY_PAIR`
        *   `PSA_KEY_TYPE_IS_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_IS_RSA`
        *   `PSA_KEY_TYPE_IS_UNSTRUCTURED`
        *   `PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY`
        *   `PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR`

*   Add ECC family `PSA_ECC_FAMILY_FRP` for the FRP256v1 curve.

*   Restructure `psa_algorithm_t` encoding, to increase consistency across algorithm categories.

    -   Algorithms that include a hash operation all use the same structure to encode the hash algorithm. The following ``PSA_ALG_XXXX_GET_HASH()`` macros have all been replaced by a single macro `PSA_ALG_GET_HASH()`:

        *   ``PSA_ALG_HKDF_GET_HASH()``
        *   ``PSA_ALG_HMAC_GET_HASH()``
        *   ``PSA_ALG_RSA_OAEP_GET_HASH()``
        *   ``PSA_ALG_SIGN_GET_HASH()``
        *   ``PSA_ALG_TLS12_PRF_GET_HASH()``
        *   ``PSA_ALG_TLS12_PSK_TO_MS_GET_HASH()``

    -   Stream cipher algorithm macros have been removed; the key type indicates which cipher to use. Instead of ``PSA_ALG_ARC4`` and ``PSA_ALG_CHACHA20``, use `PSA_ALG_STREAM_CIPHER`.

    All of the other ``PSA_ALG_XXX`` macros have updated values or updated example implementations.

    -   The following macros have new values:

        *   `PSA_ALG_ANY_HASH`
        *   `PSA_ALG_CBC_MAC`
        *   `PSA_ALG_CBC_NO_PADDING`
        *   `PSA_ALG_CBC_PKCS7`
        *   `PSA_ALG_CCM`
        *   `PSA_ALG_CFB`
        *   `PSA_ALG_CHACHA20_POLY1305`
        *   `PSA_ALG_CMAC`
        *   `PSA_ALG_CTR`
        *   `PSA_ALG_ECDH`
        *   `PSA_ALG_ECDSA_ANY`
        *   `PSA_ALG_FFDH`
        *   `PSA_ALG_GCM`
        *   `PSA_ALG_MD2`
        *   `PSA_ALG_MD4`
        *   `PSA_ALG_MD5`
        *   `PSA_ALG_OFB`
        *   `PSA_ALG_RIPEMD160`
        *   `PSA_ALG_RSA_PKCS1V15_CRYPT`
        *   `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`
        *   `PSA_ALG_SHA_1`
        *   `PSA_ALG_SHA_224`
        *   `PSA_ALG_SHA_256`
        *   `PSA_ALG_SHA_384`
        *   `PSA_ALG_SHA_512`
        *   `PSA_ALG_SHA_512_224`
        *   `PSA_ALG_SHA_512_256`
        *   `PSA_ALG_SHA3_224`
        *   `PSA_ALG_SHA3_256`
        *   `PSA_ALG_SHA3_384`
        *   `PSA_ALG_SHA3_512`
        *   `PSA_ALG_XTS`

    -   The following macros with specification-defined values have new example implementations:

        *   `PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG()`
        *   `PSA_ALG_AEAD_WITH_SHORTENED_TAG()`
        *   `PSA_ALG_DETERMINISTIC_ECDSA()`
        *   `PSA_ALG_ECDSA()`
        *   `PSA_ALG_FULL_LENGTH_MAC()`
        *   `PSA_ALG_HKDF()`
        *   `PSA_ALG_HMAC()`
        *   `PSA_ALG_IS_AEAD()`
        *   `PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER()`
        *   `PSA_ALG_IS_ASYMMETRIC_ENCRYPTION()`
        *   `PSA_ALG_IS_BLOCK_CIPHER_MAC()`
        *   `PSA_ALG_IS_CIPHER()`
        *   `PSA_ALG_IS_DETERMINISTIC_ECDSA()`
        *   `PSA_ALG_IS_ECDH()`
        *   `PSA_ALG_IS_ECDSA()`
        *   `PSA_ALG_IS_FFDH()`
        *   `PSA_ALG_IS_HASH()`
        *   `PSA_ALG_IS_HASH_AND_SIGN()`
        *   `PSA_ALG_IS_HKDF()`
        *   `PSA_ALG_IS_HMAC()`
        *   `PSA_ALG_IS_KEY_AGREEMENT()`
        *   `PSA_ALG_IS_KEY_DERIVATION()`
        *   `PSA_ALG_IS_MAC()`
        *   `PSA_ALG_IS_RANDOMIZED_ECDSA()`
        *   `PSA_ALG_IS_RAW_KEY_AGREEMENT()`
        *   `PSA_ALG_IS_RSA_OAEP()`
        *   `PSA_ALG_IS_RSA_PKCS1V15_SIGN()`
        *   `PSA_ALG_IS_RSA_PSS()`
        *   `PSA_ALG_IS_SIGN()`
        *   `PSA_ALG_IS_SIGN_MESSAGE()`
        *   `PSA_ALG_IS_STREAM_CIPHER()`
        *   `PSA_ALG_IS_TLS12_PRF()`
        *   `PSA_ALG_IS_TLS12_PSK_TO_MS()`
        *   `PSA_ALG_IS_WILDCARD()`
        *   `PSA_ALG_KEY_AGREEMENT()`
        *   `PSA_ALG_KEY_AGREEMENT_GET_BASE()`
        *   `PSA_ALG_KEY_AGREEMENT_GET_KDF()`
        *   `PSA_ALG_RSA_OAEP()`
        *   `PSA_ALG_RSA_PKCS1V15_SIGN()`
        *   `PSA_ALG_RSA_PSS()`
        *   `PSA_ALG_TLS12_PRF()`
        *   `PSA_ALG_TLS12_PSK_TO_MS()`
        *   `PSA_ALG_TRUNCATED_MAC()`

*   Added ECB block cipher mode, with no padding, as `PSA_ALG_ECB_NO_PADDING`.

*   Add functions to suspend and resume hash operations:

    -   `psa_hash_suspend()` halts the current operation and outputs a hash suspend state.
    -   `psa_hash_resume()` continues a previously suspended hash operation.

    The format of the hash suspend state is documented in :secref:`hash-suspend-state`, and supporting macros are provided for using the |API|:

    -   `PSA_HASH_SUSPEND_OUTPUT_SIZE()`
    -   `PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE`
    -   `PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH`
    -   `PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH()`
    -   `PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH()`
    -   `PSA_HASH_BLOCK_LENGTH()`

*   Complement :code:`PSA_ERROR_STORAGE_FAILURE` with new error codes :code:`PSA_ERROR_DATA_CORRUPT` and :code:`PSA_ERROR_DATA_INVALID`. These permit an implementation to distinguish different causes of failure when reading from key storage.

*   Added input step `PSA_KEY_DERIVATION_INPUT_CONTEXT` for key derivation, supporting obvious mapping from the step identifiers to common KDF constructions.

Clarifications
~~~~~~~~~~~~~~

*   Clarified rules regarding modification of parameters in concurrent environments.

*   Guarantee that :code:`psa_destroy_key(PSA_KEY_ID_NULL)` always returns :code:`PSA_SUCCESS`.

*   Clarified the TLS PSK to MS key-agreement algorithm.

*   Document the key policy requirements for all APIs that accept a key parameter.

*   Document more of the error codes for each function.

Other changes
~~~~~~~~~~~~~

*   Require C99 for this specification instead of C89.

*   Removed references to non-standard mbed-crypto header files. The only header file that applications need to include is :file:`psa/crypto.h`.

*   Reorganized the API reference, grouping the elements in a more natural way.

*   Improved the cross referencing between all of the document sections, and from code snippets to API element descriptions.


Changes between *1.0 beta 2* and *1.0 beta 3*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Change the value of error codes, and some names, to align
    with other PSA Certified APIs. The name changes are:

    -   :code:`PSA_ERROR_UNKNOWN_ERROR` → :code:`PSA_ERROR_GENERIC_ERROR`
    -   :code:`PSA_ERROR_OCCUPIED_SLOT` → :code:`PSA_ERROR_ALREADY_EXISTS`
    -   :code:`PSA_ERROR_EMPTY_SLOT` → :code:`PSA_ERROR_DOES_NOT_EXIST`
    -   :code:`PSA_ERROR_INSUFFICIENT_CAPACITY` → :code:`PSA_ERROR_INSUFFICIENT_DATA`
    -   :code:`PSA_ERROR_TAMPERING_DETECTED` → :code:`PSA_ERROR_CORRUPTION_DETECTED`

*   Change the way keys are created to avoid “half-filled” handles
    that contained key metadata, but no key material.
    Now, to create a key, first fill in a data structure containing
    its attributes, then pass this structure to a function that
    both allocates resources for the key and fills in the key
    material. This affects the following functions:

    -   `psa_import_key()`, `psa_generate_key()`, ``psa_generator_import_key()``
        and `psa_copy_key()` now take an attribute structure, as
        a pointer to `psa_key_attributes_t`, to specify key metadata.
        This replaces the previous method of passing arguments to
        ``psa_create_key()`` or to the key material creation function
        or calling ``psa_set_key_policy()``.
    -   ``psa_key_policy_t`` and functions operating on that type
        no longer exist. A key's policy is now accessible as part of
        its attributes.
    -   ``psa_get_key_information()`` is also replaced by accessing the
        key's attributes, retrieved with `psa_get_key_attributes()`.
    -   ``psa_create_key()`` no longer exists. Instead, set the key id
        attribute and the lifetime attribute before creating the
        key material.

*   Allow `psa_aead_update()` to buffer data.

*   New buffer size calculation macros.

*   Key identifiers are no longer specific to a given lifetime value. ``psa_open_key()`` no longer takes a ``lifetime`` parameter.

*   Define a range of key identifiers for use by applications and a separate range for use by implementations.

*   Avoid the unusual terminology "generator": call them
    "key-derivation operations" instead. Rename a number of functions
    and other identifiers related to for clarity and consistency:

    -   ``psa_crypto_generator_t`` → `psa_key_derivation_operation_t`
    -   ``PSA_CRYPTO_GENERATOR_INIT`` → `PSA_KEY_DERIVATION_OPERATION_INIT`
    -   ``psa_crypto_generator_init()`` → `psa_key_derivation_operation_init()`
    -   ``PSA_GENERATOR_UNBRIDLED_CAPACITY`` → `PSA_KEY_DERIVATION_UNLIMITED_CAPACITY`
    -   ``psa_set_generator_capacity()`` → `psa_key_derivation_set_capacity()`
    -   ``psa_get_generator_capacity()`` → `psa_key_derivation_get_capacity()`
    -   ``psa_key_agreement()`` → `psa_key_derivation_key_agreement()`
    -   ``psa_generator_read()`` → `psa_key_derivation_output_bytes()`
    -   ``psa_generate_derived_key()`` → `psa_key_derivation_output_key()`
    -   ``psa_generator_abort()`` → `psa_key_derivation_abort()`
    -   ``psa_key_agreement_raw_shared_secret()`` → `psa_raw_key_agreement()`
    -   ``PSA_KDF_STEP_xxx`` → ``PSA_KEY_DERIVATION_INPUT_xxx``
    -   ``PSA_xxx_KEYPAIR`` → ``PSA_xxx_KEY_PAIR``

*   Convert TLS1.2 KDF descriptions to multi-part key derivation.

Clarifications
~~~~~~~~~~~~~~

*   Specify ``psa_generator_import_key()`` for most key types.

*   Clarify the behavior in various corner cases.

*   Document more error conditions.



Changes between *1.0 beta 1* and *1.0 beta 2*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Remove obsolete definition ``PSA_ALG_IS_KEY_SELECTION``.
*   `PSA_AEAD_FINISH_OUTPUT_SIZE`: remove spurious parameter ``plaintext_length``.

Clarifications
~~~~~~~~~~~~~~

*   ``psa_key_agreement()``: document ``alg`` parameter.

Other changes
~~~~~~~~~~~~~

*   Document formatting improvements.


Planned changes for version |docversion|.x
------------------------------------------

Future versions of this specification that use a |docversion|.x version will describe the same API as this specification.
Any changes will not affect application compatibility and will not introduce major features.
These updates are intended to add minor requirements on implementations, introduce optional definitions, make corrections, clarify potential or actual ambiguities, or improve the documentation.

These are the changes that might be included in a version 1.2.x:

*   Declare identifiers for additional cryptographic algorithms.
*   Mandate certain checks when importing some types of asymmetric keys.
*   Specify the computation of algorithm and key type values.
*   Further clarifications on API usage and implementation.

.. _future:

Future additions
----------------

Major additions to the API will be defined in future drafts and editions of a 1.x or 2.x version of this specification. Features that are being considered include:

*   Integration of the PQC extension.
*   Further PQC algorithms as they are standardized.
*   Interruptible (incremental) operations for long-running computation in a constrained execution context.
*   Import and export of additional key formats and wrapped key structures.
*   Key discovery mechanisms.
    This would enable an application to locate a key by its name or attributes.
*   Implementation capability description.
    This would enable an application to determine the algorithms, key types and storage lifetimes that the implementation provides.
*   An ownership and access control mechanism allowing a multi-client implementation to have privileged clients that are able to manage keys of other clients.
