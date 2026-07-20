.. SPDX-FileCopyrightText: Copyright 2018-2020, 2022-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _api:

API reference
=============

.. header:: psa/initial_attestation
    :copyright: Copyright 2018-2020, 2022-2026 Arm Limited and/or its affiliates
    :license: Apache-2.0
    :c++:
    :guard:
    :system-include: stddef.h stdint.h

    /* This file is a reference template for implementation of the
     * PSA Attestation API v2.0
     */

The |API| defines a header file that is provided by the implementation. The header is :file:`psa/initial_attestation.h`.

All the elements are defined in the C language. The |API| makes use of standard C data types, including the fixed-width integer types from the ISO C99 specification update :cite:`C99`.

API conventions
---------------

All functions return a status indication of type ``psa_status_t``, which is defined by :cite-title:`PSA STAT`. The value ``0`` (``PSA_SUCCESS``) indicates successful operation, and a negative value indicates an error. Each API documents the specific error codes that might be returned, and the meaning of each error.

All parameters of pointer type must be valid, non-null pointers unless the pointer is to a buffer of length 0 or the function's documentation explicitly describes the behavior when the pointer is null. For implementations where a null pointer dereference usually aborts the application, passing NULL as a function parameter where a null pointer is not allowed should abort the caller in the habitual manner.

Pointers to input parameters may be in read-only memory. Output parameters must be in writable memory. Output parameters that are not buffers must also be readable, and the implementation must be able to write to a non-buffer output parameter and read back the same value.


Status codes
------------

The |API| uses the status code definitions that are shared with the other PSA APIs.

The following elements are defined in :file:`psa/error.h` from :cite-title:`PSA STAT`:

.. code-block:: xref

   typedef int32_t psa_status_t;

   #define PSA_SUCCESS ((psa_status_t)0)

   #define PSA_ERROR_GENERIC_ERROR         ((psa_status_t)-132)
   #define PSA_ERROR_INVALID_ARGUMENT      ((psa_status_t)-135)
   #define PSA_ERROR_BUFFER_TOO_SMALL      ((psa_status_t)-138)
   #define PSA_ERROR_SERVICE_FAILURE       ((psa_status_t)-144)

These definitions must be available to an application that includes the :file:`psa/initial_attestation.h` header file.

.. admonition:: Implementation note

   An implementation is permitted to define the status code interface elements within :file:`psa/initial_attestation.h`, or to define them via inclusion of a :file:`psa/error.h` header file that is shared with the implementation of other PSA APIs.


General definitions
-------------------

.. macro:: PSA_INITIAL_ATTEST_API_VERSION_MAJOR
   :api-version: major

   .. summary:: The major version of this implementation of the Attestation API.

.. macro:: PSA_INITIAL_ATTEST_API_VERSION_MINOR
   :api-version: minor

   .. summary:: The minor version of this implementation of the Attestation API.

.. macro:: PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE
   :definition: /* implementation-specific value */

   .. summary:: The maximum possible size of a token.

   The value of this constant is |impdef|.

.. _challenge sizes:

Challenge sizes
---------------

The following constants define the valid challenge sizes that must be supported by the function
`psa_initial_attest_get_token()` and `psa_initial_attest_get_token_size()`.

An implementation must not support other challenge sizes.

.. macro:: PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32
   :definition: (32u)

   .. summary:: A challenge size of 32 bytes (256 bits).

.. macro:: PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48
   :definition: (48u)

   .. summary:: A challenge size of 48 bytes (384 bits).

.. macro:: PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64
   :definition: (64u)

   .. summary:: A challenge size of 64 bytes (512 bits).


Attestation
-----------

.. function:: psa_initial_attest_get_token

   .. summary::

      Retrieve the Initial Attestation Token.

   .. param:: const uint8_t *auth_challenge

      Buffer with a challenge object. The challenge object is data provided by the caller. For example, it may be a cryptographic nonce or a hash of data (such as an external object record).

      If a hash of data is provided then it is the caller's responsibility to ensure that the data is protected against replay attacks (for example, by including a cryptographic nonce within the data).

   .. param:: size_t challenge_size

      Size of the buffer ``auth_challenge`` in bytes. The size must always be a supported challenge size. Supported challenge sizes are defined in :secref:`challenge sizes`.

   .. param:: uint8_t *token_buf

      Output buffer where the attestation token is to be written.

   .. output:: *token_buf

      On success, the attestation token.

   .. param:: size_t token_buf_size

      Size of ``token_buf``. The expected size can be determined by using the `psa_initial_attest_get_token_size` function.

   .. param:: size_t *token_size

      Output variable for the actual token size.

   .. output:: *token_size

      On success, the number of bytes written into ``token_buf``.

   .. return:: psa_status_t

   .. retval:: PSA_SUCCESS

      Action was performed successfully.

   .. retval:: PSA_ERROR_SERVICE_FAILURE

      The implementation failed to fully initialize.

   .. retval:: PSA_ERROR_BUFFER_TOO_SMALL

      ``token_buf`` is too small for the attestation token.

   .. retval:: PSA_ERROR_INVALID_ARGUMENT

      The challenge size is not supported.

   .. retval:: PSA_ERROR_GENERIC_ERROR

      An unspecified internal error has occurred.

   Retrieves the Initial Attestation Token. A challenge can be passed as an input to mitigate replay attacks.


.. function:: psa_initial_attest_get_token_size

   .. summary::

      Calculate the size of an Initial Attestation Token.

   .. param:: size_t challenge_size

      Size of a challenge object in bytes. This must be a supported challenge size as specified in :secref:`challenge sizes`.

   .. param:: size_t *token_size

      Output variable for the token size.

   .. output:: *token_size

      On success, the size of an attestation token in bytes when using the specified ``challenge_size``

   .. return:: psa_status_t

   .. retval:: PSA_SUCCESS

      Action was performed successfully.

   .. retval:: PSA_ERROR_SERVICE_FAILURE

      The implementation failed to fully initialize.

   .. retval:: PSA_ERROR_INVALID_ARGUMENT

      The challenge size is not supported.

   .. retval:: PSA_ERROR_GENERIC_ERROR

      An unspecified internal error has occurred.

   Retrieve the exact size of the Initial Attestation Token in bytes, given a specific challenge size.
