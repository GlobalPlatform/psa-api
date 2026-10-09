..  SPDX-FileCopyrightText: Copyright 2020-2026 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _crypto-driver-purpose:

Crypto Driver Interface
=======================

Purpose
-------

The :cite-title:`PSA CRYPT` allows applications and system software to perform cryptographic operations without depending on how those operations are implemented. An implementation can use general-purpose software, optimized software, hardware accelerators, protected key stores, secure elements, or a combination of these.

The PSA Crypto API does not depend on the PSA Crypto Driver Interface specification. An implementation can conform to the Crypto API without using the architecture or any of the interfaces defined here.

This specification defines an optional architecture and portable integration interfaces for implementations that separate the implementation of the Crypto API from the providers of cryptographic operations and key-management capabilities. Its intended use is within an implementation of the PSA Crypto API, with which it shares concepts, types, operation families, and programming conventions.

Portable integration contracts
------------------------------

A product can require cryptographic capabilities from several sources. For example, one hardware block might accelerate AES, a secure element might store private keys and perform signatures, and optimized software might provide the remaining algorithms. The required combination varies between products, even when they run the same application software.

This specification allows independently supplied components to be combined behind one application-facing Crypto API. It defines the following constituent material:

*  Common contracts that apply across the integration boundaries.
*  A structured Driver description that declares the capabilities and integration properties of a Driver.
*  The Dispatch interface used to request a configured cryptographic capability.
*  The Driver interface implemented by providers of cryptographic operations and key-management capabilities.
*  Integration requirements for constructing a product-specific implementation from the selected components.

The specification standardizes the portable boundaries and the responsibilities on each side of them. It does not standardize a particular Crypto API implementation, Dispatch Layer implementation, build system, code generator, or source-tree organization.
