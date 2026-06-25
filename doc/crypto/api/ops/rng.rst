.. SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. header:: psa/crypto
    :seq: 310

Other cryptographic services
============================

.. _rng:

Random number generation
------------------------

.. function:: psa_generate_random

    .. summary::
        Generate random bytes.

    .. param:: uint8_t * output
        Output buffer for the generated data.
    .. param:: size_t output_size
        Number of bytes to generate and output.

    .. return:: psa_status_t
    .. retval:: PSA_SUCCESS
        Success.
        ``output`` contains ``output_size`` bytes of generated random data.
    .. retval:: PSA_ERROR_NOT_SUPPORTED
    .. retval:: PSA_ERROR_INSUFFICIENT_ENTROPY
    .. retval:: PSA_ERROR_INSUFFICIENT_MEMORY
    .. retval:: PSA_ERROR_COMMUNICATION_FAILURE
    .. retval:: PSA_ERROR_CORRUPTION_DETECTED
    .. retval:: PSA_ERROR_BAD_STATE
        The library requires initializing. See :secref:`library-init`.

    .. warning::
        This function **can** fail! Callers MUST check the return status and MUST NOT use the content of the output buffer if the return status is not :code:`PSA_SUCCESS`.

    .. note::
        To generate a random key, use `psa_generate_key()` or `psa_generate_key_custom()` instead.
