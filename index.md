---
title: PSA Certified APIs
description: The official place for the latest published documents of the PSA Certified APIs
---

<!--
SPDX-FileCopyrightText: Copyright 2022-2026 Arm Limited and/or its affiliates
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# PSA Certified APIs

This is the official place for the latest published documents of the PSA Certified APIs.

Specification source files, updates, and discussions, as well as reference headers and example code, can be found in the associated [PSA Certified APIs GitHub project][psa-api-gh].

Test suites to verify a correct implementation can be found in the [PSA Certified APIs Test suites][psa-api-ats].

[psa-api-gh]:       https://github.com/arm-software/psa-api
[psa-api-ats]:      https://github.com/ARM-software/psa-arch-tests/tree/main/api-tests/dev_apis


## Specifications

The following specifications are part of the PSA Certified APIs:

Specification | | | | |
-|-|-|-|-
Crypto API | 1.5 | [HTML][crypto-html] | [&darr; PDF][crypto-pdf] | [All versions](crypto/)
Secure Storage API | 1.0 | [HTML][storage-html] | [&darr; PDF][storage-pdf] | [All versions](storage/)
Attestation API | 2.0 | [HTML][attestation-html] | [&darr; PDF][attestation-pdf] | [All versions](attestation/)
Firmware Update API | 1.0 | [HTML][fwu-html] | [&darr; PDF][fwu-pdf] | [All versions](fwu/)
Status code API | 1.0 | [HTML][status-code-html] | [&darr; PDF][status-code-pdf] | [All versions](status-code/)

### Extensions

Extension specifications introduce new functionality that is not yet stable enough for inclusion in the main specification.

Specification | Extension | | | | |
-|-|-|-|-|-
Crypto API | PAKE | 1.2 Final | | | *Superseded* |
Crypto API | PQC | 1.4 Final | | | *Superseded* |

Since Crypto API 1.3, the PAKE Extension is integrated into the Crypto API specification.
Since Crypto API 1.5, the PQC Extension is integrated into the Crypto API specification.
Older versions of these extensions can be found on the [Crypto API](crypto/) page.

### In development

These specifications are being developed towards an initial 1.0 version:

Specification | | | | |
-|-|-|-|-|-
Crypto Driver Interface | 1.0 Alpha 1 | [HTML][driver-html] | [&darr; PDF][driver-pdf] | [All versions](crypto-driver/)

[status-code-html]:  status-code/1.0/
[status-code-pdf]:   status-code/1.0/IHI0097-PSA_Certified_Status_code_API-1.0.5.pdf
[crypto-html]:       crypto/1.5/
[crypto-pdf]:        crypto/1.5/IHI0086-PSA_Certified_Crypto_API-1.5.0.pdf
[storage-html]:      storage/1.0/
[storage-pdf]:       storage/1.0/IHI0087-PSA_Certified_Secure_Storage_API-1.0.4.pdf
[attestation-html]:  attestation/2.0/
[attestation-pdf]:   attestation/2.0/IHI0085-PSA_Certified_Attestation_API-2.0.0.pdf
[fwu-html]:          fwu/1.0/
[fwu-pdf]:           fwu/1.0/IHI0093-PSA_Certified_Firmware_Update_API-1.0.1.pdf
[driver-html]:       crypto-driver/1.0/
[driver-pdf]:        crypto-driver/1.0/111106-PSA_Certified_Crypto_Driver_Interface-1.0-alp.1.pdf

## Feedback

If you have questions or comments on any of the specifications, or suggestions for enhancements, please [raise a new issue][psa-api-issue] in the PSA Certified APIs GitHub project.

Please indicate which specification the issue applies to. This can be done by:

* Providing a link to the section of the specification on this website.
* Providing the document name, full version, and section or page number in the PDF.

[psa-api-issue]:    https://github.com/arm-software/psa-api/issues/new

## License

The latest versions of the PSA Certified APIs that are hosted on this website are licensed under the Creative Commons [Attribution–Share Alike 4.0 International license][CC-BY-SA-4.0] and [Apache License, Version 2.0][APACHE-2.0]. Some earlier versions of the specifications are licensed under a non-confidential license from Arm.

Refer to individual documents for license details.

[CC-BY-SA-4.0]:     https://creativecommons.org/licenses/by-sa/4.0
[APACHE-2.0]:       https://www.apache.org/licenses/LICENSE-2.0

----

*Copyright 2022-2026, Arm Limited and/or its affiliates*
