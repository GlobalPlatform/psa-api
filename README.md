<!--
SPDX-FileCopyrightText: Copyright 2022-2026 Arm Limited and/or its affiliates
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# PSA Certified API Specifications

This is the official place for the latest documents of the PSA Certified API.

This GitHub repository contains:
*  Specification source files
*  Reference copies of the PSA Certified API header files
*  Examples of usage and implementation of the PSA Certified APIs
*  Discussions of updates to the specifications
*  Proposed changes to the specifications

Officially released specification documents can be found in the associated [PSA Certified API website](https://arm-software.github.io/psa-api/).

## Specifications

The following specifications are part of the PSA Certified API.

Specification | Published | Document source | Reference headers | Dashboard
-|-|-|-|-
Crypto API | [1.5.0][crypto-specs] | [doc/crypto/] | [headers/crypto/1.5/] | [Project board][crypto-dash]
Secure Storage API | [1.0.4][storage-specs] | [doc/storage/] |  [headers/storage/1.0/] | [Project board][storage-dash] |
Attestation API | [2.0.0][attestation-specs] | [doc/attestation/] |  [headers/attestation/2.0/] | [Project board][attestation-dash] |
Firmware Update API | [1.0.1][fwu-specs] | [doc/fwu/] |  [headers/fwu/1.0/] | [Project board][fwu-dash]
Status code API | [1.0.5][status-specs] | [doc/status-code/] |  [headers/status-code/1.0/] | [Project board][status-code-dash] |

[crypto-specs]:         https://arm-software.github.io/psa-api/crypto/
[storage-specs]:        https://arm-software.github.io/psa-api/storage/
[attestation-specs]:    https://arm-software.github.io/psa-api/attestation/
[fwu-specs]:            https://arm-software.github.io/psa-api/fwu/
[status-specs]:         https://arm-software.github.io/psa-api/status-code/

[crypto-dash]:          https://github.com/orgs/ARM-software/projects/5/views/3
[storage-dash]:         https://github.com/orgs/ARM-software/projects/5/views/4
[attestation-dash]:     https://github.com/orgs/ARM-software/projects/5/views/5
[fwu-dash]:             https://github.com/orgs/ARM-software/projects/5/views/6
[status-code-dash]:     https://github.com/orgs/ARM-software/projects/5/views/7

[doc/crypto/]:          doc/crypto
[doc/storage/]:         doc/storage
[doc/attestation/]:     doc/attestation
[doc/fwu/]:             doc/fwu
[doc/status-code/]:     doc/status-code

[headers/crypto/1.5/]:      headers/crypto/1.5
[headers/storage/1.0/]:     headers/storage/1.0
[headers/attestation/2.0/]: headers/attestation/2.0
[headers/fwu/1.0/]:         headers/fwu/1.0
[headers/status-code/1.0/]: headers/status-code/1.0

### Extensions

Extension specifications introduce new functionality that is not yet stable enough for inclusion in the main specification.

API | Extension | Published | Document source | Reference headers | Dashboard
-|-|-|-|-|-
Crypto API | PAKE | [*Integrated in 1.3.0*][crypto-specs] | *n/a* | *n/a* | *n/a*
Crypto API | PQC | [*Integrated in 1.5*][crypto-specs] | *n/a* | *n/a* | *n/a*

### In development

The following specifications are being developed towards an initial 1.0 version:

Specification | Published | Document source | Reference headers | Dashboard
-|-|-|-|-
Crypto Driver Interface | [1.0 Alpha-1][crypto-driver-specs] | [doc/crypto-driver/] | *n/a* | [Project board][crypto-driver-dash]

[crypto-driver-specs]:  https://arm-software.github.io/psa-api/crypto-driver/
[crypto-driver-dash]:   https://github.com/orgs/ARM-software/projects/5/views/8
[doc/crypto-driver/]:   doc/crypto-driver

## Reference header files

Reference header files for each minor version of each API are provided in the [headers/](headers) folder.

## Test Suite

Test suites are available to validate compliance of API implementations against the specifications for Crypto, Attestation, and Secure Storage APIs, from:
[github.com/ARM-software/psa-arch-tests](https://github.com/ARM-software/psa-arch-tests)

Compliance badges can be obtained from [PSA Certified](https://www.psacertified.org/getting-certified/functional-api-certification/) to showcase compatible products.


## Example source code

Source code examples of both usage, and implementation, of the PSA Certified APIs are provided in the [examples/](/examples) folder.

## Related Projects

Known projects that implement or use the PSA Certified APIs are listed in [related-projects](/related-projects.md).


## License

### Text and illustrations

Text and illustrations in this project are licensed under Creative Commons [Attribution–Share Alike 4.0 International license][CC-BY-SA-4.0] (CC BY-SA 4.0).

**Grant of patent license**. Subject to the terms and conditions of this license (both the CC BY-SA 4.0 Public License and this Patent License), each Licensor hereby grants to You a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable (except as stated in this section) patent license to make, have made, use, offer to sell, sell, import, and otherwise transfer the Licensed Material, where such license applies only to those patent claims licensable by such Licensor that are necessarily infringed by their contribution(s) alone or by combination of their contribution(s) with the Licensed Material to which such contribution(s) was submitted. If You institute patent litigation against any entity (including a cross-claim or counterclaim in a lawsuit) alleging that the Licensed Material or a contribution incorporated within the Licensed Material constitutes direct or contributory patent infringement, then any licenses granted to You under this license for that Licensed Material shall terminate as of the date such litigation is filed.

The Arm trademarks featured here are registered trademarks or trademarks of Arm Limited (or its subsidiaries) in the US and/or elsewhere. All rights reserved. Please visit [arm.com/company/policies/trademarks][trademarks] for more information about Arm's trademarks.

### About the license

The language in the additional patent license is largely identical to that in section 3 of [Apache License, Version 2.0][APACHE-2.0] (Apache 2.0) with two exceptions:

1. Changes are made related to the defined terms, to align those defined terms with the terminology in CC BY-SA 4.0 rather than Apache 2.0 (for example, changing "Work" to "Licensed Material").

2. The scope of the defensive termination clause is changed from "any patent licenses granted to You" to "any licenses granted to You". This change is intended to help maintain a healthy ecosystem by providing additional protection to the community against patent litigation claims.

[CC-BY-SA-4.0]:     https://creativecommons.org/licenses/by-sa/4.0
[APACHE-2.0]:       https://www.apache.org/licenses/LICENSE-2.0
[trademarks]:       https://www.arm.com/company/policies/trademarks

### Source code

Source code samples in this project are licensed under the [Apache License, Version 2.0][APACHE-2.0] (the "License"); you may not use such samples except in compliance with the License. You may obtain a copy of the License at [apache.org/licenses/LICENSE-2.0][APACHE-2.0].

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.

## Feedback

If you have questions or comments on any of the PSA Certified API specifications, or suggestions for enhancements, please [raise a new issue][psa-api-issue].

Please indicate which specification the issue applies to. This can be done by:

* Providing a link to the section of the specification on this website.
* Providing the document name, full version, and section or page number in the PDF.

[psa-api-issue]:    https://github.com/arm-software/psa-api/issues/new

## Contributing

Anyone may contribute to the PSA Certified API. Discussion of changes and enhancement happens in this repository's [Issues][issues] and [Pull requests][pulls]. See [CONTRIBUTING](CONTRIBUTING.md) for details.

[issues]:           https://github.com/arm-software/psa-api/issues
[pulls]:            https://github.com/arm-software/psa-api/pulls

----

*Copyright 2022-2026 Arm Limited and/or its affiliates*
