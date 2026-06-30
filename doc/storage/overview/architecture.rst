.. SPDX-FileCopyrightText: Copyright 2018-2019, 2022 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

Architecture
============

Use Cases and Rationale
-----------------------

Two use cases are addressed by |API|:

* Secure storage for device intimate data (Internal Trusted Storage).
* Protection for data-at-rest (Protected Storage).

Internal Trusted Storage aims at providing a place for devices to store their most intimate secrets, either to ensure data privacy or data integrity. For example, a device identity key requires confidentiality, whereas an authority public key is public data but requires integrity. Other critical values that are part of a :term:`Root of Trust Service` --- for example, secure time values, monotonic counter values, or firmware image hashes --- will also need trusted storage.

Protected Storage is meant to protect larger data-sets against physical attacks. It aims to provide the ability for a firmware developer to store data onto external flash, with a promise of data-at-rest protection, including device-bound encryption, integrity, and replay protection. It should be possible to select the appropriate protection level --- for example, encryption only, or integrity only, or both --- depending on the threat model of the device and the nature of its deployment.

Technical Background
--------------------

Modern embedded platforms have multiple types of storage, each with different security properties.

Most embedded microprocessors (MCU) have on-chip flash storage that can be made inaccessible except to software running on the MCU. If the storage is made inaccessible to software other than that of the :term:`Platform Root of Trust` (PRoT), then it can be used to store key material, replay protection values, or other data critical to the secure operation of the device.

In addition, many platforms also have external storage that requires confidentiality, integrity, and replay protection from attackers with physical access to the device.

By providing consistent APIs for accessing storage, software in both the :term:`NSPE` and :term:`SPE` can be written in a platform-independent manner. This improves portability between platforms that implement the PSA APIs.

The Protected Storage API
-------------------------

The Protected Storage API is the general-purpose API that most developers should use. It is intended to be used to protect storage media that are external to the MCU package.

If the Protected Storage API is implemented using external storage without hardware protection, the data must be stored using authenticated encryption, as well as replay-protection values stored using the Internal Trusted Storage API. If the external storage has hardware protection --- for example, remote locations or tamper proof enclosures --- the need for cryptographic protection will be different.

|API| provides flags, `PSA_STORAGE_FLAG_NO_CONFIDENTIALITY` and `PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION`, enabling the caller to request a lower level of protection.

*  `PSA_STORAGE_FLAG_NO_CONFIDENTIALITY` requests integrity but not confidentiality. For example, this might be selected when storing other party's public keys. This flag does not affect replay protection.

*  `PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION` requests confidentiality and integrity protection of the data as controlled by `PSA_STORAGE_FLAG_NO_CONFIDENTIALITY`, but does not require the implementation to store data that would detect replacement with a previously valid value. For all other data objects, the implementation must ensure that the version returned is the most recently stored version.

.. admonition:: Implementation note

   This is usually achieved by creating a hash table or tree of all the file tags and storing the root in Internal Trusted Storage. Some implementations may only store the root and recreate the tree at boot --- in which case when it detects and error it cannot tell which file has been tampered with and must reject all attempts to read replay protected files.

The implementation is permitted to treat these flags as indicative, and to apply a higher level of protection if it does not implement every protection class. It must not apply a lower level of protection than that requested.

An implementation must treat the `PSA_STORAGE_FLAG_WRITE_ONCE` flag as definitive, if it is supported.

When reporting meta data, `psa_ps_get_info()` should report the actual protection level applied, and not the requested level.

The Internal Trusted Storage API
--------------------------------

The Internal Trusted Storage API is a more specialized API. Uses of the Internal Trusted Storage API will be less common. It is intended to be used for assets that must be placed inside internal flash. Some examples of assets that require this are replay protection values for external storage, and keys used by components of the :term:`PRoT`.

Storing assets that don't fit this requirement is permissible. In fact, it is expected that many platforms will have the Protected Storage API call directly into the Internal Trusted Storage API. For example, this can be done on platforms that do not have external flash.

While this document makes no requirements about the size of the storage available by the Internal Trusted Storage API, it is expected to be limited, and therefore should be used for small, security-critical values.

As the Internal Storage is implicitly confidential and protected from replay, the implementation can ignore the flags requesting lower levels of protection. However, it must honor the `PSA_STORAGE_FLAG_WRITE_ONCE` flag.

UIDs
----

``uids`` in the |API| are defined as ``uint64_t``. This is expected to be larger than would be used on any system. This large namespace is chosen to allow a :term:`Root of Trust Service` to easily manage assets on behalf of other services.

For example, consider a cryptography service running as a RoT Service. When a service running in a :term:`Secure Partition` requests key storage from the cryptography service, the cryptography service can concatenate a numerical identity of the requesting partition (for example, a ``int32_t`` in the :cite-title:`PSA FFM`) with the key identifier (for example, a ``uint32_t`` in the :cite-title:`PSA CRYPT`) to generate the ``uid`` of the Internal Trusted Storage entry for the key. This allows the cryptography service to easily manage isolation between the key namespaces of its various clients.

Requirements for ``uid``:

*  The value zero (``0``) is reserved, and will result in an error if passed to any of the |API| functions.

*  Each partition can use any of the non-zero ``uids`` in the full 64-bit range.

*  ``uid`` namespaces are independent. Using a ``uid`` in one partition has no impact on the ``uids`` or data assets in another partition.

*  Data assets are always private. There is no mechanism that enables one partition to access a data asset owned by another partition.

The implication is that the implementation cannot divide the ``uid`` range between partitions, but it must use a partition identify, in addition to the ``uid``, to identify a specific data asset.

Atomicity of Operations
-----------------------

In the event of power failure or other interruption of operations that modify storage, implementations of the |API| must maintain the properties shown in :numref:`tab-acid`.

.. list-table:: Properties of storage operations
   :name: tab-acid
   :class: longtable
   :stub-columns: 1
   :widths: 1 5

   *  -  Atomicity
      -  After the operation, the data assets of the storage service either contain the new data or are unchanged. Atomicity should be guaranteed in all situations --- for example, an invalid request, a software crash or a power cycle --- and must not result in corruption of the data assets. The only exceptions to this are situations involving storage failures or corruption.

   *  -  Consistency
      -  In the |API|, each operation is individually atomic. A multi-threaded application using |API| must not be able to observe any intermediate state in the data assets. If thread 'B' calls the |API| while thread 'A' is in the middle of an operation that modifies a data asset, thread 'B' must either see the state of the asset before, or the state of the asset after, the operation requested by thread 'A'.

   *  -  Isolation
      -  A partition using the storage service cannot cause a change in the data assets belonging to a different partition.

   *  -  Durability
      -  When an operation that modifies storage returns to the caller, the data is persisted. System reset or power fail at this point will not revert the data assets to the previous state.


Components
----------

:numref:`tab-components` lists the significant components in a system that implements |API|.

.. list-table:: Components in a system that implements the Trusted Storage API
   :name: tab-components
   :class: longtable
   :header-rows: 1
   :widths: 1 3

   *  -  Component
      -  Description

   *  -  Internal Trusted Storage API
      -  The storage API described in this document intended for access to internal flash memory.
   *  -  Internal Trusted Storage service
      -  A :term:`Platform Root of Trust` service that implements the Internal Trusted Storage API.
   *  -  Protected Storage API
      -  The general-purpose storage API described in this document.
   *  -  Protected Storage service
      -  A service, implemented either in the :term:`Application Root of Trust` or the :term:`NSPE`, that implements the Protected Storage API.
   *  -  :term:`Secure Partition Manager`
      -  The entity in the :term:`Secure Processing Environment` responsible for communicating requests between the various secure services.
