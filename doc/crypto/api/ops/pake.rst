.. SPDX-FileCopyrightText: Copyright 2022-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 300

.. _pake:

Password-authenticated key exchange (PAKE)
==========================================

PAKE protocols provide an interactive method for two or more parties to establish cryptographic keys based on knowledge of a low entropy secret, such as a password.

These can provide strong security for communication from a weak password, because the password is not directly communicated as part of the key exchange.

This chapter is divided into the following sections:

*   :secref:`pake-common-api` --- the common interface elements, including the PAKE operation.
*   :secref:`pake-jpake` --- the J-PAKE protocol, and the associated interface elements.
*   :secref:`pake-spake2p` --- the SPAKE2+ protocols, and the associated interface elements.
*   :secref:`pake-wpa3-sae` --- the WPA3-SAE protocol, and the associated interface elements.

.. _pake-common-api:

Common API for PAKE
-------------------

This section defines all of the common interfaces used to carry out a PAKE protocol:

*   :secref:`pake-primitive`
*   :secref:`pake-cipher-suite`
*   :secref:`pake-roles`
*   :secref:`pake-steps`
*   :secref:`pake-operation`
*   :secref:`pake-support`

.. _pake-primitive:

PAKE primitives
---------------

A PAKE algorithm specifies a sequence of interactions between the participants.
Many PAKE algorithms are designed to allow different cryptographic primitives to be used for the key establishment operation, so long as all the participants are using the same underlying cryptography.

The cryptographic primitive for a PAKE operation is specified using a `psa_pake_primitive_t` value, which can be constructed using the `PSA_PAKE_PRIMITIVE()` macro, or can be provided as a numerical constant value.

A PAKE primitive is required when constructing a PAKE cipher-suite object, `psa_pake_cipher_suite_t`, which fully specifies the PAKE operation to be carried out.

.. typedef:: uint32_t psa_pake_primitive_t

    .. summary::
        Encoding of the primitive associated with the PAKE.

        .. versionadded:: 1.1

    PAKE primitive values are constructed using `PSA_PAKE_PRIMITIVE()`.

    :numref:`fig-pake-primitive` shows how the components of the primitive are encoded into a `psa_pake_primitive_t` value.

    .. figure:: /figure/pake/pake_primitive.*
        :name: fig-pake-primitive

        PAKE primitive encoding

    .. rationale::

        An integral type is required for `psa_pake_primitive_t` to enable values of this type to be compile-time-constants.
        This allows them to be used in ``case`` statements, and used to calculate static buffer sizes with `PSA_PAKE_OUTPUT_SIZE()` and `PSA_PAKE_INPUT_SIZE()`.

    The components of a PAKE primitive value can be extracted using the `PSA_PAKE_PRIMITIVE_GET_TYPE()`, `PSA_PAKE_PRIMITIVE_GET_FAMILY()`, and `PSA_PAKE_PRIMITIVE_GET_BITS()`.
    These can be used to set key attributes for keys used in PAKE algorithms.
    :secref:`spake2p-registration` provides an example of this usage.

.. typedef:: uint8_t psa_pake_primitive_type_t

    .. summary::
        Encoding of the type of the PAKE's primitive.

        .. versionadded:: 1.1

    The range of PAKE primitive type values is divided as follows:

    :code:`0x00`
        Reserved as an invalid primitive type.
    :code:`0x01 - 0x7f`
        Specification-defined primitive type.
        Primitive types defined by this standard always have bit 7 clear.
        Unallocated primitive type values in this range are reserved for future use.
    :code:`0x80 - 0xff`
        Implementation-defined primitive type.
        Implementations that define additional primitive types must use an encoding with bit 7 set.

    For specification-defined primitive types, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.

.. macro:: PSA_PAKE_PRIMITIVE_TYPE_ECC
    :definition: ((psa_pake_primitive_type_t)0x01)

    .. summary::
        The PAKE primitive type indicating the use of elliptic curves.

        .. versionadded:: 1.1

    The values of the ``family`` and ``bits`` components of the PAKE primitive identify a specific elliptic curve, using the same mapping that is used for ECC keys.
    See the definition of ``psa_ecc_family_t``.
    Here ``family`` and ``bits`` refer to the values used to construct the PAKE primitive using `PSA_PAKE_PRIMITIVE()`.

    Input and output during the operation can involve group elements and scalar values:

    *   The format for group elements is the same as that for public keys on the specific elliptic curve.
        See *Key format* within the definition of `PSA_KEY_TYPE_ECC_PUBLIC_KEY()`.
    *   The format for scalars is the same as that for private keys on the specific elliptic curve.
        See *Key format* within the definition of `PSA_KEY_TYPE_ECC_KEY_PAIR()`.

.. macro:: PSA_PAKE_PRIMITIVE_TYPE_DH
    :definition: ((psa_pake_primitive_type_t)0x02)

    .. summary::
        The PAKE primitive type indicating the use of a finite field Diffie-Hellman group.

        .. versionadded:: 1.1

    The values of the ``family`` and ``bits`` components of the PAKE primitive identify a specific finite field Diffie-Hellman group, using the same mapping that is used for finite field Diffie-Hellman keys.
    See the definition of ``psa_dh_family_t``.
    Here ``family`` and ``bits`` refer to the values used to construct the PAKE primitive using `PSA_PAKE_PRIMITIVE()`.

    Input and output during the operation can involve group elements and scalar values:

    *   The format for group elements is the same as that for public keys in the specific finite field Diffie-Hellman group.
        See *Key format* within the definition of `PSA_KEY_TYPE_DH_PUBLIC_KEY()`.
    *   The format for scalars is the same as that for private keys in the specific finite field Diffie-Hellman group.
        See *Key format* within the definition of `PSA_KEY_TYPE_DH_PUBLIC_KEY()`.


.. typedef:: uint8_t psa_pake_family_t

    .. summary::
        Encoding of the family of the primitive associated with the PAKE.

        .. versionadded:: 1.1

    For more information on the family values, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.

.. macro:: PSA_PAKE_PRIMITIVE
    :definition: /* specification-defined value */

    .. summary::
        Construct a PAKE primitive from type, family and bit-size.

        .. versionadded:: 1.1

    .. param:: pake_type
        The type of the primitive: a value of type `psa_pake_primitive_type_t`.
    .. param:: pake_family
        The family of the primitive.
        The type and interpretation of this parameter depends on ``pake_type``.
        For more information, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.
    .. param:: pake_bits
        The bit-size of the primitive: a value of type ``size_t``.
        The interpretation of this parameter depends on ``pake_type`` and ``family``.
        For more information, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.

    .. return:: psa_pake_primitive_t
        The constructed primitive value.
        Return ``0`` if the requested primitive can't be encoded as `psa_pake_primitive_t`.

    A PAKE primitive value is used to specify a PAKE operation, as part of a PAKE cipher suite.

.. macro:: PSA_PAKE_PRIMITIVE_GET_TYPE
    :definition: /* specification-defined value */

    .. summary::
        Extract the PAKE primitive type from a PAKE primitive.

        .. versionadded:: 1.2

    .. param:: pake_primitive
        A PAKE primitive: a value of type `psa_pake_primitive_t`.

    .. return:: psa_pake_primitive_type_t
        The PAKE primitive type, if ``pake_primitive`` is a supported PAKE primitive.
        Unspecified if ``pake_primitive`` is not a supported PAKE primitive.

.. macro:: PSA_PAKE_PRIMITIVE_GET_FAMILY
    :definition: /* specification-defined value */

    .. summary::
        Extract the family from a PAKE primitive.

        .. versionadded:: 1.2

    .. param:: pake_primitive
        A PAKE primitive: a value of type `psa_pake_primitive_t`.

    .. return:: psa_pake_family_t
        The PAKE primitive family, if ``pake_primitive`` is a supported PAKE primitive.
        Unspecified if ``pake_primitive`` is not a supported PAKE primitive.

    For more information on the family values, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.

.. macro:: PSA_PAKE_PRIMITIVE_GET_BITS
    :definition: /* specification-defined value */

    .. summary::
        Extract the bit-size from a PAKE primitive.

        .. versionadded:: 1.2

    .. param:: pake_primitive
        A PAKE primitive: a value of type `psa_pake_primitive_t`.

    .. return:: size_t
        The PAKE primitive bit-size, if ``pake_primitive`` is a supported PAKE primitive.
        Unspecified if ``pake_primitive`` is not a supported PAKE primitive.

    For more information on the bit-size values, see `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH`.

.. _pake-cipher-suite:

PAKE cipher suites
------------------

Most PAKE algorithms have parameters that must be specified by the application.
These parameters include the following:

*   The cryptographic primitive used for key establishment, specified using a `PAKE primitive <pake-primitive>`.
*   A cryptographic hash algorithm.
*   Whether the application requires the shared secret before, or after, it is confirmed.

The hash algorithm is encoded into the PAKE algorithm identifier. The `psa_pake_cipher_suite_t` object is used to fully specify a PAKE operation, combining the PAKE protocol with all of the above parameters.

A PAKE cipher suite is required when setting up a PAKE operation in `psa_pake_setup()`.


.. typedef:: /* implementation-defined type */ psa_pake_cipher_suite_t

    .. summary::
        The type of an object describing a PAKE cipher suite.

        .. versionadded:: 1.1

    This is the object that represents the cipher suite used for a PAKE algorithm.
    The PAKE cipher suite specifies the PAKE algorithm, and the options selected for that algorithm.
    The cipher suite includes the following attributes:

    *   The PAKE algorithm itself.
    *   The hash algorithm, encoded within the PAKE algorithm.
    *   The PAKE primitive, which identifies the prime order group used for the key exchange operation.
        See :secref:`pake-primitive`.
    *   Whether to confirm the shared secret.

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

    Before calling any function on a PAKE cipher suite object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_pake_cipher_suite_t cipher_suite;
            memset(&cipher_suite, 0, sizeof(cipher_suite));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_pake_cipher_suite_t cipher_suite;

    *   Initialize the object to the initializer `PSA_PAKE_CIPHER_SUITE_INIT`, for example:

        .. code-block:: xref

            psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;

    *   Assign the result of the function `psa_pake_cipher_suite_init()` to the object, for example:

        .. code-block:: xref

            psa_pake_cipher_suite_t cipher_suite;
            cipher_suite = psa_pake_cipher_suite_init();

    Following initialization, the cipher-suite object contains the following values:

    .. list-table::
        :header-rows: 1
        :widths: 1 4
        :align: left

        *   -   Attribute
            -   Value

        *   -   algorithm
            -   `PSA_ALG_NONE` --- an invalid algorithm identifier.
        *   -   primitive
            -   ``0`` --- an invalid PAKE primitive.
        *   -   key confirmation
            -   `PSA_PAKE_CONFIRMED_KEY` --- requesting that the secret key is confirmed before it can be returned.

    Valid algorithm, primitive, and key confirmation values must be set when using a PAKE cipher suite.

    .. admonition:: Implementation note

        Implementations are recommended to define the cipher-suite object as a simple data structure, with fields corresponding to the individual cipher suite attributes.
        In such an implementation, each function ``psa_pake_cs_set_xxx()`` sets a field and the corresponding function ``psa_pake_cs_get_xxx()`` retrieves the value of the field.

        An implementation can report attribute values that are equivalent to the original one, but have a different encoding.
        For example, an implementation can use a more compact representation for attributes where many bit-patterns are invalid or not supported, and store all values that it does not support as a special marker value.
        In such an implementation, after setting an invalid value, the corresponding get function returns an invalid value which might not be the one that was originally stored.

.. macro:: PSA_PAKE_CIPHER_SUITE_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a PAKE cipher suite object of type `psa_pake_cipher_suite_t`.

        .. versionadded:: 1.1

.. function:: psa_pake_cipher_suite_init

    .. summary::
        Return an initial value for a PAKE cipher suite object.

        .. versionadded:: 1.1

    .. return:: psa_pake_cipher_suite_t

.. function:: psa_pake_cs_get_algorithm

    .. summary::
        Retrieve the PAKE algorithm from a PAKE cipher suite.

        .. versionadded:: 1.1

    .. param:: const psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to query.

    .. return:: psa_algorithm_t
        The PAKE algorithm stored in the cipher suite object.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. function:: psa_pake_cs_set_algorithm

    .. summary::
        Declare the PAKE algorithm for the cipher suite.

        .. versionadded:: 1.1

    .. param:: psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to write to.
    .. param:: psa_algorithm_t alg
        The PAKE algorithm to write: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_PAKE(alg)` is true.

    .. return:: void

    This function overwrites any PAKE algorithm previously set in ``cipher_suite``.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. function:: psa_pake_cs_get_primitive

    .. summary::
        Retrieve the primitive from a PAKE cipher suite.

        .. versionadded:: 1.1

    .. param:: const psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to query.

    .. return:: psa_pake_primitive_t
        The primitive stored in the cipher suite object.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. function:: psa_pake_cs_set_primitive

    .. summary::
        Declare the primitive for a PAKE cipher suite.

        .. versionadded:: 1.1

    .. param:: psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to write to.
    .. param:: psa_pake_primitive_t primitive
        The PAKE primitive to write: a value of type `psa_pake_primitive_t`.
        If this is ``0``, the primitive type in ``cipher_suite`` becomes unspecified.

    .. return:: void

    This function overwrites any primitive previously set in ``cipher_suite``.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. macro:: PSA_PAKE_CONFIRMED_KEY
    :definition: 0

    .. summary::
        A key confirmation value that indicates an confirmed key in a PAKE cipher suite.

        .. versionadded:: 1.2

    This key confirmation value will result in the PAKE algorithm exchanging data to verify that the shared key is identical for both parties.
    This is the default key confirmation value in an initialized PAKE cipher suite object.

    Some algorithms do not include confirmation of the shared key.

.. macro:: PSA_PAKE_UNCONFIRMED_KEY
    :definition: 1

    .. summary::
        A key confirmation value that indicates an unconfirmed key in a PAKE cipher suite.

        .. versionadded:: 1.2

    This key confirmation value will result in the PAKE algorithm terminating prior to confirming that the resulting shared key is identical for both parties.

    Some algorithms do not support returning an unconfirmed shared key.

    .. warning::

        When the shared key is not confirmed as part of the PAKE operation, the application is responsible for mitigating risks that arise from the possible mismatch in the output keys.

.. function:: psa_pake_cs_get_key_confirmation

    .. summary::
        Retrieve the key confirmation from a PAKE cipher suite.

        .. versionadded:: 1.2

    .. param:: const psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to query.

    .. return:: uint32_t
        A key confirmation value: either `PSA_PAKE_CONFIRMED_KEY` or `PSA_PAKE_UNCONFIRMED_KEY`.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. function:: psa_pake_cs_set_key_confirmation

    .. summary::
        Declare the key confirmation from a PAKE cipher suite.

        .. versionadded:: 1.2

    .. param:: psa_pake_cipher_suite_t* cipher_suite
        The cipher suite object to write to.
    .. param:: uint32_t key_confirmation
        The key confirmation value to write: either `PSA_PAKE_CONFIRMED_KEY` or `PSA_PAKE_UNCONFIRMED_KEY`.

    .. return:: void

    This function overwrites any key confirmation previously set in ``cipher_suite``.

    The documentation of individual PAKE algorithms specifies which key confirmation values are valid for the algorithm.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs.
        It can be efficiently implemented as a ``static inline`` function or a function-like macro.

.. _pake-roles:

PAKE roles
----------

Some PAKE algorithms need to know which role each participant is taking in the algorithm.
For example:

*   Augmented PAKE algorithms typically have a client and a server participant.
*   Some symmetric PAKE algorithms assign an order to the two participants.

.. typedef:: uint8_t psa_pake_role_t

    .. summary::
        Encoding of the application role in a PAKE algorithm.

        .. versionadded:: 1.1

    This type is used to encode the application's role in the algorithm being executed.
    For more information see the documentation of individual PAKE role constants.

.. macro:: PSA_PAKE_ROLE_NONE
    :definition: ((psa_pake_role_t)0x00)

    .. summary::
        A value to indicate no role in a PAKE algorithm.

        .. versionadded:: 1.1

    This value can be used in a call to `psa_pake_set_role()` for symmetric PAKE algorithms which do not assign roles.

.. macro:: PSA_PAKE_ROLE_FIRST
    :definition: ((psa_pake_role_t)0x01)

    .. summary::
        The first peer in a balanced PAKE.

        .. versionadded:: 1.1

    Although balanced PAKE algorithms are symmetric, some of them need the peers to be ordered for the transcript calculations.
    If the algorithm does not need a specific ordering, then either do not call `psa_pake_set_role()`, or use `PSA_PAKE_ROLE_NONE` as the role parameter.

.. macro:: PSA_PAKE_ROLE_SECOND
    :definition: ((psa_pake_role_t)0x02)

    .. summary::
        The second peer in a balanced PAKE.

        .. versionadded:: 1.1

    Although balanced PAKE algorithms are symmetric, some of them need the peers to be ordered for the transcript calculations.
    If the algorithm does not need a specific ordering, then either do not call `psa_pake_set_role()`, or use `PSA_PAKE_ROLE_NONE` as the role parameter.

.. macro:: PSA_PAKE_ROLE_CLIENT
    :definition: ((psa_pake_role_t)0x11)

    .. summary::
        The client in an augmented PAKE.

        .. versionadded:: 1.1

    Augmented PAKE algorithms need to differentiate between client and server.

.. macro:: PSA_PAKE_ROLE_SERVER
    :definition: ((psa_pake_role_t)0x12)

    .. summary::
        The server in an augmented PAKE.

        .. versionadded:: 1.1

    Augmented PAKE algorithms need to differentiate between client and server.


.. _pake-steps:

PAKE step types
---------------

.. typedef:: uint8_t psa_pake_step_t

    .. summary::
        Encoding of input and output steps for a PAKE algorithm.

        .. versionadded:: 1.1

    Some PAKE algorithms need to exchange more data than a single key share.
    This type encodes additional input and output steps for such algorithms.

.. macro:: PSA_PAKE_STEP_KEY_SHARE
    :definition: ((psa_pake_step_t)0x01)

    .. summary::
        A key share being sent to or received from a PAKE participant.

        .. versionadded:: 1.1

    The format for both input and output using this step is the same as the format for public keys on the group specified by the PAKE operation's primitive.

    The public-key formats are defined in the documentation for `psa_export_public_key()`.

    For information regarding how the group is determined, consult the documentation `PSA_PAKE_PRIMITIVE()`.

.. macro:: PSA_PAKE_STEP_ZK_PUBLIC
    :definition: ((psa_pake_step_t)0x02)

    .. summary::
        A Schnorr NIZKP public key being sent to or received from a PAKE participant.

        .. versionadded:: 1.1

    This is the ephemeral public key in the Schnorr Non-Interactive Zero-Knowledge Proof, this is the value denoted by *V* in :RFC:`8235`.

    The format for both input and output at this step is the same as that for public keys on the group specified by the PAKE operation's primitive.

    For more information on the format, consult the documentation of `psa_export_public_key()`.

    For information regarding how the group is determined, consult the documentation `PSA_PAKE_PRIMITIVE()`.

.. macro:: PSA_PAKE_STEP_ZK_PROOF
    :definition: ((psa_pake_step_t)0x03)

    .. summary::
        A Schnorr NIZKP proof being sent to or received from a PAKE participant.

        .. versionadded:: 1.1

    This is the proof in the Schnorr Non-Interactive Zero-Knowledge Proof, this is the value denoted by *r* in :RFC:`8235`.

    Both for input and output, the value at this step is an integer less than the order of the group specified by the PAKE operation's primitive.
    The format depends on the group as well:

    *   For Montgomery curves, the encoding is little endian.
    *   For other elliptic curves, and for finite field Diffie-Hellman groups, the encoding is big endian. See :cite:`SEC1` §2.3.8.

    In both cases leading zeroes are permitted as long as the length in bytes does not exceed the byte length of the group order.

    For information regarding how the group is determined, consult the documentation `PSA_PAKE_PRIMITIVE()`.

.. macro:: PSA_PAKE_STEP_CONFIRM
    :definition: ((psa_pake_step_t)0x04)

    .. summary::
        A key confirmation value being sent to or received from a PAKE participant.

        .. versionadded:: 1.2

    This value is used during the key confirmation phase of a PAKE protocol.
    The use of this step, and format of the value depends on the algorithm and cipher suite:

    *   For a SPAKE2+ algorithm, the format for both input and output at this step is the same as the output of the MAC algorithm specified in the cipher suite.
        See :secref:`spake2p-operation`.

    *   For a WPA3-SAE algorithm, the format for both input and output at this step is a 2-byte little-endian *send-confirm* counter, followed by the *confirm* value, which is the output from the hash algorithm specified in the cipher suite.
        See :secref:`wpa3-sae-operation`.

.. macro:: PSA_PAKE_STEP_SALT
    :definition: ((psa_pake_step_t)0x05)

    .. summary::
        A salt value used for deriving shared secrets within a PAKE operation.

        .. versionadded:: 1.4

    This input can be used during the key exchange phase of a PAKE protocol.
    The use of this step, and format of the value depends on the algorithm and cipher suite:

    *   For a WPA3-SAE algorithm, a salt value must be provided as defined in `[IEEE-802.11]` §12.4.5.4.
        See :secref:`wpa3-sae-operation`.

.. macro:: PSA_PAKE_STEP_COMMIT
    :definition: ((psa_pake_step_t)0x06)

    .. summary::
        A commitment value being sent to or received from a PAKE participant.

        .. versionadded:: 1.4

    This input and output is used during the key exchange phase of a PAKE protocol.
    The use of this step, and format of the value depends on the algorithm and cipher suite:

    *   For a WPA3-SAE algorithm, the format for input and output at this step is a concatenation of the *commit-scalar* and *COMMIT-ELEMENT* values, as defined in `[IEEE-802.11]` §12.4.7.3.

        See :secref:`wpa3-sae-operation`.

        .. note::

            These values are adjacent in the WPA3-SAE Authentication frame defined in `[IEEE-802.11]` §9.3.3.11.
            The concatenated value can be output directly to, or input directly from, the frame buffer.

.. macro:: PSA_PAKE_STEP_CONFIRM_COUNT
    :definition: ((psa_pake_step_t)0x07)

    .. summary::
        A counter used as part of key confirmation.

        .. versionadded:: 1.4

    This value is input during the key confirmation phase of a PAKE protocol.
    It enables multiple confirmation attempts to result in distinct confirmation values.
    The use of this step, and format of the value depends on the algorithm and cipher suite:

    *   For a WPA3-SAE algorithm, the format for input at this step is the 2-byte little-endian *send-confirm* counter.
        See :secref:`wpa3-sae-operation`.

.. macro:: PSA_PAKE_STEP_KEY_ID
    :definition: ((psa_pake_step_t)0x08)

    .. summary::
        A key identifier value from a PAKE operation.

        .. versionadded:: 1.4

    This value can be output from a PAKE operation following key confirmation.
    The use of this step, and format of the value depends on the algorithm and cipher suite:

    *   For a WPA3-SAE algorithm, the format of the output at this step is the 16-byte PMKID.
        See :secref:`wpa3-sae-operation`.

.. _pake-operation:

Multi-part PAKE operations
--------------------------

.. typedef:: /* implementation-defined type */ psa_pake_operation_t

    .. summary::
        The type of the state object for PAKE operations.

        .. versionadded:: 1.1

    Before calling any function on a PAKE operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_pake_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_pake_operation_t operation;

    *   Initialize the object to the initializer `PSA_PAKE_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_pake_operation_t operation = PSA_PAKE_OPERATION_INIT;

    *   Assign the result of the function `psa_pake_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_pake_operation_t operation;
            operation = psa_pake_operation_init();

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_PAKE_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a PAKE operation object of type `psa_pake_operation_t`.

        .. versionadded:: 1.1

.. function:: psa_pake_operation_init

    .. summary::
        Return an initial value for a PAKE operation object.

        .. versionadded:: 1.1

    .. return:: psa_pake_operation_t

.. function:: psa_pake_setup

    .. summary::
        Setup a password-authenticated key exchange.

        .. versionadded:: 1.1

        .. versionchanged:: 1.2 Added key to the operation setup.

    .. param:: psa_pake_operation_t * operation
        The operation object to set up.
        It must have been initialized as per the documentation for `psa_pake_operation_t` and not yet in use.
    .. param:: psa_key_id_t password_key
        Identifier of the key holding the password or a value derived from the password.
        It must remain valid until the operation terminates.

        The valid key types depend on the PAKE algorithm, and participant role.
        Refer to the documentation of individual PAKE algorithms for more information.

        The key must permit the usage `PSA_KEY_USAGE_DERIVE`.
    .. param:: const psa_pake_cipher_suite_t * cipher_suite
        The cipher suite to use.
        A PAKE cipher suite fully characterizes a PAKE algorithm, including the PAKE algorithm.

        The cipher suite must be compatible with the key type of ``password_key``.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``password_key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        ``password_key`` does not have the `PSA_KEY_USAGE_DERIVE` flag, or it does not permit the algorithm in ``cipher_suite``.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm in ``cipher_suite`` is not a PAKE algorithm, or encodes an invalid hash algorithm.
        *   The PAKE primitive in ``cipher_suite`` is not compatible with the PAKE algorithm.
        *   The key confirmation value in ``cipher_suite`` is not compatible with the PAKE algorithm and primitive.
        *   The key type or key size of ``password_key`` is not compatible with ``cipher_suite``.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   The algorithm in ``cipher_suite`` is not a supported PAKE algorithm, or encodes an unsupported hash algorithm.
        *   The PAKE primitive in ``cipher_suite`` is not supported or not compatible with the PAKE algorithm.
        *   The key confirmation value in ``cipher_suite`` is not supported, or not compatible, with the PAKE algorithm and primitive.
        *   The key type or key size of ``password_key`` is not supported with ``cipher suite``.
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The sequence of operations to set up a password-authenticated key exchange operation is as follows:

    1.  Allocate a PAKE operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_pake_operation_t`.
        For example, using `PSA_PAKE_OPERATION_INIT`.
    #.  Call `psa_pake_setup()` to specify the cipher suite and provide the password or password-derived key.
    #.  Call ``psa_pake_set_xxx()`` functions on the operation to complete the setup.
        The exact sequence of ``psa_pake_set_xxx()`` functions that needs to be called depends on the algorithm in use.

    A typical sequence of calls to perform a password-authenticated key exchange:

    1.  Call :code:`psa_pake_output(operation, PSA_PAKE_STEP_KEY_SHARE, ...)` to get the key share that needs to be sent to the peer.
    #.  Call :code:`psa_pake_input(operation, PSA_PAKE_STEP_KEY_SHARE, ...)` to provide the key share that was received from the peer.
    #.  Depending on the algorithm additional calls to `psa_pake_output()` and `psa_pake_input()` might be necessary.
    #.  Call `psa_pake_get_shared_key()` to access the shared secret.

    Refer to the documentation of individual PAKE algorithms for details on the required set up and operation for each algorithm, and for constraints on the format and content of valid passwords.

    After a successful call to `psa_pake_setup()`, the operation is active, and the application must eventually terminate the operation.
    The following events terminate an operation:

    *   A successful call to `psa_pake_get_shared_key()`.
    *   A call to `psa_pake_abort()`.

    If `psa_pake_setup()` returns an error, the operation object is unchanged.
    If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_pake_abort()`.

    See :secref:`multi-part-operations`.


.. function:: psa_pake_set_role

    .. summary::
        Set the application role for a password-authenticated key exchange.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: psa_pake_role_t role
        A value of type `psa_pake_role_t` indicating the application role in the PAKE algorithm.
        See :secref:`pake-roles`.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and `psa_pake_set_role()`, `psa_pake_input()`, and `psa_pake_output()` must not have been called yet.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``role`` is not a valid PAKE role in the operation's algorithm.
        *   ``role`` is not compatible with the operation's key type.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``role`` is not a valid PAKE role, or is not supported for the operation's algorithm.
        *   ``role`` is not supported with the operation's key type.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    Not all PAKE algorithms need to differentiate the communicating participants.
    For PAKE algorithms that do not require a role to be specified, the application can do either of the following:

    *   Not call `psa_pake_set_role()` on the PAKE operation.
    *   Call `psa_pake_set_role()` with the `PSA_PAKE_ROLE_NONE` role.

    Refer to the documentation of individual PAKE algorithms for more information.

.. function:: psa_pake_set_user

    .. summary::
        Set the user ID for a password-authenticated key exchange.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: const uint8_t * user_id
        The user ID to authenticate with.
    .. param:: size_t user_id_len
        Size of the ``user_id`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and `psa_pake_set_user()`, `psa_pake_input()`, and `psa_pake_output()` must not have been called yet.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``user_id`` is not valid for the operation's algorithm and cipher suite.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The value of ``user_id`` is not supported by the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    Call this function to set the user ID.
    For PAKE algorithms that associate a user identifier with both participants in the session, also call `psa_pake_set_peer()` with the peer ID.
    For PAKE algorithms that associate a single user identifier with the session, call `psa_pake_set_user()` only.

    Refer to the documentation of individual PAKE algorithms for more information.

.. function:: psa_pake_set_peer

    .. summary::
        Set the peer ID for a password-authenticated key exchange.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: const uint8_t * peer_id
        The peer's ID to authenticate.
    .. param:: size_t peer_id_len
        Size of the ``peer_id`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and `psa_pake_set_peer()`, `psa_pake_input()`, and `psa_pake_output()` must not have been called yet.
        *   Calling `psa_pake_set_peer()` is invalid with the operation's algorithm.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``peer_id`` is not valid for the operation's algorithm and cipher suite.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The value of ``peer_id`` is not supported by the implementation.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    Call this function in addition to `psa_pake_set_user()` for PAKE algorithms that associate a user identifier with both participants in the session.
    For PAKE algorithms that associate a single user identifier with the session, call `psa_pake_set_user()` only.

    Refer to the documentation of individual PAKE algorithms for more information.

.. function:: psa_pake_set_context

    .. summary::
        Set the context data for a password-authenticated key exchange.

        .. versionadded:: 1.2

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: const uint8_t * context
        The peer's ID to authenticate.
    .. param:: size_t context_len
        Size of the ``context`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and `psa_pake_set_context()`, `psa_pake_input()`, and `psa_pake_output()` must not have been called yet.
        *   Calling `psa_pake_set_context()` is invalid with the operation's algorithm.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``context`` is not valid for the operation's algorithm and cipher suite.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The value of ``context`` is not supported by the implementation.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    Call this function for PAKE algorithms that accept additional context data as part of the protocol setup.

    Refer to the documentation of individual PAKE algorithms for more information.

.. function:: psa_pake_output

    .. summary::
        Get output for a step of a password-authenticated key exchange.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: psa_pake_step_t step
        The step of the algorithm for which the output is requested.
    .. param:: uint8_t * output
        Buffer where the output is to be written.
        The format of the output depends on the ``step``, see :secref:`pake-steps`.
    .. param:: size_t output_size
        Size of the ``output`` buffer in bytes.
        This must be appropriate for the cipher suite and output step:

        *   A sufficient output size is :code:`PSA_PAKE_OUTPUT_SIZE(alg, primitive, step)` where ``alg`` and ``primitive`` are the PAKE algorithm and primitive in the operation's cipher suite, and ``step`` is the output step.
        *   `PSA_PAKE_OUTPUT_MAX_SIZE` evaluates to the maximum output size of any supported PAKE algorithm, primitive and step.
    .. param:: size_t * output_length
        On success, the number of bytes of the returned output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``(*output_length)`` bytes of ``output`` contain the output.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active and fully set up, and this call must conform to the algorithm's requirements for ordering of input and output steps.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``step`` is not compatible with the operation's algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``step`` is not supported with the operation's algorithm.
    .. retval:: PSA_ERROR_BUFFER_TOO_SMALL
        The size of the ``output`` buffer is too small.
        `PSA_PAKE_OUTPUT_SIZE()` or `PSA_PAKE_OUTPUT_MAX_SIZE` can be used to determine a sufficient buffer size.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    Depending on the algorithm being executed, you might need to call this function several times or you might not need to call this at all.

    The exact sequence of calls to perform a password-authenticated key exchange depends on the algorithm in use.
    Refer to the documentation of individual PAKE algorithms for more information.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_pake_abort()`.

.. function:: psa_pake_input

    .. summary::
        Provide input for a step of a password-authenticated key exchange.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: psa_pake_step_t step
        The step for which the input is provided.
    .. param:: const uint8_t * input
        Buffer containing the input.
        The format of the input depends on the ``step``, see :secref:`pake-steps`.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active and fully set up, and this call must conform to the algorithm's requirements for ordering of input and output steps.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   ``step`` is not compatible with the operation's algorithm.
        *   The input is not valid for the operation's algorithm, cipher suite or ``step``.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The verification fails for a `PSA_PAKE_STEP_ZK_PROOF` or `PSA_PAKE_STEP_CONFIRM` input step.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The following conditions can result in this error:

        *   ``step`` is not supported with the operation's algorithm.
        *   The input is not supported for the operation's algorithm, cipher suite or ``step``.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    Depending on the algorithm being executed, you might need to call this function several times or you might not need to call this at all.

    The exact sequence of calls to perform a password-authenticated key exchange depends on the algorithm in use.
    Refer to the documentation of individual PAKE algorithms for more information.

    `PSA_PAKE_INPUT_SIZE()` or `PSA_PAKE_INPUT_MAX_SIZE` can be used to allocate buffers of sufficient size to transfer inputs that are received from the peer into the operation.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_pake_abort()`.

.. function:: psa_pake_get_shared_key

    .. summary::
        Extract the shared secret from the PAKE as a key.

        .. versionadded:: 1.2

    .. param:: psa_pake_operation_t * operation
        Active PAKE operation.
    .. param:: const psa_key_attributes_t * attributes
        The attributes for the new key.

        The following attributes are required for all keys:

        *   The key type.
            All PAKE algorithms can output a key of type `PSA_KEY_TYPE_DERIVE` or `PSA_KEY_TYPE_HMAC`.
            PAKE algorithms that produce a pseudorandom shared secret, can also output block-cipher key types, for example `PSA_KEY_TYPE_AES`.
            Refer to the documentation of individual PAKE algorithms for more information.

        The following attributes must be set for keys used in cryptographic operations:

        *   The key permitted-algorithm policy, see :secref:`permitted-algorithms`.
        *   The key usage flags, see :secref:`key-usage-flags`.

        The following attributes must be set for keys that do not use the default `PSA_KEY_LIFETIME_VOLATILE` lifetime:

        *   The key lifetime, see :secref:`key-lifetimes`.
        *   The key identifier is required for a key with a persistent lifetime, see :secref:`key-identifiers`.

        The following attributes are optional:

        *   If the key size is nonzero, it must be equal to the size of the PAKE shared secret.

        .. note::
            This is an input parameter: it is not updated with the final key attributes.
            The final attributes of the new key can be queried by calling `psa_get_key_attributes()` with the key's identifier.

    .. param:: psa_key_id_t * key
        On success, an identifier for the newly created key. `PSA_KEY_ID_NULL` on failure.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        If the key is persistent, the key material and the key's metadata have been saved to persistent storage.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The state of PAKE operation ``operation`` is not valid: it must be ready to return the shared secret.

            For an unconfirmed key, this will be when the key-exchange output and input steps are complete, but prior to any key-confirmation output and input steps.

            For a confirmed key, this will be when all key-exchange and key-confirmation output and input steps are complete.
        *   The library requires initializing by a call to `psa_crypto_init()`.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        This is an attempt to create a persistent key, and there is already a persistent key with the given identifier.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The key attributes, as a whole, are not supported for creation from a PAKE secret, either by the implementation in general or in the specified storage location.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The key type is not valid for output from this operation's algorithm.
        *   The key size is nonzero.
        *   The key lifetime is invalid.
        *   The key identifier is not valid for the key lifetime.
        *   The key usage flags include invalid values.
        *   The key's permitted-usage algorithm is invalid.
        *   The key attributes, as a whole, are invalid.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The implementation does not permit creating a key with the specified attributes due to some implementation-specific policy.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID

    The shared secret is retrieved as a key.
    Its location, policy, and type are taken from ``attributes``.

    The size of the returned key is always the bit-size of the PAKE shared secret, rounded up to a whole number of bytes. The size of the shared secret is dependent on the PAKE algorithm and cipher suite.

    This is the final call in a PAKE operation, which retrieves the shared secret as a key.
    It is recommended that this key is used as an input to a key-derivation operation to produce additional cryptographic keys.
    For some PAKE algorithms, the shared secret is also suitable for use as a key in cryptographic operations such as encryption.
    Refer to the documentation of individual PAKE algorithms for more information.

    Depending on the key confirmation requested in the cipher suite, `psa_pake_get_shared_key()` must be called either before or after the key-confirmation output and input steps for the PAKE algorithm.
    The key confirmation affects the guarantees that can be made about the shared key:

    .. list-table::
        :class: borderless
        :widths: 1 4

        *   -   **Unconfirmed key**
            -   If the cipher suite used to set up the operation requested an unconfirmed key, the application must call `psa_pake_get_shared_key()` after the key-exchange output and input steps are completed.
                The PAKE algorithm provides a cryptographic guarantee that only a peer who used the same password, and identity inputs, is able to compute the same key.
                However, there is no guarantee that the peer is the participant it claims to be, and was able to compute the same key.

                Since the peer is not authenticated, no action should be taken that assumes that the peer is who it claims to be.
                For example, do not access restricted resources on the peer's behalf until an explicit authentication has succeeded.

                .. note::
                    Some PAKE algorithms do not enable the output of the shared secret until it has been confirmed.

        *   -   **Confirmed key**
            -   If the cipher suite used to set up the operation requested a confirmed key, the application must call `psa_pake_get_shared_key()` after the key-exchange and key-confirmation output and input steps are completed.

                Following key confirmation, the PAKE algorithm provides a cryptographic guarantee that the peer used the same password and identity inputs, and has computed the identical shared secret key.

                .. note::
                    Some PAKE algorithms do not include any key-confirmation steps.

    The exact sequence of calls to perform a password-authenticated key exchange depends on the algorithm in use.
    Refer to the documentation of individual PAKE algorithms for more information.

    When this function returns successfully, ``operation`` becomes inactive.
    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_pake_abort()`.

.. function:: psa_pake_abort

    .. summary::
        Abort a PAKE operation.

        .. versionadded:: 1.1

    .. param:: psa_pake_operation_t * operation
        Initialized PAKE operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing by a call to `psa_crypto_init()`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself.
    Once aborted, the operation object can be reused for another operation by calling `psa_pake_setup()` again.

    This function can be called any time after the operation object has been initialized as described in `psa_pake_operation_t`.

    In particular, calling `psa_pake_abort()` after the operation has been terminated by a call to `psa_pake_abort()` or `psa_pake_get_shared_key()` is safe and has no effect.


.. _pake-support:

PAKE support macros
-------------------

.. macro:: PSA_PAKE_OUTPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_pake_output()`, in bytes.

        .. versionadded:: 1.1

    .. param:: alg
        A PAKE algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_PAKE(alg)` is true.
    .. param:: primitive
        A primitive of type `psa_pake_primitive_t` that is compatible with algorithm ``alg``.
    .. param:: output_step
        A value of type `psa_pake_step_t` that is valid for the algorithm ``alg``.

    .. return::
        A sufficient output buffer size for the specified PAKE algorithm, primitive, and output step.
        An implementation can return either ``0`` or a correct size for a PAKE algorithm, primitive, and output step that it recognizes, but does not support.
        If the parameters are not valid, the return value is unspecified.

    If the size of the output buffer is at least this large, it is guaranteed that `psa_pake_output()` will not fail due to an insufficient buffer size.
    The actual size of the output might be smaller in any given call.

    See also `PSA_PAKE_OUTPUT_MAX_SIZE`

.. macro:: PSA_PAKE_OUTPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient output buffer size for `psa_pake_output()` for any of the supported PAKE algorithms, primitives and output steps.

        .. versionadded:: 1.1

    If the size of the output buffer is at least this large, it is guaranteed that `psa_pake_output()` will not fail due to an insufficient buffer size.

    See also `PSA_PAKE_OUTPUT_SIZE()`.

.. macro:: PSA_PAKE_INPUT_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient buffer size for inputs to `psa_pake_input()`.

        .. versionadded:: 1.1

    .. param:: alg
        A PAKE algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_PAKE(alg)` is true.
    .. param:: primitive
        A primitive of type `psa_pake_primitive_t` that is compatible with algorithm ``alg``.
    .. param:: input_step
        A value of type `psa_pake_step_t` that is valid for the algorithm ``alg``.

    .. return::
        A sufficient buffer size for the specified PAKE algorithm, primitive, and input step.
        An implementation can return either ``0`` or a correct size for a PAKE algorithm, primitive, and output step that it recognizes, but does not support.
        If the parameters are not valid, the return value is unspecified.

    The value returned by this macro is guaranteed to be large enough for any valid input to `psa_pake_input()` in an operation with the specified parameters.

    This macro can be useful when transferring inputs from the peer into the PAKE operation.

    See also `PSA_PAKE_INPUT_MAX_SIZE`

.. macro:: PSA_PAKE_INPUT_MAX_SIZE
    :definition: /* implementation-defined value */

    .. summary::
        Sufficient buffer size for inputs to `psa_pake_input()` for any of the supported PAKE algorithms, primitives and input steps.

        .. versionadded:: 1.1

    This macro can be useful when transferring inputs from the peer into the PAKE operation.

    See also `PSA_PAKE_INPUT_SIZE()`.


.. _pake-jpake:

The J-PAKE protocol
-------------------

J-PAKE is the password-authenticated key exchange by juggling protocol, defined by :RFC-title:`8236`.
This protocol uses the Schnorr Non-Interactive Zero-Knowledge Proof (NIZKP), as defined by :RFC-title:`8235`.

J-PAKE is a balanced PAKE, without key confirmation.

.. _jpake-cipher-suites:

J-PAKE cipher suites
~~~~~~~~~~~~~~~~~~~~

When setting up a PAKE cipher suite to use the J-PAKE protocol:

*   Use the `PSA_ALG_JPAKE()` algorithm, parameterized by the required hash algorithm.
*   Use a PAKE primitive for the required elliptic curve, or finite field group.
*   J-PAKE does not confirm the shared secret key that results from the key exchange.

For example, the following code creates a cipher suite to select J-PAKE using P-256 with the SHA-256 hash function:

.. code-block:: xref

    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE(PSA_ALG_SHA_256));
    psa_pake_cs_set_primitive(&cipher_suite,
                              PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
                                                 PSA_ECC_FAMILY_SECP_R1, 256));
    psa_pake_cs_set_key_confirmation(&cipher_suite, PSA_PAKE_UNCONFIRMED_KEY);

More information on selecting a specific elliptic curve or finite field Diffie-Hellman group is provided with the `PSA_PAKE_PRIMITIVE_TYPE_ECC` and `PSA_PAKE_PRIMITIVE_TYPE_DH` constants.

.. _jpake-passwords:

J-PAKE password processing
~~~~~~~~~~~~~~~~~~~~~~~~~~

The PAKE operation for J-PAKE expects a key of type type `PSA_KEY_TYPE_PASSWORD` or PSA_KEY_TYPE_PASSWORD_HASH`.
The same key value must be provided to the PAKE operation in both participants.

The key can be the password text itself, in an agreed character encoding, or some value derived from the password, as required by a higher level protocol.
For low-entropy passwords, it is recommended that a key-stretching derivation algorithm, such as PBKDF2, is used, and the resulting password hash is used as the key input to the PAKE operation.

.. _jpake-operation:

J-PAKE operation
~~~~~~~~~~~~~~~~

The J-PAKE operation follows the protocol shown in :numref:`fig-jpake`.

.. figure:: /figure/pake/j-pake.*
    :name: fig-jpake

    The J-PAKE protocol

    The variable names :math:`x1`, :math:`g1`, and so on, are taken from the finite field implementation of J-PAKE in :RFC:`8236#2`.

    Details of the computation for the key shares and zero-knowledge proofs are in :RFC:`8236` and :RFC:`8235`.

Setup
^^^^^

J-PAKE does not assign roles to the participants, so it is not necessary to call `psa_pake_set_role()`.

J-PAKE requires both an application and a peer identity.
If the peer identity provided to `psa_pake_set_peer()` does not match the data received from the peer, then the call to `psa_pake_input()` for the `PSA_PAKE_STEP_ZK_PROOF` step will fail with :code:`PSA_ERROR_INVALID_SIGNATURE`.

J-PAKE does not use a context.
A call to `psa_pake_set_context()` for a J-PAKE operation will fail with :code:`PSA_ERROR_BAD_STATE`.

The following steps demonstrate the application code for 'User' in :numref:`fig-jpake`. The code flow for the 'Peer' is the same as for 'User', as J-PAKE is a balanced PAKE.

1.  To prepare a J-PAKE operation, initialize and set up a `psa_pake_operation_t` object by calling the following functions:

    .. code-block:: xref

        psa_pake_operation_t jpake = PSA_PAKE_OPERATION_INIT;

        psa_pake_setup(&jpake, pake_key, &cipher_suite);
        psa_pake_set_user(&jpake, ...);
        psa_pake_set_peer(&jpake, ...);

    See :secref:`jpake-cipher-suites` and :secref:`jpake-passwords` for details on the requirements for the cipher suite and key.

    The key material is used as an array of bytes, which is converted to an integer as described in :cite-title:`SEC1` §2.3.8, before reducing it modulo :math:`q`.
    Here, :math:`q` is the order of the group defined by the cipher-suite primitive.
    `psa_pake_setup()` will return an error if the result of the conversion and reduction is ``0``.

Key exchange
^^^^^^^^^^^^

After setup, the key exchange flow for J-PAKE is as follows:

2.  Round one.

    The application can either extract the round one output values first, and then provide the round one inputs that are received from the Peer; or provide the peer inputs first, and then extract the outputs.

    To get the first round data that needs to be sent to the peer, make the following calls to `psa_pake_output()`, in the order shown:

    .. code-block:: xref

        // Get g1
        psa_pake_output(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Get V1, the ZKP public key for x1
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Get r1, the ZKP proof for x1
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);
        // Get g2
        psa_pake_output(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Get V2, the ZKP public key for x2
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Get r2, the ZKP proof for x2
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);

    To provide the first round data received from the peer to the operation, make the following calls to `psa_pake_input()`, in the order shown:

    .. code-block:: xref

        // Set g3
        psa_pake_input(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Set V3, the ZKP public key for x3
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Set r3, the ZKP proof for x3
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);
        // Set g4
        psa_pake_input(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Set V4, the ZKP public key for x4
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Set r4, the ZKP proof for x4
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);

3.  Round two.

    The application can either extract the round two output values first, and then provide the round two inputs that are received from the Peer; or provide the peer inputs first, and then extract the outputs.

    To get the second round data that needs to be sent to the peer, make the following calls to `psa_pake_output()`, in the order shown:

    .. code-block:: xref

        // Get A
        psa_pake_output(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Get V5, the ZKP public key for x2*s
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Get r5, the ZKP proof for x2*s
        psa_pake_output(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);

    To provide the second round data received from the peer to the operation, make the following calls to `psa_pake_input()`, in the order shown:

    .. code-block:: xref

        // Set B
        psa_pake_input(&jpake, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Set V6, the ZKP public key for x4*s
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PUBLIC, ...);
        // Set r6, the ZKP proof for x4*s
        psa_pake_input(&jpake, PSA_PAKE_STEP_ZK_PROOF, ...);

Extract shared secret
^^^^^^^^^^^^^^^^^^^^^

4.  To use the shared secret, extract it as a key-derivation key.
    For example, to extract a derivation key for HKDF-SHA-256:

    .. code-block:: xref

        // Set up the key attributes
        psa_key_attributes_t att = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&att, PSA_KEY_TYPE_DERIVE);
        psa_set_key_usage_flags(&att, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&att, PSA_ALG_HKDF(PSA_ALG_SHA_256));

        // Get Ka=Kb=K
        psa_key_id_t shared_key;
        psa_pake_get_shared_key(&jpake, &att, &shared_key);

For more information about the format of the values which are passed for each step, see :secref:`pake-steps`.

If the verification of a Zero-knowledge proof provided by the peer fails, then the corresponding call to `psa_pake_input()` for the `PSA_PAKE_STEP_ZK_PROOF` step will return :code:`PSA_ERROR_INVALID_SIGNATURE`.

The shared secret that is produced by J-PAKE is not suitable for use as an encryption key.
It must be used as an input to a key-derivation operation to produce additional cryptographic keys.

.. warning::

    At the end of this sequence there is a cryptographic guarantee that only a peer that used the same password is able to compute the same key.
    But there is no guarantee that the peer is the participant it claims to be, or that the peer used the same password during the exchange.

    At this point, authentication is implicit --- material encrypted or authenticated using the computed key can only be decrypted or verified by someone with the same key.
    The peer is not authenticated at this point, and no action should be taken by the application which assumes that the peer is authenticated, for example, by accessing restricted resources.

    To make the authentication explicit, there are various methods to confirm that both parties have the same key. See :RFC:`8236#5` for two examples.

J-PAKE algorithms
-----------------

.. macro:: PSA_ALG_JPAKE
    :definition: /* specification-defined value */

    .. summary::
        Macro to build the Password-authenticated key exchange by juggling (J-PAKE) algorithm.

        .. versionadded:: 1.1

        .. versionchanged:: 1.2 Parameterize J-PAKE algorithm by hash.

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        A J-PAKE algorithm, parameterized by a specific hash.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is J-PAKE as defined by :RFC:`8236`, instantiated with the following parameters:

    *   The primitive group can be either an elliptic curve or defined over a finite field.
    *   The Schnorr NIZKP, using the same group as the J-PAKE algorithm.
    *   The cryptographic hash function, ``hash_alg``.

    J-PAKE does not confirm the shared secret key that results from the key exchange.

    The shared secret that is produced by J-PAKE is not suitable for use as an encryption key.
    It must be used as an input to a key-derivation operation to produce additional cryptographic keys.

    See :secref:`pake-jpake` for the J-PAKE protocol flow and how to implement it with the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_PASSWORD`
        | `PSA_KEY_TYPE_PASSWORD_HASH`

.. macro:: PSA_ALG_IS_JPAKE
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a J-PAKE algorithm.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a J-PAKE algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    J-PAKE algorithms are constructed using :code:`PSA_ALG_JPAKE(hash_alg)`.

.. _pake-spake2p:

The SPAKE2+ protocol
--------------------

SPAKE2+ is the augmented password-authenticated key exchange protocol, defined by :rfc-title:`9383`.
SPAKE2+ includes confirmation of the shared secret key that results from the key exchange.

SPAKE2+ is required by :cite-title:`MATTER`, as MATTER_PAKE.
:cite:`MATTER` uses an earlier draft of the SPAKE2+ protocol, :cite-title:`SPAKE2P-2`.

Although the operation of the PAKE is similar for both of these variants, they have different key schedules for the derivation of the shared secret.

.. _spake2p-cipher-suites:

SPAKE2+ cipher suites
~~~~~~~~~~~~~~~~~~~~~

SPAKE2+ is instantiated with the following parameters:

*   An elliptic curve group.
*   A cryptographic hash function.
*   A key-derivation function.
*   A keyed MAC function.

Valid combinations of these parameters are defined in the table of cipher suites in :rfc:`9383#4`.

When setting up a PAKE cipher suite to use the SPAKE2+ protocol defined in :rfc:`9383`:

*   For cipher-suites that use HMAC for key confirmation, use the `PSA_ALG_SPAKE2P_HMAC()` algorithm, parameterized by the required hash algorithm.
*   For cipher-suites that use CMAC-AES-128 for key confirmation, use the `PSA_ALG_SPAKE2P_CMAC()` algorithm, parameterized by the required hash algorithm.
*   Use a PAKE primitive for the required elliptic curve.

For example, the following code creates a cipher suite to select SPAKE2+ using edwards25519 with the SHA-256 hash function:

.. code-block:: xref

    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256));
    psa_pake_cs_set_primitive(&cipher_suite,
                              PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
                                                 PSA_ECC_FAMILY_TWISTED_EDWARDS, 255));

When setting up a PAKE cipher suite to use the SPAKE2+ protocol used by :cite:`MATTER`:

*   Use the `PSA_ALG_SPAKE2P_MATTER` algorithm.
*   Use the :code:`PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256)` PAKE primitive.

The following code creates a cipher suite to select the :cite:`MATTER` variant of SPAKE2+:

.. code-block:: xref

    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_SPAKE2P_MATTER);
    psa_pake_cs_set_primitive(&cipher_suite,
                              PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
                                                 PSA_ECC_FAMILY_SECP_R1, 256));

.. _spake2p-registration:

SPAKE2+ registration
~~~~~~~~~~~~~~~~~~~~

The SPAKE2+ protocol has distinct roles for the two participants:

*   The *Prover* takes the role of client.
    It uses the protocol to prove that it knows the secret password, and produce a shared secret.
*   The *Verifier* takes the role of server.
    It uses the protocol to verify the client's proof, and produce a shared secret.

The registration phase of SPAKE2+ provides the initial password processing, described in :rfc:`9383#3.2`.
The result of registration is two pairs of values --- :math:`(w0, w1)` and :math:`(w0, L)` --- that need to be provided during the authentication phase to the Prover and Verifier, respectively.
The design of SPAKE2+ ensures that knowledge of :math:`(w0, L)` does not enable an attacker to determine the password, or to compute :math:`w1`.

In the |API|, the registration output values are managed as an asymmetric key pair:

*   The Prover values, :math:`(w0, w1)`, are stored in a key of type `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR()`.
*   The Verifier values, :math:`(w0, L)`, are stored in a key of type `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY()`, or derived from the matching `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR()`.

The SPAKE2+ key types are parameterized by the same elliptic curve as the SPAKE2+ cipher suite.

The key pair is derived from the initial SPAKE2+ password prior to starting the PAKE operation.
It is recommended to use a key-stretching derivation algorithm, for example PBKDF2.
This process can take place immediately before the PAKE operation, or derived at some earlier point and stored by the participant.
Alternatively, the Verifier can be provisioned with the `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY()` for the protocol, by the Prover, or some other agent.
:numref:`fig-spake2p-reg` illustrates some example SPAKE2+ key-derivation flows.

The resulting SPAKE2+ key pair must be protected at least as well as the password.
The public key, exported from the key pair, does not need to be kept confidential.
It is recommended that the Verifier stores only the public key, because disclosure of the public key does not enable an attacker to impersonate the Prover.

.. figure:: /figure/pake/spake2plus-reg.*
    :name: fig-spake2p-reg

    Examples of SPAKE2+ key-derivation procedures

    The variable names :math:`w0`, :math:`w1`, and :math:`L` are taken from the description of SPAKE2+ in :RFC:`9383`.

    Details of the computation for the key-derivation values are in :RFC:`9383#3.2`.

The following steps demonstrate the derivation of a SPAKE2+ key pair using PBKDF2-HMAC-SHA256, for use with a SPAKE2+ cipher suite, ``cipher_suite``. See :secref:`spake2p-cipher-suites` for an example of how to construct the cipher suite object.

1.  Allocate and initialize a key-derivation object:

    .. code-block:: xref

        psa_key_derivation_operation_t pbkdf = PSA_KEY_DERIVATION_OPERATION_INIT;

#.  Setup the key derivation from the SPAKE2+ password, ``password_key``, and parameters ``pbkdf2_params``:

    .. code-block:: xref

        psa_key_derivation_setup(&pbkdf, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256));
        psa_key_derivation_input_key(&pbkdf, PSA_KEY_DERIVATION_INPUT_PASSWORD, password_key);
        psa_key_derivation_input_integer(&pbkdf, PSA_KEY_DERIVATION_INPUT_COST, pbkdf2_params.cost);
        psa_key_derivation_input_bytes(&pbkdf, PSA_KEY_DERIVATION_INPUT_SALT,
                                               &pbkdf2_params.salt, pbkdf2_params.salt_len);

#.  Allocate and initialize a key attributes object:

    .. code-block:: xref

        psa_key_attributes_t att = PSA_KEY_ATTRIBUTES_INIT;

#.  Set the key type, size, and policy from the ``cipher_suite`` object:

    .. code-block:: xref

        const psa_pake_primitive_t primitive = psa_pake_cs_get_primitive(&cipher_suite);

        psa_set_key_type(&att,
                         PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(PSA_PAKE_PRIMITIVE_GET_FAMILY(primitive)));
        psa_set_key_bits(&att, PSA_PAKE_PRIMITIVE_GET_BITS(primitive));
        psa_set_key_usage_flags(&att, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&att, psa_pake_cs_get_algorithm(&cipher_suite));

#.  Derive the key:

    .. code-block:: xref

        psa_key_id_t spake2p_key;
        psa_key_derivation_output_key(&att, &pbkdf, &spake2p_key);
        psa_key_derivation_abort(&pbkdf);

See :secref:`spake2p-keys` for details of the key types, key-pair derivation, and public-key format.

.. _spake2p-operation:

SPAKE2+ operation
~~~~~~~~~~~~~~~~~

The SPAKE2+ operation follows the protocol shown in :numref:`fig-spake2p`.

.. figure:: /figure/pake/spake2plus.*
    :name: fig-spake2p

    The SPAKE2+ authentication and key confirmation protocol

    The variable names :math:`w0`, :math:`w1`, :math:`L`, and so on, are taken from the description of SPAKE2+ in :RFC:`9383`.

    Details of the computation for the key shares is in :RFC:`9383#3.3` and confirmation values in :RFC:`9383#3.4`.

Setup
^^^^^

In SPAKE2+, the Prover uses the `PSA_PAKE_ROLE_CLIENT` role, and the Verifier uses the `PSA_PAKE_ROLE_SERVER` role.

The key passed to the Prover must be a SPAKE2+ key pair, which is derived as recommended in :secref:`spake2p-registration`.
The key passed to the Verifier can either be a SPAKE2+ key pair, or a SPAKE2+ public key.
A SPAKE2+ public key is imported from data that is output by calling `psa_export_public_key()` on a SPAKE2+ key pair.

Both participants in SPAKE2+ have an optional identity.
If no identity value is provided, then a zero-length string is used for that identity in the protocol.
If the participants do not supply the same identity values to the protocol, the computed secrets will be different, and key confirmation will fail.

Participants in SPAKE2+ can optionally provide a context:

*   If `psa_pake_set_context()` is called, then the context and its encoded length are included in the SPAKE2+ transcript computation.
    This includes the case of a zero-length context.
*   If `psa_pake_set_context()` is not called, then the context and its encoded length are omitted entirely from the SPAKE2+ transcript computation.
    See :RFC:`9383#3.3`.

If the participants do not supply the same context value to the protocol, the computed secrets will be different, and key confirmation will fail.

The following steps demonstrate the application code for both Prover and Verifier in :numref:`fig-spake2p`.

**Prover**
    To prepare a SPAKE2+ operation for the Prover, initialize and set up a `psa_pake_operation_t` object by calling the following functions:

    .. code-block:: xref

        psa_pake_operation_t spake2p_p = PSA_PAKE_OPERATION_INIT;

        psa_pake_setup(&spake2p_p, pake_key_p, &cipher_suite);
        psa_pake_set_role(&spake2p_p, PSA_PAKE_ROLE_CLIENT);

    The key ``pake_key_p`` is a SPAKE2+ key pair, `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR()`.
    See :secref:`spake2p-cipher-suites` for details on constructing a suitable cipher suite.

**Prover**
    Provide any additional, optional, parameters:

    .. code-block:: xref

        psa_pake_set_user(&spake2p_p, ...);       // Prover identity
        psa_pake_set_peer(&spake2p_p, ...);       // Verifier identity
        psa_pake_set_context(&spake2p_p, ...);    // Optional context

**Verifier**
    To prepare a SPAKE2+ operation for the Verifier, initialize and set up a `psa_pake_operation_t` object by calling the following functions:

    .. code-block:: xref

        psa_pake_operation_t spake2p_v = PSA_PAKE_OPERATION_INIT;

        psa_pake_setup(&spake2p_v, pake_key_v, &cipher_suite);
        psa_pake_set_role(&spake2p_v, PSA_PAKE_ROLE_SERVER);

    The key ``pake_key_v`` is a SPAKE2+ key pair, `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR()`, or public key, `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY()`.
    See :secref:`spake2p-cipher-suites` for details on constructing a suitable cipher suite.

**Verifier**
    Provide any additional, optional, parameters:

    .. code-block:: xref

        psa_pake_set_user(&spake2p_v, ...);       // Verifier identity
        psa_pake_set_peer(&spake2p_v, ...);       // Prover identity
        psa_pake_set_context(&spake2p_v, ...);    // Optional context

Key exchange and confirmation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After setup, the key exchange and confirmation flow for SPAKE2+ is as follows.

.. note::

    The sequence of calls for the Prover, and the sequence for the Verifier, must be in exactly this order.

**Prover**
    To get the key share to send to the Verifier, call:

    .. code-block:: xref

        // Get shareP
        psa_pake_output(&spake2p_p, PSA_PAKE_STEP_KEY_SHARE, ...);

**Verifier**
    To provide and validate the key share received from the Prover, call:

    .. code-block:: xref

        // Set shareP
        psa_pake_input(&spake2p_v, PSA_PAKE_STEP_KEY_SHARE, ...);

**Verifier**
    To get the Verifier key share and confirmation value to send to the Prover, call:

    .. code-block:: xref

        // Get shareV
        psa_pake_output(&spake2p_v, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Get confirmV
        psa_pake_output(&spake2p_v, PSA_PAKE_STEP_CONFIRM, ...);

**Prover**
    To provide and validate the key share and verify the confirmation value received from the Verifier, call:

    .. code-block:: xref

        // Set shareV
        psa_pake_input(&spake2p_p, PSA_PAKE_STEP_KEY_SHARE, ...);
        // Set confirmV
        psa_pake_input(&spake2p_p, PSA_PAKE_STEP_KEY_CONFIRM, ...);

**Prover**
    To get the Prover key confirmation value to send to the Verifier, call:

    .. code-block:: xref

        // Get confirmP
        psa_pake_output(&spake2p_p, PSA_PAKE_STEP_CONFIRM, ...);

**Verifier**
    To verify the confirmation value received from the Prover, call:

    .. code-block:: xref

        // Set confirmP
        psa_pake_input(&spake2p_v, PSA_PAKE_STEP_CONFIRM, ...);

Extract shared secret
^^^^^^^^^^^^^^^^^^^^^

**Prover**
    To use the shared secret, extract it as a key-derivation key.
    For example, to extract a derivation key for HKDF-SHA-256:

    .. code-block:: xref

        // Set up the key attributes
        psa_key_attributes_t att = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&att, PSA_KEY_TYPE_DERIVE);
        psa_set_key_usage_flags(&att, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&att, PSA_ALG_HKDF(PSA_ALG_SHA_256));

        // Get K_shared
        psa_key_id_t shared_key;
        psa_pake_get_shared_key(&spake2p_p, &att, &shared_key);

**Verifier**
    To use the shared secret, extract it as a key-derivation key.
    The same key attributes can be used as the Prover:

    .. code-block:: xref

        // Get K_shared
        psa_key_id_t shared_key;
        psa_pake_get_shared_key(&spake2p_v, &att, &shared_key);

The shared secret that is produced by SPAKE2+ is pseudorandom.
Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

For more information about the format of the values which are passed for each step, see :secref:`pake-steps`.

If the validation of a key share fails, then the corresponding call to `psa_pake_input()` for the `PSA_PAKE_STEP_KEY_SHARE` step will return :code:`PSA_ERROR_INVALID_ARGUMENT`.
If the verification of a key confirmation value fails, then the corresponding call to `psa_pake_input()` for the `PSA_PAKE_STEP_CONFIRM` step will return :code:`PSA_ERROR_INVALID_SIGNATURE`.

.. _spake2p-algorithms:

SPAKE2+ algorithms
------------------

.. macro:: PSA_ALG_SPAKE2P_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build the SPAKE2+ algorithm, using HMAC-based key confirmation.

        .. versionadded:: 1.2

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        A SPAKE2+ algorithm, using HMAC for key confirmation, parameterized by a specific hash.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is SPAKE2+, as defined by :RFC-title:`9383`, for cipher suites that use HMAC for key confirmation.
    SPAKE2+ cipher suites are specified in :rfc:`9383#4`.
    See :secref:`spake2p-cipher-suites`.

    The shared secret that is produced by SPAKE2+ is pseudorandom.
    Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

    See :secref:`pake-spake2p` for the SPAKE2+ protocol flow and how to implement it with the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR`
        | `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY` (verification only)

.. macro:: PSA_ALG_SPAKE2P_CMAC
    :definition: /* specification-defined value */

    .. summary::
        Macro to build the SPAKE2+ algorithm, using CMAC-based key confirmation.

        .. versionadded:: 1.2

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        A SPAKE2+ algorithm, using CMAC for key confirmation, parameterized by a specific hash.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.


    This is SPAKE2+, as defined by :RFC-title:`9383`, for cipher suites that use CMAC-AES-128 for key confirmation.
    SPAKE2+ cipher suites are specified in :rfc:`9383#4`.
    The cipher suite's hash algorithm is used as input to `PSA_ALG_SPAKE2P_CMAC()`.

    The shared secret that is produced by SPAKE2+ is pseudorandom.
    Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

    See :secref:`pake-spake2p` for the SPAKE2+ protocol flow and how to implement it with the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR`
        | `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY` (verification only)

.. macro:: PSA_ALG_SPAKE2P_MATTER
    :definition: ((psa_algorithm_t)0x0A000609)

    .. summary::
        The SPAKE2+ algorithm, as used by the Matter v1 specification.

        .. versionadded:: 1.2

    This is the PAKE algorithm specified as MATTER_PAKE in :cite-title:`MATTER`.
    This is based on draft-02 of the SPAKE2+ protocol, :cite-title:`SPAKE2P-2`.
    :cite:`MATTER` specifies a single SPAKE2+ cipher suite, P256-SHA256-HKDF-HMAC-SHA256.

    The shared secret that is produced by this operation must be processed as directed by the :cite:`MATTER` specification.

    This algorithm uses the same SPAKE2+ key types, key derivation, protocol flow, and the API usage described in :secref:`pake-spake2p`.
    However, the following aspects are different:

    *   The key schedule is different.
        This affects the computation of the shared secret and key confirmation values.
    *   The protocol inputs and outputs have been renamed between draft-02 and the final RFC, as follows:

        .. csv-table::
            :header-rows: 1
            :widths: auto
            :align: left

            RFC 9383, Draft-02
            shareP, pA
            shareV, pB
            confirmP, cA
            confirmV, cB
            K_shared, Ke

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_SPAKE2P_KEY_PAIR`
        | `PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY` (verification only)

.. macro:: PSA_ALG_IS_SPAKE2P
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a SPAKE2+ algorithm.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a SPAKE2+ algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    SPAKE2+ algorithms are constructed using :code:`PSA_ALG_SPAKE2P_HMAC(hash_alg)`, :code:`PSA_ALG_SPAKE2P_CMAC(hash_alg)`, or :code:`PSA_ALG_SPAKE2P_MATTER`.

.. macro:: PSA_ALG_IS_SPAKE2P_HMAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a SPAKE2+ algorithm that uses a HMAC-based key confirmation.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a SPAKE2+ algorithm that uses a HMAC-based key confirmation, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    SPAKE2+ algorithms, using HMAC-based key confirmation, are constructed using :code:`PSA_ALG_SPAKE2P_HMAC(hash_alg)`.

.. macro:: PSA_ALG_IS_SPAKE2P_CMAC
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a SPAKE2+ algorithm that uses a CMAC-based key confirmation.

        .. versionadded:: 1.2

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a SPAKE2+ algorithm that uses a CMAC-based key confirmation, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    SPAKE2+ algorithms, using CMAC-based key confirmation, are constructed using :code:`PSA_ALG_SPAKE2P_CMAC(hash_alg)`.


.. _pake-wpa3-sae:

The WPA3-SAE protocol
---------------------

WPA3-SAE is a balanced, password-authenticated key exchange protocol, defined by :cite-title:`IEEE-802.11`.
It is used as the authentication and key exchange protocol for WLAN access points and mesh networks.
WPA3-SAE includes confirmation of the shared secret key that results from the key exchange.

.. _wpa3-sae-cipher-suites:

WPA3-SAE cipher suites
~~~~~~~~~~~~~~~~~~~~~~

WPA3-SAE is instantiated with the following parameters:

*   An elliptic curve group or a finite field cyclic group.
*   A cryptographic hash function.

`[IEEE-802.11]` describes three variants of the WPA3-SAE algorithm.
These differ in the method used to generate a password element (PWE) from the password, and in the size of the key confirmation key (SAE-KCK) and pairwise master key (PMK).

:numref:`tab-wpa3-sae-variants` summarizes the properties of the different algorithm variants.

.. list-table:: WPA3-SAE algorithm variants
    :name: tab-wpa3-sae-variants
    :header-rows: 1
    :widths: 4 3 3 2 2

    *   -   Algorithm variant
        -   PWE method
        -   Hash algorithm
        -   SAE-KCK size
        -   PMK size

    *   -   Looping
        -   Looping
        -   SHA-256
        -   256
        -   256
    *   -   Hash-to-element
        -   Hash-to-element
        -   SHA-256

            SHA-384

            SHA-512
        -   256

            384

            512
        -   256

            256

            256
    *   -   Group-dependent-hash
        -   Hash-to-element
        -   SHA-256

            SHA-384

            SHA-512
        -   256

            384

            512
        -   256

            384

            512

When setting up a PAKE cipher suite to use the WPA3-SAE protocol:

*   For the looping variant, use the :code:`PSA_ALG_WPA3_SAE_FIXED(PSA_ALG_SHA_256)` algorithm.
*   For the hash-to-element variant, use the :code:`PSA_ALG_WPA3_SAE_FIXED(hash_alg)` algorithm, where ``hash_alg`` is the required hash algorithm.
*   For the group-dependent-hash variant, use the :code:`PSA_ALG_WPA3_SAE_GDH(hash_alg)` algorithm, where ``hash_alg`` is the required hash algorithm.
*   Use a PAKE primitive for the required elliptic curve or finite field group.

Valid elliptic curves and finite field groups for WPA3-SAE are defined in `[IEEE-802.11]` §12.4.4.1.
For the hash-to-element and group-dependent-hash variants, the required hash algorithm is determined from the size of the prime for the cyclic group.
See Table 12-1 in `[IEEE-802.11]` §12.4.2.

If the hash algorithm in the cipher suite is not compatible with the WPA3-SAE algorithm and PAKE primitive, the call to `psa_pake_setup()` will fail with :code:`PSA_ERROR_INVALID_ARGUMENT`.

For example, the following code creates a PAKE cipher suite for WPA3-SAE using hash-to-element over the secp256r1 elliptic curve (IANA group 19):

.. code-block:: xref

    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_WPA3_SAE_FIXED(PSA_ALG_SHA_256));
    psa_pake_cs_set_primitive(&cipher_suite,
                              PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
                                                 PSA_ECC_FAMILY_SECP_R1, 256));


.. _wpa3-sae-passwords:

WPA3-SAE password processing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WPA3-SAE defines the following two methods for deriving the password element PWE from the password:

.. list-table::
    :widths: 1 4
    :class: borderless

    *   -   Looping method
        -   Repeatedly sample candidate element values using a hash computed from the password, until a valid element is found.
            This derivation occurs as part of the authentication flow.

    *   -   Hash-to-element method
        -   Derive a password token element PT from the password, using the hash-to-curve procedure for elliptic curve groups, and a direct method for finite field groups.
            This derivation can be carried out when the network SSID and password is provisioned to the device, and PT is stored as part of the configuration.

            During authentication, PWE is derived from PT.

The hash-to-element method is recommended, as it is less vulnerable to timing-based attacks, and reduces the authentication time.

:numref:`fig-wpa3-sae-pt` illustrates the password processing required prior to the WPA3-SAE authentication flow.

.. figure:: /figure/pake/wpa3-sae-pt.*
    :name: fig-wpa3-sae-pt

    WPA3-SAE password processing

For both methods, the password must be imported as a key of type `PSA_KEY_TYPE_PASSWORD`.
The password must be encoded as defined in `[IEEE-802.11]` §12.4.3.

.. note::

    `[IEEE-802.11]` specifies that the same password is used for any configured WPA3-SAE cipher suites, and with any configured PWE-derivation methods.
    The wildcard key policy `PSA_ALG_WPA3_SAE_ANY` permits a password key to be used for any valid derivation method, and with any valid WPA3-SAE cipher suite.

.. rubric:: Looping method

Provide the password key directly to the WPA3-SAE PAKE operation in the call to `psa_pake_setup()`.

.. rubric:: Hash-to-element method

To use the hash-to-element method:

1.  A WPA3-SAE password token is derived from the WPA3-SAE password, using a key-derivation operation with the `PSA_ALG_WPA3_SAE_H2E()` algorithm.
    The `PSA_ALG_WPA3_SAE_H2E()` algorithm is parameterized by the hash used in the required WPA3-SAE cipher suite.

    The password token is output from the key-derivation operation as a key of type `PSA_KEY_TYPE_WPA3_SAE_ECC()` or `PSA_KEY_TYPE_WPA3_SAE_DH()`.
    The key type is parameterized by the elliptic curve or finite field Diffie-Hellman group used in the required WPA3-SAE cipher suite.

    The password token key must be protected at least as well as the password.

#.  Pass the password token key to the WPA3-SAE PAKE operation in the call to `psa_pake_setup()`.

.. note::

    The wildcard key policy `PSA_ALG_WPA3_SAE_ANY` permits a password token key to be used with both the `PSA_ALG_WPA3_SAE_FIXED()` and `PSA_ALG_WPA3_SAE_GDH()` PAKE algorithms.

The following steps demonstrate the derivation of a password token for use with the group-dependent-hash variant of WPA3-SAE.
The selected cipher suite in the example is IANA Group 20: ECC using secp384r1, hash function SHA-384.

1.  Allocate and initialize a key-derivation object:

    .. code-block:: xref

        psa_key_derivation_operation_t h2e_kdf = PSA_KEY_DERIVATION_OPERATION_INIT;

#.  Setup the key derivation from the WPA3-SAE password, ``password_key``, with network SSID ``ssid``:

    .. code-block:: xref

        psa_key_derivation_setup(&h2e_kdf, PSA_ALG_WPA3_SAE_H2E(PSA_ALG_SHA_384));
        psa_key_derivation_input_bytes(&h2e_kdf, PSA_KEY_DERIVATION_INPUT_SALT, ssid, ssid_len);
        psa_key_derivation_input_key(&h2e_kdf, PSA_KEY_DERIVATION_INPUT_PASSWORD, password_key);

#.  Allocate and initialize a key attributes object:

    .. code-block:: xref

        psa_key_attributes_t pt_att = PSA_KEY_ATTRIBUTES_INIT;

#.  Set the key type, size, and policy:

    .. code-block:: xref

        psa_set_key_type(&pt_att,
                         PSA_KEY_TYPE_WPA3_SAE_ECC(PSA_ECC_FAMILY_SECP_R1));
        psa_set_key_bits(&pt_att, 384);
        psa_set_key_usage_flags(&pt_att, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&pt_att, PSA_ALG_WPA3_SAE_GDH(PSA_ALG_SHA_384));

#.  Derive the password token key:

    .. code-block:: xref

        psa_key_id_t pt_key;
        psa_key_derivation_output_key(&pt_att, &h2e_kdf, &pt_key);
        psa_key_derivation_abort(&h2e_kdf);

See :secref:`wpa3-sae-keys` for details of the key types and key derivation.

.. _wpa3-sae-operation:

WPA3-SAE operation
~~~~~~~~~~~~~~~~~~

The WPA3-SAE authentication operation follows the protocol shown in :numref:`fig-wpa3-sae`.

.. figure:: /figure/pake/wpa3-sae.*
    :name: fig-wpa3-sae

    The WPA3-SAE authentication and key confirmation protocol

    The variable names *commit-scalar*, *COMMIT-ELEMENT*, *peer-commit-scalar*, and so on, are taken from the description of WPA3-SAE in `[IEEE-802.11]` §12.4.5.

Setup
^^^^^

The type of keys used to set up a PAKE multi-part operation for WPA3-SAE depends on the variant of WPA3-SAE that is required:

*   For the *Looping* variant, use a `PSA_KEY_TYPE_PASSWORD` key containing the secret password.
*   For the *Hash-to-element* and *Group-dependent-hash* variants, use a `PSA_KEY_TYPE_WPA3_SAE_ECC` or `PSA_KEY_TYPE_WPA3_SAE_DH` key that is derived from the secret password, as described in :secref:`wpa3-sae-passwords`.

WPA-SAE does not assign roles to the participants, so it is not necessary to call `psa_pake_set_role()`.

WPA-SAE requires the MAC addresses of both participants, which are provided to the PAKE multi-part operation as the user and peer identities.

WPA-SAE does not use a context.
A call to `psa_pake_set_context()` for a WPA-SAE operation will fail with :code:`PSA_ERROR_BAD_STATE`.

The following steps demonstrate the application code for STA-A in :numref:`fig-wpa3-sae`.
The flow for STA-B is the same as for STA-A, as WPA3-SAE is a balanced PAKE.

1.  To prepare a WPA3-SAE operation, initialize and set up a `psa_pake_operation_t` object by calling the following functions:

    .. code-block:: xref

        psa_pake_operation_t wpa3_sae = PSA_PAKE_OPERATION_INIT;

        psa_pake_setup(&wpa3_sae, pt_key, &cipher_suite);
        psa_pake_set_user(&wpa3_sae, &sta_a_mac, mac_length);
        psa_pake_set_peer(&wpa3_sae, &sta_b_mac, mac_length);

    See :secref:`wpa3-sae-cipher-suites` and :secref:`wpa3-sae-passwords` for details on the requirements for the cipher suite and key.

Commit
^^^^^^

2.  Exchange commitment values to establish shared secret and confirmation keys.

    The application can either extract the commitment values first, and then provide the commitment values that are received from the peer; or provide the peer inputs first, and then extract the outputs.

    To get the commitment values to send to STA-B, call:

    .. code-block:: xref

        // Get commit-scalar || COMMIT-ELEMENT
        psa_pake_output(&wpa3_sae, PSA_PAKE_STEP_COMMIT, ...);

    To provide and validate the commitment values from STA-B, call:

    .. code-block:: xref

        // Set peer-commit-scalar || PEER-COMMIT-ELEMENT
        psa_pake_input(&wpa3_sae, PSA_PAKE_STEP_COMMIT, ...);

3.  Provide the salt used for shared secret derivation, as described in `[IEEE-802.11]` §12.4.5.4.
    For Hash-to-element and Group-dependent-hash variants, this is the list of rejected groups.

    .. code-block:: xref

        // Set salt
        psa_pake_input(&wpa3_sae, PSA_PAKE_STEP_SALT, ...);

Confirm
^^^^^^^

4.  Exchange and verify confirmation values.

    WPA3-SAE can make multiple attempts to confirm key establishment, to mitigate frame losses that can occur.
    To prevent replay of confirmation messages, each attempt generates a distinct confirmation value by including a confirmation counter value.

    The application can either extract a confirmation value first, and then provide a confirmation value received from the peer; or provide the peer input first, and then extract the output.

    To get a confirmation value to send to STA-B, the confirmation counter value :math:`send{-}confirm` must be updated before extracting the combined *send-confirm* || *confirm* value, as follows:

    .. code-block:: xref

        // Set send-confirm counter
        psa_pake_input(&wpa3_sae, PSA_PAKE_STEP_CONFIRM_COUNT, ...);
        // Get combined send-confirm || confirm value
        psa_pake_output(&wpa3_sae, PSA_PAKE_STEP_CONFIRM, ...);

    To verify a confirmation value received from the peer, call:

    .. code-block:: xref

        // Set combined peer-send-confirm || peer-confirm value
        psa_pake_input(&wpa3_sae, PSA_PAKE_STEP_CONFIRM, ...);

    .. note::

        The application is permitted to request new confirmation values, or verify additional peer confirmation values, even after a peer confirmation value has been successfully verified.

Extract shared secret
^^^^^^^^^^^^^^^^^^^^^

5.  Optionally, to extract the identity of the shared secret key, PMKID, call:

    .. code-block:: xref

        // Get PMKID
        psa_pake_output(&wpa3_sae, PSA_PAKE_STEP_KEY_ID, ...);

6.  To use the shared secret, extract it as a key-derivation key.
    For example, to extract a derivation key for HKDF-SHA-256:

    .. code-block:: xref

        // Set up the key attributes
        psa_key_attributes_t att = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&att, PSA_KEY_TYPE_DERIVE);
        psa_set_key_usage_flags(&att, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&att, PSA_ALG_HKDF(PSA_ALG_SHA_256));

        // Get K_shared
        psa_key_id_t shared_key;
        psa_pake_get_shared_key(&wpa3_sae, &att, &shared_key);

The shared secret that is produced by WPA3-SAE is pseudorandom.
Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

For more information about the format of the values which are passed for each step, see :secref:`pake-steps`.

If the validation of a commitment value fails, then the corresponding call to `psa_pake_input()` for the `PSA_PAKE_STEP_COMMIT` step will return :code:`PSA_ERROR_INVALID_ARGUMENT`.
If the verification of a confirmation value fails, then the corresponding call to `psa_pake_input()` for the `PSA_PAKE_STEP_CONFIRM` step will return :code:`PSA_ERROR_INVALID_SIGNATURE`.

.. _wpa3-sae-algorithms:

WPA3-SAE algorithms
-------------------


.. macro:: PSA_ALG_WPA3_SAE_FIXED
    :definition: /* specification-defined value */

    .. summary::
        Macro to build the WPA3-SAE algorithm, with fixed-sized PMK output key.

        .. versionadded:: 1.4

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        A WPA3-SAE algorithm, for the Looping or Hash-to-element variants, parameterized by a specific hash.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is WPA3-SAE, as defined by :cite-title:`IEEE-802.11` §12.4, using the Looping or Hash-to-element password element derivation procedure, with fixed-sized PMK output key.

    The hash algorithm specified must match one of the supported WPA3-SAE cipher suites.
    See :secref:`wpa3-sae-cipher-suites`.

    The shared secret that is produced by WPA3-SAE is pseudorandom.
    Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

    See :secref:`pake-wpa3-sae` for the WPA3-SAE protocol flow and how to implement it with the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_PASSWORD`
        | `PSA_KEY_TYPE_WPA3_SAE_ECC`
        | `PSA_KEY_TYPE_WPA3_SAE_DH`

.. macro:: PSA_ALG_WPA3_SAE_GDH
    :definition: /* specification-defined value */

    .. summary::
        Macro to build the WPA3-SAE algorithm, with group-dependent size of the PMK output key.

        .. versionadded:: 1.4

    .. param:: hash_alg
        A hash algorithm: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_HASH(hash_alg)` is true.

    .. return::
        A WPA3-SAE algorithm, for the group-dependent-hash variant, parameterized by a specific hash.

        Unspecified if ``hash_alg`` is not a supported hash algorithm.

    This is WPA3-SAE, as defined by :cite-title:`IEEE-802.11` §12.4, using the hash-to-element password element derivation procedure, with group-dependent size for the PMK output key.

    The hash algorithm specified must match one of the supported WPA3-SAE cipher suites.
    See :secref:`wpa3-sae-cipher-suites`.

    The shared secret that is produced by WPA3-SAE is pseudorandom.
    Although it can be used directly as an encryption key, it is recommended to use the shared secret as an input to a key-derivation operation to produce additional cryptographic keys.

    See :secref:`pake-wpa3-sae` for the WPA3-SAE protocol flow and how to implement it with the |API|.

    .. subsection:: Compatible key types

        | `PSA_KEY_TYPE_WPA3_SAE_ECC`
        | `PSA_KEY_TYPE_WPA3_SAE_DH`

.. macro:: PSA_ALG_IS_WPA3_SAE
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a WPA3-SAE algorithm.

        .. versionadded:: 1.4

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a WPA3-SAE algorithm, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    WPA3-SAE algorithms are constructed using :code:`PSA_ALG_WPA3_SAE_FIXED(hash_alg)` or :code:`PSA_ALG_WPA3_SAE_GDH(hash_alg)`.

.. macro:: PSA_ALG_IS_WPA3_SAE_FIXED
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a WPA3-SAE algorithm with a fixed-sized output key.

        .. versionadded:: 1.4

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a WPA3-SAE algorithm with a fixed-sized output key, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    WPA3-SAE algorithms with a fixed-sized output key, are constructed using :code:`PSA_ALG_WPA3_SAE_FIXED(hash_alg)`.

.. macro:: PSA_ALG_IS_WPA3_SAE_GDH
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified algorithm is a WPA3-SAE algorithm with a group-dependent size for the output key.

        .. versionadded:: 1.4

    .. param:: alg
        An algorithm identifier: a value of type `psa_algorithm_t`.

    .. return::
        ``1`` if ``alg`` is a WPA3-SAE algorithm with a group-dependent size for the output key, ``0`` otherwise.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported PAKE algorithm identifier.

    WPA3-SAE algorithms with a group-dependent size for the output key, are constructed using :code:`PSA_ALG_WPA3_SAE_GDH(hash_alg)`.

.. macro:: PSA_ALG_WPA3_SAE_ANY
    :definition: ((psa_algorithm_t)0x0a0088ff)

    .. summary::
        A wildcard algorithm for WPA3-SAE password keys and password token keys.

        .. versionadded:: 1.4

    If a password key (key type `PSA_KEY_TYPE_PASSWORD`) specifies `PSA_ALG_WPA3_SAE_ANY` as its permitted algorithm, then the key can be used for any WPA3-SAE cipher suite with the `PSA_ALG_WPA3_SAE_H2E` key-derivation algorithm, and with the `PSA_ALG_WPA3_SAE_FIXED` PAKE algorithm.

    If a WPA3-SAE password token key specifies `PSA_ALG_WPA3_SAE_ANY` as its permitted algorithm, then the key can be used with both the `PSA_ALG_WPA3_SAE_FIXED()` and `PSA_ALG_WPA3_SAE_GDH()` PAKE algorithms.
