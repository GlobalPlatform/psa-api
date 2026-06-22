.. SPDX-FileCopyrightText: Copyright 2018-2019, 2022-2025 Arm Limited and/or its affiliates
.. SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. Releases of this specification

.. release:: 1.0 beta 2
    :date: Feb 2019
    :confidentiality: Non-confidential

    Initial publication.

.. release:: 1.0.0
    :date: June 2019
    :confidentiality: Non-confidential

    First stable release with 1.0 API finalized.

    Uses the common PSA Certified Status codes.

    Modified the API parameters to align with other PSA Certified APIs.

    Added storage flags to specify protection requirement.

.. release:: 1.0.1
    :date: October 2022
    :confidentiality: Non-confidential

    Relicensed as open source under CC BY-SA 4.0.

    Documentation clarifications.

.. release:: 1.0.2
    :date: March 2023
    :confidentiality: Non-confidential

    Documentation clarifications.

.. release:: 1.0.3
    :date: January 2024
    :confidentiality: Non-confidential

    Provide a Security Risk Assessment.

.. release:: 1.0.4
    :date: September 2025
    :confidentiality: Non-confidential

    GlobalPlatform governance of PSA Certified evaluation scheme.

.. release-info::
    :extend:

    The detailed changes in each release are described in :secref:`document-history`.


.. References used in this specification

.. reference:: PSM
   :title: Platform Security Model
   :doc_no: ARM DEN 0128
   :url: developer.arm.com/documentation/den0128

.. reference:: PSA-CRYPT
    :title: PSA Certified Crypto API
    :doc_no: IHI 0086
    :url: arm-software.github.io/psa-api/crypto

.. reference:: PSA-STAT
    :title: PSA Certified Status code API
    :doc_no: ARM IHI 0097
    :url: arm-software.github.io/psa-api/status-code

.. reference:: PSA-FFM
    :title: Arm® Platform Security Architecture Firmware Framework
    :doc_no: ARM DEN 0063
    :url: developer.arm.com/documentation/den0063

.. reference:: SP800-30
    :title: NIST Special Publication 800-30 Revision 1: Guide for Conducting Risk Assessments
    :author: NIST
    :publication: September 2012
    :url: doi.org/10.6028/NIST.SP.800-30r1

.. Glossary terms used in this specification

.. term:: Application Root of Trust
    :abbr: ARoT

    This is the security domain in which additional security services are implemented. See :cite-title:`PSM`.

.. scterm:: Implementation Defined

    Behavior that is not defined by the this specification, but is defined and documented by individual implementations.

    Firmware developers can choose to depend on :sc:`IMPLEMENTATION DEFINED` behavior, but must be aware that their code might not be portable to another implementation.

.. term:: Non-secure Processing Environment
    :abbr: NSPE

    This is the security domain outside of the :term:`Secure Processing Environment`. It is the Application domain, typically containing the application firmware and hardware.

.. term:: Platform Root of Trust
    :abbr: PRoT

    The overall trust anchor for the system. This ensures the platform is securely booted and configured, and establishes the secure environments required to protect security services. See :cite-title:`PSM`.

.. term:: Root of Trust
    :abbr: RoT

    This is the minimal set of software, hardware and data that is implicitly trusted in the platform --- there is no software or hardware at a deeper level that can verify that the Root of Trust is authentic and unmodified.

.. term:: Root of Trust Service
    :abbr: RoT Service

    A set of related security operations that are provided by a :term:`Root of Trust`.

.. term:: Secure Partition

    A processing context with protected runtime state within the :term:`Secure Processing Environment`. A secure partition may implement one or more :term:`RoT Service`\s, accessible via well-defined interfaces.

.. term:: Secure Processing Environment
    :abbr: SPE

    This is the security domain that includes the :term:`Platform Root of Trust` and the :term:`Application Root of Trust` domains.

.. term:: Secure Partition Manager
    :abbr: SPM

    Part of the :term:`Secure Processing Environment` that is responsible for allocating resources to :term:`Secure Partition`\s, managing the isolation and execution of software within partitions, and providing IPC between partitions.


.. potential-for-change::

    The contents of this specification are stable for version 1.0.

    The following may change in updates to the version 1.0 specification:

    * Small optional feature additions.
    * Clarifications.

    Significant additions, or any changes that affect the compatibility of the
    interfaces defined in this specification will only be included in a
    new major or minor version of the specification.

.. about::