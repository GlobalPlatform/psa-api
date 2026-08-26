.. SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 245

.. _key-wrapping:

Key wrapping
============

Key wrapping is the process of encrypting a key, so that the resulting ciphertext can be stored, or transported, in a form that maintains the confidentiality of the key material.
Key unwrapping reverses this process, extracting the key from the ciphertext.
Some key-wrapping schemes also provide integrity protection, to ensure that modification of the ciphertext can be detected.

Some key-wrapping algorithms operate on arbitrary data, and provide authenticated encryption that is specifically designed for key values.
For example, the AES Key-wrap algorithm AES-KW.
For this type of algorithm, the |API| provides a simple pair of functions, `psa_unwrap_key()` and `psa_wrap_key()`, that unwrap or wrap key data in the default export format.
When using one of these key-wrapping algorithms, the key attributes are managed by the application.

.. note::
    Other key-wrapping schemes define both the format of the wrapped key material and the algorithm that is used to perform the wrapping.
    For example PKCS#8 defines *EncryptedPrivateKeyInfo*, which is also described in :rfc-title:`5958`.
    Wrapped-key formats typically encode the key type and wrapping algorithm within the output data, and can also include other key attributes.
    This version of the |API| does not support these key-wrapping schemes, but this is planned for a future version.

.. rationale::

    Key-wrapping algorithms are categorized separately to other authenticated encryption algorithms in the |API|. Key-wrapping algorithms ideally have the following properties:

    *   Deterministic --- not requiring a nonce or IV to be provided by the application, or generated randomly.
    *   Robust --- every bit of plaintext can affect every bit of ciphertext.

    As a result, key-wrapping algorithms are typically special-purpose authenticated encryption algorithms.

.. _key-wrapping-algorithms:

Key-wrapping algorithms
-----------------------

.. macro:: PSA_ALG_KW
    :definition: ((psa_algorithm_t)0x0B400100)

    .. summary::
        A key-wrapping algorithm based on the NIST Key Wrap (KW) mode of a block cipher.

        .. versionadded:: 1.4

    KW is defined for block ciphers that have a 128-bit block size.
    The underlying block cipher is determined by the key type.

    Keys to be wrapped must have a length equal to a multiple of the 'semi-block' size for the block cipher.
    That is, a multiple of 8 bytes.

    To wrap keys that are not a multiple of the semi-block size, `PSA_ALG_KWP` can be used.

    This is the NIST Key Wrap algorithm, using any block-cipher that operates on 128-bit blocks, as defined in :cite-title:`SP800-38F`.
    A definition of AES-KW is also found in :rfc-title:`3394`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_KWP
    :definition: ((psa_algorithm_t)0x0BC00200)

    .. summary::
        A key-wrapping algorithm based on the NIST Key Wrap with Padding (KWP) mode of a block cipher.

        .. versionadded:: 1.4

    KWP is defined for block ciphers that have a 128-bit block size.
    The underlying block cipher is determined by the key type.

    This algorithm can wrap a key of any length.

    This is the NIST Key Wrap with Padding algorithm, using any block-cipher that operates on 128-bit blocks, as defined in :cite-title:`SP800-38F`.
    A definition of AES-KWP is also found in :rfc-title:`5649`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

Key wrapping functions
----------------------

.. function:: psa_unwrap_key

    .. summary::
        Unwrap and import a key using a specified wrapping key.

        .. versionadded:: 1.4

    .. param:: const psa_key_attributes_t * attributes
        The attributes for the new key.

        The following attributes are required for all keys:

        *   The key type determines how the decrypted ``data`` buffer is interpreted.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default volatile lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        The following attributes are optional:

        *   If the key size is nonzero, it must be equal to the key size determined from ``data``.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.
    .. param:: psa_key_id_t wrapping_key
        Identifier of the key to use for the unwrapping operation.
        It must permit the usage `PSA_KEY_USAGE_UNWRAP`.
    .. param:: psa_algorithm_t alg
        The key-wrapping algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_WRAP(alg)` is true.
    .. param:: const uint8_t * data
        Buffer containing the wrapped key data.
        The content of this buffer is unwrapped using the algorithm ``alg``, and then interpreted according to the type declared in ``attributes``.
    .. param:: size_t data_length
        Size of the ``data`` buffer in bytes.
    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key.
        `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The wrapped key data could not be authenticated.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``wrapping_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a key-wrapping algorithm.
        *   ``wrapping_key`` is not supported for use with ``alg``.
        *   The key attributes, as a whole, are not supported, either by the implementation in general or in the specified storage location.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a key-wrapping algorithm.
        *   ``wrapping_key`` is not compatible with ``alg``.
        *   The key type is invalid.
        *   The key size is nonzero, and is incompatible with the wrapped key data in ``data``.
        *   The key lifetime is invalid.
        *   The key identifier is not valid for the key lifetime.
        *   The key usage flags include invalid values.
        *   The key's permitted-usage algorithm is invalid.
        *   The key attributes, as a whole, are invalid.
        *   The key data is not correctly formatted for the key type.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *    The wrapping key does not have the `PSA_KEY_USAGE_UNWRAP` flag, or it does not permit the requested algorithm.
        *    The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    The key is unwrapped and extracted from the provided ``data`` buffer. Its location, policy, and type are taken from ``attributes``.

    The wrapped key data determines the key size.
    :code:``psa_get_key_bits(attributes)`` must either match the determined key size or be ``0``.

    Implementations must reject an attempt to unwrap a key if the determined key size is ``0``.

    .. note::
        A call to `psa_unwrap_key()` first applies the decryption procedure associated with the key-wrapping algorithm ``alg``, using the ``wrapping_key`` key, to the supplied ``data`` buffer.
        The resulting plaintext is retained within the cryptoprocessor, and used with the provided ``attributes`` to create a key, as if they were inputs to `psa_import_key()`.

    .. note::
        The |API| does not support asymmetric private key objects outside of a key pair.
        When unwrapping a private key, the corresponding key-pair type is created.
        If the imported key data does not contain the public key, then the implementation will reconstruct the public key from the private key as needed.

    .. admonition:: Implementation note

        It is recommended that the implementation supports unwrapping any key data that can be produced by a call to `psa_wrap_key()`, with the same key-wrapping algorithm and key, and matching key attributes.

        It is recommended that implementations reject wrapped key data if it might be erroneous, for example, if it is the wrong type or is truncated.

.. function:: psa_wrap_key

    .. summary::
        Wrap and export a key using a specified wrapping key.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t wrapping_key
        Identifier of the key to use for the wrapping operation.
        It must permit the usage `PSA_KEY_USAGE_WRAP`.
    .. param:: psa_algorithm_t alg
        The key-wrapping algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_WRAP(alg)` is true.
    .. param:: psa_key_id_t key
        Identifier of the key to wrap.
        It must permit the usage `PSA_KEY_USAGE_EXPORT`.
    .. param:: uint8_t * data
        Buffer where the wrapped key data is to be written.
    .. param:: size_t data_size
        Size of the ``data`` buffer in bytes.
        This must be appropriate for the key:

        *   The required output size is :code:`PSA_WRAP_KEY_OUTPUT_SIZE(wrap_key_type, alg, type, bits)`, where ``wrap_key_type`` is the type of the wrapping key, ``alg`` is the key-wrapping algorithm, ``type`` is the type of the key being wrapped, and ``bits`` is the bit-size of the key being wrapped.
        *   `PSA_WRAP_KEY_PAIR_MAX_SIZE` evaluates to the maximum wrapped output size of any supported key pair, in any supported combination of key-wrapping algorithm and wrapping-key type.
        *   This API defines no maximum size for wrapped symmetric keys.
            Arbitrarily large data items can be stored in the key store, for example certificates that correspond to a stored private key or input material for key derivation.
    .. param:: size_t * data_length
        On success, the number of bytes that make up the wrapped key data.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*data_length)`` bytes of ``data`` contain the wrapped key.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        The following conditions can result in this error:

        *   ``wrapping_key`` is not a valid key identifier.
        *   ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   The wrapping key does not have the `PSA_KEY_USAGE_WRAP` flag, or it does not permit the requested algorithm.
        *   The key to be wrapped does not have the `PSA_KEY_USAGE_EXPORT` flag.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a key-wrapping algorithm.
        *   ``wrapping_key`` is not compatible with ``alg``.
        *   ``key`` has a size that is not valid for ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a key-wrapping algorithm.
        *   ``wrapping_key`` is not supported for use with ``alg``.
        *   The storage location of ``key`` does not support export of the key.
        *   The implementation does not support export of keys with the type of ``key``.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``data`` buffer is too small.
        `PSA_WRAP_KEY_OUTPUT_SIZE()` or `PSA_WRAP_KEY_PAIR_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Wrap a key from the key store into a data buffer using a specified key-wrapping algorithm and key-wrapping key.
    On success, the output contains the wrapped key value.
    The policy of the key to be wrapped must have the usage flag `PSA_KEY_USAGE_EXPORT` set.

    The output of this function can be passed to `psa_unwrap_key()`, specifying the same algorithm and wrapping key, with the same attributes as ``key``, to create an equivalent key object.

    .. note::
        A call to `psa_wrap_key()` first evaluates the key data for ``key``, as if `psa_export_key()` is called, but retaining the key data within the cryptoprocessor.
        If this succeeds, the encryption procedure associated with the key-wrapping algorithm ``alg``, using the ``wrapping_key`` key, is applied to the key data.
        The resulting ciphertext is then returned.


Support macros
--------------

.. macro:: PSA_WRAP_KEY_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_wrap_key()`.

        .. versionadded:: 1.4

    .. param:: wrap_key_type
       A supported key-wrapping key type.
    .. param:: alg
       A supported key-wrapping algorithm.
    .. param:: key_type
        A supported key type.
    .. param:: key_bits
        The size of the key in bits.

    .. return::
        If the parameters are valid and supported, return a buffer size in bytes that guarantees that `psa_wrap_key()` will not fail with :code:`PSA_ERROR_BUFFER_TOO_SMALL`. If the parameters are a valid combination that is not supported by the implementation, this macro must return either a sensible size or ``0``. If the parameters are not valid, the return value is unspecified.

    See also `PSA_WRAP_KEY_PAIR_MAX_SIZE`.

.. macro:: PSA_WRAP_KEY_PAIR_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient buffer size for wrapping any asymmetric key pair.

        .. versionadded:: 1.4

    This value must be a sufficient buffer size when calling `psa_wrap_key()` to export any asymmetric key pair that is supported by the implementation, regardless of the exact key type and key size.

    See also `PSA_WRAP_KEY_OUTPUT_SIZE()`.
