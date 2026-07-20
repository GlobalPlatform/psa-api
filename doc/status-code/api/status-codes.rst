.. SPDX-FileCopyrightText: Copyright 2017-2022, 2024 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

Status codes
============

.. _error-codes:

Overview
--------

The PSA APIs are often implemented together in a larger framework. For example, the :cite-title:`TF-M` project implements all of the PSA functional APIs as Root of Trust Services within the Secure Processing Environment that it provides. Using a common definition for status and error codes enables easier integration and inter-operation of these APIs.

The PSA APIs use the convention that status codes that are negative indicate an error, and zero or positive values indicate success. These are identified in the API by the `psa_status_t` type.

Status codes ``-129`` to ``-248`` are for use by PSA API specifications. These codes are defined in the current PSA specifications, or are reserved for future PSA specifications. Status codes in this range are used in the following ways:

*  A set of standard error codes that cover failure conditions that are common to more than one PSA API.
*  Error codes that are specific to an individual PSA API.

Status codes in this range must only be used as defined in a PSA specification.

In the context of an implementation of :cite-title:`PSA FFM`:

*  The :term:`Secure Partition Manager` (SPM) implementation can define error codes in the range ``-249`` to ``-256`` for :sc:`IMPLEMENTATION DEFINED` purposes.
*  A :term:`Root of Trust Service` (RoT Service) can define additional error codes in the ranges ``-1`` to ``-128`` and ``-257`` to ``MIN_INT32`` for RoT Service-specific error conditions.

:numref:`tab-error-codes` defines the common error codes and reserved ranges for the PSA APIs. See the error code macros and function definitions in :secref:`api` for details on their usage.

.. csv-table:: Standard error codes
   :name: tab-error-codes
   :header-rows: 1
   :widths: 20 9 31

   Status code name, Value, Condition
   *Success*, ``>= 1``,API-specific status code.
   ``PSA_SUCCESS``, ``0``, General success status code.
   *API-specific error*, ``-1`` to ``-128``, API-specific error code.
   ``PSA_ERROR_PROGRAMMER_ERROR``, ``-129``, Connection dropped due to :scterm:`PROGRAMMER ERROR`.
   ``PSA_ERROR_CONNECTION_REFUSED``, ``-130``, Connection to the service is not permitted.
   ``PSA_ERROR_CONNECTION_BUSY``, ``-131``, Connection to the service is not possible.
   ``PSA_ERROR_GENERIC_ERROR``, ``-132``, An error not related to a specific failure cause.
   ``PSA_ERROR_NOT_PERMITTED``, ``-133``, The operation is denied by a policy.
   ``PSA_ERROR_NOT_SUPPORTED``, ``-134``, The operation or a parameter is not supported.
   ``PSA_ERROR_INVALID_ARGUMENT``, ``-135``, One or more parameters are invalid.
   ``PSA_ERROR_INVALID_HANDLE``, ``-136``, A handle parameter is not valid.
   ``PSA_ERROR_BAD_STATE``, ``-137``, The operation is not valid in the current state.
   ``PSA_ERROR_BUFFER_TOO_SMALL``, ``-138``, An output buffer parameter is too small.
   ``PSA_ERROR_ALREADY_EXISTS``, ``-139``, An identifier or index is already in use.
   ``PSA_ERROR_DOES_NOT_EXIST``, ``-140``, An identified resource does not exist.
   ``PSA_ERROR_INSUFFICIENT_MEMORY``, ``-141``, There is not enough runtime memory.
   ``PSA_ERROR_INSUFFICIENT_STORAGE``, ``-142``, There is not enough persistent storage.
   ``PSA_ERROR_INSUFFICIENT_DATA``, ``-143``, A data source has insufficient capacity left.
   ``PSA_ERROR_SERVICE_FAILURE``, ``-144``, Failure within the service.
   ``PSA_ERROR_COMMUNICATION_FAILURE``, ``-145``, Communication failure with another component.
   ``PSA_ERROR_STORAGE_FAILURE``, ``-146``, Storage failure that may have led to data loss.
   ``PSA_ERROR_HARDWARE_FAILURE``, ``-147``, General hardware failure.
   *Reserved*, ``-148``, Reserved for PSA APIs.
   ``PSA_ERROR_INVALID_SIGNATURE``, ``-149``, "A signature, MAC or hash is incorrect."
   *Reserved*, ``-150``, Reserved for PSA APIs.
   ``PSA_ERROR_CORRUPTION_DETECTED``, ``-151``, Internal data has been tampered with.
   ``PSA_ERROR_DATA_CORRUPT``, ``-152``, Stored data has been corrupted.
   ``PSA_ERROR_DATA_INVALID``, ``-153``,  Data read from storage is not valid.
   *Reserved*, ``-154`` to ``-247``, Reserved for PSA APIs.
   ``PSA_OPERATION_INCOMPLETE``, ``-248``, The requested operation is not finished.
   *SPM Implementation error*, ``-249`` to ``-256``, Reserved for the SPM implementation.
   *API-specific error*, ``<= -257``, API-specific error code.


.. _api:

API Reference
-------------

.. header:: psa/error
   :copyright: Copyright 2017-2022 Arm Limited and/or its affiliates
   :license: Apache-2.0
   :guard:
   :c++:
   :system-include: stddef.h stdint.h

   // This file is a reference template for implementation of the PSA Status code API

These are common status and error codes for all PSA APIs, and for SPM and RoT Service APIs. See :secref:`error-codes` for a summary of the status codes.

The API elements described in the following sections :std:numref:`status-type` to :std:numref:`status-pending`, must be defined in a header file :file:`psa/error.h`. See :secref:`reference-headers` for a reference version of this header file.

It is permitted for these API elements to also be defined in header files that are part of an implementation of another PSA API, for example, in :file:`psa/crypto.h`.

.. admonition:: Implementation note

   In an implementation of any PSA API, it essential that the status code macros are defined precisely as shown in the API specifications and reference header files. In particular, there is no white-space in the definition.

   The C language only permits a macro definition to be repeated within a compilation, if every definition is identical, including the white-space separation.

.. _status-type:

Status type
^^^^^^^^^^^

.. typedef:: int32_t psa_status_t
   :guard: PSA_SUCCESS
   :comment: Prevent multiple definitions of psa_status_t, if PSA_SUCCESS is already defined in an external header

   .. summary:: A status code type used for all PSA APIs.

   A zero or positive value indicates success, the interpretation of the value depends on the specific operation.

   A negative integer value indicates an error.

   .. admonition:: Implementation note

      This definition is permitted to be present in multiple header files that are included in a single compilation.

      To prevent a compilation error from duplicate definitions of `psa_status_t`, the definition of `psa_status_t` must be guarded, by testing for an exiting definition of `PSA_SUCCESS`, in any header file that defines `psa_status_t`.

      The definition of `psa_status_t` above shows the recommended form of the guard.

.. _status-success:

Success code
^^^^^^^^^^^^

.. macro:: PSA_SUCCESS
   :definition: ((psa_status_t)0)

   .. summary:: A status code to indicate general success.

   This is a general return value to indicate success of the operation.

.. _status-errors:

Error codes
^^^^^^^^^^^

.. macro:: PSA_ERROR_PROGRAMMER_ERROR
   :definition: ((psa_status_t)-129)

   .. summary:: A status code that indicates a :scterm:`PROGRAMMER ERROR` in the client.

   This error indicates that the function has detected an abnormal call, which typically indicates a programming error in the caller, or an abuse of the API.

   This error has a specific meaning in an implementation of :cite-title:`PSA FFM`.

.. macro:: PSA_ERROR_CONNECTION_REFUSED
   :definition: ((psa_status_t)-130)

   .. summary:: A status code that indicates that the caller is not permitted to connect to a Service.

   This message has a specific meaning in an implementation of :cite-title:`PSA FFM`.


.. macro:: PSA_ERROR_CONNECTION_BUSY
   :definition: ((psa_status_t)-131)

   .. summary:: A status code that indicates that the caller cannot connect to a service.

   This message has a specific meaning in an implementation of :cite-title:`PSA FFM`.


.. macro:: PSA_ERROR_GENERIC_ERROR
   :definition: ((psa_status_t)-132)

   .. summary:: A status code that indicates an error that does not correspond to any defined failure cause.

   Functions can return this error code if none of the other standard error codes are applicable.

   .. note::

      For new APIs, it is recommended that additional error codes are defined by the API for important error conditions which do not correspond to an existing status code.


.. macro:: PSA_ERROR_NOT_PERMITTED
   :definition: ((psa_status_t)-133)

   .. summary:: A status code that indicates that the requested action is denied by a policy.

   It is recommended that a function returns this error code when the parameters are recognized as valid and supported, and a policy explicitly denies the requested operation.

   If a subset of the parameters of a function call identify a forbidden operation, and another subset of the parameters are not valid or not supported, it is unspecified whether the function returns with `PSA_ERROR_NOT_PERMITTED`, `PSA_ERROR_NOT_SUPPORTED`, or `PSA_ERROR_INVALID_ARGUMENT`.


.. macro:: PSA_ERROR_NOT_SUPPORTED
   :definition: ((psa_status_t)-134)

   .. summary:: A status code that indicates that the requested operation or a parameter is not supported.

   This error code is recommended for indicating that optional functionality in an API specification is not provided by the implementation.

   If a combination of parameters is recognized and identified as not valid, prefer to return `PSA_ERROR_INVALID_ARGUMENT` instead.


.. macro:: PSA_ERROR_INVALID_ARGUMENT
   :definition: ((psa_status_t)-135)

   .. summary:: A status code that indicates that the parameters passed to the function are invalid.

   Functions can return this error any time a parameter or combination of parameters are recognized as invalid.

   It is recommended that a function returns a more specific error code where applicable, for example `PSA_ERROR_INVALID_HANDLE`, `PSA_ERROR_DOES_NOT_EXIST`, or `PSA_ERROR_ALREADY_EXISTS`.


.. macro:: PSA_ERROR_INVALID_HANDLE
   :definition: ((psa_status_t)-136)

   .. summary:: A status code that indicates that a handle parameter is not valid.

   A function can return this error any time it expects a parameter to be a handle to an existing resource, but the handle is invalid.

   This status code is not recommended when a caller is requesting a specific handle value to be used for a new handle. In such cases, it is recommended that a function returns `PSA_ERROR_ALREADY_EXISTS` if value is valid but already in use, and `PSA_ERROR_INVALID_ARGUMENT` or a more specific status code if the value is otherwise invalid.


.. macro:: PSA_ERROR_BAD_STATE
   :definition: ((psa_status_t)-137)

   .. summary:: A status code that indicates that the requested action cannot be performed in the current state.

   It is recommended that a function returns this error when an operation is requested out of sequence.


.. macro:: PSA_ERROR_BUFFER_TOO_SMALL
   :definition: ((psa_status_t)-138)

   .. summary:: A status code that indicates that an output buffer parameter is too small.

   A function can return this error if an output parameter is too small for the requested output data.

   It is recommended that a function only returns this error code in cases where performing the operation with a larger output buffer would succeed. However, a function can also return this error if a function has invalid or unsupported parameters in addition to an insufficient output buffer size.


.. macro:: PSA_ERROR_ALREADY_EXISTS
   :definition: ((psa_status_t)-139)

   .. summary:: A status code that indicates that an identifier or index is already in use.

   A function can return this error if the call is attempting to reuse an identifier or a resource index that is already allocated or in use.

   It is recommended that this error code is not used for a handle or index that is invalid. For these situations, return `PSA_ERROR_PROGRAMMER_ERROR`, `PSA_ERROR_INVALID_HANDLE`, or `PSA_ERROR_INVALID_ARGUMENT`.


.. macro:: PSA_ERROR_DOES_NOT_EXIST
   :definition: ((psa_status_t)-140)

   .. summary:: A status code that indicates that an identified resource does not exist.

   A function can return this error if a request identifies a resource that has not been created or is not present.

   It is recommended that this error code is not used for a handle or index that is invalid. For these situations, return `PSA_ERROR_PROGRAMMER_ERROR`, `PSA_ERROR_INVALID_HANDLE`, or `PSA_ERROR_INVALID_ARGUMENT`.


.. macro:: PSA_ERROR_INSUFFICIENT_MEMORY
   :definition: ((psa_status_t)-141)

   .. summary:: A status code that indicates that there is not enough runtime memory.

   A function can return this error if runtime memory required for the requested operation cannot be allocated.

   If the operation involves multiple components, this error can refer to available memory in any of the components.


.. macro:: PSA_ERROR_INSUFFICIENT_STORAGE
   :definition: ((psa_status_t)-142)

   .. summary:: A status code that indicates that there is not enough persistent storage.

   A function can return this error if the operation involves storing data in non-volatile memory, and when there is insufficient space on the host media.

   Operations that do not directly store persistent data can also return this error code if the implementation requires a mandatory log entry for the requested action and the log storage space is full.


.. macro:: PSA_ERROR_INSUFFICIENT_DATA
   :definition: ((psa_status_t)-143)

   .. summary:: A status code that indicates that a data source has insufficient capacity left.

   A function can return this error if the operation attempts to extract data from a source which has been exhausted.


.. macro:: PSA_ERROR_SERVICE_FAILURE
   :definition: ((psa_status_t)-144)

   .. summary:: A status code that indicates an error within the service.

   A function can return this error if it unable to operate correctly. For example, if an essential initialization operation failed.

   For failures that are related to hardware peripheral errors, it is recommended that the function returns `PSA_ERROR_COMMUNICATION_FAILURE` or `PSA_ERROR_HARDWARE_FAILURE`.


.. macro:: PSA_ERROR_COMMUNICATION_FAILURE
   :definition: ((psa_status_t)-145)

   .. summary:: A status code that indicates a communication failure between the function and another service or component.

   A function can return this error if there is a fault in the communication between the implementation and another service or peripheral used to provide the requested service. A communication failure may be transient or permanent depending on the cause.

   .. warning::
      If a function returns this error, it is undetermined whether the requested action has completed.

      Returning `PSA_SUCCESS` is recommended on successful completion whenever possible. However, a function can return `PSA_ERROR_COMMUNICATION_FAILURE` if the requested action was completed successfully in an external component, but there was a breakdown of communication before this was reported to the application.


.. macro:: PSA_ERROR_STORAGE_FAILURE
   :definition: ((psa_status_t)-146)

   .. summary:: A status code that indicates a storage failure that may have led to data loss.

   A function can return this error to indicate that some persistent storage could not be read or written. It does not indicate the following situations, which have specific error codes:

   *  A corruption of volatile memory --- use `PSA_ERROR_CORRUPTION_DETECTED`.
   *  A communication error between the processor and the storage hardware --- use `PSA_ERROR_COMMUNICATION_FAILURE`.
   *  When the storage is in a valid state but is full --- use `PSA_ERROR_INSUFFICIENT_STORAGE`.
   *  When the storage or stored data is corrupted --- use `PSA_ERROR_DATA_CORRUPT`.
   *  When the stored data is not valid --- use `PSA_ERROR_DATA_INVALID`.

   A storage failure does not indicate that any data that was previously read is invalid. However, this previously read data may no longer be readable from storage.

   It is recommended to only use this error code to report a permanent storage corruption. However, transient errors while reading the storage can also be reported using this error code.


.. macro:: PSA_ERROR_HARDWARE_FAILURE
   :definition: ((psa_status_t)-147)

   .. summary:: A status code that indicates that a hardware failure was detected.

   A function can return this error to report a general hardware fault. A hardware failure may be transient or permanent depending on the cause.


.. macro:: PSA_ERROR_INVALID_SIGNATURE
   :definition: ((psa_status_t)-149)

   .. summary:: A status code that indicates that a signature, MAC or hash is incorrect.

   A function can return this error to report when a verification calculation completes successfully, and the value to be verified is incorrect.


.. macro:: PSA_ERROR_CORRUPTION_DETECTED
   :definition: ((psa_status_t)-151)

   .. summary:: A status code that indicates that internal data has been tampered with.

   A function can return this error if it detects an invalid state that cannot happen during normal operation and that indicates that the implementation's security guarantees no longer hold. Depending on the implementation architecture and on its security and safety goals, the implementation might forcibly terminate the application.

   This error should not be used to indicate a hardware failure that merely makes it impossible to perform the requested operation, instead use `PSA_ERROR_COMMUNICATION_FAILURE`, `PSA_ERROR_STORAGE_FAILURE`, `PSA_ERROR_HARDWARE_FAILURE`, or other applicable error code.

   This error should not be used to report modification of application state, or misuse of the API.

   If an application receives this error code, there is no guarantee that previously accessed or computed data was correct and remains confidential. In this situation, it is recommended that applications perform no further security functions and enter a safe failure state.


.. macro:: PSA_ERROR_DATA_CORRUPT
   :definition: ((psa_status_t)-152)

   .. summary:: A status code that indicates that stored data has been corrupted.

   A function can return this error to indicate that some persistent storage has suffered corruption.  It does not indicate the following situations, which have specific error codes:

   *  A corruption of volatile memory --- use `PSA_ERROR_CORRUPTION_DETECTED`.
   *  A communication error between the processor and its external storage --- use `PSA_ERROR_COMMUNICATION_FAILURE`.
   *  When the storage is in a valid state but is full --- use `PSA_ERROR_INSUFFICIENT_STORAGE`.
   *  When the storage fails for other reasons --- use `PSA_ERROR_STORAGE_FAILURE`.
   *  When the stored data is not valid --- use `PSA_ERROR_DATA_INVALID`.

   Note that a storage corruption does not indicate that any data that was previously read is invalid. However this previously read data might no longer be readable from storage.

   It is recommended to only use this error code to report when a storage component indicates that the stored data is corrupt, or fails an integrity check.


.. macro:: PSA_ERROR_DATA_INVALID
   :definition: ((psa_status_t)-153)

   .. summary:: A status code that indicates that data read from storage is not valid for the implementation.

   This error indicates that some data read from storage does not have a valid format. It does not indicate the following situations, which have specific error codes:

   *  When the storage or stored data is corrupted --- use `PSA_ERROR_DATA_CORRUPT`.
   *  When the storage fails for other reasons --- use `PSA_ERROR_STORAGE_FAILURE`.
   *  An invalid argument to the API --- use `PSA_ERROR_INVALID_ARGUMENT`.

   This error is typically a result of an integration failure, where the implementation reading the data is not compatible with the implementation that stored the data.

   It is recommended to only use this error code to report when data that is successfully read from storage is invalid.


.. _status-pending:

Unfinished operation code
^^^^^^^^^^^^^^^^^^^^^^^^^

.. macro:: PSA_OPERATION_INCOMPLETE
   :definition: ((psa_status_t)-248)

   .. summary:: A status code that indicates that the requested operation is interruptible, and still has work to do.

   This status code does not mean that the operation has failed or that it has succeeded. The operation must be repeated until it completes with either success or failure.

   .. subsection:: Usage

      This is an example of how this status code can be used:

      .. code-block:: xref

         psa_status_t r = start_operation();

         if (r == PSA_SUCCESS) {
            do {
               r = complete_operation();
            } while (r == PSA_OPERATION_INCOMPLETE);
         }

         if (r == PSA_SUCCESS) {
            /* Handle success */
         } else {
            /* Handle errors */
         }
