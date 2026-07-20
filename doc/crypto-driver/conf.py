# SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
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
    'title': 'PSA Crypto Driver Interface',

    # Document copyright date, default to year of 'date'
    'copyright_date': '2020-2026',

    # Document identifier, marked as open issue if not provided
    'doc_id': '111106',

    # The short X.Y version. MANDATORY
    'version': '1.0',
    # Document quality status, marked as open issue if not provided
    'quality': 'ALP',
   # Document maintenance revision
    'issue_no': 1,
    # Document draft revision
    'draft': 1,
    # Document status
    'status': 'DFT',

    # Id of the legal notice for this document
    # Marked as open issue if not provided
    #'license': 'psa-certified-api-license',

    # Document date, default to build date
    'date': 'September 2025',

    # Default header file for API definitions
    # default to None, and can be set in documentation source
    #'header': 'psa/crypto',

    # Doxygen annotation level of the generated header
    #    0 : None (default)
    #    1 : Primary API elements
    #    2 : Sub-elements of API - parameters, fields, values
    'header_doxygen': 2,

   # List of optional content that should be included in the build.
    # Valid options are:
    #   'rationale' : This enables output of ..rationale:: directives
    #   'banner'    : This enables output of the title page banner
    #   'todo'      : This enables output of ..todo:: directives
    'include_content': ['banner','rationale'],

    # Optional ordering of return error values
    # This list is used to create a standard ordering of return value responses
    # throughout the document, irrespective of their ordering in the source text
    # Return values that are not in the ordering are sorted above any that are in
    # the list and appear in source text order.

    #'error_order': [],

    # Include the C Identifier index. Default to True
    'identifier_index': False,

    # Specify where to add page breaks in main/appendix
    #   'none'     : no page breaks
    #   'appendix' : just before the appendices
    #   'chapter'  : before every chapter
    # Default to 'appendix'
    'page_break': 'chapter'
    }

# Set up and run the psa-api-tool configuration

import os

psa_api_tool_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
psa_api_tool_path = os.environ.get('PSA_API_TOOL') or psa_api_tool_path
exec(compile(open(os.path.join(psa_api_tool_path,'psa-api-conf.py'),
                  encoding='utf-8').read(),
             'psa-api-conf.py', 'exec'))
