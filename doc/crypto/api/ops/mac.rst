.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 220

.. _macs:

Message authentication codes (MAC)
==================================

The single-part MAC functions are:

*   `psa_mac_compute()` to calculate the MAC of a message.
*   `psa_mac_verify()` to compare the MAC of a message with a reference value.

The `psa_mac_operation_t` `multi-part operation <multi-part-operations>` allows messages to be processed in fragments. A multi-part MAC operation is used as follows:

1.  Initialize the `psa_mac_operation_t` object to zero, or by assigning the value of the associated macro `PSA_MAC_OPERATION_INIT`.
#.  Call `psa_mac_sign_setup()` or `psa_mac_verify_setup()` to specify the algorithm and key.
#.  Call the `psa_mac_update()` function on successive chunks of the message.
#.  At the end of the message, call the required finishing function:

    -   To calculate the MAC of the message, call `psa_mac_sign_finish()`.
    -   To verify the MAC of the message against a reference value, call `psa_mac_verify_finish()`.

To abort the operation or recover from an error, call `psa_mac_abort()`.

.. _mac-algorithms:

MAC algorithms
--------------

.. macro:: PSA_ALG_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an HMAC message-authentication-code algorithm from an underlying hash algorithm.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

        See `below <hash-for-hmac_>`_ on selecting a hash algorithm for use with HMAC.

    .. return::
        The corresponding HMAC algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    For example, :code:`PSA_ALG_HMAC(PSA_ALG_SHA_256)` is HMAC-SHA-256.

    The HMAC construction is defined in :RFC-title:`2104`.

    .. _hash-for-hmac:

    .. subsection:: Choice of hash algorithm

        An HMAC block size must be defined for use with each hash algorithm, which is at least as large as the hash output size.

        HMAC was designed for hashes that use a Merkle-Damgård construction, for example, MD5, SHA-1, and SHA-2.
        For these hash algorithms, the HMAC block size is defined to be the hash input-block size.

        Some algorithms do not have a defined HMAC block size.
        For example, Ascon (`PSA_ALG_ASCON_HASH256`) or Shake-based hashes (`PSA_ALG_SHAKE256_512`).

        :numref:`tab-hmac-hash` lists the valid hash algorithms for use with HMAC, and their HMAC block and output sizes in bytes.

        .. csv-table:: Hash algorithms that can be used with HMAC
            :name: tab-hmac-hash
            :header-rows: 1
            :class: longtable

            Algorithm, HMAC block size, Output size
            `PSA_ALG_AES_MMO_ZIGBEE`, 16, 16
            `PSA_ALG_MD2`, 16, 16
            `PSA_ALG_MD4`, 64, 16
            `PSA_ALG_MD5`, 64, 16
            `PSA_ALG_RIPEMD160`, 64, 20
            `PSA_ALG_SHA_1`, 64, 20
            `PSA_ALG_SHA_224`, 64, 28
            `PSA_ALG_SHA_256`, 64, 32
            `PSA_ALG_SHA_384`, 128, 48
            `PSA_ALG_SHA_512`, 128, 64
            `PSA_ALG_SHA_512_224`, 128, 28
            `PSA_ALG_SHA_512_256`, 128, 32
            `PSA_ALG_SHA3_224`, 144, 28
            `PSA_ALG_SHA3_256`, 136, 32
            `PSA_ALG_SHA3_384`, 104, 48
            `PSA_ALG_SHA3_512`, 72, 64
            `PSA_ALG_SM3`, 64, 32
            `PSA_ALG_BLAKE2S_HASH256`, 64, 32
            `PSA_ALG_BLAKE2B_HASH512`, 128, 64

        .. admonition:: Implementation note

            The current version of this specification does not specify the behavior of `PSA_ALG_HMAC` with hash algorithms that are absent from :numref:`tab-hmac-hash`.

            It is recommended that these hash algorithms are not supported with `PSA_ALG_HMAC`, or with other compound algorithms based on HMAC.
            The other compound algorithms include `PSA_ALG_DETERMINISTIC_ECDSA`, `PSA_ALG_HKDF`, `PSA_ALG_HKDF_EXTRACT`, `PSA_ALG_HKDF_EXPAND`, `PSA_ALG_SP800_108_COUNTER_HMAC`, `PSA_ALG_PBKDF2_HMAC`.

            Future versions of the |API| might specify HMAC support for these hash algorithms, and will define the block size to use for HMAC.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_HMAC`

.. macro:: PSA_ALG_CBC_MAC
    :definition: ((psa_algorithm_t)0x03c00100)

    .. summary::
        The CBC-MAC message-authentication-code algorithm, constructed over a block cipher.

    .. warning::
        CBC-MAC is insecure in many cases. A more secure mode, such as `PSA_ALG_CMAC`, is recommended.

    The CBC-MAC algorithm must be used with a key for a block cipher. For example, one of `PSA_KEY_TYPE_AES`.

    CBC-MAC is defined as *MAC Algorithm 1* in :cite-title:`ISO9797`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CMAC
    :definition: ((psa_algorithm_t)0x03c00200)

    .. summary::
        The CMAC message-authentication-code algorithm, constructed over a block cipher.

    The CMAC algorithm must be used with a key for a block cipher. For example, when used with a key with type `PSA_KEY_TYPE_AES`, the resulting operation is AES-CMAC.

    CMAC is defined in :cite-title:`SP800-38B`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_BLAKE2_MAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an BLAKE2 message-authentication-code algorithm from a BLAKE2 hash algorithm.

        .. versionadded:: 1.5

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_BLAKE2_HASH(hash_alg)` is true.

    .. return::
        The corresponding BLAKE2 MAC algorithm.

        Unspecified if ``hash_alg`` is not a supported BLAKE2 hash algorithm.

    For example, :code:`PSA_ALG_BLAKE2_MAC(PSA_ALG_BLAKE2S_HASH256)` is BLAKE2s-MAC.

    BLAKE2 MAC is a BLAKE2 keyed-hash with a non-zero-length key.
    This macro is used to construct the BLAKE2 MAC for BLAKE2s, BLAKE2sp, BLAKE2b, and BLAKE2bp, by passing the appropriate BLAKE2 hash algorithm identifier as ``hash_alg``.

    BLAKE2s and BLAKE2b are defined in :rfc-title:`7693`. BLAKE2sp and BLAKE2bp are defined in :cite-title:`BLAKE2`.

    The BLAKE2 MAC algorithms must be used with a `PSA_KEY_TYPE_BLAKE2` key:

    *   For BLAKE2s and BLAKE2sp, the key must not be more than 256 bits (32 bytes) in length.
    *   For BLAKE2b and BLAKE2bp, the key must not be more than 512 bits (64 bytes) in length.

    The default tag length for BLAKE2 MACs is the output length of the underlying BLAKE2 hash:

    *   For BLAKE2s and BLAKE2sp, the default and maximum tag length is 32 bytes.
    *   For BLAKE2b and BLAKE2bp, the default and maximum tag length is 64 bytes.

    BLAKE2 permits any non-zero tag length from one to the maximum tag size.
    To select a non-default tag length ``tag_len``, use :code:`PSA_ALG_TRUNCATED_MAC(PSA_ALG_BLAKE2_MAC(blake2_hash), tag_len)` as the algorithm.

    .. note::
        To use BLAKE2 as a cryptographic hash, see `PSA_ALG_BLAKE2S_HASH256`, `PSA_ALG_BLAKE2SP_HASH256`, `PSA_ALG_BLAKE2B_HASH512`, and `PSA_ALG_BLAKE2BP_HASH512`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_BLAKE2`

.. macro:: PSA_ALG_TRUNCATED_MAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a truncated MAC algorithm.

    .. param:: mac_alg
        A MAC algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(mac_alg)` is true. This can be a truncated or untruncated MAC algorithm.
    .. param:: mac_length
        Desired length of the truncated MAC in bytes. This must be at most the untruncated length of the MAC and must be at least an implementation-specified minimum. The implementation-specified minimum must not be zero.

    .. return::
        The corresponding MAC algorithm with the specified length.

        Unspecified if ``mac_alg`` is not a supported MAC algorithm or if ``mac_length`` is too small or too large for the specified MAC algorithm.

    A truncated MAC algorithm is identical to the corresponding MAC algorithm except that the MAC value for the truncated algorithm consists of only the first ``mac_length`` bytes of the MAC value for the untruncated algorithm.

    .. note::
        This macro might allow constructing algorithm identifiers that are not valid, either because the specified length is larger than the untruncated MAC or because the specified length is smaller than permitted by the implementation.

    .. note::
        It is implementation-defined whether a truncated MAC that is truncated to the same length as the MAC of the untruncated algorithm is considered identical to the untruncated algorithm for policy comparison purposes.

    The untruncated MAC algorithm can be recovered using `PSA_ALG_FULL_LENGTH_MAC()`.

    .. subsection:: Compatible key types

        The resulting truncated MAC algorithm is compatible with the same key types as the MAC algorithm used to construct it.

.. macro:: PSA_ALG_FULL_LENGTH_MAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to construct the MAC algorithm with an untruncated MAC, from a truncated MAC algorithm.

    .. param:: mac_alg
        A MAC algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(mac_alg)` is true. This can be a truncated or untruncated MAC algorithm.

    .. return::
        The corresponding MAC algorithm with an untruncated MAC.

        Unspecified if ``mac_alg`` is not a supported MAC algorithm.

    .. subsection:: Compatible key types

        The resulting untruncated MAC algorithm is compatible with the same key types as the MAC algorithm used to construct it.

.. macro:: PSA_ALG_AT_LEAST_THIS_LENGTH_MAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a MAC minimum-MAC-length wildcard algorithm.

        .. versionadded:: 1.1

    .. param:: mac_alg
        A MAC algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true. This can be a truncated or untruncated MAC algorithm.
    .. param:: min_mac_length
        Desired minimum length of the message authentication code in bytes. This must be at most the untruncated length of the MAC and must be at least ``1``.

    .. return::
        The corresponding MAC wildcard algorithm with the specified minimum MAC length.

        Unspecified if ``mac_alg`` is not a supported MAC algorithm or if ``min_mac_length`` is less than ``1`` or too large for the specified MAC algorithm.

    A key with a minimum-MAC-length MAC wildcard algorithm as permitted-algorithm policy can be used with all MAC algorithms sharing the same base algorithm, and where the (potentially truncated) MAC length of the specific algorithm is equal to or larger then the wildcard algorithm's minimum MAC length.

    ..  note::
        When setting the minimum required MAC length to less than the smallest MAC length permitted by the base algorithm, this effectively becomes an 'any-MAC-length-permitted' policy for that base algorithm.

    The untruncated MAC algorithm can be recovered using `PSA_ALG_FULL_LENGTH_MAC()`.

    .. subsection:: Compatible key types

        The resulting wildcard MAC algorithm is compatible with the same key types as the MAC algorithm used to construct it.


Single-part MAC functions
-------------------------

.. function:: psa_mac_compute

    .. summary::
        Calculate the message authentication code (MAC) of a message.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must permit the usage `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        The MAC algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the input message.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * mac
        Buffer where the MAC value is to be written.
    .. param:: size_t mac_size
        Size of the ``mac`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The exact MAC size is :code:`PSA_MAC_LENGTH(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are attributes of the key used to compute the MAC.
        *   `PSA_MAC_MAX_SIZE` evaluates to the maximum MAC size of any supported MAC algorithm.

    .. param:: size_t * mac_length
        On success, the number of bytes that make up the MAC value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*mac_length)`` bytes of ``mac`` contain the MAC value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a MAC algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   ``input_length`` is too large for ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a MAC algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``mac`` buffer is too small.
        `PSA_MAC_LENGTH()` or `PSA_MAC_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    .. note::
        To verify the MAC of a message against an expected value, use `psa_mac_verify()` instead. Beware that comparing integrity or authenticity data such as MAC values with a function such as ``memcmp()`` is risky because the time taken by the comparison might leak information about the MAC value which could allow an attacker to guess a valid MAC and thereby bypass security controls.

.. function:: psa_mac_verify

    .. summary::
        Calculate the MAC of a message and compare it with a reference value.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must permit the usage `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        The MAC algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the input message.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * mac
        Buffer containing the expected MAC value.
    .. param:: size_t mac_length
        Size of the ``mac`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The expected MAC is identical to the actual MAC of the input.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The calculated MAC of the message does not match the value in ``mac``.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a MAC algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   ``input_length`` is too large for ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a MAC algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

Multi-part MAC operations
-------------------------

.. typedef:: /* implementation-defined type */ psa_mac_operation_t

    .. summary::
        The type of the state object for multi-part MAC operations.

    Before calling any function on a MAC operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_mac_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_mac_operation_t operation;

    *   Initialize the object to the initializer `PSA_MAC_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;

    *   Assign the result of the function `psa_mac_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_mac_operation_t operation;
            operation = psa_mac_operation_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_MAC_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a MAC operation object of type `psa_mac_operation_t`.

.. function:: psa_mac_operation_init

    .. summary::
        Return an initial value for a MAC operation object.

    .. return:: psa_mac_operation_t

.. function:: psa_mac_sign_setup

    .. summary::
        Set up a multi-part MAC calculation operation.

    .. param:: psa_mac_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_mac_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        The MAC algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a MAC algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a MAC algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.

    This function sets up the calculation of the message authentication code (MAC) of a byte string. To verify the MAC of a message against an expected value, use `psa_mac_verify_setup()` instead.

    The sequence of operations to calculate a MAC is as follows:

    1.  Allocate a MAC operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_mac_operation_t`, e.g. `PSA_MAC_OPERATION_INIT`.
    #.  Call `psa_mac_sign_setup()` to specify the algorithm and key.
    #.  Call `psa_mac_update()` zero, one or more times, passing a fragment of the message each time. The MAC that is calculated is the MAC of the concatenation of these messages in order.
    #.  At the end of the message, call `psa_mac_sign_finish()` to finish calculating the MAC value and retrieve it.

    After a successful call to `psa_mac_sign_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_mac_sign_finish()`.
    *   A call to `psa_mac_abort()`.

    If `psa_mac_sign_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_mac_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_mac_verify_setup

    .. summary::
        Set up a multi-part MAC verification operation.

    .. param:: psa_mac_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_mac_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        The MAC algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a MAC algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a MAC algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.

    This function sets up the verification of the message authentication code (MAC) of a byte string against an expected value.

    The sequence of operations to verify a MAC is as follows:

    1.  Allocate a MAC operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_mac_operation_t`, e.g. `PSA_MAC_OPERATION_INIT`.
    #.  Call `psa_mac_verify_setup()` to specify the algorithm and key.
    #.  Call `psa_mac_update()` zero, one or more times, passing a fragment of the message each time. The MAC that is calculated is the MAC of the concatenation of these messages in order.
    #.  At the end of the message, call `psa_mac_verify_finish()` to finish calculating the actual MAC of the message and verify it against the expected value.

    After a successful call to `psa_mac_verify_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_mac_verify_finish()`.
    *   A call to `psa_mac_abort()`.

    If `psa_mac_verify_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_mac_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_mac_update

    .. summary::
        Add a message fragment to a multi-part MAC operation.

    .. param:: psa_mac_operation_t * operation
        Active MAC operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to add to the MAC calculation.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input for the operation is too large for the MAC algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must call `psa_mac_sign_setup()` or `psa_mac_verify_setup()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_mac_abort()`.

.. function:: psa_mac_sign_finish

    .. summary::
        Finish the calculation of the MAC of a message.

    .. param:: psa_mac_operation_t * operation
        Active MAC operation.
    .. param:: uint8_t * mac
        Buffer where the MAC value is to be written.
    .. param:: size_t mac_size
        Size of the ``mac`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The exact MAC size is :code:`PSA_MAC_LENGTH(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are attributes of the key, and ``alg`` is the algorithm used to compute the MAC.
        *   `PSA_MAC_MAX_SIZE` evaluates to the maximum MAC size of any supported MAC algorithm.

    .. param:: size_t * mac_length
        On success, the number of bytes that make up the MAC value.
        This is always :code:`PSA_MAC_LENGTH(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are attributes of the key, and ``alg`` is the algorithm used to compute the MAC.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*mac_length)`` bytes of ``mac`` contain the MAC value.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be an active mac sign operation.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``mac`` buffer is too small.
        `PSA_MAC_LENGTH()` or `PSA_MAC_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must call `psa_mac_sign_setup()` before calling this function. This function calculates the MAC of the message formed by concatenating the inputs passed to preceding calls to `psa_mac_update()`.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_mac_abort()`.

    .. warning::
        It is not recommended to use this function when a specific value is expected for the MAC. Call `psa_mac_verify_finish()` instead with the expected MAC value.

        Comparing integrity or authenticity data such as MAC values with a function such as ``memcmp()`` is risky because the time taken by the comparison might leak information about the hashed data which could allow an attacker to guess a valid MAC and thereby bypass security controls.

.. function:: psa_mac_verify_finish

    .. summary::
        Finish the calculation of the MAC of a message and compare it with an expected value.

    .. param:: psa_mac_operation_t * operation
        Active MAC operation.
    .. param:: const uint8_t * mac
        Buffer containing the expected MAC value.
    .. param:: size_t mac_length
        Size of the ``mac`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The expected MAC is identical to the actual MAC of the message.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The calculated MAC of the message does not match the value in ``mac``.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be an active mac verify operation.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must call `psa_mac_verify_setup()` before calling this function. This function calculates the MAC of the message formed by concatenating the inputs passed to preceding calls to `psa_mac_update()`. It then compares the calculated MAC with the expected MAC passed as a parameter to this function.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_mac_abort()`.

    .. note::
        Implementations must make the best effort to ensure that the comparison between the actual MAC and the expected MAC is performed in constant time.

.. function:: psa_mac_abort

    .. summary::
        Abort a MAC operation.

    .. param:: psa_mac_operation_t * operation
        Initialized MAC operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_mac_sign_setup()` or `psa_mac_verify_setup()` again.

    This function can be called any time after the operation object has been initialized by one of the methods described in `psa_mac_operation_t`.

    In particular, calling `psa_mac_abort()` after the operation has been terminated by a call to `psa_mac_abort()`, `psa_mac_sign_finish()` or `psa_mac_verify_finish()` is safe and has no effect.

Support macros
--------------

.. macro:: PSA_ALG_IS_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an HMAC algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an HMAC algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    HMAC is a family of MAC algorithms that are based on a hash function.

.. macro:: PSA_ALG_IS_BLAKE2_MAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a BLAKE2 MAC algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a BLAKE2 MAC algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    BLAKE2 is a family of hash and MAC algorithms.

.. macro:: PSA_ALG_IS_BLOCK_CIPHER_MAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a MAC algorithm based on a block cipher.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a MAC algorithm based on a block cipher, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_MAC_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The size of the output of `psa_mac_compute()` and `psa_mac_sign_finish()`, in bytes.

    .. param:: key_type
        The type of the MAC key.
    .. param:: key_bits
        The size of the MAC key in bits.
    .. param:: alg
        A MAC algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_MAC(alg)` is true.

    .. return::
        The MAC length for the specified algorithm with the specified key parameters.

        ``0`` if the MAC algorithm is not recognized.

        Either ``0`` or the correct length for a MAC algorithm that the implementation recognizes, but does not support.

        Unspecified if the key parameters are not consistent with the algorithm.

    If the size of the MAC buffer is at least this large, it is guaranteed that `psa_mac_compute()` and `psa_mac_sign_finish()` will not fail due to an insufficient buffer size.

    This is also the MAC length that `psa_mac_verify()` and `psa_mac_verify_finish()` expect.

    See also `PSA_MAC_MAX_SIZE`.

.. macro:: PSA_MAC_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient buffer size for storing the MAC output by `psa_mac_verify()` and `psa_mac_verify_finish()`, for any of the supported key types and MAC algorithms.

    If the size of the MAC buffer is at least this large, it is guaranteed that `psa_mac_verify()` and `psa_mac_verify_finish()` will not fail due to an insufficient buffer size.

    See also `PSA_MAC_LENGTH()`.
