# SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited
# SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
# SPDX-License-Identifier: Apache-2.0

# -*- coding: utf-8 -*-
#
# This is a common set of configuration options for using Sphinx to build
# PSA API specifications. Project specific definitions are contained in
# the conf.py file that forms the master document directory.
#
# This script is included and executed as part of conf.py, it is not a
# standalone python module or script.
#
# conf.py must have set up:
# * a dictionary doc_info, with project specific information and
#   configuration.
# * a string path psa_api_tool_path that defines the the path containing this
#   file, either relative to conf.py, or absolute.
#

import sys, os, re
from datetime import date
import importlib

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
sys.path.insert(0, os.path.abspath(psa_api_tool_path))

# When used within the extension, psa_api_tool_path must be a relative path
psa_api_tool_path = os.path.relpath(psa_api_tool_path)

# Validate the selected document template, or use the default one
def template_path(template):
    return os.path.join(psa_api_tool_path, 'templates', template)

default_template = 'psa-api-2026'

template = doc_info.get('template', default_template)
psa_api_template_path = template_path(template)
if not os.path.isdir(psa_api_template_path):
    print ('WARNING: Document template "{}" not found, falling back to "{}"'.format(template, default_template))
    psa_api_template_path = template_path(default_template)

# Process any template-specific configuration
# This must provide some required config, such as font info for html/latex
# This can also override, or fill in the doc_info dictionary

template_info = {}

template_conf_file = os.path.join(psa_api_template_path,'template-conf.py')
if os.path.exists(template_conf_file):
    exec(compile(open(template_conf_file, encoding='utf-8').read(), 'template-conf.py', 'exec'))

# -- helper functions

def subst_rst(item):
    if type(item) is str:
        return re.sub(r'(<<.+>>)',r'\ :issue:`\1`\ ',item)
    else:
        return item

def subst_latex(item):
    if type(item) is str:
        item = item.replace('_',r'\_')
        item = re.sub(r'(<<.+>>)',r'\\DUrole{issue}{\1}',item)
        return re.sub(r':(.+):`(.+)`',r'\\DUrole{\1}{\2}',item)
    else:
        return item

quality_map = {
    'DEV': 'Development',
    'ALP': 'Alpha',
    'BET': 'Beta',
    'REL': 'Final',
    }

def check_quality(quality):
    if not quality:
        return 'REL'    # Default to Final
    if quality.upper() in quality_map:
        return quality.upper()
    for q in quality_map.items():
        if q[1] == quality:
            return q[0]
    print("error: Invalid doc_info.quality value")
    return '<<Quality>>'

def full_quality(quality):
    return quality_map.get(quality, '<<Quality>>')

status_map = {
    'DFT': 'Draft',
    'CRV': 'Committee Review',
    'CPR': 'Post Committee Review',
    'MRV': 'Member Review',
    'MPR': 'Post Member Review',
    'MRC': 'Member Release Candidate',
    'MEM': 'Member Release',
    'PRV': 'Public Review',
    'PPR': 'Post Public Review',
    'PRC': 'Public Release Candidate',
    'PUB': 'Public Release',
    }

def check_status(status):
    if not status:
        return None
    if status.upper() in status_map:
        return status.upper()
    for s in status_map.items():
        if s[1] == status:
            return s[0]
    print("error: Invalid doc_info.status value")
    return '<<Status>>'

def full_status(status):
    return status_map.get(status, '<<Status>>')

def make_release(v, q, i, draft):
    release = f'{v}'
    release_full = release
    if q != 'REL':
        release += f'-{q.lower()}'
        release_full += f' {full_quality(q)}'
    issue = ''
    if i > 0 or draft:
        issue = f'{i}'
        if draft:
            issue += f'.{draft}'
    if issue:
        release += f'.{issue}'
        if q == 'REL':
            release_full += f'.{issue}'
        else:
            release_full += f' revision {issue}'
    return (release, release_full, issue)

def split_version(version):
    s = [int(x) for x in version.split('.')]
    if len(s) == 1:
        s.append(0)
    return s[0], s[1]

# -- Build PSA API specification configuration -----------------------------------

now = date.today()

# Extract and build the Sphinx configuration variables and document data
title = doc_info['title'].split('\n')
fulltitle = ' '.join(title)
rsttitle = ' |br| '.join(title)
htmltitle = '<br />'.join(title)
latextitle = '\\par '.join(title)
title = title[-1]

project = doc_info.get('project', fulltitle)
author = doc_info.get('author', 'Unattributed')
version = doc_info['version']
owner = doc_info.get('owner')
copyright_date = doc_info.get('copyright_date', now.strftime('%Y'))
doc_id = doc_info.get('doc_id', '<<Document ID>>')
quality = check_quality(doc_info.get('quality'))
quality_full = full_quality(quality)
issue_no = doc_info.get('issue_no', '<<Issue Number>>')
status = check_status(doc_info.get('status'))
draft = doc_info.get('draft')
# Handle old-style draft/rc scheme
release_candidate = None if draft else doc_info.get('release_candidate')
if status:
    if type(draft) is bool:
        print("error: Mixing old-style doc_info.draft with doc_info.status")
        draft = '<<draft>>' if draft else 0
    if release_candidate is not None:
        print("error: Mixing old-style doc_info.release_candidate with doc_info.status. Ignoring rc")
        draft = '<<rc{}>>'.format(release_candidate)
else:
    # no new-style status has been set, determine from draft/rc
    if release_candidate:
        status = 'PRC'
        draft = release_candidate
    elif type(draft) is int:
        # Mixing new-style draft sequencing with no status!
        print("error: Document status value required")
        status = '<<Status>>'
    elif draft:
        status = 'DFT'
        draft = 1
    else:
        status = 'PUB'
        draft = 0
# Enforce draft revision provided when required by status
if status in ('MEM','PUB'):
    if type(draft) is int and draft > 0:
        print("error: Release status must not have draft revision number")
        draft = '<<{}>>'.format(draft)
else:
    if not draft:
        print("error: Non-release status must have draft revision number")
        draft = '<<draft>>'

status_full = full_status(status)

status_watermark = {
    'DFT': 'DRAFT',
    'CRV': 'Review',
    'CPR': 'Review',
    'MRV': 'Review',
    'MPR': 'Review',
    'MRC': 'Candidate',
    'PRV': 'Review',
    'PPR': 'Review',
    'PRC': 'Candidate',
}
quality_watermark = {
    'DEV': 'Development',
    'ALP': 'ALPHA',
    'BET': 'BETA'
}
watermark = doc_info.get('watermark',
                         status_watermark.get(status,
                                              quality_watermark.get(quality)))

feedback = doc_info.get('feedback')
nowdate = now.strftime('%B %Y' if status == 'MEM' or status == 'PUB' else '%d/%m/%Y')
docdate = nowdate if status == 'DFT' else doc_info.get('date', nowdate)

c_index = doc_info.get('identifier_index', True)
page_break = doc_info.get('page_break', template_info.get('page_break','appendix'))

majorversion, minorversion = split_version(version)
extension = doc_info.get('extension_doc', None)
if extension:
    if extension == True:
        # Support previous syntax for this config item
        extension = 'Extension'
    version = '{} {}'.format(version, extension)

# The full version, including alpha/beta/rc tags.
release, release_full, issue = make_release(version, quality, issue_no, draft)

# Document filename
build_file = template_info.get('make_filename')
docname = build_file(doc_info, doc_id, fulltitle, release, status_full) if build_file else None
if not docname:
    docname = project.lower()
docname = docname.replace(' ','_')

# Build filename
filename = doc_info.get('filename', docname)

# Copyright notice, default to author
copyright_text = doc_info.get('copyright', author)
copyright = ' {} {}'.format(copyright_date, copyright_text)

# Create tags based on the content inclusion configuration

include_content = set(doc_info.get('include_content',[]))

if status == 'DFT':
    include_content.update(['rationale', 'todo', 'banner', 'comment'])
elif status in ('CRV','CPR','MRV','MPR','PRV','PPR'):
    include_content.update(['rationale', 'banner'])
elif status in ('MRC','PRC'):
    include_content.update(['banner'])

for option in include_content:
    tags.add('include_{}'.format(option))

# Substitutions for use in source and latex documents

doc_terms = {
    'docid': doc_id,                        # FPG
    'docfulltitle': fulltitle,              #  PG
    'docrsttitle': rsttitle,                #  P
    'dochtmltitle': htmltitle,              #  PG
    'doclatextitle': latextitle,            #  P
    'doctitle': title,                      # FPG
    'API': title,                           #AF
    'APIversion': version,                  #A  P
    'docversion': version,                  # legacy usage
    'majorversion' : '``{}``'.format(majorversion), # F
    'minorversion' : '``{}``'.format(minorversion), # F
    'hexversion' : '``0x{:02X}{:02X}``'.format(majorversion, minorversion), # F
    'docquality': quality_full,             #  P
    'docissue': issue or str(issue_no),     #  P
    'docstatus': status_full,               #   G
    'docauthor': author,                    #  PG
    'docdate': docdate,                     #  P
    'docrelease': release,                  #   G
    'docreleasefull': release_full,         # FP
    'doccopyright': copyright,              #  PG
    'docowner': owner,                      #  P
    'docconfidentiality': 'Non-confidential',
    'docfeedback': feedback,                #  PG
    'docwatermark': watermark,              #  PG
    'docchapterbreak': ('1' if page_break == 'chapter' else ''),   #  PG
    'docappendixbreak': ('1' if page_break == 'appendix' else ''), #  PG
}
# Filter out any missing or empty items
doc_terms = dict((k,v) for k,v in doc_terms.items() if v is not None and v != '')
# Add any extra terms from the template
for fd in template_info.get('terms', {}).items():
    doc_terms[fd[0]] = fd[1].format(**doc_terms)
# Add any formatted terms from the template
for fd in template_info.get('formatted_terms', {}).items():
    tag = fd[0]
    data = fd[1]
    doc_terms[tag] = f':{tag}:`{data}`'.format(**doc_terms)

logo_file = template_info['logo_file']

# -- psa-api-tool extension configuration --------------------------------------

primary_domain = 'psa_c'

psa_api_license = doc_info.get('license', 'missing')

psa_api_c_header = doc_info.get('header', filename)

psa_api_retval_order = doc_info.get('error_order',[])

psa_api_header_doxygen = doc_info.get('header_doxygen', 0)

psa_api_front_sections = template_info.get('front_sections',[])

# -- General configuration ------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.todo',
    'sphinx.ext.graphviz',
    'psa-api-tool'
]

try:
    if importlib.util.find_spec('sphinxext.opengraph') is not None:
        # Configuration for opengraph metadata
        ogp_site_url = '{{ site.url }}/'
        ogp_site_name = '{} {}'.format(fulltitle, version)
        extensions.append('sphinxext.opengraph')
except ModuleNotFoundError:
    pass

# Add any paths that contain templates here, relative to this directory.
templates_path = [os.path.join(psa_api_template_path,'sphinx-templates')]

# The suffix(es) of source filenames.
source_suffix =  {'.rst': 'restructuredtext'}

# The encoding of source files.
#source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
#language = None

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
#exclude_patterns = []

# Custom roles for specifications
roles = ['sc', 'issue'] + [k for k in template_info.get('formatted_terms',{})]
# Additional common substitutions
terms = {
    'impdef': ':sc:`implementation defined`',
    }

prolog = ['.. |br| raw:: html','','   <br />']
prolog += ['.. role:: {}'.format(r) for r in roles]
prolog += ['.. |{}| replace:: {}'.format(k, v) for k,v in terms.items()]
prolog += ['.. |{}| replace:: {}'.format(k, subst_rst(v)) for k,v in doc_terms.items()]
prolog += ['.. include:: {}'.format(fn) for fn in doc_info.get('prolog_files',[])]
if watermark:
    prolog += ['.. only:: html','','  .. container:: watermark','','    |docwatermark|','']
rst_prolog = '\n'.join(prolog) + '\n\n'

# The reST default role (used for this markup: `text`) to use for all
# documents.
default_role = 'any'

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# If true, keep warnings as "system message" paragraphs in the built documents.
keep_warnings = True

# Use a 'todo' in the include_content to indicate if TODOs are processed
todo_include_todos = ('todo' in include_content)

# Hide the source file name in the 'todolist'
todo_link_only = True

highlight_language = 'none'

# Number figures and tables, using the whole document scope
numfig = True
numfig_secnum_depth = template_info.get('numfig_sec_depth', 0)
numfig_format = {
    'figure': 'Figure %s',
    'table': 'Table %s',
    'code-block': 'Listing %s',
    'section': '§%s'
}

# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = template_info.get('html_theme', 'alabaster')

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
html_theme_options = {
    'fixed_sidebar': True,
}

# Add any paths that contain custom themes here, relative to this directory.
#html_theme_path = []

# The name for this set of Sphinx documents.
html_title = '{} {}'.format(fulltitle, version)

# A shorter title for the navigation bar.  Default is the same as html_title.
#html_short_title = None

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
html_logo = os.path.join(psa_api_template_path,logo_file+'.svg')

html_css_files = template_info['html_css_files']

# The name of an image file (relative to this directory) to use as a favicon of
# the docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
#html_favicon = None

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = [os.path.join(psa_api_template_path, 'html-static')]

# Add any extra paths that contain custom files (such as robots.txt or
# .htaccess) here, relative to this directory. These files are copied
# directly to the root of the documentation.
#html_extra_path = []

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
#html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
html_sidebars = {
   '**': ['toc.html', 'indextoc.html', 'searchbox.html'],
}

# A dictionary of values to pass into the template engine’s context for all
# pages.
html_context = doc_terms

# Additional templates that should be rendered to pages, maps page names to
# template names.
#html_additional_pages = {}

# If false, no module index is generated.
html_domain_indices = c_index
if not c_index:
    html_sidebars['**'].remove('indextoc.html')

# If false, no index is generated.
#html_use_index = True

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, the reST sources are included in the HTML build as _sources/name.
html_copy_source = False

# If true, links to the reST sources are added to the pages.
#html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
html_show_sphinx = False

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = None

# Language to be used for generating the HTML full-text search index.
# Sphinx supports the following languages:
#   'da', 'de', 'en', 'es', 'fi', 'fr', 'h', 'it', 'ja'
#   'nl', 'no', 'pt', 'ro', 'r', 'sv', 'tr'
#html_search_language = 'en'

# A dictionary with options for the search language support, empty by default.
# Now only 'ja' uses this config value
#html_search_options = {'type': 'default'}

# The name of a javascript file (relative to the configuration directory) that
# implements a search results scorer. If empty, the default will be used.
#html_search_scorer = 'scorer.js'

# Suffix for section numbers in HTML output. This removes trailing '.'
html_secnumber_suffix = ' '

# -- Options for LaTeX output ---------------------------------------------

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, filename+'.tex', fulltitle, author, 'manual'),
]

# Appendices are indicated in PSA API specifications using the .. appendix:: toctree
latex_appendices = []

latex_additional_files = [os.path.join(psa_api_template_path, 'psa-api-tool.sty')]
if 'latex_files' in template_info:
    latex_additional_files += [os.path.join(psa_api_template_path, f) for f in template_info['latex_files']]

# Construct the latex preamble
# Define all the project info used for the title page and footer
latex_preamble = ['\\def\\{}{{{}}}'.format(k,subst_latex(v)).replace('&','\\&') for k,v in doc_terms.items()]
# Include the PSA API-specific styling and content
latex_preamble += [r'\input{psa-api-tool.sty}']
# Improve all the code blocks if sphinx version supports it
latex_preamble += [r'\useverbatimfortt']

latex_elements = {
# The paper size ('letterpaper' or 'a4paper').
    'papersize': 'a4paper',
    'maketitle': r'',#r'\psamaketitle',
    'tableofcontents': r'',

# The font size ('10pt', '11pt' or '12pt').
    'pointsize': template_info['latex_pointsize'],

# font package: use the fonts specified in the template info, this should include any necessary \usepackage commands
    'fontpkg': '\n'.join(template_info['latex_fonts']),

# Include the PAS API specification definitions and preamble.
    'preamble': '\n'.join(latex_preamble),

# Latex figure (float) alignment
    'figure_align': '!ht',

# Other configuration
    'sphinxsetup': ','.join(template_info['latex_sphinxsetup']),

# Fix issue with mismatched flags when including textcomp package
# See https://github.com/sphinx-doc/sphinx/issues/4727#issuecomment-372096951
# Underlying issue is fixed in Sphinx 1.7.2, but this is harmless
    'passoptionstopackages': '''
\\PassOptionsToPackage{warn}{textcomp}
\\PassOptionsToPackage{linktocpage=true}{hyperref}
''',

# remove blank pages between chapters
    'extraclassoptions': 'openany,oneside',

# Keep the chapter titles simple
    'fncychap': '',
}

# The name of an image file (relative to this directory) to place at the top of
# the title page.
latex_logo = os.path.join(psa_api_template_path, logo_file+'.pdf')

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#latex_use_parts = False

# If true, show page references after internal links.
#latex_show_pagerefs = False

# If true, show URL addresses after external links.
#latex_show_urls = False

# If false, no module index is generated.
latex_domain_indices = c_index

# Set the standard table format for specifications. Individual tables can override
latex_table_style = template_info['latex_table_style']

#-- Options for graphviz extension ----------------------------------------

# Use SVG for html output, not PNG
graphviz_output_format = 'svg'

# Set the font used for graphviz diagrams
graphviz_dot_args = template_info['graphviz_dot_args']

#-- Options for mathjax ---------------------------------------------------

mathjax3_config = template_info['mathjax3_config']
