.. SPDX-FileCopyrightText: Copyright 2018-2024 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _library-conventions:

Library conventions
-------------------

Header files
~~~~~~~~~~~~

The header file for the |API| has the name :file:`psa/crypto.h`. All of the API elements that are provided by an implementation must be visible to an application program that includes this header file.

..  code-block:: none

    #include "psa/crypto.h"

Implementations must provide their own version of the :file:`psa/crypto.h` header file. Implementations can provide a subset of the API defined in this specification and a subset of the available algorithms. :secref:`appendix-example-header` provides an incomplete, example header file which includes all of the API elements. See also :secref:`implementation-considerations`.

The |API| uses the status code definitions that are shared with the other PSA APIs. :cite-title:`PSA STAT` defines these status codes in the :file:`psa/error.h` header file. Applications are not required to explicitly include the :file:`psa/error.h` header file when using these status codes with the |API|. See :secref:`status-codes`.

.. _api-conventions:

API conventions
~~~~~~~~~~~~~~~

The interface in this specification is defined in terms of C macros, data types, and functions.

Identifier names
^^^^^^^^^^^^^^^^

All of the identifiers defined in the |API| begin with the prefix ``psa_``, for types and functions, or ``PSA_`` for macros.

Future versions of this specification will use the same prefix for additional API elements. It is recommended that applications and implementations do not use this prefix for their own identifiers, to avoid a potential conflict with a future version of the |API|.

Basic types
^^^^^^^^^^^

This specification makes use of standard C data types, including the fixed-width integer types from the ISO C99 specification update :cite:`C99`. The following standard C types are used:

..  csv-table::
    :widths: auto
    :align: left

    ``int32_t``, a 32-bit signed integer
    ``uint8_t``, an 8-bit unsigned integer
    ``uint16_t``, a 16-bit unsigned integer
    ``uint32_t``, a 32-bit unsigned integer
    ``uint64_t``, a 64-bit unsigned integer
    ``size_t``, an unsigned integer large enough to hold the size of an object in memory

Data types
^^^^^^^^^^

Integral types are defined for specific API elements to provide clarity in the interface definition, and to improve code readability. For example, `psa_algorithm_t` and :code:`psa_status_t`.

For enum-like integral types, the value ``0`` is usually reserved by the API to indicate an unspecified or invalid value.

Structure types are declared using ``typedef`` instead of a ``struct`` tag, also to improve code readability.

Fully-defined types must be declared exactly as defined in this specification. Types that are not fully defined in this specification must be defined by an implementation. See :secref:`implementation-defined-type`.

Constants
^^^^^^^^^

Constant values are defined using C macros. Constants defined in this specification have names that are all upper-case.

A constant macro evaluates to a compile-time constant expression.

Function-like macros
^^^^^^^^^^^^^^^^^^^^

Function-like macros are C macros that take parameters, providing supporting functionality in the API. Function-like macros defined in this specification have names that are all upper-case.

Function-like macros are permitted to evaluate each argument multiple times or zero times. Providing arguments that have side effects will result in :scterm:`IMPLEMENTATION DEFINED` behavior, and is non-portable.

If all of the arguments to a function-like macro are compile-time constant expressions, the then result evaluates to a compile-time constant expression.

If an argument to a function-like macro has an invalid value (for example, a value outside the domain of the function-like macro), then the result is :scterm:`IMPLEMENTATION DEFINED`.

Functions
^^^^^^^^^

Functions defined in this specification have names that are all lower-case.

An implementation is permitted to declare any API function with ``static inline`` linkage, instead of the default ``extern`` linkage.

An implementation is permitted to also define a function-like macro with the same name as a function in this specification. If an implementation defines a function-like macro for a function from this specification, then:

*   The implementation must also provide a definition of the function. This enables an application to take the address of a function defined in this specification.
*   The function-like macro must expand to code that evaluates each of its arguments exactly once, as if the call was made to a C function. This enables an application to safely use arbitrary expressions as arguments to a function defined in this specification.

If a non-pointer argument to a function has an invalid value (for example, a value outside the domain of the function), then the function will normally return an error, as specified in the function definition. See also :secref:`error-handling`.

If a pointer argument to a function has an invalid value (for example, a pointer outside the address space of the program, or a null pointer), the result is :scterm:`IMPLEMENTATION DEFINED`. See also :secref:`pointer-conventions`.


.. _error-handling:

Error handling
~~~~~~~~~~~~~~

Return status
^^^^^^^^^^^^^

Almost all functions return a status indication of type :code:`psa_status_t`. This
is an enumeration of integer values, with ``0`` (:code:`PSA_SUCCESS`) indicating
successful operation and other values indicating errors. The exceptions are
functions which only access objects that are intended to be implemented as
simple data structures. Such functions cannot fail and either return
``void`` or a data value.

Unless specified otherwise, if multiple error conditions apply, an
implementation is free to return any of the applicable error codes. The choice
of error code is considered an implementation quality issue. Different
implementations can make different choices, for example to favor code size over
ease of debugging or vice versa.

In particular, in the |API|, there are many conditions where the specification permits a function to return either :code:`PSA_ERROR_INVALID_ARGUMENT` or :code:`PSA_ERROR_NOT_SUPPORTED`.
For example, `psa_hash_compute()` is passed a hash algorithm that the implementation does not support, it is :scterm:`implementation defined` whether :code:`PSA_ERROR_INVALID_ARGUMENT` or :code:`PSA_ERROR_NOT_SUPPORTED` is returned.

.. note::

    This flexibility supports the `scalability design goal<scalable>`.
    It permits implementations to not check whether unsupported algorithm identifier and key type values are valid or invalid.

If the behavior is undefined, for example, if a function receives an invalid
pointer as a parameter, this specification makes no guarantee that the function
will return an error. Implementations are encouraged to return an error or halt
the application in a manner that is appropriate for the platform if the
undefined behavior condition can be detected. However, application developers need to be aware that undefined behavior conditions cannot be detected in general.

Behavior on error
^^^^^^^^^^^^^^^^^

In general, function calls must be implemented atomically:

*   When a function returns a type other than :code:`psa_status_t`, the requested
    action has been carried out.
*   When a function returns the status :code:`PSA_SUCCESS`, the requested action has
    been carried out.
*   When a function returns another status of type :code:`psa_status_t`, no action
    has been carried out. Unless otherwise documented by the API or the
    implementation, the content of output parameters is not defined. The state of
    the system has not changed, except as described below.

In general, functions that modify the system state, for example, creating or
destroying a key, must leave the system state unchanged if they return an error
code. There are specific conditions that can result in different behavior:

*   The status :code:`PSA_ERROR_BAD_STATE` indicates that a parameter was not in a
    valid state for the requested action. This parameter might have been modified
    by the call and is now in an error state. The only valid action on an
    object in an error state is to abort it with the appropriate
    ``psa_xxx_abort()`` function. See :secref:`multi-part-operations`.
*   The status :code:`PSA_ERROR_INSUFFICIENT_DATA` indicates that a key
    derivation object has reached its maximum capacity. The key derivation
    operation might have been modified by the call. Any further attempt to obtain
    output from the key-derivation operation will return
    :code:`PSA_ERROR_INSUFFICIENT_DATA`.
*   The status :code:`PSA_ERROR_COMMUNICATION_FAILURE` indicates that the
    communication between the application and the cryptoprocessor has broken
    down. In this case, the cryptoprocessor must either finish the requested
    action successfully, or interrupt the action and roll back the system to its
    original state. Because it is often impossible to report the outcome to the
    application after a communication failure, this specification does not
    provide a way for the application to determine whether the action was
    successful.
*   The statuses :code:`PSA_ERROR_STORAGE_FAILURE`, :code:`PSA_ERROR_DATA_CORRUPT`, :code:`PSA_ERROR_HARDWARE_FAILURE`
    and :code:`PSA_ERROR_CORRUPTION_DETECTED` might indicate data corruption in the
    system state. When a function returns one of these statuses, the system state
    might have changed from its previous state before the function call, even
    though the function call failed.
*   Some system states cannot be rolled back, for example, the internal state of
    the random number generator or the content of access logs.

.. admonition:: Implementation note

    When a function returns an error status, it is recommended
    that implementations set output parameters to safe defaults to avoid leaking
    confidential data and limit risk, in case an application does not properly
    handle all errors.

Parameter conventions
~~~~~~~~~~~~~~~~~~~~~

.. _pointer-conventions:

Pointer conventions
^^^^^^^^^^^^^^^^^^^

Unless explicitly stated in the documentation of a function, all pointers must
be valid pointers to an object of the specified type.

A parameter is considered a **buffer** if it points to an array of bytes. A
buffer parameter always has the type ``uint8_t *`` or ``const uint8_t *``, and
always has an associated parameter indicating the size of the array. Note that a
parameter of type ``void *`` is never considered a buffer.

All parameters of pointer type must be valid non-null pointers, unless the
pointer is to a buffer of length ``0`` or the function’s documentation
explicitly describes the behavior when the pointer is null. Passing a null
pointer as a function parameter in other cases is expected to abort the caller
on implementations where this is the normal behavior for a null pointer
dereference.

Pointers to input parameters can be in read-only memory. Output parameters must
be in writable memory. Output parameters that are not buffers must also be
readable, and the implementation must be able to write to a non-buffer output
parameter and read back the same value, as explained in
:secref:`stability-of-parameters`.

Input buffer sizes
^^^^^^^^^^^^^^^^^^

For input buffers, the parameter convention is:

``const uint8_t *foo``
    Pointer to the first byte of the data. The pointer
    can be invalid if the buffer size is ``0``.

``size_t foo_length``
    Size of the buffer in bytes.

The interface never uses input-output buffers.

.. _output-buffers:

Output buffer sizes
^^^^^^^^^^^^^^^^^^^

For output buffers, the parameter convention is:

``uint8_t *foo``
    Pointer to the first byte of the data. The pointer can be
    invalid if the buffer size is ``0``.

``size_t foo_size``
    The size of the buffer in bytes.

``size_t *foo_length``
    On successful return, contains the length of the
    output in bytes.

The content of the data buffer and of ``*foo_length`` on errors is unspecified,
unless explicitly mentioned in the function description. They might be unmodified
or set to a safe default. On successful completion, the content of the buffer
between the offsets ``*foo_length`` and ``foo_size`` is also unspecified.

Functions return :code:`PSA_ERROR_BUFFER_TOO_SMALL` if the buffer size is
insufficient to carry out the requested operation. The interface defines macros
to calculate a sufficient buffer size for each operation that has an output
buffer. These macros return compile-time constants if their arguments are
compile-time constants, so they are suitable for static or stack allocation.
Refer to an individual function’s documentation for the associated output size
macro.

Some functions always return exactly as much data as the size of the output
buffer. In this case, the parameter convention changes to:

``uint8_t *foo``
    Pointer to the first byte of the output. The pointer can be
    invalid if the buffer size is ``0``.

``size_t foo_length``
    The number of bytes to return in ``foo`` if
    successful.

.. _buffer-overlap:

Overlap between parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

Output parameters that are not buffers must not overlap with any input buffer or
with any other output parameter. Otherwise, the behavior is undefined.

Output buffers can overlap with input buffers. In this event, the implementation
must return the same result as if the buffers did not overlap. The
implementation must behave as if it had copied all the inputs into temporary
memory, as far as the result is concerned. However, it is possible that overlap
between parameters will affect the performance of a function call. Overlap might
also affect memory management security if the buffer is located in memory that
the caller shares with another security context, as described in
:secref:`stability-of-parameters`.

.. _stability-of-parameters:

Stability of parameters
^^^^^^^^^^^^^^^^^^^^^^^

In some environments, it is possible for the content of a parameter to change
while a function is executing. It might also be possible for the content of an
output parameter to be read before the function terminates. This can happen if
the application is multithreaded. In some implementations, memory can be shared
between security contexts, for example, between tasks in a multitasking
operating system, between a user land task and the kernel, or between the
Non-secure world and the Secure world of a trusted execution environment.

This section describes the assumptions that an implementation can make about
function parameters, and the guarantees that the implementation must provide
about how it accesses parameters.

Parameters that are not buffers are assumed to be under the caller’s full
control. In a shared memory environment, this means that the parameter must be
in memory that is exclusively accessible by the application. In a multithreaded
environment, this means that the parameter must not be modified during the
execution, and the value of an output parameter is undetermined until the
function returns. The implementation can read an input parameter that is not a
buffer multiple times and expect to read the same data. The implementation can
write to an output parameter that is not a buffer and expect to read back the
value that it last wrote. The implementation has the same permissions on buffers
that overlap with a buffer in the opposite direction.

In an environment with multiple threads or with shared memory, the
implementation carefully accesses non-overlapping buffer parameters in order to
prevent any security risk resulting from the content of the buffer being
modified or observed during the execution of the function. In an input buffer
that does not overlap with an output buffer, the implementation reads each byte
of the input once, at most. The implementation does not read from an output
buffer that does not overlap with an input buffer. Additionally, the
implementation does not write data to a non-overlapping output buffer if this
data is potentially confidential and the implementation has not yet verified
that outputting this data is authorized.

Unless otherwise specified, the implementation must not keep a reference to any
parameter once a function call has returned.

Key types and algorithms
~~~~~~~~~~~~~~~~~~~~~~~~

Types of cryptographic keys and cryptographic algorithms are encoded separately.
Each is encoded by using an integral type: `psa_key_type_t` and
`psa_algorithm_t`, respectively.

There is some overlap in the information conveyed by key types and algorithms.
Both types contain enough information, so that the meaning of an algorithm type
value does not depend on what type of key it is used with, and vice versa.
However, the particular instance of an algorithm might depend on the key type. For
example, the algorithm `PSA_ALG_GCM` can be instantiated as any AEAD algorithm
using the GCM mode over a block cipher. The underlying block cipher is
determined by the key type.

Key types do not encode the key size. For example, AES-128, AES-192 and AES-256
share a key type `PSA_KEY_TYPE_AES`.

Structure of key types and algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Both types use a partial bitmask structure, which allows the analysis and
building of values from parts. However, the interface defines constants, so that
applications do not need to depend on the encoding, and an implementation might
only care about the encoding for code size optimization.

The encodings follows a few conventions:

*   The highest bit is a vendor flag. Current and future versions of this
    specification will only define values where this bit is clear.
    Implementations that wish to define additional implementation-specific values
    must use values where this bit is set, to avoid conflicts with future
    versions of this specification.
*   The next few highest bits indicate the algorithm or key category:
    hash, MAC, symmetric cipher, asymmetric encryption, and so on.
*   The following bits identify a family of algorithms or keys in a category-dependent
    manner.
*   In some categories and algorithm families, the lowest-order bits indicate a
    variant in a systematic way. For example, algorithm families that are
    parametrized around a hash function encode the hash in the 8 lowest bits.

The :secref:`appendix-encodings` appendix provides a full definition of the encoding of key types and algorithm identifiers.


.. _concurrency:

Concurrent calls
~~~~~~~~~~~~~~~~

In some environments, an application can make calls to the |API| in
separate threads. In such an environment, *concurrent calls* are two or more
calls to the API whose execution can overlap in time.

**Sequential consistency**
    The result of two or more concurrent calls must be consistent with the
    same set of calls being executed sequentially in some order, provided that
    the calls obey the following constraints:

    *   There is no overlap between an output parameter of one call and an
        input or output parameter of another call. Overlap between input
        parameters is permitted.

    *   A call to :code:`psa_destroy_key()` must not overlap with a concurrent
        call to any of the following functions:

        -   Any call where the same key identifier is a parameter to the call.
        -   Any call in a multi-part operation, where the same key identifier
            was used as a parameter to a previous step in the multi-part
            operation.

    *   Concurrent calls must not use the same operation object.

    If any of these constraints are violated, the behavior is undefined.

    The consistency requirement does not apply to errors that arise
    from resource failures or limitations. For example, errors resulting from
    resource exhaustion can arise in concurrent execution that do not arise in
    sequential execution.

    As an example of this rule: suppose two calls are executed concurrently
    which both attempt to create a new key with the same key identifier that is
    not already in the key store. Then:

    *   If one call returns :code:`PSA_ERROR_ALREADY_EXISTS`, then the other
        call must succeed.
    *   If one of the calls succeeds, then the other must fail: either with
        :code:`PSA_ERROR_ALREADY_EXISTS` or some other error status.
    *   Both calls can fail with error codes that are not
        :code:`PSA_ERROR_ALREADY_EXISTS`.

**Parameter stability**
    If the application concurrently modifies an input parameter while a
    function call is in progress, the behavior is undefined.

Individual implementations can provide additional guarantees.
