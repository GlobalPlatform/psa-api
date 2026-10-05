..  SPDX-FileCopyrightText: Copyright 2020-2025 Arm Limited and/or its affiliates
..  SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform, Inc. and contributors
..  SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

.. _overview:
.. _driver-packages:

Driver packages
===============

Deliverables for a driver
~~~~~~~~~~~~~~~~~~~~~~~~~

To write a driver, you need to implement some functions with C linkage, and to declare these functions in a **driver description file**.
The driver description file declares which functions the driver implements and what cryptographic mechanisms they support.
If the driver description references custom types, macros or constants, you also need to provide C header files defining those elements.

The concrete syntax for a driver description file is JSON.
The structure of this JSON file is specified in the section :secref:`driver-description-syntax`.

A driver therefore consists of:

*   A driver description file (in JSON format).
*   C header files defining the types required by the driver description.
    The names of these header files are declared in the driver description file.
*   An object file compiled for the target platform defining the entry point functions specified by the driver description.
    Implementations may allow drivers to be provided as source files and compiled with the core instead of being pre-compiled.

How to provide the driver description file, the C header files and the object code is implementation-dependent.

.. _driver-description-list:

Driver description list
~~~~~~~~~~~~~~~~~~~~~~~

Crypto API core implementations should support multiple drivers.
The driver description files are passed to the implementation as an ordered list in an unspecified manner.
This may be, for example, a list of file names passed on a command line, or a JSON list whose elements are individual driver descriptions.
