.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 230

.. _ciphers:

Unauthenticated ciphers
=======================

.. warning::

    The unauthenticated cipher API is provided to implement legacy protocols and for use cases where the data integrity and authenticity is guaranteed by non-cryptographic means.

    It is recommended that newer protocols use :secref:`aead`.

The single-part functions for encrypting or decrypting a message using an unauthenticated symmetric cipher are:

*   `psa_cipher_encrypt()` to encrypt a message using an unauthenticated symmetric cipher. The encryption function generates a random initialization vector (IV). Use the multi-part API to provide a deterministic IV: this is not secure in general, but can be secure in some conditions that depend on the algorithm.
*   `psa_cipher_decrypt()` to decrypt a message using an unauthenticated symmetric cipher.

The `psa_cipher_operation_t` `multi-part operation <multi-part-operations>` permits alternative initialization parameters and allows messages to be processed in fragments. A multi-part cipher operation is used as follows:

1.  Initialize the `psa_cipher_operation_t` object to zero, or by assigning the value of the associated macro `PSA_CIPHER_OPERATION_INIT`.
#.  Call `psa_cipher_encrypt_setup()` or `psa_cipher_decrypt_setup()` to specify the algorithm and key.
#.  Provide additional parameters:

    -   When encrypting data, generate or set an IV, nonce, or similar initial value such as an initial counter value. To generate a random IV, which is recommended in most protocols, call `psa_cipher_generate_iv()`. To set the IV, call `psa_cipher_set_iv()`.
    -   When decrypting, set the IV or nonce. To set the IV, call `psa_cipher_set_iv()`.
#.  Call the `psa_cipher_update()` function on successive chunks of the message.
#.  Call `psa_cipher_finish()` to complete the operation and return any final output.

To abort the operation or recover from an error, call `psa_cipher_abort()`.

.. _cipher-algorithms:

Cipher algorithms
-----------------

.. macro:: PSA_ALG_STREAM_CIPHER
    :definition: ((psa_algorithm_t)0x04800100)

    .. summary::
        The stream cipher mode of a stream cipher algorithm.

    The underlying stream cipher is determined by the key type. The ARC4, ChaCha20, and XChaCha20 ciphers use this algorithm identifier.

    .. subsection:: ARC4

        To use ARC4, use a key type of `PSA_KEY_TYPE_ARC4` and algorithm id `PSA_ALG_STREAM_CIPHER`.

        .. warning::
            The ARC4 cipher is weak and deprecated and is only recommended for use in legacy applications.

        The ARC4 cipher does not use an initialization vector (IV). When using a multi-part cipher operation with the `PSA_ALG_STREAM_CIPHER` algorithm and an ARC4 key, `psa_cipher_generate_iv()` and `psa_cipher_set_iv()` must not be called.

    .. subsection:: ChaCha20

        To use ChaCha20, use a key type of `PSA_KEY_TYPE_CHACHA20` and algorithm id `PSA_ALG_STREAM_CIPHER`.

        Implementations must support the variant that is defined in :rfc-title:`8439#2.4`, which has a 96-bit nonce and a 32-bit counter. Implementations can optionally also support the original variant, as defined in :cite-title:`CHACHA20`, which has a 64-bit nonce and a 64-bit counter. Except where noted, the :RFC:`8439` variant must be used.

        ChaCha20 defines a nonce and an initial counter to be provided to the encryption and decryption operations. When using a ChaCha20 key with the `PSA_ALG_STREAM_CIPHER` algorithm, these values are provided using the initialization vector (IV) functions in the following ways:

        *   A call to `psa_cipher_encrypt()` will generate a random 12-byte nonce, and set the counter value to zero. The random nonce is output as a 12-byte IV value in the output.

        *   A call to `psa_cipher_decrypt()` will use first 12 bytes of the input buffer as the nonce and set the counter value to zero.

        *   A call to `psa_cipher_generate_iv()` on a multi-part cipher operation will generate and return a random 12-byte nonce and set the counter value to zero.

        *   A call to `psa_cipher_set_iv()` on a multi-part cipher operation can support the following IV sizes:

            -   12 bytes: the provided IV is used as the nonce, and the counter value is set to zero.
            -   16 bytes: the first four bytes of the IV are used as the counter value (encoded as little-endian), and the remaining 12 bytes are used as the nonce.
            -   8 bytes: the cipher operation uses the original :cite:`CHACHA20` definition of ChaCha20: the provided IV is used as the 64-bit nonce, and the 64-bit counter value is set to zero.
            -   It is recommended that implementations do not support other sizes of IV.

    .. subsection:: XChaCha20

        To use XChaCha20, use a key type of `PSA_KEY_TYPE_XCHACHA20` and algorithm id `PSA_ALG_STREAM_CIPHER`.

        XChaCha20 is a variation of ChaCha20 that uses a 192-bit nonce and a 64-bit counter. The larger nonce provides much lower probability of nonce misuse.

        When using an XChaCha20 key with the `PSA_ALG_STREAM_CIPHER` algorithm, the nonce and an initial counter values are provided using the initialization vector (IV) functions in the following ways:

        *   A call to `psa_cipher_encrypt()` will generate a random 24-byte nonce, and set the counter value to zero. The random nonce is output as a 24-byte IV value in the output.

        *   A call to `psa_cipher_decrypt()` will use first 24 bytes of the input buffer as the nonce and set the counter value to zero.

        *   A call to `psa_cipher_generate_iv()` on a multi-part cipher operation will generate and return a random 24-byte nonce and set the counter value to zero.

        *   A call to `psa_cipher_set_iv()` on a multi-part cipher operation can support the following IV sizes:

            -   24 bytes: the provided IV is used as the nonce, and the counter value is set to zero.
            -   32 bytes: the first 8 bytes of the IV are used as the counter value (encoded as little-endian), and the remaining 24 bytes are used as the nonce.

            Other sizes of IV are invalid.

        XChaCha20 is defined in :cite-title:`XCHACHA`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_ARC4`
        | `PSA_KEY_TYPE_CHACHA20`
        | `PSA_KEY_TYPE_XCHACHA20`

.. macro:: PSA_ALG_CTR
    :definition: ((psa_algorithm_t)0x04c01000)

    .. summary::
        A stream cipher built using the Counter (CTR) mode of a block cipher.

    CTR is a stream cipher which is built from a block cipher. The underlying block cipher is determined by the key type. For example, to use AES-128-CTR, use this algorithm with a key of type `PSA_KEY_TYPE_AES` and a size of 128 bits (16 bytes).

    The CTR block cipher mode is defined in :cite-title:`SP800-38A`.

    CTR mode operates using a *counter block* which is the same size as the cipher block length. The counter block is updated for each block, or a partial final block, that is encrypted or decrypted.

    For the `PSA_ALG_CTR` algorithm, the counter block is initialized from the IV. The counter block is then treated as a single, big-endian encoded integer, and the counter block is updated by incrementing this integer by ``1``.

    The security of CTR mode depends on using counter block values that are unique across all messages encrypted using the same key value.
    This is achieved by using suitable initial counter block values, the appropriate way to do this depends on the application use case:

    *   If the application is using CTR mode to implement a protocol that specifies the construction of the IV, then the application must use a multi-part cipher operation, and call `psa_cipher_set_iv()` with the appropriate IV for encryption and decryption operations.

        .. note::

            The protocol must use the same counter block update strategy as the one specified here.

    *   If the application is able to construct a unique *nonce* value for each time the same key is used to encrypt data, then it is recommended that the application uses a multi-part cipher operation, and call `psa_cipher_set_iv()` using the nonce as the IV for encryption and decryption operations.

        The nonce length, :math:`n` bytes, must satisfy :math:`1\le n\le b`, where :math:`b` is the cipher block size in bytes. To avoid a counter-block collision with other nonce values, the application must ensure that at most :math:`2^{8(b-n)}` blocks of data are encrypted in any single operation.

        For example, when using CTR encryption with an AES key, the cipher block size is 16 bytes. The application can provide a 12-byte nonce when setting the IV. This leaves 4 bytes for the counter, allowing up to :math:`2^{32}` blocks (64GB) of message data to be encrypted in each message.

    *   Otherwise, it is recommended that the application uses a random IV value when encrypting data, and transmits the IV along with the ciphertext for use when decrypting the data. This can be achieved with either the single-part cipher functions or the multi-part cipher operation:

        -    In a multi-part cipher encryption operation, call `psa_cipher_generate_iv()`, which returns the IV value. To use the same IV in a multi-part cipher decryption operation, call `psa_cipher_set_iv()`.
        -    A call to `psa_cipher_encrypt()` will generate a random counter block value. This is the first block of output. A call to `psa_cipher_decrypt()` will use first block of the input buffer as the initial counter block value.

    When using `PSA_ALG_CTR`, if the IV passed to `psa_cipher_set_iv()` is shorter than a cipher block, the initial counter block is formed by padding the end of the IV with zero bytes up to the block length.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CCM_STAR_NO_TAG
    :definition: ((psa_algorithm_t)0x04c01300)

    .. summary::
        The CCM* cipher mode without authentication.

        .. versionadded:: 1.2

    This is CCM* as specified in :cite-title:`IEEE-CCM` §7, with a tag length of 0. For CCM* with a nonzero tag length, use the AEAD algorithm `PSA_ALG_CCM`.

    The underlying block cipher is determined by the key type.

    The IV generated or set in the cipher API is used as the nonce in the CCM* operation. An implementation must support the default IV length of 13. Support for setting a shorter IV is optional.

    The maximum message length that can be encrypted is dependent on the length of the IV. See `PSA_ALG_CCM` for details of this relationship.

    .. _using-ccm-star-no-tag:

    .. subsection:: Usage in Zigbee

        The Zigbee message encryption algorithm is based on CCM*. This is detailed in :cite-title:`ZIGBEE` §B.1.1 and §A.

        *   For unauthenticated messages --- when tag length :math:`M = 0` --- the `PSA_ALG_CCM_STAR_NO_TAG` algorithm is used with an AES-128 key in a multi-part cipher operation. The 13-byte IV must be constructed as specified in `[ZIGBEE]`, and provided to the operation using `psa_cipher_set_iv()`.

            .. note::

                An implementation of Zigbee cannot use the single-part `psa_cipher_encrypt()` function, as this generates a random IV, which is not valid for the Zigbee protocol.

        *   For authenticated messages --- when tag length :math:`M \in \{4, 8, 16\}` --- the :code:`PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_length)` algorithm is used with an AES-128 key, where ``tag_length`` is the required value of :math:`M`. The 13-byte nonce must be constructed as specified in `[ZIGBEE]`.

            As the default tag length for CCM is 16, then `PSA_ALG_CCM` algorithm can be used when :math:`M = 16`.

        *   To enable a single AES-128 key to be used for both the `PSA_ALG_CCM_STAR_NO_TAG` cipher and `PSA_ALG_CCM` AEAD algorithm, the key can be defined with the wildcard `PSA_ALG_CCM_STAR_ANY_TAG` permitted algorithm.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CFB
    :definition: ((psa_algorithm_t)0x04c01100)

    .. summary::
        A stream cipher built using the Cipher Feedback (CFB) mode of a block cipher.

    The underlying block cipher is determined by the key type. This is the variant of CFB where each iteration encrypts or decrypts a segment of the input that is the same length as the cipher block size. For example, using `PSA_ALG_CFB` with a key of type `PSA_KEY_TYPE_AES` will result in the AES-CFB-128 cipher.

    .. rationale::

        Other segment sizes, such as CFB-8, are not currently supported in the |API|. The use of CFB has diminished, as CBC and CTR modes tend to be favoured.

    CFB mode requires an initialization vector (IV) that is the same size as the cipher block length.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    The CFB block cipher mode is defined in :cite-title:`SP800-38A`, using a segment size :math:`s` equal to the block size :math:`b`. The definition in `[SP800-38A]` is extended to allow an incomplete final block of input, in which case the algorithm discards the final bytes of the key stream when encrypting or decrypting the final partial block.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_OFB
    :definition: ((psa_algorithm_t)0x04c01200)

    .. summary::
        A stream cipher built using the Output Feedback (OFB) mode of a block cipher.

    The underlying block cipher is determined by the key type.

    OFB mode requires an initialization vector (IV) that is the same size as the cipher block length. OFB mode requires that the IV is a nonce, and must be unique for each use of the mode with the same key.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    The OFB block cipher mode is defined in :cite-title:`SP800-38A`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_XTS
    :definition: ((psa_algorithm_t)0x0440ff00)

    .. summary::
        The XEX with Ciphertext Stealing (XTS) cipher mode of a block cipher.

    XTS is a cipher mode which is built from a block cipher, designed for use in disk encryption. It requires at least one full cipher block length of input, but beyond this minimum the input does not need to be a whole number of blocks.

    XTS mode uses two keys for the underlying block cipher. These are provided by using a key that is twice the normal key size for the cipher. For example, to use AES-256-XTS the application must create a key with type `PSA_KEY_TYPE_AES` and bit size ``512``.

    XTS mode requires an initialization vector (IV) that is the same size as the cipher block length. The IV for XTS is typically defined to be the sector number of the disk block being encrypted or decrypted.

    The XTS block cipher mode is defined in :cite-title:`IEEE-XTS`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_ECB_NO_PADDING
    :definition: ((psa_algorithm_t)0x04404400)

    .. summary::
        The Electronic Codebook (ECB) mode of a block cipher, with no padding.

    .. warning::
        ECB mode does not protect the confidentiality of the encrypted data except in extremely narrow circumstances. It is recommended that applications only use ECB if they need to construct an operating mode that the implementation does not provide. Implementations are encouraged to provide the modes that applications need in preference to supporting direct access to ECB.

    The underlying block cipher is determined by the key type.

    This symmetric cipher mode can only be used with messages whose lengths are a multiple of the block size of the chosen block cipher.

    ECB mode does not accept an initialization vector (IV). When using a multi-part cipher operation with this algorithm, `psa_cipher_generate_iv()` and `psa_cipher_set_iv()` must not be called.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    The ECB block cipher mode is defined in :cite-title:`SP800-38A`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CBC_NO_PADDING
    :definition: ((psa_algorithm_t)0x04404000)

    .. summary::
        The Cipher Block Chaining (CBC) mode of a block cipher, with no padding.

    The underlying block cipher is determined by the key type.

    This symmetric cipher mode can only be used with messages whose lengths are a multiple of the block size of the chosen block cipher.

    CBC mode requires an initialization vector (IV) that is the same size as the cipher block length.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    The CBC block cipher mode is defined in :cite-title:`SP800-38A`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CBC_PKCS7
    :definition: ((psa_algorithm_t)0x04404100)

    .. summary::
        The Cipher Block Chaining (CBC) mode of a block cipher, with PKCS#7 padding.

    The underlying block cipher is determined by the key type.

    CBC mode requires an initialization vector (IV) that is the same size as the cipher block length.

    .. note::
        The cipher block length can be determined using `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.

    The CBC block cipher mode is defined in :cite-title:`SP800-38A`. The padding operation is defined by :RFC-title:`2315#10.3`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_DES`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

Single-part cipher functions
----------------------------

.. function:: psa_cipher_encrypt

    .. summary::
        Encrypt a message using a symmetric cipher.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must permit the usage `PSA_KEY_USAGE_ENCRYPT`.
    .. param:: psa_algorithm_t alg
        The cipher algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the message to encrypt.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * output
        Buffer where the output is to be written. The output contains the IV followed by the ciphertext proper.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length)`  where ``key_type`` is the type of ``key``.
        *   :code:`PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length)` evaluates to the maximum output size of any supported cipher encryption.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the encrypted output.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_ENCRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a cipher algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   The ``input_length`` is not valid for the algorithm and key type. For example, the algorithm is a based on block cipher and requires a whole number of blocks, but the total input size is not a multiple of the block size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a cipher algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small. `PSA_CIPHER_ENCRYPT_OUTPUT_SIZE()` or `PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing, see :secref:`library-init`.

    This function encrypts a message with a random initialization vector (IV).
    The length of the IV is :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)` where ``key_type`` is the type of ``key``.
    The output of `psa_cipher_encrypt()` is the IV followed by the ciphertext.

    Use the multi-part operation interface with a `psa_cipher_operation_t` object to provide other forms of IV or to manage the IV and ciphertext independently.

.. function:: psa_cipher_decrypt

    .. summary::
        Decrypt a message using a symmetric cipher.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_DECRYPT`.
    .. param:: psa_algorithm_t alg
        The cipher algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.
    .. param:: const uint8_t * input
        Buffer containing the message to decrypt. This consists of the IV followed by the ciphertext proper.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * output
        Buffer where the plaintext is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length)`  where ``key_type`` is the type of ``key``.
        *   :code:`PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length)` evaluates to the maximum output size of any supported cipher decryption.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the plaintext.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_DECRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a cipher algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   The ``input_length`` is not valid for the algorithm and key type. For example, the algorithm is a based on block cipher and requires a whole number of blocks, but the total input size is not a multiple of the block size.
    .. retval:: PSA_ERROR_INVALID_PADDING
        The algorithm uses padding, and the input does not contain valid padding.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a cipher algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small. `PSA_CIPHER_DECRYPT_OUTPUT_SIZE()` or `PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing, see :secref:`library-init`.

    This function decrypts a message encrypted with a symmetric cipher.

    The input to this function must contain the IV followed by the ciphertext, as output by `psa_cipher_encrypt()`. The IV must be :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)` bytes in length, where ``key_type`` is the type of ``key``.

    Use the multi-part operation interface with a `psa_cipher_operation_t` object to decrypt data which is not in the expected input format.

Multi-part cipher operations
----------------------------

.. typedef:: /* implementation-defined type */ psa_cipher_operation_t

    .. summary::
        The type of the state object for multi-part cipher operations.

    Before calling any function on a cipher operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_cipher_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_cipher_operation_t operation;

    *   Initialize the object to the initializer `PSA_CIPHER_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

    *   Assign the result of the function `psa_cipher_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_cipher_operation_t operation;
            operation = psa_cipher_operation_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_CIPHER_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a cipher operation object of type `psa_cipher_operation_t`.

.. function:: psa_cipher_operation_init

    .. summary::
        Return an initial value for a cipher operation object.

    .. return:: psa_cipher_operation_t

.. function:: psa_cipher_encrypt_setup

    .. summary::
        Set the key for a multi-part symmetric encryption operation.

    .. param:: psa_cipher_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_cipher_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_ENCRYPT`.
    .. param:: psa_algorithm_t alg
        The cipher algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_ENCRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a cipher algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a cipher algorithm.
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
        *   The library requires initializing, see :secref:`library-init`.

    The sequence of operations to encrypt a message with a symmetric cipher is as follows:

    1.  Allocate a cipher operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_cipher_operation_t`, e.g. `PSA_CIPHER_OPERATION_INIT`.
    #.  Call `psa_cipher_encrypt_setup()` to specify the algorithm and key.
    #.  Call either `psa_cipher_generate_iv()` or `psa_cipher_set_iv()` to generate or set the initialization vector (IV), if the algorithm requires one. It is recommended to use `psa_cipher_generate_iv()` unless the protocol being implemented requires a specific IV value.
    #.  Call `psa_cipher_update()` zero, one or more times, passing a fragment of the message each time.
    #.  Call `psa_cipher_finish()`.

    After a successful call to `psa_cipher_encrypt_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_cipher_finish()`.
    *   A call to `psa_cipher_abort()`.

    If `psa_cipher_encrypt_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_cipher_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_cipher_decrypt_setup

    .. summary::
        Set the key for a multi-part symmetric decryption operation.

    .. param:: psa_cipher_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_cipher_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_DECRYPT`.
    .. param:: psa_algorithm_t alg
        The cipher algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_DECRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a cipher algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a cipher algorithm.
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
        *   The library requires initializing, see :secref:`library-init`.

    The sequence of operations to decrypt a message with a symmetric cipher is as follows:

    1.  Allocate a cipher operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_cipher_operation_t`, e.g. `PSA_CIPHER_OPERATION_INIT`.
    #.  Call `psa_cipher_decrypt_setup()` to specify the algorithm and key.
    #.  Call `psa_cipher_set_iv()` with the initialization vector (IV) for the decryption, if the algorithm requires one. This must match the IV used for the encryption.
    #.  Call `psa_cipher_update()` zero, one or more times, passing a fragment of the message each time.
    #.  Call `psa_cipher_finish()`.

    After a successful call to `psa_cipher_decrypt_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_cipher_finish()`.
    *   A call to `psa_cipher_abort()`.

    If `psa_cipher_decrypt_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_cipher_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_cipher_generate_iv

    .. summary::
        Generate an initialization vector (IV) for a symmetric encryption operation.

    .. param:: psa_cipher_operation_t * operation
        Active cipher operation.
    .. param:: uint8_t * iv
        Buffer where the generated IV is to be written.
    .. param:: size_t iv_size
        Size of the ``iv`` buffer in bytes. This must be at least :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)` where ``key_type`` and ``alg`` are type of key and the algorithm respectively that were used to set up the cipher operation.
    .. param:: size_t * iv_length
        On success, the number of bytes of the generated IV.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*iv_length)`` bytes of ``iv`` contain the generated IV.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The cipher algorithm does not use an IV.
        *   The operation state is not valid: it must be active, with no IV set.
        *   The library requires initializing, see :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``iv`` buffer is too small. `PSA_CIPHER_IV_LENGTH()` or `PSA_CIPHER_IV_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function generates a random IV, nonce or initial counter value for the encryption operation as appropriate for the chosen algorithm, key type and key size.

    The generated IV is always the default length for the key and algorithm: :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)`, where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation. To generate different lengths of IV, use `psa_generate_random()` and `psa_cipher_set_iv()`.

    If the cipher algorithm does not use an IV, calling this function returns a :code:`PSA_ERROR_BAD_STATE` error. For these algorithms, :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)` will be zero.

    The application must call `psa_cipher_encrypt_setup()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_cipher_abort()`.

.. function:: psa_cipher_set_iv

    .. summary::
        Set the initialization vector (IV) for a symmetric encryption or decryption operation.

    .. param:: psa_cipher_operation_t * operation
        Active cipher operation.
    .. param:: const uint8_t * iv
        Buffer containing the IV to use.
    .. param:: size_t iv_length
        Size of the IV in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The cipher algorithm does not use an IV.
        *   The operation state is not valid: it must be an active cipher encrypt operation, with no IV set.
        *   The library requires initializing, see :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The chosen algorithm does not use an IV.
        *   ``iv_length`` is not valid for the chosen algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``iv_length`` is not supported for use with the operation's algorithm and key.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function sets the IV, nonce or initial counter value for the encryption or decryption operation.

    If the cipher algorithm does not use an IV, calling this function returns a :code:`PSA_ERROR_BAD_STATE` error. For these algorithms, :code:`PSA_CIPHER_IV_LENGTH(key_type, alg)` will be zero.

    The application must call `psa_cipher_encrypt_setup()` or `psa_cipher_decrypt_setup()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_cipher_abort()`.

    .. note::
        When encrypting, `psa_cipher_generate_iv()` is recommended instead of using this function, unless implementing a protocol that requires a non-random IV.

.. function:: psa_cipher_update

    .. summary::
        Encrypt or decrypt a message fragment in an active cipher operation.

    .. param:: psa_cipher_operation_t * operation
        Active cipher operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to encrypt or decrypt.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * output
        Buffer where the output is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length)`  where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   :code:`PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length)` evaluates to the maximum output size of any supported cipher algorithm.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the output data.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with an IV set if required for the algorithm.
        *   The library requires initializing, see :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small. `PSA_CIPHER_UPDATE_OUTPUT_SIZE()` or `PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input size passed to this operation is too large for this particular algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input size passed to this operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The following must occur before calling this function:

    1.  Call either `psa_cipher_encrypt_setup()` or `psa_cipher_decrypt_setup()`. The choice of setup function determines whether this function encrypts or decrypts its input.
    #.  If the algorithm requires an IV, call `psa_cipher_generate_iv()` or `psa_cipher_set_iv()`. `psa_cipher_generate_iv()` is recommended when encrypting.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_cipher_abort()`.

    .. note::

        This function does not require the input to be aligned to any particular block boundary. If the implementation can only process a whole block at a time, it must consume all the input provided, but it might delay the end of the corresponding output until a subsequent call to `psa_cipher_update()` provides sufficient input, or a subsequent call to `psa_cipher_finish()` indicates the end of the input. The amount of data that can be delayed in this way is bounded by the associated output size macro: `PSA_CIPHER_UPDATE_OUTPUT_SIZE()` or `PSA_CIPHER_FINISH_OUTPUT_SIZE()`.

.. function:: psa_cipher_finish

    .. summary::
        Finish encrypting or decrypting a message in a cipher operation.

    .. param:: psa_cipher_operation_t * operation
        Active cipher operation.
    .. param:: uint8_t * output
        Buffer where the last part of the output is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg)`  where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   `PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported cipher algorithm.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the final output.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input size passed to this operation is not valid for this particular algorithm. For example, the algorithm is a based on block cipher and requires a whole number of blocks, but the total input size is not a multiple of the block size.
    .. retval:: PSA_ERROR_INVALID_PADDING
        This is a decryption operation for an algorithm that includes padding, and the ciphertext does not contain valid padding.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with an IV set if required for the algorithm.
        *   The library requires initializing, see :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small. `PSA_CIPHER_FINISH_OUTPUT_SIZE()` or `PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must call `psa_cipher_encrypt_setup()` or `psa_cipher_decrypt_setup()` before calling this function. The choice of setup function determines whether this function encrypts or decrypts its input.

    This function finishes the encryption or decryption of the message formed by concatenating the inputs passed to preceding calls to `psa_cipher_update()`.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_cipher_abort()`.

.. function:: psa_cipher_abort

    .. summary::
        Abort a cipher operation.

    .. param:: psa_cipher_operation_t * operation
        Initialized cipher operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing, see :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_cipher_encrypt_setup()` or `psa_cipher_decrypt_setup()` again.

    This function can be called any time after the operation object has been initialized as described in `psa_cipher_operation_t`.

    In particular, calling `psa_cipher_abort()` after the operation has been terminated by a call to `psa_cipher_abort()` or `psa_cipher_finish()` is safe and has no effect.

Support macros
--------------

.. macro:: PSA_ALG_IS_STREAM_CIPHER
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a stream cipher.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a stream cipher algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier or if it is not a symmetric cipher algorithm.

    A stream cipher is a symmetric cipher that encrypts or decrypts messages by applying a bitwise-xor with a stream of bytes that is generated from a key.

.. macro:: PSA_ALG_CCM_STAR_ANY_TAG
    :definition: ((psa_algorithm_t)0x04c09300)

    .. summary::
        A wildcard algorithm that permits the use of the key with CCM* as both an AEAD and an unauthenticated cipher algorithm.

        .. versionadded:: 1.2

    If a block-cipher key specifies `PSA_ALG_CCM_STAR_ANY_TAG` as its permitted algorithm, then the key can be used with the `PSA_ALG_CCM_STAR_NO_TAG` unauthenticated cipher, the `PSA_ALG_CCM` AEAD algorithm, and truncated `PSA_ALG_CCM` AEAD algorithms.

.. macro:: PSA_CIPHER_ENCRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_encrypt()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        A cipher algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.
    .. param:: input_length
        Size of the input in bytes.

    .. return::
        A sufficient output size for the specified key type and algorithm. If the key type or cipher algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and cipher algorithm that it recognizes, but does not support.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_encrypt()` will not fail due to an insufficient buffer size. Depending on the algorithm, the actual size of the output might be smaller.

    See also `PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_encrypt()`, for any of the supported key types and cipher algorithms.

    .. param:: input_length
        Size of the input in bytes.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_encrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_CIPHER_ENCRYPT_OUTPUT_SIZE()`.

.. macro:: PSA_CIPHER_DECRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_decrypt()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        A cipher algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.
    .. param:: input_length
        Size of the input in bytes.

    .. return::
        A sufficient output size for the specified key type and algorithm. If the key type or cipher algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and cipher algorithm that it recognizes, but does not support.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_decrypt()` will not fail due to an insufficient buffer size. Depending on the algorithm, the actual size of the output might be smaller.

    See also `PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_decrypt()`, for any of the supported key types and cipher algorithms.

    .. param:: input_length
        Size of the input in bytes.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_decrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_CIPHER_DECRYPT_OUTPUT_SIZE()`.

.. macro:: PSA_CIPHER_IV_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The default IV size for a cipher algorithm, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        A cipher algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.

    .. return::
        The default IV size for the specified key type and algorithm.
        If the algorithm does not use an IV, return ``0``.
        If the key type or cipher algorithm is not recognized, or the parameters are incompatible, return ``0``.
        An implementation can return either ``0`` or a correct size for a key type and cipher algorithm that it recognizes, but does not support.

    The IV that is generated as part of a call to `psa_cipher_encrypt()` is always the default IV length for the algorithm.

    This macro can be used to allocate a buffer of sufficient size to store the IV output from `psa_cipher_generate_iv()` when using a multi-part cipher operation.

    See also `PSA_CIPHER_IV_MAX_SIZE`.

.. macro:: PSA_CIPHER_IV_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient buffer size for storing the IV generated by `psa_cipher_generate_iv()`, for any of the supported key types and cipher algorithms.

    If the size of the IV buffer is at least this large, it is guaranteed that `psa_cipher_generate_iv()` will not fail due to an insufficient buffer size.

    See also `PSA_CIPHER_IV_LENGTH()`.

.. macro:: PSA_CIPHER_UPDATE_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_update()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        A cipher algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.
    .. param:: input_length
        Size of the input in bytes.

    .. return::
        A sufficient output size for the specified key type and algorithm. If the key type or cipher algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and cipher algorithm that it recognizes, but does not support.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_update()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE`.

.. macro:: PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_update()`, for any of the supported key types and cipher algorithms.

    .. param:: input_length
        Size of the input in bytes.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_update()` will not fail due to an insufficient buffer size.

    See also `PSA_CIPHER_UPDATE_OUTPUT_SIZE()`.

.. macro:: PSA_CIPHER_FINISH_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_finish()`.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        A cipher algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_CIPHER(alg)` is true.

    .. return::
        A sufficient output size for the specified key type and algorithm. If the key type or cipher algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and cipher algorithm that it recognizes, but does not support.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_finish()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE`.

.. macro:: PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_cipher_finish()`, for any of the supported key types and cipher algorithms.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_cipher_finish()` will not fail due to an insufficient buffer size.

    See also `PSA_CIPHER_FINISH_OUTPUT_SIZE()`.

.. macro:: PSA_BLOCK_CIPHER_BLOCK_LENGTH
    :definition: /* specification-defined value */

    .. summary::
        The block size of a block cipher.

    .. param:: type
        A cipher key type: a value of type `psa_key_type_t`.

    .. return::
        The block size for a block cipher, or ``1`` for a stream cipher. The return value is undefined if ``type`` is not a supported cipher key type.

    .. note::
        It is possible to build stream cipher algorithms on top of a block cipher, for example CTR mode (`PSA_ALG_CTR`). This macro only takes the key type into account, so it cannot be used to determine the size of the data that `psa_cipher_update()` might buffer for future processing in general.

    See also `PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE`.

.. macro:: PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        The maximum block size of a block cipher supported by the implementation.

    See also `PSA_BLOCK_CIPHER_BLOCK_LENGTH()`.
