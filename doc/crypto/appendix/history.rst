.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

Changes to the API
==================

.. _changes:

Document change history
-----------------------

This section provides the detailed changes made between published version of the document.

Changes between *1.5* and *1.6*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*  Added interruptible operations for asymmetric signatures, key generation, public-key export, and key agreement. See:

   -  :secref:`interruptible-sign` and :secref:`interruptible-verify`
   -  :secref:`interruptible-generate-key`
   -  :secref:`interruptible-export-key`
   -  :secref:`interruptible-key-agreement`

   These APIs enable applications to bound the expensive computation performed in a single call. See :secref:`interruptible-operations`.

Clarifications and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

*  Corrected the WPA3-SAE operation example code: the send-confirm counter input step is `PSA_PAKE_STEP_CONFIRM_COUNT`, and the shared key is extracted from the ``wpa3_sae`` operation.
*  Corrected the SPAKE2+ operation example code: the Prover input step for the Verifier confirmation value is `PSA_PAKE_STEP_CONFIRM`.

Other changes
~~~~~~~~~~~~~

*  Migrated the document to the 2026 PSA API template.

   This changes the document front matter structure and publication styling, without changing the API.

Changes between *1.4.1* and *1.5*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes to the API
~~~~~~~~~~~~~~~~~~

*   Added support for BLAKE2:

    -   BLAKE2s and BLAKE2sp cryptographic hashes, `PSA_ALG_BLAKE2S_HASH256` and `PSA_ALG_BLAKE2SP_HASH256`.
    -   BLAKE2b and BLAKE2bp cryptographic hashes, `PSA_ALG_BLAKE2B_HASH512` and `PSA_ALG_BLAKE2BP_HASH512`.
    -   A BLAKE2 MAC based on a BLAKE2 hash, `PSA_ALG_BLAKE2_MAC()`.

*   Provide multi-part operations for asymmetric signatures.
    This enables many message-signature algorithms to be used for fragmented messages.
    See :secref:`multi-part-signature`.


Other changes
~~~~~~~~~~~~~

*   Integrated the PQC algorithms and key types from :cite-title:`PSA PQC`.

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


Planned changes for version |APIversion|.x
------------------------------------------

Future versions of this specification that use a |docversion|.x version will describe the same API as this specification.
Any changes will not affect application compatibility and will not introduce new features.
These updates are intended to add minor requirements on implementations, introduce optional definitions, make corrections, clarify potential or actual ambiguities, or improve the documentation.

.. _future:

Future additions
----------------

Major additions to the API will be defined in future drafts and editions of a 1.x or 2.x version of this specification. Features that are being considered include:

*   Further PQC algorithms as they are standardized.
*   Interruptible (incremental) operations for long-running computation in a constrained execution context.
*   Import and export of additional key formats and wrapped key structures.
*   Key discovery mechanisms.
    This would enable an application to locate a key by its name or attributes.
*   Implementation capability description.
    This would enable an application to determine the algorithms, key types and storage lifetimes that the implementation provides.
*   An ownership and access control mechanism allowing a multi-client implementation to have privileged clients that are able to manage keys of other clients.
