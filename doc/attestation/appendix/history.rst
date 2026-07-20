.. SPDX-FileCopyrightText: Copyright 2018-2020, 2022-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _document-history:

Document history
================

..  list-table::
    :class: longtable
    :header-rows: 1
    :widths: 4 16

    * - Date
      - Changes

    * - TBD
      - *Draft GlobalPlatform publication revision*

        * Migrated the document to the 2026 PSA API template.
        * Changed the document front matter structure and publication styling, without changing the API.

    * - June 2019
      - *1.0.0*

        * First stable release

    * - August 2019
      - *1.0.1*

        * Fixed typos and descriptions based on feedback.
        * Recommend type byte 0x01 for arm_psa_UEID.
        * Remove erroneous guidance regarding EAT's origination claim - it should not be used to find a verification service.

    * - February 2020
      - *1.0.2*

        * Clarify the claim number of Instance ID
        * Permit COSE-Mac0 for signing tokens (with appropriate warning)
        * Update URLs

    * - October 2022
      - *1.0.3*

        * Relicensed the document under Attribution-ShareAlike 4.0 International with a patent license derived from Apache License 2.0. See :secref:`license`.
        * Fix CBOR type of ``arm_psa_origination`` to text string. Spec and example were in conflict, and the example was correct.
        * Added CDDL definition to the appendices, which can be helpful to developers.
        * Instance ID definition for symmetric keys has been improved. The specific constructions are now recommended rather than normative.
        * Clarified the optionality of map entries in the Software Components claim.

    * - September 2025
      - *1.0.4*

        * Updated introduction to reflect GlobalPlatform assuming the governance of the PSA Certified evaluation scheme.

    * - May 2026
      - *2.0.0*

        * Update the API to use the PSA attestation token, defined in :rfc-title:`9783`. The token and report format, CDDL definition, and example token are no longer required in this specification.
