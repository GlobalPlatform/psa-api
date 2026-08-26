.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 250

.. _kdf:

Key derivation
==============

A key derivation encodes a deterministic method to generate a finite stream of bytes. This data stream is computed by the cryptoprocessor and extracted in chunks. If two key-derivation operations are constructed with the same parameters, then they produce the same output.

A key derivation consists of two phases:

1.  Input collection. This is sometimes known as *extraction*: the operation “extracts” information from the inputs to generate a pseudorandom intermediate secret value.
#.  Output generation. This is sometimes known as *expansion*: the operation “expands” the intermediate secret value to the desired output length.

The specification defines a `multi-part operation <multi-part-operations>` API for key derivation that allows:

*   Multiple key and non-key outputs to be produced from a single derivation operation object.
*   Key and non-key outputs can be extracted from the key-derivation object, or compared with existing key and non-key values.
*   Algorithms that require high-entropy secret inputs. For example `PSA_ALG_HKDF`.
*   Algorithms that work with low-entropy secret inputs, or passwords. For example `PSA_ALG_PBKDF2_HMAC()`.

An implementation with :term:`isolation` has the following properties:

*   The intermediate state of the key derivation is not visible to the caller.
*   If an output of the derivation is a non-exportable key, then this key cannot be recovered outside the isolation boundary.
*   If an output of the derivation is compared using `psa_key_derivation_verify_bytes()` or `psa_key_derivation_verify_key()`, then the output is not visible to the caller.

Applications use the `psa_key_derivation_operation_t` type to create key-derivation operations. The operation object is used as follows:

1.  Initialize a `psa_key_derivation_operation_t` object to zero or to `PSA_KEY_DERIVATION_OPERATION_INIT`.
#.  Call `psa_key_derivation_setup()` to select a key-derivation algorithm.
#.  Call the functions `psa_key_derivation_input_key()` or `psa_key_derivation_key_agreement()` to provide the secret inputs, and `psa_key_derivation_input_bytes()` or `psa_key_derivation_input_integer()` to provide the non-secret inputs, to the key-derivation algorithm. Many key-derivation algorithms take multiple inputs; the ``step`` parameter to these functions indicates which input is being provided. The documentation for each key-derivation algorithm describes the expected inputs for that algorithm and in what order to pass them.
#.  Optionally, call `psa_key_derivation_set_capacity()` to set a limit on the amount of data that can be output from the key-derivation operation.
#.  Call an output or verification function:

    *   `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()` to create a derived key.
    *   `psa_key_derivation_output_bytes()` to export the derived data.
    *   `psa_key_derivation_verify_key()` to compare a derived key with an existing key value.
    *   `psa_key_derivation_verify_bytes()` to compare derived data with a buffer.

    These functions can be called multiple times to read successive output from the key derivation, until the stream is exhausted when its capacity has been reached.
#.  Key derivation does not finish in the same way as other multi-part operations. Call `psa_key_derivation_abort()` to release the key-derivation operation memory when the object is no longer required.

To recover from an error, call `psa_key_derivation_abort()` to release the key-derivation operation memory.

A key-derivation operation cannot be rewound. Once a part of the stream has been output, it cannot be output again. This ensures that the same part of the output will not be used for different purposes.

.. _key-derivation-algorithms:

Key-derivation algorithms
-------------------------

.. macro:: PSA_ALG_HKDF
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an HKDF algorithm.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding HKDF algorithm. For example, :code:`PSA_ALG_HKDF(PSA_ALG_SHA_256)` is HKDF using HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is the HMAC-based Extract-and-Expand Key Derivation Function (HKDF) specified by :RFC-title:`5869`.

    This key-derivation algorithm uses the following inputs:

    *   `PSA_KEY_DERIVATION_INPUT_SALT` is the salt used in the "extract" step. It is optional; if omitted, the derivation uses an empty salt.
    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the secret key (input keying material) used in the "extract" step.
    *   `PSA_KEY_DERIVATION_INPUT_INFO` is the info string used in the "expand" step.

    If `PSA_KEY_DERIVATION_INPUT_SALT` is provided, it must be before `PSA_KEY_DERIVATION_INPUT_SECRET`. `PSA_KEY_DERIVATION_INPUT_INFO` can be provided at any time after setup and before starting to generate output.

    .. warning::
       HKDF processes the salt as follows: first hash it with ``hash_alg`` if the salt is longer than the block size of the hash algorithm; then pad with null bytes up to the block size. As a result, it is possible for distinct salt inputs to result in the same outputs. To ensure unique outputs, it is recommended to use a fixed length for salt values.

    Each input may only be passed once.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_HKDF` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_HKDF`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for the secret key)
        | `PSA_KEY_TYPE_RAW_DATA` (for the other inputs)

.. macro:: PSA_ALG_HKDF_EXTRACT
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an HKDF-Extract algorithm.

        .. versionadded:: 1.1

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding HKDF-Extract algorithm. For example, :code:`PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256)` is HKDF-Extract using HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is the Extract step of HKDF as specified by :RFC-title:`5869#2.2`.

    This key-derivation algorithm uses the following inputs:

    *   `PSA_KEY_DERIVATION_INPUT_SALT` is the salt.
    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the input keying material used in the "extract" step.

    The inputs are mandatory and must be passed in the order above. Each input may only be passed once.

    .. warning::
       HKDF-Extract is not meant to be used on its own. `PSA_ALG_HKDF` should be used instead if possible. `PSA_ALG_HKDF_EXTRACT` is provided as a separate algorithm for the sake of protocols that use it as a building block. It may also be a slight performance optimization in applications that use HKDF with the same salt and key but many different info strings.

    .. warning::
       HKDF processes the salt as follows: first hash it with ``hash_alg`` if the salt is longer than the block size of the hash algorithm; then pad with null bytes up to the block size. As a result, it is possible for distinct salt inputs to result in the same outputs. To ensure unique outputs, it is recommended to use a fixed length for salt values.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_HKDF_EXTRACT` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_HKDF_EXTRACT`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for the input keying material)
        | `PSA_KEY_TYPE_RAW_DATA` (for the salt)

.. macro:: PSA_ALG_HKDF_EXPAND
    :definition: /* specification-defined value */

    .. summary::
        Macro to build an HKDF-Expand algorithm.

        .. versionadded:: 1.1

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding HKDF-Expand algorithm. For example, :code:`PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_256)` is HKDF-Expand using HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is the Expand step of HKDF as specified by :RFC-title:`5869#2.3`.

    This key-derivation algorithm uses the following inputs:

    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the pseudorandom key (PRK).
    *   `PSA_KEY_DERIVATION_INPUT_INFO` is the info string.

    The inputs are mandatory and must be passed in the order above. Each input may only be passed once.

    .. warning::
       HKDF-Expand is not meant to be used on its own. `PSA_ALG_HKDF` should be used instead if possible. `PSA_ALG_HKDF_EXPAND` is provided as a separate algorithm for the sake of protocols that use it as a building block. It may also be a slight performance optimization in applications that use HKDF with the same salt and key but many different info strings.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_HKDF_EXPAND` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_HKDF_EXPAND`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for the pseudorandom key)
        | `PSA_KEY_TYPE_RAW_DATA` (for the info string)

.. macro:: PSA_ALG_SP800_108_COUNTER_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a NIST SP 800-108 conformant, counter-mode KDF algorithm based on HMAC.

        .. versionadded:: 1.2

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding key-derivation algorithm. For example, the counter-mode KDF using HMAC-SHA-256 is :code:`PSA_ALG_SP800_108_COUNTER_HMAC(PSA_ALG_SHA_256)`.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is an HMAC-based, counter mode key-derivation function, using the construction recommended by :cite-title:`SP800-108`, §4.1.

    This key-derivation algorithm uses the following inputs:

    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the secret input keying material, :math:`K_{IN}`.
    *   `PSA_KEY_DERIVATION_INPUT_LABEL` is the :math:`Label`. It is optional; if omitted, :math:`Label` is a zero-length string. If provided, it must not contain any null bytes.
    *   `PSA_KEY_DERIVATION_INPUT_CONTEXT` is the :math:`Context`. It is optional; if omitted, :math:`Context` is a zero-length string.

    Each input can only be passed once. Inputs must be passed in the order above.

    This algorithm uses the output length as part of the derivation process. In the derivation this value is :math:`L`, the required output size in bits. After setup, the initial capacity of the key-derivation operation is :math:`2^{29}-1` bytes (``0x1fffffff``). The capacity can be set to a lower value by calling `psa_key_derivation_set_capacity()`.

    When the first output is requested, the value of :math:`L` is calculated as :math:`L=8*cap`, where :math:`cap` is the value of `psa_key_derivation_get_capacity()`. Subsequent calls to `psa_key_derivation_set_capacity()` are not permitted for this algorithm.

    The derivation is constructed as described in :cite:`SP800-108` §4.1, with the iteration counter :math:`i` and output length :math:`L` encoded as big-endian, 32-bit values. The resulting output stream :math:`K_1\ ||\ K_2\ ||\ K_3\ ||\ ...` is computed as:

    .. math::

        K_i = \text{HMAC}( K_{IN}, [i]_4\ ||\ Label\ ||\ \texttt{0x00}\ ||\ Context\ ||\ [L]_4 ),\quad\text{for }i = 1, 2, 3, ...

    Where :math:`[x]_n` is the big-endian, :math:`n`-byte encoding of the integer :math:`x`.

    .. rationale::

        :cite:`SP800-108` describes a set of general constructions for key-derivation algorithms, with flexibility for specific implementation requirements.

        The precise definition provided here enables compatibility between different implementations of the |API|.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_SP800_108_COUNTER_HMAC` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_SP800_108_COUNTER_HMAC`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_HMAC` (for the secret key)
        | `PSA_KEY_TYPE_DERIVE` (for the secret key)
        | `PSA_KEY_TYPE_RAW_DATA` (for the other inputs)

.. macro:: PSA_ALG_SP800_108_COUNTER_CMAC
    :definition: ((psa_algorithm_t)0x08000800)

    .. summary::
        Macro to build a NIST SP 800-108 conformant, counter-mode KDF algorithm based on CMAC.

        .. versionadded:: 1.2

    This is a CMAC-based, counter mode key-derivation function, using the construction recommended by :cite-title:`SP800-108`, §4.1.

    This key-derivation algorithm uses the following inputs:

    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the secret input keying material, :math:`K_{IN}`. This must be a block-cipher key that is compatible with the CMAC algorithm, and must be input using `psa_key_derivation_input_key()`. See also `PSA_ALG_CMAC`.
    *   `PSA_KEY_DERIVATION_INPUT_LABEL` is the :math:`Label`. It is optional; if omitted, :math:`Label` is a zero-length string. If provided, it must not contain any null bytes.
    *   `PSA_KEY_DERIVATION_INPUT_CONTEXT` is the :math:`Context`. It is optional; if omitted, :math:`Context` is a zero-length string.

    Each input can only be passed once. Inputs must be passed in the order above.

    This algorithm uses the output length as part of the derivation process. In the derivation this value is :math:`L`, the required output size in bits. After setup, the initial capacity of the key-derivation operation is :math:`2^{29}-1` bytes (``0x1fffffff``). The capacity can be set to a lower value by calling `psa_key_derivation_set_capacity()`.

    When the first output is requested, the value of :math:`L` is calculated as :math:`L=8*cap`, where :math:`cap` is the value of `psa_key_derivation_get_capacity()`. Subsequent calls to `psa_key_derivation_set_capacity()` are not permitted for this algorithm.

    The derivation is constructed as described in :cite:`SP800-108` §4.1, with the following details:

    *   The iteration counter :math:`i` and output length :math:`L` are encoded as big-endian, 32-bit values.
    *   The mitigation to make the CMAC-based construction robust is implemented.

    The resulting output stream :math:`K_1\ ||\ K_2\ ||\ K_3\ ||\ ...` is computed as:

    .. math::

        K_0 &= \text{CMAC}( K_{IN}, Label\ ||\ \texttt{0x00}\ ||\ Context\ ||\ [L]_4\ )

        K_i &= \text{CMAC}( K_{IN}, [i]_4\ ||\ Label\ ||\ \texttt{0x00}\ ||\ Context\ ||\ [L]_4\ ||\ K_0 ),\quad\text{for }i = 1, 2, 3, ...

    Where :math:`[x]_n` is the big-endian, :math:`n`-byte encoding of the integer :math:`x`.

    .. rationale::

        :cite:`SP800-108` describes a set of general constructions for key-derivation algorithms, with flexibility for specific implementation requirements.

        The precise definition provided here enables compatibility between different implementations of the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_AES` (for the secret key)
        | `PSA_KEY_TYPE_ARIA` (for the secret key)
        | `PSA_KEY_TYPE_CAMELLIA` (for the secret key)
        | `PSA_KEY_TYPE_SM4` (for the secret key)
        | `PSA_KEY_TYPE_RAW_DATA` (for the other inputs)

.. macro:: PSA_ALG_TLS12_PRF
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a TLS-1.2 PRF algorithm.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding TLS-1.2 PRF algorithm. For example, :code:`PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256)` represents the TLS 1.2 PRF using HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    TLS 1.2 uses a custom pseudorandom function (PRF) for key schedule, specified in :RFC-title:`5246#5`. It is based on HMAC and can be used with either SHA-256 or SHA-384.

    This key-derivation algorithm uses the following inputs, which must be passed in the order given here:

    *   `PSA_KEY_DERIVATION_INPUT_SEED` is the seed.
    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the secret key.
    *   `PSA_KEY_DERIVATION_INPUT_LABEL` is the label.

    Each input may only be passed once.

    For the application to TLS-1.2 key expansion:

    *   The seed is the concatenation of ``ServerHello.Random + ClientHello.Random``.
    *   The label is ``"key expansion"``.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for the secret key)
        | `PSA_KEY_TYPE_RAW_DATA` (for the other inputs)

.. macro:: PSA_ALG_TLS12_PSK_TO_MS
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a TLS-1.2 PSK-to-MasterSecret algorithm.

        .. versionchanged:: 1.1 Added step to support cipher-suites that include a key-exchange.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding TLS-1.2 PSK to MS algorithm. For example, :code:`PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256)` represents the TLS-1.2 PSK to MasterSecret derivation PRF using HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    In a pure-PSK handshake in TLS 1.2, the master secret (MS) is derived from the pre-shared key (PSK) through the application of padding (:RFC-title:`4279#2`) and the TLS-1.2 PRF (:RFC-title:`5246#5`). The latter is based on HMAC and can be used with either SHA-256 or SHA-384.

    This key-derivation algorithm uses the following inputs, which must be passed in the order given here:

    *   `PSA_KEY_DERIVATION_INPUT_SEED` is the seed.
    *   `PSA_KEY_DERIVATION_INPUT_OTHER_SECRET` is the other secret for the computation of the premaster secret. This input is optional; if omitted, it defaults to a string of null bytes with the same length as the secret (PSK) input.
    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the PSK. The PSK must not be larger than `PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE`.
    *   `PSA_KEY_DERIVATION_INPUT_LABEL` is the label.

    Each input may only be passed once.

    For the application to TLS-1.2:

    *   The seed, which is forwarded to the TLS-1.2 PRF, is the concatenation of the ``ClientHello.Random + ServerHello.Random``.
    *   The other secret depends on the key exchange specified in the cipher suite:

        -   For a plain PSK cipher suite (:RFC:`4279#2`), omit `PSA_KEY_DERIVATION_INPUT_OTHER_SECRET`.
        -   For a DHE-PSK (:RFC:`4279#3`) or ECDHE-PSK cipher suite (:RFC-title:`5489#2`), the other secret should be the output of the `PSA_ALG_FFDH` or `PSA_ALG_ECDH` key agreement performed with the peer. The recommended way to pass this input is to use a key-derivation algorithm constructed as :code:`PSA_ALG_KEY_AGREEMENT(ka_alg, PSA_ALG_TLS12_PSK_TO_MS(hash_alg))` and to call `psa_key_derivation_key_agreement()`. Alternatively, this input may be an output of `psa_key_agreement()` passed with `psa_key_derivation_input_key()`, or an equivalent input passed with `psa_key_derivation_input_bytes()` or `psa_key_derivation_input_key()`.
        -   For a RSA-PSK cipher suite (:RFC:`4279#4`), the other secret should be the 48-byte client challenge (the ``PreMasterSecret`` of :RFC:`5246#7.4.7.1`) concatenation of the TLS version and a 46-byte random string chosen by the client. On the server, this is typically an output of `psa_asymmetric_decrypt()` using `PSA_ALG_RSA_PKCS1V15_CRYPT`, passed to the key-derivation operation with `psa_key_derivation_input_bytes()`.

    *   The label is ``"master secret"`` or ``"extended master secret"``.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for the PSK)
        | `PSA_KEY_TYPE_RAW_DATA` (for the other inputs)

.. macro:: PSA_ALG_TLS12_ECJPAKE_TO_PMS
    :definition: ((psa_algorithm_t)0x08000609)

    .. summary::
        The TLS 1.2 ECJPAKE-to-PMS key-derivation algorithm.

        .. versionadded:: 1.2

    This KDF is defined in :cite-title:`TLS-ECJPAKE` §8.7. This specifies the use of a KDF to derive the TLS 1.2 session secrets from the output of EC J-PAKE over the secp256r1 Elliptic curve (the 256-bit curve in `PSA_ECC_FAMILY_SECP_R1`). EC J-PAKE operations can be performed using a PAKE operation, see :secref:`pake`.

    This KDF takes the shared secret :math:`K` (an uncompressed EC point in case of EC J-PAKE) and calculates :math:`\text{SHA256}(K.x)`.

    This function takes a single input:

    *   `PSA_KEY_DERIVATION_INPUT_SECRET` is the shared secret :math:`K` from EC J-PAKE. For secp256r1, the input is exactly 65 bytes.

        The shared secret can be obtained by calling :code:`psa_pake_get_shared_key()` on a PAKE operation that is performing the EC J-PAKE algorithm. See :secref:`pake`.

    The 32-byte output has to be read in a single call to either `psa_key_derivation_output_bytes()`, `psa_key_derivation_output_key()`, or `psa_key_derivation_output_key_custom()`. The size of the output is defined as `PSA_TLS12_ECJPAKE_TO_PMS_OUTPUT_SIZE`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` --- the secret key is extracted from a PAKE operation by calling :code:`psa_pake_get_shared_key()`.

.. macro:: PSA_ALG_WPA3_SAE_H2E
    :definition: /* specification-defined value */

    .. summary::
        The WPA3-SAE hash-to-element password token key-derivation algorithm.

        .. versionadded:: 1.4

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.
        This includes `PSA_ALG_ANY_HASH` when specifying the algorithm in a key policy.

    This KDF is defined in :cite-title:`IEEE-802.11` §12.4.4.
    This specifies the hash-to-element procedures for deriving a WPA3-SAE password token from a network SSID and password.
    The resulting password token is then used during a WPA3-SAE PAKE operation.

    This key-derivation algorithm uses the following inputs, which must be passed in the order given here:

    *   `PSA_KEY_DERIVATION_INPUT_SALT` is the network SSID.
    *   `PSA_KEY_DERIVATION_INPUT_PASSWORD` is the password.
    *   `PSA_KEY_DERIVATION_INPUT_INFO` is the password identifier.
        It is optional.

    This key derivation algorithm can only be used to derive and output a single key, which is obtained by a call to `psa_key_derivation_output_key()`.
    The output has to be read as a key of type `PSA_KEY_TYPE_WPA3_SAE_DH` or `PSA_KEY_TYPE_WPA3_SAE_ECC`.
    Requesting any other key type, or calling `psa_key_derivation_output_bytes()`, returns an error status.

    The ``hash_alg`` parameter to `PSA_ALG_WPA3_SAE_H2E()` determines the hash function used for the derivation.
    The key attributes of the output key indicate the elliptic curve or finite field group used for the derivation.

    If the elliptic curve or finite field group specified in the key attributes is not compatible with the hash function used for the derivation, `psa_key_derivation_output_bytes()` returns :code:`PSA_ERROR_INVALID_ARGUMENT`.
    See :secref:`wpa3-sae-cipher-suites`.

    :secref:`wpa3-sae-keys` provides details of the derivation procedures.

    .. note::

        To use a single password key with `PSA_ALG_WPA3_SAE_H2E` for any WPA3-SAE cipher suite, create the key with the wildcard `PSA_ALG_WPA3_SAE_ANY` permitted algorithm.

.. macro:: PSA_ALG_PBKDF2_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build a PBKDF2-HMAC password-hashing or key-stretching algorithm.

        .. versionadded:: 1.1

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        The corresponding PBKDF2-HMAC-XXX algorithm. For example, :code:`PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256)` is the algorithm identifier for PBKDF2-HMAC-SHA-256.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    PBKDF2 is specified by :RFC-title:`8018#5.2`. This macro constructs a PBKDF2 algorithm that uses a pseudorandom function based on HMAC with the specified hash.

    This key-derivation algorithm uses the following inputs, which must be provided in the following order:

    *   `PSA_KEY_DERIVATION_INPUT_COST` is the iteration count.
        This input step must be used exactly once.
    *   `PSA_KEY_DERIVATION_INPUT_SALT` is the salt.
        This input step must be used one or more times; if used several times, the inputs will be concatenated.
        This can be used to build the final salt from multiple sources, both public and secret (also known as pepper).
    *   `PSA_KEY_DERIVATION_INPUT_PASSWORD` is the password to be hashed.
        This input step must be used exactly once.

    .. admonition:: Implementation note

        The current version of this specification does not specify the behavior of `PSA_ALG_PBKDF2_HMAC` with hash algorithms that are not listed in :numref:`tab-hmac-hash`.
        It is recommended that these hash algorithms are not supported with `PSA_ALG_PBKDF2_HMAC`, as discussed in the specification of `PSA_ALG_HMAC`.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for password input)
        | `PSA_KEY_TYPE_PASSWORD` (for password input)
        | `PSA_KEY_TYPE_PEPPER` (for salt input)
        | `PSA_KEY_TYPE_RAW_DATA` (for salt input)
        | `PSA_KEY_TYPE_PASSWORD_HASH` (for key verification)

.. macro:: PSA_ALG_PBKDF2_AES_CMAC_PRF_128
    :definition: ((psa_algorithm_t)0x08800200)

    .. summary::
        The PBKDF2-AES-CMAC-PRF-128 password-hashing or key-stretching algorithm.

        .. versionadded:: 1.1

    PBKDF2 is specified by :RFC-title:`8018#5.2`. This algorithm specifies the PBKDF2 algorithm using the AES-CMAC-PRF-128 pseudorandom function specified by :RFC:`4615`

    This key-derivation algorithm uses the same inputs as `PSA_ALG_PBKDF2_HMAC()` with the same constraints.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_DERIVE` (for password input)
        | `PSA_KEY_TYPE_PASSWORD` (for password input)
        | `PSA_KEY_TYPE_PEPPER` (for salt input)
        | `PSA_KEY_TYPE_RAW_DATA` (for salt input)
        | `PSA_KEY_TYPE_PASSWORD_HASH` (for key verification)


Input step types
----------------

.. typedef:: uint16_t psa_key_derivation_step_t

    .. summary::
        Encoding of the step of a key derivation.

    .. admonition:: Implementation note

        It is recommended that the value ``0`` is not allocated as a valid key-derivation step.

.. macro:: PSA_KEY_DERIVATION_INPUT_SECRET
    :definition: /* implementation-defined value */

    .. summary::
        A high-entropy secret input for key derivation.

    This is typically a key of type `PSA_KEY_TYPE_DERIVE` passed to `psa_key_derivation_input_key()`, or the shared secret resulting from a key agreement obtained via `psa_key_derivation_key_agreement()`.

    For some algorithms, a specific type of key is required. For example, see `PSA_ALG_SP800_108_COUNTER_CMAC`.

    The secret can also be a direct input passed to `psa_key_derivation_input_bytes()`. In this case, the derivation operation cannot be used to derive keys: the operation will not permit a call to `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()`.

.. macro:: PSA_KEY_DERIVATION_INPUT_OTHER_SECRET
    :definition: /* implementation-defined value */

    .. summary::
        A high-entropy additional secret input for key derivation.

        .. versionadded:: 1.1

    This is typically the shared secret resulting from a key agreement obtained via `psa_key_derivation_key_agreement()`. It may alternatively be a key of type `PSA_KEY_TYPE_DERIVE` passed to `psa_key_derivation_input_key()`, or a direct input passed to `psa_key_derivation_input_bytes()`.

.. macro:: PSA_KEY_DERIVATION_INPUT_PASSWORD
    :definition: /* implementation-defined value */

    .. summary::
        A low-entropy secret input for password hashing or key stretching.

        .. versionadded:: 1.1

    This is usually a key of type `PSA_KEY_TYPE_PASSWORD` passed to `psa_key_derivation_input_key()` or a direct input passed to `psa_key_derivation_input_bytes()` that is a password or passphrase. It can also be high-entropy secret, for example, a key of type `PSA_KEY_TYPE_DERIVE`, or the shared secret resulting from a key agreement.

    If the secret is a direct input, the derivation operation cannot be used to derive keys: the operation will not permit a call to `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()`.

.. macro:: PSA_KEY_DERIVATION_INPUT_LABEL
    :definition: /* implementation-defined value */

    .. summary::
        A label for key derivation.

    This is typically a direct input. It can also be a key of type `PSA_KEY_TYPE_RAW_DATA`.

.. macro:: PSA_KEY_DERIVATION_INPUT_CONTEXT
    :definition: /* implementation-defined value */

    .. summary::
        A context for key derivation.

    This is typically a direct input. It can also be a key of type `PSA_KEY_TYPE_RAW_DATA`.

.. macro:: PSA_KEY_DERIVATION_INPUT_SALT
    :definition: /* implementation-defined value */

    .. summary::
        A salt for key derivation.

    This is typically a direct input. It can also be a key of type `PSA_KEY_TYPE_RAW_DATA` or `PSA_KEY_TYPE_PEPPER`.

.. macro:: PSA_KEY_DERIVATION_INPUT_INFO
    :definition: /* implementation-defined value */

    .. summary::
        An information string for key derivation.

    This is typically a direct input. It can also be a key of type `PSA_KEY_TYPE_RAW_DATA`.

.. macro:: PSA_KEY_DERIVATION_INPUT_SEED
    :definition: /* implementation-defined value */

    .. summary::
        A seed for key derivation.

    This is typically a direct input. It can also be a key of type `PSA_KEY_TYPE_RAW_DATA`.

.. macro:: PSA_KEY_DERIVATION_INPUT_COST
    :definition: /* implementation-defined value */

    .. summary::
        A cost parameter for password hashing or key stretching.

        .. versionadded:: 1.1

    This must be a direct input, passed to `psa_key_derivation_input_integer()`.

Key-derivation functions
------------------------

.. typedef:: /* implementation-defined type */ psa_key_derivation_operation_t

    .. summary::
        The type of the state object for key-derivation operations.

    Before calling any function on a key-derivation operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_key_derivation_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_key_derivation_operation_t operation;

    *   Initialize the object to the initializer `PSA_KEY_DERIVATION_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;

    *   Assign the result of the function `psa_key_derivation_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_key_derivation_operation_t operation;
            operation = psa_key_derivation_operation_init();

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_KEY_DERIVATION_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a key-derivation operation object of type `psa_key_derivation_operation_t`.

.. function:: psa_key_derivation_operation_init

    .. summary::
        Return an initial value for a key-derivation operation object.

    .. return:: psa_key_derivation_operation_t

.. function:: psa_key_derivation_setup

    .. summary::
        Set up a key-derivation operation.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to set up. It must have been initialized but not set up yet.
    .. param:: psa_algorithm_t alg
        The algorithm to compute. This must be one of the following:

        *   A key-derivation algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_DERIVATION(alg)` is true.
        *   A key-agreement and key-derivation algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_KEY_AGREEMENT(alg)` is true and :code:`PSA_ALG_IS_RAW_KEY_AGREEMENT(alg)` is false.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``alg`` is neither a key-derivation algorithm, nor a key-agreement and key-derivation algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``alg`` is not supported or is not a key-derivation algorithm, or a key-agreement and key-derivation algorithm.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.

    A key-derivation algorithm takes some inputs and uses them to generate a byte stream in a deterministic way. This byte stream can be used to produce keys and other cryptographic material.

    A key-agreement and key-derivation algorithm uses a key-agreement protocol to provide a shared secret which is used for the key derivation. See `psa_key_derivation_key_agreement()`.

    The sequence of operations to derive a key is as follows:

    1.  Allocate a key-derivation operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_key_derivation_operation_t`, e.g. `PSA_KEY_DERIVATION_OPERATION_INIT`.
    #.  Call `psa_key_derivation_setup()` to specify the algorithm.
    #.  Provide the inputs for the key derivation by calling `psa_key_derivation_input_bytes()` or `psa_key_derivation_input_key()` as appropriate. Which inputs are needed, in what order, whether keys are permitted, and what type of keys depends on the algorithm.
    #.  Optionally set the operation's maximum capacity with `psa_key_derivation_set_capacity()`. This can be done before, in the middle of, or after providing inputs. For some algorithms, this step is mandatory because the output depends on the maximum capacity.
    #.  To derive a key, call `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()`. To derive a byte string for a different purpose, call `psa_key_derivation_output_bytes()`. Successive calls to these functions use successive output bytes calculated by the key-derivation algorithm.
    #.  Clean up the key-derivation operation object with `psa_key_derivation_abort()`.

    After a successful call to `psa_key_derivation_setup()`, the operation is active, and the application must eventually terminate the operation with a call to `psa_key_derivation_abort()`.

    If `psa_key_derivation_setup()` returns an error, the operation object is unchanged. If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_key_derivation_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_key_derivation_get_capacity

    .. summary::
        Retrieve the current capacity of a key-derivation operation.

    .. param:: const psa_key_derivation_operation_t * operation
        The operation to query.
    .. param:: size_t * capacity
        On success, the capacity of the operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The maximum number of bytes that this key derivation can return is ``(*capacity)``.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The capacity of a key derivation is the maximum number of bytes that it can return. Reading :math:`N` bytes of output from a key-derivation operation reduces its capacity by at least :math:`N`. The capacity can be reduced by more than :math:`N` in the following situations:

    *   Calling `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()` can reduce the capacity by more than the key size, depending on the type of key being generated. See  `psa_key_derivation_output_key()` for details of the key-derivation process.
    *   When the `psa_key_derivation_operation_t` object is operating as a deterministic random bit generator (DBRG), which reduces capacity in whole blocks, even when less than a block is read.

.. function:: psa_key_derivation_set_capacity

    .. summary::
        Set the maximum capacity of a key-derivation operation.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to modify.
    .. param:: size_t capacity
        The new capacity of the operation. It must be less or equal to the operation's current capacity.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``capacity`` is larger than the operation's current capacity. In this case, the operation object remains valid and its capacity remains unchanged.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The capacity of a key-derivation operation is the maximum number of bytes that the key-derivation operation can return from this point onwards.

    .. note::

        For some algorithms, the capacity value can affect the output of the key derivation. For example, see `PSA_ALG_SP800_108_COUNTER_HMAC`.

.. function:: psa_key_derivation_input_bytes

    .. summary::
        Provide an input for key derivation or key agreement.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to use. It must have been set up with `psa_key_derivation_setup()` and must not have produced any output yet.
    .. param:: psa_key_derivation_step_t step
        Which step the input data is for.
    .. param:: const uint8_t * data
        Input data to use.
    .. param:: size_t data_length
        Size of the ``data`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``step`` is not compatible with the operation's algorithm.
        *   ``step`` does not permit direct inputs.
        *   ``data_length`` is too small or too large for ``step`` in this particular algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``step`` is not supported with the operation's algorithm.
        *   ``data_length`` is is not supported for ``step`` in this particular algorithm.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid for this input ``step``. This can happen if the application provides a step out of order or repeats a step that may not be repeated.
        *   The library requires initializing. See :secref:`library-init`.

    Which inputs are required and in what order depends on the algorithm. Refer to the documentation of each key-derivation or key-agreement algorithm for information.

    This function passes direct inputs, which is usually correct for non-secret inputs. To pass a secret input, which is normally in a key object, call `psa_key_derivation_input_key()` instead of this function. Refer to the documentation of individual step types (``PSA_KEY_DERIVATION_INPUT_xxx`` values of type `psa_key_derivation_step_t`) for more information.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

.. function:: psa_key_derivation_input_integer

    .. summary::
        Provide a numeric input for key derivation or key agreement.

        .. versionadded:: 1.1

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to use. It must have been set up with `psa_key_derivation_setup()` and must not have produced any output yet.
    .. param:: psa_key_derivation_step_t step
        Which step the input data is for.
    .. param:: uint64_t value
        The value of the numeric input.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``step`` is not compatible with the operation's algorithm.
        *   ``step`` does not permit numerical inputs.
        *   ``value`` is not valid for ``step`` in the operation's algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``step`` is not supported with the operation's algorithm.
        *   ``value`` is not supported for ``step`` in the operation's algorithm.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid for this input ``step``. This can happen if the application provides a step out of order or repeats a step that may not be repeated.
        *   The library requires initializing. See :secref:`library-init`.

    Which inputs are required and in what order depends on the algorithm.
    However, when an algorithm requires a particular order, numeric inputs usually come first as they tend to be configuration parameters.
    Refer to the documentation of each key-derivation or key-agreement algorithm for information.

    This function is used for inputs which are fixed-size non-negative integers.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

.. function:: psa_key_derivation_input_key

    .. summary::
        Provide an input for key derivation in the form of a key.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to use. It must have been set up with `psa_key_derivation_setup()` and must not have produced any output yet.
    .. param:: psa_key_derivation_step_t step
        Which step the input data is for.
    .. param:: psa_key_id_t key
        Identifier of the key. The key must have an appropriate type for ``step``, it must permit the usage `PSA_KEY_USAGE_DERIVE` or `PSA_KEY_USAGE_VERIFY_DERIVATION` (see note_), and it must permit the algorithm used by the operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   The key has neither the `PSA_KEY_USAGE_DERIVE` nor the `PSA_KEY_USAGE_VERIFY_DERIVATION` usage flag.
        *   The key does not permit the operation's algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``step`` is not compatible with the operation's algorithm.
        *   ``step`` does not permit key inputs of the given type, or does not permit key inputs at all.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``step`` is not supported with the operation's algorithm.
        *   Key inputs of the given type are not supported for ``step`` in the operation's algorithm.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid for this input ``step``. This can happen if the application provides a step out of order or repeats a step that may not be repeated.
        *   The library requires initializing. See :secref:`library-init`.

    Which inputs are required and in what order depends on the algorithm. Refer to the documentation of each key-derivation or key-agreement algorithm for information.

    This function obtains input from a key object, which is usually correct for secret inputs or for non-secret personalization strings kept in the key store. To pass a non-secret parameter which is not in the key store, call `psa_key_derivation_input_bytes()` instead of this function. Refer to the documentation of individual step types (``PSA_KEY_DERIVATION_INPUT_xxx`` values of type `psa_key_derivation_step_t`) for more information.

    .. _note:

    .. note::
        Once all inputs steps are completed, the following operations are permitted:

        *   `psa_key_derivation_output_bytes()` --- if each input was either a direct input, or a key with usage flag `PSA_KEY_USAGE_DERIVE`.
        *   `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()` --- if the input for step `PSA_KEY_DERIVATION_INPUT_SECRET` or `PSA_KEY_DERIVATION_INPUT_PASSWORD` was a key with usage flag `PSA_KEY_USAGE_DERIVE`, and every other input was either a direct input or a key with usage flag `PSA_KEY_USAGE_DERIVE`.
        *   `psa_key_derivation_verify_bytes()`
        *   `psa_key_derivation_verify_key()`

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

.. function:: psa_key_derivation_output_bytes

    .. summary::
        Read some data from a key-derivation operation.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to read from.
    .. param:: uint8_t * output
        Buffer where the output will be written.
    .. param:: size_t output_length
        Number of bytes to output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``output_length`` bytes of ``output`` contain the derived data.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        One of the inputs was a key whose policy did not permit `PSA_KEY_USAGE_DERIVE`.
    .. retval:: PSA_ERROR_INSUFFICIENT_DATA
        The operation's capacity was less than ``output_length`` bytes. In this case, the following occurs:

        *   No output is written to the output buffer.
        *   The operation's capacity is set to zero.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with all required input steps complete.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function calculates output bytes from a key-derivation algorithm and returns those bytes. If the key derivation's output is viewed as a stream of bytes, this function consumes the requested number of bytes from the stream and returns them to the caller. The operation's capacity decreases by the number of bytes read.

    A request to extract more data than the remaining capacity --- :code:`output_length > psa_key_derivation_get_capacity()` --- fails with :code:`PSA_ERROR_INSUFFICIENT_DATA`, and sets the remaining capacity to zero.

    If the operation's capacity is zero, and ``output_length`` is zero, then it is :scterm:`implementation defined` whether this function returns :code:`PSA_SUCCESS` or :code:`PSA_ERROR_INSUFFICIENT_DATA`.

    If this function returns an error status other than :code:`PSA_ERROR_INSUFFICIENT_DATA`, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

.. function:: psa_key_derivation_output_key

    .. summary::
        Derive a key from an ongoing key-derivation operation.

    .. param:: const psa_key_attributes_t * attributes
        The attributes for the new key.

        The following attributes are required for all keys:

        *   The key type. It must not be an asymmetric public key.
        *   The key size. It must be a valid size for the key type.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.

            If the key type to be created is `PSA_KEY_TYPE_PASSWORD_HASH`, then the permitted-algorithm policy must be either the same as the current operation's algorithm, or `PSA_ALG_NONE`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default `PSA_KEY_LIFETIME_VOLATILE` lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to read from.
    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key.
        For persistent keys, this is the key identifier defined in ``attributes``.
        `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_INSUFFICIENT_DATA
        There was not enough data to create the desired key. In this case, the following occurs:

        *   No key is generated.
        *   The operation's capacity is set to zero.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The key attributes, as a whole, are not supported, either by the implementation in general or in the specified storage location.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The key type is invalid, or is an asymmetric public-key type.
        *   The key type is `PSA_KEY_TYPE_PASSWORD_HASH`, and the permitted-algorithm policy is not the same as the current operation's algorithm.
        *   The key size is not valid for the key type. Implementations must reject an attempt to derive a key of size ``0``.
        *   The key lifetime is invalid.
        *   The key identifier is not valid for the key lifetime.
        *   The key usage flags include invalid values.
        *   The key's permitted-usage algorithm is invalid.
        *   The key attributes, as a whole, are invalid.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   A `PSA_KEY_DERIVATION_INPUT_SECRET` or `PSA_KEY_DERIVATION_INPUT_PASSWORD` input step was neither provided through a key, nor the result of a key agreement.
        *   One of the inputs was a key whose policy did not permit `PSA_KEY_USAGE_DERIVE`.
        *   The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with all required input steps complete.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function calculates output bytes from a key-derivation algorithm and uses those bytes to generate a key deterministically. The key's location, policy, type and size are taken from ``attributes``.

    If the key derivation's output is viewed as a stream of bytes, this function consumes the required number of bytes from the stream. The operation's capacity decreases by the number of bytes used to derive the key.

    A request that needs to extract more data than the remaining capacity fails with :code:`PSA_ERROR_INSUFFICIENT_DATA`, and sets the remaining capacity to zero.

    If this function returns an error status other than :code:`PSA_ERROR_INSUFFICIENT_DATA`, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

    How much output is produced and consumed from the operation, and how the key is derived, depends on the key type. The key-derivation procedures for standard key-derivation algorithms are described in the *Key derivation* section of each key definition in :secref:`key-types`. Implementations can use other methods for implementation-specific algorithms.

    .. rationale::

        Permitting implementation defined methods for algorithms not specified in the |API| permits implementations to use other appropriate procedures in cases where interoperability with other implementations is not required.

    For algorithms that take a `PSA_KEY_DERIVATION_INPUT_SECRET` or `PSA_KEY_DERIVATION_INPUT_PASSWORD` input step, the input to that step must be provided with `psa_key_derivation_input_key()`. Future versions of this specification might include additional restrictions on the derived key based on the attributes and strength of the secret key.

     .. note::

        This function is equivalent to calling `psa_key_derivation_output_key_custom()` with the production parameters `PSA_CUSTOM_KEY_PARAMETERS_INIT` and ``custom_data_length == 0`` (``custom_data`` is ignored).

.. function:: psa_key_derivation_output_key_custom

    .. summary:: Derive a key from an ongoing key-derivation operation with custom production parameters.

        .. versionadded:: 1.3

    .. param:: const psa_key_attributes_t * attributes
        The attributes for the new key.

        The following attributes are required for all keys:

        *   The key type. It must not be an asymmetric public key.
        *   The key size. It must be a valid size for the key type.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.

            If the key type to be created is `PSA_KEY_TYPE_PASSWORD_HASH`, then the permitted-algorithm policy must be either the same as the current operation's algorithm, or `PSA_ALG_NONE`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default `PSA_KEY_LIFETIME_VOLATILE` lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to read from.
    .. param:: const psa_custom_key_parameters_t * custom
        Customized production parameters for the key derivation.

        When this is `PSA_CUSTOM_KEY_PARAMETERS_INIT` with ``custom_data_length == 0``,
        this function is equivalent to `psa_key_derivation_output_key()`.
    .. param:: const uint8_t * custom_data
        A buffer containing additional variable-sized production parameters.
    .. param:: size_t custom_data_length
        Length of ``custom_data`` in bytes.
    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key.
        For persistent keys, this is the key identifier defined in ``attributes``.
        `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_INSUFFICIENT_DATA
        There was not enough data to create the desired key. In this case, the following occurs:

        *   No key is generated.
        *   The operation's capacity is set to zero.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   The key attributes, as a whole, are not supported, either by the implementation in general or in the specified storage location.
        *   The production parameters are not supported by the implementation.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The key type is invalid, or is an asymmetric public-key type.
        *   The key type is `PSA_KEY_TYPE_PASSWORD_HASH`, and the permitted-algorithm policy is not the same as the current operation's algorithm.
        *   The key size is not valid for the key type. Implementations must reject an attempt to derive a key of size ``0``.
        *   The key lifetime is invalid.
        *   The key identifier is not valid for the key lifetime.
        *   The key usage flags include invalid values.
        *   The key's permitted-usage algorithm is invalid.
        *   The key attributes, as a whole, are invalid.
        *   The production parameters are invalid.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The following conditions can result in this error:

        *   A `PSA_KEY_DERIVATION_INPUT_SECRET` or `PSA_KEY_DERIVATION_INPUT_PASSWORD` input step was neither provided through a key, nor the result of a key agreement.
        *   One of the inputs was a key whose policy did not permit `PSA_KEY_USAGE_DERIVE`.
        *   The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with all required input steps complete.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    This function calculates output bytes from a key-derivation algorithm and uses those bytes to generate a key deterministically. The key's location, policy, type and size are taken from ``attributes``.

    This function operates in a similar way to `psa_key_derivation_output_key()`, but enables explicit production parameters to be provided when deriving a key.
    For example, the production parameters can be used to select an alternative key-derivation process, or configure additional key parameters.
    See `psa_key_derivation_output_key()` for the operation of this function with the default production parameters.

    See `psa_custom_key_parameters_t` for a list of non-default production parameters. See the key type definitions in :secref:`key-types` for details of the custom production parameters used for key derivation.

.. function:: psa_key_derivation_verify_bytes

    .. summary::
        Compare output data from a key-derivation operation to an expected value.

        .. versionadded:: 1.1

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to read from.
    .. param:: const uint8_t * expected_output
        Buffer containing the expected derivation output.
    .. param:: size_t output_length
        Length of the expected output. This is also the number of bytes that will be read.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The output of the key-derivation operation matches ``expected_output``.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The output of the key-derivation operation does not match the value in ``expected_output``.
    .. retval:: PSA_ERROR_INSUFFICIENT_DATA
        The operation's capacity was less than ``output_length`` bytes. In this case, the operation's capacity is set to zero.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with all required input steps complete.
        *   The library requires initializing. See :secref:`library-init`.

    This function calculates output bytes from a key-derivation algorithm and compares those bytes to an expected value.
    If the key derivation's output is viewed as a stream of bytes, this function destructively reads ``output_length`` bytes from the stream before comparing them with ``expected_output``.
    The operation's capacity decreases by the number of bytes read.

    A request to extract more data than the remaining capacity --- :code:`output_length > psa_key_derivation_get_capacity()` --- fails with :code:`PSA_ERROR_INSUFFICIENT_DATA`, and sets the remaining capacity to zero.

    If the operation's capacity is zero, and ``output_length`` is zero, then it is :scterm:`implementation defined` whether this function returns :code:`PSA_SUCCESS` or :code:`PSA_ERROR_INSUFFICIENT_DATA`.

    If this function returns an error status other than :code:`PSA_ERROR_INSUFFICIENT_DATA`, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

    .. note::

        A call to `psa_key_derivation_verify_bytes()` is functionally equivalent to the following code:

        .. code-block:: xref

            uint8_t tmp[output_length];
            psa_key_derivation_output_bytes(operation, tmp, output_length);
            if (memcmp(expected_output, tmp, output_length) != 0)
                return PSA_ERROR_INVALID_SIGNATURE;

        However, calling `psa_key_derivation_verify_bytes()` works even if an input key's policy does not include `PSA_KEY_USAGE_DERIVE`.

    .. admonition:: Implementation note

        Implementations must make the best effort to ensure that the comparison between the actual key-derivation output and the expected output is performed in constant time.

.. function:: psa_key_derivation_verify_key

    .. summary::
        Compare output data from a key-derivation operation to an expected value stored in a key.

        .. versionadded:: 1.1

    .. param:: psa_key_derivation_operation_t * operation
        The key-derivation operation object to read from.
    .. param:: psa_key_id_t expected
        A key of type `PSA_KEY_TYPE_PASSWORD_HASH` containing the expected output.
        The key must permit the usage `PSA_KEY_USAGE_VERIFY_DERIVATION`, and the permitted algorithm must match the operation's algorithm.

        The value of this key is typically computed by a previous call to `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()`.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The output of the key-derivation operation matches the ``expected`` key value.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``expected`` is not a valid key identifier.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The key type is not `PSA_KEY_TYPE_PASSWORD_HASH`.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The ``expected`` key does not have the `PSA_KEY_USAGE_VERIFY_DERIVATION` flag, or it does not permit the requested algorithm.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The output of the key-derivation operation does not match the value of the ``expected`` key.
    .. retval:: PSA_ERROR_INSUFFICIENT_DATA
        The operation's capacity was less than the length of the ``expected`` key. In this case, the operation's capacity is set to zero.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, with all required input steps complete.
        *   The library requires initializing. See :secref:`library-init`.

    This function calculates output bytes from a key-derivation algorithm and compares those bytes to an expected value, provided as key of type `PSA_KEY_TYPE_PASSWORD_HASH`.
    If the key derivation's output is viewed as a stream of bytes, this function destructively reads the number of bytes corresponding to the length of the ``expected`` key from the stream before comparing them with the key value.
    The operation's capacity decreases by the number of bytes read.

    A request that needs to extract more data than the remaining capacity fails with :code:`PSA_ERROR_INSUFFICIENT_DATA`, and sets the remaining capacity to zero.

    If this function returns an error status other than :code:`PSA_ERROR_INSUFFICIENT_DATA`, the operation enters an error state and must be aborted by calling `psa_key_derivation_abort()`.

    .. note::

        A call to `psa_key_derivation_verify_key()` is functionally equivalent to exporting the ``expected`` key and calling `psa_key_derivation_verify_bytes()` on the result, except that it works when the key cannot be exported.

    .. admonition:: Implementation note

        Implementations must make the best effort to ensure that the comparison between the actual key-derivation output and the expected output is performed in constant time.

.. function:: psa_key_derivation_abort

    .. summary::
        Abort a key-derivation operation.

    .. param:: psa_key_derivation_operation_t * operation
        The operation to abort.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself. Once aborted, the operation object can be reused for another operation by calling `psa_key_derivation_setup()` again.

    This function can be called at any time after the operation object has been initialized as described in `psa_key_derivation_operation_t`.

    In particular, it is valid to call `psa_key_derivation_abort()` twice, or to call `psa_key_derivation_abort()` on an operation that has not been set up.

Support macros
--------------

.. macro:: PSA_ALG_IS_KEY_DERIVATION_STRETCHING
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a key-stretching or password-hashing algorithm.

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a key-stretching or password-hashing algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm algorithm identifier.

    A key-stretching or password-hashing algorithm is a key-derivation algorithm that is suitable for use with a low-entropy secret such as a password.
    Equivalently, it's a key-derivation algorithm that uses a `PSA_KEY_DERIVATION_INPUT_PASSWORD` input step.

.. macro:: PSA_ALG_IS_HKDF
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an HKDF algorithm (:code:`PSA_ALG_HKDF(hash_alg)`).

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an HKDF algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

    HKDF is a family of key-derivation algorithms that are based on a hash function and the HMAC construction.

.. macro:: PSA_ALG_IS_HKDF_EXTRACT
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an HKDF-Extract algorithm (:code:`PSA_ALG_HKDF_EXTRACT(hash_alg)`).

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an HKDF-Extract algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_HKDF_EXPAND
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is an HKDF-Expand algorithm (:code:`PSA_ALG_HKDF_EXPAND(hash_alg)`).

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is an HKDF-Expand algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_SP800_108_COUNTER_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a key-derivation algorithm constructed using :code:`PSA_ALG_SP800_108_COUNTER_HMAC(hash_alg)`.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a key-derivation algorithm constructed using :code:`PSA_ALG_SP800_108_COUNTER_HMAC()`, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_TLS12_PRF
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a TLS-1.2 PRF algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a TLS-1.2 PRF algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_TLS12_PSK_TO_MS
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a TLS-1.2 PSK to MS algorithm.

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a TLS-1.2 PSK to MS algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_PBKDF2_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a PBKDF2-HMAC algorithm.

        .. versionadded:: 1.1

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a PBKDF2-HMAC algorithm, ``0`` otherwise. This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_ALG_IS_WPA3_SAE_H2E
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a WPA3-SAE hash-to-element key-derivation algorithm

        .. versionadded:: 1.4

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a WPA3-SAE hash-to-element algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported key-derivation algorithm identifier.

.. macro:: PSA_KEY_DERIVATION_UNLIMITED_CAPACITY
    :definition: /* implementation-defined value */

    .. summary::
        Use the maximum possible capacity for a key-derivation operation.

    Use this value as the capacity argument when setting up a key derivation to specify that the operation will use the maximum possible capacity. The value of the maximum possible capacity depends on the key-derivation algorithm.

.. macro:: PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns the maximum supported length of the PSK for the TLS-1.2 PSK-to-MS key derivation.

    This implementation-defined value specifies the maximum length for the PSK input used with a `PSA_ALG_TLS12_PSK_TO_MS()` key-agreement algorithm.

    Quoting :RFC-title:`4279#5.3`:

        TLS implementations supporting these cipher suites MUST support arbitrary PSK identities up to 128 octets in length, and arbitrary PSKs up to 64 octets in length. Supporting longer identities and keys is RECOMMENDED.

    Therefore, it is recommended that implementations define `PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE` with a value greater than or equal to ``64``.

.. macro:: PSA_TLS12_ECJPAKE_TO_PMS_OUTPUT_SIZE
    :definition: 32

    .. summary::
        The size of the output from the TLS 1.2 ECJPAKE-to-PMS key-derivation algorithm, in bytes.

        .. versionadded:: 1.2

    This value can be used when extracting the result of a key-derivation operation that was set up with the `PSA_ALG_TLS12_ECJPAKE_TO_PMS` algorithm.
