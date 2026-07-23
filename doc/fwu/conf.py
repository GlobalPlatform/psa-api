# SPDX-FileCopyrightText: Copyright 2020-2026 Arm Limited and/or its affiliates
# SPDX-License-Identifier: Apache-2.0

import os

psa_api_tool_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
psa_api_tool_path = os.environ.get('PSA_API_TOOL') or psa_api_tool_path
psa_api_config_path = os.path.join(os.path.dirname(__file__), 'psa-api.toml')
exec(compile(open(os.path.join(psa_api_tool_path,'psa-api-conf.py'),
                  encoding='utf-8').read(),
             'psa-api-conf.py', 'exec'))
