.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 140

.. _key-lifetimes:

Key lifetimes
=============

The lifetime of a key indicates where it is stored and which application and system actions will create and destroy it.

Lifetime values are composed from:

*   A persistence level, which indicates what device management actions can cause it to be destroyed. In particular, it indicates whether the key is volatile or persistent. See `psa_key_persistence_t` for more information.

*   A location indicator, which indicates where the key is stored and where operations on the key are performed. See `psa_key_location_t` for more information.

There are two main types of lifetime, indicated by the persistence level: *volatile* and *persistent*.

Volatile keys
-------------

Volatile keys are automatically destroyed when the application instance terminates or on a power reset of the device. Volatile keys can be explicitly destroyed by the application.

Volatile keys have the persistence level `PSA_KEY_PERSISTENCE_VOLATILE` in the key lifetime value, see :secref:`key-lifetime-encoding`.
Unless the key lifetime is explicitly set in the key attributes before creating a key, a volatile key will be created with the default `PSA_KEY_LIFETIME_VOLATILE` lifetime value.

To create a volatile key:

1.  Populate a `psa_key_attributes_t` object with the required type, size, policy and other key attributes.
#.  If a non-default storage location is being used, set the key lifetime in the attributes object.
#.  Create the key with one of the key creation functions. If successful, these functions output a transient `key identifier <key-identifiers>`.

To destroy a volatile key: call `psa_destroy_key()` with the key identifier. There must be a matching call to `psa_destroy_key()` for each successful call to a create a volatile key.

Persistent keys
---------------

Persistent keys are preserved until the application explicitly destroys them or until an implementation-specific device management event occurs, for example, a factory reset.

Each persistent key has a permanent key identifier, which acts as a name for the key.
Within an application, the key identifier corresponds to a single key. The
application specifies the key identifier when the key is created and when
using the key.

The lifetime attribute of a persistent key indicates how and where it is stored. The default lifetime value for a persistent key is `PSA_KEY_LIFETIME_PERSISTENT`, which corresponds to a default storage area. This specification defines how implementations can provide other lifetime values corresponding to
different storage areas with different retention policies, or to secure elements
with different security characteristics.

To create a persistent key:

1.  Populate a `psa_key_attributes_t` object with the key’s type, size, policy and other attributes.
#.  In the attributes object, set the desired lifetime and persistent identifier for the key.
#.  Create the key with one of the key creation functions. If successful, these functions output the `key identifier <key-identifiers>` that was specified by the application in step 2.

To access an existing persistent key: use the key identifier in any API that requires a key.

To destroy a persistent key: call `psa_destroy_key()` with the key identifier. Destroying a persistent key permanently removes it from memory and storage.

By default, persistent key material is removed from volatile memory when not in use. Frequently used persistent keys can benefit from caching, depending on the implementation and the application. Caching can be enabled by creating the key with the `PSA_KEY_USAGE_CACHE` policy. Cached keys can be removed from volatile memory by calling `psa_purge_key()`. See also :secref:`memory-cleanup` and :secref:`key-material`.


.. _key-lifetime-encoding:

Key lifetime encoding
---------------------

.. typedef:: uint32_t psa_key_lifetime_t

    .. summary::
        Encoding of key lifetimes.

    The lifetime of a key indicates where it is stored and which application and system actions will create and destroy it.

    Lifetime values have the following structure:

    Bits[7:0]: Persistence level
        This value indicates what device management actions can cause it to be destroyed. In particular, it indicates whether the key is *volatile* or *persistent*. See `psa_key_persistence_t` for more information.

        :code:`PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime)` returns the persistence level for a key ``lifetime`` value.

    Bits[31:8]: Location indicator
        This value indicates where the key material is stored (or at least where it is accessible in cleartext) and where operations on the key are performed. See `psa_key_location_t` for more information.

        :code:`PSA_KEY_LIFETIME_GET_LOCATION(lifetime)` returns the location indicator for a key ``lifetime`` value.

    Volatile keys are automatically destroyed when the application instance terminates or on a power reset of the device. Persistent keys are preserved until the application explicitly destroys them or until an implementation-specific device management event occurs, for example, a factory reset.

    Persistent keys have a key identifier of type `psa_key_id_t`. This identifier remains valid throughout the lifetime of the key, even if the application instance that created the key terminates.

    This specification defines two basic lifetime values:

    *   Keys with the lifetime `PSA_KEY_LIFETIME_VOLATILE` are volatile. All implementations should support this lifetime.
    *   Keys with the lifetime `PSA_KEY_LIFETIME_PERSISTENT` are persistent. All implementations that have access to persistent storage with appropriate security guarantees should support this lifetime.


.. typedef:: uint8_t psa_key_persistence_t

    .. summary::
        Encoding of key persistence levels.

    What distinguishes different persistence levels is which device management events can cause keys to be destroyed. For example, power reset, transfer of device ownership, or a factory reset are device management events that can affect keys at different persistence levels. The specific management events which affect persistent keys at different levels is outside the scope of the |API|.

    Values for persistence levels defined by |API| are shown in :numref:`persistence-levels`.

    .. list-table:: Key persistence level values
        :name: persistence-levels
        :class: longtable
        :header-rows: 1
        :widths: 2,3
        :align: left

        *   -   Persistence level
            -   Definition

        *   -   :code:`0 = PSA_KEY_PERSISTENCE_VOLATILE`
            -   Volatile key.

                A volatile key is automatically destroyed by the implementation when the application instance terminates. In particular, a volatile key is automatically destroyed on a power reset of the device.

        *   -   :code:`1 = PSA_KEY_PERSISTENCE_DEFAULT`
            -   Persistent key with a default lifetime.

                Implementations should support this value if they support persistent keys at all. Applications should use this value if they have no specific needs that are only met by implementation-specific features.

        *   -   ``2 - 127``
            -   Persistent key with a PSA API-specified lifetime.

                The |API| does not define the meaning of these values, but another PSA API may do so.

        *   -   ``128 - 254``
            -   Persistent key with a vendor-specified lifetime.

                No PSA API will define the meaning of these values, so implementations may choose the meaning freely. As a guideline, higher persistence levels should cause a key to survive more management events than lower levels.

        *   -   :code:`255 = PSA_KEY_PERSISTENCE_READ_ONLY`
            -   Read-only or write-once key.

                A key with this persistence level cannot be destroyed. Implementations that support such keys may either allow their creation through the |API|, preferably only to applications with the appropriate privilege, or only expose keys created through implementation-specific means such as a factory ROM engraving process.

                Note that keys that are read-only due to policy restrictions rather than due to physical limitations should not have this persistence level.

    .. note::
        Key persistence levels are 8-bit values. Key management interfaces operate on lifetimes (type `psa_key_lifetime_t`), and encode the persistence value as the lower 8 bits of a 32-bit value.


.. typedef:: uint32_t psa_key_location_t

    .. summary::
        Encoding of key location indicators.

    If an implementation of the |API| can make calls to external cryptoprocessors such as secure elements, the location of a key indicates which secure element performs the operations on the key. If the key material is not stored persistently inside the secure element, it must be stored in a wrapped form such that only the secure element can access the key material in cleartext.

    Values for location indicators defined by this specification are shown in :numref:`location-indicators`.

    .. list-table:: Key location indicator values
        :name: location-indicators
        :class: longtable
        :header-rows: 1
        :widths: 1,3
        :align: left

        *   -   Location indicator
            -   Definition

        *   -   ``0``
            -   Primary local storage.

                All implementations should support this value. The primary local storage is typically the same storage area that contains the key metadata.

        *   -   ``1``
            -   Primary secure element.

                Implementations should support this value if there is a secure element attached to the operating environment. As a guideline, secure elements may provide higher resistance against side channel and physical attacks than the primary local storage, but may have restrictions on supported key types, sizes, policies and operations and may have different performance characteristics.

        *   -   ``2 - 0x7fffff``
            -   Other locations defined by a PSA specification.

                The |API| does not currently assign any meaning to these locations, but future versions of this specification or other PSA APIs may do so.

        *   -   ``0x800000 - 0xffffff``
            -   Vendor-defined locations.

                No PSA API will assign a meaning to locations in this range.

    .. note::

        Key location indicators are 24-bit values. Key management interfaces operate on lifetimes (type `psa_key_lifetime_t`), and encode the location as the upper 24 bits of a 32-bit value.


Lifetime values
---------------

.. macro:: PSA_KEY_LIFETIME_VOLATILE
    :definition: ((psa_key_lifetime_t) 0x00000000)

    .. summary::
        The default lifetime for volatile keys.

    A volatile key only exists as long as its identifier is not destroyed. The key material is guaranteed to be erased on a power reset.

    A key with this lifetime is typically stored in the RAM area of the Crypto API implementation. However this is an implementation choice. If an implementation stores data about the key in a non-volatile memory, it must release all the resources associated with the key and erase the key material if the calling application terminates.

.. macro:: PSA_KEY_LIFETIME_PERSISTENT
    :definition: ((psa_key_lifetime_t) 0x00000001)

    .. summary::
        The default lifetime for persistent keys.

    A persistent key remains in storage until it is explicitly destroyed or until the corresponding storage area is wiped. This specification does not define any mechanism to wipe a storage area. Implementations are permitted to provide their own mechanism, for example, to perform a factory reset, to prepare for device refurbishment, or to uninstall an application.

    This lifetime value is the default storage area for the calling application. Implementations can offer other storage areas designated by other lifetime values as implementation-specific extensions.

.. macro:: PSA_KEY_PERSISTENCE_VOLATILE
    :definition: ((psa_key_persistence_t) 0x00)

    .. summary::
        The persistence level of volatile keys.

    See `psa_key_persistence_t` for more information.

.. macro:: PSA_KEY_PERSISTENCE_DEFAULT
    :definition: ((psa_key_persistence_t) 0x01)

    .. summary::
        The default persistence level for persistent keys.

    See `psa_key_persistence_t` for more information.

.. macro:: PSA_KEY_PERSISTENCE_READ_ONLY
    :definition: ((psa_key_persistence_t) 0xff)

    .. summary::
        A persistence level indicating that a key is never destroyed.

    See `psa_key_persistence_t` for more information.

.. macro:: PSA_KEY_LOCATION_LOCAL_STORAGE
    :definition: ((psa_key_location_t) 0x000000)

    .. summary::
        The local storage area for persistent keys.

    This storage area is available on all systems that can store persistent keys without delegating the storage to a third-party cryptoprocessor.

    See `psa_key_location_t` for more information.

.. macro:: PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT
    :definition: ((psa_key_location_t) 0x000001)

    .. summary::
        The default secure element storage area for persistent keys.

    This storage location is available on systems that have one or more secure elements that are able to store keys.

    Vendor-defined locations must be provided by the system for storing keys in additional secure elements.

    See `psa_key_location_t` for more information.


Attribute accessors
-------------------

.. function:: psa_set_key_lifetime

    .. summary::
        Set the lifetime of a key, for a persistent key or a non-default location.

    .. param:: psa_key_attributes_t * attributes
        The attribute object to write to.
    .. param:: psa_key_lifetime_t lifetime
        The lifetime for the key.

        If this is a volatile lifetime (such that :code:`PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)` is true), the key identifier attribute is reset to `PSA_KEY_ID_NULL`.

    .. return:: void

    To make a key persistent, give it a persistent key identifier by using `psa_set_key_id()`. By default, a key that has a persistent identifier is stored in the default storage area identifier by `PSA_KEY_LIFETIME_PERSISTENT`. Call this function to choose a specific storage area, or to explicitly declare the key as volatile.

    This function does not access storage, it merely stores the given value in the attribute object. The persistent key will be written to storage when the attribute object is passed to a key creation function such as `psa_import_key()`, `psa_generate_key()`, `psa_generate_key_custom()`, `psa_key_derivation_output_key()`, `psa_key_derivation_output_key_custom()`, `psa_key_agreement()`, `psa_encapsulate()`, `psa_decapsulate()`, `psa_pake_get_shared_key()`, or `psa_copy_key()`.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.

.. function:: psa_get_key_lifetime

    .. summary::
        Retrieve the lifetime from key attributes.

    .. param:: const psa_key_attributes_t * attributes
        The key attribute object to query.

    .. return:: psa_key_lifetime_t
        The lifetime value stored in the attribute object.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.


Support macros
--------------

.. macro:: PSA_KEY_LIFETIME_GET_PERSISTENCE
    :definition: ((psa_key_persistence_t) ((lifetime) & 0x000000ff))

    .. summary::
        Extract the persistence level from a key lifetime.

    .. param:: lifetime
        The lifetime value to query: a value of type `psa_key_lifetime_t`.

.. macro:: PSA_KEY_LIFETIME_GET_LOCATION
    :definition: ((psa_key_location_t) ((lifetime) >> 8))

    .. summary::
        Extract the location indicator from a key lifetime.

    .. param:: lifetime
        The lifetime value to query: a value of type `psa_key_lifetime_t`.

.. macro:: PSA_KEY_LIFETIME_IS_VOLATILE
    :definition: (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)

    .. summary::
        Whether a key lifetime indicates that the key is volatile.

    .. param:: lifetime
        The lifetime value to query: a value of type `psa_key_lifetime_t`.

    .. return::
        ``1`` if the key is volatile, otherwise ``0``.

    A volatile key is automatically destroyed by the implementation when the application instance terminates. In particular, a volatile key is automatically destroyed on a power reset of the device.

    A key that is not volatile is persistent. Persistent keys are preserved until the application explicitly destroys them or until an implementation-specific device management event occurs, for example, a factory reset.

.. macro:: PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
    :definition: ((location) << 8 | (persistence))

    .. summary::
        Construct a lifetime from a persistence level and a location.

    .. param:: persistence
        The persistence level: a value of type `psa_key_persistence_t`.
    .. param:: location
        The location indicator: a value of type `psa_key_location_t`.

    .. return::
        The constructed lifetime value.
