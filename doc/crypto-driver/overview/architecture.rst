.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _architecture:

Architecture
============

This chapter describes the architecture used when an implementation adopts the interfaces in this specification. It is not a required internal architecture for every implementation of the PSA Crypto API.

Architecture overview
---------------------

:numref:`fig-crypto-driver-architecture` shows the principal components, portable interfaces, and call paths. Applications and system software call the Crypto API implemented by the Core. The Core requests cryptographic work through the Dispatch interface. The configuration-specific Dispatch Layer implements that interface, selects an eligible Driver capability, and calls the corresponding entry point through the Driver interface.

.. figure:: /figure/architecture.*
   :name: fig-crypto-driver-architecture
   :width: 80%

   Core, Dispatch, and Driver architecture

A Driver can use the same Dispatch interface to request a lower-level capability. This Driver-chaining path returns through the Dispatch Layer and is subject to the same configured selection of lower-level capabilities.

Driver descriptions and product configuration provide input to the construction or configuration of the Dispatch Layer. They are integration inputs, not additional calls in the runtime path shown in the figure.

Components and interfaces
-------------------------

Clients and the Crypto API
~~~~~~~~~~~~~~~~~~~~~~~~~~

Applications and system software are clients of the Crypto API exposed by the Core. They select cryptographic algorithms and refer to keys using the Crypto API rather than selecting a Driver entry point.

The application-facing interface is specified in :cite-title:`PSA CRYPT`.

This is the primary architecture for this specification. Other cryptographic abstractions can reuse the Dispatch or Driver interface where suitable, but this architecture is used to explain the behavior of the interfaces in this specification.

Core
~~~~

The **Core** implements the application-facing Crypto API. It owns the high-level key-management and policy responsibilities of that API, including coordinating the mapping from a key identifier to the applicable key attributes and key material or Driver-specific key representation. Key material and persistent state do not necessarily reside in the Core.

The Core requests all cryptographic work and Driver-backed key-management work through the Dispatch interface.

The shared contracts for key handling, operation state, validation, and other responsibilities at the interfaces are defined in :secref:`common-contracts`.

.. admonition:: Change from ALPHA

   Unlike the ALPHA architecture, the Core does not select a special built-in cryptographic implementation outside Dispatch. Software implementations supplied with the Core are presented as Drivers through Dispatch.

Dispatch interface
~~~~~~~~~~~~~~~~~~

The **Dispatch interface** is the portable interface through which the Core requests a cryptographic or key-management capability. A Driver uses the same interface when it obtains a lower-level capability through Driver chaining. The caller describes the requested operation and supplies the information required for routing without specifying the Driver that will perform the work.

The Dispatch entry points and their contracts are specified in :secref:`crypto-dispatch-interface`.

Dispatch Layer
~~~~~~~~~~~~~~

The **Dispatch Layer** is the configuration-specific implementation of the Dispatch interface. It identifies an eligible Driver capability, adapts the Dispatch call to the applicable Driver entry point, and invokes that entry point through the Driver interface.

The Dispatch Layer can be generated, written by an integrator, supplied as part of an implementation, or constructed using a combination of these approaches. Its internal organization and construction mechanism are not specified, provided that it implements the portable contracts.

The routing contracts are defined in :secref:`common-routing-contracts`. Guidance on constructing the Dispatch Layer is provided in :secref:`dispatch-construction`.

Driver interface and Drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **Driver interface** is the portable interface implemented by cryptographic providers and called by the Dispatch Layer. It consists of entry-point families for cryptographic operations, key management, and other capabilities included in this specification.

A **Driver** implements any useful subset of these entry points and supplies a Driver description for the implemented capabilities. A Driver can interact with cryptographic hardware, provide a software implementation, or combine both approaches.

The Driver entry points and their contracts are specified in :secref:`crypto-driver-interface`. Driver packaging requirements are described in :secref:`driver-packages`.

Driver descriptions and product configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A **Driver description** declares the capabilities and integration properties of a Driver. It provides the portable information needed by an integrator or tool to construct routing for a selected product configuration. The description states what a Driver provides; it does not prescribe one Dispatch implementation or construction tool.

The **product configuration** selects the Drivers and capabilities included in one integrated implementation and can provide integration-specific selection policy. The interfaces permit this selection and routing to be resolved statically, including at build time, but do not require every implementation to do so.

The Driver-description information model and capability declarations are specified in :secref:`driver-description-reference`. Their use with product configuration to construct routing is described in :secref:`dispatch-construction`.

Routing and composition
-----------------------

Selection and fallback
~~~~~~~~~~~~~~~~~~~~~~

For each Dispatch call, the Dispatch Layer selects an eligible capability from the configured Drivers. Eligibility can depend on properties such as the operation, algorithm, key type, key size, or key location.

A Driver can report that a request is not supported by its selected capability. Dispatch attempts another eligible capability only when the Driver description permits fallback for that capability. If no eligible capability is available, or fallback ends without a capability supporting the request, the Dispatch call fails.

.. admonition:: Change from ALPHA

   Unlike the ALPHA architecture, there is no built-in software implementation for cryptographic operations that are not supported by any Driver. Software implementations supplied with the Core are presented as Drivers through Dispatch.

The matching, ordering, fallback, error, and cleanup rules are defined in :secref:`common-routing-contracts` and :secref:`common-error-contracts`.

Multipart Driver affinity
~~~~~~~~~~~~~~~~~~~~~~~~~

The Dispatch Layer selects a Driver when a multi-part operation is set up and records that selection in the operation state. It routes later calls for the operation to the same Driver. This affinity allows the Driver to maintain operation state without requiring every later call to repeat capability selection.

The operation-state representation and lifecycle requirements are defined in :secref:`common-operation-contracts`.

Driver chaining
~~~~~~~~~~~~~~~

**Driver chaining** occurs when a Driver requests a lower-level cryptographic capability through the Dispatch interface. For example, a key-derivation Driver can request a MAC capability, whose selected implementation can in turn request a hash capability.

Dispatch resolves the request in the same way as it does for a call from the Core, using an eligible capability that matches the request parameters. The selected capability can be in the same Driver or in a different Driver without creating an explicit dependency between those Drivers.

Chaining is constrained by ordered cryptographic composition layers. A capability can chain only to a lower layer, which prevents recursive or cyclic composition.

The composition and recursion-prevention rules are defined in :secref:`common-routing-contracts`.

.. todo::

   Define the composition-layer taxonomy, whether a layer is assigned to a whole Driver or an individual capability, how the assignment is represented in a Driver description, and how an integration validates the resulting call graph.

Keys and Driver categories
--------------------------

Version 1.0 of this specification defines :term:`Transparent Driver` and :term:`Opaque Driver` categories. These categories distinguish Drivers according to their key-handling behaviors as described in :secref:`driver-categories`.

.. admonition:: Change from ALPHA

   Unlike the ALPHA architecture, 'transparent' and 'opaque' are only used as Driver categories, and are not used to describe the key-handling behaviors themselves.

Drivers for secure elements that can cryptographically protect or store keys are typically opaque drivers. Keys associated with an opaque driver are used only by capabilities in the same driver.

Drivers for software implementations or hardware-accelerated cryptographic operations are typically transparent drivers. Capabilities in transparent drivers work with keys that are provided in formats specified by the PSA Crypto API. Typically, these are keys that are managed and stored by the Core, or keys provided in a Driver-chaining call.

Built-in and provisioned keys can exist outside Core-managed key storage. The Driver architecture supplies the information and entry points needed for the Core to coordinate their key identifiers and attributes while the owning Driver provides access to the key or operations using it.

Portable boundaries and implementation freedom
-----------------------------------------------

This specification defines the responsibilities of the Core, Dispatch Layer, and Drivers at their named interfaces. It does not standardize source directories, build files, generators, preprocessor configuration, or the internal data structures of those components unless a later interface contract explicitly requires a shared representation.

Drivers can require platform services or shared utility functions, such as memory management, synchronization, constant-time operations, or data-format helpers. The interface and integration requirements for these services are described in :secref:`supporting-services`. For implementation-specific services, consult the documentation for the Core implementation or platform SDK. These supporting services are not depicted in the principal cryptographic call path in :numref:`fig-crypto-driver-architecture`.
