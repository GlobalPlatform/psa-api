# SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited
# SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
# SPDX-License-Identifier: Apache-2.0

# -*- coding: utf-8 -*-
#
# Adjust or reset the template_info dictionary with customized
# sphinx configurations for this template

template_info['logo_file'] = 'logo'
template_info['html_theme'] = 'alabaster'
template_info['html_css_files'] = [
    ('https://fonts.googleapis.com', { 'rel': 'preconnect' }),
    ('https://fonts.gstatic.com', { 'rel': 'preconnect', 'crossorigin': None }),
    ('https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,300;0,400;1,300;1,400&family=Roboto+Mono:ital,wght@0,300;0,400;1,300&display=swap', { 'rel': 'stylesheet' }),
]
template_info['mathjax3_config'] = {
    'chtml': {
        'scale': 0.9,
        'mtextInheritFont': False,
        'mtextFont': "Roboto",
    }
}
template_info['latex_pointsize'] = '11pt'
template_info['latex_fonts']= [
    r'\usepackage[scale=.95,sfdefault]{roboto}',
    r'\usepackage[scale=.76]{roboto-mono}',
    ]
template_info['latex_sphinxsetup'] = [
    # Reduce margins
    'hmargin={2cm,2cm}',
    'vmargin={2.5cm, 3cm}',
    'marginpar=1.27cm',
    # Format the verbatim blocks
    'verbatimwithframe=true',
    'verbatimsep=3pt',
    'VerbatimBorderColor={rgb}{0.9,0.9,0.9}',
    'verbatimborder=0.5pt',
    'VerbatimColor={rgb}{0.97,0.97,0.97}',
    # format hyperlink color
    'InnerLinkColor={rgb}{0.5,0,0.59}',
    'OuterLinkColor={rgb}{0.5,0,0.59}',
    # format admonitions
    'noteBorderColor={rgb}{0.667,0.667,0.667}',
    'warningBorderColor={rgb}{.75,0.5,0.5}',
    'warningborder=2pt',
    # Use attention admonition for the front page banner
    'attentionBorderColor={rgb}{.8,.8,0}',
    'attentionBgColor={rgb}{1,1,.7}',
    'attentionborder=1pt',
    # Use error admonition for rationale boxes
    'errorBorderColor={rgb}{.5,.75,.5}',
    'errorBgColor={rgb}{.9,.95,.9}',
    'errorborder=1pt',
    # Use hint admonition for comment boxes
    'hintBorderColor={rgb}{.6,.6,.6}',
    'hintBgColor={rgb}{.97,.97,.97}',
    'hintborder=0pt',
    'hintTextColor={rgb}{.4,.4,.4}',
    # Use the normal font for headings
    'HeaderFamily=\\normalfont\\mdseries',
]
template_info['latex_table_style'] = ['booktabs','nocolorrows']
template_info['graphviz_dot_args'] = [
    '-Gfontname=Lato',
    '-Gfontsize=12',
    '-Nfontname=Lato',
    '-Nfontsize=12',
    '-Efontname=Lato',
    '-Efontsize=12'
]

def make_doc_filename(info, id, title, version, status):
    doc_parts = [info.get('filetitle',title), version]
    status = status.split(' ')[-1].lower()
    if status != 'release':
        doc_parts += [status]
    return '-'.join(doc_parts)
template_info['make_filename'] = make_doc_filename

template_info['front_sections'] = [
        'introduction',
        'api-status',
        'feedback',
        'audience',
        'license',
        'references',
        'terms',
        'abbreviations',
        'release-info',
        'todos',
    ]
template_info['numfig_sec_depth'] = 1
template_info['page_break'] = 'chapter'

doc_info.setdefault('author', 'The PSA API contributors')
doc_info.setdefault('copyright', 'The PSA API contributors')
doc_info.setdefault('feedback', 'visit :url:`github.com/globalplatform/psa-api/issues`' +
                                ' to create a new issue at the PSA API GitHub project')
doc_info.setdefault('license', 'psa-api-license')
