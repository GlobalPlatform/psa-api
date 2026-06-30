.. SPDX-FileCopyrightText: Copyright 2018-2019, 2022-2025 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

API Reference
=============

Status codes
------------

The |API| uses the status code definitions that are shared with the other PSA APIs.

The following elements are defined in :file:`psa/error.h` from :cite-title:`PSA STAT`:

.. code-block:: xref

   typedef int32_t psa_status_t;

   #define PSA_SUCCESS ((psa_status_t)0)

   #define PSA_ERROR_GENERIC_ERROR         ((psa_status_t)-132)
   #define PSA_ERROR_NOT_PERMITTED         ((psa_status_t)-133)
   #define PSA_ERROR_NOT_SUPPORTED         ((psa_status_t)-134)
   #define PSA_ERROR_INVALID_ARGUMENT      ((psa_status_t)-135)
   #define PSA_ERROR_DOES_NOT_EXIST        ((psa_status_t)-140)
   #define PSA_ERROR_INSUFFICIENT_STORAGE  ((psa_status_t)-142)
   #define PSA_ERROR_STORAGE_FAILURE       ((psa_status_t)-146)
   #define PSA_ERROR_INVALID_SIGNATURE     ((psa_status_t)-149)
   #define PSA_ERROR_DATA_CORRUPT          ((psa_status_t)-152)

These definitions must be available to an application that includes either of the :file:`psa/internal_trusted_storage.h` or :file:`psa/protected_storage.h` header files.

.. admonition:: Implementation note

   An implementation is permitted to define the status code interface elements within the |API| header files, or to define them via inclusion of a :file:`psa/error.h` header file that is shared with the implementation of other PSA APIs.

General Definitions
-------------------

.. header:: psa/storage_common
    :copyright: Copyright 2019 Arm Limited and/or its affiliates
    :license: Apache-2.0
    :c++:
    :guard:
    :system-include: stddef.h stdint.h

    /* This file is a reference template for implementation of the
     * PSA Secure Storage API v1.0
     *
     * This file includes common definitions
     */

These definitions must be defined in the header file :file:`psa/storage_common.h`.

.. struct:: psa_storage_info_t

    .. summary::
        A container for metadata associated with a specific ``uid``.

    .. field:: size_t capacity
        The allocated capacity of the storage associated with a ``uid``.
    .. field:: size_t size
        The size of the data associated with a ``uid``.
    .. field:: psa_storage_create_flags_t flags
        The flags set when the ``uid`` was create



.. typedef:: uint32_t psa_storage_create_flags_t

    .. summary::
        Flags used when creating a data entry.


.. typedef:: uint64_t psa_storage_uid_t

    .. summary::
        A type for ``uid`` used for identifying data.


.. macro:: PSA_STORAGE_FLAG_NONE
    0u

    No flags to pass.

.. macro:: PSA_STORAGE_FLAG_WRITE_ONCE
    (1u << 0)

    The data associated with the ``uid`` will not be able to be modified or deleted. Intended to be used to set bits in `psa_storage_create_flags_t`.

.. macro:: PSA_STORAGE_FLAG_NO_CONFIDENTIALITY
    (1u << 1)

    The data associated with the ``uid`` is public and therefore does not require confidentiality. It therefore only needs to be integrity protected.

.. macro:: PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION
    (1u << 2)

    The data associated with the ``uid`` does not require replay protection. This can permit faster storage --- but it permits an attacker with physical access to revert to an earlier version of the data.

.. macro:: PSA_STORAGE_SUPPORT_SET_EXTENDED
    (1u << 0)

    Flag indicating that `psa_ps_create()` and `psa_ps_set_extended()` are supported.

.. _ITS-API:

Internal Trusted Storage API
----------------------------

.. header:: psa/internal_trusted_storage
    :copyright: Copyright 2019 Arm Limited and/or its affiliates
    :license: Apache-2.0
    :c++:
    :guard:
    :system-include: stddef.h stdint.h
    :include: psa/error.h psa/storage_common.h

    /* This file is a reference template for implementation of the
     * PSA Secure Storage API v1.0
     *
     * This file describes the Internal Trusted Storage API
     */

These definitions must be defined in the header file :file:`psa/internal_trusted_storage.h`.


.. macro:: PSA_ITS_API_VERSION_MAJOR
    :api-version: major

    .. summary::
        The major version number of the Internal Trusted Storage API.

    It will be incremented on significant updates that can include breaking changes.

.. macro:: PSA_ITS_API_VERSION_MINOR
    :api-version: minor

    .. summary::
        The minor version number of the Internal Trusted Storage API.

    It will be incremented in small updates that are unlikely to include breaking changes.


.. function:: psa_its_set

    .. summary::
        Set the data associated with the specified ``uid``.

    .. param:: psa_storage_uid_t uid
        The identifier for the data.
    .. param:: size_t data_length
        The size in bytes of the data in ``p_data``.
        If ``data_length == 0`` the implementation will create a zero-length asset associated with the ``uid``.
        While no data can be stored in such an asset, a call to `psa_its_get_info()` will return ``PSA_SUCCESS``.
    .. param:: const void * p_data
        A buffer of ``data_length`` containing the data to store.
    .. param:: psa_storage_create_flags_t create_flags
        The flags that the data will be stored with.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The operation failed because the provided ``uid`` value was already created with `PSA_STORAGE_FLAG_WRITE_ONCE`.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The operation failed because one or more of the flags provided in ``create_flags`` is not supported or is not valid.
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
        The operation failed because there was insufficient space on the storage medium.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * the ``uid`` is ``0``.

        * caller cannot access some or all of the memory in the range [``p_data``, ``p_data + data_length - 1``].

    Stores data in the internal storage.

    * The ``uid`` value must not be zero.

    * If ``uid`` exists it must not have been created as with `PSA_STORAGE_FLAG_WRITE_ONCE` --- would result in ``PSA_ERROR_NOT_PERMITTED``

    * The caller must have access all memory from ``p_data`` to ``p_data + data_length``.

    * Even if all parameters are correct, the function can fail if there is insufficient storage space or in the case of a storage failure.


.. function:: psa_its_get

    .. summary::
        Retrieve data associated with a provided ``uid``.

    .. param:: psa_storage_uid_t uid
        The ``uid`` value.
    .. param:: size_t data_offset
        The starting offset of the data requested.
    .. param:: size_t data_size
        The amount of data requested.
    .. param:: void * p_data
        On success, the buffer where the data will be placed.
    .. param:: size_t * p_data_length
        On success, this will contain size of the data placed in ``p_data``.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The caller cannot access some or all of the memory in the range [``p_data``, ``p_data + data_size - 1``].

        * ``data_offset`` is larger than the size of the data associated with ``uid``.


    Retrieves data associated with ``uid``, starting at ``data_offset`` bytes from the beginning of the data. Fetches the lesser of ``data_size`` or ``uid.size - data_offset`` bytes, which can be zero.

    `psa_its_get()` must not return bytes from beyond the end of ``uid``.

    Upon successful completion, the data will be placed in the ``p_data`` buffer, which must be at least ``data_size`` bytes in size. The length of the data returned will be in ``p_data_length``. Any bytes beyond ``p_data_length`` are left unmodified.

    If ``data_size`` is ``0`` or ``data_offset == uid.size``, the contents of ``p_data_length`` will be set to zero, but the contents of ``p_data`` are unchanged. The function returns ``PSA_SUCCESS``.

    * The ``uid`` value must not be zero.

    * The value of ``data_offset`` must be less than or equal to the length of ``uid``.

    * If ``data_ffset`` is greater than ``uid.size``, no data is retrieved and the functions returns PSA_INVALID_ARGUMENT.

    * If ``data_size`` is not zero, ``p_data`` must mot be ``NULL``.

    * The call must have access to the memory from ``p_data`` to ``p_data + data_size - 1``.

    * If the location ``uid`` exists the lesser of ``data_size`` or ``uid.size - data_offset`` bytes are written to the output buffer and ``p_data_length`` is set to the number of bytes written, which can be zero.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.


.. function:: psa_its_get_info

    .. summary::
        Retrieve the metadata about the provided ``uid``.

    .. param:: psa_storage_uid_t uid
        The ``uid`` value.
    .. param:: struct psa_storage_info_t * p_info
        A pointer to the `psa_storage_info_t` struct that will be populated with the metadata.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The caller cannot access some or all of the memory in the range [``p_info``, :code:`p_info + sizeof(psa_storage_info_t) - 1`]

    Retrieves the metadata stored for a given ``uid`` as a `psa_storage_info_t` structure.

    * The ``uid`` value must not be zero.

    * The call must have access to the memory from ``p_info`` to :code:`p_info + sizeof(psa_storage_info_t) - 1`.

    * If the location ``uid`` exists the metadata for the object is written to ``p_info``.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.


.. function:: psa_its_remove

    .. summary::
        Remove the provided ``uid`` and its associated data from the storage.

    .. param:: psa_storage_uid_t uid
        The ``uid`` value.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The ``uid`` is ``0``.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The operation failed because the provided ``uid`` value was created with `PSA_STORAGE_FLAG_WRITE_ONCE`.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).

    Deletes the data from internal storage.

    * The ``uid`` value must not be zero.

    * If ``uid`` exists it and any metadata are removed from storage.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.

.. _PS-API:

Protected Storage API
---------------------

.. header:: psa/protected_storage
    :copyright: Copyright 2019 Arm Limited and/or its affiliates
    :license: Apache-2.0
    :c++:
    :guard:
    :system-include: stddef.h stdint.h
    :include: psa/error.h psa/storage_common.h

    /* This file is a reference template for implementation of the
     * PSA Secure Storage API v1.0
     *
     * This file describes the Protected Storage API
     */

These definitions must be defined in the header file :file:`psa/protected_storage.h`.

.. macro:: PSA_PS_API_VERSION_MAJOR
    :api-version: major

    .. summary::
        The major version number of the Protected Storage API.

    It will be incremented on significant updates that can include breaking changes.

.. macro:: PSA_PS_API_VERSION_MINOR
    :api-version: minor

    .. summary::
        The minor version number of the Protected Storage API.

    It will be incremented in small updates that are unlikely to include breaking changes.

.. function:: psa_ps_set

    .. summary::
        Set the data associated with the specified ``uid``.

    .. param:: psa_storage_uid_t uid
        The identifier for the data.
    .. param:: size_t data_length
        The size in bytes of the data in ``p_data``.
        If ``data_length == 0`` the implementation will create a zero-length asset associated with the ``uid``.
        While no data can be stored in such an asset, a call to `psa_ps_get_info()` will return ``PSA_SUCCESS``.
    .. param:: const void * p_data
        A buffer containing the data.
    .. param:: psa_storage_create_flags_t create_flags
        The flags indicating the properties of the data.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The operation failed because the provided ``uid`` value was already created with `PSA_STORAGE_FLAG_WRITE_ONCE`.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The operation failed because caller cannot access some or all of the memory in the range [``p_data``, ``p_data + data_length - 1``].
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The operation failed because one or more of the flags provided in ``create_flags`` is not supported or is not valid.
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
        The operation failed because there was insufficient space on the storage medium.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation failed because of an unspecified internal failure.

    The newly created asset has a capacity and size that are equal to ``data_length``.


    * The ``uid`` value must not be zero.

    * If ``uid`` exists it must not have been created as with `PSA_STORAGE_FLAG_WRITE_ONCE` - would result in ``PSA_ERROR_NOT_PERMITTED``

    * The caller must have access all memory from ``p_data`` to ``p_data + data_length``.

    * Even if all parameters are correct, the function can fail if there is insufficient storage space or in the case of a storage failure.


.. function:: psa_ps_get

    .. summary::
        Retrieve data associated with a provided ``uid``.

    .. param:: psa_storage_uid_t uid
        The ``uid`` value.
    .. param:: size_t data_offset
        The starting offset of the data requested. This must be less than or equal to ``uid.size``.
    .. param:: size_t data_size
        The amount of data requested.
    .. param:: void * p_data
        On success, the buffer where the data will be placed.
    .. param:: size_t * p_data_length
        On success, will contain size of the data placed in ``p_data``.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The caller cannot access some or all of the memory in the range [``p_data``, ``p_data + data_size - 1``].

        * ``data_offset`` is larger than the size of the data associated with ``uid``.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation failed because of an unspecified internal failure.
    .. retval:: PSA_ERROR_DATA_CORRUPT
        The operation failed because the data associated with the ``uid`` has been corrupted.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The operation failed because the data associated with the ``uid`` failed authentication.

    Retrieves data associated with ``uid``, starting at ``data_offset`` bytes from the beginning of the data. Fetches the smaller of   ``data_size`` or ``uid.size - data_offset`` bytes.  This can be zero.

    `psa_ps_get()` must not return bytes from beyond the end of ``uid``.

    Upon successful completion, the data will be placed in the ``p_data`` buffer, which must be at least ``data_size`` bytes in size. The length of the data returned will be in ``p_data_length``. Any bytes beyond ``p_data_length`` are left unmodified.

    If ``data_size`` is ``0`` or ``data_offset == uid.size``, the contents of ``p_data_length`` will be set to zero, but the contents of ``p_data`` are unchanged. The function returns ``PSA_SUCCESS``.

    * The ``uid`` value must not be zero.

    * The value of ``data_offset`` must be less than or equal to the length of ``uid``.

    * If ``data_offset`` is greater than ``uid.size`` the function retrieves no data and returns ``PSA_ERROR_INVALID_ARGUMENT``

    * If ``data_size`` is not zero, ``p_data`` must mot be ``NULL``.

    * The call must have access to the memory from ``p_data`` to ``p_data + data_size - 1``.

    * If the location ``uid`` exists the lesser of ``data_size`` and ``uid.size - data_offset`` bytes are written to the output buffer and ``p_data_length`` is set to the number of bytes written, which can be zero.

    * Any bytes in the buffer beyond ``p_data_length`` are left unmodified.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.


.. function:: psa_ps_get_info

    .. summary::
        Retrieve the metadata about the provided ``uid``.

    .. param:: psa_storage_uid_t uid
        The identifier for the data.
    .. param:: struct psa_storage_info_t * p_info
        A pointer to the `psa_storage_info_t` struct that will be populated with the metadata.

    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The caller cannot access some or all of the memory in the range [``p_info``, :code:`p_info + sizeof(psa_storage_info_t) - 1`]
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation failed because of an unspecified internal failure.
    .. retval:: PSA_ERROR_DATA_CORRUPT
        The operation failed because the data associated with the ``uid`` has been corrupted.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The operation failed because the data associated with the ``uid`` failed authentication.

    Retrieves the metadata stored for a given ``uid`` as a `psa_storage_info_t` structure.

    * The ``uid`` value must not be zero.

    * The call must have access to the memory from ``p_info`` to :code:`p_info + sizeof(psa_storage_info_t) - 1`.

    * If the location ``uid`` exists the metadata for the object is written to ``p_info``.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.


.. function:: psa_ps_remove

    .. summary::
        Remove the provided ``uid`` and its associated data from the storage.

    .. param:: psa_storage_uid_t uid
        The identifier for the data to be removed.


    .. return:: psa_status_t
        A status indicating the success or failure of the operation.

    .. retval:: PSA_SUCCESS
        The operation completed successfully.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The operation failed because the provided ``uid`` value was not found in the storage.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The ``uid`` is ``0``.
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The operation failed because the provided ``uid`` value was created with `PSA_STORAGE_FLAG_WRITE_ONCE`.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation failed because of an unspecified internal failure.

    Removes previously stored data and any associated metadata, including rollback protection data.

    * The ``uid`` value must not be zero.

    * If the location ``uid`` exists, it and any metadata are removed.

    * Even if all parameters are correct, the function can fail in the case of a storage failure.


.. function:: psa_ps_create

    .. summary::
        Reserves storage for the specified ``uid``.

    .. param:: psa_storage_uid_t uid
        A unique identifier for the asset.
    .. param:: size_t capacity
        The allocated capacity, in bytes, of the ``uid``.
    .. param:: psa_storage_create_flags_t create_flags
        Flags indicating properties of the storage.

    .. return:: psa_status_t

    .. retval:: PSA_SUCCESS
        The storage was successfully reserved.
    .. retval:: PSA_ERROR_INSUFFICIENT_STORAGE
        ``capacity`` is bigger than the current available space.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The function is not implemented or one or more ``create_flags`` are not supported.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because the ``uid`` is ``0``.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation has failed due to an unspecified error.
    .. retval:: PSA_ERROR_ALREADY_EXISTS
        Storage for the specified ``uid`` already exists.

    Reserves storage for the specified ``uid``. Upon success, the capacity of the storage is ``capacity``, and the size is ``0``.

    It is only necessary to call this function for assets that will be written with the `psa_ps_set_extended()` function. If only the `psa_ps_set()` function is needed, calls to this function are redundant.

    This function cannot be used to replace or resize an existing asset and attempting to do so will return ``PSA_ERROR_ALREADY_EXISTS``.

    If the `PSA_STORAGE_FLAG_WRITE_ONCE` flag is passed, `psa_ps_create()` will return ``PSA_ERROR_NOT_SUPPORTED``.

    This function is optional. Consult the platform documentation to determine if it is implemented or perform a call to `psa_ps_get_support()`. This function must be implemented if `psa_ps_get_support()` returns `PSA_STORAGE_SUPPORT_SET_EXTENDED`.

    * The ``uid`` value must not be zero.

    * If ``uid`` must not exist.

    * The flag `PSA_STORAGE_FLAG_WRITE_ONCE` must not be set.

    * Even if all parameters are correct, the function can fail if there is insufficient storage space or in the case of a storage failure.



.. function:: psa_ps_set_extended

    .. summary::
        Overwrite part of the data of the specified ``uid``.

    .. param:: psa_storage_uid_t uid
        The unique identifier for the asset.
    .. param:: size_t data_offset
        Offset within the asset to start the write.
    .. param:: size_t data_length
        The size in bytes of the data in ``p_data`` to write.
    .. param:: const void * p_data
        Pointer to a buffer which contains the data to write.

    .. return:: psa_status_t

    .. retval:: PSA_SUCCESS
        The asset exists, the input parameters are correct and the data is correctly written in the physical storage.
    .. retval:: PSA_ERROR_INVALID_ARGUMENT
        The operation failed because either:

        * The ``uid`` is ``0``.

        * The caller cannot access some or all of the memory in the range [``p_data``, ``p_data + data_size - 1``].

        * One or more of the preconditions regarding ``data_offset``, ``size``, or ``data_length`` was violated.
    .. retval:: PSA_ERROR_DOES_NOT_EXIST
        The specified ``uid`` was not found.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
        The implementation does not support this function.
    .. retval:: PSA_ERROR_STORAGE_FAILURE
        The operation failed because the physical storage has failed (Fatal error).
    .. retval:: PSA_ERROR_GENERIC_ERROR
        The operation failed due to an unspecified error.
    .. retval:: PSA_ERROR_DATA_CORRUPT
        The operation failed because the existing data has been corrupted.
    .. retval:: PSA_ERROR_INVALID_SIGNATURE
        The operation failed because the existing data failed authentication (MAC check failed).
    .. retval:: PSA_ERROR_NOT_PERMITTED
        The operation failed because it was attempted on an asset which was written with the flag `PSA_STORAGE_FLAG_WRITE_ONCE`.

    Sets partial data into an asset based on the given identifier, ``data_offset``, ``data length`` and ``p_data``.

    Before calling this function, the storage must have been reserved with a call to `psa_ps_create()`. It can also be used to overwrite data in an asset that was created with a call to `psa_ps_set()`.

    Calling this function with ``data_length == 0`` is permitted. This makes no change to the stored data.

    This function can overwrite existing data and/or extend it up to the capacity for the ``uid`` specified in `psa_ps_create()` but cannot create gaps.

    This function is optional. Consult the platform documentation to determine if it is implemented or perform a call to `psa_ps_get_support()`. This function must be implemented if `psa_ps_get_support()` returns `PSA_STORAGE_SUPPORT_SET_EXTENDED`.

    * The ``uid`` value must not be zero.

    * If ``uid`` exists it must not have been created as with `PSA_STORAGE_FLAG_WRITE_ONCE` - would result in ``PSA_ERROR_NOT_PERMITTED``

    * ``data_offset <= size``

    * ``data_offset + data_length <= capacity``

    * Even if all parameters are correct, the function can fail in the case of a storage failure.

    On Success:

    * ``size = max(size, data_offset + data_length)``

    * ``capacity`` unchanged.



.. function:: psa_ps_get_support

    .. return:: uint32_t

    .. summary::
        Returns a bitmask with flags set for the optional features supported by the implementation.

    Currently defined flags are limited to:

    * `PSA_STORAGE_SUPPORT_SET_EXTENDED`
