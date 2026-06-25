.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

|API| library
=============

.. header:: psa/crypto
    :seq: 10
    :copyright: Copyright 2018-2026 Arm Limited and/or its affiliates
    :license: Apache-2.0
    :c++:
    :guard:
    :system-include: stddef.h stdint.h
    :include: psa/error.h

    /* This file is a reference template for implementation of the
     * PSA Certified Crypto API v1.5
     */


.. _api-version:

API version
-----------

.. macro:: PSA_CRYPTO_API_VERSION_MAJOR
    :api-version: major

    .. summary::
        The major version of this implementation of the Crypto API.

.. macro:: PSA_CRYPTO_API_VERSION_MINOR
    :api-version: minor

    .. summary::
        The minor version of this implementation of the Crypto API.

.. _library-init:

Library initialization
----------------------

It is recommended that applications initialize the |API| implementation, before calling any other function, except as otherwise indicated.
This is typically achieved by calling `psa_crypto_init()`.

Some implementations provide the ability to selectively initialize a subset of the functionality, using calls to `psa_crypto_init_subsystem()`.
For example, this permits use cases such as early-stage bootloaders, that need to decrypt or authenticate firmware, where it is unnecessary to wait for a random bit generator to collect enough entropy.
In these implementations, calling `psa_crypto_init()` is equivalent to calling `psa_crypto_init_subsystem()` for all available subsystems.

Applications are permitted to call these functions more than once.
Once a subsystem is successfully initialized, subsequent calls to initialize the same subsystem are guaranteed to succeed.

If the application calls any function that returns a :code:`psa_status_t` result code before initializing the related subsystems, the following will occur:

*   If initialization of the library is essential for secure operation of the function, the implementation must return :code:`PSA_ERROR_BAD_STATE` or other appropriate error.

*   If failure to initialize the library does not compromise the security of the function, the implementation must either provide the expected result for the function, or return :code:`PSA_ERROR_BAD_STATE` or other appropriate error.

.. note::

    The following scenarios are examples where an implementation can require that the library has been initialized:

    *   A client-server implementation, in which `psa_crypto_init()`, or :code:`psa_crypto_init_subsystem(PSA_CRYPTO_SUBSYSTEM_COMMUNICATION)`, establishes the communication with the server.
        No key management or cryptographic operation can be performed until this is done.

    *   An implementation in which `psa_crypto_init()`, or :code:`psa_crypto_init_subsystem(PSA_CRYPTO_SUBSYSTEM_RANDOM)`, initializes the random bit generator.
        No operations that require output from the random bit generator can be performed until this is done.
        For example, random data, key, IV, or nonce generation; randomized signature or encryption; key encapsulation; password-authenticated key exchange; and algorithms that are implemented with blinding.

.. warning::

    The set of functions that depend on successful initialization of specific subsystems, is :scterm:`IMPLEMENTATION DEFINED`.
    Applications that rely on calling functions before initializing the library might not be portable to other implementations.

.. function:: psa_crypto_init

    .. summary::
        Library initialization.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_HARDWARE_FAILURE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_DATA_CORRUPT

    It is recommended that applications call this function before calling any other function in this module, except as otherwise indicated.

    Applications are permitted to call this function more than once. Once a call succeeds, subsequent calls are guaranteed to succeed.

    For finer control over initialization, see `psa_crypto_init_subsystem()`.

    See also :secref:`library-init`.

.. typedef:: uint32_t psa_crypto_subsystem_t

    .. summary::
        Encoding of a subsystem of the |API| implementation.

        .. versionadded:: 1.5

    This type is used to specify implementation subsystems in a call to `psa_crypto_init_subsystem()`.
    Values of this type are ``PSA_CRYPTO_SUBSYSTEM_xxx`` constants, or a bitwise-or of two or more of them.

    .. admonition:: Implementation note

        An implementation can define additional subsystem identifier values for use with `psa_crypto_init_subsystem()`.

.. macro:: PSA_CRYPTO_SUBSYSTEM_COMMUNICATION
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for the communication with the server, if this is a client that communicates with a server where the key store is located.

        .. versionadded:: 1.5

    In a client-server implementation, initializing this subsystem is necessary before any API function other than library initialization and functions accessing local data structures such as key attributes.

    In a library implementation, initializing this subsystem has no effect, and always succeeds.

.. macro:: PSA_CRYPTO_SUBSYSTEM_KEYS
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for the key store in memory.

        .. versionadded:: 1.5

    Initializing this subsystem allows creating, accessing and destroying volatile keys in the default location, that is, keys with the lifetime `PSA_KEY_LIFETIME_VOLATILE`.

    Persistent keys also require `PSA_CRYPTO_SUBSYSTEM_STORAGE`.
    Keys in other locations also require `PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS`.


.. macro:: PSA_CRYPTO_SUBSYSTEM_STORAGE
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for access to keys in storage.

        .. versionadded:: 1.5

    Initializing this subsystem and the `PSA_CRYPTO_SUBSYSTEM_KEYS` subsystem allows creating, accessing, and destroying persistent keys.

    Persistent keys in secure elements also require `PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS`.

.. macro:: PSA_CRYPTO_SUBSYSTEM_ACCELERATORS
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for :term:`cryptographic accelerator`\ s.

        .. versionadded:: 1.5

    Initializing this subsystem results in initialization of cryptographic hardware accelerator interfaces.

    Initializing this subsystem allows cryptographic operations that are implemented in an accelerator.

.. macro:: PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for :term:`secure element`\ s.

        .. versionadded:: 1.5

    Initializing this subsystem results in initialization of all cryptographic secure elements.

    Initializing this subsystem as well as `PSA_CRYPTO_SUBSYSTEM_KEYS` allows creating, accessing, and destroying keys in a secure element. That is, keys whose location is not `PSA_KEY_LOCATION_LOCAL_STORAGE`.

.. macro:: PSA_CRYPTO_SUBSYSTEM_RANDOM
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for the random generator.

        .. versionadded:: 1.5

    Initializing this subsystem initializes all entropy providers, and access to entropy sources.

    Initializing this subsystem is necessary for `psa_generate_random()`, `psa_generate_key()`, `psa_encapsulate()`, PAKE operations, and some operations using key pairs.

    It is guaranteed that the following operations do not to require this subsystem:

    *   Retrieving key attributes.
    *   Hash operations.
    *   Signature verification operations.
    *   Exporting symmetric keys.
    *   Exporting asymmetric public keys from public-key objects.

    Is it :scterm:`implementation defined` whether other operations require the initialization of this subsystem.

.. macro:: PSA_CRYPTO_SUBSYSTEM_BUILTIN_KEYS
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for access to :term:`built-in key`\ s.

        .. versionadded:: 1.5

    Initializing this subsystem as well as `PSA_CRYPTO_SUBSYSTEM_KEYS` allows access to built-in keys.

.. macro:: PSA_CRYPTO_ALL_SUBSYSTEMS
    :definition: /* implementation-defined value */

    .. summary::
        Crypto subsystem identifier for all available subsystems.

        .. versionadded:: 1.5

    Using this value in a call to `psa_crypto_init_subsystem()` is equivalent to calling `psa_crypto_init()`.

    .. admonition:: Implementation note

        It is recommended that :code:`psa_crypto_init_subsystem(PSA_CRYPTO_ALL_SUBSYSTEMS)` is implemented in a way that provides compatibility with future versions of the implementation.
        That is, addition of a new subsystem to an implementation does not require recompilation of the application so that this call will still initialize all subsystems.

.. function:: psa_crypto_init_subsystem

    .. summary::
        Partial library initialization.

        .. versionadded:: 1.5

    .. param:: psa_crypto_subsystem_t subsystem
        The subsystem, or set of subsystems, to initialize.
        This must be one of the ``PSA_CRYPTO_SUBSYSTEM_xxx`` values, one of the implementation-specific subsystem values, or a bitwise-or of them.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``subsystem`` is not a bitwise-or of one or more of the crypto subsystem identifier values.
        These values can be defined in this specification or by the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
    .. retval:: PSA_ERROR_HARDWARE_FAILURE
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_DATA_CORRUPT

    Applications may call this function on the same subsystem more than once.
    Once a call succeeds, subsequent calls with the same subsystem are guaranteed to succeed.

    Initializing a subsystem may initialize other subsystems if the implementation needs them internally.
    For example:

    *   In a typical client-server implementation, `PSA_CRYPTO_SUBSYSTEM_COMMUNICATION` is required for all other subsystems, and therefore initializing any other subsystem also initializes `PSA_CRYPTO_SUBSYSTEM_COMMUNICATION`.
    *   In a system where the random bit generator is provided by a secure element, initializing `PSA_CRYPTO_SUBSYSTEM_RANDOM` can also initialize `PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS`.
    *   In a system where protected communication with a secure element requires a per-session key, initializing `PSA_CRYPTO_SUBSYSTEM_SECURE_ELEMENTS` can also initialize `PSA_CRYPTO_SUBSYSTEM_RANDOM`.

    .. warning::
        Application code that depends on the initialization dependencies of a specific implementation or system, might not be directly portable to other implementations.

    Calling `psa_crypto_init_subsystem()` with for a subsystem that is not used by the implementation must have no effect, and return :code:`PSA_SUCCESS`.
    In effect, this is indicating that there is no further initialization required for this subsystem.

    Calling `psa_crypto_init()` is equivalent to calling :code:`psa_crypto_init_subsystem(PSA_CRYPTO_ALL_SUBSYSTEMS)`.

    See also :secref:`library-init`.

    .. note::

        Multiple subsystems can be initialized in the same call by passing a bitwise-or of ``PSA_CRYPTO_SUBSYSTEM_xxx`` values.
        If the initialization of one subsystem fails, it is unspecified whether other requested subsystems are initialized or not.
