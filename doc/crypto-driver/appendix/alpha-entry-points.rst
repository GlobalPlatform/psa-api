..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _driver-entry-points:

Driver entry points
-------------------

Overview of driver entry points
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Drivers define functions, each of which implements an aspect of a capability of a driver, such as a cryptographic operation, a part of a cryptographic operation, or a key management action.
These functions are called the **entry points** of the driver.
Most driver entry points correspond to a particular function in the Crypto API.
For example, if a call to ``psa_sign_hash()`` is dispatched to a driver, it invokes the driver's ``sign_hash`` function.

All driver entry points return a status of type ``psa_status_t`` which should use the status codes documented for PSA services in general and for the Crypto API.
In particular: ``PSA_SUCCESS`` indicates that the function succeeded, and ``PSA_ERROR_xxx`` values indicate that an error occurred.

The signature of a driver entry point generally looks like the signature of the Crypto API that it implements, with some modifications.
This section gives an overview of modifications that apply to whole classes of entry points.
Refer to the reference section for each entry point or entry point family for details.

*   For entry points that operate on an existing key, the ``psa_key_id_t`` parameter is replaced by a sequence of three parameters that describe the key:

    1.  ``const psa_key_attributes_t *attributes``: the key attributes.
    2.  ``const uint8_t *key_buffer``: a key material or key context buffer.
    3.  ``size_t key_buffer_size``: the size of the key buffer in bytes.

    For transparent drivers, the key buffer contains the key material, in the same format as defined for ``psa_export_key()`` and ``psa_export_public_key()`` in the Crypto API.
    For opaque drivers, the content of the key buffer is entirely up to the driver.

*   For entry points that involve a multi-part operation, the operation state type (``psa_XXX_operation_t``) is replaced by a driver-specific operation state type (*prefix*\ ``_XXX_operation_t``).

*   For entry points that are involved in key creation, the ``psa_key_id_t *`` output parameter is replaced by a sequence of parameters that convey the key context:

    1.  ``uint8_t *key_buffer``: a buffer for the key material or key context.
    2.  ``size_t key_buffer_size``: the size of the key buffer in bytes.
    3.  ``size_t *key_buffer_length``: the length of the data written to the key buffer in bytes.

Some entry points are grouped in families that must be implemented as a whole.
If a driver supports an entry point family, it must provide all the entry points in the family.

Drivers can also have entry points related to random generation.
A transparent driver can provide a `random generation interface <random-generation-entry-points>`.
Separately, transparent and opaque drivers can have `entropy collection entry points <entropy-collection-entry-point>`.

General considerations on driver entry point parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Buffer parameters for driver entry points obey the following conventions:

*   An input buffer has the type ``const uint8_t *`` and is immediately followed by a parameter of type ``size_t`` that indicates the buffer size.
*   An output buffer has the type ``uint8_t *`` and is immediately followed by a parameter of type ``size_t`` that indicates the buffer size.
    A third parameter of type ``size_t *`` is provided to report the actual length of the data written in the buffer if the function succeeds.
*   An in-out buffer has the type ``uint8_t *`` and is immediately followed by a parameter of type ``size_t`` that indicates the buffer size.
    In-out buffers are only used when the input and the output have the same length.

Buffers of size 0 may be represented with either a null pointer or a non-null pointer.

Input buffers and other input-only parameters (``const`` pointers) may be in read-only memory.
Overlap is possible between input buffers, and between an input buffer and an output buffer, but not between two output buffers or between a non-buffer parameter and another parameter.

Driver entry points for single-part cryptographic operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following driver entry points perform a cryptographic operation in one shot (single-part operation):

*   ``"hash_compute"`` (transparent drivers only): calculation of a hash.
    Called by ``psa_hash_compute()`` and ``psa_hash_compare()``.
    To verify a hash with ``psa_hash_compare()``, the core calls the driver's ``"hash_compute"`` entry point and compares the result with the reference hash value.
*   ``"mac_compute"``: calculation of a MAC.
    Called by ``psa_mac_compute()`` and possibly ``psa_mac_verify()``.
    To verify a mac with ``psa_mac_verify()``, the core calls an applicable driver's ``"mac_verify"`` entry point if there is one, otherwise the core calls an applicable driver's ``"mac_compute"`` entry point and compares the result with the reference MAC value.
*   ``"mac_verify"``: verification of a MAC.
    Called by ``psa_mac_verify()``.
    This entry point is mainly useful for drivers of secure elements that verify a MAC without revealing the correct MAC.
    Although transparent drivers may implement this entry point in addition to ``"mac_compute"``, it is generally not useful because the core can call the ``"mac_compute"`` entry point and compare with the expected MAC value.
*   ``"cipher_encrypt"``: unauthenticated symmetric cipher encryption.
    Called by ``psa_cipher_encrypt()``.
*   ``"cipher_decrypt"``: unauthenticated symmetric cipher decryption.
    Called by ``psa_cipher_decrypt()``.
*   ``"aead_encrypt"``: authenticated encryption with associated data.
    Called by ``psa_aead_encrypt()``.
*   ``"aead_decrypt"``: authenticated decryption with associated data.
    Called by ``psa_aead_decrypt()``.
*   ``"asymmetric_encrypt"``: asymmetric encryption.
    Called by ``psa_asymmetric_encrypt()``.
*   ``"asymmetric_decrypt"``: asymmetric decryption.
    Called by ``psa_asymmetric_decrypt()``.
*   ``"sign_hash"``: signature of an already calculated hash.
    Called by ``psa_sign_hash()`` and possibly ``psa_sign_message()``.
    To sign a message with ``psa_sign_message()``, the core calls an applicable driver's ``"sign_message"`` entry point if there is one, otherwise the core calls an applicable driver's ``"hash_compute"`` entry point followed by an applicable driver's ``"sign_hash"`` entry point.
*   ``"verify_hash"``: verification of an already calculated hash.
    Called by ``psa_verify_hash()`` and possibly ``psa_verify_message()``.
    To verify a message with ``psa_verify_message()``, the core calls an applicable driver's ``"verify_message"`` entry point if there is one, otherwise the core calls an applicable driver's ``"hash_compute"`` entry point followed by an applicable driver's ``"verify_hash"`` entry point.
*   ``"sign_message"``: signature of a message.
    Called by ``psa_sign_message()``.
*   ``"verify_message"``: verification of a message.
    Called by ``psa_verify_message()``.
*   ``"key_agreement"``: key agreement without a subsequent key derivation.
    Called by ``psa_raw_key_agreement()`` and possibly ``psa_key_derivation_key_agreement()``.

Driver entry points for multi-part operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

General considerations on multi-part operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The entry points that implement each step of a multi-part operation are grouped into a family.
A driver that implements a multi-part operation must define all of the entry points in this family as well as a type that represents the operation context.
The lifecycle of a driver operation context is similar to the lifecycle of an API operation context:

1.  The core initializes operation context objects to either all-bits-zero or to logical zero (``{0}``), at its discretion.
#.  The core calls the ``xxx_setup`` entry point for this operation family.
    If this fails, the core destroys the operation context object without calling any other driver entry point on it.
#.  The core calls other entry points that manipulate the operation context object, respecting the constraints.
#.  If any entry point fails, the core calls the driver's ``xxx_abort`` entry point for this operation family, then destroys the operation context object without calling any other driver entry point on it.
#.  If a “finish” entry point fails, the core destroys the operation context object without calling any other driver entry point on it.
    The finish entry points are: *prefix*\ ``_mac_sign_finish``, *prefix*\ ``_mac_verify_finish``, *prefix*\ ``_cipher_finish``, *prefix*\ ``_aead_finish``, *prefix*\ ``_aead_verify``.

If a driver implements a multi-part operation but not the corresponding single-part operation, the core calls the driver's multipart operation entry points to perform the single-part operation.

Multi-part operation entry point family ``"hash_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This family corresponds to the calculation of a hash in multiple steps.

This family applies to transparent drivers only.

This family requires the following type and entry points:

*   Type ``"hash_operation_t"``: the type of a hash operation context.
    It must be possible to copy a hash operation context byte by byte, therefore hash operation contexts must not contain any embedded pointers (except pointers to global data that do not change after the setup step).
*   ``"hash_setup"``: called by ``psa_hash_setup()``.
*   ``"hash_update"``: called by ``psa_hash_update()``.
*   ``"hash_finish"``: called by ``psa_hash_finish()`` and ``psa_hash_verify()``.
*   ``"hash_abort"``: called by all multi-part hash functions of the Crypto API.

To verify a hash with ``psa_hash_verify()``, the core calls the driver's *prefix*\ ``_hash_finish`` entry point and compares the result with the reference hash value.

For example, a driver with the prefix ``"acme"`` that implements the ``"hash_multipart"`` entry point family must define the following type and entry points (assuming that the capability does not use the ``"names"`` property to declare different type and entry point names):

.. code-block::

    typedef ... acme_hash_operation_t;
    psa_status_t acme_hash_setup(acme_hash_operation_t *operation,
                                 psa_algorithm_t alg);
    psa_status_t acme_hash_update(acme_hash_operation_t *operation,
                                  const uint8_t *input,
                                  size_t input_length);
    psa_status_t acme_hash_finish(acme_hash_operation_t *operation,
                                  uint8_t *hash,
                                  size_t hash_size,
                                  size_t *hash_length);
    psa_status_t acme_hash_abort(acme_hash_operation_t *operation);

Operation family ``"mac_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Operation family ``"mac_verify_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Operation family ``"cipher_encrypt_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Operation family ``"cipher_decrypt_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Operation family ``"aead_encrypt_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Operation family ``"aead_decrypt_multipart"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

Driver entry points for key derivation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Key derivation is more complex than other multipart operations for several reasons:

*   There are multiple inputs and outputs.
*   Multiple drivers can be involved.
    This happens when an operation combines a key agreement and a subsequent symmetric key derivation, each of which can have independent drivers.
    This also happens when deriving an asymmetric key, where processing the secret input and generating the key output might involve different drivers.
*   When multiple drivers are involved, they are not always independent: if the secret input is managed by an opaque driver, it might not allow the core to retrieve the intermediate output and pass it to another driver.
*   The involvement of an opaque driver cannot be determined as soon as the operation is set up (since ``psa_key_derivation_setup()`` does not determine the key input).

.. _key-derivation-driver-dispatch-logic:

Key derivation driver dispatch logic
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core decides whether to dispatch a key derivation operation to a driver based on the location associated with the input step ``PSA_KEY_DERIVATION_INPUT_SECRET``.

1.  If this step is passed via ``psa_key_derivation_input_key()`` for a key in a secure element:

    *   If the driver for this secure element implements the ``"key_derivation"`` family for the specified algorithm, the core calls that driver's ``"key_derivation_setup"`` and subsequent entry points.
        Note that for all currently specified algorithms, the key type for the secret input does not matter.
    *   Otherwise the core calls the secure element driver's `"export_key" <key-management-with-opaque-drivers>` entry point.
2.  Otherwise (`or on fallback? <fallback-for-key-derivation-in-opaque-drivers>`), if there is a transparent driver for the specified algorithm, the core calls that driver's ``"key_derivation_setup"`` and subsequent entry points.
3.  Otherwise, or on fallback, the core uses its built-in implementation.

Summary of entry points for the operation family ``"key_derivation"``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A key derivation driver has the following entry points:

*   ``"key_derivation_setup"`` (mandatory): always the first entry point to be called.
    This entry point provides the `initial inputs <key-derivation-driver-initial-inputs>`.
    See :secref:`key-derivation-driver-setup`.
*   ``"key_derivation_input_step"`` (mandatory if the driver supports a key derivation algorithm with long inputs, otherwise ignored): provide an extra input for the key derivation.
    This entry point is only mandatory in drivers that support algorithms that have extra inputs.
    See :secref:`key-derivation-driver-long-inputs`.
*   ``"key_derivation_output_bytes"`` (mandatory): derive cryptographic material and output it.
    See :secref:`key-derivation-driver-outputs`.
*   ``"key_derivation_output_key"``, ``"key_derivation_verify_bytes"``, ``"key_derivation_verify_key"`` (optional, opaque drivers only): derive key material which remains inside the same secure element.
    See :secref:`key-derivation-driver-outputs`.
*   ``"key_derivation_set_capacity"`` (mandatory for opaque drivers that implement ``"key_derivation_output_key"`` for “cooked”, i.e. non-raw-data key types; ignored for other opaque drivers; not permitted for transparent drivers): update the capacity policy on the operation.
    See :secref:`key-derivation-driver-operation-capacity`.
*   ``"key_derivation_abort"`` (mandatory): always the last entry point to be called.

For naming purposes, here and in the following subsection, this specification takes the example of a driver with the prefix ``"acme"`` that implements the ``"key_derivation"`` entry point family with a capability that does not use the ``"names"`` property to declare different type and entry point names.
Such a driver must implement the following type and functions, as well as the entry points listed above and described in the following subsections:

.. code-block::

    typedef ... acme_key_derivation_operation_t;
    psa_status_t acme_key_derivation_abort(acme_key_derivation_operation_t *operation);

.. _key-derivation-driver-initial-inputs:

Key derivation driver initial inputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core conveys the initial inputs for a key derivation via an opaque data structure of type ``psa_crypto_driver_key_derivation_inputs_t``.

.. code-block::

    typedef ... psa_crypto_driver_key_derivation_inputs_t; // implementation-specific type

A driver receiving an argument that points to a ``psa_crypto_driver_key_derivation_inputs_t`` can retrieve its contents by calling one of the type-specific functions below.
To determine the correct function, the driver can call ``psa_crypto_driver_key_derivation_get_input_type()``.

.. code-block::

    enum psa_crypto_driver_key_derivation_input_type_t {
        PSA_KEY_DERIVATION_INPUT_TYPE_INVALID = 0,
        PSA_KEY_DERIVATION_INPUT_TYPE_OMITTED,
        PSA_KEY_DERIVATION_INPUT_TYPE_BYTES,
        PSA_KEY_DERIVATION_INPUT_TYPE_KEY,
        PSA_KEY_DERIVATION_INPUT_TYPE_INTEGER,
        // Implementations may add other values, and may freely choose the
        // numerical values for each identifer except as explicitly specified
        // above.
    };
    psa_crypto_driver_key_derivation_input_type_t psa_crypto_driver_key_derivation_get_input_type(
        const psa_crypto_driver_key_derivation_inputs_t *inputs,
        psa_key_derivation_step_t step);

The function ``psa_crypto_driver_key_derivation_get_input_type()`` determines whether a given step is present and how to access its value:

*   ``PSA_KEY_DERIVATION_INPUT_TYPE_INVALID``: the step is invalid for the algorithm of the operation that the inputs are for.
*   ``PSA_KEY_DERIVATION_INPUT_TYPE_OMITTED``: the step is optional for the algorithm of the operation that the inputs are for, and has been omitted.
*   ``PSA_KEY_DERIVATION_INPUT_TYPE_BYTES``: the step is valid and present and is a transparent byte string.
    Call ``psa_crypto_driver_key_derivation_get_input_size()`` to obtain the size of the input data.
    Call ``psa_crypto_driver_key_derivation_get_input_bytes()`` to make a copy of the input data (design note: `why a copy? <key-derivation-inputs-and-buffer-ownership>`).
*   ``PSA_KEY_DERIVATION_INPUT_TYPE_KEY``: the step is valid and present and is a byte string passed via a key object.
    Call ``psa_crypto_driver_key_derivation_get_input_key()`` to obtain a pointer to the key context.
*   ``PSA_KEY_DERIVATION_INPUT_TYPE_INTEGER``: the step is valid and present and is an integer.
    Call ``psa_crypto_driver_key_derivation_get_input_integer()`` to retrieve the integer value.

.. code-block::

    psa_status_t psa_crypto_driver_key_derivation_get_input_size(
        const psa_crypto_driver_key_derivation_inputs_t *inputs,
        psa_key_derivation_step_t step,
        size_t *size);
    psa_status_t psa_crypto_driver_key_derivation_get_input_bytes(
        const psa_crypto_driver_key_derivation_inputs_t *inputs,
        psa_key_derivation_step_t step,
        uint8_t *buffer, size_t buffer_size, size_t *buffer_length);
    psa_status_t psa_crypto_driver_key_derivation_get_input_key(
        const psa_crypto_driver_key_derivation_inputs_t *inputs,
        psa_key_derivation_step_t step,
        const psa_key_attributes_t *attributes,
        uint8_t** p_key_buffer, size_t *key_buffer_size);
    psa_status_t psa_crypto_driver_key_derivation_get_input_integer(
        const psa_crypto_driver_key_derivation_inputs_t *inputs,
        psa_key_derivation_step_t step,
        uint64_t *value);

The get-data functions take the following parameters:

*   The first parameter ``inputs`` must be a pointer passed by the core to a key derivation driver setup entry point which has not returned yet.
*   The ``step`` parameter indicates the input step whose content the driver wants to retrieve.
*   On a successful invocation of ``psa_crypto_driver_key_derivation_get_input_size``, the core sets ``*size`` to the size of the specified input in bytes.
*   On a successful invocation of ``psa_crypto_driver_key_derivation_get_input_bytes``, the core fills the first *N* bytes of ``buffer`` with the specified input and sets ``*buffer_length`` to *N*, where *N* is the length of the input in bytes.
    The value of ``buffer_size`` must be at least *N*, otherwise this function fails with the status ``PSA_ERROR_BUFFER_TOO_SMALL``.
*   On a successful invocation of ``psa_crypto_driver_key_derivation_get_input_key``, the core sets ``*key_buffer`` to a pointer to a buffer containing the key context and ``*key_buffer_size`` to the size of the key context in bytes.
    The key context buffer remains valid for the duration of the driver entry point.
    If the driver needs to access the key context after the current entry point returns, it must make a copy of the key context.
*   On a successful invocation of ``psa_crypto_driver_key_derivation_get_input_integer``, the core sets ``*value`` to the value of the specified input.

These functions can return the following statuses:

*   ``PSA_SUCCESS``: the call succeeded and the requested value has been copied to the output parameter (``size``, ``buffer``, ``value`` or ``p_key_buffer``) and if applicable the size of the value has been written to the applicable parameter (``buffer_length``, ``key_buffer_size``).
*   ``PSA_ERROR_DOES_NOT_EXIST``: the input step is valid for this particular algorithm, but it is not part of the initial inputs.
    This is not a fatal error.
    The driver will receive the input later as a `long input <key-derivation-driver-long-inputs>`.
*   ``PSA_ERROR_INVALID_ARGUMENT``: the input type is not compatible with this function or was omitted.
    Call ``psa_crypto_driver_key_derivation_get_input_type()`` to find out the actual type of this input step.
    This is not a fatal error and the driver can, for example, subsequently call the appropriate function on the same step.
*   ``PSA_ERROR_BUFFER_TOO_SMALL`` (``psa_crypto_driver_key_derivation_get_input_bytes`` only): the output buffer is too small.
    This is not a fatal error and the driver can, for example, subsequently call the same function again with a larger buffer.
    Call ``psa_crypto_driver_key_derivation_get_input_size`` to obtain the required size.
*   The core may return other errors such as ``PSA_ERROR_CORRUPTION_DETECTED`` or ``PSA_ERROR_COMMUNICATION_FAILURE`` to convey implementation-specific error conditions.
    Portable drivers should treat such conditions as fatal errors.

.. _key-derivation-driver-setup:

Key derivation driver setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A key derivation driver must implement the following entry point:

.. code-block::

    psa_status_t acme_key_derivation_setup(
        acme_key_derivation_operation_t *operation,
        psa_algorithm_t alg,
        const psa_crypto_driver_key_derivation_inputs_t *inputs);

*   ``operation`` is a zero-initialized operation object.
*   ``alg`` is the algorithm for the key derivation operation.
    It does not include a key agreement component.
*   ``inputs`` is an opaque pointer to the `initial inputs <key-derivation-driver-initial-inputs>` for the key derivation.

.. _key-derivation-driver-long-inputs:

Key derivation driver long inputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some key derivation algorithms take long inputs which it would not be practical to pass in the `initial inputs <key-derivation-driver-initial-inputs>`.
A driver that implements a key derivation algorithm that takes such inputs must provide a ``"key_derivation_input_step"`` entry point.
The core calls this entry point for all the long inputs after calling ``"acme_key_derivation_setup"``.
A long input step may be fragmented into multiple calls of ``psa_key_derivation_input_bytes()``, and the core may reassemble or refragment those fragments before passing them to the driver.
Calls to this entry point for different step values occur in an unspecified order and may be interspersed.

.. code-block::

    psa_status_t acme_key_derivation_input_step(
        acme_key_derivation_operation_t *operation,
        psa_key_derivation_step_t step,
        const uint8_t *input, size_t input_length);

At the time of writing, no standard key derivation algorithm has long inputs.
It is likely that such algorithms will be added in the future.

.. _key-derivation-driver-operation-capacity:

Key derivation driver operation capacity
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core keeps track of an operation's capacity and enforces it.
The core guarantees that it will not request output beyond the capacity of the operation, with one exception: opaque drivers that support `"key_derivation_output_key" <key-derivation-driver-outputs>`, i.e. for key types where the derived key material is not a direct copy of the key derivation's output stream.

Such drivers must enforce the capacity limitation and must return ``PSA_ERROR_INSUFFICIENT_CAPACITY`` from any output request that exceeds the operation's capacity.
Such drivers must provide the following entry point:

.. code-block::

    psa_status_t acme_key_derivation_set_capacity(
        acme_key_derivation_operation_t *operation,
        size_t capacity);

``capacity`` is guaranteed to be less or equal to any value previously set through this entry point, and is guaranteed not to be ``PSA_KEY_DERIVATION_UNLIMITED_CAPACITY``.

If this entry point has not been called, the operation has an unlimited capacity.

.. _key-derivation-driver-outputs:

Key derivation driver outputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A key derivation driver must provide the following entry point:

.. code-block::

    psa_status_t acme_key_derivation_output_bytes(
        acme_key_derivation_operation_t *operation,
        uint8_t *output, size_t length);

An opaque key derivation driver may provide the following entry points:

.. code-block::

    psa_status_t acme_key_derivation_output_key(
        const psa_key_attributes_t *attributes,
        acme_key_derivation_operation_t *operation,
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);
    psa_status_t acme_key_derivation_verify_bytes(
        acme_key_derivation_operation_t *operation,
        const uint8_t *expected output, size_t length);
    psa_status_t acme_key_derivation_verify_key(
        acme_key_derivation_operation_t *operation,
        uint8_t *key_buffer, size_t key_buffer_size);

The core calls a key derivation driver's output entry point when the application calls ``psa_key_derivation_output_bytes()``, ``psa_key_derivation_output_key()``, ``psa_key_derivation_verify_bytes()`` or ``psa_key_derivation_verify_key()``.

If the key derivation's ``PSA_KEY_DERIVATION_INPUT_SECRET`` input is in a secure element and the derivation operation is handled by that secure element, the core performs the following steps:

*   For a call to ``psa_key_derivation_output_key()``:

    1.  If the derived key is in the same secure element, if the driver has an ``"key_derivation_output_key"`` entry point, call that entry point.
        If the driver has no such entry point, or if that entry point returns ``PSA_ERROR_NOT_SUPPORTED``, continue with the following steps, otherwise stop.
    #.  If the driver's capabilities indicate that its ``"import_key"`` entry point does not support the derived key, stop and return ``PSA_ERROR_NOT_SUPPORTED``.
    #.  Otherwise proceed as for ``psa_key_derivation_output_bytes()``, then import the resulting key material.

*   For a call to ``psa_key_derivation_verify_key()``:

    1.  If the driver has a ``"key_derivation_verify_key"`` entry point, call it and stop.
    #.  Call the driver's ``"export_key"`` entry point on the key object that contains the expected value, then proceed as for ``psa_key_derivation_verify_bytes()``.

*   For a call to ``psa_key_derivation_verify_bytes()``:

    1.  If the driver has a ``"key_derivation_verify_bytes"`` entry point, call that entry point on the expected output, then stop.
    #.  Otherwise, proceed as for ``psa_key_derivation_output_bytes()``, and compare the resulting output to the expected output inside the core.

*   For a call to ``psa_key_derivation_output_bytes()``:

    1.  Call the ``"key_derivation_output_bytes"`` entry point.
        The core may call this entry point multiple times to implement a single call from the application when deriving a cooked (non-raw) key as described below, or if the output size exceeds some implementation limit.

If the key derivation operation is not handled by an opaque driver as described above, the core calls the ``"key_derivation_output_bytes"`` from the applicable transparent driver (or multiple drivers in succession if fallback applies).
In some cases, the core then calls additional entry points in the same or another driver:

*   For a call to ``psa_key_derivation_output_key()`` for some key types, the core calls a transparent driver's ``"derive_key"`` entry point.
    See :secref:`transparent-cooked-key-derivation`.
*   For a call to ``psa_key_derivation_output_key()`` where the derived key is in a secure element, call that secure element driver's ``"import_key"`` entry point.

.. _transparent-cooked-key-derivation:

Transparent cooked key derivation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Key derivation is said to be *raw* for some key types, where the key material of a derived (8\ *n*)-bit key consists of the next *n* bytes of output from the key derivation, and *cooked* otherwise.
When deriving a raw key, the core only calls the driver's ``"output_bytes"`` entry point, except when deriving a key entirely inside a secure element as described in :secref:`key-derivation-driver-outputs`.
When deriving a cooked key, the core calls a transparent driver's ``"derive_key"`` entry point if available.

A capability for cooked key derivation contains the following properties (this is not a subset of `the usual entry point properties <capability-syntax>`):

*   ``"entry_points"`` (mandatory, list of strings).
    Must be ``["derive_key"]``.
*   ``"derived_types"`` (mandatory, list of strings).
    Each element is a `key type specification <key-type-specifications>`.
    This capability only applies when deriving a key of the specified type.
*   ``"derived_sizes"`` (optional, list of integers).
    Each element is a size for the derived key, in bits.
    This capability only applies when deriving a key of the specified sizes.
    If absent, this capability applies to all sizes for the specified types.
*   ``"memory"`` (optional, boolean).
    If present and true, the driver must define a type ``"derive_key_memory_t"`` and the core will allocate an object of that type as specified below.
*   ``"names"`` (optional, object).
    A mapping from entry point names to C function and type names, as usual.
*   ``"fallback"`` (optional, boolean).
    If present and true, the driver may return ``PSA_ERROR_NOT_SUPPORTED`` if it only partially supports the specified mechanism, as usual.

A transparent driver with the prefix ``"acme"`` that implements cooked key derivation must provide the following type and function:

.. code-block::

    typedef ... acme_derive_key_memory_t; // only if the "memory" property is true
    psa_status_t acme_derive_key(
        const psa_key_attributes_t *attributes,
        const uint8_t *input, size_t input_length,
        acme_derive_key_memory_t *memory, // if the "memory" property is false: void*
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

*   ``attributes`` contains the attributes of the specified key.
    Note that only the key type and the bit-size are guaranteed to be set.
*   ``input`` is a buffer of ``input_length`` bytes which contains the raw key stream, i.e. the data that ``psa_key_derivation_output_bytes()`` would return.
*   If ``"memory"`` property in the driver capability is true, ``memory`` is a data structure that the driver may use to store data between successive calls of the ``"derive_key"`` entry point to derive the same key.
    If the ``"memory"`` property is false or absent, the ``memory`` parameter is a null pointer.
*   ``key_buffer`` is a buffer for the output material, in the appropriate `export format <key-format-for-transparent-drivers>` for the key type.
    Its size is ``key_buffer_size`` bytes.
*   On success, ``*key_buffer_length`` must contain the number of bytes written to ``key_buffer``.

This entry point may return the following statuses:

*   ``PSA_SUCCESS``: a key was derived successfully.
    The driver has placed the representation of the key in ``key_buffer``.
*   ``PSA_ERROR_NOT_SUPPORTED`` (for the first call only) (only if fallback is enabled): the driver cannot fulfill this request, but a fallback driver might.
*   ``PSA_ERROR_INSUFFICIENT_DATA``: the core must call the ``"derive_key"`` entry point again with the same ``memory`` object and with subsequent data from the key stream.
*   Any other error is a fatal error.

The core calls the ``"derive_key"`` entry point in a loop until it returns a status other than ``PSA_ERROR_INSUFFICIENT_DATA``.
Each call has a successive fragment of the key stream.
The ``memory`` object is guaranteed to be the same for successive calls, but note that its address may change between calls.
Before the first call, ``*memory`` is initialized to all-bits-zero.

For standard key types, the ``"derive_key"`` entry point is called with a certain input length as follows:

*   ``PSA_KEY_TYPE_DES``: the length of the key.
*   ``PSA_KEY_TYPE_ECC_KEY_PAIR(…)``, ``PSA_KEY_TYPE_DH_KEY_PAIR(…)``: *m* bytes, where the bit-size of the key *n* satisfies 8(*m*-1) < *n* <= 8\ *m*.
*   ``PSA_KEY_TYPE_RSA_KEY_PAIR``: an implementation-defined length.
    A future version of this specification may specify a length.
*   Other key types: not applicable.

See :secref:`cooked-key-derivation-issue` for some points that may not be fully settled.

.. _key-agreement:

Key agreement
^^^^^^^^^^^^^

The core always decouples key agreement from symmetric key derivation.

To implement a call to ``psa_key_derivation_key_agreement()`` where the private key is in a secure element that has a ``"key_agreement_to_key"`` entry point which is applicable for the given key type and algorithm, the core calls the secure element driver as follows:

1.  Call the ``"key_agreement_to_key"`` entry point to create a key object containing the shared secret.
    The key object is volatile and has the type ``PSA_KEY_TYPE_DERIVE``.
2.  Call the ``"key_derivation_setup"`` entry point, passing the resulting key object .
3.  Perform the rest of the key derivation, up to and including the call to the ``"key_derivation_abort"`` entry point.
4.  Call the ``"destroy_key"`` entry point to destroy the key containing the key object.

In other cases, the core treats ``psa_key_derivation_key_agreement()`` as if it was a call to ``psa_raw_key_agreement()`` followed by a call to ``psa_key_derivation_input_bytes()`` on the shared secret.

The entry points related to key agreement have the following prototypes for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_key_agreement(psa_algorithm_t alg,
                                    const psa_key_attributes_t *our_attributes,
                                    const uint8_t *our_key_buffer,
                                    size_t our_key_buffer_length,
                                    const uint8_t *peer_key,
                                    size_t peer_key_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length);
    psa_status_t acme_key_agreement_to_key(psa_algorithm_t alg,
                                           const psa_key_attributes_t *our_attributes,
                                           const uint8_t *our_key_buffer,
                                           size_t our_key_buffer_length,
                                           const uint8_t *peer_key,
                                           size_t peer_key_length,
                                           const psa_key_attributes_t *shared_secret_attributes,
                                           uint8_t *shared_secret_key_buffer,
                                           size_t shared_secret_key_buffer_size,
                                           size_t *shared_secret_key_buffer_length);

Note that unlike most other key creation entry points, in ``"acme_key_agreement_to_key"``, the attributes for the shared secret are not placed near the beginning, but rather grouped with the other parameters related to the shared secret at the end of the parameter list.
This is to avoid potential confusion with the attributes of the private key that is passed as an input.

Driver entry points for PAKE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A PAKE operation is divided into two stages: collecting inputs and computation.
Core side is responsible for keeping inputs and core set-data functions do not have driver entry points.
Collected inputs are available for drivers via get-data functions for ``password``, ``role`` and ``cipher_suite``.

PAKE driver dispatch logic
^^^^^^^^^^^^^^^^^^^^^^^^^^

The core decides whether to dispatch a PAKE operation to a driver based on the location of the provided password.
When all inputs are collected and ``"psa_pake_output"`` or ``"psa_pake_input"`` is called for the first time ``"pake_setup"`` driver entry point is invoked.

1.  If the location of the ``password`` is the local storage

    -   if there is a transparent driver for the specified ciphersuite, the core calls that driver's ``"pake_setup"`` and subsequent entry points.
    -   otherwise, or on fallback, the core uses its built-in implementation.
2.  If the location of the ``password`` is the location of a secure element
    -   the core calls the ``"pake_setup"`` entry point of the secure element driver and subsequent entry points.

Summary of entry points for PAKE
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A PAKE driver has the following entry points:

*   ``"pake_setup"`` (mandatory): always the first entry point to be called.
    It is called when all inputs are collected and the computation stage starts.
*   ``"pake_output"`` (mandatory): derive cryptographic material for the specified step and output it.
*   ``"pake_input"`` (mandatory): provides cryptographic material in the format appropriate for the specified step.
*   ``"pake_get_implicit_key"`` (mandatory): returns implicitly confirmed shared secret from a PAKE.
*   ``"pake_abort"`` (mandatory): always the last entry point to be called.

For naming purposes, here and in the following subsection, this specification takes the example of a driver with the prefix ``"acme"`` that implements the PAKE entry point family with a capability that does not use the ``"names"`` property to declare different type and entry point names.
Such a driver must implement the following type and functions, as well as the entry points listed above and described in the following subsections:

.. code-block::

    typedef ... acme_pake_operation_t;
    psa_status_t acme_pake_abort( acme_pake_operation_t *operation );

.. _pake-driver-inputs:

PAKE driver inputs
^^^^^^^^^^^^^^^^^^

The core conveys the initial inputs for a PAKE operation via an opaque data structure of type ``psa_crypto_driver_pake_inputs_t``.

.. code-block::

    typedef ... psa_crypto_driver_pake_inputs_t; // implementation-specific type

A driver receiving an argument that points to a ``psa_crypto_driver_pake_inputs_t`` can retrieve its contents by calling one of the get-data functions below.

.. code-block::

    psa_status_t psa_crypto_driver_pake_get_password_len(
        const psa_crypto_driver_pake_inputs_t *inputs,
        size_t *password_len);

    psa_status_t psa_crypto_driver_pake_get_password_bytes(
        const psa_crypto_driver_pake_inputs_t *inputs,
        uint8_t *buffer, size_t buffer_size, size_t *buffer_length);

    psa_status_t psa_crypto_driver_pake_get_password_key(
        const psa_crypto_driver_pake_inputs_t *inputs,
        uint8_t** p_key_buffer, size_t *key_buffer_size,
        const psa_key_attributes_t *attributes);

    psa_status_t psa_crypto_driver_pake_get_user_len(
        const psa_crypto_driver_pake_inputs_t *inputs,
        size_t *user_len);

    psa_status_t psa_crypto_driver_pake_get_user(
        const psa_crypto_driver_pake_inputs_t *inputs,
        uint8_t *user_id, size_t user_id_size, size_t *user_id_len);

    psa_status_t psa_crypto_driver_pake_get_peer_len(
        const psa_crypto_driver_pake_inputs_t *inputs,
        size_t *peer_len);

    psa_status_t psa_crypto_driver_pake_get_peer(
        const psa_crypto_driver_pake_inputs_t *inputs,
        uint8_t *peer_id, size_t peer_id_size, size_t *peer_id_length);

    psa_status_t psa_crypto_driver_pake_get_cipher_suite(
        const psa_crypto_driver_pake_inputs_t *inputs,
        psa_pake_cipher_suite_t *cipher_suite);

The get-data functions take the following parameters:

The first parameter ``inputs`` must be a pointer passed by the core to a PAKE driver setup entry point.
Next parameters are return buffers (must not be null pointers).

These functions can return the following statuses:

*   ``PSA_SUCCESS``: value has been successfully obtained
*   ``PSA_ERROR_BAD_STATE``: the inputs are not ready
*   ``PSA_ERROR_BUFFER_TOO_SMALL`` (``psa_crypto_driver_pake_get_password_bytes`` and ``psa_crypto_driver_pake_get_password_key`` only): the output buffer is too small.
    This is not a fatal error and the driver can, for example, subsequently call the same function again with a larger buffer.
    Call ``psa_crypto_driver_pake_get_password_len`` to obtain the required size.

PAKE driver setup
^^^^^^^^^^^^^^^^^

.. code-block::

    psa_status_t acme_pake_setup( acme_pake_operation_t *operation,
                                  const psa_crypto_driver_pake_inputs_t *inputs );


*   ``operation`` is a zero-initialized operation object.
*   ``inputs`` is an opaque pointer to the `inputs <pake-driver-inputs>` for the PAKE operation.

The setup driver function should preserve the inputs using get-data functions.

The pointer output by ``psa_crypto_driver_pake_get_password_key`` is only valid until the "pake_setup" entry point returns.
Opaque drivers must copy all relevant data from the key buffer during the "pake_setup" entry point and must not store the pointer itself.

PAKE driver output
^^^^^^^^^^^^^^^^^^

.. code-block::

    psa_status_t acme_pake_output(acme_pake_operation_t *operation,
                                  psa_crypto_driver_pake_step_t step,
                                  uint8_t *output,
                                  size_t output_size,
                                  size_t *output_length);

*   ``operation`` is an operation object.
*   ``step`` computation step based on which driver should perform an action.
*   ``output`` buffer where the output is to be written.
*   ``output_size`` size of the output buffer in bytes.
*   ``output_length`` the number of bytes of the returned output.

For ``PSA_ALG_JPAKE`` the following steps are available for output operation:
``step`` can be one of the following values:

*   ``PSA_JPAKE_X1_STEP_KEY_SHARE``     Round 1: output our key share (for ephemeral private key X1)
*   ``PSA_JPAKE_X1_STEP_ZK_PUBLIC``     Round 1: output Schnorr NIZKP public key for the X1 key
*   ``PSA_JPAKE_X1_STEP_ZK_PROOF``      Round 1: output Schnorr NIZKP proof for the X1 key
*   ``PSA_JPAKE_X2_STEP_KEY_SHARE``     Round 1: output our key share (for ephemeral private key X2)
*   ``PSA_JPAKE_X2_STEP_ZK_PUBLIC``     Round 1: output Schnorr NIZKP public key for the X2 key
*   ``PSA_JPAKE_X2_STEP_ZK_PROOF``      Round 1: output Schnorr NIZKP proof for the X2 key
*   ``PSA_JPAKE_X2S_STEP_KEY_SHARE``    Round 2: output our X2S key
*   ``PSA_JPAKE_X2S_STEP_ZK_PUBLIC``    Round 2: output Schnorr NIZKP public key for the X2S key
*   ``PSA_JPAKE_X2S_STEP_ZK_PROOF``     Round 2: output Schnorr NIZKP proof for the X2S key

PAKE driver input
^^^^^^^^^^^^^^^^^

.. code-block::

    psa_status_t acme_pake_input(acme_pake_operation_t *operation,
                                 psa_crypto_driver_pake_step_t step,
                                 uint8_t *input,
                                 size_t input_size);

*   ``operation`` is an operation object.
*   ``step`` computation step based on which driver should perform an action.
*   ``input`` buffer containing the input.
*   ``input_length`` length of the input in bytes.

For ``PSA_ALG_JPAKE`` the following steps are available for input operation:

*   ``PSA_JPAKE_X1_STEP_KEY_SHARE``     Round 1: input key share from peer (for ephemeral private key X1)
*   ``PSA_JPAKE_X1_STEP_ZK_PUBLIC``     Round 1: input Schnorr NIZKP public key for the X1 key
*   ``PSA_JPAKE_X1_STEP_ZK_PROOF``      Round 1: input Schnorr NIZKP proof for the X1 key
*   ``PSA_JPAKE_X2_STEP_KEY_SHARE``     Round 1: input key share from peer (for ephemeral private key X2)
*   ``PSA_JPAKE_X2_STEP_ZK_PUBLIC``     Round 1: input Schnorr NIZKP public key for the X2 key
*   ``PSA_JPAKE_X2_STEP_ZK_PROOF``      Round 1: input Schnorr NIZKP proof for the X2 key
*   ``PSA_JPAKE_X4S_STEP_KEY_SHARE``    Round 2: input X4S key from peer
*   ``PSA_JPAKE_X4S_STEP_ZK_PUBLIC``    Round 2: input Schnorr NIZKP public key for the X4S key
*   ``PSA_JPAKE_X4S_STEP_ZK_PROOF``     Round 2: input Schnorr NIZKP proof for the X4S key

The core checks that ``input_length`` is not greater than ``PSA_PAKE_INPUT_SIZE(alg, prim, step)`` and
the driver can rely on that.

PAKE driver get implicit key
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

    psa_status_t acme_pake_get_implicit_key(
                                acme_pake_operation_t *operation,
                                uint8_t *output, size_t output_size,
                                size_t *output_length );

*   ``operation`` The driver PAKE operation object to use.
*   ``output`` Buffer where the implicit key is to be written.
*   ``output_size`` Size of the output buffer in bytes.
*   ``output_length`` On success, the number of bytes of the implicit key.

.. _driver-entry-points-for-key-management:

Driver entry points for key management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver entry points for key management differ significantly between `transparent drivers <key-management-with-transparent-drivers>` and `opaque drivers <key-management-with-opaque-drivers>`.
This section describes common elements.
Refer to the applicable section for each driver type for more information.

The entry points that create or format key data have the following prototypes for a driver with the prefix ``"acme"``:

.. code-block::

    psa_status_t acme_import_key(const psa_key_attributes_t *attributes,
                                 const uint8_t *data,
                                 size_t data_length,
                                 uint8_t *key_buffer,
                                 size_t key_buffer_size,
                                 size_t *key_buffer_length,
                                 size_t *bits); // additional parameter, see below
    psa_status_t acme_generate_key(const psa_key_attributes_t *attributes,
                                   uint8_t *key_buffer,
                                   size_t key_buffer_size,
                                   size_t *key_buffer_length);

Additionally, opaque drivers can create keys through their `"key_derivation_output_key" <key-derivation-driver-outputs>` and :ref:`"key_agreement_key" <key-agreement>` entry points.
Transparent drivers can create key material through their `"derive_key" <transparent-cooked-key-derivation>` entry point.

TODO: copy

*   The key attributes (``attributes``) have the same semantics as in the Crypto API.
*   For the ``"import_key"`` entry point, the input in the ``data`` buffer is either the export format or an implementation-specific format that the core documents as an acceptable input format for ``psa_import_key()``.
*   The size of the key data buffer ``key_buffer`` is sufficient for the internal representation of the key.
    For a transparent driver, this is the key's `export format <key-format-for-transparent-drivers>`.
    For an opaque driver, this is the size determined from the driver description and the key attributes, as specified in the section :secref:`key-format-for-opaque-drivers`.
*   For an opaque driver with an ``"allocate_key"`` entry point, the content of the key data buffer on entry is the output of that entry point.
*   The ``"import_key"`` entry point must determine or validate the key size and set ``*bits`` as described in :secref:`key-size-determination-on-import`.

All key creation entry points must ensure that the resulting key is valid as specified in :secref:`key-validation`.
This is primarily important for import entry points since the key data comes from the application.

.. _key-size-determination-on-import:

Key size determination on import
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``"import_key"`` entry point must determine or validate the key size.
The Crypto API exposes the key size as part of the key attributes.
When importing a key, the key size recorded in the key attributes can be either a size specified by the caller of the API (who may not be trusted), or ``0`` which indicates that the size must be calculated from the data.

When the core calls the ``"import_key"`` entry point to process a call to ``psa_import_key``, it passes an ``attributes`` structure such that ``psa_get_key_bits(attributes)`` is the size passed by the caller of ``psa_import_key``.
If this size is ``0``, the ``"import_key"`` entry point must set the ``bits`` input-output parameter to the correct key size.
The semantics of ``bits`` is as follows:

*   The core sets ``*bits`` to ``psa_get_key_bits(attributes)`` before calling the ``"import_key"`` entry point.
*   If ``*bits == 0``, the driver must determine the key size from the data and set ``*bits`` to this size.
    If the key size cannot be determined from the data, the driver must return ``PSA_ERROR_INVALID_ARGUMENT`` (as of version 1.0 of the Crypto API specification, it is possible to determine the key size for all standard key types).
*   If ``*bits != 0``, the driver must check the value of ``*bits`` against the data and return ``PSA_ERROR_INVALID_ARGUMENT`` if it does not match.
    If the driver entry point changes ``*bits`` to a different value but returns ``PSA_SUCCESS``, the core will consider the key as invalid and the import will fail.

.. _key-validation:

Key validation
^^^^^^^^^^^^^^

Key creation entry points must produce valid key data.
Key data is *valid* if operations involving the key are guaranteed to work functionally and not to cause indirect security loss.
Operation functions are supposed to receive valid keys, and should not have to check and report invalid keys.
For example:

*   If a cryptographic mechanism is defined as having keying material of a certain size, or if the keying material involves integers that have to be in a certain range, key creation must ensure that the keying material has an appropriate size and falls within an appropriate range.
*   If a cryptographic operation involves a division by an integer which is provided as part of a key, key creation must ensure that this integer is nonzero.
*   If a cryptographic operation involves two keys A and B (or more), then the creation of A must ensure that using it does not risk compromising B.
    This applies even if A's policy does not explicitly allow a problematic operation, but A is exportable.
    In particular, public keys that can potentially be used for key agreement are considered invalid and must not be created if they risk compromising the private key.
*   On the other hand, it is acceptable for import to accept a key that cannot be verified as valid if using this key would at most compromise the key itself and material that is secured with this key.
    For example, RSA key import does not need to verify that the primes are actually prime.
    Key import may accept an insecure key if the consequences of the insecurity are no worse than a leak of the key prior to its import.

With opaque drivers, the key context can only be used by code from the same driver, so key validity is primarily intended to report key creation errors at creation time rather than during an operation.
With transparent drivers, the key context can potentially be used by code from a different provider, so key validity is critical for interoperability.

This section describes some minimal validity requirements for standard key types.

*   For symmetric key types, check that the key size is suitable for the type.
*   For DES (``PSA_KEY_TYPE_DES``), additionally verify the parity bits.
*   For RSA (``PSA_KEY_TYPE_RSA_PUBLIC_KEY``, ``PSA_KEY_TYPE_RSA_KEY_PAIR``), check the syntax of the key and make sanity checks on its components.
    TODO: what sanity checks? Value ranges (e.g. p < n), sanity checks such as parity, minimum and maximum size, what else?
*   For elliptic curve private keys (``PSA_KEY_TYPE_ECC_KEY_PAIR``), check the size and range.
    TODO: what else?
*   For elliptic curve public keys (``PSA_KEY_TYPE_ECC_PUBLIC_KEY``), check the size and range, and that the point is on the curve.
    TODO: what else?

.. _entropy-collection-entry-point:

Entropy collection entry point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A driver can declare an entropy source by providing a ``"get_entropy"`` entry point.
This entry point has the following prototype for a driver with the prefix ``"acme"``:

.. code-block::

    typedef uint32_t psa_driver_get_entropy_flags_t;

    psa_status_t acme_get_entropy(psa_driver_get_entropy_flags_t flags,
                                  size_t *estimate_bits,
                                  uint8_t *output,
                                  size_t output_size);

The semantics of the parameters is as follows:

*   ``flags``: a bit-mask of `entropy collection flags <entropy-collection-flags>`.
*   ``estimate_bits``: on success, an estimate of the amount of entropy that is present in the ``output`` buffer, in bits.
    This must be at least ``1`` on success.
    The value is ignored on failure.
    Drivers should return a conservative estimate, even in circumstances where the quality of the entropy source is degraded due to environmental conditions (e.g. undervolting, low temperature, etc.).
*   ``output``: on success, this buffer contains non-deterministic data with an estimated entropy of at least ``*estimate_bits`` bits.
    When the entropy is coming from a hardware peripheral, this should preferably be raw or lightly conditioned measurements from a physical process, such that statistical tests run over a sufficiently large amount of output can confirm the entropy estimates.
    But this specification also permits entropy sources that are fully conditioned, for example when the Crypto API implementation is running within an application in an operating system and ``"get_entropy"`` returns data from the random generator in the operating system's kernel.
*   ``output_size``: the size of the ``output`` buffer in bytes.
    This size should be large enough to allow a driver to pass unconditioned data with a low density of entropy; for example a peripheral that returns eight bytes of data with an estimated one bit of entropy cannot provide meaningful output in less than 8 bytes.

Note that there is no output parameter indicating how many bytes the driver wrote to the buffer.
Such an output length indication is not necessary because the entropy may be located anywhere in the buffer, so the driver may write less than ``output_size`` bytes but the core does not need to know this.
The output parameter ``estimate_bits`` contains the amount of entropy, expressed in bits, which may be significantly less than ``output_size * 8``.

The entry point may return the following statuses:

*   ``PSA_SUCCESS``: success.
    The output buffer contains some entropy.
*   ``PSA_ERROR_INSUFFICIENT_ENTROPY``: no entropy is available without blocking.
    This is only permitted if the ``PSA_DRIVER_GET_ENTROPY_NONBLOCK`` flag is set.
    The core may call ``get_entropy`` again later, giving time for entropy to be gathered or for adverse environmental conditions to be rectified.
*   ``PSA_ERROR_NOT_SUPPORTED``: a flag is not recognized.
    The core may try again with different flags.
*   Other error codes indicate a transient or permanent failure of the entropy source.

Unlike most other entry points, if multiple transparent drivers include a ``"get_entropy"`` point, the core will call all of them (as well as the entry points from opaque drivers).
Fallback is not applicable to ``"get_entropy"``.

.. _entropy-collection-flags:

Entropy collection flags
^^^^^^^^^^^^^^^^^^^^^^^^

*   ``PSA_DRIVER_GET_ENTROPY_NONBLOCK``: If this flag is clean, the driver should block until it has at least one bit of entropy.
    If this flag is set, the driver should avoid blocking if no entropy is readily available.
*   ``PSA_DRIVER_GET_ENTROPY_KEEPALIVE``: This flag is intended to help with energy management for entropy-generating peripherals.
    If this flag is set, the driver should expect another call to ``acme_get_entropy`` after a short time.
    If this flag is clear, the core is not expecting to call the ``"get_entropy"`` entry point again within a short amount of time (but it may do so nonetheless).

A very simple core can just pass ``flags=0``.
All entropy drivers should support this case.

If the entry point returns ``PSA_ERROR_NOT_SUPPORTED``, the core may try calling the entry point again with fewer flags.
Drivers should be consistent from one call to the next with respect to which flags they support.
The core may cache an acceptable flag mask on its first call to an entry point.

Entropy collection and blocking
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The intent of the ``NONBLOCK`` and ``KEEPALIVE`` `flags <entropy-collection-flags>` is to support drivers for TRNG (True Random Number Generator, i.e. an entropy source peripheral) that have a long ramp-up time, especially on platforms with multiple entropy sources.

Here is a suggested call sequence for entropy collection that leverages these flags:

1.  The core makes a first round of calls to ``"get_entropy"`` on every source with the ``NONBLOCK`` flag set and the ``KEEPALIVE`` flag set, so that drivers can prepare the TRNG peripheral.
2.  The core makes a second round of calls with the ``NONBLOCK`` flag clear and the ``KEEPALIVE`` flag clear to gather needed entropy.
3.  If the second round does not collect enough entropy, the core makes more similar rounds, until the total amount of collected entropy is sufficient.

Miscellaneous driver entry points
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _driver-initialization:

Driver initialization
^^^^^^^^^^^^^^^^^^^^^

A driver may declare an ``"init"`` entry point in a capability with no algorithm, key type or key size.
If so, the core calls this entry point once during the initialization of the Crypto API implementation.
If the init entry point of any driver fails, the initialization of the Crypto API implementation fails.

When multiple drivers have an init entry point, the order in which they are called is unspecified.
It is also unspecified whether other drivers' ``"init"`` entry points are called if one or more init entry point fails.

On platforms where the Crypto API implementation is a subsystem of a single application, the initialization of the Crypto API implementation takes place during the call to ``psa_crypto_init()``.
On platforms where the Crypto API implementation is separate from the application or applications, the initialization of the Crypto API implementation takes place before or during the first time an application calls ``psa_crypto_init()``.

The init entry point does not take any parameter.

Combining multiple drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~

To declare a cryptoprocessor can handle both cleartext and wrapped keys, you need to provide two driver descriptions, one for a transparent driver and one for an opaque driver.
You can use the mapping in capabilities' ``"names"`` property to arrange for multiple driver entry points to map to the same C function.
