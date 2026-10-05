:orphan:

.. SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _open-issues:

Open questions
--------------

Value representation
~~~~~~~~~~~~~~~~~~~~

Integers
^^^^^^^^

It would be better if there was a uniform requirement on integer values.
Do they have to be JSON integers? C preprocessor integers (which could be e.g. a macro defined in some header file)? C compile-time constants (allowing ``sizeof``)?

This choice is partly driven by the use of the values, so they might not be uniform.
Note that if the value can be zero and it's plausible that the core would want to statically allocate an array of the given size, the core needs to know whether the value is 0 so that it could use code like

.. code-block::

    #if ACME_FOO_SIZE != 0
        uint8_t foo[ACME_FOO_SIZE];
    #endif

Driver declarations
~~~~~~~~~~~~~~~~~~~

Declaring driver entry points
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core may want to provide declarations for the driver entry points so that it can compile code using them.
At the time of writing this paragraph, the driver headers must define types but there is no obligation for them to declare functions.
The core knows what the function names and argument types are, so it can generate prototypes.

It should be ok for driver functions to be function-like macros or function pointers.

Driver location values
^^^^^^^^^^^^^^^^^^^^^^

How does a driver author decide which location values to use? It should be possible to combine drivers from different sources.
Use the same vendor assignment as for PSA services?

Can the driver assembly process generate distinct location values as needed? This can be convenient, but it's also risky: if you upgrade a device, you need the location values to be the same between builds.

The current plan is to maintain a registry of vendors and assign a location namespace to each vendor.
Parts of the namespace would be reserved for implementations and integrators.

Multiple transparent drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When multiple transparent drivers implement the same mechanism, which one is called? The first one? The last one? Unspecified? Or is this an error (excluding capabilities with fallback enabled)?

The current choice is that the first one is used, which allows having a preference order on drivers, but may mask integration errors.

Driver function interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~

Driver function parameter conventions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Should 0-size buffers be guaranteed to have a non-null pointers?

Should drivers really have to cope with overlap?

Should the core guarantee that the output buffer size has the size indicated by the applicable buffer size macro (which may be an overestimation)?

.. _key-derivation-inputs-and-buffer-ownership:

Key derivation inputs and buffer ownership
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Why is ``psa_crypto_driver_key_derivation_get_input_bytes`` a copy, rather than giving a pointer?

The main reason is to avoid complex buffer ownership.
A driver entry point does not own memory after the entry point return.
This is generally necessary because an API function does not own memory after the entry point returns.
In the case of key derivation inputs, this could be relaxed because the driver entry point is making callbacks to the core: these functions could return a pointer that is valid until the driver entry point returns, which would allow the driver to process the data immediately (e.g. hash it rather than copy it).

Partial computations in drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Substitution points
^^^^^^^^^^^^^^^^^^^

Earlier drafts of the driver interface had a concept of *substitution points*: places in the calculation where a driver may be called.
Some hardware doesn't do the whole calculation, but only the “main” part.
This goes both for transparent and opaque drivers.
Some common examples:

*   A processor that performs the RSA exponentiation, but not the padding.
    The driver should be able to leverage the padding code in the core.
*   A processor that performs a block cipher operation only for a single block, or only in ECB mode, or only in CTR mode.
    The core would perform the block mode (CBC, CTR, CCM, ...).

This concept, or some other way to reuse portable code such as specifying inner functions like ``psa_rsa_pad`` in the core, should be added to the specification.

Key management
~~~~~~~~~~~~~~

Mixing drivers in key derivation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

How does ``psa_key_derivation_output_key`` work when the extraction part and the expansion part use different drivers?

Public key calculation
^^^^^^^^^^^^^^^^^^^^^^

ECC key pairs are represented as the private key value only.
The public key needs to be calculated from that.
Both transparent drivers and opaque drivers provide a function to calculate the public key (``"export_public_key"``).

The specification doesn't mention when the public key might be calculated.
The core may calculate it on creation, on demand, or anything in between.
Opaque drivers have a choice of storing the public key in the key context or calculating it on demand and can convey whether the core should store the public key with the ``"store_public_key"`` property.
Is this good enough or should the specification include non-functional requirements?

Symmetric key validation with transparent drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Should the entry point be called for symmetric keys as well?

Support for custom import formats
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:secref:`driver-entry-points-for-key-management` states that the input to ``"import_key"`` can be an implementation-defined format.
Is this a good idea? It reduces driver portability, since a core that accepts a custom format would not work with a driver that doesn't accept this format.
On the other hand, if a driver accepts a custom format, the core should let it through because the driver presumably handles it more efficiently (in terms of speed and code size) than the core could.

Allowing custom formats also causes a problem with import: the core can't know the size of the key representation until it knows the bit-size of the key, but determining the bit-size of the key is part of the job of the ``"import_key"`` entry point.
For standard key types, this could plausibly be an issue for RSA private keys, where an implementation might accept a custom format that omits the CRT parameters (or that omits *d*).

Opaque drivers
~~~~~~~~~~~~~~

Opaque driver persistent state
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The driver is allowed to update the state at any time.
Is this ok?

An example use case for updating the persistent state at arbitrary times is to renew a key that is used to encrypt communications between the application processor and the secure element.

``psa_crypto_driver_get_persistent_state`` does not identify the calling driver, so the driver needs to remember which driver it's calling.
This may require a thread-local variable in a multithreaded core.
Is this ok?

.. _cooked-key-derivation-issue:

Open questions around cooked key derivation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``"derive_key"`` is not a clear name.
Can we use a better one?

For the ``"derive_key"`` entry point, how does the core choose ``input_length``? Doesn't the driver know better? Should there be a driver entry point to determine the length, or should there be a callback that allows the driver to retrieve the input? Note that for some key types, it's impossible to predict the amount of input in advance, because it depends on some complex calculation or even on random data, e.g. if doing a randomized pseudo-primality test.
However, for all key types except RSA, the specification mandates how the key is derived, which practically dictates how the pseudorandom key stream is consumed.
So it's probably ok.

.. _fallback-for-key-derivation-in-opaque-drivers:

Fallback for key derivation in opaque drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Should `dispatch to an opaque driver <key-derivation-driver-dispatch-logic>` allow fallback, so that if ``"key_derivation_setup"`` returns ``PSA_ERROR_NOT_SUPPORTED`` then the core exports the key from the secure element instead?

Should the `"key_derivation_output_key" <key-derivation-driver-outputs>` capability indicate which key types the driver can derive? How should fallback work? For example, consider a secure element that implements HMAC, HKDF and ECDSA, and that can derive an HMAC key from HKDF without exporting intermediate material but can only import or randomly generate ECC keys.
How does this driver convey that it can't derive an ECC key with HKDF, but it can let the core do this and import the resulting key?

Randomness
~~~~~~~~~~

Input to ``"add_entropy"``
^^^^^^^^^^^^^^^^^^^^^^^^^^

Should the input to the `"add_entropy" entry point <entropy-injection>` be a full-entropy buffer (with data from all entropy sources already mixed), raw entropy direct from the entropy sources, or give the core a choice?

*   Raw data: drivers must implement entropy mixing.
    ``"add_entropy"`` needs an extra parameter to indicate the amount of entropy in the data.
    The core must not do any conditioning.
*   Choice: drivers must implement entropy mixing.
    ``"add_entropy"`` needs an extra parameter to indicate the amount of entropy in the data.
    The core may do conditioning if it wants, but doesn't have to.
*   Full entropy: drivers don't need to do entropy mixing.

Flags for ``"get_entropy"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Are the `entropy collection flags <entropy-collection-flags>` well-chosen?

Random generator instantiations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

May the core instantiate a random generation context more than once? In other words, can there be multiple objects of type ``acme_random_context_t``?

Functionally, one RNG is as good as any.
If the core wants some parts of the system to use a deterministic generator for reproducibility, it can't use this interface anyway, since the RNG is not necessarily deterministic.
However, for performance on multiprocessor systems, a multithreaded core could prefer to use one RNG instance per thread.
