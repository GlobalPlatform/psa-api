.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _key-identifiers:

Key identifiers
===============

Key identifiers are integral values that act as permanent names for persistent keys, or as transient references to volatile keys. Key identifiers use the `psa_key_id_t` type, and the range of identifier values is divided as follows:

:code:`PSA_KEY_ID_NULL = 0`
    Reserved as an invalid key identifier.
:code:`PSA_KEY_ID_USER_MIN - PSA_KEY_ID_USER_MAX`
    Applications can freely choose persistent key identifiers in this range.
:code:`PSA_KEY_ID_VENDOR_MIN - PSA_KEY_ID_VENDOR_MAX`
    Implementations can define additional persistent key identifiers in this range for :term:`built-in key`\ s, and must allocate any volatile key identifiers from this range.

Key identifiers outside these ranges are reserved for future use.

Key identifiers are output from a successful call to one of the key creation functions. For persistent keys, this is the same identifier as the one specified in the key attributes used to create the key. The key identifier remains valid until it is invalidated by passing it to `psa_destroy_key()`. A volatile key identifier must not be used after it has been invalidated.

If an invalid key identifier is provided as a parameter in any function, the function will return :code:`PSA_ERROR_INVALID_HANDLE`; except for the special case of calling :code:`psa_destroy_key(PSA_KEY_ID_NULL)`, which has no effect and always returns :code:`PSA_SUCCESS`.

Valid key identifiers must have distinct values within the same application. If the implementation provides :term:`caller isolation`, then key identifiers are local to each application. That is, keys that are created by two applications with the same key identifier are two separate keys.


Key identifier type
-------------------

.. header:: psa/crypto
    :seq: 110

.. typedef:: uint32_t psa_key_id_t

    .. summary::
        Key identifier.

    A key identifier can be a permanent name for a persistent key, or a transient reference to volatile key. See :secref:`key-identifiers`.

.. header:: psa/crypto
    :seq: 150

.. macro:: PSA_KEY_ID_NULL
    :definition: ((psa_key_id_t)0)

    .. summary::
        The null key identifier.

    The null key identifier is always invalid, except when used without in a call to `psa_destroy_key()` which will return :code:`PSA_SUCCESS`.

.. macro:: PSA_KEY_ID_USER_MIN
    :definition: ((psa_key_id_t)0x00000001)

    .. summary::
        The minimum value for a key identifier chosen by the application.

.. macro:: PSA_KEY_ID_USER_MAX
    :definition: ((psa_key_id_t)0x3fffffff)

    .. summary::
        The maximum value for a key identifier chosen by the application.

.. macro:: PSA_KEY_ID_VENDOR_MIN
    :definition: ((psa_key_id_t)0x40000000)

    .. summary::
        The minimum value for a key identifier chosen by the implementation.

.. macro:: PSA_KEY_ID_VENDOR_MAX
    :definition: ((psa_key_id_t)0x7fffffff)

    .. summary::
        The maximum value for a key identifier chosen by the implementation.


Attribute accessors
-------------------

.. function:: psa_set_key_id

    .. summary::
        Declare a key as persistent and set its key identifier.

    .. param:: psa_key_attributes_t * attributes
        The attribute object to write to.
    .. param:: psa_key_id_t id
        The persistent identifier for the key.

    .. return:: void

    The application must choose a value for ``id`` between `PSA_KEY_ID_USER_MIN` and `PSA_KEY_ID_USER_MAX`.

    If the attribute object currently declares the key as volatile, this function sets the persistence level in the lifetime attribute to `PSA_KEY_PERSISTENCE_DEFAULT` without changing the key location. See :secref:`key-lifetimes`.

    This function does not access storage, it merely stores the given value in the attribute object. The persistent key will be written to storage when the attribute object is passed to a key creation function such as `psa_import_key()`, `psa_generate_key()`, `psa_generate_key_custom()`, `psa_key_derivation_output_key()`, `psa_key_derivation_output_key_custom()`, `psa_key_agreement()`, `psa_encapsulate()`, `psa_decapsulate()`, `psa_pake_get_shared_key()`, or `psa_copy_key()`.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.

.. function:: psa_get_key_id

    .. summary::
        Retrieve the key identifier from key attributes.

    .. param:: const psa_key_attributes_t * attributes
        The key attribute object to query.

    .. return:: psa_key_id_t
        The persistent identifier stored in the attribute object. This value is unspecified if the attribute object declares the key as volatile.

    .. admonition:: Implementation note

        This is a simple accessor function that is not required to validate its inputs. It can be efficiently implemented as a ``static inline`` function or a function-like-macro.
