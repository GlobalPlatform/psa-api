---
title: PSA Certified Crypto API
description: The Crypto API provides symmetric and asymmetric cryptography, hash, RNG, and key storage services with support for different key lifetime policies
---

<!--
SPDX-FileCopyrightText: Copyright 2022-2026 Arm Limited and/or its affiliates
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# PSA Certified<br />Crypto API

The Crypto API provides symmetric and asymmetric cryptography, hash, RNG, and key storage services with support for different key lifetime policies.

See the [PSA Certified APIs][psa-api] page for other PSA Certified APIs.

Specification source files, updates, and discussions, as well as reference headers and example code, can be found in the associated [PSA Certified APIs GitHub project][psa-api-gh].

Test suites to verify a correct implementation can be found in the [PSA Certified APIs Test suites][psa-api-ats].

[psa-api]:          ../
[psa-api-gh]:       https://github.com/arm-software/psa-api
[psa-api-ats]:      https://github.com/ARM-software/psa-arch-tests/tree/main/api-tests/dev_apis

## Specifications

Version | | | Publication
-|-|-|-:
1.5.0 | [HTML][1-5-html] | [&darr; PDF][1-5-0-pdf] | June 2026
1.4.1 | [HTML][1-4-html] | [&darr; PDF][1-4-1-pdf] | January 2026
1.4.0 | | [&darr; PDF][1-4-0-pdf] | November 2025
1.3.2 | [HTML][1-3-html] | [&darr; PDF][1-3-2-pdf] | September 2025
1.3.1 | | [&darr; PDF][1-3-1-pdf] | June 2025
1.3.0 | | [&darr; PDF][1-3-0-pdf] | March 2025
1.2.1 | [HTML][1-2-html] | [&darr; PDF][1-2-1-pdf] | March 2024
1.2.0 | | [&darr; PDF][1-2-0-pdf] | February 2024
1.1.2 | [HTML][1-1-html] | [&darr; PDF][1-1-2-pdf] | March 2023
1.1.1 | | [&darr; PDF][1-1-1-pdf] | October 2022
1.1.0 | | [&darr; PDF][1-1-0-pdf] | February 2022
1.0.1 | [HTML][1-0-html] | [&darr; PDF][1-0-1-pdf] | August 2020
1.0.0 | | [&darr; PDF][1-0-0-pdf] | Feburary 2020

## Extensions

Extension | Version | | | Publication
-|-|-|-|-:
PQC | 1.5.0 | *Integrated* | *n/a* | June 2026
PQC | 1.4 Final 1 | [HTML][pqc-rel-html] | [&darr; PDF][pqc-rel-1-pdf] | January 2026
PQC | 1.4 Final 0 | | [&darr; PDF][pqc-rel-0-pdf] | November 2025
PQC | 1.3 Beta 3 | [HTML][pqc-beta-html] | [&darr; PDF][pqc-beta-3-pdf] | September 2025
PQC | 1.3 Beta 2 | | [&darr; PDF][pqc-beta-2-pdf] | July 2025
PQC | 1.3 Beta 1 | | [&darr; PDF][pqc-beta-1-pdf] | June 2025
PQC | 1.3 Beta 0 | | [&darr; PDF][pqc-beta-0-pdf] | March 2025
PAKE | 1.3.0 | *Integrated* | *n/a* | March 2025
PAKE | 1.2 Final 1 | [HTML][pake-rel-html] | [&darr; PDF][pake-rel-1-pdf] | March 2024
PAKE | 1.2 Final | | [&darr; PDF][pake-rel-0-pdf] | February 2024
PAKE | 1.1 Beta 1 | [HTML][pake-beta-html] | [&darr; PDF][pake-beta-1-pdf] | October 2022
PAKE | 1.1 Beta 0 | | [&darr; PDF][pake-beta-0-pdf] | February 2022

[1-5-html]:             1.5/
[1-5-0-pdf]:            1.5/IHI0086-PSA_Certified_Crypto_API-1.5.0.pdf

[1-4-html]:             1.4/
[1-4-1-pdf]:            1.4/IHI0086-PSA_Certified_Crypto_API-1.4.1.pdf
[1-4-0-pdf]:            1.4/IHI0086-PSA_Certified_Crypto_API-1.4.0.pdf

[1-3-html]:             1.3/
[1-3-2-pdf]:            1.3/IHI0086-PSA_Certified_Crypto_API-1.3.2.pdf
[1-3-1-pdf]:            1.3/IHI0086-PSA_Certified_Crypto_API-1.3.1.pdf
[1-3-0-pdf]:            1.3/IHI0086-PSA_Certified_Crypto_API-1.3.0.pdf

[1-2-html]:             1.2/
[1-2-1-pdf]:            1.2/IHI0086-PSA_Certified_Crypto_API-1.2.1.pdf
[1-2-0-pdf]:            1.2/IHI0086-PSA_Certified_Crypto_API-1.2.0.pdf

[1-1-html]:             1.1/
[1-1-2-pdf]:            1.1/IHI0086-PSA_Certified_Crypto_API-1.1.2.pdf
[1-1-1-pdf]:            1.1/IHI0086-PSA_Certified_Crypto_API-1.1.1.pdf
[1-1-0-pdf]:            1.1/IHI0086-PSA_Cryptography_API-1.1.0.pdf

[1-0-html]:             1.0/
[1-0-1-pdf]:            1.0/IHI0086-PSA_Cryptography_API-1.0.1.pdf
[1-0-0-pdf]:            1.0/IHI0086-PSA_Cryptography_API-1.0.0.pdf

[pake-rel-html]:        1.2/ext-pake/
[pake-rel-1-pdf]:       1.2/ext-pake/AES0058-PSA_Certified_Crypto_API-1.2_PAKE_Extension.1.pdf
[pake-rel-0-pdf]:       1.2/ext-pake/AES0058-PSA_Certified_Crypto_API-1.2_PAKE_Extension.0.pdf

[pake-beta-html]:       1.1/ext-pake/
[pake-beta-1-pdf]:      1.1/ext-pake/AES0058-PSA_Certified_Crypto_API-1.1_PAKE_Extension-bet.1.pdf
[pake-beta-0-pdf]:      1.1/ext-pake/AES0058-PSA_Cryptography_API-1.1_PAKE_Extension-bet.0.pdf

[pqc-rel-html]:         1.4/ext-pqc/
[pqc-rel-1-pdf]:        1.4/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.4_PQC_Extension.1.pdf
[pqc-rel-0-pdf]:        1.4/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.4_PQC_Extension.0.pdf

[pqc-beta-html]:        1.3/ext-pqc/
[pqc-beta-3-pdf]:       1.3/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.3_PQC_Extension-bet.3.pdf
[pqc-beta-2-pdf]:       1.3/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.3_PQC_Extension-bet.2.pdf
[pqc-beta-1-pdf]:       1.3/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.3_PQC_Extension-bet.1.pdf
[pqc-beta-0-pdf]:       1.3/ext-pqc/AES0119-PSA_Certified_Crypto_API-1.3_PQC_Extension-bet.0.pdf

*Copyright 2022-2026, Arm Limited and/or its affiliates*
