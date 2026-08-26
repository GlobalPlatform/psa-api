.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 120

.. _key-attributes:

Key attributes
==============

Key attributes are managed in a `psa_key_attributes_t` object. These are used when a key is created, after which the key attributes are fixed. Attributes of an existing key can be queried using `psa_get_key_attributes()`.

Description of the individual attributes is found in the following sections:

*   :secref:`key-types`
*   :secref:`key-identifiers`
*   :secref:`key-lifetimes`
*   :secref:`key-policy`


Managing key attributes
-----------------------

.. typedef:: /* implementation-defined type */ psa_key_attributes_t

    .. summary::
        The type of an object containing key attributes.

    This is the object that represents the metadata of a key object. Metadata that can be stored in attributes includes:

    *   The location of the key in storage, indicated by its key identifier and its lifetime.
    *   The key's policy, comprising usage flags and a specification of the permitted algorithm(s).
    *   Information about the key itself: the key type and its size.
    *   Implementations can define additional attributes.

    The actual key material is not considered an attribute of a key. Key attributes do not contain information that is generally considered highly confidential.

    .. note::
        Implementations are recommended to define the attribute object as a simple data structure, with fields corresponding to the individual key attributes. In such an implementation, each function ``psa_set_key_xxx()`` sets a field and the corresponding function ``psa_get_key_xxx()`` retrieves the value of the field.

        An implementations can report attribute values that are equivalent to the original one, but have a different encoding. For example, an implementation can use a more compact representation for types where many bit-patterns are invalid or not supported, and store all values that it does not support as a special marker value. In such an implementation, after setting an invalid value, the corresponding get function returns an invalid value which might not be the one that was originally stored.

    This is an implementation-defined type. Applications that make assumptions about the content of this object will result in implementation-specific behavior, and are non-portable.

    An attribute object can contain references to auxiliary resources, for example pointers to allocated memory or indirect references to pre-calculated values. In order to free such resources, the application must call `psa_reset_key_attributes()`. As an exception, calling `psa_reset_key_attributes()` on an attribute object is optional if the object has only been modified by the following functions since it was initialized or last reset with `psa_reset_key_attributes()`:

    *   `psa_set_key_id()`
    *   `psa_set_key_lifetime()`
    *   `psa_set_key_type()`
    *   `psa_set_key_bits()`
    *   `psa_set_key_usage_flags()`
    *   `psa_set_key_algorithm()`

    Before calling any function on a key attribute object, the application must initialize it by any of the following means:

    *   Set the object to all-bits-zero, for example:

        .. code-block:: xref

            psa_key_attributes_t attributes;
            memset(&attributes, 0, sizeof(attributes));

    *   Initialize the object to logical zero values by declaring the object as static or global without an explicit initializer, for example:

        .. code-block:: xref

            static psa_key_attributes_t attributes;

    *   Initialize the object to the initializer `PSA_KEY_ATTRIBUTES_INIT`, for example:

        .. code-block:: xref

            psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    *   Assign the result of the function `psa_key_attributes_init()` to the object, for example:

        .. code-block:: xref

            psa_key_attributes_t attributes;
            attributes = psa_key_attributes_init();

    A freshly initialized attribute object contains the following values:

    .. list-table::
        :header-rows: 1
        :widths: auto
        :align: left

        *   -   Attribute
            -   Value

        *   -   lifetime
            -   `PSA_KEY_LIFETIME_VOLATILE`.
        *   -   key identifier
            -   `PSA_KEY_ID_NULL` --- which is not a valid key identifier.
        *   -   type
            -   `PSA_KEY_TYPE_NONE` --- meaning that the type is unspecified.
        *   -   key size
            -   ``0`` --- meaning that the size is unspecified.
        *   -   usage flags
            -   ``0`` --- which permits no usage except exporting a public key.
        *   -   algorithm
            -   `PSA_ALG_NONE` --- which does not permit cryptographic usage, but permits exporting.

    .. rubric:: Usage

    A typical sequence to create a key is as follows:

    1.  Create and initialize an attribute object.
    #.  If the key is persistent, call `psa_set_key_id()`. Also call `psa_set_key_lifetime()` to place the key in a non-default location.
    #.  If the key is volatile in a non-default location, call `psa_set_key_lifetime()` to specify the location.
    #.  Set the key policy with `psa_set_key_usage_flags()` and `psa_set_key_algorithm()`.
    #.  Set the key type with `psa_set_key_type()`. Skip this step if copying an existing key with `psa_copy_key()`.
    #.  When generating a random key with `psa_generate_key()` or `psa_generate_key_custom()`, or deriving a key with `psa_key_derivation_output_key()` or `psa_key_derivation_output_key_custom()`, set the desired key size with `psa_set_key_bits()`.
    #.  Call a key creation function: `psa_import_key()`, `psa_generate_key()`, `psa_generate_key_custom()`, `psa_key_derivation_output_key()`, `psa_key_derivation_output_key_custom()`, `psa_key_agreement()`, `psa_encapsulate()`, `psa_decapsulate()`, `psa_pake_get_shared_key()`, `psa_copy_key()`, or `psa_attach_key()`. This function reads the attribute object, creates a key with these attributes, and outputs an identifier for the newly created key.
    #.  Optionally call `psa_reset_key_attributes()`, now that the attribute object is no longer needed. Currently this call is not required as the attributes defined in this specification do not require additional resources beyond the object itself.

    A typical sequence to query a key's attributes is as follows:

    1.  Call `psa_get_key_attributes()`.
    #.  Call ``psa_get_key_xxx()`` functions to retrieve the required attribute(s).
    #.  Call `psa_reset_key_attributes()` to free any resources that can be used by the attribute object.

    Once a key has been created, it is impossible to change its attributes.

.. macro:: PSA_KEY_ATTRIBUTES_INIT
    :definition: /* implementation-defined value */

    .. summary::
        This macro returns a suitable initializer for a key attribute object of type `psa_key_attributes_t`.

.. function:: psa_key_attributes_init

    .. summary::
        Return an initial value for a key attribute object.

    .. return:: psa_key_attributes_t

.. function:: psa_get_key_attributes

    .. summary::
        Retrieve the attributes of a key.

    .. param:: psa_key_id_t key
        Identifier of the key to query.
    .. param:: psa_key_attributes_t * attributes
        On entry, ``*attributes`` must be in a valid state. On successful return, it contains the attributes of the key. On failure, it is equivalent to a freshly-initialized attribute object.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        ``attributes`` contains the attributes of the key.
    .. retval:: PSA_ERROR_INVALID_HANDLE
        ``key`` is not a valid key identifier.
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_STORAGE_FAILURE
    .. retval:: PSA_ERROR_DATA_CORRUPT
    .. retval:: PSA_ERROR_DATA_INVALID
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    This function first resets the attribute object as with `psa_reset_key_attributes()`. It then copies the attributes of the given key into the given attribute object.

    .. note::
        This function clears any previous content from the attribute object and therefore expects it to be in a valid state. In particular, if this function is called on a newly allocated attribute object, the attribute object  must be initialized before calling this function.

    .. note::
        This function might allocate memory or other resources. Once this function has been called on an attribute object, `psa_reset_key_attributes()` must be called to free these resources.

.. function:: psa_reset_key_attributes

    .. summary::
        Reset a key attribute object to a freshly initialized state.

    .. param:: psa_key_attributes_t * attributes
        The attribute object to reset.

    .. return:: void

    The attribute object must be initialized as described in the documentation of the type `psa_key_attributes_t` before calling this function. Once the object has been initialized, this function can be called at any time.

    This function frees any auxiliary resources that the object might contain.
