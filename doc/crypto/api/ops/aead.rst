.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 240

.. _aead:

Authenticated encryption with associated data (AEAD)
====================================================

The single-part AEAD functions are:

*   `psa_aead_encrypt()` to encrypt a message using an authenticated symmetric cipher.
*   `psa_aead_decrypt()` to decrypt a message using an authenticated symmetric cipher.

These functions follow the interface recommended by :RFC-title:`5116`.

The encryption function requires a nonce to be provided. To generate a random nonce, either call `psa_generate_random()` or use the AEAD multi-part API.

The `psa_aead_operation_t` `multi-part operation <multi-part-operations>` permits alternative initialization parameters and allows messages to be processed in fragments. A multi-part AEAD operation is used as follows:

1.  Initialize the `psa_aead_operation_t` object to zero, or by assigning the value of the associated macro `PSA_AEAD_OPERATION_INIT`.
#.  Call `psa_aead_encrypt_setup()` or `psa_aead_decrypt_setup()` to specify the algorithm and key.
#.  Provide additional parameters:

    -   If the algorithm requires it, call `psa_aead_set_lengths()` to specify the length of the non-encrypted and encrypted inputs to the operation.
    -   When encrypting, call either `psa_aead_generate_nonce()` or `psa_aead_set_nonce()` to generate or set the nonce.
    -   When decrypting, call `psa_aead_set_nonce()` to set the nonce.
#.  Call `psa_aead_update_ad()` zero or more times with fragments of the non-encrypted additional data.
#.  Call `psa_aead_update()` zero or more times with fragments of the plaintext or ciphertext to encrypt or decrypt.
#.  At the end of the message, call the required finishing function:

    -   To complete an encryption operation, call `psa_aead_finish()` to compute and return authentication tag.
    -   To complete a decryption operation, call `psa_aead_verify()` to compute the authentication tag and verify it against a reference value.

To abort the operation or recover from an error, call `psa_aead_abort()`.

.. note::
    Using a multi-part interface to authenticated encryption raises specific issues.

    *   Multi-part authenticated decryption produces intermediate results that are not authenticated. Revealing unauthenticated results, either directly or indirectly through the application’s behavior, can compromise the confidentiality of all inputs that are encrypted with the same key. See the :ref:`detailed warning <aead-multi-part-warning>`.

    *   For encryption, some common algorithms cannot be processed in a streaming fashion. For SIV mode, the whole plaintext must be known before the encryption can start; the multi-part AEAD API is not meant to be usable with SIV mode. For CCM mode, the length of the plaintext must be known before the encryption can start; the application can call the function `psa_aead_set_lengths()` to provide these lengths before providing input.

.. _aead-algorithms:

AEAD algorithms
---------------

.. macro:: PSA_ALG_CCM
    :definition: ((psa_algorithm_t)0x05500100)

    .. summary::
        The *Counter with CBC-MAC* (CCM) authenticated encryption algorithm.

    CCM is defined for block ciphers that have a 128-bit block size. The underlying block cipher is determined by the key type.

    To use `PSA_ALG_CCM` with a multi-part AEAD operation, the application must call `psa_aead_set_lengths()` before providing the nonce, the additional data and plaintext to the operation.

    CCM requires a nonce of between 7 and 13 bytes in length. The length of the nonce affects the maximum length of the plaintext than can be encrypted or decrypted. If the nonce has length :math:`N`, then the plaintext length :math:`pLen` is encoded in :math:`L = 15 - N` octets, this requires that :math:`pLen < 2^{8L}`.

    The value for :math:`L` that is used with `PSA_ALG_CCM` depends on the function used to provide the nonce:

    *   A call to `psa_aead_encrypt()`, `psa_aead_decrypt()`, or `psa_aead_set_nonce()` will set :math:`L = 15 - \mathtt{nonce\_length}`. If the plaintext length cannot be encoded in :math:`L` octets, then a :code:`PSA_ERROR_INVALID_ARGUMENT` error is returned.

    *   A call to `psa_aead_generate_nonce()` on a multi-part cipher operation will select the smallest integer :math:`L \geq 2`, where :math:`pLen < 2^{8L}`, with :math:`pLen` being the ``plaintext_length`` provided to `psa_aead_set_lengths()`. The call to `psa_aead_generate_nonce()` will generate and return a random nonce of length :math:`15 - L` bytes.

    CCM supports authentication tag sizes of 4, 6, 8, 10, 12, 14, and 16 bytes. The default tag length is 16. Shortened tag lengths can be requested using :code:`PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_length)`, where ``tag_length`` is a valid CCM tag length.

    The CCM block cipher mode is defined in :RFC-title:`3610`.

    .. subsection:: Usage in Zigbee

        The CCM* algorithm is required by :cite-title:`ZIGBEE`.

        *   `PSA_ALG_CCM`, and its truncated variants, can be used to implement CCM* for non-zero tag lengths.
        *   For unauthenticated CCM*, with a zero-length tag, use the `PSA_ALG_CCM_STAR_NO_TAG` cipher algorithm.

        See also :ref:`Usage in Zigbee <using-ccm-star-no-tag>` under `PSA_ALG_CCM_STAR_NO_TAG`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_GCM
    :definition: ((psa_algorithm_t)0x05500200)

    .. summary::
        The *Galois/Counter Mode* (GCM) authenticated encryption algorithm.

    GCM is defined for block ciphers that have a 128-bit block size. The underlying block cipher is determined by the key type.

    GCM requires a nonce of at least 1 byte in length. The maximum supported nonce size is :scterm:`implementation defined`. Calling `psa_aead_generate_nonce()` will generate a random 12-byte nonce.

    GCM supports authentication tag sizes of 4, 8, 12, 13, 14, 15, and 16 bytes. The default tag length is 16. Shortened tag lengths can be requested using :code:`PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, tag_length)`, where ``tag_length`` is a valid GCM tag length.

    The GCM block cipher mode is defined in :cite-title:`SP800-38D`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES`
        | `PSA_KEY_TYPE_ARIA`
        | `PSA_KEY_TYPE_CAMELLIA`
        | `PSA_KEY_TYPE_SM4`

.. macro:: PSA_ALG_CHACHA20_POLY1305
    :definition: ((psa_algorithm_t)0x05100500)

    .. summary::
        The ChaCha20-Poly1305 AEAD algorithm.

    There are two defined variants of ChaCha20-Poly1305:

    *   An implementation that supports ChaCha20-Poly1305 must support the variant defined by :rfc-title:`8439`, which has a 96-bit nonce and 32-bit counter.
    *   An implementation can optionally also support the original variant defined by :cite-title:`CHACHA20`, which has a 64-bit nonce and 64-bit counter.

    The variant used for the AEAD encryption or decryption operation, depends on the nonce provided for an AEAD operation using `PSA_ALG_CHACHA20_POLY1305`:

    *   A nonce provided in a call to `psa_aead_encrypt()`, `psa_aead_decrypt()` or `psa_aead_set_nonce()` must be 8 or 12 bytes. The size of nonce will select the appropriate variant of the algorithm.

    *   A nonce generated by a call to `psa_aead_generate_nonce()` will be 12 bytes, and will use the :rfc:`8439` variant.

    Implementations must support 16-byte tags. It is recommended that truncated tag sizes are rejected.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_CHACHA20`

.. macro:: PSA_ALG_XCHACHA20_POLY1305
    :definition: ((psa_algorithm_t)0x05100600)

    .. summary::
        The XChaCha20-Poly1305 AEAD algorithm.

        .. versionadded:: 1.2

    XChaCha20-Poly1305 is a variation of the ChaCha20-Poly1305 AEAD algorithm, but uses a 192-bit nonce. The larger nonce provides much lower probability of nonce misuse.

    XChaCha20-Poly1305 requires a 24-byte nonce.

    Implementations must support 16-byte tags. It is recommended that truncated tag sizes are rejected.

    XChaCha20-Poly1305 is defined in :cite-title:`XCHACHA`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_XCHACHA20`

.. macro:: PSA_ALG_ASCON_AEAD128
    :definition: ((psa_algorithm_t)0x05100700)

    .. summary::
        The Ascon-AEAD128 AEAD algorithm.

        .. versionadded:: 1.4

    There are two variants of Ascon-AEAD128 defined in :cite-title:`SP800-232`:

    *   An implementation that supports Ascon-AEAD128 must provide the standard variant, using a 128-bit key.
        This is defined in `[SP800-232]` §4.1.
    *   An implementation can optionally also provide the nonce-masking variant, using a 256-bit key.
        This is defined in `[SP800-232]` §4.2.2.

    The variant is selected based on the size of the key.

    Both variants require a 128-bit (16 byte) nonce, which must not be reused with the same key.

    Implementations must support 16-byte tags.
    Truncated tags of at least 4 bytes are permitted, but it is recommended that truncated tag sizes are at least 8 bytes.
    See `[SP800-232]` §4.2.1 and §4.3.R4.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_ASCON`

.. macro:: PSA_ALG_AEAD_WITH_SHORTENED_TAG
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a AEAD algorithm with a shortened tag.

    .. param:: aead_alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(aead_alg)` is true.
    .. param:: tag_length
        Desired length of the authentication tag in bytes.

    .. return::
        The corresponding AEAD algorithm with the specified tag length.

        Unspecified if ``aead_alg`` is not a supported AEAD algorithm or if ``tag_length`` is not valid for the specified AEAD algorithm.

    An AEAD algorithm with a shortened tag is similar to the corresponding AEAD algorithm, but has an authentication tag that consists of fewer bytes. Depending on the algorithm, the tag length might affect the calculation of the ciphertext.

    The AEAD algorithm with a default length tag can be recovered using `PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG()`.

    .. subsection:: Compatible key types

        The resulting AEAD algorithm is compatible with the same key types as the AEAD algorithm used to construct it.

.. macro:: PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG
    :definition: /* specification-defined value */

    .. summary::
        An AEAD algorithm with the default tag length.

    .. param:: aead_alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(aead_alg)` is true.

    .. return::
        The corresponding AEAD algorithm with the default tag length for that algorithm.

    This macro can be used to construct the AEAD algorithm with default tag length from an AEAD algorithm with a shortened tag. See also `PSA_ALG_AEAD_WITH_SHORTENED_TAG()`.

    .. subsection:: Compatible key types

        The resulting AEAD algorithm is compatible with the same key types as the AEAD algorithm used to construct it.

.. macro:: PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an AEAD minimum-tag-length wildcard algorithm.

        .. versionadded:: 1.1

    .. param:: aead_alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(aead_alg)` is true.
    .. param:: min_tag_length
        Desired minimum length of the authentication tag in bytes. This must be at least ``1`` and at most the largest permitted tag length of the algorithm.

    .. return::
        The corresponding AEAD wildcard algorithm with the specified minimum tag length.

        Unspecified if ``aead_alg`` is not a supported AEAD algorithm or if ``min_tag_length`` is less than ``1`` or too large for the specified AEAD algorithm.

    A key with a minimum-tag-length AEAD wildcard algorithm as permitted-algorithm policy can be used with all AEAD algorithms sharing the same base algorithm, and where the tag length of the specific algorithm is equal to or larger then the minimum tag length specified by the wildcard algorithm.

    .. note::
        When setting the minimum required tag length to less than the smallest tag length permitted by the base algorithm, this effectively becomes an 'any-tag-length-permitted' policy for that base algorithm.

    The AEAD algorithm with a default length tag can be recovered using `PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG()`.

    .. subsection:: Compatible key types

        The resulting wildcard AEAD algorithm is compatible with the same key types as the AEAD algorithm used to construct it.


Single-part AEAD functions
--------------------------

.. function:: psa_aead_encrypt

    .. summary::
        Process an authenticated encryption operation.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must permit the usage `PSA_KEY_USAGE_ENCRYPT`.
    .. param:: psa_algorithm_t alg
        The AEAD algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.
    .. param:: const uint8_t * nonce
        Nonce or IV to use.
    .. param:: size_t nonce_length
        Size of the ``nonce`` buffer in bytes. This must be appropriate for the selected algorithm. The default nonce size is :code:`PSA_AEAD_NONCE_LENGTH(key_type, alg)` where ``key_type`` is the type of ``key``.
    .. param:: const uint8_t * additional_data
        Additional data that will be authenticated but not encrypted.
    .. param:: size_t additional_data_length
        Size of ``additional_data`` in bytes.
    .. param:: const uint8_t * plaintext
        Data that will be authenticated and encrypted.
    .. param:: size_t plaintext_length
        Size of ``plaintext`` in bytes.
    .. param:: uint8_t * ciphertext
        Output buffer for the authenticated and encrypted data. The additional data is not part of this output. For algorithms where the encrypted data and the authentication tag are defined as separate outputs, the authentication tag is appended to the encrypted data.
    .. param:: size_t ciphertext_size
        Size of the ``ciphertext`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length)`  where ``key_type`` is the type of ``key``.
        *   :code:`PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length)` evaluates to the maximum ciphertext size of any supported AEAD encryption.

    .. param:: size_t * ciphertext_length
        On success, the size of the output in the ``ciphertext`` buffer.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*ciphertext_length)`` bytes of ``ciphertext`` contain the output.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_ENCRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an AEAD algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   ``nonce_length`` is not valid for use with ``alg`` and ``key``.
        *   ``additional_data_length`` or ``plaintext_length`` are too large for ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an AEAD algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``nonce_length`` is not supported for use with ``alg`` and ``key``.
        *   ``additional_data_length`` or ``plaintext_length`` are too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``ciphertext`` buffer is too small. `PSA_AEAD_ENCRYPT_OUTPUT_SIZE()` or `PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

.. function:: psa_aead_decrypt

    .. summary::
        Process an authenticated decryption operation.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must permit the usage `PSA_KEY_USAGE_DECRYPT`.
    .. param:: psa_algorithm_t alg
        The AEAD algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.
    .. param:: const uint8_t * nonce
        Nonce or IV to use.
    .. param:: size_t nonce_length
        Size of the ``nonce`` buffer in bytes. This must be appropriate for the selected algorithm. The default nonce size is :code:`PSA_AEAD_NONCE_LENGTH(key_type, alg)` where ``key_type`` is the type of ``key``.
    .. param:: const uint8_t * additional_data
        Additional data that has been authenticated but not encrypted.
    .. param:: size_t additional_data_length
        Size of ``additional_data`` in bytes.
    .. param:: const uint8_t * ciphertext
        Data that has been authenticated and encrypted. For algorithms where the encrypted data and the authentication tag are defined as separate inputs, the buffer must contain the encrypted data followed by the authentication tag.
    .. param:: size_t ciphertext_length
        Size of ``ciphertext`` in bytes.
    .. param:: uint8_t * plaintext
        Output buffer for the decrypted data.
    .. param:: size_t plaintext_size
        Size of the ``plaintext`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length)`  where ``key_type`` is the type of ``key``.
        *   :code:`PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length)` evaluates to the maximum plaintext size of any supported AEAD decryption.

    .. param:: size_t * plaintext_length
        On success, the size of the output in the ``plaintext`` buffer.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*plaintext_length)`` bytes of ``plaintext`` contain the output.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The ciphertext is not authentic.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_DECRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an AEAD algorithm.
        *   ``key`` is not compatible with ``alg``.
        *   ``nonce_length`` is not valid for use with ``alg`` and ``key``.
        *   ``additional_data_length`` or ``ciphertext_length`` are too large for ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an AEAD algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``nonce_length`` is not supported for use with ``alg`` and ``key``.
        *   ``additional_data_length`` or ``plaintext_length`` are too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``plaintext`` buffer is too small. `PSA_AEAD_DECRYPT_OUTPUT_SIZE()` or `PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

Multi-part AEAD operations
--------------------------

.. _aead-multi-part-warning:

.. warning::
    When decrypting using a multi-part AEAD operation, there is no guarantee that the input or output is valid until `psa_aead_verify()` has returned :code:`PSA_SUCCESS`.

    A call to `psa_aead_update()` or `psa_aead_update_ad()` returning :code:`PSA_SUCCESS` **does not** indicate that the input and output is valid.

    Until an application calls `psa_aead_verify()` and it has returned :code:`PSA_SUCCESS`, the following rules apply to input and output data from a multi-part AEAD operation:

    *   Do not trust the input. If the application takes any action that depends on the input data, this action will need to be undone if the input turns out to be invalid.

    *   Store the output in a confidential location. In particular, the application must not copy the output to a memory or storage space which is shared.

    *   Do not trust the output. If the application takes any action that depends on the tentative decrypted data, this action will need to be undone if the input turns out to be invalid. Furthermore, if an adversary can observe that this action took place, for example, through timing, they might be able to use this fact as an oracle to decrypt any message encrypted with the same key.

    An application that does not follow these rules might be vulnerable to maliciously constructed AEAD input data.


.. typedef:: /* implementation-defined type */ psa_aead_operation_t

    .. summary::
        The type of the state object for multi-part AEAD operations.

    Before calling any function on an AEAD operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_aead_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_aead_operation_t operation;

    *   Initialize the object to the initializer `PSA_AEAD_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_aead_operation_t operation = PSA_AEAD_OPERATION_INIT;

    *   Assign the result of the function `psa_aead_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_aead_operation_t operation;
            operation = psa_aead_operation_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_AEAD_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for an AEAD operation object of type `psa_aead_operation_t`.

.. function:: psa_aead_operation_init

    .. summary::
        Return an initial value for an AEAD operation object.

    .. return:: psa_aead_operation_t

.. function:: psa_aead_encrypt_setup

    .. summary::
        Set the key for a multi-part authenticated encryption operation.

    .. param:: psa_aead_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_aead_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_ENCRYPT`.
    .. param:: psa_algorithm_t alg
        The AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_ENCRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an AEAD algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an AEAD algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The sequence of operations to encrypt a message with authentication is as follows:

    1.  Allocate an AEAD operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_aead_operation_t`, e.g. `PSA_AEAD_OPERATION_INIT`.
    #.  Call `psa_aead_encrypt_setup()` to specify the algorithm and key.
    #.  If needed, call `psa_aead_set_lengths()` to specify the length of the inputs to the subsequent calls to `psa_aead_update_ad()` and `psa_aead_update()`. See the documentation of `psa_aead_set_lengths()` for details.
    #.  Call either `psa_aead_generate_nonce()` or `psa_aead_set_nonce()` to generate or set the nonce. It is recommended to use `psa_aead_generate_nonce()` unless the protocol being implemented requires a specific nonce value.
    #.  Call `psa_aead_update_ad()` zero, one or more times, passing a fragment of the non-encrypted additional authenticated data each time.
    #.  Call `psa_aead_update()` zero, one or more times, passing a fragment of the message to encrypt each time.
    #.  Call `psa_aead_finish()`.

    After a successful call to `psa_aead_encrypt_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_aead_finish()`.
    *   A call to `psa_aead_abort()`.

    If `psa_aead_encrypt_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_aead_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_aead_decrypt_setup

    .. summary::
        Set the key for a multi-part authenticated decryption operation.

    .. param:: psa_aead_operation_t * operation
        The operation object to set up. It must have been initialized as per the documentation for `psa_aead_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must remain valid until the operation terminates.
        It must permit the usage `PSA_KEY_USAGE_DECRYPT`.
    .. param:: psa_algorithm_t alg
        The AEAD algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_DECRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an AEAD algorithm.
        *   ``key`` is not compatible with ``alg``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an AEAD algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The sequence of operations to decrypt a message with authentication is as follows:

    1.  Allocate an AEAD operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_aead_operation_t`, e.g. `PSA_AEAD_OPERATION_INIT`.
    #.  Call `psa_aead_decrypt_setup()` to specify the algorithm and key.
    #.  If needed, call `psa_aead_set_lengths()` to specify the length of the inputs to the subsequent calls to `psa_aead_update_ad()` and `psa_aead_update()`. See the documentation of `psa_aead_set_lengths()` for details.
    #.  Call `psa_aead_set_nonce()` with the nonce for the decryption.
    #.  Call `psa_aead_update_ad()` zero, one or more times, passing a fragment of the non-encrypted additional authenticated data each time.
    #.  Call `psa_aead_update()` zero, one or more times, passing a fragment of the ciphertext to decrypt each time.
    #.  Call `psa_aead_verify()`.

    After a successful call to `psa_aead_decrypt_setup()`, the operation is active, and the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_aead_verify()`.
    *   A call to `psa_aead_abort()`.

    If `psa_aead_decrypt_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_aead_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_aead_set_lengths

    .. summary::
        Declare the lengths of the message and additional data for AEAD.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: size_t ad_length
        Size of the non-encrypted additional authenticated data in bytes.
    .. param:: size_t plaintext_length
        Size of the plaintext to encrypt in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and `psa_aead_set_nonce()` and `psa_aead_generate_nonce()` must not have been called yet.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``ad_length`` or ``plaintext_length`` are too large for the chosen algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``ad_length`` or ``plaintext_length`` are too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call this function before calling `psa_aead_set_nonce()` or `psa_aead_generate_nonce()`, if the algorithm for the operation requires it. If the algorithm does not require it, calling this function is optional, but if this function is called then the implementation must enforce the lengths.

    *   For `PSA_ALG_CCM`, calling this function is required.
    *   For the other AEAD algorithms defined in this specification, calling this function is not required.
    *   For vendor-defined algorithm, refer to the vendor documentation.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

.. function:: psa_aead_generate_nonce

    .. summary::
        Generate a random nonce for an authenticated encryption operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: uint8_t * nonce
        Buffer where the generated nonce is to be written.
    .. param:: size_t nonce_size
        Size of the ``nonce`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_NONCE_LENGTH(key_type, alg)` where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   `PSA_AEAD_NONCE_MAX_SIZE` evaluates to a sufficient output size for any supported AEAD algorithm.
    .. param:: size_t * nonce_length
        On success, the number of bytes of the generated nonce.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*nonce_length)`` bytes of ``nonce`` contain the generated nonce.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be an active AEAD encryption operation, with no nonce set.
        *   The operation state is not valid: this is an algorithm which requires `psa_aead_set_lengths()` to be called before setting the nonce.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``nonce`` buffer is too small. `PSA_AEAD_NONCE_LENGTH()` or `PSA_AEAD_NONCE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function generates a random nonce for the authenticated encryption operation with an appropriate size for the chosen algorithm, key type and key size.

    Most algorithms generate a default-length nonce, as returned by `PSA_AEAD_NONCE_LENGTH()`. Some algorithms can return a shorter nonce from `psa_aead_generate_nonce()`, see the individual algorithm descriptions for details.

    The application must call `psa_aead_encrypt_setup()` before calling this function. If applicable for the algorithm, the application must call `psa_aead_set_lengths()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

.. function:: psa_aead_set_nonce

    .. summary::
        Set the nonce for an authenticated encryption or decryption operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: const uint8_t * nonce
        Buffer containing the nonce to use.
    .. param:: size_t nonce_length
        Size of the nonce in bytes. This must be a valid nonce size for the chosen algorithm. The default nonce size is :code:`PSA_AEAD_NONCE_LENGTH(key_type, alg)` where ``key_type`` and ``alg`` are type of key and the algorithm respectively that were used to set up the AEAD operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with no nonce set.
        *   The operation state is not valid: this is an algorithm which requires `psa_aead_set_lengths()` to be called before setting the nonce.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``nonce_length`` is not valid for the chosen algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``nonce_length`` is not supported for use with the operation's algorithm and key.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function sets the nonce for the authenticated encryption or decryption operation.

    The application must call `psa_aead_encrypt_setup()` or `psa_aead_decrypt_setup()` before calling this function. If applicable for the algorithm, the application must call `psa_aead_set_lengths()` before calling this function.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

    .. note::
        When encrypting, `psa_aead_generate_nonce()` is recommended instead of using this function, unless implementing a protocol that requires a non-random IV.

.. function:: psa_aead_update_ad

    .. summary::
        Pass additional data to an active AEAD operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: const uint8_t * input
        Buffer containing the fragment of additional data.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.

        .. warning::
            When decrypting, do not trust the additional data until `psa_aead_verify()` succeeds.

            See the :ref:`detailed warning <aead-multi-part-warning>`.

    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, have a nonce set, have lengths set if required by the algorithm, and `psa_aead_update()` must not have been called yet.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        Excess additional data: the total input length to `psa_aead_update_ad()` is greater than the additional data length that was previously specified with `psa_aead_set_lengths()`, or is too large for the chosen AEAD algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total additional data length is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    Additional data is authenticated, but not encrypted.

    This function can be called multiple times to pass successive fragments of the additional data. This function must not be called after passing data to encrypt or decrypt with `psa_aead_update()`.

    The following must occur before calling this function:

    1.  Call either `psa_aead_encrypt_setup()` or `psa_aead_decrypt_setup()`.
    #.  Set the nonce with `psa_aead_generate_nonce()` or `psa_aead_set_nonce()`.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

.. function:: psa_aead_update

    .. summary::
        Encrypt or decrypt a message fragment in an active AEAD operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to encrypt or decrypt.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * output
        Buffer where the output is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length)` where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   :code:`PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length)` evaluates to the maximum output size of any supported AEAD algorithm.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` of ``output`` contains the output data.

        .. warning::
            When decrypting, do not use the output until `psa_aead_verify()` succeeds.

            See the :ref:`detailed warning <aead-multi-part-warning>`.

    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, have a nonce set, and have lengths set if required by the algorithm.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small. `PSA_AEAD_UPDATE_OUTPUT_SIZE()` or `PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE()` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   Incomplete additional data: the total length of input to `psa_aead_update_ad()` is less than the additional data length that was previously specified with `psa_aead_set_lengths()`.
        *   Excess input data: the total length of input to `psa_aead_update()` is greater than the plaintext length that was previously specified with `psa_aead_set_lengths()`, or is too large for the specific AEAD algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input length is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The following must occur before calling this function:

    1.  Call either `psa_aead_encrypt_setup()` or `psa_aead_decrypt_setup()`. The choice of setup function determines whether this function encrypts or decrypts its input.
    #.  Set the nonce with `psa_aead_generate_nonce()` or `psa_aead_set_nonce()`.
    #.  Call `psa_aead_update_ad()` to pass all the additional data.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

    .. note::

        This function does not require the input to be aligned to any particular block boundary. If the implementation can only process a whole block at a time, it must consume all the input provided, but it might delay the end of the corresponding output until a subsequent call to `psa_aead_update()` provides sufficient input, or a subsequent call to `psa_aead_finish()` or `psa_aead_verify()` indicates the end of the input. The amount of data that can be delayed in this way is bounded by the associated output size macro: `PSA_AEAD_UPDATE_OUTPUT_SIZE()`, `PSA_AEAD_FINISH_OUTPUT_SIZE()`, or `PSA_AEAD_VERIFY_OUTPUT_SIZE()`.

.. function:: psa_aead_finish

    .. summary::
        Finish encrypting a message in an AEAD operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: uint8_t * ciphertext
        Buffer where the last part of the ciphertext is to be written.
    .. param:: size_t ciphertext_size
        Size of the ``ciphertext`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg)` where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   `PSA_AEAD_FINISH_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported AEAD algorithm.

    .. param:: size_t * ciphertext_length
        On success, the number of bytes of returned ciphertext.
    .. param:: uint8_t * tag
        Buffer where the authentication tag is to be written.
    .. param:: size_t tag_size
        Size of the ``tag`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The exact tag size is :code:`PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size of the key, and ``alg`` is the algorithm that were used in the call to `psa_aead_encrypt_setup()`.
        *   `PSA_AEAD_TAG_MAX_SIZE` evaluates to the maximum tag size of any supported AEAD algorithm.

    .. param:: size_t * tag_length
        On success, the number of bytes that make up the returned tag.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*tag_length)`` bytes of ``tag`` contain the authentication tag.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be an active encryption operation with a nonce set.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``ciphertext`` or ``tag`` buffer is too small.
        `PSA_AEAD_FINISH_OUTPUT_SIZE()` or `PSA_AEAD_FINISH_OUTPUT_MAX_SIZE` can be used to determine the required ``ciphertext`` buffer size.
        `PSA_AEAD_TAG_LENGTH()` or `PSA_AEAD_TAG_MAX_SIZE` can be used to determine the required ``tag`` buffer size.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   Incomplete additional data: the total length of input to `psa_aead_update_ad()` is less than the additional data length that was previously specified with `psa_aead_set_lengths()`.
        *   Incomplete plaintext: the total length of input to `psa_aead_update()` is less than the plaintext length that was previously specified with `psa_aead_set_lengths()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The operation must have been set up with `psa_aead_encrypt_setup()`.

    This function finishes the authentication of the additional data formed by concatenating the inputs passed to preceding calls to `psa_aead_update_ad()` with the plaintext formed by concatenating the inputs passed to preceding calls to `psa_aead_update()`.

    This function has two output buffers:

    *   ``ciphertext`` contains trailing ciphertext that was buffered from preceding calls to `psa_aead_update()`.
    *   ``tag`` contains the authentication tag.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

.. function:: psa_aead_verify

    .. summary::
        Finish authenticating and decrypting a message in an AEAD operation.

    .. param:: psa_aead_operation_t * operation
        Active AEAD operation.
    .. param:: uint8_t * plaintext
        Buffer where the last part of the plaintext is to be written. This is the remaining data from previous calls to `psa_aead_update()` that could not be processed until the end of the input.
    .. param:: size_t plaintext_size
        Size of the ``plaintext`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   A sufficient output size is :code:`PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg)` where ``key_type`` is the type of key and ``alg`` is the algorithm that were used to set up the operation.
        *   `PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported AEAD algorithm.

    .. param:: size_t * plaintext_length
        On success, the number of bytes of returned plaintext.
    .. param:: const uint8_t * tag
        Buffer containing the expected authentication tag.
    .. param:: size_t tag_length
        Size of the ``tag`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        For a decryption operation, it is now safe to use the additional data and the plaintext output.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The calculated authentication tag does not match the value in ``tag``.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be an active decryption operation with a nonce set.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``plaintext`` buffer is too small. `PSA_AEAD_VERIFY_OUTPUT_SIZE()` or `PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   Incomplete additional data: the total length of input to `psa_aead_update_ad()` is less than the additional data length that was previously specified with `psa_aead_set_lengths()`.
        *   Incomplete ciphertext: the total length of input to `psa_aead_update()` is less than the plaintext length that was previously specified with `psa_aead_set_lengths()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The operation must have been set up with `psa_aead_decrypt_setup()`.

    This function finishes the authenticated decryption of the message components:

    *   The additional data consisting of the concatenation of the inputs passed to preceding calls to `psa_aead_update_ad()`.
    *   The ciphertext consisting of the concatenation of the inputs passed to preceding calls to `psa_aead_update()`.
    *   The tag passed to this function call.

    If the authentication tag is correct, this function outputs any remaining plaintext and reports success. If the authentication tag is not correct, this function returns :code:`PSA_ERROR_INVALID_SIGNATURE`.

    When this function returns successfully, the operation becomes inactive. If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_aead_abort()`.

    .. admonition:: Implementation note

        Implementations must make the best effort to ensure that the comparison between the actual tag and the expected tag is performed in constant time.

.. function:: psa_aead_abort

    .. summary::
        Abort an AEAD operation.

    .. param:: psa_aead_operation_t * operation
        Initialized AEAD operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_aead_encrypt_setup()` or `psa_aead_decrypt_setup()` again.

    This function can be called any time after the operation object has been initialized as described in `psa_aead_operation_t`.

    In particular, calling `psa_aead_abort()` after the operation has been terminated by a call to `psa_aead_abort()`, `psa_aead_finish()` or `psa_aead_verify()` is safe and has no effect.

Support macros
--------------

.. macro:: PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an AEAD mode on a block cipher.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an AEAD algorithm which is an AEAD mode based on a block cipher, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_AEAD_ENCRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient ciphertext buffer size for `psa_aead_encrypt()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.
    .. param:: plaintext_length
        Size of the plaintext in bytes.

    .. return::
        The AEAD ciphertext size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the ciphertext buffer is at least this large, it is guaranteed that `psa_aead_encrypt()` will not fail due to an insufficient buffer size. Depending on the algorithm, the actual size of the ciphertext might be smaller.

    See also `PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient ciphertext buffer size for `psa_aead_encrypt()`, for any of the supported key types and AEAD algorithms.

    .. param:: plaintext_length
        Size of the plaintext in bytes.

    If the size of the ciphertext buffer is at least this large, it is guaranteed that `psa_aead_encrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_ENCRYPT_OUTPUT_SIZE()`.

.. macro:: PSA_AEAD_DECRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient plaintext buffer size for `psa_aead_decrypt()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.
    .. param:: ciphertext_length
        Size of the ciphertext in bytes.

    .. return::
        The AEAD plaintext size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the plaintext buffer is at least this large, it is guaranteed that `psa_aead_decrypt()` will not fail due to an insufficient buffer size. Depending on the algorithm, the actual size of the plaintext might be smaller.

    See also `PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient plaintext buffer size for `psa_aead_decrypt()`, for any of the supported key types and AEAD algorithms.

    .. param:: ciphertext_length
        Size of the ciphertext in bytes.

    If the size of the plaintext buffer is at least this large, it is guaranteed that `psa_aead_decrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_DECRYPT_OUTPUT_SIZE()`.

.. macro:: PSA_AEAD_NONCE_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The default nonce size for an AEAD algorithm, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return::
        The default nonce size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the nonce buffer is at least this large, it is guaranteed that `psa_aead_generate_nonce()` will not fail due to an insufficient buffer size.

    For most AEAD algorithms, `PSA_AEAD_NONCE_LENGTH()` evaluates to the exact size of the nonce generated by `psa_aead_generate_nonce()`.

    See also `PSA_AEAD_NONCE_MAX_SIZE`.

.. macro:: PSA_AEAD_NONCE_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient buffer size for storing the nonce generated by `psa_aead_generate_nonce()`, for any of the supported key types and AEAD algorithms.

    If the size of the nonce buffer is at least this large, it is guaranteed that `psa_aead_generate_nonce()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_NONCE_LENGTH()`.

.. macro:: PSA_AEAD_UPDATE_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_aead_update()`.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.
    .. param:: input_length
        Size of the input in bytes.

    .. return::
        A sufficient output buffer size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_aead_update()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE`.

.. macro:: PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_aead_update()`, for any of the supported key types and AEAD algorithms.

    .. param:: input_length
        Size of the input in bytes.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_aead_update()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_UPDATE_OUTPUT_SIZE()`.

.. macro:: PSA_AEAD_FINISH_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient ciphertext buffer size for `psa_aead_finish()`.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return::
        A sufficient ciphertext buffer size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the ciphertext buffer is at least this large, it is guaranteed that `psa_aead_finish()` will not fail due to an insufficient ciphertext buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_AEAD_FINISH_OUTPUT_MAX_SIZE`.

.. macro:: PSA_AEAD_FINISH_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient ciphertext buffer size for `psa_aead_finish()`, for any of the supported key types and AEAD algorithms.

    If the size of the ciphertext buffer is at least this large, it is guaranteed that `psa_aead_finish()` will not fail due to an insufficient ciphertext buffer size.

    See also `PSA_AEAD_FINISH_OUTPUT_SIZE()`.

.. macro:: PSA_AEAD_TAG_LENGTH
    :definition: /* implementation-defined value */

    .. summary::
        The length of a tag for an AEAD algorithm, in bytes.

    .. param:: key_type
        The type of the AEAD key.
    .. param:: key_bits
        The size of the AEAD key in bits.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return::
        The tag length for the specified algorithm and key.
        If the AEAD algorithm does not have an identified tag that can be distinguished from the rest of the ciphertext, return ``0``. If the AEAD algorithm is not recognized, return ``0``. An implementation can return either ``0`` or a correct size for an AEAD algorithm that it recognizes, but does not support.

    This is the size of the tag output from `psa_aead_finish()`.

    If the size of the tag buffer is at least this large, it is guaranteed that `psa_aead_finish()` will not fail due to an insufficient tag buffer size.

    See also `PSA_AEAD_TAG_MAX_SIZE`.

.. macro:: PSA_AEAD_TAG_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient buffer size for storing the tag output by `psa_aead_finish()`, for any of the supported key types and AEAD algorithms.

    If the size of the tag buffer is at least this large, it is guaranteed that `psa_aead_finish()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_TAG_LENGTH()`.

.. macro:: PSA_AEAD_VERIFY_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient plaintext buffer size for `psa_aead_verify()`, in bytes.

    .. param:: key_type
        A symmetric key type that is compatible with algorithm ``alg``.
    .. param:: alg
        An AEAD algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_AEAD(alg)` is true.

    .. return::
        A sufficient plaintext buffer size for the specified key type and algorithm. If the key type or AEAD algorithm is not recognized, or the parameters are incompatible, return ``0``. An implementation can return either ``0`` or a correct size for a key type and AEAD algorithm that it recognizes, but does not support.

    If the size of the plaintext buffer is at least this large, it is guaranteed that `psa_aead_verify()` will not fail due to an insufficient plaintext buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE`.

.. macro:: PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient plaintext buffer size for `psa_aead_verify()`, for any of the supported key types and AEAD algorithms.

    If the size of the plaintext buffer is at least this large, it is guaranteed that `psa_aead_verify()` will not fail due to an insufficient buffer size.

    See also `PSA_AEAD_VERIFY_OUTPUT_SIZE()`.
