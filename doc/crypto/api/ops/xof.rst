.. SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 215

.. _xof:

Extendable-output functions (XOF)
=================================

An eXtendable-Output Function (XOF) is similar to a cryptographic hash, transforming an arbitrary amount of input data into pseudorandom output.
Unlike hash algorithms, an XOF can produce an arbitrary amount of output.

XOF algorithms are often used as a building block in other algorithms, as they are suitable for use in hashing, key-derivation, and as a pseudorandom function (PRF).

In the |API|, support for XOF algorithms is provided by the `psa_xof_operation_t` `multi-part operation <multi-part-operations>`, and XOF algorithm identifiers.
A multi-part XOF operation is used as follows:

1.  Initialize the `psa_xof_operation_t` object to zero, or by assigning the value of the associated macro `PSA_XOF_OPERATION_INIT`.
#.  Call `psa_xof_setup()` to specify the required XOF algorithm.
#.  If the algorithm has a context, call `psa_xof_set_context()` to provide the context value.
#.  Call the `psa_xof_update()` function on successive chunks of the input data.
#.  After input is complete, call `psa_xof_output()` one or more times to extract successive chunks of output.
#.  When output is complete, call `psa_xof_abort()` to end the operation.

To abort the operation or recover from an error, call `psa_xof_abort()`.

.. note::

    For an XOF algorithm:

    *   The result does not depend on how the overall input is fragmented.
        For example, calling `psa_xof_update()` twice with input :math:`i_1` and :math:`i_2` has the same effect as calling `psa_xof_update()` once with the concatenation :math:`i_1\ ||\ i_2`.
    *   The overall output does not depend on how the output is fragmented.
        If the output is considered as a stream of bytes, `psa_xof_output()` is an operation that reads bytes in sequence from the stream of data.

.. _xof-algorithms:

XOF algorithms
--------------

.. macro:: PSA_ALG_SHAKE128
    :definition: ((psa_algorithm_t)0x0D000100)

    .. summary::
        The SHAKE128 XOF algorithm.

        .. versionadded:: 1.4

    SHAKE128 is one of the KECCAK family of algorithms.

    SHAKE128 is defined in :cite-title:`FIPS202`.

    Some fixed output-length hash algorithms based on SHAKE128 are also provided in the |API|:

    *   `PSA_ALG_SHAKE128_256`

.. macro:: PSA_ALG_SHAKE256
    :definition: ((psa_algorithm_t)0x0D000200)

    .. summary::
        The SHAKE256 XOF algorithm.

        .. versionadded:: 1.4

    SHAKE256 is one of the KECCAK family of algorithms.

    SHAKE256 is defined in `[FIPS202]`.

    Some fixed output-length hash algorithms based on SHAKE256 are also provided in the |API|:

    *   `PSA_ALG_SHAKE256_192`
    *   `PSA_ALG_SHAKE256_256`
    *   `PSA_ALG_SHAKE256_512`

.. macro:: PSA_ALG_ASCON_XOF128
    :definition: ((psa_algorithm_t)0x0D000300)

    .. summary::
        The Ascon-XOF128 XOF algorithm.

        .. versionadded:: 1.4

    Ascon-XOF128 is defined in :cite-title:`SP800-232` §5.2.

    .. note::
        To use the Ascon-Hash256 hash algorithm, see `PSA_ALG_ASCON_HASH256`.

.. macro:: PSA_ALG_ASCON_CXOF128
    :definition: ((psa_algorithm_t)0x0D008300)

    .. summary::
        The Ascon-CXOF128 XOF algorithm, with context.

        .. versionadded:: 1.4

    Ascon-CXOF128 is defined in :cite-title:`SP800-232` §5.3.

    The context value must be provided by calling `psa_xof_set_context()` on the XOF multi-part operation, before providing any input data.

Multi-part XOF operations
-------------------------

.. typedef:: /* implementation-defined type */ psa_xof_operation_t

    .. summary::
        The type of the state object for multi-part XOF operations.

        .. versionadded:: 1.4

    Before calling any function on an XOF operation object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_xof_operation_t operation;
            memset(&operation, 0, sizeof(operation));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_xof_operation_t operation;

    *   Initialize the object to the initializer `PSA_XOF_OPERATION_INIT`, for example:

        .. code-block:: xref

            psa_xof_operation_t operation = PSA_XOF_OPERATION_INIT;

    *   Assign the result of the function `psa_xof_operation_init()` to the object, for example:

        .. code-block:: xref

            psa_xof_operation_t operation;
            operation = psa_xof_operation_init();

    This is an implementation-defined type.
    Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

.. macro:: PSA_XOF_OPERATION_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for an XOF operation object of type `psa_xof_operation_t`.

        .. versionadded:: 1.4

.. function:: psa_xof_operation_init

    .. summary::
        Return an initial value for an XOF operation object.

        .. versionadded:: 1.4

    .. return:: psa_xof_operation_t

.. function:: psa_xof_setup

    .. summary::
        Set up an XOF operation.

        .. versionadded:: 1.4

    .. param:: psa_xof_operation_t * operation
        The operation object to set up.
        It must have been initialized as per the documentation for `psa_xof_operation_t` and not yet in use.
    .. param:: psa_algorithm_t alg
        The XOF algorithm to compute: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_XOF(alg)` is true.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success. The operation is now active.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        ``alg`` is not supported or is not an XOF algorithm.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        ``alg`` is not an XOF algorithm.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be inactive.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The sequence of operations to generate XOF output is as follows:

    1.  Allocate an XOF operation object which will be passed to all the functions listed here.
    #.  Initialize the operation object with one of the methods described in the documentation for `psa_xof_operation_t`, e.g. `PSA_XOF_OPERATION_INIT`.
    #.  Call `psa_xof_setup()` to specify the algorithm.
    #.  For an XOF algorithm that has a context, call `psa_xof_set_context()` to provide the context.
    #.  Call `psa_xof_update()` zero, one, or more times, passing a fragment of the input each time.
    #.  To extract XOF output data, call `psa_xof_output()` one or more times.

    After a successful call to `psa_xof_setup()`, the operation is active, and the application must eventually terminate the operation with a call to `psa_xof_abort()`.

    If `psa_xof_setup()` returns an error, the operation object is unchanged.
    If a subsequent function call with an active operation returns an error, the operation enters an error state.

    To abandon an active operation, or reset an operation in an error state, call `psa_xof_abort()`.

    See :secref:`multi-part-operations`.

.. function:: psa_xof_set_context

    .. summary::
        Provide a context for a multi-part XOF operation.

        .. versionadded:: 1.4

    .. param:: psa_xof_operation_t * operation
        Active XOF operation.
    .. param:: const uint8_t * context
        Buffer containing the context value.
    .. param:: size_t context_length
        Size of the ``context`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and no call to `psa_xof_set_context()`, `psa_xof_update()`, or `psa_xof_output()` has been made.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The following conditions can result in this error:

        *   The algorithm does not support a context value.
            See `PSA_ALG_XOF_HAS_CONTEXT()`.
        *   The context value is not valid for the XOF algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The context value is not supported by this implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    This function sets the context value in a multi-part XOF operation, when using an XOF algorithm that has a context parameter.

    The application must call `psa_xof_setup()` before calling this function.
    For an XOF algorithm with a context parameter, this function must be called immediately after `psa_xof_setup()`, before calling any other function on the XOF operation.

    This function must not be called if the XOF algorithm does not have a context parameter.
    The macro `PSA_ALG_XOF_HAS_CONTEXT()` can be used to determine if a context value is required for the XOF algorithm.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_xof_abort()`.

.. function:: psa_xof_update

    .. summary::
        Add input to a multi-part XOF operation.

        .. versionadded:: 1.4

    .. param:: psa_xof_operation_t * operation
        Active XOF operation.
    .. param:: const uint8_t * input
        Buffer containing the input fragment.
    .. param:: size_t input_length
        Size of the ``input`` buffer in bytes.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active, and no call to `psa_xof_output()` has been made.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The total input for the operation is too large for the XOF algorithm.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The total input for the operation is too large for the implementation.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    The application must call `psa_xof_setup()` before calling this function.

    This function can be called zero, one, or more times to provide input for the XOF.
    The input to the XOF is only finalized on the first call to `psa_xof_output()`.

    `psa_xof_update()` cannot be called on an XOF operation once `psa_xof_output()` has been called on the operation.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_xof_abort()`.

.. function:: psa_xof_output

    .. summary::
        Extract data from an XOF operation.

        .. versionadded:: 1.4

    .. param:: psa_xof_operation_t * operation
        Active XOF operation.
    .. param:: uint8_t * output
        Buffer where the output will be written.
    .. param:: size_t output_length
        Number of bytes to output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The first ``output_length`` bytes of ``output`` contain the data.
    .. retval:: PSA_ERROR_BAD_STATE
        The following conditions can result in this error:

        *   The operation state is not valid: it must be active.
        *   The library requires initializing. See :secref:`library-init`.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED

    This function calculates output bytes from the XOF algorithm and returns those bytes.
    If the key derivation's output is viewed as a stream of bytes, this function consumes the requested number of bytes from the stream and returns them to the caller.

    The application must call `psa_xof_setup()` and supply all input data, using calls to `psa_xof_update()`, before calling this function.
    The input to the XOF is finalized on the first call to `psa_xof_output()` before data is extracted from the XOF.

    If this function returns an error status, the operation enters an error state and must be aborted by calling `psa_xof_abort()`.

.. function:: psa_xof_abort

    .. summary::
        Abort an XOF operation.

        .. versionadded:: 1.4

    .. param:: psa_xof_operation_t * operation
        Initialized XOF operation.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        The operation object can now be discarded or reused.
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    Aborting an operation frees all associated resources except for the ``operation`` object itself.
    Once aborted, the operation object can be reused for another operation by calling `psa_xof_setup()` again.

    This function can be called any time after the operation object has been initialized by one of the methods described in `psa_xof_operation_t`.

    In particular, calling `psa_xof_abort()` after the operation has been terminated by a call to `psa_xof_abort()` is safe and has no effect.


Support macros
--------------

.. macro:: PSA_ALG_XOF_HAS_CONTEXT
    :definition: /* specification-defined value */

    .. summary::
        Whether the specified XOF algorithm has a context parameter.

        .. versionadded:: 1.4

    .. param:: alg
        An XOF algorithm identifier: a value of type `psa_algorithm_t` such that :code:`PSA_ALG_IS_XOF(alg)` is true.

    .. return::
        ``1`` if ``alg`` is an XOF algorithm that has a context parameter.
        ``0`` if ``alg`` is an XOF algorithm that does not have a context parameter.
        This macro can return either ``0`` or ``1`` if ``alg`` is not a supported XOF algorithm identifier.
