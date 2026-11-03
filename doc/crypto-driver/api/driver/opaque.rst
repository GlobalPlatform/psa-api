..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _opaque-drivers:

Opaque drivers
--------------

Opaque drivers allow a Crypto API implementation to delegate cryptographic operations to a separate environment that might not allow exporting key material in cleartext.
The opaque driver interface is designed so that the core never inspects the representation of a key.
The opaque driver interface is designed to support two subtypes of cryptoprocessors:

*   Some cryptoprocessors do not have persistent storage for individual keys.
    The representation of a key is the key material wrapped with a master key which is located in the cryptoprocessor and never exported from it.
    The core stores this wrapped key material on behalf of the cryptoprocessor.
*   Some cryptoprocessors have persistent storage for individual keys.
    The representation of a key is an identifier such as label or slot number.
    The core stores this identifier.

.. _key-format-for-opaque-drivers:

Key format for opaque drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The format of a key for opaque drivers is an opaque blob.
The content of this blob is fully up to the driver.
The core merely stores this blob.

Note that since the core stores the key context blob as it is in memory, it must only contain data that is meaningful after a reboot.
In particular, it must not contain any pointers or transient handles.

The ``"key_context"`` property in the `driver description <driver-description-top-level-element>` specifies how to calculate the size of the key context as a function of the key type and size.
This is an object with the following properties:

*   ``"base_size"`` (integer or string, optional): this many bytes are included in every key context.
    If omitted, this value defaults to 0.
*   ``"key_pair_size"`` (integer or string, optional): this many bytes are included in every key context for a key pair.
    If omitted, this value defaults to 0.
*   ``"public_key_size"`` (integer or string, optional): this many bytes are included in every key context for a public key.
    If omitted, this value defaults to 0.
*   ``"symmetric_factor"`` (integer or string, optional): every key context for a symmetric key includes this many times the key size.
    If omitted, this value defaults to 0.
*   ``"store_public_key"`` (boolean, optional): If specified and true, for a key pair, the key context includes space for the public key.
    If omitted or false, no additional space is added for the public key.
*   ``"size_function"`` (string, optional): the name of a function that returns the number of bytes that the driver needs in a key context for a key.
    This may be a pointer to function.
    This must be a C identifier; more complex expressions are not permitted.
    If the core uses this function, it supersedes all the other properties except for ``"builtin_key_size"`` (where applicable, if present).
*   ``"builtin_key_size"`` (integer or string, optional): If specified, this overrides all other methods (including the ``"size_function"`` entry point) to determine the size of the key context for `built-in keys <built-in-keys>`.
    This allows drivers to efficiently represent application keys as wrapped key material, but built-in keys by an internal identifier that takes up less space.

The integer properties must be C language constants.
A typical value for ``"base_size"`` is ``sizeof(acme_key_context_t)`` where ``acme_key_context_t`` is a type defined in a driver header file.

Size of a dynamically allocated key context
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the core supports dynamic allocation for the key context and chooses to use it, and the driver specification includes the ``"size_function"`` property, the size of the key context is at least

.. code-block::

    size_function(key_type, key_bits)

where ``size_function`` is the function named in the ``"size_function"`` property, ``key_type`` is the key type and ``key_bits`` is the key size in bits.
The prototype of the size function is

.   code-block::

    size_t size_function(psa_key_type_t key_type, size_t key_bits);

Size of a statically allocated key context
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the core does not support dynamic allocation for the key context or chooses not to use it, or if the driver specification does not include the ``"size_function"`` property, the size of the key context for a key of type ``key_type`` and of size ``key_bits`` bits is:

*   For a key pair (``PSA_KEY_TYPE_IS_KEY_PAIR(key_type)`` is true):

    .. code-block::

        base_size + key_pair_size + public_key_overhead

    where ``public_key_overhead = PSA_EXPORT_PUBLIC_KEY_MAX_SIZE(key_type, key_bits)`` if the ``"store_public_key"`` property is true and ``public_key_overhead = 0`` otherwise.

*   For a public key (``PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type)`` is true):

    .. code-block::

        base_size + public_key_size

*   For a symmetric key (not a key pair or public key):

    .. code-block::

        base_size + symmetric_factor * key_bytes

    where ``key_bytes = ((key_bits + 7) / 8)`` is the key size in bytes.

Key context size for a secure element with storage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the key is stored in the secure element and the driver only needs to store a label for the key, use ``"base_size"`` as the size of the label plus any other metadata that the driver needs to store, and omit the other properties.

If the key is stored in the secure element, but the secure element does not store the public part of a key pair and cannot recompute it on demand, additionally use the ``"store_public_key"`` property with the value ``true``.
Note that this only influences the size of the key context: the driver code must copy the public key to the key context and retrieve it on demand in its ``export_public_key`` entry point.

Key context size for a secure element without storage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the key is stored in wrapped form outside the secure element, and the wrapped form of the key plus any metadata has up to *N* bytes of overhead, use *N* as the value of the ``"base_size"`` property and set the ``"symmetric_factor"`` property to 1.
Set the ``"key_pair_size"`` and ``"public_key_size"`` properties appropriately for the largest supported key pair and the largest supported public key respectively.

.. _key-management-with-opaque-drivers:

Key management with opaque drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Opaque drivers may provide the following key management entry points:

*   ``"export_key"``: called by ``psa_export_key()``, or by ``psa_copy_key()`` when copying a key from or to a different `location <lifetimes-and-locations>`, or `as a fallback for key derivation <key-derivation-driver-dispatch-logic>`.
*   ``"export_public_key"``: called by the core to obtain the public key of a key pair.
    The core may call this entry point at any time to obtain the public key, which can be for ``psa_export_public_key()`` but also at other times, including during a cryptographic operation that requires the public key such as a call to ``psa_verify_message()`` on a key pair object.
*   ``"import_key"``: called by ``psa_import_key()``, or by ``psa_copy_key()`` when copying a key from another location.
*   ``"generate_key"``: called by ``psa_generate_key()``.
*   ``"key_derivation_output_key"``: called by ``psa_key_derivation_output_key()``.
*   ``"copy_key"``: called by ``psa_copy_key()`` when copying a key within the same `location <lifetimes-and-locations>`.
*   ``"get_builtin_key"``: called by functions that access a key to retrieve information about a `built-in key <built-in-keys>`.

In addition, secure elements that store the key material internally must provide the following two entry points:

*   ``"allocate_key"``: called by ``psa_import_key()``, ``psa_generate_key()``, ``psa_key_derivation_output_key()`` or ``psa_copy_key()`` before creating a key in the location of this driver.
*   ``"destroy_key"``: called by ``psa_destroy_key()``.

Key creation in a secure element without storage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the key creation process for secure elements that do not store the key material.
The driver must obtain a wrapped form of the key material which the core will store.
A driver for such a secure element has no ``"allocate_key"`` or ``"destroy_key"`` entry point.

When creating a key with an opaque driver which does not have an ``"allocate_key"`` or ``"destroy_key"`` entry point:

1.  The core allocates memory for the key context.
2.  The core calls the driver's import, generate, derive or copy entry point.
3.  The core saves the resulting wrapped key material and any other data that the key context may contain.

To destroy a key, the core simply destroys the wrapped key material, without invoking driver code.

.. _key-management-in-a-secure-element-with-storage:

Key management in a secure element with storage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the key creation and key destruction processes for secure elements that have persistent storage for the key material.
A driver for such a secure element has two mandatory entry points:

*   ``"allocate_key"``: this function obtains an internal identifier for the key.
    This may be, for example, a unique label or a slot number.
*   ``"destroy_key"``: this function invalidates the internal identifier and destroys the associated key material.

These functions have the following prototypes for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_allocate_key(const psa_key_attributes_t *attributes,
                                   uint8_t *key_buffer,
                                   size_t key_buffer_size);
    psa_status_t acme_destroy_key(const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size);

When creating a persistent key with an opaque driver which has an ``"allocate_key"`` entry point:

1.  The core calls the driver's ``"allocate_key"`` entry point.
    This function typically allocates an internal identifier for the key without modifying the state of the secure element and stores the identifier in the key context.
    This function should not modify the state of the secure element.
    It may modify the copy of the persistent state of the driver in memory.

#.  The core saves the key context to persistent storage.

#.  The core calls the driver's key creation entry point.

#.  The core saves the updated key context to persistent storage.

If a failure occurs after the ``"allocate_key"`` step but before the call to the second driver entry point, the core will do one of the following:

*   Fail the creation of the key without indicating this to the driver.
    This can happen, in particular, if the device loses power immediately after the key allocation entry point returns.
*   Call the driver's ``"destroy_key"`` entry point.

To destroy a key, the core calls the driver's ``"destroy_key"`` entry point.

Note that the key allocation and destruction entry points must not rely solely on the key identifier in the key attributes to identify a key.
Some implementations of the Crypto API store keys on behalf of multiple clients, and different clients may use the same key identifier to designate different keys.
The manner in which the core distinguishes keys that have the same identifier but are part of the key namespace for different clients is implementation-dependent and is not accessible to drivers.
Some typical strategies to allocate an internal key identifier are:

*   Maintain a set of free slot numbers which is stored either in the secure element or in the driver's persistent storage.
    To allocate a key slot, find a free slot number, mark it as occupied and store the number in the key context.
    When the key is destroyed, mark the slot number as free.
*   Maintain a monotonic counter with a practically unbounded range in the secure element or in the driver's persistent storage.
    To allocate a key slot, increment the counter and store the current value in the key context.
    Destroying a key does not change the counter.

TODO: explain constraints on how the driver updates its persistent state for resilience

TODO: some of the above doesn't apply to volatile keys

Key creation entry points in opaque drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The key creation entry points have the following prototypes for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_import_key(const psa_key_attributes_t *attributes,
                                 const uint8_t *data,
                                 size_t data_length,
                                 uint8_t *key_buffer,
                                 size_t key_buffer_size,
                                 size_t *key_buffer_length,
                                 size_t *bits);
    psa_status_t acme_generate_key(const psa_key_attributes_t *attributes,
                                   uint8_t *key_buffer,
                                   size_t key_buffer_size,
                                   size_t *key_buffer_length);

If the driver has an `"allocate_key" entry point <key-management-in-a-secure-element-with-storage>`, the core calls the ``"allocate_key"`` entry point with the same attributes on the same key buffer before calling the key creation entry point.

TODO: derivation, copy

Key export entry points in opaque drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The key export entry points have the following prototypes for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_export_key(const psa_key_attributes_t *attributes,
                                 const uint8_t *key_buffer,
                                 size_t key_buffer_size,
                                 uint8_t *data,
                                 size_t data_size,
                                 size_t *data_length);
    psa_status_t acme_export_public_key(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        uint8_t *data,
                                        size_t data_size,
                                        size_t *data_length);

The core will only call ``acme_export_public_key`` on a private key.
Drivers implementers may choose to store the public key in the key context buffer or to recalculate it on demand.
If the key context includes the public key, it needs to have an adequate size; see :secref:`key-format-for-opaque-drivers`.

The core guarantees that the size of the output buffer (``data_size``) is sufficient to export any key with the given attributes.
The driver must set ``*data_length`` to the exact size of the exported key.

.. _opaque-driver-persistent-state:

Opaque driver persistent state
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The core maintains persistent state on behalf of an opaque driver.
This persistent state consists of a single byte array whose size is given by the ``"persistent_state_size"`` property in the `driver description <driver-description-top-level-element>`.

The core loads the persistent state in memory before it calls the driver's `init entry point <driver-initialization>`.
It is adjusted to match the size declared by the driver, in case a driver upgrade changes the size:

*   The first time the driver is loaded on a system, the persistent state is all-bits-zero.
*   If the stored persistent state is smaller than the declared size, the core pads the persistent state with all-bits-zero at the end.
*   If the stored persistent state is larger than the declared size, the core truncates the persistent state to the declared size.

The core provides the following callback functions, which an opaque driver may call while it is processing a call from the driver:

.. code-block::

    psa_status_t psa_crypto_driver_get_persistent_state(uint_8_t **persistent_state_ptr);
    psa_status_t psa_crypto_driver_commit_persistent_state(size_t from, size_t length);

``psa_crypto_driver_get_persistent_state`` sets ``*persistent_state_ptr`` to a pointer to the first byte of the persistent state.
This pointer remains valid during a call to a driver entry point.
Once the entry point returns, the pointer is no longer valid.
The core guarantees that calls to ``psa_crypto_driver_get_persistent_state`` within the same entry point return the same address for the persistent state, but this address may change between calls to an entry point.

``psa_crypto_driver_commit_persistent_state`` updates the persistent state in persistent storage.
Only the portion at byte offsets ``from`` inclusive to ``from + length`` exclusive is guaranteed to be updated; it is unspecified whether changes made to other parts of the state are taken into account.
The driver must call this function after updating the persistent state in memory and before returning from the entry point, otherwise it is unspecified whether the persistent state is updated.

The core will not update the persistent state in storage while an entry point is running except when the entry point calls ``psa_crypto_driver_commit_persistent_state``.
It may update the persistent state in storage after an entry point returns.

In a multithreaded environment, the driver may only call these two functions from the thread that is executing the entry point.

.. _built-in-keys:

Built-in keys
^^^^^^^^^^^^^

Opaque drivers may declare built-in keys.
Built-in keys can be accessed, but not created, through the Crypto API.

A built-in key is identified by its location and its **slot number**.
Drivers that support built-in keys must provide a ``"get_builtin_key"`` entry point to retrieve the key data and metadata.
The core calls this entry point when it needs to access the key, typically because the application requested an operation on the key.
The core may keep information about the key in cache, and successive calls to access the same slot number should return the same data.
This entry point has the following prototype:

.. code-block::

    psa_status_t acme_get_builtin_key(psa_drv_slot_number_t slot_number,
                                      psa_key_attributes_t *attributes,
                                      uint8_t *key_buffer,
                                      size_t key_buffer_size,
                                      size_t *key_buffer_length);

If this function returns ``PSA_SUCCESS`` or ``PSA_ERROR_BUFFER_TOO_SMALL``, it must fill ``attributes`` with the attributes of the key (except for the key identifier).
On success, this function must also fill ``key_buffer`` with the key context.

On entry, ``psa_get_key_lifetime(attributes)`` is the location at which the driver was declared and a persistence level with which the platform is attempting to register the key.
The driver entry point may choose to change the lifetime (``psa_set_key_lifetime(attributes, lifetime)``) of the reported key attributes to one with the same location but a different persistence level, in case the driver has more specific knowledge about the actual persistence level of the key which is being retrieved.
For example, if a driver knows it cannot delete a key, it may override the persistence level in the lifetime to ``PSA_KEY_PERSISTENCE_READ_ONLY``.
The standard attributes other than the key identifier and lifetime have the value conveyed by ``PSA_KEY_ATTRIBUTES_INIT``.

The output parameter ``key_buffer`` points to a writable buffer of ``key_buffer_size`` bytes.
If the driver has a `"builtin_key_size" property <key-format-for-opaque-drivers>` property, ``key_buffer_size`` has this value, otherwise ``key_buffer_size`` has the value determined from the key type and size.

Typically, for a built-in key, the key context is a reference to key material that is kept inside the secure element, similar to the format returned by `"allocate_key" <key-management-in-a-secure-element-with-storage>`.
A driver may have built-in keys even if it doesn't have an ``"allocate_key"`` entry point.

This entry point may return the following status values:

*   ``PSA_SUCCESS``: the requested key exists, and the output parameters ``attributes`` and ``key_buffer`` contain the key metadata and key context respectively, and ``*key_buffer_length`` contains the length of the data written to ``key_buffer``.
*   ``PSA_ERROR_BUFFER_TOO_SMALL``: ``key_buffer_size`` is insufficient.
    In this case, the driver must pass the key's attributes in ``*attributes``.
    In particular, ``get_builtin_key(slot_number, &attributes, NULL, 0)`` is a way for the core to obtain the key's attributes.
*   ``PSA_ERROR_DOES_NOT_EXIST``: the requested key does not exist.
*   Other error codes such as ``PSA_ERROR_COMMUNICATION_FAILURE`` or ``PSA_ERROR_HARDWARE_FAILURE`` indicate a transient or permanent error.

The core will pass authorized requests to destroy a built-in key to the `"destroy_key" <key-management-in-a-secure-element-with-storage>` entry point if there is one.
If built-in keys must not be destroyed, it is up to the driver to reject such requests.
