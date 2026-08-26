.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 210

.. _hashes:

Message digests (Hashes)
========================

The single-part hash functions are:

*   `psa_hash_compute()` to calculate the hash of a message.
*   `psa_hash_compare()` to compare the hash of a message with a reference value.

The `psa_hash_operation_t` `multi-part operation <multi-part-operations>` allows messages to be processed in fragments. A multi-part hash operation is used as follows:

1.  Initialize the `psa_hash_operation_t` object to zero, or by assigning the value of the associated macro `PSA_HASH_OPERATION_INIT`.
#.  Call `psa_hash_setup()` to specify the required hash algorithm, call `psa_hash_clone()` to duplicate the state of *active* `psa_hash_operation_t` object, or call `psa_hash_resume()` to restart a hash operation with the output from a previously suspended hash operation.
#.  Call the `psa_hash_update()` function on successive chunks of the message.
#.  At the end of the message, call the required finishing function:

    -   To suspend the hash operation and extract a hash suspend state, call `psa_hash_suspend()`. The output state can subsequently be used to resume the hash operation.
    -   To calculate the digest of a message, call `psa_hash_finish()`.
    -   To verify the digest of a message against a reference value, call `psa_hash_verify()`.

To abort the operation or recover from an error, call `psa_hash_abort()`.

.. _hash-algorithms:

Hash algorithms
---------------

.. macro:: PSA_ALG_MD2
    :definition: ((psa_algorithm_t)0x02000001)

    .. summary::
        The MD2 message-digest algorithm.

    .. warning::
        The MD2 hash is weak and deprecated and is only recommended for use in legacy applications.

    MD2 is defined in :RFC-title:`1319`.

.. macro:: PSA_ALG_MD4
    :definition: ((psa_algorithm_t)0x02000002)

    .. summary::
        The MD4 message-digest algorithm.

    .. warning::
        The MD4 hash is weak and deprecated and is only recommended for use in legacy applications.

    MD4 is defined in :RFC-title:`1320`.

.. macro:: PSA_ALG_MD5
    :definition: ((psa_algorithm_t)0x02000003)

    .. summary::
        The MD5 message-digest algorithm.

    .. warning::
        The MD5 hash is weak and deprecated and is only recommended for use in legacy applications.

    MD5 is defined in :RFC-title:`1321`.

.. macro:: PSA_ALG_RIPEMD160
    :definition: ((psa_algorithm_t)0x02000004)

    .. summary::
        The RIPEMD-160 message-digest algorithm.

    RIPEMD-160 is defined in :cite-title:`RIPEMD`, and also in :cite-title:`ISO10118`.

.. macro:: PSA_ALG_AES_MMO_ZIGBEE
    :definition: ((psa_algorithm_t)0x02000007)

    .. summary::
        The *Zigbee* 1.0 hash function based on a Matyas-Meyer-Oseas (MMO) construction using AES-128.

        .. versionadded:: 1.2

    This is the cryptographic hash function based on the Merkle-Damgård construction over a Matyas-Meyer-Oseas one-way compression function and the AES-128 block cipher, with the parametrization defined in :cite-title:`ZIGBEE` §B.6.

    This hash function can operate on input strings of up to :math:`2^{29} - 1` bytes.

    .. warning::
        The length of the hash is 128 bits, which makes brute-force attacks against collision resistance feasible.
        This algorithm is only recommended for use in the Zigbee protocol.

    .. note::
        AES-MMO-Zigbee is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID and the short length of the hash.

        It is recommended that this compound algorithm is not supported with `PSA_ALG_AES_MMO_ZIGBEE`.

    .. note::
        The Zigbee keyed hash function from `[ZIGBEE]` §B.1.4 is :code:`PSA_ALG_HMAC(PSA_ALG_AES_MMO_ZIGBEE)`.

.. macro:: PSA_ALG_SHA_1
    :definition: ((psa_algorithm_t)0x02000005)

    .. summary::
        The SHA-1 message-digest algorithm.

    .. warning::
        The SHA-1 hash is weak and deprecated and is only recommended for use in legacy applications.

    SHA-1 is defined in :cite-title:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_224
    :definition: ((psa_algorithm_t)0x02000008)

    .. summary::
        The SHA-224 message-digest algorithm.

    SHA-224 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_256
    :definition: ((psa_algorithm_t)0x02000009)

    .. summary::
        The SHA-256 message-digest algorithm.

    SHA-256 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_384
    :definition: ((psa_algorithm_t)0x0200000a)

    .. summary::
        The SHA-384 message-digest algorithm.

    SHA-384 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_512
    :definition: ((psa_algorithm_t)0x0200000b)

    .. summary::
        The SHA-512 message-digest algorithm.

    SHA-512 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_512_224
    :definition: ((psa_algorithm_t)0x0200000c)

    .. summary::
        The SHA-512/224 message-digest algorithm.

    SHA-512/224 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_512_256
    :definition: ((psa_algorithm_t)0x0200000d)

    .. summary::
        The SHA-512/256 message-digest algorithm.

    SHA-512/256 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA_256_192
    :definition: ((psa_algorithm_t)0x0200000E)

    .. summary::
        The SHA-256/192 message digest algorithm.

        .. versionadded:: 1.3

    SHA-256/192 is the first 192 bits (24 bytes) of the SHA-256 output.
    SHA-256 is defined in :cite:`FIPS180-4`.

.. macro:: PSA_ALG_SHA3_224
    :definition: ((psa_algorithm_t)0x02000010)

    .. summary::
        The SHA3-224 message-digest algorithm.

    SHA3-224 is defined in :cite-title:`FIPS202`.

.. macro:: PSA_ALG_SHA3_256
    :definition: ((psa_algorithm_t)0x02000011)

    .. summary::
        The SHA3-256 message-digest algorithm.

    SHA3-256 is defined in :cite:`FIPS202`.

.. macro:: PSA_ALG_SHA3_384
    :definition: ((psa_algorithm_t)0x02000012)

    .. summary::
        The SHA3-384 message-digest algorithm.

    SHA3-384 is defined in :cite:`FIPS202`.

.. macro:: PSA_ALG_SHA3_512
    :definition: ((psa_algorithm_t)0x02000013)

    .. summary::
        The SHA3-512 message-digest algorithm.

    SHA3-512 is defined in :cite:`FIPS202`.

.. macro:: PSA_ALG_SHAKE256_512
    :definition: ((psa_algorithm_t)0x02000015)

    .. summary::
        The SHAKE256/512 message digest algorithm.

        .. versionadded:: 1.1

    SHAKE256/512 is the first 512 bits (64 bytes) of the SHAKE256 output.
    The SHAKE256 XOF is defined in :cite:`FIPS202`.

    This is the pre-hashing for Ed448ph, see `PSA_ALG_ED448PH`, and can be used as pre-hashing for ML-DSA or SLH-DSA, see `PSA_ALG_HASH_ML_DSA` and `PSA_ALG_HASH_SLH_DSA`.

    .. note::
        To use SHAKE256 as an XOF, see :secref:`xof` and `PSA_ALG_SHAKE256`.

    .. note::
        For other scenarios where a hash function based on SHA3 or SHAKE is required, SHA3-512 is recommended.
        SHA3-512 has the same output size, and a theoretically higher security strength.

    .. note::
        The current version of this specification does not specify SHAKE256/512 in certain compound algorithms, due to the lack of standardization of some required parameters:

        *   SHAKE256/512 is not specified with compound algorithms based on HMAC, due to the lack of a standard HMAC block size.
            See `PSA_ALG_HMAC` for details.
        *   SHAKE256/512 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that these compound algorithms are not supported with `PSA_ALG_SHAKE256_512`.

.. macro:: PSA_ALG_SHAKE128_256
    :definition: ((psa_algorithm_t)0x02000016)

    .. summary::
        The SHAKE128/256 message digest algorithm.

        .. versionadded:: 1.3

    SHAKE128/256 is the first 256 bits (32 bytes) of the SHAKE128 output.
    The SHAKE128 XOF is defined in :cite:`FIPS202`.

    This can be used as pre-hashing for ML-DSA or SLH-DSA, see `PSA_ALG_HASH_ML_DSA` and `PSA_ALG_HASH_SLH_DSA`.

    .. note::
        To use SHAKE128 as an XOF, see :secref:`xof` and `PSA_ALG_SHAKE128`.

    .. note::
        For other scenarios where a hash function based on SHA3 or SHAKE is required, SHA3-256 is recommended.
        SHA3-256 has the same output size, and a theoretically higher security strength.

.. macro:: PSA_ALG_SHAKE256_192
    :definition: ((psa_algorithm_t)0x02000017)

    .. summary::
        The SHAKE256/192 message digest algorithm.

        .. versionadded:: 1.3

    SHAKE256/192 is the first 192 bits (24 bytes) of the SHAKE256 output.
    SHAKE256 is defined in :cite:`FIPS202`.

.. macro:: PSA_ALG_SHAKE256_256
    :definition: ((psa_algorithm_t)0x02000018)

    .. summary::
        The SHAKE256/256 message digest algorithm.

        .. versionadded:: 1.3

    SHAKE256/256 is the first 256 bits (32 bytes) of the SHAKE256 output.
    SHAKE256 is defined in :cite:`FIPS202`.

.. macro:: PSA_ALG_SM3
    :definition: ((psa_algorithm_t)0x02000014)

    .. summary::
        The SM3 message-digest algorithm.

    SM3 is defined in :cite-title:`ISO10118`, and also in :cite-title:`CSTC0004`.

.. macro:: PSA_ALG_ASCON_HASH256
    :definition: ((psa_algorithm_t)0x02000019)

    .. summary::
        The Ascon-Hash256 message-digest algorithm.

        .. versionadded:: 1.4

    Ascon-Hash256 is defined in :cite-title:`SP800-232` §5.1.

    .. note::
        To use the Ascon XOF algorithms, see `PSA_ALG_ASCON_XOF128` and `PSA_ALG_ASCON_CXOF128`.

    .. note::
        The current version of this specification does not specify Ascon-Hash256 in certain compound algorithms, due to the lack of standardization of some required parameters:

        *   Ascon-Hash256 is not specified with compound algorithms based on HMAC, due to the lack of a standard HMAC block size.
            See `PSA_ALG_HMAC` for details.
        *   Ascon-Hash256 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that these compound algorithms are not supported with `PSA_ALG_ASCON_HASH256`.

.. macro:: PSA_ALG_BLAKE2S_HASH256
    :definition: ((psa_algorithm_t)0x0200001C)

    .. summary::
        The BLAKE2s cryptographic hash with 256 bits of output (BLAKE2s-256).

        .. versionadded:: 1.5

    The BLAKE2s-256 hash is BLAKE2s with a zero-length key, and a 256-bit (32-byte) output.
    BLAKE2s is defined in :rfc-title:`7693`.

    .. note::
        To use BLAKE2s as a MAC, see `PSA_ALG_BLAKE2_MAC`.

        Where a protocol requires the use of HMAC or HKDF-HMAC, BLAKE2s-256 can be used with HMAC.
        See `PSA_ALG_HMAC`.

    .. note::
        BLAKE2s-256 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that this compound algorithm is not supported with `PSA_ALG_BLAKE2S_HASH256`.

.. macro:: PSA_ALG_BLAKE2SP_HASH256
    :definition: ((psa_algorithm_t)0x0200001D)

    .. summary::
        The BLAKE2sp cryptographic hash with 256 bits of output (BLAKE2sp-256).

        .. versionadded:: 1.5

    The BLAKE2sp-256 hash is the parallel version of BLAKE2s with a zero-length key, and a 256-bit (32-byte) output.
    BLAKE2sp is defined in :cite-title:`BLAKE2`.

    .. note::
        To use BLAKE2sp as a MAC, see `PSA_ALG_BLAKE2_MAC`.

    .. note::
        BLAKE2sp-256 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that this compound algorithm is not supported with `PSA_ALG_BLAKE2SP_HASH256`.

.. macro:: PSA_ALG_BLAKE2B_HASH512
    :definition: ((psa_algorithm_t)0x0200001E)

    .. summary::
        The BLAKE2b cryptographic hash with 512 bits of output (BLAKE2b-512).

        .. versionadded:: 1.5

    The BLAKE2b-512 hash is BLAKE2b with a zero-length key, and a 512-bit (64-byte) output.
    BLAKE2b is defined in :rfc:`7693`.

    .. note::
        To use BLAKE2b as a MAC, see `PSA_ALG_BLAKE2_MAC`.

        Where a protocol requires the use of HMAC or HKDF-HMAC, BLAKE2b-512 can be used with HMAC.
        See `PSA_ALG_HMAC`.

    .. note::
        BLAKE2b-512 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that this compound algorithm is not supported with `PSA_ALG_BLAKE2B_HASH512`.

.. macro:: PSA_ALG_BLAKE2BP_HASH512
    :definition: ((psa_algorithm_t)0x0200001F)

    .. summary::
        The BLAKE2bp cryptographic hash with 512 bits of output (BLAKE2bp-512).

        .. versionadded:: 1.5

    The BLAKE2bp-512 hash is the parallel version of BLAKE2b with a zero-length key, and a 512-bit (64-byte) output.
    BLAKE2bp is defined in :cite-title:`BLAKE2`.

    .. note::
        To use BLAKE2bp as a MAC, see `PSA_ALG_BLAKE2_MAC`.

    .. note::
        BLAKE2bp-512 is not specified with `PSA_ALG_RSA_PKCS1V15_SIGN`, due to the lack of a standard OID.

        It is recommended that this compound algorithm is not supported with `PSA_ALG_BLAKE2BP_HASH512`.

.. macro:: PSA_ALG_IS_BLAKE2_HASH
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a BLAKE2 hash algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a BLAKE2 hash algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    BLAKE2 is a family of hash and MAC algorithms.


Single-part hashing functions
-----------------------------

.. function:: psa_hash_compute

    .. summary::
        Calculate the hash (digest) of a message.

    .. param:: psa_algorithm_t alg
        The hash algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the message to hash.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * hash
        Buffer where the hash is to be written.
    .. param:: size_t hash_size
        Size of the ``hash`` buffer in bytes.
        This must be at least :code:`PSA_HASH_LENGTH(alg)`.
    .. param:: size_t * hash_length
        On success, the number of bytes that make up the hash value. This is always :code:`PSA_HASH_LENGTH(alg)`.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*hash_length)`` bytes of ``hash`` contain the hash value.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a hash algorithm.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a hash algorithm.
        *   ``input_length`` is too large for ``alg``.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``hash`` buffer is too small.
        `PSA_HASH_LENGTH()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    .. note::
        To verify the hash of a message against an expected value, use `psa_hash_compare()` instead.

.. function:: psa_hash_compare

    .. summary::
        Calculate the hash (digest) of a message and compare it with a reference value.

    .. param:: psa_algorithm_t alg
        The hash algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the message to hash.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * hash
        Buffer containing the expected hash value.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The expected hash is identical to the actual hash of the input.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The calculated hash of the message does not match the value in ``hash``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a hash algorithm.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a hash algorithm.
        *   ``input_length`` is too large for ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

.. _hash-mp:

Multi-part hashing operations
-----------------------------

.. typedef:: /* implementation-defined type */ psa_hash_operation_t

    .. summary::
        The type of the state object for multi-part hash operations.

    Before calling any function on a hash operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_hash_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_hash_operation_t operation;

    *   Initialize the object to the initializer `PSA_HASH_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;

    *   Assign the result of the function `psa_hash_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_hash_operation_t operation;
            operation = psa_hash_operation_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_HASH_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a hash operation object of type `psa_hash_operation_t`.

.. function:: psa_hash_operation_init

    .. summary::
        Return an initial value for a hash operation object.

    .. return:: psa_hash_operation_t

.. function:: psa_hash_setup

    .. summary::
        Set up a multi-part hash operation.

    .. param:: psa_hash_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_hash_operation_t` and not yet in use.
    .. param:: psa_algorithm_t alg
        The hash algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``alg`` is not supported or is not a hash algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``alg`` is not a hash algorithm.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The sequence of operations to calculate a hash (message digest) is as follows:

    1.  Allocate a hash operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_hash_operation_t`, e.g. `PSA_HASH_OPERATION_INIT`.
    #.  Call `psa_hash_setup()` to specify the algorithm.
    #.  Call `psa_hash_update()` zero, one or more times, passing a fragment of the message each time. The hash that is calculated is the hash of the concatenation of these messages in order.
    #.  To calculate the hash, call `psa_hash_finish()`. To compare the hash with an expected value, call `psa_hash_verify()`. To suspend the hash operation and extract the current state, call `psa_hash_suspend()`.

    After a successful call to `psa_hash_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_hash_finish()` or `psa_hash_verify()` or `psa_hash_suspend()`.
    *   A call to `psa_hash_abort()`.

    If `psa_hash_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_hash_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_hash_update

    .. summary::
        Add a message fragment to a multi-part hash operation.

    .. param:: psa_hash_operation_t * operation
        Active hash operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to hash.
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
        The total input for the operation is too large for the hash algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_hash_setup()` or `psa_hash_resume()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_hash_abort()`.

.. function:: psa_hash_finish

    .. summary::
        Finish the calculation of the hash of a message.

    .. param:: psa_hash_operation_t * operation
        Active hash operation.
    .. param:: uint8_t * hash
        Buffer where the hash is to be written.
    .. param:: size_t hash_size
        Size of the ``hash`` buffer in bytes. This must be at least :code:`PSA_HASH_LENGTH(alg)` where ``alg`` is the algorithm that the operation performs.
    .. param:: size_t * hash_length
        On success, the number of bytes that make up the hash value. This is always :code:`PSA_HASH_LENGTH(alg)` where ``alg`` is the hash algorithm that the operation performs.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*hash_length)`` bytes of ``hash`` contain the hash value.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``hash`` buffer is too small.
        `PSA_HASH_LENGTH()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_hash_setup()` or `psa_hash_resume()` before calling this function. This function calculates the hash of the message formed by concatenating the inputs passed to preceding calls to `psa_hash_update()`.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_hash_abort()`.

    .. warning::
        It is not recommended to use this function when a specific value is expected for the hash. Call `psa_hash_verify()` instead with the expected hash value.

        Comparing integrity or authenticity data such as hash values with a function such as ``memcmp()`` is risky because the time taken by the comparison might leak information about the hashed data which could allow an attacker to guess a valid hash and thereby bypass security controls.

.. function:: psa_hash_verify

    .. summary::
        Finish the calculation of the hash of a message and compare it with an expected value.

    .. param:: psa_hash_operation_t * operation
        Active hash operation.
    .. param:: const uint8_t * hash
        Buffer containing the expected hash value.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The expected hash is identical to the actual hash of the message.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The calculated hash of the message does not match the value in ``hash``.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_hash_setup()` before calling this function. This function calculates the hash of the message formed by concatenating the inputs passed to preceding calls to `psa_hash_update()`. It then compares the calculated hash with the expected hash passed as a parameter to this function.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_hash_abort()`.

    .. note::
        Implementations must make the best effort to ensure that the comparison between the actual hash and the expected hash is performed in constant time.

.. function:: psa_hash_abort

    .. summary::
        Abort a hash operation.

    .. param:: psa_hash_operation_t * operation
        Initialized hash operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_hash_setup()` again.

    This function can be called any time after the operation object has been initialized by one of the methods described in `psa_hash_operation_t`.

    In particular, calling `psa_hash_abort()` after the operation has been terminated by a call to `psa_hash_abort()`, `psa_hash_finish()` or `psa_hash_verify()` is safe and has no effect.

.. function:: psa_hash_suspend

    .. summary::
        Halt the hash operation and extract the intermediate state of the hash computation.

    .. param:: psa_hash_operation_t * operation
        Active hash operation.
    .. param:: uint8_t * hash_state
        Buffer where the hash suspend state is to be written.
    .. param:: size_t hash_state_size
        Size of the ``hash_state`` buffer in bytes.
        This must be appropriate for the selected algorithm:

        *   A sufficient output size is :code:`PSA_HASH_SUSPEND_OUTPUT_SIZE(alg)`  where ``alg`` is the algorithm that was used to set up the operation.
        *   `PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported hash algorithm.

    .. param:: size_t * hash_state_length
        On success, the number of bytes that make up the hash suspend state.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*hash_state_length)`` bytes of ``hash_state`` contain the intermediate hash state.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``hash_state`` buffer is too small.
        `PSA_HASH_SUSPEND_OUTPUT_SIZE()` or `PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The hash algorithm being computed does not support suspend and resume.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_hash_setup()` or `psa_hash_resume()` before calling this function. This function extracts an intermediate state of the hash computation of the message formed by concatenating the inputs passed to preceding calls to `psa_hash_update()`.

    This function can be used to halt a hash operation, and then resume the hash operation at a later time, or in another application, by transferring the extracted hash suspend state to a call to `psa_hash_resume()`.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_hash_abort()`.

    Hash suspend and resume is not defined for the SHA3 family of hash algorithms. :secref:`hash-suspend-state` defines the format of the output from `psa_hash_suspend()`.

    .. warning::
        Applications must not use any of the hash suspend state as if it was a hash output. Instead, the suspend state must only be used to resume a hash operation, and `psa_hash_finish()` or `psa_hash_verify()` can then calculate or verify the final hash value.

    .. rubric:: Usage

    The sequence of operations to suspend and resume a hash operation is as follows:

    1.  Compute the first part of the hash.

        a.  Allocate an operation object and initialize it as described in the documentation for `psa_hash_operation_t`.
        #.  Call `psa_hash_setup()` to specify the algorithm.
        #.  Call `psa_hash_update()` zero, one or more times, passing a fragment of the message each time.
        #.  Call `psa_hash_suspend()` to extract the hash suspend state into a buffer.

    #.  Pass the hash state buffer to the application which will resume the operation.

    #.  Compute the rest of the hash.

        a.  Allocate an operation object and initialize it as described in the documentation for `psa_hash_operation_t`.
        #.  Call `psa_hash_resume()` with the extracted hash state.
        #.  Call `psa_hash_update()` zero, one or more times, passing a fragment of the message each time.
        #.  To calculate the hash, call `psa_hash_finish()`. To compare the hash with an expected value, call `psa_hash_verify()`.

    If an error occurs at any step after a call to `psa_hash_setup()` or `psa_hash_resume()`, the operation will need to be reset by a call to `psa_hash_abort()`. The application can call `psa_hash_abort()` at any time after the operation has been initialized.

.. function:: psa_hash_resume

    .. summary::
        Set up a multi-part hash operation using the hash suspend state from a previously suspended hash operation.

    .. param:: psa_hash_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_hash_operation_t` and not yet in use.
    .. param:: const uint8_t * hash_state
        A buffer containing the suspended hash state which is to be resumed. This must be in the format output by `psa_hash_suspend()`, which is described in :secref:`hash-suspend-state-format`.
    .. param:: size_t hash_state_length
        Length of ``hash_state`` in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The provided hash suspend state is for an algorithm that is not supported.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``hash_state`` does not correspond to a valid hash suspend state. See :secref:`hash-suspend-state-format` for the definition.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    See `psa_hash_suspend()` for an example of how to use this function to suspend and resume a hash operation.

    After a successful call to `psa_hash_resume()`, the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_hash_finish()`, `psa_hash_verify()` or `psa_hash_suspend()`.
    *   A call to `psa_hash_abort()`.

.. function:: psa_hash_clone

    .. summary::
        Clone a hash operation.

    .. param:: const psa_hash_operation_t * source_operation
        The active hash operation to clone.
    .. param:: psa_hash_operation_t * target_operation
        The operation object to set up. It must be initialized but not active.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        ``target_operation`` is ready to continue the same hash operation as ``source_operation``.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The ``source_operation`` state is not valid: it must be active.
        *   The ``target_operation`` state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY

    This function copies the state of an ongoing hash operation to a new operation object. In other words, this function is equivalent to calling `psa_hash_setup()` on ``target_operation`` with the same algorithm that ``source_operation`` was set up for, then `psa_hash_update()` on ``target_operation`` with the same input that that was passed to ``source_operation``. After this function returns, the two objects are independent, i.e. subsequent calls involving one of the objects do not affect the other object.

Support macros
--------------

.. macro:: PSA_HASH_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The size of the output of `psa_hash_compute()` and `psa_hash_finish()`, in bytes.

    .. param:: alg
        A hash algorithm or an HMAC algorithm: a value of type `psa_algorithm_t` such that :code:`(PSA_ALG_IS_HASH(alg) || PSA_ALG_IS_HMAC(alg))` is true.

    .. return::
        The hash length for the specified hash algorithm. If the hash algorithm is not recognized, return ``0``. An implementation can return either ``0`` or the correct size for a hash algorithm that it recognizes, but does not support.

    This is also the hash length that `psa_hash_compare()` and `psa_hash_verify()` expect.

    See also `PSA_HASH_MAX_SIZE`.

.. macro:: PSA_HASH_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Maximum size of a hash.

    It is recommended that this value is the maximum size of a hash supported by the implementation, in bytes. The value must not be smaller than this maximum.

    See also `PSA_HASH_LENGTH()`.

.. macro:: PSA_HASH_SUSPEND_OUTPUT_SIZE
    :definition: /* specification-defined value */

    .. summary::
        A sufficient hash suspend state buffer size for `psa_hash_suspend()`, in bytes.

    .. param:: alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.

    .. return::
        A sufficient output size for the algorithm. If the hash algorithm is not recognized, or is not supported by `psa_hash_suspend()`, return ``0``. An implementation can return either ``0`` or a correct size for a hash algorithm that it recognizes, but does not support.

        For a supported hash algorithm ``alg``, the following expression is true:

        .. code-block:: xref

            PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) == PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH +
                                                 PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) +
                                                 PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) +
                                                 PSA_HASH_BLOCK_LENGTH(alg) - 1

    If the size of the hash state buffer is at least this large, it is guaranteed that `psa_hash_suspend()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE`.

.. macro:: PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient hash suspend state buffer size for `psa_hash_suspend()`, for any supported hash algorithms.

    If the size of the hash state buffer is at least this large, it is guaranteed that `psa_hash_suspend()` will not fail due to an insufficient buffer size.

    See also `PSA_HASH_SUSPEND_OUTPUT_SIZE()`.

.. macro:: PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH
    :definition: ((size_t)4)

    .. summary::
        The size of the *algorithm* field that is part of the output of `psa_hash_suspend()`, in bytes.

    Applications can use this value to unpack the hash suspend state that is output by `psa_hash_suspend()`.

.. macro:: PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH
    :definition: /* specification-defined value */

    .. summary::
        The size of the *input-length* field that is part of the output of `psa_hash_suspend()`, in bytes.

    .. param:: alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.

    .. return::
        The size, in bytes, of the *input-length* field of the hash suspend state for the specified hash algorithm. If the hash algorithm is not recognized, return ``0``. An implementation can return either ``0`` or the correct size for a hash algorithm that it recognizes, but does not support.

        The algorithm-specific values are defined in :secref:`hash-suspend-state-constants`.

    Applications can use this value to unpack the hash suspend state that is output by `psa_hash_suspend()`.

.. macro:: PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH
    :definition: /* specification-defined value */

    .. summary::
        The size of the *hash-state* field that is part of the output of `psa_hash_suspend()`, in bytes.

    .. param:: alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.

    .. return::
        The size, in bytes, of the *hash-state* field of the hash suspend state for the specified hash algorithm. If the hash algorithm is not recognized, return ``0``. An implementation can return either ``0`` or the correct size for a hash algorithm that it recognizes, but does not support.

        The algorithm-specific values are defined in :secref:`hash-suspend-state-constants`.

    Applications can use this value to unpack the hash suspend state that is output by `psa_hash_suspend()`.

.. macro:: PSA_HASH_BLOCK_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The input block size of a hash algorithm, in bytes.

    .. param:: alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(alg)` is true.

    .. return::
        The block size in bytes for the specified hash algorithm. If the hash algorithm is not recognized, return ``0``. An implementation can return either ``0`` or the correct size for a hash algorithm that it recognizes, but does not support.

    Hash algorithms process their input data in blocks. Hash operations will retain any partial blocks until they have enough input to fill the block or until the operation is finished.

    This affects the output from `psa_hash_suspend()`.


.. _hash-suspend-state:

Hash suspend state
------------------

The hash suspend state is output by `psa_hash_suspend()` and input to `psa_hash_resume()`.

.. note::
    Hash suspend and resume is not defined for the SM3 algorithm and the SHA3 family of hash algorithms.

.. _hash-suspend-state-format:

Hash suspend state format
^^^^^^^^^^^^^^^^^^^^^^^^^

The hash suspend state has the following format:

.. math::

    hash\_suspend\_state = algorithm\ ||\ input\_length\ ||\ hash\_state\ ||\ unprocessed\_input

The fields in the hash suspend state are defined as follows:

:math:`algorithm`
    A big-endian 32-bit unsigned integer.

    The |API| algorithm identifier value.

    The byte length of the :math:`algorithm` field can be evaluated using `PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH`.

:math:`input\_length`
    A big-endian unsigned integer

    The content of this field is algorithm-specific:

    *   For MD2, this is the number of bytes in :math:`unprocessed\_input`.
    *   For all other hash algorithms, this is the total number of bytes of input to the hash computation. This includes the :math:`unprocessed\_input` bytes.

    The size of this field is algorithm-specific:

    *   For MD2: :math:`input\_length` is an 8-bit unsigned integer.
    *   For MD4, MD5, RIPEMD-160, SHA-1, SHA-224, and SHA-256: :math:`input\_length` is a 64-bit unsigned integer.
    *   For SHA-512/224, SHA-512/256, SHA-384, and SHA-512: :math:`input\_length` is a 128-bit unsigned integer.

    The length, in bytes, of the :math:`input\_length` field can be calculated using :code:`PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg)` where ``alg`` is a hash algorithm.
    See :secref:`hash-suspend-state-constants`.

:math:`hash\_state`
    An array of bytes

    Algorithm-specific intermediate hash state:

    *   For MD2: 16 bytes of internal checksum, then 48 bytes of intermediate digest.
    *   For MD4 and MD5: 4x 32-bit integers, in little-endian encoding.
    *   For RIPEMD-160: 5x 32-bit integers, in little-endian encoding.
    *   For SHA-1: 5x 32-bit integers, in big-endian encoding.
    *   For SHA-224 and SHA-256: 8x 32-bit integers, in big-endian encoding.
    *   For SHA-512/224, SHA-512/256, SHA-384, and SHA-512: 8x 64-bit integers, in big-endian encoding.

    The length of this field is specific to the algorithm.
    The length, in bytes, of the :math:`hash\_state` field can be calculated using :code:`PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg)` where ``alg`` is a hash algorithm.
    See :secref:`hash-suspend-state-constants`.

:math:`unprocessed\_input`
    :math:`0\ \text{to}\ (hash\_block\_size - 1)` bytes

    A partial block of unprocessed input data. This is between zero and :math:`hash\_block\_size - 1` bytes of data, the length can be calculated by:

    .. math::

        \text{length}(unprocessed\_input) = input\_length \mod hash\_block\_size.

    The value of :math:`hash\_block\_size` is specific to the hash algorithm.
    The size of a hash block can be calculated using :code:`PSA_HASH_BLOCK_LENGTH(alg)` where ``alg`` is a hash algorithm.
    See :secref:`hash-suspend-state-constants`.

.. _hash-suspend-state-constants:

Hash suspend state field sizes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following table defines the algorithm-specific field lengths for the hash suspend state returned by `psa_hash_suspend()`. All of the field lengths are in bytes. To compute the field lengths for algorithm ``alg``, use the following expressions:

*   :code:`PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH` returns the length of the :math:`algorithm` field.
*   :code:`PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg)` returns the length of the :math:`input\_length` field.
*   :code:`PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg)` returns the length of the :math:`hash\_state` field.
*   :code:`PSA_HASH_BLOCK_LENGTH(alg) - 1` is the maximum length of the :math:`unprocessed\_bytes` field.
*   :code:`PSA_HASH_SUSPEND_OUTPUT_SIZE(alg)` returns the maximum size of the hash suspend state.

.. csv-table::
    :header-rows: 1
    :widths: auto
    :align: left

    Hash algorithm, :math:`input\_length` size (bytes), :math:`hash\_state` length (bytes), :math:`unprocessed\_bytes` length (bytes)
    `PSA_ALG_MD2`, 1, 64, 0 -- 15
    `PSA_ALG_MD4`, 8, 16, 0 -- 63
    `PSA_ALG_MD5`, 8, 16, 0 -- 63
    `PSA_ALG_RIPEMD160`, 8, 20, 0 -- 63
    `PSA_ALG_SHA_1`, 8, 20, 0 -- 63
    `PSA_ALG_SHA_224`, 8, 32, 0 -- 63
    `PSA_ALG_SHA_256`, 8, 32, 0 -- 63
    `PSA_ALG_SHA_512_224`, 16, 64, 0 -- 127
    `PSA_ALG_SHA_512_256`, 16, 64, 0 -- 127
    `PSA_ALG_SHA_384`, 16, 64, 0 -- 127
    `PSA_ALG_SHA_512`, 16, 64, 0 -- 127
