..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _driver-description:

Driver description
------------------

.. _driver-description-syntax:

Driver description syntax
~~~~~~~~~~~~~~~~~~~~~~~~~

The concrete syntax for a driver description file is JSON.

In addition to the properties described here, any JSON object may have a property called ``"_comment"`` of type string, which will be ignored.

Crypto API core implementations may support additional properties.
Such properties must use names consisting of the implementation's name, a slash, and additional characters.
For example, the Yoyodyne implementation may use property names such as ``"yoyodyne/foo"`` and ``"yoyodyne/widgets/girth"``.

.. _driver-description-top-level-element:

Driver description top-level element
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A driver description is a JSON object containing the following properties:

*   ``"prefix"`` (mandatory, string).
    This must be a valid, non-empty prefix for a C identifier.
    All the types and functions provided by the driver have a name that starts with this prefix unless overridden with a ``"name"`` element in the applicable capability as described below.
*   ``"type"`` (mandatory, string).
    One of ``"transparent"`` or ``"opaque"``.
*   ``"headers"`` (optional, array of strings).
    A list of header files.
    These header files must define the types, macros and constants referenced by the driver description.
    They may declare the entry point functions, but this is not required.
    They may include other PSA headers and standard headers of the platform.
    Whether they may include other headers is implementation-specific.
    If omitted, the list of headers is empty.
    The header files must be present at the specified location relative to a directory on the compiler's include path when compiling glue code between the core and the drivers.
*   ``"capabilities"`` (mandatory, array of `capabilities <driver-description-capability>`).
    A list of **capabilities**.
    Each capability describes a family of functions that the driver implements for a certain class of cryptographic mechanisms.
*   ``"key_context"`` (not permitted for transparent drivers, mandatory for opaque drivers): information about the `representation of keys <key-format-for-opaque-drivers>`.
*   ``"persistent_state_size"`` (not permitted for transparent drivers, optional for opaque drivers, integer or string).
    The size in bytes of the `persistent state of the driver <opaque-driver-persistent-state>`.
    This may be either a non-negative integer or a C constant expression of type ``size_t``.
*   ``"location"`` (not permitted for transparent drivers, optional for opaque drivers, integer or string).
    The `location value <lifetimes-and-locations>` for which this driver is invoked.
    In other words, this determines the lifetimes for which the driver is invoked.
    This may be either a non-negative integer or a C constant expression of type ``psa_key_location_t``.

.. _driver-description-capability:

Driver description capability
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _capability-syntax:

Capability syntax
^^^^^^^^^^^^^^^^^

A capability declares a family of functions that the driver implements for a certain class of cryptographic mechanisms.
The capability specifies which key types and algorithms are covered and the names of the types and functions that implement it.

A capability is a JSON object containing the following properties:

*   ``"entry_points"`` (mandatory, list of strings).
    Each element is the name of a `driver entry point <driver-entry-points>` or driver entry point family.
    An entry point is a function defined by the driver.
    If specified, the core will invoke this capability of the driver only when performing one of the specified operations.
    The driver must implement all the specified entry points, as well as the types if applicable.
*   ``"algorithms"`` (optional, list of strings).
    Each element is an `algorithm specification <algorithm-specifications>`.
    If specified, the core will invoke this capability of the driver only when performing one of the specified algorithms.
    If omitted, the core will invoke this capability for all applicable algorithms.
*   ``"key_types"`` (optional, list of strings).
    Each element is a `key type specification <key-type-specifications>`.
    If specified, the core will invoke this capability of the driver only for operations involving a key with one of the specified key types.
    If omitted, the core will invoke this capability of the driver for all applicable key types.
*   ``"key_sizes"`` (optional, list of integers).
    If specified, the core will invoke this capability of the driver only for operations involving a key with one of the specified key sizes.
    If omitted, the core will invoke this capability of the driver for all applicable key sizes.
    Key sizes are expressed in bits.
*   ``"names"`` (optional, object).
    A mapping from entry point names described by the ``"entry_points"`` property, to the name of the C function in the driver that implements the corresponding function.
    If a function is not listed here, name of the driver function that implements it is the driver's prefix followed by an underscore (``_``) followed by the function name.
    If this property is omitted, it is equivalent to an empty object (so each entry point *suffix* is implemented by a function called *prefix*\ ``_``\ *suffix*).
*   ``"fallback"`` (optional for transparent drivers, not permitted for opaque drivers, boolean).
    If present and true, the driver may return ``PSA_ERROR_NOT_SUPPORTED``, in which case the core should call another driver or use built-in code to perform this operation.
    If absent or false, the driver is expected to fully support the mechanisms described by this capability.
    See the section :secref:`fallback` for more information.

Capability semantics
^^^^^^^^^^^^^^^^^^^^

When the Crypto API implementation performs a cryptographic mechanism, it invokes available driver entry points as described in the section :secref:`driver-entry-points`.

A driver is considered available for a cryptographic mechanism that invokes a given entry point if all of the following conditions are met:

*   The driver specification includes a capability whose ``"entry_points"`` list either includes the entry point or includes an entry point family that includes the entry point.
*   If the mechanism involves an algorithm:

    *   either the capability does not have an ``"algorithms"`` property;
    *   or the value of the capability's ``"algorithms"`` property includes an `algorithm specification <algorithm-specifications>` that matches this algorithm.
*   If the mechanism involves a key:

    *   either the key is transparent (its location is ``PSA_KEY_LOCATION_LOCAL_STORAGE``) and the driver is transparent;
    *   or the key is opaque (its location is not ``PSA_KEY_LOCATION_LOCAL_STORAGE``) and the driver is an opaque driver whose location is the key's location.
*   If the mechanism involves a key:

    *   either the capability does not have a ``"key_types"`` property;
    *   or the value of the capability's ``"key_types"`` property includes a `key type specification <key-type-specifications>` that matches this algorithm.
*   If the mechanism involves a key:

    *   either the capability does not have a ``"key_sizes"`` property;
    *   or the value of the capability's ``"key_sizes"`` property includes the key's size.

If a driver includes multiple applicable capabilities for a given combination of entry point, algorithm, key type and key size, and all the capabilities map the entry point to the same function name, the driver is considered available for this cryptographic mechanism.
If a driver includes multiple applicable capabilities for a given combination of entry point, algorithm, key type and key size, and at least two of these capabilities map the entry point to the different function names, the driver specification is invalid.

If multiple transparent drivers have applicable capabilities for a given combination of entry point, algorithm, key type and key size, the first matching driver in the `specification list <driver-description-list>` is invoked.
If the capability has `fallback <fallback>` enabled and the first driver returns ``PSA_ERROR_NOT_SUPPORTED``, the next matching driver is invoked, and so on.

If multiple opaque drivers have the same location, the list of driver specifications is invalid.

Capability examples
^^^^^^^^^^^^^^^^^^^

Example 1: the following capability declares that the driver can perform deterministic ECDSA signatures (but not signature verification) using any hash algorithm and any curve that the core supports.
If the prefix of this driver is ``"acme"``, the function that performs the signature is called ``acme_sign_hash``.

.. code-block::

    {
        "entry_points": ["sign_hash"],
        "algorithms": ["PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_ANY_HASH)"],
    }

Example 2: the following capability declares that the driver can perform deterministic ECDSA signatures using SHA-256 or SHA-384 with a SECP256R1 or SECP384R1 private key (with either hash being possible in combination with either curve).
If the prefix of this driver is ``"acme"``, the function that performs the signature is called ``acme_sign_hash``.

.. code-block::

    {
        "entry_points": ["sign_hash"],
        "algorithms": ["PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256)",
                       "PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_384)"],
        "key_types": ["PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)"],
        "key_sizes": [256, 384]
    }

Algorithm and key specifications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _algorithm-specifications:

Algorithm specifications
^^^^^^^^^^^^^^^^^^^^^^^^

An algorithm specification is a string consisting of a ``PSA_ALG_xxx`` macro that specifies a cryptographic algorithm or an algorithm wildcard policy defined by the Crypto API.
If the macro takes arguments, the string must have the syntax of a C macro call and each argument must be an algorithm specification or a decimal or hexadecimal literal with no suffix, depending on the expected type of argument.

Spaces are optional after commas.
Whether other whitespace is permitted is implementation-specific.

Valid examples:

.. code-block::

    PSA_ALG_SHA_256
    PSA_ALG_HMAC(PSA_ALG_SHA_256)
    PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256))
    PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH)

.. _key-type-specifications:

Key type specifications
^^^^^^^^^^^^^^^^^^^^^^^

An algorithm specification is a string consisting of a ``PSA_KEY_TYPE_xxx`` macro that specifies a key type defined by the Crypto API.
If the macro takes an argument, the string must have the syntax of a C macro call and each argument must be the name of a constant of suitable type (curve or group).

The name ``_`` may be used instead of a curve or group to indicate that the capability concerns all curves or groups.

Valid examples:

.. code-block::

    PSA_KEY_TYPE_AES
    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)
    PSA_KEY_TYPE_ECC_KEY_PAIR(_)
