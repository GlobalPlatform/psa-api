.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 260

.. _sign:

Asymmetric signature
====================

An asymmetric signature algorithm provides two functions:

*   **Sign**: Calculate a message signature using a private, or secret, key.
*   **Verify**: Check that a signature matches a message using a public key.

Successful verification indicates that the message signature was calculated using the private key that is associated with the public key.

In the |API|, an asymmetric-sign function requires an asymmetric key pair; and an asymmetric-verify function requires an asymmetric public key or key pair.

.. rubric:: Signature schemes

The |API| supports the following signature schemes:

*   :secref:`rsa-sign-algorithms`
*   :secref:`ecdsa-sign-algorithms`
*   :secref:`eddsa-sign-algorithms`
*   :secref:`slh-dsa-algorithms`
*   :secref:`ml-dsa-algorithms`
*   :secref:`lms-algorithms`
*   :secref:`xmss-algorithms`

.. rubric:: Types of signature algorithm

There are three categories of asymmetric signature algorithm in the |API|:

*   Hash-and-sign algorithms, that have two distinct phases:

    -   Calculate a hash of the message
    -   Calculate a signature over the hash

    For these algorithms, the asymmetric signature API allows applications to either calculate the full message signature, or calculate the signature of a pre-computed hash. For example, this enables the application to use a multi-part hash operation to calculate the hash of a large message, prior to calculating or verifying a signature on the calculated hash.

    The following algorithms are in this category:

    | `PSA_ALG_RSA_PKCS1V15_SIGN`
    | `PSA_ALG_RSA_PSS`
    | `PSA_ALG_RSA_PSS_ANY_SALT`
    | `PSA_ALG_ECDSA`
    | `PSA_ALG_DETERMINISTIC_ECDSA`
    | `PSA_ALG_ED25519PH`
    | `PSA_ALG_ED448PH`
    | `PSA_ALG_HASH_SLH_DSA`
    | `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA`
    | `PSA_ALG_HASH_ML_DSA`
    | `PSA_ALG_DETERMINISTIC_HASH_ML_DSA`

*   Message signature algorithms that do not separate the message processing from the signature calculations. This approach can provide better security against certain types of attack.

    For these algorithms, it is not possible to inject a pre-computed hash into the middle of the algorithm. An application can choose to calculate a message hash, and sign that instead of the message --- but this is not functionally equivalent to signing the message, and eliminates the security benefits of signing the message directly.

    Some of these algorithms still permit the signature of a large message to be calculated, or verified, by providing the message data in fragments. This is possible when the algorithm only processes the message data once. See :secref:`multi-part-signature` and the individual algorithm descriptions for details.

    The following algorithms are in this category:

    | `PSA_ALG_PURE_EDDSA`
    | `PSA_ALG_EDDSA_CTX`
    | `PSA_ALG_SLH_DSA`
    | `PSA_ALG_DETERMINISTIC_SLH_DSA`
    | `PSA_ALG_ML_DSA`
    | `PSA_ALG_DETERMINISTIC_ML_DSA`
    | `PSA_ALG_LMS`
    | `PSA_ALG_HSS`
    | `PSA_ALG_XMSS`
    | `PSA_ALG_XMSS_MT`

*   Specialized signature algorithms, that use part of a standard signature algorithm within a specific protocol. It is recommended that these algorithms are only used for that purpose, with inputs as specified by the higher-level protocol. See the individual algorithm descriptions for details on their usage.

    The following algorithms are in this category:

    | `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`
    | `PSA_ALG_ECDSA_ANY`

.. rubric:: Signature functions

The |API| provides several functions for calculating and verifying signatures:

*   The single-part signature and verification functions, `psa_sign_message()` and `psa_verify_message()`, take a message as one of their inputs, and perform the sign or verify algorithm.

    These functions can be used on any hash-and-sign, or message signature, algorithms. See also `PSA_ALG_IS_SIGN_MESSAGE()`.

*   The single-part functions, `psa_sign_hash()` and `psa_verify_hash()`, typically take a message hash as one of their inputs, and perform the sign or verify algorithm.

    These functions can be used on any hash-and-sign signature algorithm. It is recommended that the input to these functions is a hash, computed using the corresponding hash algorithm. To determine which hash algorithm to use, the macro `PSA_ALG_GET_HASH()` can be called on the signature algorithm identifier.

    These functions can also be used on the specialized signature algorithms, with a hash or encoded-hash as input. See also `PSA_ALG_IS_SIGN_HASH()`.

*   Many modern signature algorithms have been designed to also accept a context parameter to provide domain separation.
    Version 1.4 of the |API| introduced four new functions that accept contexts: `psa_sign_message_with_context()`, `psa_sign_hash_with_context()`, `psa_verify_message_with_context()`, and `psa_verify_hash_with_context()`.

    If called with a zero-length context, these functions produce the same signature as the associated function without a context parameter.

    .. note::

        If a signature scheme treats the absence of a context parameter differently to a zero-length context, the |API| defines distinct algorithm identifiers for the two variants.
        For example, when using a 255-bit key with EdDSA, `PSA_ALG_PURE_EDDSA` implements Ed25519 (without a context) and `PSA_ALG_EDDSA_CTX` implements Ed25519ctx (with a context, which can be zero-length).
        See :secref:`eddsa-sign-algorithms`.

    It is an error to provide a non-zero-length context with an algorithm that does not accept contexts.

    Code written to be cryptographically agile can use the new functions, provided it guards against providing a non-zero-length context with an algorithm that does not support them.

    The `PSA_ALG_SIGN_SUPPORTS_CONTEXT()` macro can be used to determine if the implementation of an algorithm supports the use of non-zero-length contexts.

See :secref:`single-part-signature`.

.. rubric:: Signature multi-part operations

For large or streamed messages, it might be necessary to compute or verify a signature without having the entire message available at once.

*   For hash-and-sign signature algorithms, this has been possible since version 1.0 of the |API| by using a multi-part hash operation followed by a call to `psa_sign_hash()` or `psa_verify_hash()`.
*   Version 1.5 of the |API| introduces multi-part signature operations, `psa_sign_operation_t` and `psa_verify_operation_t`, that enable many more signature algorithms to be used with a fragmented message, including some of the message signature algorithms.

    .. note::
        Message signature algorithms that require the message data to be processed more than once cannot be used in a multi-part operation, and can only be used with the single-part functions.
        For example, the PureEdDSA and SLH-DSA algorithms cannot be used with a multi-part sign operation, but can be used with a multi-part verify operation.

        See the individual algorithm descriptions for details.

See :secref:`multi-part-signature`.

.. rubric:: Signature interruptible operations

Some applications need to bound the expensive computation performed in an individual signature API call.

*   Version 1.6 of the |API| introduces optional interruptible signature and verification operations, `psa_sign_iop_t` and `psa_verify_iop_t`. They limit the computation performed in a call and can return :code:`PSA_OPERATION_INCOMPLETE` when further calls are required.

    These operations are distinct from the multi-part operations and are intended for applications that require bounded execution time. The interruptible operations use a zero-length context when the algorithm has a context parameter.

See :secref:`interruptible-sign` and :secref:`interruptible-verify`.

.. _rsa-sign-algorithms:

RSA signature algorithms
------------------------

.. macro:: PSA_ALG_RSA_PKCS1V15_SIGN
    :definition: /* specification-defined value */

    .. summary::
        The RSA PKCS#1 v1.5 message signature scheme, with hashing.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding RSA PKCS#1 v1.5 signature algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    RSA PKCS#1 v1.5 does not have a context parameter.
    However, the sign or verify with context functions can be used with a zero-length context.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This signature scheme is defined by :RFC-title:`8017#8.2` under the name RSASSA-PKCS1-v1_5.

    When used with `psa_sign_hash()` or `psa_verify_hash()`, the provided ``hash`` parameter is used as :math:`H` from step 2 onwards in the message encoding algorithm ``EMSA-PKCS1-V1_5-ENCODE()`` in :RFC:`8017#9.2`. :math:`H` is the message digest, computed using the ``hash_alg`` hash algorithm.

    :numref:`tab-rsa-pkcs1v15-oid` lists the OID to use when formatting the hash.
    Note that the DER-encoded OID in the table is just the data value, without the OID tag and length.

    .. csv-table:: OID to use for RSA PKCS#1 v1.5
        :name: tab-rsa-pkcs1v15-oid
        :header-rows: 1
        :class: longtable
        :widths: 6 7 6 9

        Hash algorithm, OID (dot notation), OID (ASN.1 hex), Reference
        `PSA_ALG_MD2`, 1.2.840.113549.2.2, ``2a864886f70d0202``, :RFC-title:`8017#B.1`
        `PSA_ALG_MD4`, 1.2.840.113549.2.4, ``2a864886f70d0204``, :RFC-title:`1320#1`
        `PSA_ALG_MD5`, 1.2.840.113549.2.5, ``2a864886f70d0205``, :RFC:`8017#B.1`
        `PSA_ALG_RIPEMD160`, 1.3.36.3.2.1, ``2b24030201``, :cite-title:`MailTrusT` §4.1.4
        `PSA_ALG_SHA_1`, 1.3.14.3.2.26, ``2b0e03021a``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_224`, 2.16.840.1.101.3.4.2.4, ``608648016503040204``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_256`, 2.16.840.1.101.3.4.2.1, ``608648016503040201``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_384`, 2.16.840.1.101.3.4.2.2, ``608648016503040202``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_512`, 2.16.840.1.101.3.4.2.3, ``608648016503040203``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_512_224`, 2.16.840.1.101.3.4.2.5, ``608648016503040205``, :RFC:`8017#B.1`
        `PSA_ALG_SHA_512_256`, 2.16.840.1.101.3.4.2.6, ``608648016503040206``, :RFC:`8017#B.1`
        `PSA_ALG_SHA3_224`, 2.16.840.1.101.3.4.2.7, ``608648016503040207``, :RFC-title:`9688#2`
        `PSA_ALG_SHA3_256`, 2.16.840.1.101.3.4.2.8, ``608648016503040208``, :RFC:`9688#2`
        `PSA_ALG_SHA3_384`, 2.16.840.1.101.3.4.2.9, ``608648016503040209``, :RFC:`9688#2`
        `PSA_ALG_SHA3_512`, 2.16.840.1.101.3.4.2.10, ``60864801650304020a``, :RFC:`9688#2`
        `PSA_ALG_SM3`, 1.2.156.10197.1.504, ``2a811ccf55018378``, :cite-title:`SM3-draft` §8.1.3

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_RSA_PKCS1V15_SIGN` with hash algorithms that lack a standard OID, namely:

        *   `PSA_ALG_AES_MMO_ZIGBEE`
        *   `PSA_ALG_ASCON_HASH256`
        *   `PSA_ALG_SHAKE256_512`

        It is recommended that these hash algorithms are not supported with `PSA_ALG_RSA_PKCS1V15_SIGN`.
        Future versions of the |API| might specify what OID to use.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_RSA_PKCS1V15_SIGN_RAW
    :definition: ((psa_algorithm_t) 0x06000200)

    .. summary::
        The raw RSA PKCS#1 v1.5 signature algorithm, without hashing.

    This specialized signature algorithm can only be used with the `psa_sign_hash()` and `psa_verify_hash()` functions, their context variants with a zero-length context, or the interruptible signature and verification operations.
    RSA PKCS#1 v1.5 does not have a context parameter.

    This signature scheme is defined by :RFC-title:`8017#8.2` under the name RSASSA-PKCS1-v1_5.

    The ``hash`` parameter to `psa_sign_hash()` or `psa_verify_hash()` is used as :math:`T` from step 3 onwards in the message encoding algorithm ``EMSA-PKCS1-V1_5-ENCODE()`` in :RFC:`8017#9.2`. :math:`T` is normally the DER encoding of the *DigestInfo* structure produced by step 2 in the message encoding algorithm, but it can be any byte string within the available length.

    The wildcard key policy :code:`PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH)` also permits a key to be used with the `PSA_ALG_RSA_PKCS1V15_SIGN_RAW` signature algorithm.

    .. rationale::

        RSA keys that are used for TLS session establishment can be used in different versions of the TLS protocol.

        *   Versions 1.0 and 1.1 of the TLS protocol uses the `PSA_ALG_RSA_PKCS1V15_SIGN_RAW` algorithm, which signs an encoded SHA-1 + MD5 hash.
        *   Version 1.2 of the TLS protocol uses the :code:`PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256))` algorithm, which signs the [unencoded] SHA-256 hash.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_RSA_PSS
    :definition: /* specification-defined value */

    .. summary::
        The RSA PSS message signature scheme, with hashing.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding RSA PSS signature algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    RSA PSS does not have a context parameter.
    However, the sign or verify with context functions can be used with a zero-length context.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This algorithm is randomized: each invocation returns a different, equally valid signature.

    This is the signature scheme defined by :RFC:`8017#8.1` under the name RSASSA-PSS, with the following options:

    *   The mask generation function is *MGF1* defined by :RFC:`8017#B`.
    *   When creating a signature, the salt length is equal to the length of the hash, or the largest possible salt length for the algorithm and key size if that is smaller than the hash length.
    *   When verifying a signature, the salt length must be equal to the length of the hash, or the largest possible salt length for the algorithm and key size if that is smaller than the hash length.
    *   The specified hash algorithm, ``hash_alg``,  is used to hash the input message, to create the salted hash, and for the mask generation.

    When used with `psa_sign_hash()` or `psa_verify_hash()`, the provided ``hash`` parameter is the message digest, computed using the ``hash_alg`` hash algorithm.

    .. note::

        The `PSA_ALG_RSA_PSS_ANY_SALT()` algorithm is equivalent to `PSA_ALG_RSA_PSS()` when creating a signature, but permits any salt length when verifying a signature.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_RSA_PSS_ANY_SALT
    :definition: /* specification-defined value */

    .. summary::
        The RSA PSS message signature scheme, with hashing.
        This variant permits any salt length for signature verification.

        .. versionadded:: 1.1

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding RSA PSS signature algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    RSA PSS does not have a context parameter.
    However, the sign or verify with context functions can be used with a zero-length context.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This algorithm is randomized: each invocation returns a different, equally valid signature.

    This is the signature scheme defined by :RFC:`8017#8.1` under the name RSASSA-PSS, with the following options:

    *   The mask generation function is *MGF1* defined by :RFC:`8017#B`.
    *   When creating a signature, the salt length is equal to the length of the hash, or the largest possible salt length for the algorithm and key size if that is smaller than the hash length.
    *   When verifying a signature, any salt length permitted by the RSASSA-PSS signature algorithm is accepted.
    *   The specified hash algorithm, ``hash_alg``,  is used to hash the input message, to create the salted hash, and for the mask generation.

    When used with `psa_sign_hash()` or `psa_verify_hash()`, the provided ``hash`` parameter is the message digest, computed using the ``hash_alg`` hash algorithm.

    .. note::

        The `PSA_ALG_RSA_PSS()` algorithm is equivalent to `PSA_ALG_RSA_PSS_ANY_SALT()` when creating a signature, but is strict about the permitted salt length when verifying a signature.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_RSA_KEY_PAIR`
        | `PSA_KEY_TYPE_RSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_IS_RSA_PKCS1V15_SIGN
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an RSA PKCS#1 v1.5 signature algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an RSA PKCS#1 v1.5 signature algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_ALG_IS_RSA_PSS
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an RSA PSS signature algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an RSA PSS signature algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    This macro returns ``1`` for algorithms constructed using either `PSA_ALG_RSA_PSS()` or `PSA_ALG_RSA_PSS_ANY_SALT()`.

.. macro:: PSA_ALG_IS_RSA_PSS_ANY_SALT
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an RSA PSS signature algorithm that permits any salt length.

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an RSA PSS signature algorithm that permits any salt length, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    An RSA PSS signature algorithm that permits any salt length is constructed using `PSA_ALG_RSA_PSS_ANY_SALT()`.

    See also `PSA_ALG_IS_RSA_PSS()` and `PSA_ALG_IS_RSA_PSS_STANDARD_SALT()`.

.. macro:: PSA_ALG_IS_RSA_PSS_STANDARD_SALT
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an RSA PSS signature algorithm that requires the standard salt length.

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an RSA PSS signature algorithm that requires the standard salt length, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    An RSA PSS signature algorithm that requires the standard salt length is constructed using `PSA_ALG_RSA_PSS()`.

    See also `PSA_ALG_IS_RSA_PSS()` and `PSA_ALG_IS_RSA_PSS_ANY_SALT()`.

.. _ecdsa-sign-algorithms:

ECDSA signature algorithms
--------------------------

.. macro:: PSA_ALG_ECDSA
    :definition: /* specification-defined value */

    .. summary::
        The randomized ECDSA signature scheme, with hashing.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding randomized ECDSA signature algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    ECDSA does not have a context parameter.
    However, the sign or verify with context functions can be used with a zero-length context.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    When used with `psa_sign_hash()` or `psa_verify_hash()`, the provided ``hash`` parameter is the message digest, computed using the ``hash_alg`` hash algorithm.

    This algorithm is randomized: each invocation returns a different, equally valid signature.

    The ECDSA signature scheme is defined by :cite-title:`SEC1`, and also by :cite-title:`X9-62`, with a random per-message secret number :math:`k`.

    The representation of the signature as a byte string consists of the concatenation of the signature values :math:`r` and :math:`s`.
    Each of :math:`r` and :math:`s` is encoded as a big-endian :math:`m`-octet string, where :math:`m` is the integer for which :math:`2^{8(m-1)} \leq q < 2^{8m}`, and :math:`q` is the order of the elliptic curve.

    When based on the same hash algorithm, the verification operations for `PSA_ALG_ECDSA` and `PSA_ALG_DETERMINISTIC_ECDSA` are identical. A signature created using `PSA_ALG_ECDSA` can be verified with the same key using either `PSA_ALG_ECDSA` or `PSA_ALG_DETERMINISTIC_ECDSA`. Similarly, a signature created using `PSA_ALG_DETERMINISTIC_ECDSA` can be verified with the same key using either `PSA_ALG_ECDSA` or `PSA_ALG_DETERMINISTIC_ECDSA`.

    .. note::

        A verifier cannot determine whether a signature was produced with deterministic ECDSA or with randomized ECDSA: it is only possible to verify that a signature was made with ECDSA with the private key corresponding to the public key used for the verification.

    When :code:`PSA_ALG_ECDSA(hash_alg)` is used as a permitted algorithm in a key policy, this permits:

    *   :code:`PSA_ALG_ECDSA(hash_alg)` as the algorithm in a call to any signing function.
    *   :code:`PSA_ALG_ECDSA(hash_alg)` or :code:`PSA_ALG_DETERMINISTIC_ECDSA(hash_alg)` as the algorithm in a call to any signature verification function.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(family)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(family)` (signature verification only)

        where ``family`` is a Weierstrass Elliptic curve family. That is, one of the following values:

        *   ``PSA_ECC_FAMILY_SECT_XX``
        *   ``PSA_ECC_FAMILY_SECP_XX``
        *   `PSA_ECC_FAMILY_FRP`
        *   `PSA_ECC_FAMILY_BRAINPOOL_P_R1`

.. macro:: PSA_ALG_ECDSA_ANY
    :definition: ((psa_algorithm_t) 0x06000600)

    .. summary::
        The randomized ECDSA signature scheme, without hashing.

    This specialized signature algorithm can only be used with the `psa_sign_hash()` and `psa_verify_hash()` functions, their context variants with a zero-length context, or the interruptible signature and verification operations.
    ECDSA does not have a context parameter.

    This algorithm is randomized: each invocation returns a different, equally valid signature.

    This is the same signature scheme as `PSA_ALG_ECDSA`, but without specifying a hash algorithm, and skipping the message hashing operation.

    .. warning::

        This algorithm is only recommended to sign or verify a sequence of bytes that are a pre-computed hash. Note that the input is padded with zeros on the left or truncated on the right as required to fit the curve size.

    This algorithm cannot be used with the wildcard key policy :code:`PSA_ALG_ECDSA(PSA_ALG_ANY_HASH)`. It is only permitted when `PSA_ALG_ECDSA_ANY` is the key's permitted-algorithm policy.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(family)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(family)` (signature verification only)

        where ``family`` is a Weierstrass Elliptic curve family. That is, one of the following values:

        *   ``PSA_ECC_FAMILY_SECT_XX``
        *   ``PSA_ECC_FAMILY_SECP_XX``
        *   `PSA_ECC_FAMILY_FRP`
        *   `PSA_ECC_FAMILY_BRAINPOOL_P_R1`

.. macro:: PSA_ALG_DETERMINISTIC_ECDSA
    :definition: /* specification-defined value */

    .. summary::
        Deterministic ECDSA signature scheme, with hashing.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true. This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding deterministic ECDSA signature algorithm.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    ECDSA does not have a context parameter.
    However, the sign or verify with context functions can be used with a zero-length context.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    When used with `psa_sign_hash()` or `psa_verify_hash()`, the provided ``hash`` parameter is the message digest, computed using the ``hash_alg`` hash algorithm.

    This is the deterministic ECDSA signature scheme defined by :RFC-title:`6979`.

    The representation of a signature is the same as with `PSA_ALG_ECDSA`.

    When based on the same hash algorithm, the verification operations for `PSA_ALG_ECDSA` and `PSA_ALG_DETERMINISTIC_ECDSA` are identical. A signature created using `PSA_ALG_ECDSA` can be verified with the same key using either `PSA_ALG_ECDSA` or `PSA_ALG_DETERMINISTIC_ECDSA`. Similarly, a signature created using `PSA_ALG_DETERMINISTIC_ECDSA` can be verified with the same key using either `PSA_ALG_ECDSA` or `PSA_ALG_DETERMINISTIC_ECDSA`.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_DETERMINISTIC_ECDSA` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_DETERMINISTIC_ECDSA`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. note::

        A verifier cannot determine whether a signature was produced with deterministic ECDSA or with randomized ECDSA: it is only possible to verify that a signature was made with ECDSA with the private key corresponding to the public key used for the verification.

    When :code:`PSA_ALG_DETERMINISTIC_ECDSA(hash_alg)` is used as a permitted algorithm in a key policy, this permits:

    *   :code:`PSA_ALG_DETERMINISTIC_ECDSA(hash_alg)` as the algorithm in a call to any signing function.
    *   :code:`PSA_ALG_DETERMINISTIC_ECDSA(hash_alg)` or :code:`PSA_ALG_ECDSA(hash_alg)` as the algorithm in a call to any signature verification function.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(family)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(family)` (signature verification only)

        where ``family`` is a Weierstrass Elliptic curve family. That is, one of the following values:

        *   ``PSA_ECC_FAMILY_SECT_XX``
        *   ``PSA_ECC_FAMILY_SECP_XX``
        *   `PSA_ECC_FAMILY_FRP`
        *   `PSA_ECC_FAMILY_BRAINPOOL_P_R1`

.. macro:: PSA_ALG_IS_ECDSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is ECDSA.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an ECDSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_ALG_IS_DETERMINISTIC_ECDSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is deterministic ECDSA.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a deterministic ECDSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_ECDSA()` and `PSA_ALG_IS_RANDOMIZED_ECDSA()`.

.. macro:: PSA_ALG_IS_RANDOMIZED_ECDSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is randomized ECDSA.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a randomized ECDSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_ECDSA()` and `PSA_ALG_IS_DETERMINISTIC_ECDSA()`.

.. _eddsa-sign-algorithms:

EdDSA signature algorithms
--------------------------

The PureEdDSA and HashEdDSA digital signature algorithms are defined by :RFC-title:`8032`.
They are used with the Edwards25519 and Edwards448 elliptic curve keys, see `PSA_ECC_FAMILY_TWISTED_EDWARDS`.

*   PureEdDSA is a set of message-signing algorithms, that cannot be split into a hash step, followed by a signature or verification step.

*   HashEdDSA is a pair of hash-and-sign algorithms, with a specified hash algorithm associated with each key size.

Both PureEdDSA and HashEdDSA can be used with contexts, which enables domain-separation when signatures are made of different message structures with the same key.
For EdDSA, the context is an arbitrary byte string between zero and 255 bytes in length.
Interruptible signature operations use a zero-length context.

The development of EdDSA resulted in a total of five distinct algorithms:

*   Ed25519: the original PureEdDSA algorithm for the Edwards25519 curve, which does not accept a context.
*   Ed25519ctx: a second PureEdDSA algorithm for the Edwards25519 curve, with a context parameter.
*   Ed448: the PureEdDSA algorithm for the Edwards448 curve, with a context parameter.
*   Ed25519ph: the HashEdDSA algorithm for the Edwards25519 curve, with a context parameter.
*   Ed448ph: the HashEdDSA algorithm for the Edwards448 curve, with a context parameter.

:numref:`table-eddsa-algs` shows the algorithm identifiers in the |API|, and how they are used to select the appropriate EdDSA algorithm.

.. csv-table:: EdDSA algorithm identifiers
    :name: table-eddsa-algs
    :header-rows: 1
    :widths: auto

    Algorithm identifier, With 255-bit key, With 448-bit key, Sign/verify hash, Support non-zero-length context
    `PSA_ALG_PURE_EDDSA`, Ed25519, Ed448, No, No
    `PSA_ALG_ED25519PH`, Ed25519ph, *Invalid*, Yes, Yes
    `PSA_ALG_ED448PH`, *Invalid*, Ed448ph, Yes, Yes
    `PSA_ALG_EDDSA_CTX`, Ed25519ctx, Ed448, No, Yes

.. note::

    Ed25519ctx produces a distinct signature to Ed25519, even with a zero-length context.

.. macro:: PSA_ALG_PURE_EDDSA
    :definition: ((psa_algorithm_t) 0x06000800)

    .. summary::
        Edwards-curve digital signature algorithm without pre-hashing (PureEdDSA), with zero-length context.

        .. versionadded:: 1.1

    This message-signature algorithm can be used with the `psa_sign_message()` and `psa_verify_message()` functions.
    With a zero-length context, `PSA_ALG_PURE_EDDSA` can also be used with the `psa_sign_message_with_context()` and `psa_verify_message_with_context()` functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part verify operation, but not with the multi-part sign operation. It can also be used with the interruptible signature and verification operations.

    This is the PureEdDSA digital signature algorithm defined by :RFC-title:`8032`, with zero-length context.

    PureEdDSA requires an elliptic curve key on a twisted Edwards curve (see `PSA_ECC_FAMILY_TWISTED_EDWARDS`).
    The following curves are supported:

    *   Edwards25519: the Ed25519 algorithm is computed.
        The output signature is a 64-byte string: the concatenation of :math:`R` and :math:`S` as defined by :RFC:`8032#5.1.6`.

    *   Edwards448: the Ed448 algorithm is computed, with a zero-length context.
        The output signature is a 114-byte string: the concatenation of :math:`R` and :math:`S` as defined by :RFC:`8032#5.2.6`.

    .. note::
        When using an interruptible asymmetric signature operation with this algorithm, it is not possible to fragment the message data when calculating the signature. The message must be passed in a single call to `psa_sign_iop_update()`.

        However, it is possible to fragment the message data when verifying a signature using an interruptible asymmetric verification operation.

    .. note::
        To sign or verify the pre-computed hash of a message using EdDSA, the HashEdDSA algorithms (`PSA_ALG_ED25519PH` and `PSA_ALG_ED448PH`) can be used.
        The signature produced by HashEdDSA is distinct from that produced by PureEdDSA.

    .. note::
        To sign or verify a message with a non-zero-length context using PureEdDSA, use the `PSA_ALG_EDDSA_CTX` algorithm.

        With an Edwards25519 curve key, `PSA_ALG_EDDSA_CTX` with a zero-length context creates different signatures to `PSA_ALG_PURE_EDDSA`.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS)` (signature verification only)

.. macro:: PSA_ALG_EDDSA_CTX
    :definition: ((psa_algorithm_t) 0x06000A00)

    .. summary::
        Edwards-curve digital signature algorithm without pre-hashing (PureEdDSA), with a context.

        .. versionadded:: 1.4

    This message-signature algorithm can be used with both the message and message with context signature functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part verify operation, but not with the multi-part sign operation. It can also be used with the interruptible signature and verification operations.

    This is the PureEdDSA digital signature algorithm defined by :RFC-title:`8032`, with a context parameter.
    The context parameter can be between zero and 255 bytes in length.

    PureEdDSA requires an elliptic curve key on a twisted Edwards curve (see `PSA_ECC_FAMILY_TWISTED_EDWARDS`).
    The following curves are supported:

    *   Edwards25519: the Ed25519ctx algorithm is computed.
        The output signature is a 64-byte string: the concatenation of :math:`R` and :math:`S` as defined by :RFC:`8032#5.1.6`.

    *   Edwards448: the Ed448 algorithm is computed, with a zero-length context.
        The output signature is a 114-byte string: the concatenation of :math:`R` and :math:`S` as defined by :RFC:`8032#5.2.6`.

    To use a non-zero-length context, use the message-signature functions that accept a context parameter, :code:`psa_sign_message_with_context()` and :code:`psa_verify_message_with_context()`
    The `psa_sign_message()` and `psa_verify_message()` functions use a zero-length context when computing or verifying signatures.

    .. note::
        An interruptible signature operation for this algorithm must receive the complete message in one call to `psa_sign_iop_update()`. An interruptible verification operation can receive the message in fragments.

    .. note::
        To sign or verify the pre-computed hash of a message using EdDSA, the HashEdDSA algorithms (`PSA_ALG_ED25519PH` and `PSA_ALG_ED448PH`) can be used.
        The signature produced by HashEdDSA is distinct from that produced by PureEdDSA.

    .. note::
        With an Edwards25519 curve key, `PSA_ALG_EDDSA_CTX` with a zero-length context creates different signatures to `PSA_ALG_PURE_EDDSA`.

    .. subsection:: Usage

        This is a message signing algorithm. To calculate a signature, use one of the following approaches:

        *   Call `psa_sign_message()` or `psa_sign_message_with_context()` with the message.

        Verifying a signature is similar, using `psa_verify_message()` or `psa_verify_message_with_context()`.
        A signature of a fragmented message can be verified using a `psa_verify_operation_t` multi-part verify operation.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS)` (signature verification only)

.. macro:: PSA_ALG_ED25519PH
    :definition: ((psa_algorithm_t) 0x0600090B)

    .. summary::
        Edwards-curve digital signature algorithm with pre-hashing (HashEdDSA), using the Edwards25519 curve.

        .. versionadded:: 1.1

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This calculates the Ed25519ph algorithm as specified in :RFC-title:`8032#5.1`, and requires an Edwards25519 curve key.

    The pre-hash function is SHA-512, see `PSA_ALG_SHA_512`.
    When used to sign or verify a hash, the ``hash`` parameter is the SHA-512 message digest.

    The signature functions without a context parameter use a zero-length context when computing or verifying signatures.
    To use a non-zero-length context, use the signature functions that accept a context parameter, such as :code:`psa_sign_hash_with_context()` or :code:`psa_verify_message_with_context()`
    The context parameter can be between zero and 255 bytes in length.

    .. subsection:: Usage

        This is a hash-and-sign algorithm.
        To calculate a signature, use one of the following approaches:

        *   Call `psa_sign_message()` or `psa_sign_message_with_context()` with the message.

        *   Use a `psa_sign_operation_t` multi-part sign operation.

        *   Calculate the SHA-512 hash of the message with `psa_hash_compute()`, or with a multi-part hash operation, using the hash algorithm `PSA_ALG_SHA_512`.
            Then sign the calculated hash with `psa_sign_hash()` or `psa_sign_hash_with_context()`.

        Verifying a signature is similar, using one of the following approaches:

        *   Call `psa_verify_message()`, or `psa_verify_message_with_context()` with the message.

        *   Use a `psa_verify_operation_t` multi-part verify operation.

        *   Calculate the SHA-512 hash of the message with `psa_hash_compute()`, or with a multi-part hash operation, using the hash algorithm `PSA_ALG_SHA_512`.
            Then verify the calculated hash with `psa_verify_hash()` or `psa_verify_hash_with_context()`.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS)` (signature verification only)

    .. admonition:: Implementation note

        When used to sign or verify a hash, the ``hash`` parameter to the call should be used as :math:`\text{PH}(M)` in the algorithms defined in :RFC:`8032#5.1`.

.. macro:: PSA_ALG_ED448PH
    :definition: ((psa_algorithm_t) 0x06000915)

    .. summary::
        Edwards-curve digital signature algorithm with pre-hashing (HashEdDSA), using the Edwards448 curve.

        .. versionadded:: 1.1

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This calculates the Ed448ph algorithm as specified in :RFC-title:`8032#5.2`, and requires an Edwards448 curve key.

    The pre-hash function is the first 64 bytes of the output from SHAKE256, see `PSA_ALG_SHAKE256_512`.
    When used to sign or verify a hash, the ``hash`` parameter is the truncated SHAKE256 message digest.

    The signature functions without a context parameter use a zero-length context when computing or verifying signatures.
    To use a non-zero-length context, use the signature functions that accept a context parameter, for example, `psa_sign_hash_with_context()` or `psa_verify_message_with_context()`
    The context parameter can be between zero and 255 bytes in length.

    .. subsection:: Usage

        This is a hash-and-sign algorithm.
        To calculate a signature, use one of the following approaches:

        *   Call `psa_sign_message()`, or `psa_sign_message_with_context()` with the message.

        *   Use a `psa_sign_operation_t` multi-part sign operation.

        *   Calculate the first 64 bytes of the SHAKE256 output of the message with `psa_hash_compute()`, or with a multi-part hash operation, using the hash algorithm `PSA_ALG_SHAKE256_512`.
            Then sign the calculated hash with `psa_sign_hash()` or `psa_sign_hash_with_context()`.

        Verifying a signature is similar, using one of the following approaches:

        *   Call `psa_verify_message()`, or `psa_verify_message_with_context()` with the message.

        *   Use a `psa_verify_operation_t` multi-part verify operation.

        *   Calculate the first 64 bytes of the SHAKE256 output of the message with `psa_hash_compute()`, or with a multi-part hash operation, using the hash algorithm `PSA_ALG_SHAKE256_512`.
            Then verify the calculated hash with `psa_verify_hash()` or `psa_verify_hash_with_context()`.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)`
        | :code:`PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS)` (signature verification only)

    .. admonition:: Implementation note

        When used to sign or verify a hash, the ``hash`` parameter to the call should be used as :math:`\text{PH}(M)` in the algorithms defined in :RFC:`8032#5.2`.

.. macro:: PSA_ALG_IS_HASH_EDDSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is HashEdDSA.

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a HashEdDSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. _slh-dsa-algorithms:

Stateless Hash-based signature algorithms
-----------------------------------------

The SLH-DSA signature and verification scheme is defined in :cite-title:`FIPS205`.
SLH-DSA has twelve parameter sets which provide differing security strengths, trade-off between signature size and computation cost, and selection between SHA2 and SHAKE-based hashing.

SLH-DSA keys are fairly compact, 32, 48, or 64 bytes for the public key, and double that for the key pair.
SLH-DSA signatures are much larger than those for RSA and Elliptic curve schemes, between 7.8kB and 49kB depending on the selected parameter set.
An SLH-DSA signature has the structure described in `[FIPS205]` §9.2, Figure 17.

See `[FIPS205]` §11 for details on the parameter sets, and the public key and generated signature sizes.

The generation of an SLH-DSA key depends on the full parameter specification.
The encoding of each parameter set into the key attributes is described in :secref:`slh-dsa-keys`.

`[FIPS205]` defines pure and pre-hashed variants of the signature scheme, which can either be hedged (randomized) or deterministic.
Four algorithms are defined to support these variants: `PSA_ALG_SLH_DSA`, `PSA_ALG_DETERMINISTIC_SLH_DSA`, `PSA_ALG_HASH_SLH_DSA()`, and `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()`.

.. _slh-dsa-deterministic-signatures:

.. rubric:: Hedged and deterministic signatures

Hedging incorporates fresh randomness in the signature computation, resulting in distinct signatures on every signing operation when given identical inputs.
Deterministic signatures do not require additional random data, and result in an identical signature for the same inputs.

Signature verification does not distinguish between a hedged and a deterministic signature.
Either hedged or deterministic algorithms can be used when verifying a signature.

When computing a signature, the key's permitted-algorithm policy must match the requested algorithm, treating hedged and deterministic versions as distinct.
When verifying a signature, the hedged and deterministic versions of each algorithm are considered equivalent when checking the key's permitted-algorithm policy.

.. note::

    The hedged version provides message secrecy and some protection against side-channels.
    `[FIPS205]` recommends that users should use the hedged version if either of these issues are a concern.
    The deterministic variant should only be used if the implementation does not include any source of randomness.

.. admonition:: Implementation note

    `[FIPS205]` recommends that implementations use an approved random number generator to provide the random value in the hedged version.
    However, it notes that use of the hedged variant with a weak RNG is generally preferable to the deterministic variant.

.. rationale::

    The use of fresh randomness, or not, when computing a signature seems like an implementation decision based on the capability of the system, and its vulnerability to specific threats, following the recommendations in `[FIPS205]`.

    However, the |API| gives distinct algorithm identifiers for the hedged and deterministic variants for the following reasons:

    *   `[FIPS205]` §9.1 recommends that SLH-DSA signing keys are only used to compute either deterministic, or hedged, signatures, but not both.
        Supporting this recommendation requires separate algorithm identifiers, and requiring an exact policy match for signature computation.
    *   Enable an application use case to require a specific variant.

.. rubric:: Pure and pre-hashed algorithms

The pre-hashed signature computation *HashSLH-DSA* generates distinct signatures to a pure signature *SLH-DSA*, with the same key and message hashing algorithm.

An SLH-DSA signature can only be verified with an SLH-DSA algorithm.
A HashSLH-DSA signature can only be verified with a HashSLH-DSA algorithm.

:numref:`tab-slh-dsa-oid` lists the hash algorithm OIDs to use with the HashSLH-DSA algorithm.
Note that for HashSLH-DSA the DER-encoded OID includes the tag and length.

.. csv-table:: Hash algorithm OID to use in HashSLH-DSA
    :name: tab-slh-dsa-oid
    :header-rows: 1
    :class: longtable
    :widths: 6 7 7 8

    Hash algorithm, OID (dot notation), OID (ASN.1 hex), Reference
    :code:`PSA_ALG_SHA_256`, 2.16.840.1.101.3.4.2.1, ``0609608648016503040201``, :RFC-title:`8017#B.1`
    :code:`PSA_ALG_SHA_512_256`, 2.16.840.1.101.3.4.2.6, ``0609608648016503040206``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA_384`, 2.16.840.1.101.3.4.2.2, ``0609608648016503040202``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA_512`, 2.16.840.1.101.3.4.2.3, ``0609608648016503040203``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA3_256`, 2.16.840.1.101.3.4.2.8, ``0609608648016503040208``, :RFC-title:`9688#2`
    :code:`PSA_ALG_SHA3_384`, 2.16.840.1.101.3.4.2.9, ``0609608648016503040209``, :RFC:`9688#2`
    :code:`PSA_ALG_SHA3_512`, 2.16.840.1.101.3.4.2.10, ``060960864801650304020a``, :RFC:`9688#2`
    :code:`PSA_ALG_SHAKE128_256`, 2.16.840.1.101.3.4.2.11, ``060960864801650304020b``, :RFC-title:`8702#2`
    :code:`PSA_ALG_SHAKE256_512`, 2.16.840.1.101.3.4.2.12, ``060960864801650304020c``, :RFC:`8702#2`
    :code:`PSA_ALG_SM3`, 1.2.156.10197.1.504, ``06082a811ccf55018378``, :cite-title:`SM3-draft` §8.1.3

.. _slh-dsa-contexts:

.. rubric:: Contexts

All SLH-DSA algorithms can be used with contexts, which enables domain-separation when signatures are made of different message structures with the same key.
Context values are arbitrary strings between zero and 255 bytes in length.

*   The signature functions without a context parameter provide a zero-length context when computing or verifying SLH-DSA signatures.
*   To provide a context, use the ``psa_xxxx_with_context()`` signature functions with a context parameter, such as :code:`psa_sign_message_with_context()`.
*   Interruptible signature operations use a zero-length context.

.. macro:: PSA_ALG_SLH_DSA
    :definition: ((psa_algorithm_t) 0x06004000)

    .. summary::
        Stateless hash-based digital signature algorithm without pre-hashing (SLH-DSA).

        .. versionadded:: 1.3

    This message-signature algorithm can be used with both the message and message with context signature functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part verify operation, but not with the multi-part sign operation. It can also be used with the interruptible signature and verification operations.

    This is the pure SLH-DSA digital signature algorithm, defined by :cite-title:`FIPS205`, using hedging.
    SLH-DSA requires an SLH-DSA key, which determines the SLH-DSA parameter set for the operation.

    This algorithm is randomized: each invocation returns a different, equally valid signature.
    See the `notes on hedged signatures <slh-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on SLH-DSA contexts <slh-dsa-contexts_>`_.

    .. note::
        An interruptible signature operation for this algorithm must receive the complete message in one call to `psa_sign_iop_update()`. An interruptible verification operation can receive the message in fragments.

    When `PSA_ALG_SLH_DSA` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_SLH_DSA` as the algorithm in a call to any single-part message signing function or when setting up an interruptible signature operation.
    *   `PSA_ALG_SLH_DSA` or `PSA_ALG_DETERMINISTIC_SLH_DSA` as the algorithm in a call to any message signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        To sign or verify the pre-computed hash of a message using SLH-DSA, the HashSLH-DSA algorithms (`PSA_ALG_HASH_SLH_DSA()` and `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()`) can also be used with :code:`psa_sign_hash()` and :code:`psa_verify_hash()`.

        The signature produced by HashSLH-DSA is distinct from that produced by SLH-DSA.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_SLH_DSA_KEY_PAIR()`
        | :code:`PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY()` (signature verification only)

.. macro:: PSA_ALG_DETERMINISTIC_SLH_DSA
    :definition: ((psa_algorithm_t) 0x06004100)

    .. summary::
        Deterministic stateless hash-based digital signature algorithm without pre-hashing (SLH-DSA).

        .. versionadded:: 1.3

    This message-signature algorithm can be used with both the message and message with context signature functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part verify operation, but not with the multi-part sign operation. It can also be used with the interruptible signature and verification operations.

    This is the pure SLH-DSA digital signature algorithm, defined by `[FIPS205]`, without hedging.
    SLH-DSA requires an SLH-DSA key, which determines the SLH-DSA parameter set for the operation.

    This algorithm is deterministic: each invocation with the same inputs returns an identical signature.

    .. warning::
        It is recommended to use the hedged `PSA_ALG_SLH_DSA` algorithm instead, when supported by the implementation.
        See the `notes on deterministic signatures <slh-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on SLH-DSA contexts <slh-dsa-contexts_>`_.

    .. note::
        An interruptible signature operation for this algorithm must receive the complete message in one call to `psa_sign_iop_update()`. An interruptible verification operation can receive the message in fragments.

    When `PSA_ALG_DETERMINISTIC_SLH_DSA` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_DETERMINISTIC_SLH_DSA` as the algorithm in a call to any single-part message signing function or when setting up an interruptible signature operation.
    *   `PSA_ALG_SLH_DSA` or `PSA_ALG_DETERMINISTIC_SLH_DSA` as the algorithm in a call to any message signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        To sign or verify the pre-computed hash of a message using SLH-DSA, the HashSLH-DSA algorithms (`PSA_ALG_HASH_SLH_DSA()` and `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()`) can also be used with :code:`psa_sign_hash()` and :code:`psa_verify_hash()`.

        The signature produced by HashSLH-DSA is distinct from that produced by SLH-DSA.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_SLH_DSA_KEY_PAIR()`
        | :code:`PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY()` (signature verification only)

.. macro:: PSA_ALG_HASH_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Stateless hash-based digital signature algorithm with pre-hashing (HashSLH-DSA).

        .. versionadded:: 1.3

    .. param:: hash_alg
        A hash algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.
        This includes :code:`PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding HashSLH-DSA signature algorithm, using ``hash_alg`` to pre-hash the message.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pre-hashed SLH-DSA digital signature algorithm, defined by `[FIPS205]`, using hedging.
    SLH-DSA requires an SLH-DSA key, which determines the SLH-DSA parameter set for the operation.

    .. note::
        For the pre-hashing, `[FIPS205]` §10.2 recommends the use of an approved hash function with an equivalent, or better, security strength than the chosen SLH-DSA parameter set.

        :numref:`tab-slh-dsa-oid` lists the hash algorithm OID values to use when implementing HashSLH-DSA.

    This algorithm is randomized: each invocation returns a different, equally valid signature.
    See the `notes on hedged signatures <slh-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on SLH-DSA contexts <slh-dsa-contexts_>`_.

    When `PSA_ALG_HASH_SLH_DSA()` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_HASH_SLH_DSA()` as the algorithm in a call to any signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_HASH_SLH_DSA()` or `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()` as the algorithm in a call to any signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        The signature produced by HashSLH-DSA is distinct from that produced by SLH-DSA.

    .. subsection:: Usage

        This is a hash-and-sign algorithm. To calculate a signature, use one of the following approaches:

        *   Call :code:`psa_sign_message()` or :code:`psa_sign_message_with_context()` with the message.

        *   Calculate the hash of the message with :code:`psa_hash_compute()`, or with a multi-part hash operation, using the ``hash_alg`` hash algorithm.
            Note that ``hash_alg`` can be extracted from the signature algorithm using :code:`PSA_ALG_GET_HASH(sig_alg)`.
            Then sign the calculated hash either with :code:`psa_sign_hash()` or, if the protocol requires the use of a non-zero-length context, with :code:`psa_sign_hash_with_context()`.

        *   Use a multi-part sign operation, `psa_sign_operation_t`, to process the message in one or more fragments before extracting the signature.

        Verifying a signature is similar, using :code:`psa_verify_message()` or :code:`psa_verify_hash()` instead of the signature function, using :code:`psa_verify_message_with_context()` or :code:`psa_verify_hash_with_context()` if a non-zero-length context has been used, or using a multi-part verify operation, `psa_verify_operation_t`.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_SLH_DSA_KEY_PAIR()`
        | :code:`PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY()` (signature verification only)

.. macro:: PSA_ALG_DETERMINISTIC_HASH_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Deterministic stateless hash-based digital signature algorithm with pre-hashing (HashSLH-DSA).

        .. versionadded:: 1.3

    .. param:: hash_alg
        A hash algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.
        This includes :code:`PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding deterministic HashSLH-DSA signature algorithm, using ``hash_alg`` to pre-hash the message.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pre-hashed SLH-DSA digital signature algorithm, defined by `[FIPS205]`, without hedging.
    SLH-DSA requires an SLH-DSA key, which determines the SLH-DSA parameter set for the operation.

    .. note::
        For the pre-hashing, `[FIPS205]` §10.2 recommends the use of an approved hash function with an equivalent, or better, security strength than the chosen SLH-DSA parameter set.

        :numref:`tab-slh-dsa-oid` lists the hash algorithm OID values to use when implementing HashSLH-DSA.

    This algorithm is deterministic: each invocation with the same inputs returns an identical signature.

    .. warning::
        It is recommended to use the hedged `PSA_ALG_HASH_SLH_DSA()` algorithm instead, when supported by the implementation.
        See the `notes on deterministic signatures <slh-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on SLH-DSA contexts <slh-dsa-contexts_>`_.

    When `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()` as the algorithm in a call to any signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_HASH_SLH_DSA()` or `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA()` as the algorithm in a call to any signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        The signature produced by HashSLH-DSA is distinct from that produced by SLH-DSA.

    .. subsection:: Usage

        See `PSA_ALG_HASH_SLH_DSA()` for example usage.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_SLH_DSA_KEY_PAIR()`
        | :code:`PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY()` (signature verification only)

.. macro:: PSA_ALG_IS_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is SLH-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an SLH-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_ALG_IS_HASH_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is HashSLH-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a HashSLH-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

.. macro:: PSA_ALG_IS_DETERMINISTIC_HASH_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is deterministic HashSLH-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a deterministic HashSLH-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_HASH_SLH_DSA()` and `PSA_ALG_IS_HEDGED_HASH_SLH_DSA()`.

.. macro:: PSA_ALG_IS_HEDGED_HASH_SLH_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is hedged HashSLH-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a hedged HashSLH-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_HASH_SLH_DSA()` and `PSA_ALG_IS_DETERMINISTIC_HASH_SLH_DSA()`.

.. _ml-dsa-algorithms:

Module Lattice-based signature algorithms
-----------------------------------------

The ML-DSA signature and verification scheme is defined in :cite-title:`FIPS204`.
ML-DSA has three parameter sets which provide differing security strengths.

ML-DSA keys are large: 1.2--2.5kB for the public key, and triple that for the key pair.
ML-DSA signatures are much larger than those for RSA and Elliptic curve schemes, between 2.4kB and 4.6kB, depending on the selected parameter set.

See `[FIPS204]` §4 for details on the parameter sets, and the key and generated signature sizes.

The generation of an ML-DSA key depends on the full parameter specification.
The encoding of each parameter set into the key attributes is described in :secref:`ml-dsa-keys`.

`[FIPS204]` defines pure and pre-hashed variants of the signature scheme, which can either be hedged (randomized) or deterministic.
Four algorithms are defined to support these variants: `PSA_ALG_ML_DSA`, `PSA_ALG_DETERMINISTIC_ML_DSA`, `PSA_ALG_HASH_ML_DSA()`, and `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()`.

.. _ml-dsa-deterministic-signatures:

.. rubric:: Hedged and deterministic signatures

Hedging incorporates fresh randomness in the signature computation, resulting in distinct signatures on every signing operation when given identical inputs.
Deterministic signatures do not require additional random data, and result in an identical signature for the same inputs.

Signature verification does not distinguish between a hedged and a deterministic signature.
Either hedged or deterministic algorithms can be used when verifying a signature.

When computing a signature, the key's permitted-algorithm policy must match the requested algorithm, treating hedged and deterministic versions as distinct.
When verifying a signature, the hedged and deterministic versions of each algorithm are considered equivalent when checking the key's permitted-algorithm policy.

.. note::

    The hedged version provides message secrecy and some protection against side-channels.
    `[FIPS204]` recommends that users should use the hedged version if either of these issues are a concern.
    The deterministic variant should only be used if the implementation does not include any source of randomness.

.. admonition:: Implementation note

    `[FIPS204]` recommends that implementations use an approved random number generator to provide the random value in the hedged version.
    However, it notes that use of the hedged variant with a weak RNG is generally preferable to the deterministic variant.

.. rationale::

    The use of fresh randomness, or not, when computing a signature seems like an implementation decision based on the capability of the system, and its vulnerability to specific threats, following the recommendations in `[FIPS204]`.

    However, the |API| gives distinct algorithm identifiers for the hedged and deterministic variants, to enable an application use case to require a specific variant.

.. rubric:: Pure and pre-hashed algorithms

The pre-hashed signature computation *HashML-DSA* generates distinct signatures to a pure signature *ML-DSA*, with the same key and message hashing algorithm.

An ML-DSA signature can only be verified with an ML-DSA algorithm.
A HashML-DSA signature can only be verified with a HashML-DSA algorithm.

:numref:`tab-ml-dsa-oid` lists the hash algorithm OIDs to use with the HashML-DSA algorithm.
Note that for HashML-DSA the DER-encoded OID includes the tag and length.

.. csv-table:: Hash algorithm OID to use in HashML-DSA
    :name: tab-ml-dsa-oid
    :header-rows: 1
    :class: longtable
    :widths: 6 7 7 8

    Hash algorithm, OID (dot notation), OID (ASN.1 hex), Reference
    :code:`PSA_ALG_SHA_256`, 2.16.840.1.101.3.4.2.1, ``0609608648016503040201``, :RFC-title:`8017#B.1`
    :code:`PSA_ALG_SHA_512_256`, 2.16.840.1.101.3.4.2.6, ``0609608648016503040206``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA_384`, 2.16.840.1.101.3.4.2.2, ``0609608648016503040202``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA_512`, 2.16.840.1.101.3.4.2.3, ``0609608648016503040203``, :RFC:`8017#B.1`
    :code:`PSA_ALG_SHA3_256`, 2.16.840.1.101.3.4.2.8, ``0609608648016503040208``, :RFC-title:`9688#2`
    :code:`PSA_ALG_SHA3_384`, 2.16.840.1.101.3.4.2.9, ``0609608648016503040209``, :RFC:`9688#2`
    :code:`PSA_ALG_SHA3_512`, 2.16.840.1.101.3.4.2.10, ``060960864801650304020a``, :RFC:`9688#2`
    :code:`PSA_ALG_SHAKE128_256`, 2.16.840.1.101.3.4.2.11, ``060960864801650304020b``, :RFC-title:`8702#2`
    :code:`PSA_ALG_SHAKE256_512`, 2.16.840.1.101.3.4.2.12, ``060960864801650304020c``, :RFC:`8702#2`
    :code:`PSA_ALG_SM3`, 1.2.156.10197.1.504, ``06082a811ccf55018378``, :cite-title:`SM3-draft` §8.1.3

.. _ml-dsa-contexts:

.. rubric:: Contexts

All ML-DSA algorithms can be used with contexts, which enables domain-separation when signatures are made of different message structures with the same key.
Context values are arbitrary strings between zero and 255 bytes in length.

*   The signature functions without a context parameter provide a zero-length context when computing or verifying ML-DSA signatures.
*   To provide a context, use the ``psa_xxxx_with_context()`` signature functions with a context parameter, such as :code:`psa_sign_message_with_context()`.
*   Interruptible signature operations use a zero-length context.

.. macro:: PSA_ALG_ML_DSA
    :definition: ((psa_algorithm_t) 0x06004400)

    .. summary::
        Module lattice-based digital signature algorithm without pre-hashing (ML-DSA).

        .. versionadded:: 1.3

    This message-signature algorithm can be used with both the message and message with context signature functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pure ML-DSA digital signature algorithm, defined by :cite-title:`FIPS204`, using hedging.
    ML-DSA requires an ML-DSA key, which determines the ML-DSA parameter set for the operation.

    This algorithm is randomized: each invocation returns a different, equally valid signature.
    See the `notes on hedged signatures <ml-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on ML-DSA contexts <ml-dsa-contexts_>`_.

    When `PSA_ALG_ML_DSA` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_ML_DSA` as the algorithm in a call to any message signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_ML_DSA` or `PSA_ALG_DETERMINISTIC_ML_DSA` as the algorithm in a call to any message signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        To sign or verify the pre-computed hash of a message using ML-DSA, the HashML-DSA algorithms (`PSA_ALG_HASH_ML_DSA()` and `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()`) can also be used with :code:`psa_sign_hash()` and :code:`psa_verify_hash()`.

        The signature produced by HashML-DSA is distinct from that produced by ML-DSA.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_ML_DSA_KEY_PAIR`
        | `PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_DETERMINISTIC_ML_DSA
    :definition: ((psa_algorithm_t) 0x06004500)

    .. summary::
        Deterministic module lattice-based digital signature algorithm without pre-hashing (ML-DSA).

        .. versionadded:: 1.3

    This message-signature algorithm can be used with both the message and message with context signature functions.
    It cannot be used to sign hashes.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pure ML-DSA digital signature algorithm, defined by :cite-title:`FIPS204`, without hedging.
    ML-DSA requires an ML-DSA key, which determines the ML-DSA parameter set for the operation.

    This algorithm is deterministic: each invocation with the same inputs returns an identical signature.

    .. warning::
        It is recommended to use the hedged `PSA_ALG_ML_DSA` algorithm instead, when supported by the implementation.
        See the `notes on deterministic signatures <ml-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on ML-DSA contexts <ml-dsa-contexts_>`_.

    When `PSA_ALG_DETERMINISTIC_ML_DSA` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_DETERMINISTIC_ML_DSA` as the algorithm in a call to any message signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_ML_DSA` or `PSA_ALG_DETERMINISTIC_ML_DSA` as the algorithm in a call to any message signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        To sign or verify the pre-computed hash of a message using ML-DSA, the HashML-DSA algorithms (`PSA_ALG_HASH_ML_DSA()` and `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()`) can also be used with :code:`psa_sign_hash()` and :code:`psa_verify_hash()`.

        The signature produced by HashML-DSA is distinct from that produced by ML-DSA.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_ML_DSA_KEY_PAIR`
        | :code:`PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_HASH_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Module lattice-based digital signature algorithm with pre-hashing (HashML-DSA).

        .. versionadded:: 1.3

    .. param:: hash_alg
        A hash algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.
        This includes :code:`PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding HashML-DSA signature algorithm, using ``hash_alg`` to pre-hash the message.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pre-hashed ML-DSA digital signature algorithm, defined by :cite-title:`FIPS204`, using hedging.
    ML-DSA requires an ML-DSA key, which determines the ML-DSA parameter set for the operation.

    .. note::
        For the pre-hashing, `[FIPS204]` §5.4 recommends the use of an approved hash function with an equivalent, or better, security strength than the chosen ML-DSA parameter set.

        :numref:`tab-ml-dsa-oid` lists the hash algorithm OID values to use when implementing HashML-DSA.

    This algorithm is randomized: each invocation returns a different, equally valid signature.
    See the `notes on hedged signatures <ml-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on ML-DSA contexts <ml-dsa-contexts_>`_.

    When `PSA_ALG_HASH_ML_DSA()` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_HASH_ML_DSA()` as the algorithm in a call to any signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_HASH_ML_DSA()` or `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()` as the algorithm in a call to any signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        The signature produced by HashML-DSA is distinct from that produced by ML-DSA.

    .. subsection:: Usage

        This is a hash-and-sign algorithm. To calculate a signature, use one of the following approaches:

        *   Call :code:`psa_sign_message()` or :code:`psa_sign_message_with_context()` with the message.

        *   Calculate the hash of the message with :code:`psa_hash_compute()`, or with a multi-part hash operation, using the ``hash_alg`` hash algorithm.
            Note that ``hash_alg`` can be extracted from the signature algorithm using :code:`PSA_ALG_GET_HASH(sig_alg)`.
            Then sign the calculated hash either with :code:`psa_sign_hash()` or, if the protocol requires the use of a non-zero-length context, with :code:`psa_sign_hash_with_context()`.

        *   Use a multi-part sign operation, `psa_sign_operation_t`, to process the message in one or more fragments before extracting the signature.

        Verifying a signature is similar, using :code:`psa_verify_message()` or :code:`psa_verify_hash()` instead of the signature function, using :code:`psa_verify_message_with_context()` or :code:`psa_verify_hash_with_context()` if a non-zero-length context has been used, or using a multi-part verify operation, `psa_verify_operation_t`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_ML_DSA_KEY_PAIR`
        | `PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_DETERMINISTIC_HASH_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Deterministic module lattice-based digital signature algorithm with pre-hashing (HashML-DSA).

        .. versionadded:: 1.3

    .. param:: hash_alg
        A hash algorithm: a value of type :code:`psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.
        This includes :code:`PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    .. return::
        The corresponding deterministic HashML-DSA signature algorithm, using ``hash_alg`` to pre-hash the message.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This hash-and-sign signature algorithm can be used with both the message and hash signature functions.
    This algorithm can be used with the multi-part sign and verify operations. It can also be used with the interruptible signature and verification operations.

    This is the pre-hashed ML-DSA digital signature algorithm, defined by :cite-title:`FIPS204`, without hedging.
    ML-DSA requires an ML-DSA key, which determines the ML-DSA parameter set for the operation.

    .. note::
        For the pre-hashing, `[FIPS204]` §5.4 recommends the use of an approved hash function with an equivalent, or better, security strength than the chosen ML-DSA parameter set.

        :numref:`tab-ml-dsa-oid` lists the hash algorithm OID values to use when implementing HashML-DSA.

    This algorithm is deterministic: each invocation with the same inputs returns an identical signature.

    .. warning::
        It is recommended to use the hedged `PSA_ALG_HASH_ML_DSA()` algorithm instead, when supported by the implementation.
        See the `notes on deterministic signatures <ml-dsa-deterministic-signatures_>`_.

    This algorithm has a context parameter.
    See the `notes on ML-DSA contexts <ml-dsa-contexts_>`_.

    When `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()` is used as a permitted algorithm in a key policy, this permits:

    *   `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()` as the algorithm in a call to any signing function or when setting up a multi-part sign operation or an interruptible signature operation.
    *   `PSA_ALG_HASH_ML_DSA()` or `PSA_ALG_DETERMINISTIC_HASH_ML_DSA()` as the algorithm in a call to any signature verification function or when setting up a multi-part verify operation or an interruptible verification operation.

    .. note::
        The signature produced by HashML-DSA is distinct from that produced by ML-DSA.

    .. subsection:: Usage

        See `PSA_ALG_HASH_ML_DSA()` for example usage.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_ML_DSA_KEY_PAIR`
        | `PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_IS_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is ML-DSA, without pre-hashing.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a pure ML-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    .. note::
        Use `PSA_ALG_IS_HASH_ML_DSA()` to determine if an algorithm identifier is a HashML-DSA algorithm.

.. macro:: PSA_ALG_IS_HASH_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is HashML-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a HashML-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    .. note::
        Use `PSA_ALG_IS_ML_DSA()` to determine if an algorithm identifier is a pre-hashed ML-DSA algorithm.

.. macro:: PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is deterministic HashML-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a deterministic HashML-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_HASH_ML_DSA()` and `PSA_ALG_IS_HEDGED_HASH_ML_DSA()`.

.. macro:: PSA_ALG_IS_HEDGED_HASH_ML_DSA
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is hedged HashML-DSA.

        .. versionadded:: 1.3

    .. param:: alg
        An algorithm identifier: a value of type :code:`psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a hedged HashML-DSA algorithm, ``0`` otherwise.

        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    See also `PSA_ALG_IS_HASH_ML_DSA()` and `PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA()`.

.. _lms-algorithms:

Leighton-Micali Signature algorithms
------------------------------------

The Leighton-Micali Signatures (LMS) and multi-level Hierarchical Signature Scheme (HSS) schemes are defined in :rfc-title:`8554`.

For the |API| to support signature verification, it is only necessary to define a public keys for these schemes, and the default public key formats for import and export.

.. rationale::

    At present, it is not expected that the |API| will be used to generate LMS or HSS private keys, or to carry out signing operations.
    However, there is value in supporting verification of LMS and HSS signatures.
    Therefore, the |API| does not support LMS or HSS key pairs, or the associated signing operations.

.. note::
    A full set of NIST-approved parameter sets for LMS and HSS is defined in :cite-title:`SP800-208` §4, with the additional IANA identifiers defined in :rfc-title:`9858`.

.. macro:: PSA_ALG_LMS
    :definition: ((psa_algorithm_t) 0x06004800)

    .. summary::
        Leighton-Micali Signatures (LMS) signature algorithm.

        .. versionadded:: 1.3

    This message-signature algorithm can be used with the :code:`psa_verify_message()` function, or with the multi-part verify operation, `psa_verify_operation_t`.
    It cannot be used to sign messages, or sign or verify hashes.
    LMS does not have a context parameter.
    However, :code:`psa_verify_message_with_context()` can be used with a zero-length context.

    This is the LMS stateful hash-based signature algorithm, defined by :rfc-title:`8554`.
    LMS requires an LMS key.
    The key and the signature must both encode the same LMS parameter set, which is used for the verification procedure.

    .. note::
        LMS signature calculation is not supported.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_LMS_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_HSS
    :definition: ((psa_algorithm_t) 0x06004900)

    .. summary::
        Hierarchical Signature Scheme (HSS) signature algorithm.

        .. versionadded:: 1.3

    This message-signature algorithm can be used with the :code:`psa_verify_message()` function, or with the multi-part verify operation, `psa_verify_operation_t`.
    It cannot be used to sign messages, or sign or verify hashes.
    HSS does not have a context parameter.
    However, :code:`psa_verify_message_with_context()` can be used with a zero-length context.

    This is the HSS stateful hash-based signature algorithm, defined by :rfc-title:`8554`.
    HSS requires an HSS key.
    The key and the signature must both encode the same HSS parameter set, which is used for the verification procedure.

    .. note::
        HSS signature calculation is not supported.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_HSS_PUBLIC_KEY` (signature verification only)

.. _xmss-algorithms:

XMSS and |XMSS^MT| algorithms
-----------------------------

The eXtended Merkle Signature Scheme (XMSS), and the multi-tree variant |XMSS^MT|, are defined in :rfc-title:`8391`.

For the |API| to support signature verification, it is only necessary to define public keys for these schemes, and the default public key formats for import and export.

.. rationale::

    At present, it is not expected that the |API| will be used to generate XMSS or |XMSS^MT| private keys, or to carry out signing operations.
    However, there is value in supporting verification of XMSS and |XMSS^MT| signatures.
    Therefore, the |API| does not support XMSS or |XMSS^MT| key pairs, or the associated signing operations.

.. note::
    A full set of NIST-approved parameter sets for XMSS or |XMSS^MT| is defined in :cite-title:`SP800-208` §5.

.. macro:: PSA_ALG_XMSS
    :definition: ((psa_algorithm_t) 0x06004A00)

    .. summary::
        eXtended Merkle Signature Scheme (XMSS) signature algorithm.

        .. versionadded:: 1.3

    This message-signature algorithm can be used with the :code:`psa_verify_message()` function, or with the multi-part verify operation, `psa_verify_operation_t`.
    It cannot be used to sign messages, or sign or verify hashes.
    XMSS does not have a context parameter.
    However, :code:`psa_verify_message_with_context()` can be used with a zero-length context.

    This is the XMSS stateful hash-based signature algorithm, defined by :rfc-title:`8391`.
    XMSS requires an XMSS key.
    The key and the signature must both encode the same XMSS parameter set, which is used for the verification procedure.

    .. note::
        XMSS signature calculation is not supported.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_XMSS_PUBLIC_KEY` (signature verification only)

.. macro:: PSA_ALG_XMSS_MT
    :definition: ((psa_algorithm_t) 0x06004B00)

    .. summary::
        Multi-tree eXtended Merkle Signature Scheme (|XMSS^MT|) signature algorithm.

        .. versionadded:: 1.3

    This message-signature algorithm can be used with the :code:`psa_verify_message()` function, or with the multi-part verify operation, `psa_verify_operation_t`.
    It cannot be used to sign messages, or sign or verify hashes.
    |XMSS^MT| does not have a context parameter.
    However, :code:`psa_verify_message_with_context()` can be used with a zero-length context.

    This is the |XMSS^MT| stateful hash-based signature algorithm, defined by :rfc-title:`8391`.
    |XMSS^MT| requires an |XMSS^MT| key.
    The key and the signature must both encode the same |XMSS^MT| parameter set, which is used for the verification procedure.

    .. note::
        |XMSS^MT| signature calculation is not supported.

    .. subsection:: Compatible key types

        | :code:`PSA_KEY_TYPE_XMSS_MT_PUBLIC_KEY` (signature verification only)

.. _single-part-signature:

Single-part asymmetric signature functions
------------------------------------------

.. function:: psa_sign_message

    .. summary::
        Sign a message with a private key.
        For hash-and-sign algorithms, this includes the hashing step.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.
    .. param:: const uint8_t * input
        The input message to sign.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.
    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits signing a message.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits signing a message.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is too large for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    If the algorithm has a context parameter, a zero-length context is used.
    To provide a context value, use `psa_sign_message_with_context()` instead.

    ..  note::
        To calculate a signature for a fragmented message, either:

        *   From version 1.5 of the |API|, use a `psa_sign_operation_t` multi-part sign operation.
        *   For a hash-and-sign signature algorithm, use a `psa_hash_operation_t` multi-part hash operation and then pass the resulting hash to `psa_sign_hash()`.
            :code:`PSA_ALG_GET_HASH(alg)` can be used to determine the hash algorithm to use.

        An implementation that supports interruptible signature operations can instead use `psa_sign_iop_t` when bounded execution time is required. This API has distinct availability and completion requirements, and is not a replacement for the multi-part signature API.

.. function:: psa_sign_message_with_context

    .. summary::
        Sign a message with a private key using a supplied context.
        For hash-and-sign algorithms, this includes the hashing step.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.
    .. param:: const uint8_t * input
        The input message to sign.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * context
        The context to use for this signature.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.
    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits signing a message.
        *   ``key`` is not supported for use with ``alg``.
        *   The implementation does not support this value of ``context_length`` for ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits signing a message with a non-zero-length context.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is too large for the algorithm and key type.
        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    If a context parameter is not required, `psa_sign_message()` can be used instead.

    ..  note::
        To calculate a signature for a fragmented message, either:

        *   From version 1.5 of the |API|, use a `psa_sign_operation_t` multi-part sign operation.
        *   For a hash-and-sign signature algorithm, use a `psa_hash_operation_t` multi-part hash operation and then pass the resulting hash to `psa_sign_hash_with_context()`.
            :code:`PSA_ALG_GET_HASH(alg)` can be used to determine the hash algorithm to use.

        Interruptible signature operations use a zero-length context. To use a non-zero-length context, use this function instead.

.. function:: psa_verify_message

    .. summary::
        Verify the signature of a message with a public key.
        For hash-and-sign algorithms, this includes the hashing step.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be a public key or an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.
    .. param:: const uint8_t * input
        The message whose signature is to be verified.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        ``signature`` is not the result of signing the ``input`` message with algorithm ``alg`` using the private key corresponding to ``key``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits verifying a message.
        *   ``key`` is not supported for use with ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits verifying a message.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is too large for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    If the algorithm has a context parameter, a zero-length context is used.
    To provide a context value, use `psa_verify_message_with_context()` instead.

    ..  note::
        To verify a signature for a fragmented message, either:

        *   From version 1.5 of the |API|, use a `psa_verify_operation_t` multi-part verify operation.
        *   For a hash-and-sign signature algorithm, use a `psa_hash_operation_t` multi-part hash operation and then pass the resulting hash to `psa_verify_hash()`.
            :code:`PSA_ALG_GET_HASH(alg)` can be used to determine the hash algorithm to use.

        An implementation that supports interruptible signature operations can instead use `psa_verify_iop_t` when bounded execution time is required. This API has distinct availability and completion requirements, and is not a replacement for the multi-part signature API.

.. function:: psa_verify_message_with_context

    .. summary::
        Verify the signature of a message with a public key and a supplied context.
        For hash-and-sign algorithms, this includes the hashing step.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be a public key or an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.
    .. param:: const uint8_t * input
        The message whose signature is to be verified.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.
    .. param:: const uint8_t * context
        The context to use for this signature.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        ``signature`` is not the result of signing the ``input`` message with algorithm ``alg`` using the private key corresponding to ``key``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits verifying a message.
        *   ``key`` is not supported for use with ``alg``.
        *   The implementation does not support this value of ``context_length`` for ``alg``.
        *   ``input_length`` is too large for the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits verifying a message with a non-zero-length context.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
        *   ``input_length`` is too large for the algorithm and key type.
        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    If a context parameter is not required, `psa_verify_message()` can be used instead.

    ..  note::
        To verify a signature for a fragmented message, either:

        *   From version 1.5 of the |API|, use a `psa_verify_operation_t` multi-part verify operation.
        *   For a hash-and-sign signature algorithm, use a `psa_hash_operation_t` multi-part hash operation and then pass the resulting hash to `psa_verify_hash_with_context()`.
            :code:`PSA_ALG_GET_HASH(alg)` can be used to determine the hash algorithm to use.

        Interruptible signature operations use a zero-length context. To use a non-zero-length context, use this function instead.

.. function:: psa_sign_hash

    .. summary::
        Sign a pre-computed hash with a private key.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_SIGN_HASH`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm that separates the hash and sign operations: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_HASH(alg)` is true.
    .. param:: const uint8_t * hash
        The input to sign. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.

    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_HASH` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits signing a pre-computed hash.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits signing a pre-computed hash.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to sign.
    The algorithm used to calculate this hash is encoded in the signature algorithm.
    For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash.
    In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    If the algorithm has a context parameter, a zero-length context is used.
    To provide a context value, use `psa_sign_hash_with_context()` instead.

.. function:: psa_sign_hash_with_context

    .. summary::
        Sign a pre-computed hash with a private key and a supplied context.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_SIGN_HASH`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm that separates the hash and sign operations: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_HASH(alg)` is true.
    .. param:: const uint8_t * hash
        The input to sign. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.
    .. param:: const uint8_t * context
        The context to use for this signature.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are the type and bit-size respectively of ``key``.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.

    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_HASH` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits signing a pre-computed hash.
        *   The implementation does not support this value of ``context_length`` for ``alg``.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits signing a pre-computed hash with a context.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to sign.
    The algorithm used to calculate this hash is encoded in the signature algorithm.
    For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash.
    In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    If a context parameter is not required, `psa_sign_hash()` can be used instead.

.. function:: psa_verify_hash

    .. summary::
        Verify the signature of a hash or short message using a public key.

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be a public key or an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_VERIFY_HASH`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm that separates the hash and sign operations: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_HASH(alg)` is true.
    .. param:: const uint8_t * hash
        The input whose signature is to be verified. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_HASH` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        ``signature`` is not the result of signing ``hash`` with algorithm ``alg`` using the private key corresponding to ``key``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits verifying a pre-computed hash.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits verifying a pre-computed hash.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to verify.
    The algorithm used to calculate this hash is encoded in the signature algorithm.
    For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash.
    In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    If the algorithm has a context parameter, a zero-length context is used.
    To provide a context value, use `psa_verify_hash_with_context()` instead.

.. function:: psa_verify_hash_with_context

    .. summary::
        Verify the signature of a hash or short message using a public key and a supplied context.

        .. versionadded:: 1.4

    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be a public key or an asymmetric key pair. The key must permit the usage `PSA_KEY_USAGE_VERIFY_HASH`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm that separates the hash and sign operations: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_HASH(alg)` is true.
    .. param:: const uint8_t * hash
        The input whose signature is to be verified. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.
    .. param:: const uint8_t * context
        The context to use for this signature.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_HASH` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        ``signature`` is not the result of signing ``hash`` with algorithm ``alg`` using the private key corresponding to ``key``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits verifying a pre-computed hash.
        *   The implementation does not support this value of ``context_length`` for ``alg``.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits verifying a pre-computed hash with a context.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to verify.
    The algorithm used to calculate this hash is encoded in the signature algorithm.
    For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash.
    In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    If a context parameter is not required, `psa_verify_hash()` can be used instead.

.. _multi-part-signature:

Multi-part asymmetric signature operations
------------------------------------------

.. typedef:: /* implementation-defined type */ psa_sign_operation_t

    .. summary::
        The type of the state object for multi-part sign operations.

        .. versionadded:: 1.5

    Before calling any function on a sign operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_sign_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_sign_operation_t operation;

    *   Initialize the object to the initializer `PSA_SIGN_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_sign_operation_t operation = PSA_SIGN_OPERATION_INIT;

    *   Assign the result of the function `psa_sign_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_sign_operation_t operation;
            operation = psa_sign_operation_init();

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_SIGN_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a sign operation object of type `psa_sign_operation_t`.

        .. versionadded:: 1.5

.. function:: psa_sign_operation_init

    .. summary::
        Return an initial value for a sign operation object.

        .. versionadded:: 1.5

    .. return:: psa_sign_operation_t

.. function:: psa_sign_setup

    .. summary::
        Set up a multi-part sign operation.

        .. versionadded:: 1.5

    .. param:: psa_sign_operation_t * operation
        The operation object to set up.
        It must have been initialized as per the documentation for `psa_sign_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must be an asymmetric key pair.
        It must remain valid until the operation terminates.
        The key must permit the usage `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits signing a fragmented message.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits signing a fragmented message.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing by a call to `psa_crypto_init()`.

    The sequence of operations to sign a message using a multi-part sign operation is as follows:

    1.  Allocate a sign operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_sign_operation_t`, for example `PSA_SIGN_OPERATION_INIT`.
    #.  Call `psa_sign_setup()` to specify the key pair and algorithm.
    #.  Optionally, call `psa_sign_set_context()` to provide a context.
    #.  Call `psa_sign_update()` zero, one or more times, passing a fragment of the message each time.
        The signature that is calculated is the signature of the concatenation of these messages in order.
    #.  To calculate and extract the signature, call `psa_sign_finish()`.

    After a successful call to `psa_sign_setup()`, the operation is active, and the application must eventually terminate the operation.
    The following events terminate an operation:

    *   A successful call to `psa_sign_finish()`.
    *   A call to `psa_sign_abort()`.

    If `psa_sign_setup()` returns an error, the operation object is unchanged.
    If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_sign_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_sign_set_context

    .. summary::
        Provide a context for a multi-part sign operation.

        .. versionadded:: 1.5

    .. param:: psa_sign_operation_t * operation
        Active sign operation.
    .. param:: const uint8_t * context
        Buffer containing the context value.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and no call to `psa_sign_set_context()` or `psa_sign_update()` has been made.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The context value is not supported by this implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    This function sets the context value in a multi-part sign operation.

    The application must call `psa_sign_setup()` before calling this function.

    For a signature algorithm that has a context parameter:

    *   If this function is not called, the operation uses the algorithm with a zero-length or empty context.
    *   To set a non-zero-length context, call this function after `psa_sign_setup()` and before calling any other function on the sign operation.

    If a context parameter is not supported by the algorithm, this function call can be omitted, or can be called with a zero-length context.

    The macro `PSA_ALG_SIGN_SUPPORTS_CONTEXT()` can be used to determine if a signature algorithm supports non-zero-length context values.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_abort()`.

.. function:: psa_sign_update

    .. summary::
        Add a message fragment to a multi-part sign operation.

        .. versionadded:: 1.5

    .. param:: psa_sign_operation_t * operation
        Active sign operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to sign.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input for the operation is too large for the signature algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_sign_setup()` before calling this function.

    When the last fragment of the message has been input to the multi-part sign operation, call `psa_sign_finish()` to calculate and extract the signature.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_abort()`.

.. function:: psa_sign_finish

    .. summary::
        Finish the calculation of an asymmetric signature of a message.

        .. versionadded:: 1.5

    .. param:: psa_sign_operation_t * operation
        Active sign operation.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes.
        This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)`  where ``key_type`` and ``key_bits`` are the type and bit-size of the key, and ``alg`` is the algorithm that were used in the call to `psa_sign_setup()`.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.
    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_sign_setup()` before calling this function.
    This function calculates the asymmetric signature of the message formed by concatenating the inputs passed to preceding calls to `psa_sign_update()`.

    When this function returns successfully, the operation becomes inactive.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_abort()`.

.. function:: psa_sign_abort

    .. summary::
        Abort a sign operation.

        .. versionadded:: 1.5

    .. param:: psa_sign_operation_t * operation
        Initialized sign operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_sign_setup()` again.

    This function can be called any time after the operation object has been initialized by one of the methods described in `psa_sign_operation_t`.

    In particular, calling `psa_sign_abort()` after the operation has been terminated by a call to `psa_sign_abort()` or `psa_sign_finish()` is safe and has no effect.

.. typedef:: /* implementation-defined type */ psa_verify_operation_t

    .. summary::
        The type of the state object for multi-part asymmetric signature verification operations.

        .. versionadded:: 1.5

    Before calling any function on a verify operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_verify_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_verify_operation_t operation;

    *   Initialize the object to the initializer `PSA_VERIFY_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_verify_operation_t operation = PSA_VERIFY_OPERATION_INIT;

    *   Assign the result of the function `psa_verify_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_verify_operation_t operation;
            operation = psa_verify_operation_init();

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_VERIFY_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a verify operation object of type `psa_verify_operation_t`.

        .. versionadded:: 1.5

.. function:: psa_verify_operation_init

    .. summary::
        Return an initial value for a verify operation object.

        .. versionadded:: 1.5

    .. return:: psa_verify_operation_t

.. function:: psa_verify_setup

    .. summary::
        Set up a multi-part verify operation.

        .. versionadded:: 1.5

    .. param:: psa_verify_operation_t * operation
        The operation object to set up.
        It must have been initialized as per the documentation for `psa_verify_operation_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation.
        It must be a public key or an asymmetric key pair.
        It must remain valid until the operation terminates.
        The key must permit the usage `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN_MESSAGE(alg)` is true.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now active.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported, or is not an asymmetric signature algorithm that permits verifying a fragmented message.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm that permits verifying a fragmented message.
        *   ``key`` is not a public key or an asymmetric key pair, that is compatible with ``alg``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing by a call to `psa_crypto_init()`.

    The sequence of operations to verify a message signature using a multi-part verify operation is as follows:

    1.  Allocate a verify operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_verify_operation_t`, for example `PSA_VERIFY_OPERATION_INIT`.
    #.  Call `psa_verify_setup()` to specify the key, algorithm, and signature to verify.
    #.  Optionally, call `psa_verify_set_context()` to provide a context.
    #.  Call `psa_verify_update()` zero, one or more times, passing a fragment of the message each time.
        The signature is verified against the concatenation of these messages in order.
    #.  To determine the validity of the signature, call `psa_verify_finish()`.

    After a successful call to `psa_verify_setup()`, the operation is active, and the application must eventually terminate the operation.
    The following events terminate an operation:

    *   A successful call to `psa_verify_finish()`.
    *   A call to `psa_verify_abort()`.

    If `psa_verify_setup()` returns an error, the operation object is unchanged.
    If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_verify_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_verify_set_context

    .. summary::
        Provide a context for a multi-part verify operation.

        .. versionadded:: 1.5

    .. param:: psa_verify_operation_t * operation
        Active verify operation.
    .. param:: const uint8_t * context
        Buffer containing the context value.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and no call to `psa_verify_set_context()` or `psa_verify_update()` has been made.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``context_length`` is not valid for the algorithm and key type.
        *   ``context`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The context value is not supported by this implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    This function sets the context value in a multi-part verify operation.

    The application must call `psa_verify_setup()` before calling this function.

    For a signature algorithm that has a context parameter:

    *   If this function is not called, the operation uses the algorithm with a zero-length or empty context.
    *   To set a non-zero-length context, call this function after `psa_verify_setup()` and before calling any other function on the verify operation.

    If a context parameter is not supported by the algorithm, this function call can be omitted, or can be called with a zero-length context.

    The macro `PSA_ALG_SIGN_SUPPORTS_CONTEXT()` can be used to determine if a signature algorithm supports non-zero-length context values.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_abort()`.

.. function:: psa_verify_update

    .. summary::
        Add a message fragment to a multi-part verify operation.

        .. versionadded:: 1.5

    .. param:: psa_verify_operation_t * operation
        Active verify operation.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to verify.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input for the operation is too large for the signature algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_verify_setup()` before calling this function.

    When the last fragment of the message has been input to the multi-part verify operation, call `psa_verify_finish()` to determine the validity of the signature.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_abort()`.

.. function:: psa_verify_finish

    .. summary::
        Report the validity of the message signature.

        .. versionadded:: 1.5

    .. param:: psa_verify_operation_t * operation
        Active verify operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The signature being verified by the operation is not the result of signing the concatenated message with the operation's algorithm using the private key corresponding to the operation's key.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_verify_setup()` before calling this function.
    This function verifies the asymmetric signature with the message formed by concatenating the inputs passed to preceding calls to `psa_verify_update()`.

    When this function returns successfully, the operation becomes inactive.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_abort()`.

.. function:: psa_verify_abort

    .. summary::
        Abort a verify operation.

        .. versionadded:: 1.5

    .. param:: psa_verify_operation_t * operation
        Initialized verify operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_verify_setup()` again.

    This function can be called any time after the operation object has been initialized by one of the methods described in `psa_verify_operation_t`.

    In particular, calling `psa_verify_abort()` after the operation has been terminated by a call to `psa_verify_abort()` or `psa_verify_finish()` is safe and has no effect.

.. _interruptible-sign:

Interruptible asymmetric signature
----------------------------------

The interruptible asymmetric signature operation calculates the signature of a message, or pre-computed hash, in an interruptible manner. For example, this can enable an application to remain responsive in an execution environment that does not provide multi-tasking.

An interruptible asymmetric signature operation is used as follows:

1.  Allocate an interruptible asymmetric signature operation object, of type `psa_sign_iop_t`, which will be passed to all the functions listed here.
#.  Initialize the operation object with one of the methods described in the documentation for `psa_sign_iop_t`, for example, `PSA_SIGN_IOP_INIT`.
#.  Call `psa_sign_iop_setup()` to specify the algorithm and key.
#.  Call `psa_sign_iop_setup_complete()` to complete the setup, until this function does not return :code:`PSA_OPERATION_INCOMPLETE`.
#.  Either:

    1.  Call `psa_sign_iop_hash()` with a pre-computed hash of the message to sign; or
    2.  Call `psa_sign_iop_update()` one or more times, passing a fragment of the message each time. The signature that is calculated will that be of the concatenation of these fragments, in order.
#.  Call `psa_sign_iop_complete()` to finish calculating the signature value, until this function does not return :code:`PSA_OPERATION_INCOMPLETE`.
#.  If an error occurs at any stage, or to terminate the operation early, call `psa_sign_iop_abort()`.


.. typedef:: /* implementation-defined type */ psa_sign_iop_t

    .. summary::
        The type of the state data structure for an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    Before calling any function on an interruptible asymmetric signature operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_sign_iop_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_sign_iop_t operation;

    *   Initialize the object to the initializer `PSA_SIGN_IOP_INIT`, for example:

        .. code-block:: xref

            psa_sign_iop_t operation = PSA_SIGN_IOP_INIT;

    *   Assign the result of the function `psa_sign_iop_init()` to the object, for example:

        .. code-block:: xref

            psa_sign_iop_t operation;
            operation = psa_sign_iop_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_SIGN_IOP_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro evaluates to an initializer for an interruptible asymmetric signature operation object of type `psa_sign_iop_t`.

        .. versionadded:: 1.6

.. function:: psa_sign_iop_init

    .. summary::
        Return an initial value for an interruptible asymmetric signature operation object.

        .. versionadded:: 1.6

    .. return:: psa_sign_iop_t

.. function:: psa_sign_iop_get_num_ops

    .. summary::
        Get the number of *ops* that an interruptible asymmetric signature operation has taken so far.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to inspect.

    .. return:: uint32_t
        Number of *ops* that the operation has taken so far.

    After the interruptible operation has completed, the returned value is the number of *ops* required for the entire operation. The value is reset to zero by a call to either `psa_sign_iop_setup()` or `psa_sign_iop_abort()`.

    This function can be used to tune the value passed to `psa_iop_set_max_ops()`.

    The value is undefined if the operation object has not been initialized.

.. function:: psa_sign_iop_setup

    .. summary::
        Begin the setup of an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to set up. It must have been initialized as per the documentation for `psa_sign_iop_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair. The key must either permit the usage `PSA_KEY_USAGE_SIGN_HASH` or `PSA_KEY_USAGE_SIGN_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation setup must now be completed by calling `psa_sign_iop_setup_complete()`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   The key has neither the `PSA_KEY_USAGE_SIGN_HASH` nor the `PSA_KEY_USAGE_SIGN_MESSAGE` usage flag.
        *   The key does not permit the requested algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an asymmetric signature algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm.
        *   ``key`` is not an asymmetric key pair, that is compatible with ``alg``.
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
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY

    This function sets up the calculation of an asymmetric signature of a message or pre-computed hash. To verify an asymmetric signature against an expected value, use an interruptible asymmetric verification operation, see :secref:`interruptible-verify`.

    After a successful call to `psa_sign_iop_setup()`, the operation is in setup state. Setup can be completed by calling `psa_sign_iop_setup_complete()` repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`. Once setup has begun, the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_sign_iop_complete()`.
    *   A call to `psa_sign_iop_abort()`.

    If `psa_sign_iop_setup()` returns an error, the operation object is unchanged.

.. function:: psa_sign_iop_setup_complete

    .. summary::
        Finish setting up an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to use. The operation must be in the process of being set up.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now ready for input of data to sign.
    .. retval:: PSA_OPERATION_INCOMPLETE
        The function was interrupted after exhausting the maximum *ops*. The computation is incomplete, and this function must be called again with the same operation object to continue.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation setup must have started, but not yet finished.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY

    .. note::
        This is an interruptible function, and must be called repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    When this function returns successfully, the operation is ready for data input using a call to `psa_sign_iop_hash()` or `psa_sign_iop_update()`.
    If this function returns :code:`PSA_OPERATION_INCOMPLETE`, setup is not complete, and this function must be called again to continue the operation.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_iop_abort()`.

    The amount of calculation performed in a single call to this function is determined by the maximum *ops* setting. See `psa_iop_set_max_ops()`.

.. function:: psa_sign_iop_hash

    .. summary::
        Input a pre-computed hash to an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to use. The operation must have been set up, with no data input.
    .. param:: const uint8_t * hash
        The input to sign. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now ready for completion.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation must be set up, with no data input.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_HASH` flag.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm does not allow signing of a pre-computed hash.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The implementation does not support signing of a pre-computed hash.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY

    The application must complete the setup of the operation before calling this function.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to sign. The algorithm used to calculate this hash is encoded in the signature algorithm. For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash. In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    After input of the hash, the signature operation can be completed by calling `psa_sign_iop_complete()` until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_iop_abort()`.

.. function:: psa_sign_iop_update

    .. summary::
        Add a message fragment to an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to use. The operation must have been set up, with no hash value input.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to add to the signature calculation.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation must be set up, with no pre-computed hash value input.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_SIGN_MESSAGE` flag.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm does not allow signing of a message.
        *   The total input for the operation is too large for the signature algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   The implementation does not support signing of a message.
        *   The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY

    The application must complete the setup of the operation before calling this function.

    For message-signature algorithms that process the message data multiple times when computing a signature, `psa_sign_iop_update()` must be called exactly once with the entire message content. For signature algorithms that only process the message data once, the message content can be passed in a series of calls to `psa_sign_iop_update()`.

    After input of the message, the signature operation can be completed by calling `psa_sign_iop_complete()` until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_iop_abort()`.

    .. note::

        To sign the zero-length message using an interruptible operation, call `psa_sign_iop_update()` once with a zero-length message fragment before calling `psa_sign_iop_complete()`.

.. function:: psa_sign_iop_complete

    .. summary::
        Attempt to finish the interruptible calculation of an asymmetric signature.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible asymmetric signature operation to use. The operation must have hash or message data input, or be in the process of finishing.
    .. param:: uint8_t * signature
        Buffer where the signature is to be written.
    .. param:: size_t signature_size
        Size of the ``signature`` buffer in bytes. This must be appropriate for the selected algorithm and key:

        *   The required signature size is :code:`PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)` where ``key_type`` and ``key_bits`` are attributes of the key, and ``alg`` is the algorithm used to calculate the signature.
        *   `PSA_SIGNATURE_MAX_SIZE` evaluates to the maximum signature size of any supported signature algorithm.
    .. param:: size_t * signature_length
        On success, the number of bytes that make up the returned signature value.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*signature_length)`` bytes of ``signature`` contain the signature value.
    .. retval:: PSA_OPERATION_INCOMPLETE
        The function was interrupted after exhausting the maximum *ops*. The computation is incomplete, and this function must be called again with the same operation object to continue.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation setup must be complete, or a previous call to `psa_sign_iop_complete()` returned :code:`PSA_OPERATION_INCOMPLETE`.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``signature`` buffer is too small.
        `PSA_SIGN_OUTPUT_SIZE()` or `PSA_SIGNATURE_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY

    .. note::
        This is an interruptible function, and must be called repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    When this function returns successfully, the signature is returned in ``signature``, and the operation becomes inactive.
    If this function returns :code:`PSA_OPERATION_INCOMPLETE`, no signature is returned, and this function must be called again to continue the operation.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_sign_iop_abort()`.

    The amount of calculation performed in a single call to this function is determined by the maximum *ops* setting. See `psa_iop_set_max_ops()`.

.. function:: psa_sign_iop_abort

    .. summary::
        Abort an interruptible asymmetric signature operation.

        .. versionadded:: 1.6

    .. param:: psa_sign_iop_t * operation
        The interruptible signature operation to abort.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` structure itself. Once aborted, the operation object can be reused for another operation by calling `psa_sign_iop_setup()` again.

    This function can be called at any time after the operation object has been initialized as described in `psa_sign_iop_t`.

    In particular, it is valid to call `psa_sign_iop_abort()` twice, or to call `psa_sign_iop_abort()` on an operation that has not been set up.


.. _interruptible-verify:

Interruptible asymmetric verification
-------------------------------------

The interruptible asymmetric verification operation verifies the signature of a message, or pre-computed hash, in an interruptible manner. For example, this can enable an application to remain responsive in an execution environment that does not provide multi-tasking.

An interruptible asymmetric verification operation is used as follows:

1.  Allocate an interruptible asymmetric verification operation object, of type `psa_verify_iop_t`, which will be passed to all the functions listed here.
#.  Initialize the operation object with one of the methods described in the documentation for `psa_verify_iop_t`, for example, `PSA_VERIFY_IOP_INIT`.
#.  Call `psa_verify_iop_setup()` to specify the algorithm, key, and the signature to verify.
#.  Call `psa_verify_iop_setup_complete()` to complete the setup, until this function does not return :code:`PSA_OPERATION_INCOMPLETE`.
#.  Either:

    1.  Call `psa_verify_iop_hash()` with a pre-computed hash of the message to verify; or
    2.  Call `psa_verify_iop_update()` one or more times, passing a fragment of the message each time. The signature is verified against the concatenation of these fragments, in order.
#.  Call `psa_verify_iop_complete()` to finish verifying the signature value, until this function does not return :code:`PSA_OPERATION_INCOMPLETE`.
#.  If an error occurs at any stage, or to terminate the operation early, call `psa_verify_iop_abort()`.


.. typedef:: /* implementation-defined type */ psa_verify_iop_t

    .. summary::
        The type of the state data structure for an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    Before calling any function on an interruptible asymmetric verification operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_verify_iop_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_verify_iop_t operation;

    *   Initialize the object to the initializer `PSA_VERIFY_IOP_INIT`, for example:

        .. code-block:: xref

            psa_verify_iop_t operation = PSA_VERIFY_IOP_INIT;

    *   Assign the result of the function `psa_verify_iop_init()` to the object, for example:

        .. code-block:: xref

            psa_verify_iop_t operation;
            operation = psa_verify_iop_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_VERIFY_IOP_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro evaluates to an initializer for an interruptible asymmetric verification operation object of type `psa_verify_iop_t`.

        .. versionadded:: 1.6

.. function:: psa_verify_iop_init

    .. summary::
        Return an initial value for an interruptible asymmetric verification operation object.

        .. versionadded:: 1.6

    .. return:: psa_verify_iop_t

.. function:: psa_verify_iop_get_num_ops

    .. summary::
        Get the number of *ops* that an interruptible asymmetric verification operation has taken so far.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible asymmetric verification operation to inspect.

    .. return:: uint32_t
        Number of *ops* that the operation has taken so far.

    After the interruptible operation has completed, the returned value is the number of *ops* required for the entire operation. The value is reset to zero by a call to either `psa_verify_iop_setup()` or `psa_verify_iop_abort()`.

    This function can be used to tune the value passed to `psa_iop_set_max_ops()`.

    The value is undefined if the operation object has not been initialized.

.. function:: psa_verify_iop_setup

    .. summary::
        Begin the setup of an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to set up. It must have been initialized as per the documentation for `psa_verify_iop_t` and not yet in use.
    .. param:: psa_key_id_t key
        Identifier of the key to use for the operation. It must be an asymmetric key pair or asymmetric public key. The key must either permit the usage `PSA_KEY_USAGE_VERIFY_HASH` or `PSA_KEY_USAGE_VERIFY_MESSAGE`.
    .. param:: psa_algorithm_t alg
        An asymmetric signature algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN(alg)` is true.
    .. param:: const uint8_t * signature
        Buffer containing the signature to verify.
    .. param:: size_t signature_length
        Size of the ``signature`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation setup must now be completed by calling `psa_verify_iop_setup_complete()`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   The key has neither the `PSA_KEY_USAGE_VERIFY_HASH` nor the `PSA_KEY_USAGE_VERIFY_MESSAGE` usage flag.
        *   The key does not permit the requested algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``alg`` is not supported or is not an asymmetric signature algorithm.
        *   ``key`` is not supported for use with ``alg``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``alg`` is not an asymmetric signature algorithm.
        *   ``key`` is not an asymmetric key pair, or asymmetric public key, that is compatible with ``alg``.
        *   ``signature`` is not a valid signature for the algorithm and key.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        ``signature`` is not a valid signature for the algorithm and key.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function sets up the verification of an asymmetric signature of a message or pre-computed hash. To calculate an asymmetric signature, use an interruptible asymmetric signature operation, see :secref:`interruptible-sign`.

    After a successful call to `psa_verify_iop_setup()`, the operation is in setup state. Setup can be completed by calling `psa_verify_iop_setup_complete()` repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`. Once setup has begun, the application must eventually terminate the operation. The following events terminate an operation:

    *   A successful call to `psa_verify_iop_complete()`.
    *   A call to `psa_verify_iop_abort()`.

    If `psa_verify_iop_setup()` returns an error, the operation object is unchanged.

.. function:: psa_verify_iop_setup_complete

    .. summary::
        Finish setting up an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to use. The operation must be in the process of being set up.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now ready for input of data to verify.
    .. retval:: PSA_OPERATION_INCOMPLETE
        The function was interrupted after exhausting the maximum *ops*. The computation is incomplete, and this function must be called again with the same operation object to continue.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation setup must have started, but not yet finished.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The signature is not a valid signature for the algorithm and key.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    .. note::
        This is an interruptible function, and must be called repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    When this function returns successfully, the operation is ready for data input using a call to `psa_verify_iop_hash()` or `psa_verify_iop_update()`.
    If this function returns :code:`PSA_OPERATION_INCOMPLETE`, setup is not complete, and this function must be called again to continue the operation.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_iop_abort()`.

    The amount of calculation performed in a single call to this function is determined by the maximum *ops* setting. See `psa_iop_set_max_ops()`.

.. function:: psa_verify_iop_hash

    .. summary::
        Input a pre-computed hash to an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to use. The operation must have been set up, with no data input.
    .. param:: const uint8_t * hash
        The input whose signature is to be verified. This is usually the hash of a message.

        See the description of this function, or the description of individual signature algorithms, for details of the acceptable inputs.
    .. param:: size_t hash_length
        Size of the ``hash`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation is now ready for completion.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation must be set up, with no data input.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_HASH` flag.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm does not allow verification of a pre-computed hash.
        *   ``hash_length`` is not valid for the algorithm and key type.
        *   ``hash`` is not a valid input value for the algorithm and key type.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The implementation does not support verification of a pre-computed hash.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must complete the setup of the operation before calling this function.

    For hash-and-sign signature algorithms, the ``hash`` input to this function is the hash of the message to verify. The algorithm used to calculate this hash is encoded in the signature algorithm. For such algorithms, ``hash_length`` must equal the length of the hash output: :code:`hash_length == PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))`.

    Specialized signature algorithms can apply a padding or encoding to the hash. In such cases, the encoded hash must be passed to this function. For example, see `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`.

    After input of the hash, the verification operation can be completed by calling `psa_verify_iop_complete()` until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_iop_abort()`.


.. function:: psa_verify_iop_update

    .. summary::
        Add a message fragment to an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to use. The operation must have been set up, with no hash value input.
    .. param:: const uint8_t * input
        Buffer containing the message fragment to add to the verification.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation must be set up, with no pre-computed hash value input.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The key does not have the `PSA_KEY_USAGE_VERIFY_MESSAGE` flag.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm does not allow verification of a message.
        *   The total input for the operation is too large for the signature algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   The implementation does not support signing of a message.
        *   The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The application must complete the setup of the operation before calling this function.

    For message-signature algorithms that process the message data multiple times when verifying a signature, `psa_verify_iop_update()` must be called exactly once with the entire message content. For signature algorithms that only process the message data once, the message content can be passed in a series of calls to `psa_verify_iop_update()`.

    After input of the message, the verification operation can be completed by calling `psa_verify_iop_complete()` until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_iop_abort()`.

    .. note::

        To verify the signature of the zero-length message using an interruptible operation, call `psa_verify_iop_update()` once with a zero-length message fragment before calling `psa_verify_iop_complete()`

.. function:: psa_verify_iop_complete

    .. summary::
        Attempt to finish the interruptible verification of an asymmetric signature.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to use. The operation must have hash or message data input, or be in the process of finishing.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The signature is valid.
    .. retval:: PSA_OPERATION_INCOMPLETE
        The function was interrupted after exhausting the maximum *ops*. The computation is incomplete, and this function must be called again with the same operation object to continue.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: the operation setup must be complete, or a previous call to `psa_verify_iop_complete()` returned :code:`PSA_OPERATION_INCOMPLETE`.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The signature is not the result of signing the input message, or hash value, with the requested algorithm, using the private key corresponding to the key provided to the operation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    .. note::
        This is an interruptible function, and must be called repeatedly, until it returns a status code that is not :code:`PSA_OPERATION_INCOMPLETE`.

    When this function returns successfully, the operation becomes inactive.
    If this function returns :code:`PSA_OPERATION_INCOMPLETE`, this function must be called again to continue the operation.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_verify_iop_abort()`.

    The amount of calculation performed in a single call to this function is determined by the maximum *ops* setting. See `psa_iop_set_max_ops()`.

.. function:: psa_verify_iop_abort

    .. summary::
        Abort an interruptible asymmetric verification operation.

        .. versionadded:: 1.6

    .. param:: psa_verify_iop_t * operation
        The interruptible verification operation to abort.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` structure itself. Once aborted, the operation object can be reused for another operation by calling `psa_verify_iop_setup()` again.

    This function can be called at any time after the operation object has been initialized as described in `psa_verify_iop_t`.

    In particular, it is valid to call `psa_verify_iop_abort()` twice, or to call `psa_verify_iop_abort()` on an operation that has not been set up.

Support macros
--------------

.. macro:: PSA_ALG_IS_SIGN_MESSAGE
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a signature algorithm that can be used with message signature and verification functions, multi-part signature operations, and interruptible signature operations.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a signature algorithm that can be used to sign a message. ``0`` if ``alg`` is a signature algorithm that can only be used to sign a pre-computed hash. ``0`` if ``alg`` is not a signature algorithm. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    This macro evaluates to ``1`` for hash-and-sign and message-signature algorithms.

.. macro:: PSA_ALG_IS_SIGN_HASH
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a signature algorithm that can be used with hash signature and verification functions, and with interruptible signature operations.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a signature algorithm that can be used to sign a hash. ``0`` if ``alg`` is a signature algorithm that can only be used to sign a message. ``0`` if ``alg`` is not a signature algorithm. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

    This macro evaluates to ``1`` for hash-and-sign and specialized signature algorithms.

.. macro:: PSA_ALG_IS_HASH_AND_SIGN
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a hash-and-sign algorithm that signs exactly the hash value.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a hash-and-sign algorithm that signs exactly the hash value, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported algorithm identifier.

        A wildcard signature algorithm policy, using `PSA_ALG_ANY_HASH`, returns the same value as the signature algorithm parameterized with a valid hash algorithm.

    This macro identifies algorithms that can be used with `psa_sign_hash()` that use the exact message hash value as an input the signature operation. For example, if :code:`PSA_ALG_IS_HASH_AND_SIGN(alg)` is true, the following call sequence is equivalent to :code:`psa_sign_message(key, alg, msg, msg_len, ...)`:

    .. code-block:: xref

        uint8_t hash[PSA_HASH_MAX_SIZE];
        size_t hash_len;
        psa_hash_compute(PSA_ALG_GET_HASH(alg), msg, msg_len,
                         hash, sizeof(hash), &hash_len);
        psa_sign_hash(key, alg, hash, hash_len, ...);

.. macro:: PSA_ALG_SIGN_SUPPORTS_CONTEXT
    :definition: /* implementation-defined value */

    .. summary::
        Whether the specified signature algorithm can be used with a non-zero-length context.

        .. versionadded:: 1.4

    .. param:: alg
        A signature algorithm identifier: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_SIGN(alg)` is true.

    .. return::
        ``1`` if ``alg`` is a signature algorithm that can be used with a non-zero-length context.
        ``0`` if ``alg`` is a signature algorithm that cannot be used with a non-zero-length context.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported signature algorithm identifier.

        A wildcard signature algorithm policy, using `PSA_ALG_ANY_HASH`, returns the same value as the signature algorithm parameterized with a valid hash algorithm.

    This macro identifies signature algorithms that have a context parameter, and can be used with the appropriate functions that support non-zero-length contexts.

.. macro:: PSA_ALG_ANY_HASH
    :definition: ((psa_algorithm_t)0x020000ff)

    .. summary::
        When setting a hash-and-sign algorithm in a key policy, permit any hash algorithm.

    This value can be used to form the permitted-algorithm attribute of a key policy for a signature algorithm that is parametrized by a hash. A key with this policy can then be used to perform operations using the same signature algorithm parametrized with any supported hash.
    A signature algorithm created using this macro is a wildcard algorithm, and `PSA_ALG_IS_WILDCARD()` will return true.

    This value must not be used to build other algorithms that are parametrized over a hash. For any valid use of this macro to build an algorithm ``alg``, :code:`PSA_ALG_IS_HASH_AND_SIGN(alg)` is true.

    This value cannot be used to build an algorithm specification to perform an operation. If used in this way, the operation will fail with an error.

    .. rubric:: Usage

    For example, suppose that ``PSA_xxx_SIGNATURE`` is one of the following macros:

    *   `PSA_ALG_RSA_PKCS1V15_SIGN`
    *   `PSA_ALG_RSA_PSS`
    *   `PSA_ALG_RSA_PSS_ANY_SALT`
    *   `PSA_ALG_ECDSA`
    *   `PSA_ALG_DETERMINISTIC_ECDSA`
    *   `PSA_ALG_HASH_SLH_DSA`
    *   `PSA_ALG_DETERMINISTIC_HASH_SLH_DSA`
    *   `PSA_ALG_HASH_ML_DSA`
    *   `PSA_ALG_DETERMINISTIC_HASH_ML_DSA`

    The following sequence of operations shows how `PSA_ALG_ANY_HASH` can be used in a key policy:

    1.  Set the key usage flags using `PSA_ALG_ANY_HASH`, for example:

        .. code-block:: xref

            psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE); // or VERIFY_MESSAGE
            psa_set_key_algorithm(&attributes, PSA_xxx_SIGNATURE(PSA_ALG_ANY_HASH));

    #.  Import or generate key material.
    #.  Call `psa_sign_message()` or `psa_verify_message()`, passing an algorithm built from ``PSA_xxx_SIGNATURE`` and a specific hash. Each call to sign or verify a message can use a different hash algorithm.

        .. code-block:: xref

            psa_sign_message(key, PSA_xxx_SIGNATURE(PSA_ALG_SHA_256), ...);
            psa_sign_message(key, PSA_xxx_SIGNATURE(PSA_ALG_SHA_512), ...);
            psa_sign_message(key, PSA_xxx_SIGNATURE(PSA_ALG_SHA3_256), ...);


.. macro:: PSA_SIGN_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient signature buffer size for `psa_sign_message()` and `psa_sign_hash()`.

    .. param:: key_type
        An asymmetric key type. This can be a key-pair type or a public-key type.
    .. param:: key_bits
        The size of the key in bits.
    .. param:: alg
        The signature algorithm.

    .. return::
        A sufficient signature buffer size for the specified asymmetric signature algorithm and key parameters. An implementation can return either ``0`` or a correct size for an asymmetric signature algorithm and key parameters that it recognizes, but does not support. If the parameters are not valid, the return value is unspecified.

    If the size of the signature buffer is at least this large, it is guaranteed that `psa_sign_message()` and `psa_sign_hash()` will not fail due to an insufficient buffer size. The actual size of the output might be smaller in any given call.

    See also `PSA_SIGNATURE_MAX_SIZE`.

.. macro:: PSA_SIGNATURE_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        A sufficient signature buffer size for `psa_sign_message()` and `psa_sign_hash()`, for any of the supported key types and asymmetric signature algorithms.

    If the size of the signature buffer is at least this large, it is guaranteed that `psa_sign_message()` and `psa_sign_hash()` will not fail due to an insufficient buffer size.

    See also `PSA_SIGN_OUTPUT_SIZE()`.
