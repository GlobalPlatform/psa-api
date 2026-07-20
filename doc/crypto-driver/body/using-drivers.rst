..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
..  SPDX-License-Identifier: CC-BY-SA-4.0

.. _using-drivers:

Using drivers from an application
---------------------------------

Using transparent drivers
~~~~~~~~~~~~~~~~~~~~~~~~~

Transparent drivers linked into the library are automatically used for the mechanisms that they implement.

Using opaque drivers
~~~~~~~~~~~~~~~~~~~~

Each opaque driver is assigned a `location <lifetimes-and-locations>`.
The driver is invoked for all actions that use a key in that location.
A key's location is indicated by its lifetime.
The application chooses the key's lifetime when it creates the key.

For example, the following snippet creates an AES-GCM key which is only accessible inside the secure element designated by the location ``PSA_KEY_LOCATION_acme``.

.. code-block::

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
            PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_acme));
    psa_set_key_identifier(&attributes, 42);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_size(&attributes, 128);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_key_id_t key;
    psa_generate_key(&attributes, &key);

.. _lifetimes-and-locations:

Lifetimes and locations
^^^^^^^^^^^^^^^^^^^^^^^

The PSA Crypto API defines `lifetimes <https://arm-software.github.io/psa-api/crypto/1.3/api/keys/lifetimes.html#key-lifetimes>`__ as an attribute of a key that indicates where the key is stored and which application and system actions will create and destroy it.
The lifetime is expressed as a 32-bit value (``typedef uint32_t psa_key_lifetime_t``).
An upcoming version of the Crypto API defines more structure for lifetime values to separate these two aspects of the lifetime:

*   Bits 0-7 are a *persistence level*.
    This value indicates what device management actions can cause it to be destroyed.
    In particular, it indicates whether the key is volatile or persistent.
*   Bits 8-31 are a *location indicator*.
    This value indicates where the key material is stored and where operations on the key are performed.
    Location values can be stored in a variable of type ``psa_key_location_t``.

An opaque driver is attached to a specific location.
Keys in the default location (``PSA_KEY_LOCATION_LOCAL_STORAGE = 0``) are transparent: the core has direct access to the key material.
For keys in a location that is managed by an opaque driver, only the secure element has access to the key material and can perform operations on the key, while the core only manipulates a wrapped form of the key or an identifier of the key.

Creating a key in a secure element
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core defines a compile-time constant for each opaque driver indicating its location called ``PSA_KEY_LOCATION_``\ *prefix* where *prefix* is the value of the ``"prefix"`` property in the driver description.
For convenience, Mbed TLS also declares a compile-time constant for the corresponding lifetime with the default persistence called ``PSA_KEY_LIFETIME_``\ *prefix*.
Therefore, to declare an opaque key in the location with the prefix ``foo`` with the default persistence, call ``psa_set_key_lifetime`` during the key creation as follows:

.. code-block::

    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_foo);

To declare a volatile key:

.. code-block::

    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
            PSA_KEY_LOCATION_foo,
            PSA_KEY_PERSISTENCE_VOLATILE));

Generally speaking, to declare a key with a specified persistence:

.. code-block::

    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
            PSA_KEY_LOCATION_foo,
            persistence));

