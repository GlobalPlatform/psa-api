# SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited and/or its affiliates
# SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
# SPDX-License-Identifier: CC-BY-SA-4.0 AND LicenseRef-Patent-license

# PSA API document configuration
#
# This is used to generate all of the sphinx configuration data and determine
# the document file name etc.

doc_info = {
    # Document template
    'template': 'psa-api-2026',

    # Document title, MANDATORY
    'title': 'PSA Crypto API',

    # Document copyright date, default to year of 'date'
    'copyright_date': '2018-2026',

    # Document identifier, marked as open issue if not provided
    'doc_id': 'GPD_SPE_086',

    # The short X.Y version. MANDATORY
    'version': '1.5',
    # Document maintenance revision
    'issue_no': 0,
    # Document draft revision
    'draft': 1,
    # Document status
    'status': 'DFT',

    # Id of the legal notice for this document
    # Marked as open issue if not provided
    #'license': 'psa-certified-api-license',

    # Document date, default to build date
    'date': 'June 2026',

    # Default header file for API definitions
    # default to None, and can be set in documentation source
    #'header': 'psa/crypto',

    # Doxygen annotation level of the generated header
    #    0 : None (default)
    #    1 : Primary API elements
    #    2 : Sub-elements of API - parameters, fields, values
    'header_doxygen': 2,

    # Optional ordering of return error values
    # This list is used to create a standard ordering of return value responses
    # throughout the document, irrespective of their ordering in the source text
    # Return values that are not in the ordering are sorted above any that are in
    # the list and appear in source text order.

    'error_order': [
        'PSA_SUCCESS',
        'PSA_ERROR_BAD_STATE',
        'PSA_ERROR_INVALID_HANDLE',
        'PSA_ERROR_NOT_PERMITTED',
        'PSA_ERROR_INVALID_SIGNATURE',
        'PSA_ERROR_ALREADY_EXISTS',
        'PSA_ERROR_INSUFFICIENT_DATA',
        'PSA_ERROR_BUFFER_TOO_SMALL',
        'PSA_ERROR_INVALID_PADDING',
        'PSA_ERROR_INVALID_ARGUMENT',
        'PSA_ERROR_NOT_SUPPORTED',
        'PSA_ERROR_INSUFFICIENT_ENTROPY',
        'PSA_ERROR_INSUFFICIENT_MEMORY',
        'PSA_ERROR_INSUFFICIENT_STORAGE',
        'PSA_ERROR_COMMUNICATION_FAILURE',
        'PSA_ERROR_CORRUPTION_DETECTED',
        'PSA_ERROR_STORAGE_FAILURE',
        'PSA_ERROR_DATA_CORRUPT',
        'PSA_ERROR_DATA_INVALID'
    ],

    # Include the C Identifier index. Default to True
    'identifier_index': True,

    # Specify where to add page breaks in main/appendix
    #   'none'     : no page breaks
    #   'appendix' : just before the appendices
    #   'chapter'  : before every chapter
    # Default to 'appendix'
    'page_break': 'chapter',

    'prolog_files': ['/substitutions'],
    }

# Set up and run the psa-api-tool configuration

import os

psa_api_tool_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
psa_api_tool_path = os.environ.get('PSA_API_TOOL') or psa_api_tool_path
exec(compile(open(os.path.join(psa_api_tool_path,'psa-api-conf.py'),
                  encoding='utf-8').read(),
             'psa-api-conf.py', 'exec'))
