.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 170

.. _key-policy:

Key policies
============

All keys have an associated policy that regulates which operations are permitted on the key. A key policy is composed of two elements:

*   A set of usage flags. See :secref:`key-usage-flags`.
*   A specific algorithm that is permitted with the key. See :secref:`permitted-algorithms`.

The policy is part of the key attributes that are managed by a `psa_key_attributes_t` object.

A highly constrained implementation might not be able to support all the policies that can be expressed through this interface. If an implementation cannot create a key with the required policy, it must return an appropriate error code when the key is created.


.. _permitted-algorithms:

Permitted algorithms
--------------------

The permitted algorithm is encoded using a algorithm identifier, as described in :secref:`algorithms`.

For most algorithms, this specification only defines policies that restrict keys to a single algorithm, which is consistent with both common practice and security good practice.

If the permitted algorithm is `PSA_ALG_NONE`, no cryptographic operation is permitted with the key.
The key can still be used for non-cryptographic actions such as exporting, if permitted by the usage flags.

For a cryptographic operation, the permitted algorithm value must exactly match the requested algorithm, except in the following cases:

*   The following pairs of signature algorithms are considered equivalent for verification, but not for computing the signature:

    -   `PSA_ALG_ECDSA` and `PSA_ALG_DETERMINISTIC_ECDSA`.
*   A signature algorithm constructed with `PSA_ALG_ANY_HASH` permits the specified signature scheme with any hash algorithm.
    In addition, :code:`PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH)` also permits the `PSA_ALG_RSA_PKCS1V15_SIGN_RAW` signature algorithm.
*   A standalone key-agreement algorithm also permits the specified key-agreement scheme to be combined with any key-derivation algorithm.
*   An algorithm built from `PSA_ALG_AT_LEAST_THIS_LENGTH_MAC()` permits any MAC algorithm from the same base class (for example, CMAC) which computes or verifies a MAC length greater than or equal to the length encoded in the wildcard algorithm.
*   An algorithm built from `PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG()` permits any AEAD algorithm from the same base class (for example, CCM) which computes or verifies a tag length greater than or equal to the length encoded in the wildcard algorithm.
*   The `PSA_ALG_CCM_STAR_ANY_TAG` wildcard algorithm permits the `PSA_ALG_CCM_STAR_NO_TAG` cipher algorithm, the `PSA_ALG_CCM` AEAD algorithm, and the :code:`PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_length)` truncated-tag AEAD algorithm for ``tag_length`` equal to 4, 8 or 16.
*   The wildcard key policy `PSA_ALG_WPA3_SAE_ANY` permits a password key or WPA3-SAE password token key to be used with any WPA3-SAE cipher suite.

When a key is used in a cryptographic operation, the application supplies the algorithm to use for the operation.
The algorithm and operation are checked against the key's permitted-algorithm policy.

.. function:: psa_set_key_algorithm

    .. summary::
        Declare the permitted-algorithm policy for a key.

    .. param:: psa_key_attributes_t * attributes
        The attribute object to write to.
    .. param:: psa_algorithm_t alg
        The permitted algorithm to write.

    .. return:: void

    The permitted-algorithm policy of a key encodes which algorithm or algorithms are permitted to be used with this key.

    This function overwrites any permitted-algorithm policy previously set in ``attributes``.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.

.. function:: psa_get_key_algorithm

    .. summary::
        Retrieve the permitted-algorithm policy from key attributes.

    .. param:: const psa_key_attributes_t * attributes
        The key attribute object to query.

    .. return:: psa_algorithm_t
        The algorithm stored in the attribute object.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.


.. _key-usage-flags:

Key usage flags
---------------

The usage flags are encoded in a bitmask, which has the type `psa_key_usage_t`. There are two kinds of usage flag:

1.  Key-management usage flags.

    -   The extractable flag `PSA_KEY_USAGE_EXPORT` determines whether the key material can be extracted from the cryptoprocessor, or copied outside of its current security boundary.
    -   The copyable flag `PSA_KEY_USAGE_COPY` determines whether the key material can be copied into a new key, which can have a different lifetime or a more restrictive policy.
    -   The cacheable flag `PSA_KEY_USAGE_CACHE` determines whether the implementation is permitted to retain non-essential copies of the key material in RAM. This policy only applies to persistent keys. See also :secref:`key-material`.

2.  Cryptographic-operation usage flags.

    The following usage flags determine whether the corresponding cryptographic operations are permitted with the key:

    -   `PSA_KEY_USAGE_ENCRYPT`
    -   `PSA_KEY_USAGE_DECRYPT`
    -   `PSA_KEY_USAGE_SIGN_MESSAGE`
    -   `PSA_KEY_USAGE_VERIFY_MESSAGE`
    -   `PSA_KEY_USAGE_SIGN_HASH`
    -   `PSA_KEY_USAGE_VERIFY_HASH`
    -   `PSA_KEY_USAGE_DERIVE`
    -   `PSA_KEY_USAGE_VERIFY_DERIVATION`
    -   `PSA_KEY_USAGE_WRAP`
    -   `PSA_KEY_USAGE_UNWRAP`

    The flag `PSA_KEY_USAGE_DERIVE_PUBLIC` is used in the function `psa_check_key_usage()` to query if a key can be used for the public role in the specified algorithm.

.. typedef:: uint32_t psa_key_usage_t

    .. summary::
        Encoding of permitted usage on a key.

.. macro:: PSA_KEY_USAGE_EXPORT
    :definition: ((psa_key_usage_t)0x00000001)

    .. summary::
        Permission to export the key.

    This key-management usage flag permits a key to be moved outside of the security boundary of its current storage location. In particular:

    *   This flag is required to export a key from the cryptoprocessor using `psa_export_key()`. A public key or the public part of a key pair can always be exported regardless of the value of this permission flag.

    *   This flag can also be required to make a copy of a key outside of a secure element using `psa_copy_key()`. See also `PSA_KEY_USAGE_COPY`.

    If a key does not have export permission, implementations must not permit the key to be exported in plain form from the cryptoprocessor, whether through `psa_export_key()` or through a proprietary interface. The key might still be exportable in a wrapped form, i.e. in a form where it is encrypted by another key.

.. macro:: PSA_KEY_USAGE_COPY
    :definition: ((psa_key_usage_t)0x00000002)

    .. summary::
        Permission to copy the key.

    This key-management usage flag is required to make a copy of a key using `psa_copy_key()`.

    For a key lifetime that corresponds to a secure element location that enforces the non-exportability of keys, copying a key outside the secure element also requires the usage flag `PSA_KEY_USAGE_EXPORT`. Copying the key within the secure element is permitted with just `PSA_KEY_USAGE_COPY`, if the secure element supports it. For keys with the lifetime `PSA_KEY_LIFETIME_VOLATILE` or `PSA_KEY_LIFETIME_PERSISTENT`, the usage flag `PSA_KEY_USAGE_COPY` is sufficient to permit the copy.

.. macro:: PSA_KEY_USAGE_CACHE
    :definition: ((psa_key_usage_t)0x00000004)

    .. summary::
        Permission for the implementation to cache the key.

    This key-management usage flag permits the implementation to make additional copies of the key material that are not in storage and not for the purpose of an ongoing operation. Applications can use it as a hint for the cryptoprocessor, to keep a copy of the key around for repeated access.

    An application can request that cached key material is removed from memory by calling `psa_purge_key()`.

    The presence of this usage flag when creating a key is a hint:

    *   An implementation is not required to cache keys that have this usage flag.
    *   An implementation must not report an error if it does not cache keys.

    If this usage flag is not present, the implementation must ensure key material is removed from memory as soon as it is not required for an operation, or for maintenance of a volatile key.

    This flag must be preserved when reading back the attributes for all keys, regardless of key type or implementation behavior.

    See also :secref:`key-material`.

.. macro:: PSA_KEY_USAGE_ENCRYPT
    :definition: ((psa_key_usage_t)0x00000100)

    .. summary::
        Permission to encrypt a message, or perform key encapsulation, with the key.

    This cryptographic-operation usage flag is required to use the key in a symmetric encryption operation, in an AEAD encryption-and-authentication operation, in an asymmetric encryption operation, or in a key-encapsulation operation. The flag must be present on keys used with the following APIs:

    *   `psa_cipher_encrypt()`
    *   `psa_cipher_encrypt_setup()`
    *   `psa_aead_encrypt()`
    *   `psa_aead_encrypt_setup()`
    *   `psa_asymmetric_encrypt()`
    *   `psa_encapsulate()`

    For a key pair, this concerns the public key.

.. macro:: PSA_KEY_USAGE_DECRYPT
    :definition: ((psa_key_usage_t)0x00000200)

    .. summary::
        Permission to decrypt a message, or perform key decapsulation, with the key.

    This cryptographic-operation usage flag is required to use the key in a symmetric decryption operation, in an AEAD decryption-and-verification operation, in an asymmetric decryption operation, or in a key-decapsulation operation. The flag must be present on keys used with the following APIs:

    *   `psa_cipher_decrypt()`
    *   `psa_cipher_decrypt_setup()`
    *   `psa_aead_decrypt()`
    *   `psa_aead_decrypt_setup()`
    *   `psa_asymmetric_decrypt()`
    *   `psa_decapsulate()`

    For a key pair, this concerns the private key.

.. macro:: PSA_KEY_USAGE_SIGN_MESSAGE
    :definition: ((psa_key_usage_t)0x00000400)

    .. summary::
        Permission to sign a message with the key.

    This cryptographic-operation usage flag is required to use the key in a MAC calculation operation, or in an asymmetric message signature operation. The flag must be present on keys used with the following APIs:

    *   `psa_mac_compute()`
    *   `psa_mac_sign_setup()`
    *   `psa_sign_message()`
    *   `psa_sign_message_with_context()`
    *   `psa_sign_setup()`
    *   `psa_sign_iop_setup()`, when signing a message.

    For a key pair, this concerns the private key.

.. macro:: PSA_KEY_USAGE_VERIFY_MESSAGE
    :definition: ((psa_key_usage_t)0x00000800)

    .. summary::
        Permission to verify a message signature with the key.

    This cryptographic-operation usage flag is required to use the key in a MAC verification operation, or in an asymmetric message signature verification operation. The flag must be present on keys used with the following APIs:

    *   `psa_mac_verify()`
    *   `psa_mac_verify_setup()`
    *   `psa_verify_message()`
    *   `psa_verify_message_with_context()`
    *   `psa_verify_setup()`
    *   `psa_verify_iop_setup()`, when verifying the signature of a message.

    For a key pair, this concerns the public key.

.. macro:: PSA_KEY_USAGE_SIGN_HASH
    :definition: ((psa_key_usage_t)0x00001000)

    .. summary::
        Permission to sign a message hash with the key.

    This cryptographic-operation usage flag is required to use the key to sign a pre-computed message hash in an asymmetric signature operation. The flag must be present on keys used with the following APIs:

    *   `psa_sign_hash()`
    *   `psa_sign_hash_with_context()`
    *   `psa_sign_iop_setup()` when signing a pre-computed hash.

    This flag automatically sets `PSA_KEY_USAGE_SIGN_MESSAGE`: if an application sets the flag `PSA_KEY_USAGE_SIGN_HASH` when creating a key, then the key always has the permissions conveyed by `PSA_KEY_USAGE_SIGN_MESSAGE`, and the flag `PSA_KEY_USAGE_SIGN_MESSAGE` will also be present when the application queries the usage flags of the key.

    For a key pair, this concerns the private key.

.. macro:: PSA_KEY_USAGE_VERIFY_HASH
    :definition: ((psa_key_usage_t)0x00002000)

    .. summary::
        Permission to verify a message hash with the key.

    This cryptographic-operation usage flag is required to use the key to verify a pre-computed message hash in an asymmetric signature verification operation. The flag must be present on keys used with the following APIs:

    *   `psa_verify_hash()`
    *   `psa_verify_hash_with_context()`
    *   `psa_verify_iop_setup()`, when verifying the signature of a pre-computed hash.

    This flag automatically sets `PSA_KEY_USAGE_VERIFY_MESSAGE`: if an application sets the flag `PSA_KEY_USAGE_VERIFY_HASH` when creating a key, then the key always has the permissions conveyed by `PSA_KEY_USAGE_VERIFY_MESSAGE`, and the flag `PSA_KEY_USAGE_VERIFY_MESSAGE` will also be present when the application queries the usage flags of the key.

    For a key pair, this concerns the public key.

.. macro:: PSA_KEY_USAGE_DERIVE
    :definition: ((psa_key_usage_t)0x00004000)

    .. summary::
        Permission to derive other keys or produce a password hash from this key.

    This cryptographic-operation usage flag is required to use the key for derivation in a key-derivation operation, or in a key-agreement operation.

    This flag must be present on keys used with the following APIs:

    *   `psa_key_agreement()`
    *   `psa_key_agreement_iop_setup()`
    *   `psa_key_derivation_key_agreement()`
    *   `psa_raw_key_agreement()`

    If this flag is present on all keys used in calls to `psa_key_derivation_input_key()` for a key-derivation operation, then it permits calling `psa_key_derivation_output_bytes()`, `psa_key_derivation_output_key()`, `psa_key_derivation_output_key_custom()`, `psa_key_derivation_verify_bytes()`, or `psa_key_derivation_verify_key()` at the end of the operation.

.. macro:: PSA_KEY_USAGE_VERIFY_DERIVATION
    :definition: ((psa_key_usage_t)0x00008000)

    .. summary::
        Permission to verify the result of a key derivation, including password hashing.

        .. versionadded:: 1.1

    This cryptographic-operation usage flag is required to use the key for verification in a key-derivation operation.

    This flag must be present on keys used with `psa_key_derivation_verify_key()`.

    If this flag is present on all keys used in calls to `psa_key_derivation_input_key()` for a key-derivation operation, then it permits calling `psa_key_derivation_verify_bytes()` or `psa_key_derivation_verify_key()` at the end of the operation.

.. macro:: PSA_KEY_USAGE_DERIVE_PUBLIC
    :definition: ((psa_key_usage_t)0x00000080)

    .. summary::
        Used in the `psa_check_key_usage()` function to determine if the key can be used in the public key role in a key-agreement or a PAKE operation.

        .. versionadded:: 1.4

    This cryptographic-operation usage flag is only used with the `psa_check_key_usage()` function.
    This flag is not currently checked when performing any cryptographic operation.

    For example, calling `psa_check_key_usage()` with `PSA_KEY_USAGE_DERIVE_PUBLIC` and with:

    *   `PSA_ALG_ECDH` checks that the key can be used as the public share in the ECDH key agreement.
        There are no checks on permissions as the key share is provided in a buffer.
    *   `PSA_ALG_SPAKE2P_HMAC` will check that the key can be used in the Verifier role in the SPAKE2+ algorithm.
        The key must have the `PSA_KEY_USAGE_DERIVE` permission.
    *   `PSA_ALG_HKDF` is invalid, as there is no such role in single-key derivation algorithms.

.. macro:: PSA_KEY_USAGE_WRAP
    :definition: ((psa_key_usage_t)0x00010000)

    .. summary::
        Permission to wrap another key with the key.

    This flag is required to use the key in a key-wrapping operation.
    The flag must be present on keys used with the following APIs:

    *   `psa_wrap_key()`

.. macro:: PSA_KEY_USAGE_UNWRAP
    :definition: ((psa_key_usage_t)0x00020000)

    .. summary::
        Permission to unwrap another key with the key.

    This flag is required to use the key in a key-unwrapping operation.
    The flag must be present on keys used with the following APIs:

    *   `psa_unwrap_key()`

.. function:: psa_set_key_usage_flags

    .. summary::
        Declare usage flags for a key.

    .. param:: psa_key_attributes_t * attributes
        The attribute object to write to.
    .. param:: psa_key_usage_t usage_flags
        The usage flags to write.

    .. return:: void

    Usage flags are part of a key's policy. They encode what kind of operations are permitted on the key. For more details, see :secref:`key-policy`.

    This function overwrites any usage flags previously set in ``attributes``.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.


.. function:: psa_get_key_usage_flags

    .. summary::
        Retrieve the usage flags from key attributes.

    .. param:: const psa_key_attributes_t * attributes
        The key attribute object to query.

    .. return:: psa_key_usage_t
        The usage flags stored in the attribute object.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.

.. function::  psa_check_key_usage

    .. summary::
        Query the capability of a key.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t key
        Identifier of the key to check.
    .. param:: psa_algorithm_t alg
        An algorithm identifier: a value of type `psa_algorithm_t`.
    .. param:: psa_key_usage_t usage
        A single ``PSA_KEY_USAGE_xxx`` flag.

    .. return:: psa_status_t

    .. retval:: PSA_SUCCESS
        ``key`` can be used for the requested operation on this implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``usage`` is a key-management usage flag and ``alg`` is not `PSA_ALG_NONE`.
        *   ``usage`` is a cryptographic-operation usage flag and ``alg`` is not a valid, specific algorithm.
            A 'specific algorithm' is one that is neither `PSA_ALG_NONE` nor a wildcard algorithm.
        *   ``usage`` is not a valid role for algorithm ``alg``.
        *   ``key`` is not compatible with ``alg`` and ``usage``.
    .. retval:: PSA_ERROR_INVALID_HANDLE:
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        ``key`` does not permit the requested usage or algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   The implementation does not support algorithm ``alg``.
        *   The implementation does not support using ``key`` with the operation associated with ``alg`` and ``usage``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    This function reports whether the implementation supports the use of a key with the operation associated with a provided algorithm and usage.
    This function does not attempt to perform the operation.

    If ``usage`` is a key-management usage flag, then:

    *   ``alg`` must be `PSA_ALG_NONE`.
    *   ``key`` must exist, and permit the requested usage flag.

    If ``usage`` is a cryptographic-operation usage flag, then:

    *   ``alg`` must be a valid, fully specified algorithm, and not a wildcard.
        For example:

        -   :code:`PSA_ALG_ECDSA(PSA_ALG_ANY_HASH)` is invalid as it is a wildcard algorithm.
        -   :code:`PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 9)` is invalid as it has an invalid tag-length for GCM.
        -   :code:`PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_1)` is invalid as SPAKE2+ does have SHA-1 in any cipher-suite.
    *   ``usage`` must identify a valid role within the algorithm.
        For example, if :code:`alg == PSA_ALG_GCM`, the ``usage`` must be either `PSA_KEY_USAGE_ENCRYPT` or `PSA_KEY_USAGE_DECRYPT`, as these are the key-usage policy flags for AEAD functions.
    *   ``key`` must exist, have a type and size that are compatible with the operation associated with ``alg`` and ``usage``, and have the required permission for the algorithm and usage.
        For example:

        -   An Edwards25519 key pair is not compatible with :code:`PSA_ALG_ECDSA(PSA_ALG_SHA_256)`.
        -   A ``512``-bit RSA key pair is not compatible with :code:`PSA_ALG_RSA_OAEP(PSA_ALG_SHA_512)` as the algorithm requires a larger key size.
        -   A ``512``-bit AES key (double-length key for use in AES-256-XTS) is not compatible with `PSA_ALG_CTR`.

    .. note::
        For the key pair or public key of a valid type in a key agreement function, this function returns :code:`PSA_SUCCESS` for the usage `PSA_KEY_USAGE_DERIVE_PUBLIC`, regardless of the key's policy.
        This is because the corresponding API functions take a key buffer as input, not a key object, and the key data can extracted by calling `psa_export_public_key()`, which does not require any usage flag.

    .. admonition:: Implementation note

        The intended behavior of this function is to include any check that can be made using the accessible key attributes, but without requiring logic or arithmetic using the key material.
