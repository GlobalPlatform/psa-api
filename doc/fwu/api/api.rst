.. SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/update
   :copyright: Copyright 2020-2025 Arm Limited and/or its affiliates
   :license: Apache-2.0
   :guard:
   :c++:
   :system-include: stdint.h
   :include: psa/error.h

   /* This file is a reference template for implementation of the
    * PSA Firmware Update API v1.0
    */

.. _api-reference:

API reference
=============

To enable implementation optimization for constrained devices, the |API| does not require binary compatibility between different implementations. The |API| is defined as a source-level interface, and applications that target this interface will typically need to be recompiled for different implementations.

.. _api-conventions:

API conventions
---------------

The interface in this specification is defined in terms of C macros, data types, and functions.

Identifier names
^^^^^^^^^^^^^^^^

All of the identifiers defined in the |API| begin with the prefix ``psa_``, for types and functions, or ``PSA_`` for macros.

Future versions of this specification will use the same prefix for additional API elements. It is recommended that applications and implementations do not use this prefix for their own identifiers, to avoid a potential conflict with a future version of the |API|.

Basic types
^^^^^^^^^^^

This specification makes use of standard C data types, including the fixed-width integer types from the ISO C99 specification update :cite:`C99`. The following standard C types are used:

.. csv-table::
   :widths: auto
   :align: left

   ``int32_t``, a 32-bit signed integer
   ``uint8_t``, an 8-bit unsigned integer
   ``uint16_t``, a 16-bit unsigned integer
   ``uint32_t``, a 32-bit unsigned integer
   ``size_t``, an unsigned integer large enough to hold the size of an object in memory

Data types
^^^^^^^^^^

Integral types are defined for specific API elements to provide clarity in the interface definition, and to improve code readability. For example, `psa_fwu_component_t` and :code:`psa_status_t`.

Structure types are declared using ``typedef`` instead of a ``struct`` tag, also to improve code readability.

Fully-defined types must be declared exactly as defined in this specification. Types that are not fully defined in this specification must be defined by an implementation. See :secref:`implementation-defined-type`.

Constants
^^^^^^^^^

Constant values are defined using C macros. Constants defined in this specification have names that are all upper-case.

A constant macro evaluates to a compile-time constant expression.

.. _function-convention:

Functions
^^^^^^^^^

Functions defined in this specification have names that are all lower-case.

An implementation is permitted to declare any API function with ``static inline`` linkage, instead of the default ``extern`` linkage.

An implementation is permitted to also define a function-like macro with the same name as a function in this specification. If an implementation defines a function-like macro for a function from this specification, then:

*  The implementation must also provide a definition of the function. This enables an application to take the address of a function defined in this specification.
*  The function-like macro must expand to code that evaluates each of its arguments exactly once, as if the call was made to a C function. This enables an application to safely use arbitrary expressions as arguments to a function defined in this specification.

If a non-pointer argument to a function has an invalid value (for example, a value outside the domain of the function), then the function will normally return an error, as specified in the function definition.

If a pointer argument to a function has an invalid value (for example, a pointer outside the address space of the program, or a null pointer), the result is :scterm:`IMPLEMENTATION DEFINED`. See also :secref:`pointer-conventions`.

Return status
^^^^^^^^^^^^^

All functions return a status indication of type :code:`psa_status_t`. This is an integer value, with ``0`` (:code:`PSA_SUCCESS`), or a positive value, indicating successful operation, and other values indicating errors.

Unless specified otherwise, if multiple error conditions apply, an implementation is free to return any of the applicable error codes.

If the behavior is undefined --- for example, if a function receives an invalid pointer as a parameter --- this specification does not require that the function will return an error. Implementations are encouraged to return an error or halt the application in a manner that is appropriate for the platform if the undefined behavior condition can be detected. However, application developers need to be aware that undefined behavior conditions cannot be detected in general.

.. _pointer-conventions:

Pointer conventions
^^^^^^^^^^^^^^^^^^^

Unless explicitly stated in the documentation of a function, all pointers must be valid pointers to an object of the specified type.

A parameter is considered to be a *buffer* if it points to an array of bytes. A buffer parameter always has the type ``uint8_t *`` or ``const uint8_t *``, and always has an associated parameter indicating the size of the array. Note that a parameter of type ``void *`` is never considered a buffer.

All parameters of pointer type must be valid non-null pointers, unless the pointer is to a buffer of length ``0`` or the function's documentation explicitly describes the behavior when the pointer is null.

Pointers to input parameters can be in read-only memory. Output parameters must be in writable memory.

The implementation will only access memory referenced by a pointer or buffer parameter for the duration of the function call.

Input buffers are fully consumed by the implementation after a successful function call.

Unless otherwise documented, the content of output parameters is not defined when a function returns an error status. It is recommended that implementations set output parameters to safe defaults to reduce risk, in case the caller does not properly handle all errors.

.. _implementation-defined-type:

Implementation-specific types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This specification defines a number of implementation-specific types, which represent objects whose content depends on the implementation. These are defined as C ``typedef`` types in this specification, with a comment :code:`/* implementation-defined type */` in place of the underlying type definition. For some types the specification constrains the type, for example, by requiring that the type is a ``struct``, or that it is convertible to and from an unsigned integer. In the implementation's version of the |API| header file, these types need to be defined as complete C types so that objects of these types can be instantiated by application code.

Applications that rely on the implementation specific definition of any of these types might not be portable to other implementations of this specification.


Header file
-----------

The header file for the |API| has the name :file:`psa/update.h`. All of the interface elements that are provided by an implementation must be visible to an application program that includes this header file.

.. code-block:: none

   #include "psa/update.h"

Implementations must provide their own version of the :file:`psa/update.h` header file. :secref:`appendix-example-header` provides an incomplete, example header file which includes all of the |API| elements.

This |API| uses some of the common status codes that are defined by :cite-title:`PSA STAT` as part of the :file:`psa/error.h` header file. Applications are not required to explicitly include the :file:`psa/error.h` header file when using these status codes with the |API|. See :secref:`status-codes`.

.. _required_functions:

Required functions
^^^^^^^^^^^^^^^^^^

All of the API elements defined in :secref:`api-reference` must be present for an implementation to claim compliance with this spec.

Mandatory function implementations cannot simply return ``PSA_ERROR_NOT_SUPPORTED``. Optional functions must be present, but are permitted to always return ``PSA_ERROR_NOT_SUPPORTED``.

The following functions are mandatory for all implementations:

*  `psa_fwu_query()`
*  `psa_fwu_start()`
*  `psa_fwu_write()`
*  `psa_fwu_finish()`
*  `psa_fwu_install()`
*  `psa_fwu_cancel()`
*  `psa_fwu_clean()`

If the implementation includes components that use the STAGED state, the following functions are also mandatory:

*  `psa_fwu_reject()`

If the implementation includes components that use the TRIAL state, the following functions are also mandatory:

*  `psa_fwu_reject()`
*  `psa_fwu_accept()`

If the implementation includes components that require a system restart, the following functions are also mandatory:

*  `psa_fwu_request_reboot()`


Library management
------------------

Library version
^^^^^^^^^^^^^^^

.. macro:: PSA_FWU_API_VERSION_MAJOR
   :api-version: major

   .. summary::
      The major version of this implementation of the Firmware Update API.

.. macro:: PSA_FWU_API_VERSION_MINOR
   :api-version: minor

   .. summary::
      The minor version of this implementation of the Firmware Update API.


.. _status-codes:

Status codes
------------

The |API| uses the status code definitions that are shared with the other PSA APIs. The |API| also provides some |API|-specific status codes, see :secref:`specific-errors` and :secref:`specific-success`.

Common status codes
^^^^^^^^^^^^^^^^^^^

The following elements are defined in :file:`psa/error.h` from :cite:`PSA STAT`:

.. code-block:: xref

   typedef int32_t psa_status_t;

   #define PSA_SUCCESS ((psa_status_t)0)

   #define PSA_ERROR_NOT_PERMITTED         ((psa_status_t)-133)
   #define PSA_ERROR_NOT_SUPPORTED         ((psa_status_t)-134)
   #define PSA_ERROR_INVALID_ARGUMENT      ((psa_status_t)-135)
   #define PSA_ERROR_BAD_STATE             ((psa_status_t)-137)
   #define PSA_ERROR_DOES_NOT_EXIST        ((psa_status_t)-140)
   #define PSA_ERROR_INSUFFICIENT_MEMORY   ((psa_status_t)-141)
   #define PSA_ERROR_INSUFFICIENT_STORAGE  ((psa_status_t)-142)
   #define PSA_ERROR_COMMUNICATION_FAILURE ((psa_status_t)-145)
   #define PSA_ERROR_STORAGE_FAILURE       ((psa_status_t)-146)
   #define PSA_ERROR_INVALID_SIGNATURE     ((psa_status_t)-149)

.. admonition:: Implementation note

   An implementation is permitted to define these interface elements within the :file:`psa/update.h` header, or to define them via inclusion of a :file:`psa/error.h` header file that is shared with the implementation of other PSA APIs.


.. _specific-errors:

Error codes specific to the |API|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These error codes are defined in :file:`psa/update.h`.

.. macro:: PSA_ERROR_DEPENDENCY_NEEDED
   :definition: ((psa_status_t)-156)

   .. summary:: A status code that indicates that the firmware of another component requires updating.

   This error indicates that the firmware image depends on a newer version of the firmware for another component. The firmware of the other component must be updated before this firmware image can be installed, or both components must be updated at the same time.

   See :secref:`dependencies` and :secref:`multi-component-updates`.

.. macro:: PSA_ERROR_FLASH_ABUSE
   :definition: ((psa_status_t)-160)

   .. summary:: A status code that indicates that the system is limiting i/o operations to avoid rapid flash exhaustion.

   Excessive i/o operations can cause certain types of flash memories to wear out, resulting in storage device failure. This error code can be used by a system that detects unusually high i/o activity, to reduce the risk of flash exhaustion.

   The time-out period is :scterm:`implementation defined`.

.. macro:: PSA_ERROR_INSUFFICIENT_POWER
   :definition: ((psa_status_t)-161)

   .. summary:: A status code that indicates that the system does not have enough power to carry out the request.

   A function can return this error code if it determines that there is not sufficient power or energy available to reliably complete the operation.

   Operations that update the state of the firmware can require significant energy to reprogram the non-volatile memories. It is recommended to wait until sufficient energy is available for the update process, rather than failing to update the firmware and leaving the device temporarily or permanently non-operational.


.. _specific-success:

Success status codes specific to the |API|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These success codes are defined in :file:`psa/update.h`.

.. macro:: PSA_SUCCESS_REBOOT
   :definition: ((psa_status_t)+1)

   .. summary::
      The action was completed successfully and requires a system reboot to complete installation.


.. macro:: PSA_SUCCESS_RESTART
   :definition: ((psa_status_t)+2)

   .. summary::
      The action was completed successfully and requires a restart of the component to complete installation.


Firmware components
-------------------

Component identifier
^^^^^^^^^^^^^^^^^^^^

.. typedef:: uint8_t psa_fwu_component_t

   .. summary::
      Firmware component type identifier.

   A value of type `psa_fwu_component_t` identifies a firmware component on this device. This is used to specify which component a function call applies to.

   In systems that only have a single component, it is recommended that the caller uses the value ``0`` in calls that require a component identifier.

Component version
^^^^^^^^^^^^^^^^^

.. struct:: psa_fwu_image_version_t
   :type:

   .. summary::
      Version information about a firmware image.

   .. field:: uint8_t major
      The major version of an image.
   .. field:: uint8_t minor
      The minor version of an image. If the image has no minor version then this field is set to ``0``.
   .. field:: uint16_t patch
      The revision or patch version of an image. If the image has no such version then this field is set to ``0``.
   .. field:: uint32_t build
      The build number of an image. If the image has no such number then this field is set to ``0``.

.. _component-states:

Component states
^^^^^^^^^^^^^^^^

Each of the component states defined in :secref:`state-model` has a corresponding identifier in the API. These are used to indicate the state of a component, in the ``state`` field of a `psa_fwu_component_info_t` structure returned by a call to `psa_fwu_query()`.

.. macro:: PSA_FWU_READY
   :definition: 0u

   .. summary:: The READY state: the component is ready to start another update.

   In this state, the update client can start a new firmware update, by calling `psa_fwu_start()`.

.. macro:: PSA_FWU_WRITING
   :definition: 1u

   .. summary:: The WRITING state: a new firmware image is being written to the firmware store.

   In this state, the update client transfers the firmware image to the firmware store, by calling `psa_fwu_write()`.

   When all of the image has been transferred, the update client marks the new firmware image as ready for installation, by calling `psa_fwu_finish()`.

   The update client can abort an update that is in this state, by calling `psa_fwu_cancel()`.

   .. note::
      This state is volatile for components that have :term:`volatile staging`. For other components, it is :scterm:`implementation defined` whether this state is volatile.

      When this state is volatile, the incomplete image is discarded at reboot.

.. macro:: PSA_FWU_CANDIDATE
   :definition: 2u

   .. summary:: The CANDIDATE state: a new firmware image is ready for installation.

   In this state, the update client starts the installation process of the component, by calling `psa_fwu_install()`.

   The update client can abort an update that is in this state, by calling `psa_fwu_cancel()`.

   .. note::
      This state is volatile for components that have :term:`volatile staging`. For other components, it is :scterm:`implementation defined` whether this state is volatile.

      When this state is volatile, the candidate image is discarded at reboot.

.. macro:: PSA_FWU_STAGED
   :definition: 3u

   .. summary:: The STAGED state: a new firmware image is queued for installation.

   A system reboot, or component restart, is required to complete the installation process.

   The update client can abort an update that is in this state, by calling `psa_fwu_reject()`.

   .. note::
      This state is always volatile --- on a reboot the system will attempt to install the new firmware image.

.. macro:: PSA_FWU_FAILED
   :definition: 4u

   .. summary:: The FAILED state: a firmware update has been cancelled or has failed.

   The ``error`` field of the `psa_fwu_component_info_t` structure will contain an status code indicating the reason for the failure.

   The failed firmware image needs to be erased using a call to `psa_fwu_clean()` before another update can be started.

   .. note::
      This state is volatile for components that have :term:`volatile staging`. For other components, it is :scterm:`implementation defined` whether this state is volatile.

      When this state is volatile, the failed firmware image is discarded at reboot.

.. macro:: PSA_FWU_TRIAL
   :definition: 5u

   .. summary:: The TRIAL state: a new firmware image requires testing prior to acceptance of the update.

   In this state, the update client calls `psa_fwu_accept()` or `psa_fwu_reject()` to either accept or reject the new firmware image.

   It is recommended that the new firmware is tested for correct operation, before accepting the update. This is particularly important to for systems that implement an update policy that prevents rollback to old firmware versions.

   .. note::
      This state is always volatile --- on a reboot, a component in this state will be rolled back to the previous firmware image.

.. macro:: PSA_FWU_REJECTED
   :definition: 6u

   .. summary:: The REJECTED state: a new firmware image has been rejected after testing.

   A system reboot, or component restart, is required to complete the process of reverting to the previous firmware image.

   .. note::
      This state is always volatile --- on a reboot, a component in this state will be rolled back to the previous firmware image.

.. macro:: PSA_FWU_UPDATED
   :definition: 7u

   .. summary:: The UPDATED state: a firmware update has been successful, and the new image is now *active*.

   The previous firmware image needs to be erased using a call to `psa_fwu_clean()` before another update can be started.

   .. note::
      This state is volatile for components that have :term:`volatile staging`. For other components, it is :scterm:`implementation defined` whether this state is volatile.

      When this state is volatile, the previously installed firmware image is discarded at reboot.

.. _flags:

Component flags
^^^^^^^^^^^^^^^

These flags can be present in the ``flags`` member of a `psa_fwu_component_info_t` object returned by a call to `psa_fwu_query()`.

.. macro:: PSA_FWU_FLAG_VOLATILE_STAGING
   :definition: 0x00000001u

   .. summary::
      Flag to indicate whether a candidate image in the component :term:`staging area` is discarded at system reset.

   A component with :term:`volatile staging` sets this flag in the `psa_fwu_component_info_t` object returned by a call to `psa_fwu_query`.

   If this flag is set, then image data written to the staging area is discarded after a system reset. If the system restarts while the component in is WRITING, CANDIDATE, FAILED, or UPDATED state, the component will be in the READY state after the restart.

   If this flag is not set, then an image in CANDIDATE state is retained after a system reset. It is :scterm:`implementation defined` whether a partially prepared image in WRITING state, or a discarded image in FAILED or UPDATED state, is retained after a system reset.

.. macro:: PSA_FWU_FLAG_ENCRYPTION
   :definition: 0x00000002u

   .. summary::
      Flag to indicate whether a firmware component expects encrypted images during an update.

   If set, then the firmware image for this component must be encrypted when installing.

   If not set, then the firmware image for this component must not be encrypted when installing.

.. _store_info:

Component information
^^^^^^^^^^^^^^^^^^^^^

.. typedef:: struct { /* implementation-defined type */ } psa_fwu_impl_info_t

   .. summary::
      The implementation-specific data in the component information structure.

   The members of this data structure are :scterm:`implementation defined`. This can be an empty data structure.

.. struct:: psa_fwu_component_info_t
   :type:

   .. summary::
      Information about the firmware store for a firmware component.

   .. field:: uint8_t state
      State of the component. This is one of the values defined in :secref:`component-states`.
   .. field:: psa_status_t error
      Error for *second* image when store state is REJECTED or FAILED.
   .. field:: psa_fwu_image_version_t version
      Version of *active* image.
   .. field:: uint32_t max_size
      Maximum image size in bytes.
   .. field:: uint32_t flags
      Flags that describe extra information about the firmware component. See :secref:`flags` for defined flag values.
   .. field:: uint32_t location
      Implementation-defined image location.

   .. field:: psa_fwu_impl_info_t impl
      Reserved for implementation-specific usage. For example, provide information about image encryption or compression.

   The attributes of a component are retrieved using a call to `psa_fwu_query()`.

   .. rationale::

      When a component is in a state that is not READY, there is a *second* image, or partial image, present in the firmware store. The |API| provides no mechanism to report the version of the *second* image, for the following reasons:

      *  During preparation of a new firmware image, the implementation is not required to extract version information from the firmware image manifest:

         -  This information might not be available if the firmware image has not been completely written.
         -  The update service might not be capable of extracting the version information. For example, in the untrusted-staging deployment model, verification of the manifest can be deferred until the image is installed. See :secref:`untrusted-staging`.

         If the version of an image that is being prepared is required by the update client, the update client must maintain this information locally.

      *  In TRIAL or REJECTED states, the *second* image is the previously installed firmware, which is required in case of rollback. Reporting the version of this is not required by the update client.

      *  In UPDATED or FAILED states, the *second* image needs to be erased. The version of the image data in this state has no effect on the behavior of the update client.


.. function:: psa_fwu_query

   .. summary::
      Retrieve the firmware store information for a specific firmware component.

   .. param:: psa_fwu_component_t component
      Firmware component for which information is requested.
   .. param:: psa_fwu_component_info_t *info
      Output parameter for component information.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Component information has been returned in the `psa_fwu_component_t` object at ``*info``.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.

   This function is used to query the status of a component.

   The caller is expected to know the component identifiers for all of the firmware components. This information might be built into the update client, provided by configuration data, or provided alongside the firmware images from the update server.


.. _api-functions:

Firmware installation
---------------------

Each of the component operations defined in :secref:`state-model` has a corresponding function in the API, described in sections :numref:`image-prep` to :numref:`image-trial`.

.. _image-prep:

Candidate image preparation
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following functions are used to prepare a new candidate firmware image in the component's firmware store. They act on a single component, specified by a component identifier parameter.

.. function:: psa_fwu_start

   .. summary::
      Begin a firmware update operation for a specific firmware component.

   .. param:: psa_fwu_component_t component
      Identifier of the firmware component to be updated.
   .. param:: const void *manifest
      A pointer to a buffer containing a detached manifest for the update. If the manifest is bundled with the firmware image, ``manifest`` must be ``NULL``.
   .. param:: size_t manifest_size
      The size of the detached manifest. If the manifest is bundled with the firmware image, ``manifest_size`` must be ``0``.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the component is now in WRITING state, and ready for the new image to be transferred using `psa_fwu_write()`.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_BAD_STATE
      The component is not in the READY state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The following conditions can result in this error:

      *  The caller is not authorized to call this function.
      *  The provided manifest is valid, but fails to comply with the update service's firmware update policy.
   .. retval:: PSA_ERROR_INVALID_SIGNATURE
      A signature or integrity check on the manifest has failed.
   .. retval:: PSA_ERROR_INVALID_ARGUMENT
      The following conditions can result in this error:

      *  The provided manifest is unexpected, or invalid.
      *  A detached manifest was expected, but none was provided.
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used to begin the process of preparing a new firmware image for a component, optionally providing a detached manifest. On success, the component is in WRITING state, and the update client can call `psa_fwu_write()` to transfer the new firmware image.

   If the firmware image :term:`manifest` is detached from the firmware image, it must be provided to the update service using the ``manifest`` and ``manifest_size`` parameters in `psa_fwu_start()`.

   If a detached manifest is expected by the update service for a firmware component, but none is provided, `psa_fwu_start()` returns :code:`PSA_ERROR_INVALID_ARGUMENT`. If a detached manifest is provided for a component which expects the manifest to be bundled with the image, `psa_fwu_start()` returns :code:`PSA_ERROR_INVALID_ARGUMENT`.

   To abandon an update that has been started, call `psa_fwu_cancel()`, and then `psa_fwu_clean()`.

.. macro:: PSA_FWU_LOG2_WRITE_ALIGN
   :definition: /* implementation-defined value */

   .. summary::
      Base-2 logarithm of the required alignment of firmware image data blocks when calling `psa_fwu_write()`.

   This value specifies the minimum alignment of a data block within a firmware image, when written using `psa_fwu_write()`. The value is the base-2 log of the alignment size. `PSA_FWU_LOG2_WRITE_ALIGN` is used to constrain the values of ``image_offset`` that are supported, and the handling of a data block of unaligned size, as follows:

   *  Let :code:`WRITE_ALIGN_MASK = (1<<PSA_FWU_LOG2_WRITE_ALIGN) - 1`
   *  If :code:`(image_offset & WRITE_ALIGN_MASK) != 0`, then the implementation returns :code:`PSA_ERROR_INVALID_ARGUMENT`.
   *  If :code:`(block_size & WRITE_ALIGN_MASK) != 0`, then the implementation will pad the data with :scterm:`implementation defined` values up to the next aligned size, before writing the data to the firmware image.
   *  This value does **not** constrain the alignment of the data buffer, ``block``.

   The specific value of `PSA_FWU_LOG2_WRITE_ALIGN` is an :scterm:`implementation defined`, non-negative integer. If an implementation has no alignment requirement, then it defines `PSA_FWU_LOG2_WRITE_ALIGN` to be ``0``.

   .. admonition:: Implementation note

      It is recommended that `PSA_FWU_LOG2_WRITE_ALIGN` is not greater than ``17``, which corresponds to a block size of 128 KB. This limit ensures compatibility with block-based file transfer protocols that are used within IoT systems.

   .. rationale::

      This value is the minimum size and alignment for writing image data to the firmware store. For example, this can be set to ``3`` for an implementation where the non-volatile storage used for the firmware store only supports aligned, 64-bit writes.

      For a component that has a non-volatile WRITING state, the data passed to `psa_fwu_write()` must be written into non-volatile storage. If this is not aligned with the blocks of storage, this can result in significant complexity and cost in the implementation.

      Aligning the provided data blocks with `PSA_FWU_LOG2_WRITE_ALIGN` is the minimum requirement for a client. The method demonstrated in the :secref:`example-multi-write` example, using blocks of size `PSA_FWU_MAX_WRITE_SIZE` until the final block, always satisfies the alignment requirement.

.. macro:: PSA_FWU_MAX_WRITE_SIZE
   :definition: /* implementation-defined value */

   .. summary::
      The maximum permitted size for ``block`` in `psa_fwu_write()`, in bytes.

   The specific value is an :scterm:`implementation defined` unsigned integer, and is greater than ``0``. The value must satisfy the condition :code:`(PSA_FWU_MAX_WRITE_SIZE & ((1<<PSA_FWU_LOG2_WRITE_ALIGN) - 1)) == 0`.

   .. admonition:: Implementation note

      This value is the maximum size for transferring data to the update service. The reasons for selecting a particular value can include the following:

      *  The size of the available RAM buffer within the update service used for storing the data into the firmware store.
      *  A value that is optimized for storing the data in the firmware store, for example, a multiple of the block-size of the storage media.

.. function:: psa_fwu_write

   .. summary::
      Write a firmware image, or part of a firmware image, to its staging area.

   .. param:: psa_fwu_component_t component
      Identifier of the firmware component being updated.
   .. param:: size_t image_offset
      The offset of the data block in the whole image. The offset of the first block is ``0``.

      The offset must be a multiple of the image alignment size, :code:`(1<<PSA_FWU_LOG2_WRITE_ALIGN)`.
   .. param:: const void *block
      A buffer containing a block of image data. This can be a complete image or part of the image.
   .. param:: size_t block_size
      Size of ``block``, in bytes.

      ``block_size`` must not be greater than `PSA_FWU_MAX_WRITE_SIZE`.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the data in ``block`` has been successfully stored.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_BAD_STATE
      The component is not in the WRITING state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.
   .. retval:: PSA_ERROR_INVALID_ARGUMENT
      The following conditions can result in this error:

      *  The parameter ``image_offset`` is not a multiple of :code:`(1<<PSA_FWU_LOG2_WRITE_ALIGN)`.
      *  The parameter ``block_size`` is greater than `PSA_FWU_MAX_WRITE_SIZE`.
      *  The parameter ``block_size`` is ``0``.
      *  The image region specified by ``image_offset`` and ``block_size`` does not lie inside the supported image storage.
   .. retval:: PSA_ERROR_FLASH_ABUSE
      The system has temporarily limited i/o operations to avoid rapid flash exhaustion.
   .. retval:: PSA_ERROR_INVALID_SIGNATURE
      A signature or integrity check on the provided data has failed.
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used to transfer all, or part, of a firmware image to the component's firmware store. On success, the component remains in WRITING state. Once all of the firmware image has been written to the store, a call to `psa_fwu_finish()` is required to continue the installation process.

   If the image size is less than or equal to `PSA_FWU_MAX_WRITE_SIZE`, the caller can provide the entire image in one call.

   If the image size is greater than `PSA_FWU_MAX_WRITE_SIZE`, the caller must provide the image in parts, by calling `psa_fwu_write()` multiple times with different data blocks.

   Write operations can take an extended execution time on flash memories. The caller can provide data in blocks smaller than `PSA_FWU_MAX_WRITE_SIZE` to reduce the time for each call to `psa_fwu_write()`.

   The ``image_offset`` of a data block must satisfy the firmware image alignment requirement, provided by `PSA_FWU_LOG2_WRITE_ALIGN`. If the ``block_size`` of a data block is not aligned, the data is padded with an :scterm:`implementation defined` value. It is recommended that a client only provides a block with an unaligned size when it is the final block of a firmware image.

   When data is written in multiple calls to `psa_fwu_write()`, it is the caller's responsibility to account for how much data is written at which offset within the image.

   On error, the component can remain in WRITING state. In this situation, it is not possible to determine how much of the data in ``block`` has been written to the staging area. It is :scterm:`implementation defined` whether repeating the write operation again with the same data at the same offset will correctly store the data to the staging area.

   If the data fails an integrity check, the implementation is permitted to transition the component to the FAILED state. From this state, the caller is required to use `psa_fwu_clean()` to return the store to READY state before attempting another firmware update.

   To abandon an update that has been started, call `psa_fwu_cancel()` and then `psa_fwu_clean()`.

.. function:: psa_fwu_finish

   .. summary::
      Mark a firmware image in the staging area as ready for installation.

   .. param:: psa_fwu_component_t component
      Identifier of the firmware component to install.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      The operation completed successfully: the component is now in CANDIDATE state.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_BAD_STATE
      The component is not in the WRITING state.
   .. retval:: PSA_ERROR_INVALID_SIGNATURE
      A signature or integrity check for the image has failed.
   .. retval:: PSA_ERROR_INVALID_ARGUMENT
      The firmware image is not valid.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The following conditions can result in this error:

      * The caller is not authorized to call this function.
      * The firmware image is valid, but fails to comply with the update service's firmware update policy. For example, the update service can deny the installation of older versions of firmware (rollback prevention).
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used to complete the preparation of the candidate firmware image for a component. On success, the component is in CANDIDATE state, and the update client calls `psa_fwu_install()` to initiate the installation process.

   The validity, authenticity and integrity of the image can be checked during this operation. If this verification fails, the component is transitioned to the FAILED state. From the FAILED state, the caller is required to use `psa_fwu_clean()` to return the component to READY state before attempting another firmware update.

   Dependencies on other firmware components are not checked as part of `psa_fwu_finish()`. If the implementation provides dependency verification, this is done as part of `psa_fwu_install()`, or during installation at reboot.

   To abandon an update that is in CANDIDATE state, call `psa_fwu_cancel()` and then `psa_fwu_clean()`.

.. function:: psa_fwu_cancel

   .. summary::
      Abandon an update that is in WRITING or CANDIDATE state.

   .. param:: psa_fwu_component_t component
      Identifier of the firmware component to be cancelled.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the new firmware image is rejected. The component is now in FAILED state.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_BAD_STATE
      The component is not in the WRITING or CANDIDATE state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.

   This function is used when the caller wants to abort an incomplete update process, for a component in WRITING or CANDIDATE state. This will discard the uninstalled image or partial image, and leave the component in FAILED state. To prepare for a new update after this, call `psa_fwu_clean()`.

.. function:: psa_fwu_clean

   .. summary::
      Prepare the component for another update.

   .. param:: psa_fwu_component_t component
      Identifier of the firmware component to tidy up.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the staging area is ready for a new update. The component is now in state READY.
   .. retval:: PSA_ERROR_DOES_NOT_EXIST
      There is no firmware component with the specified Id.
   .. retval:: PSA_ERROR_BAD_STATE
      The component is not in the FAILED or UPDATED state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.
   .. retval:: PSA_ERROR_INSUFFICIENT_POWER
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used to ensure that the component is ready to start another update process, after an update has succeeded, failed, or been rejected.

   If the implementation needs to perform long-running operations to erase firmware store memories, it is recommended that this is done as part of `psa_fwu_clean()`, rather than during other operations. This enables the update client to schedule this long-running operation at a time when this is less disruptive to the application.

   If this function is called when the component state is FAILED, then the staging area is cleaned, leaving the current *active* image installed.

   If this function is called when the component state is UPDATED, then the previously installed image is cleaned, leaving the new *active* image installed.


Image installation
^^^^^^^^^^^^^^^^^^

The following functions are used to install candidate firmware images. They act concurrently on all components that have been prepared as candidates for installation.

.. function:: psa_fwu_install

   .. summary::
      Start the installation of all candidate firmware images.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      The installation completed successfully: the affected components are now in TRIAL or UPDATED state.
   .. retval:: PSA_SUCCESS_REBOOT
      The installation has been initiated, but a system reboot is needed to complete the installation. The affected components are now in STAGED state.

      A system reboot can be requested using `psa_fwu_request_reboot()`.
   .. retval:: PSA_SUCCESS_RESTART
      The installation has been initiated, but the components must be restarted to complete the installation. The affected components are now in STAGED state.

      The component restart mechanism is :scterm:`implementation defined`.
   .. retval:: PSA_ERROR_BAD_STATE
      The following conditions can result in this error:

      *  An existing installation process is in progress: there is at least one component in STAGED, TRIAL, or REJECTED state.
      *  There is no component in the CANDIDATE state.
   .. retval:: PSA_ERROR_INVALID_SIGNATURE
      A signature or integrity check for the image has failed.
   .. retval:: PSA_ERROR_DEPENDENCY_NEEDED
      A different firmware image must be installed first.
   .. retval:: PSA_ERROR_INVALID_ARGUMENT
      The firmware image is not valid.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The following conditions can result in this error:

      * The caller is not authorized to call this function.
      * The firmware image is valid, but fails to comply with the update service's firmware update policy. For example, the update service can deny the installation of older versions of firmware (rollback prevention).
   .. retval:: PSA_ERROR_INSUFFICIENT_POWER
      The system does not have enough power to safely install the firmware.
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function starts the installation process atomically on all components that are in CANDIDATE state. This function reports an error if there are no components in this state. If an error occurs when installing any of the images, then none of the images will be installed.

   Only one installation process can be in progress at a time. After a successful call to `psa_fwu_install()`, another call is only permitted once the affected components have transitioned to FAILED, UPDATED, or READY state.

   Support for concurrent installation of multiple components is :scterm:`implementation defined`. Concurrent installation enables new firmware images that are interdependent to be installed. If concurrent installation is not supported, each new firmware image must be compatible with the current version of other firmware components in the system.

   Device updates that affect multiple components must be carried out in line with the system capabilities. For example:

   *  An implementation is permitted to require each component to be installed separately.
   *  An implementation is permitted to support atomic installation of any combination of components.
   *  An implementation is permitted to support atomic installation of a specific subset of components, but require other components to be installed individually

   The validity, authenticity and integrity of the images can be checked during this operation. If this verification fails, the components are transitioned to the FAILED state. From the FAILED state, the caller is required to use `psa_fwu_clean()` on each component to return them to the READY state before attempting another firmware update.

   Dependencies on other firmware components can be checked as part of `psa_fwu_install()`. The dependency check is carried out against the version of the candidate image for a component that is in CANDIDATE state, and the *active* image for other components. If this verification fails, then `PSA_ERROR_DEPENDENCY_NEEDED` is returned, and the components will remain in CANDIDATE state. A later call to `psa_fwu_install()` can be attempted after preparing a new firmware image for the dependency.

   On other error conditions, it is :scterm:`implementation defined` whether the components are all transitioned to FAILED state, or all remain in CANDIDATE state. See :secref:`behavior-on-error`.

   If a component restart, or system reboot, is required to complete installation then the implementation is permitted to defer verification checks to that point. Verification failures during a reboot will result in the components being transitioned to FAILED state. The failure reason is recorded in the ``error`` field in the `psa_fwu_component_info_t` object for each firmware component, which can be queried by the update client after restart.

   To abandon an update that is STAGED, before restarting the system or component, call `psa_fwu_reject()` and then `psa_fwu_clean()` on each component.

.. function:: psa_fwu_request_reboot

   .. summary::
      Requests the platform to reboot.

   .. return:: psa_status_t
      Result status. It is :scterm:`implementation defined` whether this function returns to the caller.
   .. retval:: PSA_SUCCESS
      The platform will reboot soon.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.
   .. retval:: PSA_ERROR_NOT_SUPPORTED
      This function call is not implemented.

   On success, the platform initiates a reboot, and might not return to the caller.

   .. admonition:: Implementation note

      This function is mandatory in an implementation where one or more components require a system reboot to complete installation.

      On other implementations, this function is optional.

      See :secref:`required_functions`.

.. function:: psa_fwu_reject

   .. summary::
      Abandon an installation that is in STAGED or TRIAL state.

   .. param:: psa_status_t error
      An application-specific error code chosen by the application. If a specific error does not need to be reported, the value should be 0. On success, this error is recorded in the ``error`` field of the `psa_fwu_component_info_t` structure corresponding to each affected component.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the new firmware images are rejected, and the previous firmware is now *active*. The affected components are now in FAILED state.
   .. retval:: PSA_SUCCESS_REBOOT
      The new firmware images are rejected, but a system reboot is needed to complete the rollback to the previous firmware. The affected components are now in REJECTED state.

      A system reboot can be requested using `psa_fwu_request_reboot()`.
   .. retval:: PSA_SUCCESS_RESTART
      The new firmware images are rejected, but the components must be restarted to complete the rollback to the previous firmware. The affected components are now in REJECTED state.

      The component restart mechanism is :scterm:`implementation defined`.
   .. retval:: PSA_ERROR_BAD_STATE
      There are no components in the STAGED or TRIAL state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.
   .. retval:: PSA_ERROR_NOT_SUPPORTED
      This function call is not implemented.
   .. retval:: PSA_ERROR_INSUFFICIENT_POWER
      The system does not have enough power to safely uninstall the firmware.
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used in the following situations:

   *  When the caller wants to abort an incomplete update process, for components in STAGED state. This will discard the uninstalled images.
   *  When the caller detects an error in new firmware that is in TRIAL state.

   If this function is called when the installation state is STAGED, then the state of affected components changes to FAILED. To prepare for a new update after this, call `psa_fwu_clean()` for each component.

   If this function is called when the installation state is TRIAL, then the action depends on whether a reboot or component restart is required to complete the rollback process:

   *  If a reboot is required, the state of affected components changes to REJECTED and `PSA_SUCCESS_REBOOT` is returned. To continue the rollback process, call `psa_fwu_request_reboot()`.

      After reboot, the affected components will be in FAILED state. To prepare for a new update after this, call `psa_fwu_clean()` for each component.
   *  If a component restart is required, the state of affected components changes to REJECTED and `PSA_SUCCESS_RESTART` is returned. To continue the rollback process, restart the affected components.

      After restart, the affected components will be in FAILED state. To prepare for a new update after this, call `psa_fwu_clean()` for each component.
   *  If no reboot or component restart is required, the state of affected components changes to FAILED and :code:`PSA_SUCCESS` is returned. To prepare for a new update after this, call `psa_fwu_clean()` for each component.

   .. admonition:: Implementation note

      This function is mandatory in an implementation for which any of the following are true:

      *  One or more components have a TRIAL state
      *  One or more components require a system reboot to complete installation
      *  One or more components require a component restart to complete installation

      On implementations where none of these hold, this function is optional.

      See :secref:`required_functions`.


.. _image-trial:

Image trial
^^^^^^^^^^^

The following function is used to manage a trial of new firmware images. It acts atomically on all components that are in TRIAL state.

.. function:: psa_fwu_accept

   .. summary::
      Accept a firmware update that is currently in TRIAL state.

   .. return:: psa_status_t
      Result status.
   .. retval:: PSA_SUCCESS
      Success: the affected components are now in UPDATED state.
   .. retval:: PSA_ERROR_BAD_STATE
      There are no components in the TRIAL state.
   .. retval:: PSA_ERROR_NOT_PERMITTED
      The caller is not authorized to call this function.
   .. retval:: PSA_ERROR_NOT_SUPPORTED
      This function call is not implemented.
   .. retval:: PSA_ERROR_INSUFFICIENT_POWER
      The system does not have enough power to safely update the firmware.
   .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
   .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
   .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
   .. retval:: PSA_ERROR_STORAGE_FAILURE

   This function is used when new firmware images in TRIAL state have been determined to be functional, to permanently accept the new firmware images. If successful, the state of affected components changes to UPDATED. To prepare for another update after this, call `psa_fwu_clean()` for each component.

   For firmware components in TRIAL state, if `psa_fwu_accept()` is not called, then rebooting the system results in the image being automatically rejected. To explicitly reject a firmware update in TRIAL state, call `psa_fwu_reject()`.

   .. admonition:: Implementation note

      This function is mandatory in an implementation where one or more components have a TRIAL state.

      On implementations where none of these hold, this function is optional.

      See :secref:`required_functions`.
