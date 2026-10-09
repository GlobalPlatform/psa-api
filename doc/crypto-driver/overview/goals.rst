.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _goals-requirements-use-cases:

Goals, requirements, and use cases
==================================

The interfaces in this specification are designed to support independently developed cryptographic components across a range of embedded and general-purpose products. The motivating configurations in this chapter are representative rather than exhaustive, and an implementation is not required to include every configuration or capability described here.

Motivating configurations
-------------------------

The architecture supports the following representative configurations:

*  **A single hardware accelerator.** A Driver can expose a narrow capability, such as an AES accelerator, while software Drivers provide algorithms that are not accelerated.

*  **Several overlapping capabilities.** More than one Driver can support the same operation family, with selection depending on properties such as the algorithm, key type, key size, key location, performance, or security characteristics.

*  **Optimized or hardened software.** An independently supplied software Driver can provide selected algorithms without being part of the Core or having a privileged built-in path.

*  **Protected key storage and operations.** A secure element or another protected environment can store keys and perform operations without exposing the key material to the Core.

*  **Built-in and provisioned keys.** A platform can contain read-only or provisioned keys that already exist when the application starts. The Crypto API describes how an application uses such keys, but leaves their provisioning and the discovery of their key identifiers implementation-defined.

*  **Composition of cryptographic capabilities.** A Driver for a high-level algorithm, such as a key derivation or password-authenticated key exchange algorithm, can request lower-level capabilities such as MAC or hash through the Dispatch interface.

*  **A constrained implementation.** A bootloader or small system can include only the required capabilities, avoid unnecessary runtime machinery, and use a Core and Dispatch Layer suited to its resource limits.

*  **Independently supplied components.** A Core, Drivers, and Dispatch tooling can come from different suppliers and can be compiled and maintained separately.

Design goals
------------

Application portability
~~~~~~~~~~~~~~~~~~~~~~~

Applications use the same Crypto API across products that integrate different combinations of hardware and software capabilities. This does not require every product to support every cryptographic capability, but the internal choice of provider does not require the application to use a provider-specific interface.

Independent composition
~~~~~~~~~~~~~~~~~~~~~~~

Cores and Drivers from different suppliers can be combined using specified interfaces, capability information, and allocated namespaces. Integration does not require a private agreement about calling conventions or the behavior of a supported entry point.

Portable software boundaries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Dispatch and Driver interfaces are portable C interfaces. Interaction with platform-specific hardware or software remains behind the applicable component boundary.

.. comment::

   If version 1.0 defines portable Platform or Utility interfaces, extend this goal to cover interfaces that ease Driver-to-platform or Driver-to-Core integration.

Flexible capability sets
~~~~~~~~~~~~~~~~~~~~~~~~

A Driver can implement any useful subset of the capabilities covered by this specification. It declares enough information about that subset for the integration to determine when its entry points are eligible to handle a request. A Driver is not invoked for a capability that it does not declare.

Configuration-specific optimization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The interfaces permit an integration to select Drivers and routing statically, including at build time. This allows an implementation to omit unused capabilities and optimize code and data for a target product without changing the portable interfaces. The specification does not require every implementation to use static configuration.

Selection and fallback
~~~~~~~~~~~~~~~~~~~~~~

The Driver descriptions and product configuration provide enough information to construct deterministic routing among the selected capabilities. They also identify when a request that is not supported by one selected capability can be offered to another eligible capability.

Uniform treatment of hardware and software
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Hardware-facing and software-only implementations participate through the same Driver and Dispatch architecture. Software supplied with a Core implementation is presented to Dispatch as one or more Drivers rather than as a special path outside the Dispatch interface.

Portable composition
~~~~~~~~~~~~~~~~~~~~

A high-level Driver can request lower-level cryptographic capabilities through Dispatch without selecting or depending on the Driver that supplies them. This composition, known as **Driver chaining**, allows the high-level Driver to benefit from the implementations selected for each target product. Chaining is constrained so that it cannot form recursive or cyclic call paths.

Stable operation routing
~~~~~~~~~~~~~~~~~~~~~~~~

After Dispatch selects a Driver for a multi-part operation, subsequent calls for that operation continue to use the same Driver. The interface contracts define enough state and lifecycle behavior for the Core, Dispatch Layer, and Driver to preserve this affinity.

Regular and testable contracts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Dispatch and Driver interfaces follow repeatable patterns derived from Crypto API capability families where practical. Irregular cases are specified explicitly. The responsibilities for initialization, validation, buffer handling, failure, and cleanup are defined precisely enough to support independent implementation, review, and testing.

Tools can be used to aid integration and to validate components or integration artifacts. Interoperability between components depends only on compliance with the interfaces and Driver-description requirements specified in this document, and not on the use of a particular tool.
