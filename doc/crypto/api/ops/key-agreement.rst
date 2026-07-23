.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 280

.. _key-agreement:

Key agreement
=============

Three functions are provided for a Diffie-Hellman-style key agreement where each party combines its own private key with the peer's public key, to produce a shared secret value:

*   A call to `psa_key_agreement()` will compute the shared secret and store the result in a new derivation key.

*   If the resulting shared secret will be used for a single key derivation, a :ref:`key-derivation operation <kdf>` can be used with the `psa_key_derivation_key_agreement()` input function. This calculates the shared secret and inputs it directly to the key-derivation operation.

*   Where an application needs direct access to the shared secret, it can call `psa_raw_key_agreement()` instead.

If an application requires bounded execution during a key agreement, it can use an interruptible key-agreement operation.
See :secref:`interruptible-key-agreement`.

Using `psa_key_agreement()`, `psa_key_derivation_key_agreement()`, or an interruptible key-agreement operation is recommended, as these do not expose the shared secret to the application.

.. note::

    In general the shared secret is not directly suitable for use as a key because it is biased.

.. _key-agreement-algorithms:

Key-agreement algorithms
------------------------

.. macro:: PSA_ALG_FFDH
    :definition: ((psa_algorithm_t)0x09010000)

    .. summary::
        The finite field Diffie-Hellman (DH) key-agreement algorithm.

    This standalone key-agreement algorithm can be used directly in a call to `psa_key_agreement()` or `psa_raw_key_agreement()`, or combined with a key-derivation operation using `PSA_ALG_KEY_AGREEMENT()` for use with `psa_key_derivation_key_agreement()`.

    When used as a key's permitted-algorithm policy, the following uses are permitted:

    *   In a call to `psa_key_agreement()` or `psa_raw_key_agreement()`, with algorithm `PSA_ALG_FFDH`.
    *   In a call to `psa_key_derivation_key_agreement()`, with any combined key-agreement and key-derivation algorithm constructed with `PSA_ALG_FFDH`.

    When used as part of a multi-part key-derivation operation, this implements a Diffie-Hellman key-agreement scheme using a single finite field Diffie-Hellman key pair for each participant. This includes the *dhEphem*, *dhOneFlow*, and *dhStatic* schemes. The input step `PSA_KEY_DERIVATION_INPUT_SECRET` is used when providing the secret and peer keys to the operation.

    The shared secret produced by this key-agreement algorithm is :math:`g^{ab}` in big-endian format. It is :math:`\lceil{(m / 8)}\rceil` bytes long where :math:`m` is the size of the prime :math:`p` in bits.

    This key-agreement scheme is defined by :cite-title:`SP800-56A` §5.7.1.1 under the name FFC DH.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DH_KEY_PAIR()`

.. macro:: PSA_ALG_ECDH
    :definition: ((psa_algorithm_t)0x09020000)

    .. summary::
        The elliptic curve Diffie-Hellman (ECDH) key-agreement algorithm.

    This standalone key-agreement algorithm can be used directly in a call to `psa_key_agreement()` or `psa_raw_key_agreement()`, or combined with a key-derivation operation using `PSA_ALG_KEY_AGREEMENT()` for use with `psa_key_derivation_key_agreement()`.

    When used as a key's permitted-algorithm policy, the following uses are permitted:

    *   In a call to `psa_key_agreement()` or `psa_raw_key_agreement()`, with algorithm `PSA_ALG_ECDH`.
    *   In a call to `psa_key_derivation_key_agreement()`, with any combined key-agreement and key-derivation algorithm constructed with `PSA_ALG_ECDH`.

    When used as part of a multi-part key-derivation operation, this implements a Diffie-Hellman key-agreement scheme using a single elliptic curve key pair for each participant. This includes the *Ephemeral unified model*, the *Static unified model*, and the *One-pass Diffie-Hellman* schemes. The input step `PSA_KEY_DERIVATION_INPUT_SECRET` is used when providing the secret and peer keys to the operation.

    The shared secret produced by key agreement is the x-coordinate of the shared secret point. It is always :math:`\lceil{(m / 8)}\rceil` bytes long where :math:`m` is the bit size associated with the curve, i.e. the bit size of the order of the curve's coordinate field. When :math:`m` is not a multiple of 8, the byte containing the most significant bit of the shared secret is padded with zero bits. The byte order is either little-endian or big-endian depending on the curve type.

    *   For Montgomery curves (curve family `PSA_ECC_FAMILY_MONTGOMERY`), the shared secret is the x-coordinate of :math:`Z = d_A Q_B = d_B Q_A` in little-endian byte order.

        -   For Curve25519, this is the X25519 function defined in :cite-title:`Curve25519`. The bit size :math:`m` is 255.
        -   For Curve448, this is the X448 function defined in :cite-title:`Curve448`. The bit size :math:`m` is 448.

    *   For Weierstrass curves (curve families ``PSA_ECC_FAMILY_SECP_XX``, ``PSA_ECC_FAMILY_SECT_XX``, `PSA_ECC_FAMILY_BRAINPOOL_P_R1` and `PSA_ECC_FAMILY_FRP`) the shared secret is the x-coordinate of :math:`Z = h d_A Q_B = h d_B Q_A` in big-endian byte order. This is the Elliptic Curve Cryptography Cofactor Diffie-Hellman primitive defined by :cite-title:`SEC1` §3.3.2 as, and also as ECC CDH by :cite-title:`SP800-56A` §5.7.1.2.

        -   Over prime fields (curve families ``PSA_ECC_FAMILY_SECP_XX``, `PSA_ECC_FAMILY_BRAINPOOL_P_R1` and `PSA_ECC_FAMILY_FRP`), the bit size is :math:`m = \lceil{\log_2(p)}\rceil` for the field :math:`\mathbb{F}_p`.
        -   Over binary fields (curve families ``PSA_ECC_FAMILY_SECT_XX``), the bit size is :math:`m` for the field :math:`\mathbb{F}_{2^m}`.

        .. note::

            The cofactor Diffie-Hellman primitive is equivalent to the standard elliptic curve Diffie-Hellman calculation :math:`Z = d_A Q_B = d_B Q_A` (`[SEC1]` §3.3.1) for curves where the cofactor :math:`h` is 1. This is true for all curves in the ``PSA_ECC_FAMILY_SECP_XX``, `PSA_ECC_FAMILY_BRAINPOOL_P_R1`, and `PSA_ECC_FAMILY_FRP` families.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(family)`

        where ``family`` is a Weierstrass or Montgomery Elliptic curve family. That is, one of the following values:

        *   ``PSA_ECC_FAMILY_SECT_XX``
        *   ``PSA_ECC_FAMILY_SECP_XX``
        *   `PSA_ECC_FAMILY_FRP`
        *   `PSA_ECC_FAMILY_BRAINPOOL_P_R1`
        *   `PSA_ECC_FAMILY_MONTGOMERY`

.. macro:: PSA_ALG_KEY_AGREEMENT
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a combined algorithm that chains a key agreement with a key derivation.

    .. param:: ka_alg
        A key-agreement algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_AGREEMENT(ka_alg)` is true.
    .. param:: kdf_alg
        A key-derivation algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_DERIVATION(kdf_alg)` is true.

    .. return::
        The corresponding key-agreement and key-derivation algorithm.

        Unspecified if ``ka_alg`` is not a supported key-agreement algorithm or ``kdf_alg`` is not a supported key-derivation algorithm.

    A combined key-agreement algorithm is used with a multi-part key-derivation operation, using a call to `psa_key_derivation_key_agreement()`.

    The component parts of a key-agreement algorithm can be extracted using `PSA_ALG_KEY_AGREEMENT_GET_BASE()` and `PSA_ALG_KEY_AGREEMENT_GET_KDF()`.

    .. subsection:: Compatible key types

        The resulting combined key-agreement algorithm is compatible with the same key types as the standalone key-agreement algorithm used to construct it.


Standalone key agreement
------------------------

.. function:: psa_key_agreement

    .. summary::
        Perform a key agreement and return the shared secret as a derivation key.

        .. versionadded:: 1.2

    .. param:: psa_key_id_t private_key
        Identifier of the private key to use.
        It must permit the usage `PSA_KEY_USAGE_DERIVE`.
    .. param:: const uint8_t * peer_key
        Public key of the peer. The peer key data is parsed with the type :code:`PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type)` where ``type`` is the type of ``private_key``, and with the same bit-size as ``private_key``. The peer key must be in the format that `psa_import_key()` accepts for this public-key type. These formats are described with the public-key type in :secref:`key-types`.
    .. param:: size_t peer_key_length
        Size of ``peer_key`` in bytes.
    .. param:: psa_algorithm_t alg
        The standalone key-agreement algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)` is true.
    .. param:: const psa_key_attributes_t * attributes
        The attributes for the new key.

        The following attributes are required for all keys:

        *   The key type, which must be one of `PSA_KEY_TYPE_DERIVE`, `PSA_KEY_TYPE_RAW_DATA`, `PSA_KEY_TYPE_HMAC`, or `PSA_KEY_TYPE_PASSWORD`.

            Implementations must support the `PSA_KEY_TYPE_DERIVE` and `PSA_KEY_TYPE_RAW_DATA` key types.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default `PSA_KEY_LIFETIME_VOLATILE` lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        The following attributes are optional:

        *   If the key size is nonzero, it must be equal to the output size of the key agreement, in bits.

            The output size, in bits, of the key agreement is :code:`8 * PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(type, bits)`, where ``type`` and ``bits`` are the type and bit-size of ``private_key``.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.

    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key. `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The new key contains the shared secret.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``private_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   ``private_key`` does not have the `PSA_KEY_USAGE_DERIVE` flag, or it does not permit the requested algorithm.
        *   The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a key-agreement algorithm.
        *   ``private_key`` is not compatible with ``alg``.
        *   ``peer_key`` is not a valid public key corresponding to ``private_key``.
        *   The output key attributes in ``attributes`` are not valid :

            -   The key type is not valid for key-agreement output.
            -   The key size is nonzero, and is not the size of the shared secret.
            -   The key lifetime is invalid.
            -   The key identifier is not valid for the key lifetime.
            -   The key usage flags include invalid values.
            -   The key's permitted-usage algorithm is invalid.
            -   The key attributes, as a whole, are invalid.

    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a key-agreement algorithm.
        *   ``private_key`` is not supported for use with ``alg``.
        *   The output key attributes, as a whole, are not supported, either by the implementation in general or in the specified storage location.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    A key-agreement algorithm takes two inputs: a private key ``private_key``, and a public key ``peer_key``.
    The result of this function is a shared secret, returned as a derivation key.

    The new key's location, policy, and type are taken from ``attributes``.

    The size of the returned key is always the bit-size of the shared secret, rounded up to a whole number of bytes.

    This key can be used as input to a key-derivation operation using `psa_key_derivation_input_key()`.

    .. warning::
        The shared secret resulting from a key-agreement algorithm such as finite field Diffie-Hellman or elliptic curve Diffie-Hellman has biases. This makes it unsuitable for use as key material, for example, as an AES key. Instead, it is recommended that a key-derivation algorithm is applied to the result, to derive unbiased cryptographic keys.

    If an application requires bounded execution during key agreement, it can use an interruptible key-agreement operation.
    See :secref:`interruptible-key-agreement`.

.. function:: psa_raw_key_agreement

    .. summary::
        Perform a key agreement and return the shared secret.

    .. param:: psa_algorithm_t alg
        The standalone key-agreement algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)` is true.
    .. param:: psa_key_id_t private_key
        Identifier of the private key to use.
        It must permit the usage `PSA_KEY_USAGE_DERIVE`.
    .. param:: const uint8_t * peer_key
        Public key of the peer. The peer key data is parsed with the type :code:`PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type)` where ``type`` is the type of ``private_key``, and with the same bit-size as ``private_key``. The peer key must be in the format that `psa_import_key()` accepts for this public-key type. These formats are described with the public-key type in :secref:`key-types`.
    .. param:: size_t peer_key_length
        Size of ``peer_key`` in bytes.
    .. param:: uint8_t * output
        Buffer where the shared secret is to be written.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes.
        This must be appropriate for the keys:

        *   The required output size is :code:`PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(type, bits)`, where ``type`` and ``bits`` are the type and bit-size of ``private_key``.
        *   `PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported standalone key-agreement algorithm.

    .. param:: size_t * output_length
        On success, the number of bytes that make up the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the shared secret.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``private_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        ``private_key`` does not have the `PSA_KEY_USAGE_DERIVE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a key-agreement algorithm.
        *   ``private_key`` is not compatible with ``alg``.
        *   ``peer_key`` is not a valid public key corresponding to ``private_key``.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small.
        `PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE()` or `PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a key-agreement algorithm.
        *   ``private_key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    A key-agreement algorithm takes two inputs: a private key ``private_key``, and a public key ``peer_key``. The result of this function is a shared secret, returned in the ``output`` buffer.

    .. warning::
        The result of a key-agreement algorithm such as finite field Diffie-Hellman or elliptic curve Diffie-Hellman has biases, and is not suitable for direct use as key material, for example, as an AES key. Instead it is recommended that the result is used as input to a key-derivation algorithm.

        To chain a key agreement with a key derivation, either use `psa_key_agreement()` to obtain the result of the key agreement as a derivation key, or use `psa_key_derivation_key_agreement()` and other functions from the key-derivation interface.

Combining key agreement and key derivation
------------------------------------------

.. function:: psa_key_derivation_key_agreement

    .. summary::
        Perform a key agreement and use the shared secret as input to a key derivation.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to use. It must have been set up with `psa_key_derivation_setup()` with a combined key-agreement and key-derivation algorithm ``alg``: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_AGREEMENT(alg)` is true and :code:`PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)` is false.

        The operation must be ready for an input of the type given by ``step``.
    .. param:: psa_key_derivation_step_t step
        Which step the input data is for.
    .. param:: psa_key_id_t private_key
        Identifier of the private key to use.
        It must permit the usage `PSA_KEY_USAGE_DERIVE`.
    .. param:: const uint8_t * peer_key
        Public key of the peer. The peer key data is parsed with the type :code:`PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type)` where ``type`` is the type of ``private_key``, and with the same bit-size as ``private_key``. The peer key must be in the format that `psa_import_key()` accepts for this public-key type. These formats are described with the public-key type in :secref:`key-types`.
    .. param:: size_t peer_key_length
        Size of ``peer_key`` in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid for this key-agreement ``step``.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``private_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        ``private_key`` does not have the `PSA_KEY_USAGE_DERIVE` flag, or it does not permit the operation's algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The operation's algorithm is not a key-agreement algorithm.
        *   ``step`` does not permit an input resulting from a key agreement.
        *   ``private_key`` is not compatible with the operation's algorithm.
        *   ``peer_key`` is not a valid public key corresponding to ``private_key``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``private_key`` is not supported for use with the operation's algorithm.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    A key-agreement algorithm takes two inputs: a private key ``private_key``, and a public key ``peer_key``. The result of this function is a shared secret, which is directly input to the key-derivation operation. Output from the key-derivation operation can then be used as keys and other cryptographic material.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

    .. note::

        This function cannot be used when the resulting shared secret is required for multiple key derivations.

        Instead, the application can call `psa_key_agreement()` to obtain the shared secret as a derivation key. This key can be used as input to as many key-derivation operations as required.

.. _interruptible-key-agreement:

Interruptible key agreement
---------------------------

Most key-agreement algorithms are computationally expensive.

An interruptible key-agreement operation can be used instead of calling `psa_key_agreement()`, in applications that have bounded execution requirements for use cases involving key agreement.

An interruptible key-agreement operation is used as follows:

1.  Allocate an interruptible key-agreement operation object, of type `psa_key_agreement_iop_t`, which will be passed to all the functions listed here.
#.  Initialize the operation object with one of the methods described in the documentation for `psa_key_agreement_iop_t`, for example, `PSA_KEY_AGREEMENT_IOP_INIT`.
#.  Call `psa_key_agreement_iop_setup()` to specify the algorithm, and provide the private key and the peer public key.
#.  Call `psa_key_agreement_iop_complete()` to finish the key agreement and output the shared secret, until this function does not return :code:`PSA_OPERATION_INCOMPLETE`.
#.  If an error occurs at any stage, or to terminate the operation early, call `psa_key_agreement_iop_abort()`.


.. typedef:: /* implementation-defined type */ psa_key_agreement_iop_t

    .. summary::
        The type of the state data structure for an interruptible key-agreement operation.

        .. versionadded:: 1.6

    Before calling any function on an interruptible key-agreement operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_key_agreement_iop_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_key_agreement_iop_t operation;

    *   Initialize the object to the initializer `PSA_KEY_AGREEMENT_IOP_INIT`, for example:

        .. code-block:: xref

            psa_key_agreement_iop_t operation = PSA_KEY_AGREEMENT_IOP_INIT;

    *   Assign the result of the function `psa_key_agreement_iop_init()` to the object, for example:

        .. code-block:: xref

            psa_key_agreement_iop_t operation;
            operation = psa_key_agreement_iop_init();

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_KEY_AGREEMENT_IOP_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro evaluates to an initializer for an interruptible key-agreement operation object of type `psa_key_agreement_iop_t`.

        .. versionadded:: 1.6

.. function:: psa_key_agreement_iop_init

    .. summary::
        Return an initial value for an interruptible key-agreement operation object.

        .. versionadded:: 1.6

    .. return:: psa_key_agreement_iop_t

.. function:: psa_key_agreement_iop_get_num_ops

    .. summary::
        Get the number of *ops* that an interruptible key-agreement operation has taken so far.

        .. versionadded:: 1.6

    .. param:: psa_key_agreement_iop_t * operation
        The interruptible key-agreement operation to inspect.

    .. return:: uint32_t
        Number of *ops* that the operation has taken so far.

    After the interruptible operation has completed, the returned value is the number of *ops* required for the entire operation.
    The value is reset to zero by a call to either `psa_key_agreement_iop_setup()` or `psa_key_agreement_iop_abort()`.

    This function can be used to tune the value passed to `psa_iop_set_max_ops()`.

    The value is undefined if the operation object has not been initialized.

.. function:: psa_key_agreement_iop_setup

    .. summary::
        Start an interruptible operation to perform a key agreement.

        .. versionadded:: 1.6

    .. param:: psa_key_agreement_iop_t * operation
        The interruptible key-agreement operation to set up.
        It must have been initialized as per the documentation for `psa_key_agreement_iop_t`, and be inactive.
    .. param:: psa_key_id_t private_key
        Identifier of the private key to use.
        It must permit the usage `PSA_KEY_USAGE_DERIVE`.
    .. param:: const uint8_t * peer_key
        Public key of the peer.
        The peer key data is parsed with the type :code:`PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type)` where ``type`` is the type of ``private_key``, and with the same bit-size as ``private_key``.
        The peer key must be in the format that `psa_import_key()` accepts for this public key type.
        These formats are described with the public key type in :secref:`key-types`.
    .. param:: size_t peer_key_length
        Size of ``peer_key`` in bytes.
    .. param:: psa_algorithm_t alg
        The standalone key-agreement algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)` is true.
    .. param:: const psa_key_attributes_t * attributes
        The attributes for the key to be output on completion.

        The following attributes are required for all keys:

        *   The key type, which must be one of `PSA_KEY_TYPE_DERIVE`, `PSA_KEY_TYPE_RAW_DATA`, `PSA_KEY_TYPE_HMAC`, or `PSA_KEY_TYPE_PASSWORD`.

            Implementations must support the `PSA_KEY_TYPE_DERIVE` and `PSA_KEY_TYPE_RAW_DATA` key types.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default volatile lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        The following attributes are optional:

        *   If the key size is nonzero, it must be equal to the output size of the key agreement, in bits.

            The output size, in bits, of the key agreement is :code:`8 * PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(type, bits)`, where ``type`` and ``bits`` are the type and bit-size of ``private_key``.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The interruptible operation must now be completed by calling `psa_key_agreement_iop_complete()`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``private_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   ``private_key`` does not have the `PSA_KEY_USAGE_DERIVE` flag, or it does not permit the requested algorithm.
        *   The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not a key-agreement algorithm.
        *   ``private_key`` is not compatible with ``alg``.
        *   ``peer_key`` is not a valid public key corresponding to ``private_key``.
        *   The output key attributes in ``attributes`` are not valid :

            -   The key type is not valid for key-agreement output.
            -   The key size is nonzero, and is not the size of the shared secret.
            -   The key lifetime is invalid.
            -   The key identifier is not valid for the key lifetime.
            -   The key usage flags include invalid values.
            -   The key's permitted-usage algorithm is invalid.
            -   The key attributes, as a whole, are invalid.

    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not a key-agreement algorithm.
        *   ``private_key`` is not supported for use with ``alg``.
        *   The output key attributes, as a whole, are not supported, either by the implementation in general or in the specified storage location.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE

    This function sets up an interruptible operation to perform a key-agreement.
    A key-agreement algorithm takes two inputs: a private key ``private_key``, and a public key ``peer_key``.

    When the interruptible operation completes, the shared secret is output in a key. The key's location, policy, and type are taken from ``attributes``. The size of the key is always the bit-size of the shared secret, rounded up to a whole number of bytes.

    After a successful call to `psa_key_agreement_iop_setup()`, the operation is active.
    The operation can be completed by calling `psa_key_agreement_iop_complete()` repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.
    Once active, the application must eventually terminate the operation.
    The following events terminate an operation:

    *   A successful call to `psa_key_agreement_iop_complete()`.
    *   A call to `psa_key_agreement_iop_abort()`.

    If `psa_key_agreement_iop_setup()` returns an error, the operation object is unchanged.

.. function:: psa_key_agreement_iop_complete

    .. summary::
        Attempt to finish a key agreement and return the shared secret.

        .. versionadded:: 1.6

    .. param:: psa_key_agreement_iop_t * operation
        The interruptible key-agreement operation to use.
        The operation must be active.
    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key.
        `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The new key contains the shared secret.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_OPERATION_INCOMPLETE
        The function was interrupted after exhausting the maximum *ops*.
        The computation is incomplete, and this function must be called again with the same operation object to continue.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    .. note::
        This is an interruptible function, and must be called repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    When this function returns successfully, the shared secret is returned as a derivation key in ``key``, and the operation becomes inactive.
    The attributes of the new key are specified in the call to `psa_key_agreement_iop_setup()` used to set up this operation.
    This key can be input to a key derivation operation using `psa_key_derivation_input_key()`.

    .. warning::
        The shared secret resulting from a key-agreement algorithm such as finite-field Diffie-Hellman or elliptic curve Diffie-Hellman has biases. This makes it unsuitable for use as key material, for example, as an AES key. Instead, it is recommended that a key derivation algorithm is applied to the result, to derive unbiased cryptographic keys.

    If this function returns :code:`PSA_OPERATION_INCOMPLETE`, no key is returned, and this function must be called again to continue the operation.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_key_agreement_iop_abort()`.

    The amount of calculation performed in a single call to this function is determined by the maximum *ops* setting. See `psa_iop_set_max_ops()`.

.. function:: psa_key_agreement_iop_abort

    .. summary::
        Abort an interruptible key-agreement operation.

        .. versionadded:: 1.6

    .. param:: psa_key_agreement_iop_t * operation
        The interruptible key-agreement operation to abort.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` structure itself. Once aborted, the operation object can be reused for another operation by calling `psa_key_agreement_iop_setup()` again.

    This function can be called at any time after the operation object has been initialized as described in `psa_key_agreement_iop_t`.

    In particular, it is valid to call `psa_key_agreement_iop_abort()` twice, or to call `psa_key_agreement_iop_abort()` on an operation that has not been set up.

Support macros
--------------

.. macro:: PSA_ALG_KEY_AGREEMENT_GET_BASE
    :definition: /* specification-defined value */

    .. summary::
        Get the standalone key-agreement algorithm from a combined key-agreement and key-derivation algorithm.

    .. param:: alg
        A key-agreement algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_AGREEMENT(alg)` is true.

    .. return::
        The underlying standalone key-agreement algorithm if ``alg`` is a key-agreement algorithm.

        Unspecified if ``alg`` is not a key-agreement algorithm or if it is not supported by the implementation.

    See also `PSA_ALG_KEY_AGREEMENT()` and `PSA_ALG_KEY_AGREEMENT_GET_KDF()`.

.. macro:: PSA_ALG_KEY_AGREEMENT_GET_KDF
    :definition: /* specification-defined value */

    .. summary::
        Get the key-derivation algorithm used in a combined key-agreement and key-derivation algorithm.

    .. param:: alg
        A key-agreement algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_AGREEMENT(alg)` is true.

    .. return::
        The underlying key-derivation algorithm if ``alg`` is a key-agreement algorithm.

        Unspecified if ``alg`` is not a key-agreement algorithm or if it is not supported by the implementation.

    See also `PSA_ALG_KEY_AGREEMENT()` and `PSA_ALG_KEY_AGREEMENT_GET_BASE()`.

.. macro:: PSA_ALG_IS_STANDALONE_KEY_AGREEMENT
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a standalone key-agreement algorithm.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a standalone key-agreement algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    A standalone key-agreement algorithm is one that does not specify a key-derivation function. Usually, standalone key-agreement algorithms are constructed directly with a ``PSA_ALG_xxx`` macro while combined key-agreement algorithms are constructed with `PSA_ALG_KEY_AGREEMENT()`.

    The standalone key-agreement algorithm can be extracted from a combined key-agreement algorithm identifier using `PSA_ALG_KEY_AGREEMENT_GET_BASE()`.

.. macro:: PSA_ALG_IS_RAW_KEY_AGREEMENT
    :definition: PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)

    .. summary::
        Whether the specified algorithm is a standalone key-agreement algorithm.

        .. deprecated:: 1.2 Use `PSA_ALG_IS_STANDALONE_KEY_AGREEMENT()` instead.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

.. macro:: PSA_ALG_IS_FFDH
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a finite field Diffie-Hellman algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a finite field Diffie-Hellman algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-agreement algorithm identifier.

    This includes the standalone finite field Diffie-Hellman algorithm, as well as finite field Diffie-Hellman combined with any supported key-derivation algorithm.

.. macro:: PSA_ALG_IS_ECDH
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an elliptic curve Diffie-Hellman algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an elliptic curve Diffie-Hellman algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-agreement algorithm identifier.

    This includes the standalone elliptic curve Diffie-Hellman algorithm, as well as elliptic curve Diffie-Hellman combined with any supported key-derivation algorithm.

.. macro:: PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_raw_key_agreement()`.

    .. param:: key_type
        A supported key type.
    .. param:: key_bits
        The size of the key in bits.

    .. return::
        A sufficient output buffer size for the specified key type and size. An implementation can return either ``0`` or a correct size for a key type and size that it recognizes, but does not support. If the parameters are not valid, the return value is unspecified.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_raw_key_agreement()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE`.

.. macro:: PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_raw_key_agreement()`, for any of the supported key types and key-agreement algorithms.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_raw_key_agreement()` will not fail due to an insufficient buffer size.

    See also `PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE()`.
