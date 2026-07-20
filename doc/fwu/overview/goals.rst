.. SPDX-FileCopyrightText: Copyright 2020-2023, 2026 Arm Limited and/or its affiliates
.. SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _intro:

Firmware update
===============

Connected devices need a reliable and secure firmware update mechanism. Incorporating such an update mechanism is a fundamental requirement for fixing vulnerabilities, but it also enables other important capabilities such as updating configuration settings and adding new functionality. This can be particularly challenging for devices with resource constraints, as highlighted in :rfc-title:`8240`.

:numref:`fig-context` depicts the actors and agents involved in a typical firmware update scenario.

.. figure:: /figure/intro/context.*
   :name: fig-context

   A typical over-the-air firmware update scenario

In this example, the new firmware is uploaded by the Firmware creator to an Update server. The Update server communicates with an Update client application on the device, announcing the availability of new firmware. The client downloads the new firmware, and installs it into the device firmware storage.

In :numref:`fig-context`, the Update client has to combine the following capabilities:

* The specific protocols used by the network operator in which the device is deployed
* The specific mechanism used by the hardware platform to install firmware for execution

Devices developed for the Internet of Things (IoT) have a very diverse ecosystem of hardware and software developers, and utilize a broad set of communication protocols and technologies. This will lead to a large, fragmented set of Update clients, that are each tightly coupled to one hardware platform and one network protocol.

The |API| separates the software responsible for delivering the new firmware in the device, from the software that is responsible for storing and installing it in the device memory. :numref:`fig-api` shows how the |API| separates an Update client, which obtains the new firmware from the Firmware Server, from an Update service, which stores the firmware in the device memory.

.. figure:: /figure/intro/fwu-api.*
   :name: fig-api

   The |API|

In practice, this enables an Update client to be written independently of the firmware storage design, and the Update service to be written independently of the delivery mechanism.

The remainder of this document includes:

*   The design goals for the |API|. See :secref:`design-goals`.
*   A definition of the concepts and terminology used in this document. See :secref:`architecture`.
*   A description of the interface design. See :secref:`programming-model`.
*   A detailed definition of the API. See :secref:`api-reference`.

The appendices provide additional information:

*  A sample header file containing all of the API elements. See :secref:`appendix-example-header`.
*  Some example code demonstrating various use cases. See :secref:`examples`.

.. _design-goals:

Design goals
------------

This section describes the main goals and use cases for the |API|.

.. _goal-constrained:

Suitable for constrained devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The interface is suitable for a range of embedded devices: from those with resource-limited microcontrollers with one or two simple firmware images, to richer devices that have firmware images for multiple subsystems and separated applications.

For example, the following resource constraints can affect the |API|:

.. list-table::
   :header-rows: 1
   :widths: 1 3

   *  -  Resource
      -  Impact on interface requirements

   *  -  Volatile memory capacity
      -  Firmware images must be transferred to the device in blocks small enough to fit in device RAM.
   *  -  Non-volatile memory capacity
      -  Firmware updates must be small enough to be stored in memory prior to installation.
   *  -  Delivery bandwidth
      -  Firmware download can take an extended period of time. The device might restart during this process.
   *  -  Energy and power
      -  Downloading and installing updates must be reliable to avoid wasting energy on failed or repeated update attempts.
   *  -  Performance of cryptographic primitives
      -  The use of cryptographic protection for firmware updates must match the security requirements for the device.

For devices with sufficient resources, it is recommended to follow the :cite-title:`EBBR` specification, which prescribes the :cite-title:`UEFI` capsule update interface.

Updating the Platform Root of Trust
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The |API| is suitable for updating the device's :term:`Platform Root of Trust` (PRoT) firmware.

The :cite-title:`PSM` requires all of the :term:`Updatable Platform Root of Trust` firmware to be updatable. This can include bootloaders, Secure Partition Manager, Trusted OS, and runtime services. In some implementations, the PRoT can include a trusted subsystem with its own isolated and updatable firmware.

The :cite:`PSM` requirements for firmware update are also reflected in publications such as :cite-title:`IR8259` and :cite-title:`EN303645`, and in certification schemes such as :cite-title:`PSA-CERT`. `[PSA-CERT]` provides the following definition of the F.FIRMWARE_UPDATE security function, where the Target of Evaluation (TOE) refers to the PRoT:

   The TOE verifies the integrity and authenticity of the TOE update prior to performing the update.

   The TOE also rejects attempts of firmware downgrade.

Updating the Application Root of Trust
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to the PRoT firmware, other services that run in the :term:`Secure processing environment` (SPE), but outside of the PRoT, can require update via the |API|. These services may be combined with the updatable PRoT in a single firmware image, or provided in a separate firmware image.

Flexibility for different trust models
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are a number of factors that impact the trust model that is used to authorize device updates and firmware execution. For example:

*  A device can require firmware updates from multiple, mutually distrustful, firmware vendors.
*  Regulation can require implementations to use specified Certificate Authorities and PKI.
*  The entity that signs a firmware image can be distinct from the device owner or operator. An operator of a device can have a security policy that requires additional authorization to the firmware author's policy.

The |API| must be flexible enough to support the trust model required for particular products, without imposing unnecessary overheads on constrained devices.

Protocol independence
~~~~~~~~~~~~~~~~~~~~~

Different protocols are used to communicate with a device depending on the industry and application context. This includes open protocols, such as :cite-title:`LWM2M`, and proprietary protocols from cloud service providers. These protocols serve the specific needs of their respective markets.

Some of the protocols have :term:`manifest` data that is separate from the firmware image.

The |API| must be independent of the protocol used by the update client to receive an update.

Transport independence
~~~~~~~~~~~~~~~~~~~~~~

Embedded devices can receive over-the-air (OTA) firmware updates over different transport technologies, depending on the industry and the application. For example, this includes Wi-Fi, LTE, LoRa, and commercial low-power wide-area networks.

Some devices might not be directly connected to a network but may receive updates through a physical interface from an adjacent device, such as UART, CAN bus, or USB.

The |API| must be independent of the transport used by the update client to receive an update.

.. note::

   The |API| does not cover reprogramming of a device using a debug interface, for example, JTAG or SWD.

Firmware format independence
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many device manufacturers and cloud service providers have established formats for firmware images and manifests, tailored to the specific needs of their systems and markets.

The |API| must be independent of the format and encoding of firmware images and manifests, to enable adoption of the interface by systems with existing formats.

.. note::

   New standards for firmware update within IoT are being developed, such as :rfc-title:`9019`.

   This version of the |API| is suitable for some of the use cases that are defined by :rfc-title:`9124` and :cite-title:`SUIT-MFST`. For example, where the payloads are integrated in the manifest envelope, or there is just one external payload to the envelope.

   Support for the more complex use cases from :rfc:`9124`, with multiple external payloads, is not considered in version |APIversion| of the |API|, but might be in scope for future versions of the interface.

Flexibility for different hardware designs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The |API| is designed to be reasonably efficient to implement on different system-on-chip (SoC) architectures, while providing a consistent interface for update clients to target.

For example, the |API| should be effective in the following types of system:

*  SoCs that use bus filters, or equivalent security IP, to protect the :term:`SPE`.
*  SoCs that use multiple CPUs, providing an isolated CPU and memories for the SPE and another for the :term:`NSPE`.
*  Simple SoCs that use an :term:`MPU` or equivalent to protect the SPE.
*  Systems that have unified on-chip non-volatile memory used for firmware storage.
*  Systems that have isolated on-chip non-volatile memory used for firmware storage.
*  Systems that have a mixture of on-chip and external non-volatile memory used for firmware storage.

Suitable for composite devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some platforms have independent subsystems that are isolated from the main microprocessor. These subsystems can have their own firmware, which can also require updates. For example, radios, secure elements, secure enclaves, or other kinds of microcontroller.

The |API| must support an implementation updates these types of subsystem.

Robust and reliable update
~~~~~~~~~~~~~~~~~~~~~~~~~~

Devices that are remotely deployed, or are deployed in large numbers, must use an update process that does not have routine failure modes that result in devices that cannot be remotely recovered.

The |API| must support an update process that reduces the risk of in-field update failure, without compromising the requirements for :term:`secure boot`.

.. note::

   A device can also have an additional recovery capability, for example, a separate recovery firmware image that the bootloader can execute if the installed firmware cannot be verified.

   The |API| might be useful for implementation of recovery firmware, but the requirements of recovery firmware are not considered in the interface design.

Flexibility in implementation design
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The |API| is architectural and does not define a single implementation. An implementation can make trade-offs to target specific device needs. For example:

*  An implementation can provide a more robust solution, while others optimize for device cost.
*  An implementation can optimize for bandwidth efficiency, while others optimize for simplicity
*  An implementation can provide fine-grained update of personalization data, while others perform monolithic updates of all code and data.
*  An implementation can provide enhanced security for stricter markets, such as those which require encrypted firmware images, while others only use the |API| to provide a common interface across all products.

The |API| permits the omission of optional features that are not used by the implementation.
