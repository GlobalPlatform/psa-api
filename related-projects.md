<!--
SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its affiliates
SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Known implementations and projects related to the PSA APIs

Updated Sep 2025.

## Software implementations

- The reference implementation for PSA Crypto API can be found inside Mbed TLS.
  [mbed-tls.readthedocs.io/en/latest](https://mbed-tls.readthedocs.io/en/latest/)

- Other PSA APIs are included in the TF-M project.
  [www.trustedfirmware.org/projects/tf-m](https://www.trustedfirmware.org/projects/tf-m/)

- Trusted Services implements those services in a Trusted Execution Environment on Cortex A processors.
  [www.trustedfirmware.org/projects/trusted-services](https://www.trustedfirmware.org/projects/trusted-services/)

- PARSEC is a Rust-based Linux microservice based on the Crypto API.
  [parsec.community](https://parsec.community)

- Oberon PSA Crypto offers a way to support both hardware and optimized software crypto routines under the PSA Crypto API.
  [www.oberon.ch/products/oberon-psa-crypto](https://www.oberon.ch/products/oberon-psa-crypto/)

- RIOT OS is integrating the Crypto API at a system-level for all crypto needs.
  [www.riot-os.org](https://www.riot-os.org/) and the associated paper at [arxiv.org/abs/2208.09281](https://arxiv.org/abs/2208.09281).

## Crypto driver implementations

The PSA Crypto Driver Interface complements the Crypto API.
Initially developed as part of the Mbed TLS project, development now continues as part of the PSA API project.

The aim of the Driver Interface is to simplify the integration of cryptographic peripherals, secure elements, and optimized software into implementations of the Crypto API. See [doc/crypto-driver/psa-driver-interface.rst](doc/crypto-driver/psa-driver-interface.rst).

- Mbed TLS uses the PSA Crypto Driver Interface to integrate hardware cryptographic accelerators and secure elements.
  [mbed-tls.readthedocs.io/en/latest](https://mbed-tls.readthedocs.io/en/latest/)

- The Silicon Labs SDK uses PSA Crypto Driver Interface conventions.
  [www.silabs.com/developers/gecko-software-development-kit](https://www.silabs.com/developers/gecko-software-development-kit)

- The Nordic SDK uses PSA Crypto Driver Interface conventions.
  [github.com/nrfconnect/sdk-nrf/tree/main/samples/crypto/psa_tls](https://github.com/nrfconnect/sdk-nrf/tree/main/samples/crypto/psa_tls)

- Oberon PSA Crypto contains software drivers (also used in the Nordic SDK) that implement the PSA Crypto Driver Interface.
  [www.oberon.ch/products/oberon-psa-crypto/qualities](https://www.oberon.ch/products/oberon-psa-crypto/qualities/)

- RIOT OS uses the PSA Crypto Driver Interface as the platform-integration interface for all cryptographic implementations.
  [www.riot-os.org](https://www.riot-os.org/) and the associated paper at [arxiv.org/abs/2208.09281](https://arxiv.org/abs/2208.09281).

## API usage

- Mbed TLS implements TLS stack for embedded usage, based on the Crypto API. Mbed TLS also uses the Secure Storage API for the Crypto implementation of key storage.
  [mbed-tls.readthedocs.io/en/latest](https://mbed-tls.readthedocs.io/en/latest/)

- WolfSSL can consume PSA-compliant crypto backends.
  [www.wolfssl.com/platform-security-architecture-psa-crypto-api-support-wolfssl](https://www.wolfssl.com/platform-security-architecture-psa-crypto-api-support-wolfssl/)

- t_cose uses PSA Crypto APIs for portability.
  [github.com/laurencelundblade/t_cose](https://github.com/laurencelundblade/t_cose)

- The EEMBC Securemark-TLS Benchmark uses PSA Crypto APIs.
  [github.com/eembc/securemark-tls](https://github.com/eembc/securemark-tls)

- The Matter project offers a PSA-compliant crypto adapter.
  [github.com/project-chip/connectedhomeip/tree/master/src/crypto](https://github.com/project-chip/connectedhomeip/tree/master/src/crypto)

- Zephyr OS integrates PSA Crypto APIs.
  [https://docs.zephyrproject.org/latest/services/crypto/psa_crypto.html](docs.zephyrproject.org/latest/services/crypto/psa_crypto.html)

----

*Copyright 2022, 2025 Arm Limited and/or its affiliates*\
*Copyright 2026 GlobalPlatform*
