.. SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _status-codes:

Status codes
------------

The |API| uses the status code definitions that are shared with the other PSA APIs. The |API| also provides some |API|-specific status codes, see :secref:`specific-errors`.

The following elements are defined in :file:`psa/error.h` from :cite-title:`PSA STAT`:

.. code-block:: xref

    typedef int32_t psa_status_t;

    #define PSA_SUCCESS ((psa_status_t)0)

    #define PSA_ERROR_GENERIC_ERROR         ((psa_status_t)-132)
    #define PSA_ERROR_NOT_PERMITTED         ((psa_status_t)-133)
    #define PSA_ERROR_NOT_SUPPORTED         ((psa_status_t)-134)
    #define PSA_ERROR_INVALID_ARGUMENT      ((psa_status_t)-135)
    #define PSA_ERROR_INVALID_HANDLE        ((psa_status_t)-136)
    #define PSA_ERROR_BAD_STATE             ((psa_status_t)-137)
    #define PSA_ERROR_BUFFER_TOO_SMALL      ((psa_status_t)-138)
    #define PSA_ERROR_ALREADY_EXISTS        ((psa_status_t)-139)
    #define PSA_ERROR_DOES_NOT_EXIST        ((psa_status_t)-140)
    #define PSA_ERROR_INSUFFICIENT_MEMORY   ((psa_status_t)-141)
    #define PSA_ERROR_INSUFFICIENT_STORAGE  ((psa_status_t)-142)
    #define PSA_ERROR_INSUFFICIENT_DATA     ((psa_status_t)-143)
    #define PSA_ERROR_COMMUNICATION_FAILURE ((psa_status_t)-145)
    #define PSA_ERROR_STORAGE_FAILURE       ((psa_status_t)-146)
    #define PSA_ERROR_HARDWARE_FAILURE      ((psa_status_t)-147)
    #define PSA_ERROR_INVALID_SIGNATURE     ((psa_status_t)-149)
    #define PSA_ERROR_CORRUPTION_DETECTED   ((psa_status_t)-151)
    #define PSA_ERROR_DATA_CORRUPT          ((psa_status_t)-152)
    #define PSA_ERROR_DATA_INVALID          ((psa_status_t)-153)

These definitions must be available to an application that includes the :file:`psa/crypto.h` header file.

.. admonition:: Implementation note

   An implementation is permitted to define the status code interface elements within the :file:`psa/crypto.h` header file, or to define them via inclusion of a :file:`psa/error.h` header file that is shared with the implementation of other PSA APIs.

Common error codes
^^^^^^^^^^^^^^^^^^

Some of the common status codes have a more precise meaning when returned by a function in the |API|, compared to the definitions in `[PSA STAT]`. See also :secref:`error-handling`.

.. list-table::
    :class: longtable
    :header-rows: 1
    :widths: 1 2

    * - Error code
      - Meaning in the |API|

    * - :code:`PSA_ERROR_NOT_SUPPORTED`
      - `[PSA STAT]` recommends the use of :code:`PSA_ERROR_INVALID_ARGUMENT` for invalid parameter values.

        In the |API|, this is relaxed for algorithm identifier and key type parameters. It is recommended to return :code:`PSA_ERROR_INVALID_ARGUMENT` for invalid values, but :code:`PSA_ERROR_NOT_SUPPORTED` is also allowed, to permit implementations to avoid having to recognize all the cryptographic mechanisms that are defined in the PSA specification but not provided by that particular implementation.

    * - :code:`PSA_ERROR_INVALID_ARGUMENT`
      - `[PSA STAT]` recommends the use of :code:`PSA_ERROR_NOT_SUPPORTED` for unsupported parameter values.

        In the |API|, either :code:`PSA_ERROR_INVALID_ARGUMENT` or :code:`PSA_ERROR_NOT_SUPPORTED` can be returned when unsupported algorithm identifier or key type parameters are used. This allows implementations to avoid having to recognize all the cryptographic mechanisms that are defined in the PSA specification but not provided by that particular implementation.

    * - :code:`PSA_ERROR_INVALID_HANDLE`
      - A key identifier does not refer to an existing key. See also :secref:`key-ids`.

    * - :code:`PSA_ERROR_BAD_STATE`
      - Multi-part operations return this error when one of the functions is called out of sequence. Refer to the function descriptions for permitted sequencing of functions.

        Implementations can return this error if the caller has not initialized the library by a call to `psa_crypto_init()`.

    * - :code:`PSA_ERROR_BUFFER_TOO_SMALL`
      - Applications can call the ``PSA_xxx_SIZE`` macro listed in the function description to determine a sufficient buffer size.

    * - :code:`PSA_ERROR_STORAGE_FAILURE`
      - When a storage failure occurs, it is no longer possible to ensure the global integrity of the keystore. Depending on the global integrity guarantees offered by the implementation, access to other data might fail even if the data is still readable but its integrity cannot be guaranteed.

    * - :code:`PSA_ERROR_CORRUPTION_DETECTED`
      - This error code is intended as a last resort when a security breach is detected and it is unsure whether the keystore data is still protected. Implementations must only return this error code to report an alarm from a tampering detector, to indicate that the confidentiality of stored data can no longer be guaranteed, or to indicate that the integrity of previously returned data is now considered compromised.

    * - :code:`PSA_ERROR_DATA_CORRUPT`
      - When a storage failure occurs, it is no longer possible to ensure the global integrity of the keystore. Depending on the global integrity guarantees offered by the implementation, access to other data might fail even if the data is still readable but its integrity cannot be guaranteed.


.. _specific-errors:

Error codes specific to the |API|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. header:: psa/crypto
    :seq: 20

The following elements are defined in the :file:`psa/crypto.h` header file.

.. macro:: PSA_ERROR_INSUFFICIENT_ENTROPY
    :definition: ((psa_status_t)-148)

    .. summary::
        A status code that indicates that there is not enough entropy to generate random data needed for the requested action.

    This error indicates a failure of a hardware random generator. Application writers must note that this error can be returned not only by functions whose purpose is to generate random data, such as key, IV or nonce generation, but also by functions that execute an algorithm with a randomized result, as well as functions that use randomization of intermediate computations as a countermeasure to certain attacks.

    It is recommended that implementations do not return this error after `psa_crypto_init()` has succeeded. This can be achieved if the implementation generates sufficient entropy during initialization and subsequently a cryptographically secure pseudorandom generator (PRNG) is used. However, implementations might return this error at any time, for example, if a policy requires the PRNG to be reseeded during normal operation.

.. macro:: PSA_ERROR_INVALID_PADDING
    :definition: ((psa_status_t)-150)

    .. summary::
        A status code that indicates that the decrypted padding is incorrect.

    .. warning::
        In some protocols, when decrypting data, it is essential that the behavior of the application does not depend on whether the padding is correct, down to precise timing. Protocols that use authenticated encryption are recommended for use by applications, rather than plain encryption. If the application must perform a decryption of unauthenticated data, the application writer must take care not to reveal whether the padding is invalid.

    Implementations must handle padding carefully, aiming to make it impossible for an external observer to distinguish between valid and invalid padding. In particular, it is recommended that the timing of a decryption operation does not depend on the validity of the padding.
