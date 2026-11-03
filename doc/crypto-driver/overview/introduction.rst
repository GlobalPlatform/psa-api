..  SPDX-FileCopyrightText: Copyright 2020-2026 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _crypto-driver-purpose:

Crypto Driver Interface
=======================

Purpose
-------

The Crypto API defines an interface that allows applications to perform cryptographic operations in a uniform way regardless of how the operations are performed.
Under the hood, different keys may be stored and used in different hardware or in different logical partitions, and different algorithms may involve different hardware or software components.

The driver interface allows implementations of the Crypto API to be built compositionally.
An implementation of the Crypto API is composed of a **core** and zero or more **drivers**.
The core handles key management, enforces key usage policies, and dispatches cryptographic operations either to the applicable driver or to built-in code.

Functions in the Crypto API implementation invoke functions in the core.
Code from the core calls drivers as described in the present document.

Types of drivers
----------------

The PSA Crypto driver interface supports two types of cryptoprocessors, and accordingly two types of drivers.

*  **Transparent** drivers implement cryptographic operations on keys that are provided in cleartext at the beginning of each operation. They are typically used for hardware **accelerators**. When a transparent driver is available for a particular combination of parameters (cryptographic algorithm, key type and size, etc.), it is used instead of the default software implementation. Transparent drivers can also be pure software implementations that are distributed as plug-ins to a Crypto API implementation (for example, an alternative implementation with different performance characteristics, or a certified implementation).
*  **Opaque** drivers implement cryptographic operations on keys that can only be used inside a protected environment such as a **secure element**, a hardware security module, a smartcard, a secure enclave, etc. An opaque driver is invoked for the specific `key location <lifetimes-and-locations>` that the driver is registered for: the dispatch is based on the key's lifetime.

Requirements
------------

The present specification was designed to fulfill the following high-level requirements.

[Req.plugins]
   It is possible to combine multiple drivers from different providers into the same implementation, without any prior arrangement other than choosing certain names and values from disjoint namespaces.

[Req.compile]
   It is possible to compile the code of each driver and of the core separately, and link them together.
   A small amount of glue code may need to be compiled once the list of drivers is available.

[Req.types]
   Support drivers for the following types of hardware: accelerators that operate on keys in cleartext; cryptoprocessors that can wrap keys with a built-in keys but not store user keys; and cryptoprocessors that store key material.

[Req.portable]
   The interface between drivers and the core does not involve any platform-specific consideration.
   Driver calls are simple C function calls.
   Interactions with platform-specific hardware happen only inside the driver (and in fact a driver need not involve any hardware at all).

[Req.location]
   Applications can tell which location values correspond to which secure element drivers.

[Req.fallback]
   Accelerator drivers can specify that they do not fully support a cryptographic mechanism and that a fallback to core code may be necessary.
   Conversely, if an accelerator fully supports cryptographic mechanism, the core must be able to omit code for this mechanism.

[Req.mechanisms]
   Drivers can specify which mechanisms they support.
   A driver's code will not be invoked for cryptographic mechanisms that it does not support.
