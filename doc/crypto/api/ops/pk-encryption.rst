.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 270

.. _pke:

Asymmetric encryption
=====================

Asymmetric encryption is provided through the functions `psa_asymmetric_encrypt()` and `psa_asymmetric_decrypt()`.

.. _asymmetric-encryption-algorithms:

Asymmetric encryption algorithms
--------------------------------

.. macro:: PSA_ALG_RSA_PKCS1V15_CRYPT
    :definition: ((psa_algorithm_t)0x07000200)

    .. summary::
        The RSA PKCS#1 v1.5 asymmetric encryption algorithm.

    This encryption scheme is defined by :RFC-title:`8017#7.2` under the name RSAES-PKCS-v1_5.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (asymmetric encryption only)

.. macro:: PSA_ALG_RSA_OAEP
    :definition: /* specification-defined value */

    .. summary::
        The RSA OAEP asymmetric encryption algorithm.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. The hash algorithm is used for *MGF1*.

    .. return::
        The corresponding RSA OAEP encryption algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This encryption scheme is defined by :RFC:`8017#7.1` under the name RSAES-OAEP, with the following options:

    *   The mask generation function *MGF1* defined in :RFC:`8017#B.2.1`.
    *   The specified hash algorithm is used to hash the label, and for the mask generation function.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (asymmetric encryption only)

Asymmetric encryption functions
-------------------------------

.. function:: psa_asymmetric_encrypt

    .. summary::
        Encrypt a short message with a public key.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be a public key or an asymmetric key pair.
        It must permit the usage `PSA_KEY_USAGE_ENCRYPT`.
    .. param:: psa_algorithm_t alg
        The asymmetric encryption algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)` is true.
    .. param:: const uint8_t * input
        The message to encrypt.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * salt
        A salt or label, if supported by the encryption algorithm. If the algorithm does not support a salt, pass ``NULL``. If the algorithm supports an optional salt, pass ``NULL`` to indicate that there is no salt.
    .. param:: size_t salt_length
        Size of the ``salt`` buffer in bytes. If ``salt`` is ``NULL``, pass ``0``.
    .. param:: uint8_t * output
        Buffer where the encrypted message is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required output size is :code:`PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported asymmetric encryption.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the encrypted output.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_ENCRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small.
        `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE()` or `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an asymmetric encryption algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` or ``salt_length`` are too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric encryption algorithm.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is not valid for the algorithm and key type.
        *   ``salt_length`` is not valid for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    *   For `PSA_ALG_RSA_PKCS1V15_CRYPT`, no salt is supported.

.. function:: psa_asymmetric_decrypt

    .. summary::
        Decrypt a short message with a private key.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair.
        It must permit the usage `PSA_KEY_USAGE_DECRYPT`.
    .. param:: psa_algorithm_t alg
        The asymmetric encryption algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)` is true.
    .. param:: const uint8_t * input
        The message to decrypt.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * salt
        A salt or label, if supported by the encryption algorithm. If the algorithm does not support a salt, pass ``NULL``. If the algorithm supports an optional salt, pass ``NULL`` to indicate that there is no salt.
    .. param:: size_t salt_length
        Size of the ``salt`` buffer in bytes. If ``salt`` is ``NULL``, pass ``0``.
    .. param:: uint8_t * output
        Buffer where the decrypted message is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required output size is :code:`PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported asymmetric decryption.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the decrypted output.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_DECRYPT` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small.
        `PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE()` or `PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an asymmetric encryption algorithm.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` or ``salt_length`` are too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric encryption algorithm.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is not valid for the algorithm and key type.
        *   ``salt_length`` is not valid for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INVALID_PADDING
        The algorithm uses padding, and the input does not contain valid padding.
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    *   For `PSA_ALG_RSA_PKCS1V15_CRYPT`, no salt is supported.

Support macros
--------------

.. macro:: PSA_ALG_IS_RSA_OAEP
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an RSA OAEP encryption algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an RSA OAEP algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_asymmetric_encrypt()`.

    .. param:: key_type
        An asymmetric key type, either a key pair or a public key.
    .. param:: key_bits
        The size of the key in bits.
    .. param:: alg
        An asymmetric encryption algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)` is true.

    .. return::
        A sufficient output buffer size for the specified asymmetric encryption algorithm and key parameters. An implementation can return either ``0`` or a correct size for an asymmetric encryption algorithm and key parameters that it recognizes, but does not support. If the parameters are not valid, the return value is unspecified.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_asymmetric_encrypt()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_asymmetric_encrypt()`, for any of the supported key types and asymmetric encryption algorithms.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_asymmetric_encrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE()`.

.. macro:: PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_asymmetric_decrypt()`.

    .. param:: key_type
        An asymmetric key type, either a key pair or a public key.
    .. param:: key_bits
        The size of the key in bits.
    .. param:: alg
        An asymmetric encryption algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)` is true.

    .. return::
        A sufficient output buffer size for the specified asymmetric encryption algorithm and key parameters. An implementation can return either ``0`` or a correct size for an asymmetric encryption algorithm and key parameters that it recognizes, but does not support. If the parameters are not valid, the return value is unspecified.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_asymmetric_decrypt()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient output buffer size for `psa_asymmetric_decrypt()`, for any of the supported key types and asymmetric encryption algorithms.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_asymmetric_decrypt()` will not fail due to an insufficient buffer size.

    See also `PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE()`.
