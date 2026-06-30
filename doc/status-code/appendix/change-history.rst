.. SPDX-FileCopyrightText: Copyright 2022, 2024-2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _change-history:

==============
Change history
==============

Document and API Changes
~~~~~~~~~~~~~~~~~~~~~~~~

Changes in the draft GlobalPlatform publication revision
--------------------------------------------------------

*  Migrated the document to the 2026 PSA API template.

   This changes the document front matter structure and publication styling, without changing the API.

Changes between version 1.0.4 and version 1.0.5
-----------------------------------------------

*  Fixed the link in the :cite-title:`PSA FFM` document reference.

Changes between version 1.0.3 and version 1.0.4
-----------------------------------------------

*  Updated introduction to reflect GlobalPlatform assuming the governance of the PSA Certified evaluation scheme.

Changes between version 1.0.2 and version 1.0.3
-----------------------------------------------

*  Clarified the definition and scope of the :code:`PSA_ERROR_INVALID_HANDLE` status code.

Changes between version 1.0.1 and version 1.0.2
-----------------------------------------------

*  Removed the whitespace within the definition of some of the status codes. The whitespace was erroneously introduced during the separation from the :cite-title:`PSA FFM`. This change is necessary to ensure that multiple definitions of the same status code are identical, as required by the C language.


Changes between version 1.0.0 and version 1.0.1
-----------------------------------------------

*  Moved the specification of the common error codes into a separate specification.
*  Relicensed the document under Attribution-ShareAlike 4.0 International with a patent license derived from Apache License 2.0. See :secref:`license`.
*  Generalized the definitions of the error codes to better fit all PSA APIs.
*  Added definitions from other PSA APIs:

   -  `PSA_ERROR_CORRUPTION_DETECTED`
   -  `PSA_ERROR_DATA_CORRUPT`
   -  `PSA_ERROR_DATA_INVALID`

*  Added `PSA_OPERATION_INCOMPLETE` to indicate that the requested operation is unfinished. This can be used to break long-running operations into smaller pieces.


Changes prior to version 1.0.0
------------------------------

The definition of the common status codes was incorporated in the :cite-title:`PSA FFM` specification up until version 1.0.0.
