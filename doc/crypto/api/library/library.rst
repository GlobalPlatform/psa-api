.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
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
     * PSA Crypto API v1.5
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

    It is recommended that applications call this function before calling any other function in this module.

    Applications are permitted to call this function more than once. Once a call succeeds, subsequent calls are guaranteed to succeed.

    If the application calls any function that returns a :code:`psa_status_t` result code before calling `psa_crypto_init()`, the following will occur:

    *   If initialization of the library is essential for secure operation of the function, the implementation must return :code:`PSA_ERROR_BAD_STATE` or other appropriate error.

    *   If failure to initialize the library does not compromise the security of the function, the implementation must either provide the expected result for the function, or return :code:`PSA_ERROR_BAD_STATE` or other appropriate error.

    .. note::

        The following scenarios are examples where an implementation can require that the library has been initialized by calling `psa_crypto_init()`:

        *   A client-server implementation, in which `psa_crypto_init()` establishes the communication with the server. No key management or cryptographic operation can be performed until this is done.

        *   An implementation in which `psa_crypto_init()` initializes the random bit generator, and no operations that require the RNG can be performed until this is done. For example, random data, key, IV, or nonce generation; randomized signature or encryption; and algorithms that are implemented with blinding.

    .. warning::
        The set of functions that depend on successful initialization of the library is :scterm:`IMPLEMENTATION DEFINED`. Applications that rely on calling functions before initializing the library might not be portable to other implementations.


Interruptible operation limit
-----------------------------

An interruptible operation lets an application limit the computation performed by an individual function call. The limit is controlled by the *maximum ops* value.

See :secref:`interruptible-operations`.

.. function:: psa_iop_set_max_ops

    .. summary::
        Set the maximum number of *ops* allowed to be executed by an interruptible function in a single call.

        .. versionadded:: 1.6

    .. param:: uint32_t max_ops
        The maximum number of *ops* to execute in a single call. This can be a value from ``0`` to `PSA_IOP_MAX_OPS_UNLIMITED`.

    .. return:: void

    Interruptible functions use this value to limit the computation in a single call. If the limit is reached before the operation is complete, the function returns :code:`PSA_OPERATION_INCOMPLETE`. The application must call the function again until it returns a different status, or abort the operation.

    After implementation initialization, the maximum *ops* defaults to `PSA_IOP_MAX_OPS_UNLIMITED`. This permits an interruptible function to finish its calculation before returning. Call `psa_iop_set_max_ops()` to set a limit.

    .. note::

        The computation and time represented by an *op* are implementation- and function-specific. They can depend on the hardware, algorithm, key type, curve, and current stage of the operation. Successive *ops* in one operation can take different amounts of time. The ``psa_xxx_iop_get_num_ops()`` functions can help tune this value.

    .. admonition:: Implementation note

        The interpretation of the maximum is implementation-defined. An implementation intended for a hard real-time system can interpret it as a hard execution limit. An implementation for a non-real-time system can use a less strict interpretation. In either case, the implementation documentation should describe how it applies the limit.

    .. warning::
        With implementations that interpret this value as a hard limit, setting it too low can prevent any useful computation. Repeated calls can then return :code:`PSA_OPERATION_INCOMPLETE` without completing the operation.

.. function:: psa_iop_get_max_ops

    .. summary::
        Get the maximum number of *ops* allowed to be executed by an interruptible function in a single call.

        .. versionadded:: 1.6

    .. return:: uint32_t
        Maximum number of *ops* allowed to be executed by an interruptible function in a single call.

    This returns the value last set by `psa_iop_set_max_ops()`, or `PSA_IOP_MAX_OPS_UNLIMITED` if the application has not set a value.

.. macro:: PSA_IOP_MAX_OPS_UNLIMITED
    :definition: UINT32_MAX

    .. summary::
        Maximum value for use with `psa_iop_set_max_ops()`.

        .. versionadded:: 1.6

    Using this value in a call to `psa_iop_set_max_ops()` permits interruptible functions to complete their calculation before returning.
