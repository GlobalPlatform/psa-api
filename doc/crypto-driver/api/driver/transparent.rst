..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _transparent-drivers:

Transparent drivers
-------------------

.. _key-format-for-transparent-drivers:

Key format for transparent drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The format of a key for transparent drivers is the same as in applications.
Refer to the documentation in the *Key format* sub-section of each key type in `§9.2 Key types <https://arm-software.github.io/psa-api/crypto/1.3/api/keys/types.html#key-types>`__ in the Crypto API specification.
For custom key types defined by an implementation, refer to the documentation of that implementation.

.. _key-management-with-transparent-drivers:

Key management with transparent drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Transparent drivers may provide the following key management entry points:

*   `"import_key" <key-import-with-transparent-drivers>`: called by ``psa_import_key()``, only when importing a key pair or a public key (key such that ``PSA_KEY_TYPE_IS_ASYMMETRIC`` is true).
*   ``"generate_key"``: called by ``psa_generate_key()``, only when generating a key pair (key such that ``PSA_KEY_TYPE_IS_KEY_PAIR`` is true).
*   ``"key_derivation_output_key"``: called by ``psa_key_derivation_output_key()``, only when deriving a key pair (key such that ``PSA_KEY_TYPE_IS_KEY_PAIR`` is true).
*   ``"export_public_key"``: called by the core to obtain the public key of a key pair.
    The core may call this function at any time to obtain the public key, which can be for ``psa_export_public_key()`` but also at other times, including during a cryptographic operation that requires the public key such as a call to ``psa_verify_message()`` on a key pair object.

Transparent drivers are not involved when exporting, copying or destroying keys, or when importing, generating or deriving symmetric keys.

.. _key-import-with-transparent-drivers:

Key import with transparent drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As discussed in `the general section about key management entry points <driver-entry-points-for-key-management>`, the key import entry points has the following prototype for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_import_key(const psa_key_attributes_t *attributes,
                                 const uint8_t *data,
                                 size_t data_length,
                                 uint8_t *key_buffer,
                                 size_t key_buffer_size,
                                 size_t *key_buffer_length,
                                 size_t *bits);

This entry point has several roles:

1.  Parse the key data in the input buffer ``data``.
    The driver must support the export format for the key types that the entry point is declared for.
    It may support additional formats as specified in the description of `psa_import_key() <https://arm-software.github.io/psa-api/crypto/1.3/api/keys/management.html#c.psa_import_key>`__ in the Crypto API specification.
2.  Validate the key data.
    The necessary validation is described in :secref:`key-validation`.
3.  `Determine the key size <key-size-determination-on-import>` and output it through ``*bits``.
4.  Copy the validated key data from ``data`` to ``key_buffer``.
    The output must be in the canonical format documented for the key type: see the *Key format* sub-section of the key type in `§9.2 Key types <https://arm-software.github.io/psa-api/crypto/1.3/api/keys/types.html#key-types>`__, so if the input is not in this format, the entry point must convert it.

.. _random-generation-entry-points:

Random generation entry points
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A transparent driver may provide an operation family that can be used as a cryptographic random number generator.
The random generation mechanism must obey the following requirements:

*   The random output must be of cryptographic quality, with a uniform distribution.
    Therefore, if the random generator includes an entropy source, this entropy source must be fed through a CSPRNG (cryptographically secure pseudo-random number generator).
*   Random generation is expected to be fast.
    (If a device can provide entropy but is slow at generating random data, declare it as an `entropy driver <entropy-collection-entry-point>` instead.)
*   The random generator should be able to incorporate entropy provided by an outside source.
    If it isn't, the random generator can only be used if it's the only entropy source on the platform.
    (A random generator peripheral can be declared as an `entropy source <entropy-collection-entry-point>` instead of a random generator; this way the core will combine it with other entropy sources.)
*   The random generator may either be deterministic (in the sense that it always returns the same data when given the same entropy inputs) or non-deterministic (including its own entropy source).
    In other words, this interface is suitable both for PRNG (pseudo-random number generator, also known as DRBG (deterministic random bit generator)) and for NRBG (non-deterministic random bit generator).

If no driver implements the random generation entry point family, the core provides an unspecified random generation mechanism.

This operation family requires the following type, entry points and parameters (TODO: where exactly are the parameters in the JSON structure?):

*   Type ``"random_context_t"``: the type of a random generation context.
*   ``"init_random"`` (entry point, optional): if this function is present, `the core calls it once <random-generator-initialization>` after allocating a ``"random_context_t"`` object.
*   ``"add_entropy"`` (entry point, optional): the core calls this function to `inject entropy <entropy-injection>`.
    This entry point is optional if the driver is for a peripheral that includes an entropy source of its own, however `random generator drivers without entropy injection <random-generator-drivers-without-entropy-injection>` have limited portability since they can only be used on platforms with no other entropy source.
    This entry point is mandatory if ``"initial_entropy_size"`` is nonzero.
*   ``"get_random"`` (entry point, mandatory): the core calls this function whenever it needs to `obtain random data <the-get_random-entry-point>`.
*   ``"initial_entropy_size"`` (integer, mandatory): the minimum number of bytes of entropy that the core must supply before the driver can output random data.
    This can be ``0`` if the driver is for a peripheral that includes an entropy source of its own.
*   ``"reseed_entropy_size"`` (integer, optional): the minimum number of bytes of entropy that the core should supply via `"add_entropy" <entropy-injection>` when the driver runs out of entropy.
    This value is also a hint for the size to supply if the core makes additional calls to ``"add_entropy"``, for example to enforce prediction resistance.
    If omitted, the core should pass an amount of entropy corresponding to the expected security strength of the device (for example, pass 32 bytes of entropy when reseeding to achieve a security strength of 256 bits).
    If specified, the core should pass the larger of ``"reseed_entropy_size"`` and the amount corresponding to the security strength.

Random generation is not parametrized by an algorithm.
The choice of algorithm is up to the driver.

.. _random-generator-initialization:

Random generator initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``"init_random"`` entry point has the following prototype for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_init_random(acme_random_context_t *context);

The core calls this entry point once after allocating a random generation context.
Initially, the context object is all-bits-zero.

If a driver does not have an ``"init_random"`` entry point, the context object passed to the first call to ``"add_entropy"`` or ``"get_random"`` will be all-bits-zero.

.. _entropy-injection:

Entropy injection
^^^^^^^^^^^^^^^^^

The ``"add_entropy"`` entry point has the following prototype for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_add_entropy(acme_random_context_t *context,
                                  const uint8_t *entropy,
                                  size_t entropy_size);

The semantics of the parameters is as follows:

*   ``context``: a random generation context.
    On the first call to ``"add_entropy"``, this object has been initialized by a call to the driver's ``"init_random"`` entry point if one is present, and to all-bits-zero otherwise.
*   ``entropy``: a buffer containing full-entropy data to seed the random generator.
    “Full-entropy” means that the data is uniformly distributed and independent of any other observable quantity.
*   ``entropy_size``: the size of the ``entropy`` buffer in bytes.
    It is guaranteed to be at least ``1``, but it may be smaller than the amount of entropy that the driver needs to deliver random data, in which case the core will call the ``"add_entropy"`` entry point again to supply more entropy.

The core calls this function to supply entropy to the driver.
The driver must mix this entropy into its internal state.
The driver must mix the whole supplied entropy, even if there is more than what the driver requires, to ensure that all entropy sources are mixed into the random generator state.
The driver may mix additional entropy of its own.

The core may call this function at any time.
For example, to enforce prediction resistance, the core can call ``"add_entropy"`` immediately after each call to ``"get_random"``.
The core must call this function in two circumstances:

*   Before the first call to the ``"get_random"`` entry point, to supply ``"initial_entropy_size"`` bytes of entropy.
*   After a call to the ``"get_random"`` entry point returns less than the required amount of random data, to supply at least ``"reseed_entropy_size"`` bytes of entropy.

When the driver requires entropy, the core can supply it with one or more successive calls to the ``"add_entropy"`` entry point.
If the required entropy size is zero, the core does not need to call ``"add_entropy"``.

Combining entropy sources with a random generation driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section provides guidance on combining one or more `entropy sources <entropy-collection-entry-point>` (each having a ``"get_entropy"`` entry point) with a random generation driver (with an ``"add_entropy"`` entry point).

Note that ``"get_entropy"`` returns data with an estimated amount of entropy that is in general less than the buffer size.
The core must apply a mixing algorithm to the output of ``"get_entropy"`` to obtain full-entropy data.

For example, the core may use a simple mixing scheme based on a pseudorandom function family (*F*:sub:`k`) with an *E*-bit output where *E* = 8 entropy\ :sub:`size` and entropy\ :sub:`size` is the desired amount of entropy in bytes (typically the random driver's ``"initial_entropy_size"`` property for the initial seeding and the ``"reseed_entropy_size"`` property for subsequent reseeding).
The core calls the ``"get_entropy"`` points of the available entropy drivers, outputting a string *s*:sub:`i` and an entropy estimate *e*:sub:`i` on the *i*\ th call.
It does so until the total entropy estimate *e*:sub:`1` + *e*:sub:`2` + ... + *e*:sub:`n` is at least *E*.
The core then calculates *F*:sub:`k`\ (0) where *k* = *s*:sub:`1` || *s*:sub:`2` || ... || *s*:sub:`n`.
This value is a string of entropy\ :sub:`size`, and since (*F*:sub:`k`) is a pseudorandom function family, *F*:sub:`k`\ (0) is uniformly distributed over strings of entropy\ :sub:`size` bytes.
Therefore *F*:sub:`k`\ (0) is a suitable value to pass to ``"add_entropy"``.

Note that the mechanism above is only given as an example.
Implementations may choose a different mechanism, for example involving multiple pools or intermediate compression functions.

.. _random-generator-drivers-without-entropy-injection:

Random generator drivers without entropy injection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Random generator drivers should have the capability to inject additional entropy through the ``"add_entropy"`` entry point.
This ensures that the random generator depends on all the entropy sources that are available on the platform.
A driver where a call to ``"add_entropy"`` does not affect the state of the random generator is not compliant with this specification.

However, a driver may omit the ``"add_entropy"`` entry point.
This limits the driver's portability: implementations of the Crypto API specification may reject drivers without an ``"add_entropy"`` entry point, or only accept such drivers in certain configurations.
In particular, the ``"add_entropy"`` entry point is required if:

*   the implementation of the Crypto API includes an entropy source that is outside the driver; or
*   the core saves random data in persistent storage to be preserved across platform resets.

.. _the-get_random-entry-point:

The ``"get_random"`` entry point
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``"get_random"`` entry point has the following prototype for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_get_random(acme_random_context_t *context,
                                 uint8_t *output,
                                 size_t output_size,
                                 size_t *output_length);

The semantics of the parameters is as follows:

*   ``context``: a random generation context.
    If the driver's ``"initial_entropy_size"`` property is nonzero, the core must have called ``"add_entropy"`` at least once with a total of at least ``"initial_entropy_size"`` bytes of entropy before it calls ``"get_random"``.
    Alternatively, if the driver's ``"initial_entropy_size"`` property is zero and the core did not call ``"add_entropy"``, or if the driver has no ``"add_entropy"`` entry point, the core must have called ``"init_random"`` if present, and otherwise the context is all-bits zero.
*   ``output``: on success (including partial success), the first ``*output_length`` bytes of this buffer contain cryptographic-quality random data.
    The output is not used on error.
*   ``output_size``: the size of the ``output`` buffer in bytes.
*   ``*output_length``: on success (including partial success), the number of bytes of random data that the driver has written to the ``output`` buffer.
    This is preferably ``output_size``, but the driver is allowed to return less data if it runs out of entropy as described below.
    The core sets this value to 0 on entry.
    The value is not used on error.

The driver may return the following status codes:

*   ``PSA_SUCCESS``: the ``output`` buffer contains ``*output_length`` bytes of cryptographic-quality random data.
    Note that this may be less than ``output_size``; in this case the core should call the driver's ``"add_entropy"`` method to supply at least ``"reseed_entropy_size"`` bytes of entropy before calling ``"get_random"`` again.
*   ``PSA_ERROR_INSUFFICIENT_ENTROPY``: the core must supply additional entropy by calling the ``"add_entropy"`` entry point with at least ``"reseed_entropy_size"`` bytes.
*   ``PSA_ERROR_NOT_SUPPORTED``: the random generator is not available.
    This is only permitted if the driver specification for random generation has the `fallback property <fallback>` enabled.
*   Other error codes such as ``PSA_ERROR_COMMUNICATION_FAILURE`` or ``PSA_ERROR_HARDWARE_FAILURE`` indicate a transient or permanent error.

.. _fallback:

Fallback
~~~~~~~~

Sometimes cryptographic accelerators only support certain cryptographic mechanisms partially.
The capability description language allows specifying some restrictions, including restrictions on key sizes, but it cannot cover all the possibilities that may arise in practice.
Furthermore, it may be desirable to deploy the same binary image on different devices, only some of which have a cryptographic accelerators.
For these purposes, a transparent driver can declare that it only supports a `capability <driver-description-capability>` partially, by setting the capability's ``"fallback"`` property to true.

If a transparent driver entry point is part of a capability which has a true ``"fallback"`` property and returns ``PSA_ERROR_NOT_SUPPORTED``, the core will call the next transparent driver that supports the mechanism, if there is one.
The core considers drivers in the order given by the `driver description list <driver-description-list>`.

If all the available drivers have fallback enabled and return ``PSA_ERROR_NOT_SUPPORTED``, the core will perform the operation using built-in code.
As soon as a driver returns any value other than ``PSA_ERROR_NOT_SUPPORTED`` (``PSA_SUCCESS`` or a different error code), this value is returned to the application, without attempting to call any other driver or built-in code.

If a transparent driver entry point is part of a capability where the ``"fallback"`` property is false or omitted, the core should not include any other code for this capability, whether built in or in another transparent driver.
