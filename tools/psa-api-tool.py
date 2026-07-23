# SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited
# SPDX-License-Identifier: Apache-2.0

import os.path
import re
import string
import textwrap
from collections import namedtuple
from operator import attrgetter
from typing import cast

import sphinx.builders
import sphinx.directives.code
import sphinx.domains
import sphinx.environment
from docutils import nodes
from docutils.parsers.rst import directives
from sphinx import addnodes
from sphinx.directives.other import Include, TocTree
from docutils.parsers.rst.directives.tables import RSTTable
from sphinx.roles import XRefRole
from sphinx.util.nodes import make_refnode
from sphinx.util.docutils import SphinxRole, SphinxDirective
from sphinx.util.docutils import new_document

logger = sphinx.util.logging.getLogger(__name__)

def clean_dir(path):
    # Remove all files at the path and in subdirectories
    # Does not delete the directories themselves
    for the_file in os.listdir(path):
        file_path = os.path.join(path, the_file)
        if os.path.isfile(file_path):
            os.unlink(file_path)
        elif os.path.isdir(file_path):
            clean_dir(file_path)

_nested_parentheses_rs = r'(?:[^()]|\((?:[^()]|\([^()]*\))*\))*'
_nested_parentheses_re = re.compile(r'\s*\(({})\)\Z'
                                    .format(_nested_parentheses_rs))
def c_name_from_prototype(prototype):
    """Parse a C prototype and extract the name that it defines.

    This function supports most prototypes, but has a few limitations.
    The following features of the C language are not supported:
    * More than 3 levels of nested parentheses.
    * Arrays.
    * Functions or function pointers returning a function or function pointer.
    """
    # Strip optional leading "typedef" and trailing ";"
    prototype = re.sub(r'\s*;?\s*\Z', r'', prototype)
    prototype = re.sub(r'\A\s*(?:typedef)?\s*', r'', prototype)
    m = re.search(_nested_parentheses_re, prototype)
    if m:
        prototype = prototype[:m.start()]
        m = re.search(_nested_parentheses_re, prototype)
        if m:
            prototype = m.group(1)
    # Remove array subscript, or bitfield size
    m = re.search(r'(\w+)(?:\[.*\]|\s*:\s*\d+)?\Z', prototype)
    return m.group(1)

def nodes_text(nodes):
    return ''.join(n.astext() for n in nodes)

def autolinking_literal(text, element = nodes.literal):
    """Literal text with weak references.

    Return a list of inline nodes that together form ``text``.
    Isolate each C identifier (or keyword) in ``text`` as its own node,
    and make those nodes a weak reference.

    By default use nodes.literal for each element, but allow the caller
    to provide an alternate node constructor as ``element``.
    """
    parts = []
    cursor = 0
    for m in re.finditer(r'(0[xX][0-9A-Fa-f]*)|([A-Z_a-z][0-9A-Z_a-z]*)|(/\*.*?(?:\*/))|(/\*(?:.|\n|\r\n?)*?(?:\*/)|//.*?$)', text, re.M):
        item = m[0]
        if m[1]:
            continue
        elif m[2]:
            # An identifier
            xref = sphinx.addnodes.pending_xref(rawsource=item,
                                                reftype='weak',
                                                refdomain='psa_c',
                                                reftarget=item)
            xref += element(text=item)
        elif m[3]:
            # A special comment element in the prototype
            # Convert to a possible hyperlink if the text of the comment
            # is a link target
            word = re.sub(r'[ _:;,-.+=#~\/!?*]+','-',item[3:-3]).lower()
            rf = sphinx.addnodes.pending_xref(rawsource=word,
                                                reftype='ref',
                                                refdomain='std',
                                                refexplicit=True,
                                                reftarget=word)
            rf += element(text=item)
            xref = nodes.emphasis()
            xref += rf
        elif m[4]:
            # A regular C comment
            xref = nodes.emphasis(text=item)
        if cursor < m.start():
            parts.append(element(text=text[cursor:m.start()]))
        parts.append(xref)
        cursor = m.end()
    if cursor < len(text):
        parts.append(element(text=text[cursor:]))
    return parts

def autolinking_literal_block(text):
    """Literal text block with weak references.

    Return a nodes.literal_block containing auto-linked text.
    """

    # When a literal node is contained within a literal_block node, some
    # output formats compose the two formats and result in a messy rendering.
    #
    # As this is already formatted as a literal_block, we only need to use
    # Text blocks for the individual pieces of auto-linked text.
    def text_element(text):
        return nodes.Text(text)

    # Providing an explicit rawsource that is distinct from the content
    # causes the highlighter to bail out and preserve the auto-linking xrefs
    #
    # The meaningless '<psa-autolink>' is used for this purpose, so that if the
    # rawsource is used for debugging, it can still be identified in the
    # source document.
    block = nodes.literal_block(
        rawsource="<psa-autolink>" + text + '</psa-autolink>',
        language = "none")
    block += autolinking_literal(text, element=text_element)
    return block

def role_autolink(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    return autolinking_literal(text), []

class PSACodeBlock(sphinx.directives.code.CodeBlock, SphinxDirective):
    required_arguments = 0  # Override in case of using Sphinx <2.0
    optional_arguments = 1  # Override in case of using Sphinx <2.0

    def run(self):
        if not self.arguments or self.arguments[0] != 'xref':
            # A standard sphinx code-block::
            return super().run()

        code = self.content.data
        if 'linenos' in self.options or 'lineno-start' in self.options:
            start = self.options.get('lineno-start',1)
            nlines = len(code)
            just = len(str(start + nlines -1))
            numbers = [str(start + n).rjust(just) for n in range(0, nlines)]
            code = [l+'   '+r for l,r in zip(numbers,code)]
        return [autolinking_literal_block('\n'.join(code))]

class PSACodeBlockInclude(sphinx.directives.code.LiteralInclude, SphinxDirective):
    def run(self):
        if 'language' not in self.options or self.options['language'] != 'xref':
            # A standard sphinx literal-include::
            return super().run()

        # Use the literalinclude reader to implement the filtering options

        document = self.state.document
        if not document.settings.file_insertion_enabled:
            return [document.reporter.warning('File insertion disabled',
                                              line=self.lineno)]
        # convert options['diff'] to absolute path
        if 'diff' in self.options:
            raise ValueError('Cannot use "diff" option with "xref" language')

        try:
            location = self.state_machine.get_source_and_line(self.lineno)
            rel_filename, filename = self.env.relfn2path(self.arguments[0])
            self.env.note_dependency(rel_filename)

            reader = sphinx.directives.code.LiteralIncludeReader(filename, self.options, self.config)
            text, lines = reader.read(location=location)
            text = text.strip()

            if 'linenos' in self.options or 'lineno-start' in self.options:
                code = text.split('\n')
                start = reader.lineno_start
                nlines = len(code)
                just = len(str(start + nlines -1))
                numbers = [str(start + n).rjust(just) for n in range(0, nlines)]
                code = [l+'   '+r for l,r in zip(numbers,code)]
                text = '\n'.join(code)
            return [autolinking_literal_block(text)]
        except Exception as exc:
            return [document.reporter.warning(exc, line=self.lineno)]


# some raw latex nodes for document control

def latexnode(latex, directive=None):
    raw_node = nodes.raw('', text=latex, format = 'latex')
    if directive:
        raw_node.source, raw_node.line = directive.state_machine.get_source_and_line(directive.lineno)
    return raw_node

def latexnewpage(directive):
    return latexnode(r'\clearpage', directive)

def latexpageref(id, directive = None):
    return latexnode('\\psapageref{{{}}}'.format(id), directive)


# .. rationale:: directive for Information blocks that only appear if the 'include_rationale'
# tag is defined.

class Rationale(SphinxDirective):
    optional_arguments = 1
    final_argument_whitespace = True # used if derived class take an argument
    has_content = True

    def run(self):
        # build an internal only box using the rationale class/environment
        # If provided, the argument is the title, otherwise "Rationale" is used
        if not self.arguments:
            self.arguments = ['Rationale']
        title, m = self.state.inline_text('\n'.join(self.arguments), self.lineno)
        titlebold = nodes.strong()
        titlebold += title
        titlenode = nodes.paragraph()
        titlenode['classes'] += ['admonition-title']
        titlenode += titlebold
        content = nodes.container(classes = ['rationale','admonition'])
        content += titlenode
        content += m
        self.state.nested_parse(self.content, self.content_offset, content)
        # now wrap this in an ``.. only:: include_rationale`` node
        only = sphinx.addnodes.only(expr = 'include_rationale')
        only += content
        return [only]

# .. comment:: directive for Information blocks that only appear if the 'include_comment'
# tag is defined.

class Comment(SphinxDirective):
#    optional_arguments = 1
#    final_argument_whitespace = True # used if derived class take an argument
    has_content = True

    def run(self):
        # build an internal only box using the comment class/environment
        title, m = self.state.inline_text('Commentary', self.lineno)
        titlebold = nodes.strong()
        titlebold += title
        titlenode = nodes.paragraph()
        titlenode['classes'] += ['admonition-title']
        titlenode += titlebold
        content = nodes.container(classes = ['comment','admonition'])
        content += titlenode
        content += m
        self.state.nested_parse(self.content, self.content_offset, content)
        # now wrap this in an ``.. only:: include_comment`` node
        only = sphinx.addnodes.only(expr = 'include_comment')
        only += content
        return [only]

# directives for main document templating and control

class TemplateImage(directives.images.Image, SphinxDirective):
    def run(self):
        # locate image relative to the template directory, not the doc source
        file = os.path.join(self.config.psa_api_template_path,self.arguments[0])
        # The image:: directive takes a URI, so need to replace OS
        # path separators with URI path separators
        self.arguments[0] = os.path.normpath(file).replace(os.path.sep, '/')
        return super().run()

class TitlePage(SphinxDirective):
    """ The title page directive is the first thing in the main index file
        If it has content, this is taken to be the abstract for the document
    """
    required_arguments = 0
    has_content = True
    option_spec = {}

    def stash_abstract(self, content):
        item = {
            'content': content,
            'replace': True,
        }
        errors = []
        sections = self.env.domaindata['psa_c']['front-matter']
        if 'abstract' in sections:
            e = 'Duplicate directive "{}". Also defined in {}.'.format(
                    'abstract', sections['abstract'][0])
            errors.append(self.state_machine.reporter.warning(e, line = self.lineno))
        sections['abstract'] = (self.env.docname, item)
        return errors

    def run(self):
        messages = []
        source = self.state.document.current_source or self.get_source_info()[0] or ''
        if self.content:
            content = nodes.Element()
            self.state.nested_parse(self.content, self.content_offset, content)
            system_messages = [n for n in content.children if isinstance(n, nodes.system_message)]
            abstract = [n for n in content.children if not isinstance(n, nodes.system_message)]
            messages += system_messages
            if abstract and 'abstract' in self.config.psa_api_front_sections:
                messages += self.stash_abstract(abstract)
        title_path = os.path.join(os.sep,self.config.psa_api_template_path,'title-page.rst')
        lines = ['.. include:: ' + title_path]
        self.state_machine.insert_input(lines, source)
        return messages

class PSATocTree(TocTree):
    option_spec = {
        'numbered': directives.nonnegative_int,
        'maxdepth': directives.nonnegative_int,
    }

    def run(self):
        if self.html_numbered:
            num = self.options.setdefault('numbered', self.default_numbered)
        else:
            num = self.options.pop('numbered', self.default_numbered)
        self.options.setdefault('maxdepth', self.default_maxdepth)
        numfig_depth = self.env.config.numfig_secnum_depth
        fig_prefix = ''
        if numfig_depth == 1:
            fig_prefix = '\\thechapter-'
        elif numfig_depth == 2:
            fig_prefix = '\\thesection-'
        prefix = [latexnode('\\psa{}{{{}}}{{{}}}'.format(self.kind, num-1, fig_prefix), self)]
        toc = super().run()
        if self.html_numbered and self.alpha_numbers:
            toc[0][0]['alpha_numbers'] = True
        return prefix + toc

class FrontMatter(PSATocTree):
    default_numbered = 0
    html_numbered = False   # Do not number this in HTML formats, as is not sequential
    default_maxdepth = 2
    kind = 'frontmatter'

class MainToc(PSATocTree):
    default_numbered = 3
    html_numbered = True
    alpha_numbers = False
    default_maxdepth = 3
    kind = 'main'

class Appendix(PSATocTree):
    default_numbered = 3
    html_numbered = True
    alpha_numbers = True
    default_maxdepth = 3
    kind = 'appendix'

class About(Include, SphinxDirective):
    required_arguments = 0
    final_argument_whitespace = False
    option_spec = {}

    def run(self):
        self.arguments = [os.path.join(os.sep,self.config.psa_api_template_path,'about-chapter.rst')]
        return super().run()

class IncludeLicense(Include, SphinxDirective):
    required_arguments = 0
    final_argument_whitespace = False
    option_spec = {}

    def run(self):
        license_path = os.path.join(os.sep,self.config.psa_api_tool_path,'license')
        license = self.config.psa_api_license
        rel_filename, filename = self.env.relfn2path(license)
        if not os.path.isfile(filename):
            rel_filename = os.path.join(license_path,license.lower().replace('-','_')+'.rst')
            _, filename = self.env.relfn2path(rel_filename)
        error = []
        if not os.path.isfile(filename):
            rel_filename = os.path.join(license_path,'missing.rst')
            error = [self.state_machine.reporter.error(
                'Invalid or unknown license name/identifier "{}".'.format(license),
                line=self.lineno)]
        self.arguments = [rel_filename]
        return super().run() + error

class Collector(SphinxDirective):
    """ Collect structured data
        options are used for inlinable entries, and content is permitted for
        one multi-line item.
        When run this data is parsed and the dictionary is added to the
        domain data under a key named after the collector.
        Only one instance of each collected item allowed in the source
    """
    final_argument_whitespace = True # used if derived class take an argument
    content_as = 'content'
    unparsed_options = ()

    def stash(self, item):
        error = None
        name = self.name.split(":")[-1]
        sections = self.env.domaindata['psa_c']['front-matter']
        if name in sections:
            e = 'Duplicate directive "{}". Also defined in {}.'.format(
                    name, sections[name][0])
            error = self.state_machine.reporter.warning(e, line = self.lineno)
        sections[name] = (self.env.docname, item)
        return error

    def run(self):
        data = {}
        messages = []
        if len(self.arguments) > 0:
            data[self.argument_as], m = self.state.inline_text('\n'.join(self.arguments), self.lineno)
            messages += m
        for k, v in self.options.items():
            if v is None:
                data[k] = True
            elif k in self.unparsed_options:
                data[k] = v
            else:
                data[k], m = self.state.inline_text(v, self.lineno)
                messages += m
        if self.has_content:
            content = nodes.Element()
            self.state.nested_parse(self.content, self.content_offset, content)
            data[self.content_as] = content.children
        error = self.stash(data)
        if error:
            messages[0:0] = error
        return messages

class KeyedCollector(Collector):
    """ Collected data is stashed in a dictionary instead of a list.
        The data dictionary name is taken from the directive name.
        Derived classes can specify a different attribute to use as
        the key with id_key.
        By default a unique seq number is used as the dictionary key
    """
    id_key = None                    # attribute used as a key

    def stash(self, item):
        name = self.name.split(":")[-1]
        if self.id_key:
            key = canonical_rfc(nodes_text(item[self.id_key])).lower()
            item['id'] = nodes.make_id(self.id_key +'-' + key)
        else:
            # Use a unique key based on source document and line
            key = '{}.{}'.format(self.env.docname, self.lineno)

        error = None
        data = self.env.domaindata['psa_c'][self.id_key or name]
        if key in data:
            e = 'Duplicate item "{}" for directive "{}". Also defined in {}.'.format(
                    key, name, data[key][0])
            error = self.state_machine.reporter.warning(e, line = self.lineno)
        data[key] = (self.env.docname, item)
        return error

def option_choice(option, values):
    def option_check(argument):
        if argument in values:
            return argument
        raise ValueError(':{}: must be one of ({}).'.format(option, ', '.join(values)))
    return option_check

class FrontSection(Collector):
    option_spec = {
        'extend': directives.flag,
        'replace': directives.flag,
        'hide': directives.flag
    }
    has_content = True

    sections = []

    @classmethod
    def add_directives(cls, app):
        cls.sections[0:0] = app.config.psa_api_front_sections
        for sect in cls.sections:
            app.add_directive(sect, cls)

    def stash(self, item):
        name = self.name.split(":")[-1]
        errors = []
        n = sum([1 for o in self.option_spec.keys() if o in item])
        if n == 0:
            # no option provided
            if not item['content']:
                # No option provided, and no content. Treat as :hide:
                item['hide'] = True
                e = 'Empty directive "{}", treating as requesting :hide: option.'.format(name)
                errors.append(self.state_machine.reporter.warning(e, line = self.lineno))
            else:
                # default to replace
                item['replace'] = True
        elif n > 1:
            e = 'Only specify one of the options (:'
            e += ':, :'.join(self.option_spec.keys())
            e += ':) for directive "{}".'.format(name)
            errors.append(self.state_machine.reporter.error(e, line = self.lineno))

        error = super().stash(item)
        if error:
            errors.append(error)
        return errors


class InsertFrontSection(SphinxDirective):
    """ The argument is the section title
        The section id to insert must be provided by the :section: option
        The default section content follows.

        If the section is excluded by doc_info, then drop the section.

        If content is provided in the domain data, this replaces the default
        content, unless the :extend: flag was set, when it is appended to the
        default content.

        If the :break-after: flag is set a page break is made after the section.

        The :class: option wraps all the content in the specified class/environment.

        The :not-in-toc: flag will format this like a section title and content,
        but does not use a proper section node and does not result in a TOC entry.
    """
    required_arguments = 0
    optional_arguments = 1  # The heading for the section
    final_argument_whitespace = True
    option_spec = {
        'section': directives.unchanged_required,
        'break-after': directives.flag,
        'class': directives.class_option,
        'not-in-toc': directives.flag,
        'keep-if-empty': directives.flag,
    }
    has_content = True

    def new_section(self, raw, title):
        # Create the container, either a true section or a holding List
        #
        textnodes, messages = self.state.inline_text(title, self.lineno)
        if 'not-in-toc' in self.options:
            # use a styled paragraph for the title
            i = nodes.inline(title, '', classes = ['sectiontitle'])
            i += textnodes
            p = nodes.paragraph(title, '')
            p += i
            n = nodes.Element()
            n += p
            return n
        else:
            # Adapted from docutils new_subsection()
            section_node = nodes.section(raw_source=raw)
            titlenode = nodes.title(title, '', *textnodes)
            name = nodes.fully_normalize_name(titlenode.astext())
            section_node['names'].append(name)
            section_node += titlenode
            section_node += messages
            section_node.document = self.state.document
            self.state.document.note_implicit_target(section_node, section_node)
            return section_node

    def run(self):
        result = []

        name = self.options.get('section')
        if name and name not in FrontSection.sections:
            result.append(self.state_machine.reporter.error(
                'Section "{}" not recognised for "insert-section::".'.format(name),
                line = self.lineno))

        _, item = self.env.domaindata['psa_c']['front-matter'].get(name, (None, {}))

        if item.get('hide'):
            if not 'keep-if-empty' in self.options:
                # Hide this section
                return result
            # Cannot hide a keep-if-empty section
            result.append(self.state_machine.reporter.warning(
                'Cannot :hide: mandatory front section "{}".'.format(name),
                line = self.lineno))

        content = nodes.Element()
        if not item.get('replace'):
            self.state.nested_parse(self.content, self.content_offset, content)
        if item.get('replace') or item.get('extend'):
            content += item.get('content', [])
        content = content.children

        if not content and not 'keep-if-empty' in self.options:
            # Section is empty. Do not include it
            return result

        if len(self.arguments) == 0:
            self.options['not-in-toc'] = True
            section_node = nodes.Element()
        else:
            section_node = self.new_section('\n'.join(self.content), self.arguments[0])

        if 'class' in self.options:
            section_node += nodes.container(nodes_text(content),
                                      classes = self.options['class'],
                                      *content)
        else:
            section_node += content

        if 'break-after' in self.options:
            section_node.append(latexnewpage(self))

        if 'not-in-toc' in self.options:
            result.extend(section_node.children)
        else:
            result.append(section_node)
        return result

def resolve_references(node, docname, app):
    # helper to resolve references in nodes created during deferred processing
    document = new_document('')
    document += node
    app.env.resolve_references(document, docname, app.builder)
    document.remove(node)
    return node

# .. banner:: and .. insert-banner:: directive for a title-page Information block
# that only appears if the 'include_banner' tag is defined.

class banner(nodes.General, nodes.Element):
    pass

class Banner(Collector):
    has_content = True

class InsertBanner(SphinxDirective):
    def run(self):
        return [banner('')]

def process_banner_nodes(app, doctree, docname):
    _, data = app.env.domaindata['psa_c']['front-matter'].get('banner', (None,{}))
    for node in doctree.traverse(banner):
        if not app.builder.tags.has('include_banner') or not data.get('content'):
            node.parent.remove(node)
            continue

        banner_node = nodes.container(classes = ['banner','admonition'])
        banner_node += data['content']
        banner_node = resolve_references(banner_node.deepcopy(), docname, app)
        # build a box using the banner class/environment
        node.replace_self(banner_node)

class itemgetterornone:
    def __init__(self, item):
        self.item = item

    def __call__(self, obj):
        return obj.get(self.item)

class InsertTable(RSTTable, SphinxDirective):
    # Base class that will consume data from Collector directives
    option_spec = {
        'class': directives.class_option,
        'name': directives.unchanged_required,
        }
    optional_arguments = 1
    final_argument_whitespace = True

    def build_table(self, data, widths, keys=None, headers=None):
        # Build a table node and preceeding colspec from the data of dictionary
        # rows, given the structure defined.
        # Either Headers or keys have to be provided to interpret the dictionary
        # as columns.
        # If no keys are provided, the headers are used lower-case as keys.
        # Keys can either be strings, used directly in the dictionary, or callable
        # functions that are invoked on the dictionary.
        # Widths must match the number of columns in the headers/Keys
        #
        assert(keys or headers)

        keys = keys or [h.lower() for h in headers]
        ncols = len(keys)
        keys = [itemgetterornone(k) if isinstance(k, str) else k for k in keys]

        assert(widths and len(widths) == ncols)
        assert(not headers or len(headers) == ncols)

        title_node, _ = self.make_title()

        table = nodes.table('', classes = self.options.get('class',[]))
        self.set_source_info(table)
        self.add_name(table)
        if title_node:
            table += title_node

        tgroup = nodes.tgroup(cols=ncols)
        table += tgroup
        table['classes'].append('colwidths-given')
        for width in (widths):
            tgroup += nodes.colspec(colwidth=width)

        if headers:
            row = nodes.row()
            for t in headers:
                col = nodes.entry(t, nodes.paragraph(t, nodes.Text(t,t)))
                row += col
            thead = nodes.thead('', row)
            tgroup += thead

        tbody = nodes.tbody()
        for r in data:
            row = nodes.row()
            for k in keys:
                v = k(r)
                col = nodes.entry()
                if v is not None:
                    if len(v) and isinstance(v[0], nodes.Text): # workaround early sphinx issue
                        v = nodes.paragraph(nodes_text(v), *v)
                    col += v
                row += col
            r['targetdoc'] = self.env.docname
            tbody += row
        tgroup += tbody

        return [table] if len(tbody) else []

class Release(KeyedCollector):
    # Capture the information for a single document release, which is
    # collated into the Release Information table
    #
    option_spec = {
        'date': directives.unchanged,
        'confidentiality': option_choice("confidentiality", ("Confidential", "Non-confidential")),
    }
    optional_arguments = 1
    argument_as = 'version'
    content_as = 'change'
    has_content = True

class ReleaseTable(InsertTable):
    def run(self):
        self.options['class'] = ['longtable'] # splits over pages better if section is not in its own page

        data = (release for _, release in self.env.domaindata['psa_c']['release'].values())
        return self.build_table(data,
            headers = ('Date', 'Version', 'Change'),
            widths = (4,3,13)
            )

reference_kinds = ("normative", "informative")

def canonical_rfc(text):
    m = re.match(r'\ARFC([1-9][0-9]*)\Z', text, flags = re.I)
    if m:
        text = 'RFC ' + m.group(1)
    return text


class Reference(KeyedCollector):
    # Capture the information for a single referenced document, which is
    # collated into the References table, and can be referred via the
    # :cite: and :cite-ref: roles.
    option_spec = {
        'doc_no': directives.unchanged_required,    # legacy alias for doc_id
        'doc_id': directives.unchanged_required,
        'author': directives.unchanged_required,
        'title': directives.unchanged_required,
        'publication': directives.unchanged_required,
        'url': directives.uri,
        'kind': option_choice("kind", reference_kinds),
    }
    required_arguments = 1
    argument_as = 'cite'
    has_content = False
    id_key = 'cite'
    unparsed_options = ('kind',)
    default_kind = 'normative'

    def stash(self, item):
        errors = []
        # Move legacy 'doc_no' option to the current 'doc_id', without overriding
        if 'doc_no' in item:
            doc_no = item.pop('doc_no')
            if 'doc_id' in item:
                e = "Both 'doc_id' and 'doc_no' specified, ignoring 'doc_no'"
                errors.append(self.state_machine.reporter.warning(e, line = self.lineno))
            else:
               item['doc_id'] = doc_no
        item.setdefault('kind', self.default_kind)
        error = super().stash(item)
        if error:
            errors.append(error)
        return errors

class ReferenceTable(InsertTable):
    option_spec = InsertTable.option_spec.copy()
    option_spec['filter'] = option_choice("filter", ("arm", "non-arm", "with-id", "without-id", "none"))
    option_spec['kind'] = option_choice("kind", reference_kinds + ("all",))
    option_spec['layout'] = option_choice("layout", ("by-ref", "by-id"))
    option_spec['sorted'] = directives.flag

    @staticmethod
    def resolve_ref(env, ref):
        # Called during Xref resolution. The Reference table has already been inserted
        # so the target document for the citation is known
        data = env.domaindata['psa_c']['cite']
        if ref not in data:
            return '', '', ''
        _, item = data[ref]
        return item.get('targetdoc',''), item['id'], item.get('title','')

    @staticmethod
    def make_reference_target(reference):
        para = nodes.paragraph()
        para += nodes.target('', ids = [reference['id']])
        para += nodes.Text('[{}]'.format(nodes_text(reference['cite'])))
        return para

    @staticmethod
    def make_reference_content(reference):
        para = nodes.paragraph()
        if 'title' in reference:
            if 'author' in reference:
                para += reference['author']
                para += nodes.Text(', ')
            para += nodes.emphasis('',*reference['title'])
            if 'publication' in reference:
                para += nodes.Text(', ')
                para += reference['publication']
            para += nodes.Text('. ')
        if 'url' in reference:
            ref = reference['url'][0]
            if not isinstance(ref, nodes.reference):
                ref = nodes.reference(text=ref, refuri='https://' + ref)
            para += [ref]
        return para

    @staticmethod
    def make_id_reference_specification(reference):
        para = nodes.paragraph()
        para += reference.get('doc_id', reference['cite'])
        return para

    keys3 = [make_reference_target.__func__, 'doc_id', make_reference_content.__func__]
    keys2 = [make_reference_target.__func__, make_reference_content.__func__]
    keys_by_id = [make_id_reference_specification.__func__,
                  make_reference_content.__func__,
                  make_reference_target.__func__]

    def run(self):
        self.options['class'] = ['longtable'] # tabulary formats targetted cells badly

        layout = self.options.get('layout', 'by-ref')
        if layout == 'by-id':
            keys = self.keys_by_id
            headers = ['Standard / Specification', 'Description', 'Ref']
            widths = (6,14,4)
        else:
            keys = self.keys3
            headers = ['Ref', 'Document Number', 'Title']
            widths = (4,4,13)

        filter = self.options.get('filter','none')
        if filter == 'arm':
            filter = 'with-id'
        elif filter == 'non-arm':
            filter = 'without-id'
        if layout == 'by-ref' and filter == 'without-id':
            keys = self.keys2
            headers.pop(1)
            widths = (widths[0],sum(widths[1:]))

        kind = self.options.get('kind', 'all')
        data = self.env.domaindata['psa_c']['cite']
        def data_iter():
            ids = data.keys()
            if 'sorted' in self.options:
                ids = sorted(ids)

            for id in ids:
                _, item = data[id]
                if filter == 'with-id' and not 'doc_id' in item:
                    continue
                if filter == 'without-id' and 'doc_id' in item:
                    continue
                if kind != 'all' and kind != item['kind']:
                    continue
                yield item

        return self.build_table(data_iter(),
            headers = headers,
            widths = widths,
            keys = keys
            )

def role_cite(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    target = text.lower()
    ref = sphinx.addnodes.pending_xref(rawsource = rawtext,
                                       refdomain = 'psa_c',
                                       reftype = name,
                                       reftarget = target,
                                       refwarn = True,
                                       support_smartquotes=False
                                      )
    ref += nodes.inline(text, '[{}]'.format(text))
    return [ref], []

class Term(KeyedCollector):
    # Capture the information for a single glossary term
    #
    option_spec = {
        'abbr': directives.unchanged_required,
    }
    required_arguments = 1
    final_argument_whitespace = True
    argument_as = 'term'
    has_content = True
    content_as = 'definition'
    id_key = 'term'

    def stash(self, item):
        typ = self.name.split(':')[-1]
        cls = ['sc'] if typ == 'scterm' else []
        item['cls'] = cls
        a_error = None
        if typ == 'abbr':
            item['isabbr'] = True
        elif 'abbr' in item:
            # Add extra term for the abbreviation
            term = nodes_text(item['term'])
            defn = nodes.paragraph(':{}:`{}`'.format(typ, term))
            defn += role_term(typ, ':{}:`{}`'.format(typ, term), term, 0, None)[0]
            dabbr = {'term': item['abbr'], 'definition' : [defn], 'cls' : cls, 'isabbr': True }
            a_error = super().stash(dabbr)
        t_error = super().stash(item)
        if not t_error:
            return a_error
        elif not a_error:
            return t_error
        else:
            return [t_error, a_error]

term_kinds = ("terms", "abbreviations")

class TermTable(InsertTable):
    option_spec = InsertTable.option_spec.copy()
    option_spec['sorted'] = directives.flag
    option_spec['kind'] = option_choice("kind", term_kinds + ("all",))

    @staticmethod
    def resolve_ref(env, ref):
        # Called during Xref resolution. The Term table has already been inserted
        # so the target document for the citation is known
        data = env.domaindata['psa_c']['term']
        if ref not in data:
            return '', ''
        _, item = data[ref]
        return item.get('targetdoc',''), item['id']

    @staticmethod
    def make_term_target(term):
        para = nodes.paragraph()
        para += nodes.target('', ids = [term['id']])
        tnode = nodes.inline(nodes_text(term['term']), classes = term['cls'], *term['term'])
        if 'abbr' in term:
            tnode += nodes.Text(' (')
            tnode += term['abbr']
            tnode += nodes.Text(')')
        para += tnode
        return para

    keys = [make_term_target.__func__, 'definition']

    def run(self):
        self.options['class'] = ['longtable'] # tabulary formats targetted cells badly

        kind = self.options.get('kind', 'all')
        if kind == 'abbreviations':
            headers = ('Abbreviation','Meaning')
        else:
            headers = ('Term','Definition')

        data = self.env.domaindata['psa_c']['term']
        def data_iter():
            ids = data.keys()
            if 'sorted' in self.options:
                ids = sorted(ids)

            for id in ids:
                _, item = data[id]
                if kind == 'terms' and 'isabbr' in item:
                    continue
                if kind == 'abbreviations' and not 'isabbr' in item:
                    continue
                yield item

        return self.build_table(data_iter(),
            headers = headers,
            widths = (1,3),
            keys = self.keys
            )

def role_term(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    target = text.lower()
    ref = sphinx.addnodes.pending_xref(rawsource = rawtext,
                                       refdomain = 'psa_c',
                                       reftype = name,
                                       reftarget = target,
                                       refwarn = True,
                                       support_smartquotes=False
                                      )
    ref += nodes.inline(text, text)
    return [ref], []

ref_role = XRefRole(warn_dangling=True, innernodeclass = nodes.inline)

def role_secref(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    return ref_role('psa_c:secref', rawtext, text, lineno, inliner, options, content)

def role_numref(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    return ref_role('psa_c:numref', rawtext, text, lineno, inliner, options, content)

def role_rfc(role, rawtext, text, lineno, inliner, options={}, content=[]):
    # Link this into the citation/references scheme used by PSA template
    # :rfc:`nnnn`     -> :cite-title:`RFCnnnn`
    #   The cite-reference would link to an entry for the RFC in the References
    #   table.
    # :rfc:`nnnn#x.y` -> :rfc:`nnnn` >&sect;x.y<
    # :rfc:`nnnn#Z`   -> :rfc:`nnnn` >Appendix Z<
    #   The section/appendix link would be a hyperlink to the online copy of
    #   the RFC section/appendix
    #
    url_template = inliner.document.settings.rfc_base_url + inliner.rfc_url
    m = re.match(r'\A(?P<num>[1-9][0-9]*)(?:#(?P<section>.*))?\Z', text)
    if not m:
        msg = inliner.reporter.error(
            'RFC number must be a number greater than or equal to 1; '
            f'"{text}" is invalid.', line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]

    rfc = m.group('num')
    ret, _ = role_cite(role.lower().replace('rfc','cite'), rawtext, f'RFC {rfc}', lineno, inliner, **options)

    url = url_template % int(m.group('num'))
    section = m.group('section')
    if section:
        ret.append(nodes.Text(' '))
        if section[0].isdigit():
            url += f'#section-{section}'
            title = f'§{section}'
        else:
            url += f'#appendix-{section}'
            title = f'Appendix {section}'
        ret.append(nodes.reference(rawtext, title, refuri=url, **options))
    return ret, []

def role_url(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    target = text
    if '//' not in target:
        target = f'https://{target}'
    ref = nodes.reference(rawtext, text=text, refuri = target)
    return [ref], []

sra_elements = {
    'deployment-model': 'dm',
    'adversarial-model': 'am',
    'security-goal': 'sg',
    'threat': 't',
    'mitigation': 'm',
}

def canonical_sra_id(typ, text):
    typ = typ.split(':')[-1]
    typ = sra_elements.get(typ, typ)
    typ = typ.upper() + '.'
    if text.upper().startswith(typ):
        text = text[len(typ):]
    return typ + text

# role for SRA references
def role_sra_ref(name, rawtext, text, lineno, inliner,
            options={}, content=[]):
    text = canonical_sra_id(name, text)
    ref = sphinx.addnodes.pending_xref(rawsource = rawtext,
                                       refdomain = 'psa_c',
                                       reftype = name,
                                       reftarget = text,
                                       refwarn = True,
                                       support_smartquotes=False
                                      )
    ref += nodes.inline(text, text)
    return [ref], []

class SRADefinition(SphinxRole):
    # role for SRA definitions
    @staticmethod
    def resolve_ref(env, ref):
        # Called during Xref resolution.
        # Return document, and definition label, or None if not present
        return env.domaindata['psa_c']['sra'].get(ref, (None, None))

    def run(self):
        # canonicalise the text
        text = canonical_sra_id(self.name, self.text)
        key = nodes.make_id(text)

        data = self.env.domaindata['psa_c']['sra']
        error = []
        if key in data:
            e = 'Duplicate SRA item "{}" for directive "{}". Also defined in {}.'.format(
                    text, self.name, data[key][0])
            error.append(self.inliner.reporter.warning(e, line = self.lineno))
        data[key] = (self.env.docname, text)

        node = [
            nodes.target('', ids = ['sra-' + key]),
            nodes.inline(self.rawtext, text, classes=['sradef'])
        ]
        return node, error

psa_roles = {
    'code': role_autolink,
    'cite': role_cite,
    'cite-title': role_cite,
    'term': role_term,
    'scterm': role_term,
    'rfc': role_rfc,
    'rfc-title': role_rfc,
    'secref': role_secref,
    'numref': role_numref,
    'url': role_url,
    # SRA roles
    'deployment-model': SRADefinition(),
    'dm': role_sra_ref,
    'adversarial-model': SRADefinition(),
    'am': role_sra_ref,
    'security-goal': SRADefinition(),
    'sg': role_sra_ref,
    'threat': SRADefinition(),
    't': role_sra_ref,
    'mitigation': SRADefinition(),
    'm': role_sra_ref,
}
psa_directives = {
    'code-block': PSACodeBlock,
    'literalinclude': PSACodeBlockInclude,
    'rationale': Rationale,
    'comment': Comment,
}

class ThreatData:
    def __init__(self, DMs):
        # set of deployment models specified in threat:: option
        self.DMs = DMs
        # Threat definition
        self.data = {}

class ThreatSubitem(SphinxDirective):
    @property
    def threat(self):
        return self.env.domaindata['psa_threat']['local']

    @property
    def subitem(self):
        return self.name.split(':')[-1]

class ThreatElement(ThreatSubitem):
    # Process a general element in a threat definition
    has_content = True

    def run(self):
        self.assert_has_content()
        if self.subitem in self.threat.data:
            raise self.warning('Directive "{}" already provided for this threat.'.format(self.subitem))
        container = nodes.Element()
        self.state.nested_parse(self.content, self.content_offset, container)
        self.threat.data[self.subitem] = container.children
        return []

class ThreatRisk(ThreatSubitem):
    # Process a risk element in a threat definition
    optional_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        'impact': directives.unchanged_required,
        'likelihood': directives.unchanged_required,
        'risk': directives.unchanged
    }
    has_content = False
    risk_abbr = {'VL': 'Very Low', 'L': 'Low', 'M': 'Medium', 'H': 'High', 'VH': 'Very High'}
    risk_ix = {'Very Low': 0, 'Low': 1, 'Medium': 2, 'High': 3, 'Very High': 4}
    risk_matrix = ( # by likelihood, then impact
        ('Very Low' , 'Very Low' , 'Very Low' , 'Low'      , 'Low'      ),
        ('Very Low' , 'Very Low' , 'Low'      , 'Low'      , 'Medium'   ),
        ('Very Low' , 'Low'      , 'Medium'   , 'Medium'   , 'High'     ),
        ('Low'      , 'Low'      , 'Medium'   , 'High'     , 'Very High'),
        ('Low'      , 'Medium'   , 'High'     , 'Very High', 'Very High')
    )

    def parse_risk(self, risk):
        risk = self.risk_abbr.get(risk, risk)
        textnodes, messages = self.state.inline_text(risk, self.lineno)
        p = nodes.paragraph(nodes_text(textnodes))
        p += textnodes
        p += messages
        return risk, p

    @staticmethod
    def overall_risk(impact, likelihood):
        impact_ix = ThreatRisk.risk_ix.get(impact)
        likelihood_ix = ThreatRisk.risk_ix.get(likelihood)
        if impact_ix is None or likelihood_ix is None:
            return ':issue:`N/A`'
        return ThreatRisk.risk_matrix[likelihood_ix][impact_ix]

    def get_element(self, content, n, default=None):
        t, content[n] = self.parse_risk(self.options.get(n, default))
        return t

    def run(self):
        DM = self.arguments[0].strip() if self.arguments else ''
        content = {}
        impact = self.get_element(content, 'impact')
        likelihood = self.get_element(content, 'likelihood')
        self.get_element(content, 'risk',
                         self.overall_risk(impact, likelihood))

        if DM not in self.threat.DMs:
            self.threat.DMs.append(DM)
        for k, v in content.items():
            term = '-'.join((self.subitem, k))
            self.threat.data.setdefault(term, {})[DM] = v
        return []

class ThreatDomain(sphinx.domains.Domain):
    """Description of a Threat in a PSA SRA document."""
    name = 'psa_threat'
    label = 'PSA Threat'
    directives = {
        'description': ThreatElement,
        'adversarial-model': ThreatElement,
        'security-goal': ThreatElement,
        'unmitigated': ThreatRisk,
        'mitigations': ThreatElement,
        'residual': ThreatRisk,
    }
    directives.update(psa_directives)
    roles = psa_roles

    def __init__(self, env, data):
        super().__init__(env)
        self.data['local'] = data

def comma_list(s):
    return [x.strip() for x in s.split(',')]

class Threat(SphinxDirective):
    """Process a Threat definition.

       This must have at least one of a Threat Id (specified using :id: option), and/or
       a Threat title, specified as the argument to the directive.

       If :deployments: are specified, this is the number of deployments for which
       risks are defined.
    """
    has_content = True
    optional_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        'id': directives.unchanged,
        'deployment-models': comma_list,
    }

    @staticmethod
    def finalize_DMs(threat):
        if len(threat.DMs) > 1 or (len(threat.DMs)==1 and threat.DMs[0] != ''):
            for dm in threat.DMs:
                if len(dm.split())==1:
                    n, _ = role_sra_ref('psa_threat:dm', dm, dm, None, None)
                else:
                    n = nodes.inline(dm, dm, classes=['sraref'])
                p = nodes.paragraph(dm, '')
                p += n
                threat.data.setdefault('deployment-model',{})[dm] = p

    @staticmethod
    def value_or_na(n):
        if not n:
            na = 'N/A'
            n = nodes.paragraph(na, '', nodes.Text(na))
        return n

    @staticmethod
    def entry(n):
        n = Threat.value_or_na(n)
        return nodes.entry(nodes_text(n), n)

    @staticmethod
    def std_item(threat, data):
        return data

    @staticmethod
    def risk_item(threat, data):
        n_dm = len(threat.DMs)
        if n_dm == 1:
            # if we have a single DM, just output the item
            return Threat.value_or_na(data.get(threat.DMs[0]))
        else:
            # if we have more than one DM, build a borderless, unpadded table
            wrap = nodes.container('', classes=['riskrow'])
            table = nodes.table('', classes = ['borderless', 'colwidths-given'])
            #self.set_source_info(table)
            tgroup = nodes.tgroup(cols = n_dm)
            for _ in range(n_dm):
                tgroup += nodes.colspec(colwidth = 1)
            tbody = nodes.tbody()
            row = nodes.row()
            for dm in threat.DMs:
                row += Threat.entry(data.get(dm))
            tbody += row
            tgroup += tbody
            table += tgroup
            wrap += table
            return wrap

    threat_card = {
        'adversarial-model': (std_item.__func__, 'Adversarial Model'),
        'security-goal': (std_item.__func__, 'Security Goal'),
        'deployment-model': (risk_item.__func__, 'Deployment Model'),
        'unmitigated-impact': (risk_item.__func__, 'Unmitigated Impact'),
        'unmitigated-likelihood': (risk_item.__func__, 'Unmitigated Likelihood'),
        'unmitigated-risk': (risk_item.__func__, 'Unmitigated Risk'),
        'mitigations': (std_item.__func__, 'Mitigating Actions'),
        'residual-impact': (risk_item.__func__, 'Residual Impact'),
        'residual-likelihood': (risk_item.__func__, 'Residual Likelihood'),
        'residual-risk': (risk_item.__func__, 'Residual Risk'),
    }

    @staticmethod
    def description(threat):
        data = threat.data.get('description')
        if not data:
            return []

        data[0].insert(0, nodes.inline(text = 'Description: ', classes=['sralabel']))
        return data

    @staticmethod
    def item(threat, id):
        if not id in threat.data:
            return []

        f, label = Threat.threat_card[id]
        t_node = nodes.term(label)
        t_node += nodes.paragraph(label, '', nodes.inline(label, label, classes=['sralabel']))
        d_node = nodes.definition()
        d_node += f(threat, threat.data[id])
        return nodes.definition_list_item('', t_node, d_node)

    def threat_section(self, title):
        # Prepare a section
        section = nodes.section(raw_source='\n'.join(self.content))
        section.document = self.state.document

        # Add title, and provide a link target
        textnodes, messages = self.state.inline_text(title, self.lineno)
        title_node = nodes.title(title, '', *textnodes)
        section['names'].append(nodes.fully_normalize_name(title_node.astext()))
        section += title_node
        section += messages
        self.state.document.note_implicit_target(section, section)
        return section

    def run(self):
        title = []
        if 'id' in self.options:
            title.append(':threat:`{}`'.format(self.options['id']))
        if self.arguments:
            title.append(self.arguments[0])
        section = self.threat_section(': '.join(title))

        threat = ThreatData(self.options.get('deployment-models',[]))

        original_domain = self.env.temp_data['default_domain']
        try:
            self.env.temp_data['default_domain'] = ThreatDomain(self.env, threat)
            self.state.nested_parse(self.content, self.content_offset, section)
        finally:
            self.env.temp_data['default_domain'] = original_domain

        self.finalize_DMs(threat)

        # Process the threat definition
        section += self.description(threat)

        deflist = nodes.definition_list()
        for id in self.threat_card:
            deflist += self.item(threat, id)
        if deflist.children:
            section.append(nodes.container(nodes_text(deflist), classes = ['threat'], *[deflist]))

        # Finished:  discard ref to content, which is now inside the node tree
        threat.data = None
        return [section]

class C_SubItem(SphinxDirective):
    """Common base class for subitems.

    A subitem is part of a list of descriptions that apply to a specific
    aspect of an object, for example the return values or the parameters
    of a function.

    The definition of the item must immediately follow on the directive line.
    The description, if provided, can start on the next line if desired.
    """

    has_content = True
    final_argument_whitespace = True

    @property
    def desc_data(self):
        return self.env.domaindata['psa_description']['local']

    @property
    def subitem(self):
        return self.name.split(':')[-1]

    def parse_spec(self, spec):
        """Parse the argument for a list sub-item directive.

        Return the item list, spec and head nodes for the definition.
        """
        raise NotImplementedError

    def add_to_index(self, spec, node):
        pass

    def parse_content(self):
        """Parse a subitem and return the description nodes.
        """
        container = nodes.Element()
        self.state.nested_parse(self.content, self.content_offset, container)
        return container.children

    def check_content(self, arg_required=True):
        """ Check that content has been provided, and that the definition is
        present on the directive line.
        Trim the definition from the content ViewList and return the definition
        text.
        """
        self.assert_has_content()
        if self.lineno == self.content_offset+1:
            line1 = self.content[0].strip()
            self.content.trim_start()
            return line1
        if arg_required:
            raise self.warning('Argument missing for directive "{}"'.format(self.subitem))
        return None

    def check_subitem_permitted(self):
        """
        Check that a sub-item is permitted for the API element types

        The subitem must provide a set ``valid_in`` of items that it can be used in
        """
        if self.desc_data.element.objtype not in self.valid_in:
            raise self.warning('Directive "{}" not valid for API element "{}"'.format(
                            self.subitem, self.desc_data.element.objtype))

    def add_target_and_index(self, name, target):
        self.desc_data.element.add_target_and_index(name, target)

    def run(self):
        """Default run method for list sub-item directives.

        Stash the parsed definition list item in the appropriate list.
        """
        self.check_subitem_permitted()
        list, spec, head_nodes = self.parse_spec(self.check_content())

        term_node = nodes.term(nodes_text(head_nodes))
        term_node += nodes.paragraph(nodes_text(head_nodes), '', *head_nodes)
        description = nodes.definition(self.block_text)
        self.state.nested_parse(self.content, self.content_offset, description)
        defn = nodes.definition_list_item(description.astext(),
                                          term_node, description, spec=spec)
        self.add_to_index(spec, defn)
        list.append(defn)
        return []

class C_Summary(C_SubItem):

    def run(self):
        self.assert_has_content()
        if self.desc_data.summary:
            raise self.warning('Directive "{}" already provided.'.format(self.subitem))
        self.desc_data.summary = self.parse_content()
        return []

class C_Subsection(C_SubItem):
    option_spec = { 'top': directives.flag }

    def run(self):
        title, content = self.state.inline_text(self.check_content(), self.lineno)
        content += self.parse_content()
        if 'top' in self.options:
            self.desc_data.top_sections.append((title, content))
        else:
            self.desc_data.bottom_sections.append((title, content))
        return []

class C_Output(C_SubItem):
    valid_in = {'function'}

    def parse_spec(self, spec):
        return self.desc_data.outputs, spec, [nodes.literal(text=spec)]

class C_Param(C_SubItem):
    valid_in = {'function','macro'}

    def parse_spec(self, spec):
        name = c_name_from_prototype(spec)
        return self.desc_data.parameters, spec, [nodes.literal(text=name)]

class C_Return(C_SubItem):
    valid_in = {'function','macro'}

    def run(self):
        self.check_subitem_permitted()
        if self.desc_data.returns:
            raise self.warning('Directive "{}" already provided.'.format(self.subitem))
        self.desc_data.return_type = self.check_content(False)
        self.desc_data.returns = self.parse_content()
        return []

class C_Retval(C_SubItem):
    valid_in = {'function','macro'}

    def parse_spec(self, spec):
        return self.desc_data.retvals, spec, autolinking_literal(spec)

class C_Field(C_SubItem):
    valid_in = {'struct'}

    def parse_spec(self, spec):
        name = c_name_from_prototype(spec)
        return self.desc_data.fields, spec, [nodes.literal(text=name)]

class C_EnumValue(C_SubItem):
    valid_in = {'enum'}
    add_to_index = True

    def parse_spec(self, spec):
        return self.desc_data.values, spec, [nodes.literal(text=spec)]

    def add_to_index(self, spec, node):
        m = re.match(r'(\w+)(?:\s*=\s*(.+?))?\Z', spec)
        self.add_target_and_index(m.group(1), node)

class DescriptionData:
    def __init__(self, element):

        # The API element object
        self.element = element

        # A summary of the API element
        self.summary = None

        # A list of member specifications and definitions for an enum element
        self.values = []
        # A list of member specifications and definitions for a struct element
        self.fields = []
        # A list of parameter specifications and definitions for an API element
        self.parameters = []
        # A list of output specifications and definitions for an API element
        self.outputs = []

        # A description of the return value for an API element
        self.returns = []
        # The return type for a function or typedef API element
        self.return_type = None
        # A list of return values and descriptions for an API element
        self.retvals = []

        # Lists of additional description sections
        self.top_sections = []
        self.bottom_sections = []

    @staticmethod
    def make_subtitle(title):
        if isinstance(title, str):
            return nodes.rubric(title, text=title)
        else:
            t = nodes.rubric(rawsource = nodes_text(title))
            t += title
            return t

    @staticmethod
    def make_list(elements):
        if not elements:
            return []
        deflist = nodes.container(nodes_text(elements), classes = ['apisubitem'])
        deflist += nodes.definition_list(nodes_text(elements), *elements)
        return [deflist]

    def finish_list(self, title_text, elements):
        if not elements:
            return []
        title = self.make_subtitle(title_text)
        return [title] + self.make_list(elements)

    def sort_retvals(self):
        # Use the configured ordering to prioritise the return values:
        #   1. Return values that aren't a single identifier and aren't
        #      listed in the configuration.
        #   2. Return values that are listed in the configuration.
        #   3. Single identifiers that aren't listed in the configuration.
        #
        # If there is no configured order then output the list in source order
        #
        # data.retvals[] is a list of (node,value) tuples
        # return a list of nodes

        retvals = self.retvals
        if not retvals:
            return []

        order = self.element.env.app.config.psa_api_retval_order
        if order:
            priority = dict(zip(order,range(len(order))))
            def get_priority(spec):
                p = priority.get(spec, -1)
                if p < 0 and re.match(r'^[A-Z][A-Z0-9_]*$', spec):
                    p = len(order)
                return p
            retvals.sort(key=lambda rv: get_priority(rv['spec']))

        # extract and return the list of sorted nodes
        return retvals

    def finish_returns(self):
        if not self.return_type and not self.retvals and not self.returns:
            return []
        title = self.make_subtitle('Returns')
        if self.return_type:
            title += nodes.Text(': ')
            title += autolinking_literal(self.return_type)
        stuff = [title] + self.returns
        stuff += self.make_list(self.sort_retvals())
        return stuff

    def finish_subsections(self, sections):
        stuff = []
        if sections:
            for (title, subsection) in sections:
                stuff += [self.make_subtitle(title)]
                stuff += subsection
        return stuff

    def finish_top(self):
        top = self.finish_list('Fields', self.fields)
        top += self.finish_list('Values', self.values)
        top += self.finish_list('Parameters', self.parameters)
        top += self.finish_list('Outputs', self.outputs)
        top += self.finish_returns()
        top += self.finish_subsections(self.top_sections)
        return top

    def finish_bottom(self):
        return self.finish_subsections(self.bottom_sections)

class DescriptionDomain(sphinx.domains.Domain):
    """Description of an object in a PSA API document."""
    name = 'psa_description'
    label = 'PSA description'
    directives = {
        'field': C_Field,
        'output': C_Output,
        'param': C_Param,
        'return': C_Return,
        'retval': C_Retval,
        'value': C_EnumValue,
        'summary': C_Summary,
        'subsection': C_Subsection,
    }
    directives.update(psa_directives)
    roles = psa_roles

    def __init__(self, env, element=None):
        super().__init__(env)
        self.data['local'] = DescriptionData(element)

def make_c_target(name):
    return 'c.' + name

class C_Item(SphinxDirective):
    """Base class for PSA C API objects.

    Subclasses must define:

    * A field ``kind`` that provides the typeset description of what kind
      of object this is (e.g. type, struct, function, ...).
    * A method ``prototype(name, desc_data)`` that returns a pair of
      (str, node.Element) for the item, used between the header and the content.
    """
    has_content = True
    required_arguments = 0
    optional_arguments = 1
    option_spec = {
        'name': directives.unchanged,
        'header': directives.unchanged,
        'definition': directives.unchanged,
        'naked': directives.flag,
        'guard': directives.unchanged,
        'comment': directives.unchanged,
    }
    final_argument_whitespace = True
    naked = False

    @staticmethod
    def doxy_brief(nodes):
        if not nodes:
            return ''
        s = nodes_text(nodes).replace('\n',' ')
        fin = re.search(r'(\D\.\D|\d\.\D|\D\.\d)', s)    # . but not x.y
        if fin:
            s = s[:fin.start() + 2]
        return s

    @staticmethod
    def doxy_para(intro, text, indent=3):
        if text:
            return textwrap.wrap(text, 80 - indent,
                                 initial_indent=intro + ' ',
                                 subsequent_indent=' '*(len(intro)+1))
        return [intro]

    @staticmethod
    def doxy_comment(lines):
        if lines:
            c = '/**'
            for l in lines:
                c += '\n *'
                if l:
                    c += ' ' + l
            return c + '\n */\n'
        return ''

    def doxy_summary(self):
        if not self.desc_data.summary:
            return []
        return self.doxy_para('@brief',self.doxy_brief(self.desc_data.summary))

    def basic_prototype(self):
        # Return an unannotated prototype for the API element
        raise NotImplementedError

    def prototype(self, doxygen):
        # Return an annotated prototype for the API element
        # Default implementation prefixes an undecorated prototype with
        # a summary of the API element.
        # Override if more complex annotation is required

        proto, error = self.basic_prototype()
        # Document the basic summary, input parameters, and return values
        if doxygen>0:
            doxy = self.doxy_summary()
            if doxy and doxygen==2:
                # Document parameters of the API
                if self.desc_data.parameters:
                    items = [('@param ' + nodes_text(p[0]).strip(), self.doxy_brief(p[1]))
                                for p in self.desc_data.parameters]
                    width = max([len(i[0]) for i in items])
                    doxy.append('')
                    for i in items:
                        doxy.extend(self.doxy_para(i[0].ljust(width),i[1]))
                if self.desc_data.returns:
                    doxy.append('')
                    doxy.extend(self.doxy_para('@return',self.doxy_brief(self.desc_data.returns)))
            if doxy:
                proto = '\n' + self.doxy_comment(doxy) + proto
        if self.guard:
            proto = '#ifndef {}\n{}\n#endif'.format(self.guard, proto)
        if self.comment:
            lines = textwrap.wrap(self.comment, 80 - 3,
                initial_indent='/* ', subsequent_indent='   ')
            proto = '\n'.join(lines) + '\n */\n' + proto
        return proto, error

    def annotate_members(self, items, item_sep, final_sep, doxygen):
        # Annotated prototypes for structs and enums, documenting each member
        begin = '{} {} {{'.format(self.kind, self.item_name)
        if 'type' in self.options:
            begin = 'typedef ' + begin
            end = '}} {};'.format(self.item_name)
        else:
            end = '};'
        if not items:
            proto = begin + end
        else:
            subitems = []
            for item in items:
                brief = self.doxy_brief(item[1]) if doxygen==2 else ''
                if brief:
                    brief = '\n    /// '.join([''] + self.doxy_para('@brief', brief, 8))
                subitems.append(brief + '\n    ' + item['spec'])
            proto = begin + item_sep.join(subitems) + final_sep + '\n' + end

        if doxygen>0:
            doxy = self.doxy_comment(self.doxy_summary())
            if doxy:
                return '\n' + doxy + proto, None
        return proto, None

    def parse_arguments(self):
        # Either the API name must be the first line of argument, or it is
        # provided using the `name` option.
        # A macro definition can be provided as the subsequent lines of the
        # directive argument body (before options), or using the `definition`
        # option.
        # The header for the API element can be specified as the `header`
        # option, if not provided the current header in force for the source
        # file will be used.
        # Specific API element classes can extend this if required
        argument_trail = None
        if 'name' in self.options:
            self.item_name = self.options['name']
            if self.arguments:
                argument_trail = self.arguments[0]
        elif self.arguments:
            line_end = self.arguments[0].find('\n')
            if line_end <= 0:
                self.item_name = self.arguments[0]
            else:
                self.item_name = self.arguments[0][:line_end].rstrip()
                argument_trail = self.arguments[0][line_end+1:]
        else:
            raise self.warning('API name missing for directive "{}"'.format(self.objtype))
        self.definition = self.options.get('definition', argument_trail)
        self.header = self.options.get('header')
        if 'naked' in self.options:
            self.naked = True
        self.guard = self.options.get('guard')
        self.comment = self.options.get('comment')
        if self.naked and self.definition:
            raise self.warning('Naked API element "{}" cannot have a definition'.format(self.item_name))
        if self.naked and self.header:
            raise self.warning('Naked API element "{}" cannot be added to a header'.format(self.item_name))

    def add_target_and_index(self, name, node):
        # for C API items we add a prefix since names are usually not qualified
        # by a module name and so easily clash with e.g. section titles
        targetname = make_c_target(name)
        node['names'].append(targetname)
        if targetname not in self.state.document.ids:
            # If unique, use the exact target name as the id, not the output
            # from nodes.make_id(). This maintains anchor name compatibility
            node['ids'].append(targetname)
        node['first'] = False
        self.state.document.note_explicit_target(node)
        inv = self.env.domaindata['psa_c']['elements']
        if name in inv:
            self.state_machine.reporter.warning(
                'duplicate API definition of {}, other instance in {}.'.format(
                    name, self.env.doc2path(inv[name][0])), line=self.lineno)
        inv[name] = (self.env.docname, self.objtype)

    def make_prototypes(self):
        protos = []
        if not self.naked:
            # construct the standard and annotated prototypes
            proto, err = self.prototype(0)
            header_proto, _ = self.prototype(self.config.psa_api_header_doxygen)
            if err:
                protos.append(err)
            protos.append(autolinking_literal_block(proto))
            # Add the prototype to the list of API prototypes
            self.env.domains['psa_c'].add_prototype(self.item_name, self.header, self.kind,
                                                    proto, header_proto, self.env.docname)
        return protos

    def run(self):
        env = self.env

        self.kind = self.objtype = self.name.split(':')[-1]

        self.parse_arguments()
        # Prepare a section
        section = nodes.section(raw_source=self.content)
        section.document = self.state.document
        section['objtype'] = self.objtype

        title_node = nodes.title()
        title_node += nodes.literal(text=self.item_name)
        title_node += nodes.Text(' ({})'.format(self.kind))
        section += title_node

        self.add_target_and_index(self.item_name, section)

        original_default_domain = env.temp_data['default_domain']
        section_offset = len(section)
        desc_domain = DescriptionDomain(env, self)
        self.desc_data = desc_domain.data['local']
        description = nodes.Element()
        try:
            env.temp_data['default_domain'] = desc_domain
            self.state.nested_parse(self.content, self.content_offset, description)
        finally:
            env.temp_data['default_domain'] = original_default_domain
        # Insert optional summary paragraph immediately after the title
        if self.desc_data.summary:
            section += self.desc_data.summary
        # Add prototype
        section += self.make_prototypes()
        # Compile upper description sections
        top = self.desc_data.finish_top()
        section += top
        # Add description
        if description.children:
            if top:
                section.append(self.desc_data.make_subtitle('Description'))
            section += description.children
        # Add final subsections
        section += self.desc_data.finish_bottom()

        # Finished
        del desc_domain.data['local']
        return [section]

class Attribute(C_Item):
    naked = True

class Typedef(C_Item):
    def parse_arguments(self):
        super().parse_arguments()
        if re.match(r'\w+\Z', self.item_name):
            self.definition = None
        else:
            self.definition = self.item_name
            self.item_name = c_name_from_prototype(self.definition)

    def basic_prototype(self):
        proto = self.definition
        if proto is None:
            proto = '/*...*/ ' + self.item_name
        if not proto.startswith('typedef '):
            proto = 'typedef ' + proto
        if not proto.endswith(';'):
            proto += ';'
        return proto, None

class Macro(C_Item):
    version_opt = 'api-version'
    option_spec = C_Item.option_spec
    option_spec.update({ version_opt: option_choice(version_opt, ('major','minor','hex')) })
    formats = {
        'major': '{0}',
        'minor': '{1}',
        'hex'  : '(0x{0:02X}{1:02X}u)'
    }

    def basic_prototype(self):
        error = None
        definition = self.definition
        if self.version_opt in self.options:
            if definition:
                error = self.state_machine.reporter.error(
                'Cannot provide definition for version macro "{}"'.format(self.item_name),
                line=self.lineno)
            elif self.desc_data.parameters:
                error = self.state_machine.reporter.error(
                'Cannot provide arguments for version macro "{}"'.format(self.item_name),
                line=self.lineno)
            version = self.env.config.psa_api_api_version
            v = [int(x) for x in version.split('.')]
            if len(v) == 1:
                v.append(0)
            definition = self.formats[self.options[self.version_opt]].format(*v)
        elif definition is None:
            definition = '/*...*/'

        if self.desc_data.parameters:
            params = [p['spec'] for p in self.desc_data.parameters]
            args = '(' + ', '.join(params) + ')'
        else:
            args = ''
        # Very crude heuristic for line splitting. Needs work.
        if len(self.item_name) + len(args) + len(definition) > 70:
            proto = '#define {}{} \\\n    {}'.format(self.item_name, args, definition)
        else:
            proto = '#define {}{} {}'.format(self.item_name, args, definition)
        return proto, error

class Function(C_Item):
    option_spec = C_Item.option_spec
    option_spec.update({ 'type': directives.flag }) # for function pointer typedefs

    def parse_arguments(self):
        super().parse_arguments()
        if 'type' in self.options:
            self.kind = 'type'

    def basic_prototype(self):
        if self.desc_data.return_type:
            proto = self.desc_data.return_type
            error = None
        else:
            proto = 'void'
            # Issue a non-fatal error if no return type is specified
            error = self.state_machine.reporter.warning(
                'No return type for function "{}", assuming void.'.format(self.item_name),
                line=self.lineno)

        if 'type' in self.options:
            proto = 'typedef ' + proto + ' (* ' + self.item_name + ')('
            self.kind = 'type'
        else:
            proto += ' ' + self.item_name + '('

        if self.desc_data.parameters:
            sep = ',\n' + ' ' * len(proto)
            params = [p['spec'] for p in self.desc_data.parameters]
            proto += sep.join(params)
        else:
            proto += 'void'
        proto += ');'

        return proto, error

class Struct(C_Item):
    option_spec = C_Item.option_spec
    option_spec.update({ 'type': directives.flag }) # for struct typedefs

    def prototype(self, doxygen):
        return self.annotate_members(self.desc_data.fields, ';', ';', doxygen)

class Enum(C_Item):
    option_spec = C_Item.option_spec
    option_spec.update({ 'type': directives.flag }) # for enum typedefs

    def prototype(self, doxygen):
        return self.annotate_members(self.desc_data.values, ',', '', doxygen)


class PSA_C_Index(sphinx.domains.Index):
    name = 'identifiers'
    localname = 'Index of API elements'
    shortname = 'API identifiers'

    @staticmethod
    def entry_from_item(item):
        return (item['name'], 0, item['docname'], item['target'], '', '', '')

    @staticmethod
    def split_buckets(buckets, max_bucket_size):
        """Split a bucket dictionary into smaller buckets.

        ``buckets`` is a dictionary whose keys are strings and whose values
        are lists that satisfy the property that for each element ``entry`` in
        ``buckets[key]``, ``key`` is a prefix of ``entry[0].upper()``.

        This function looks for keys whose value is a list of more than
        ``max_bucket_size`` elements and splits the corresponding list by
        moving each element to a key with a longer prefix.
        """
        did_something = True
        while did_something:
            did_something = False
            for key, entries in list(buckets.items()):
                if len(buckets[key]) <= max_bucket_size:
                    continue
                del buckets[key]
                did_something = True
                length = len(key) + 1
                for entry in entries:
                    new_key = entry[0].upper()[:length]
                    buckets.setdefault(new_key, []).append(entry)

    def collect_buckets(self):
        entries = []
        for name, (doc, _) in list(self.domain.data['elements'].items()):
            entries.append( (name, 0, doc, make_c_target(name), '', '', '') )
        if not entries:
            return {}

        buckets = {'': entries}
        if len(entries) >= 10:
            self.split_buckets(buckets, len(entries) / 2)
        return buckets

    def generate(self, docnames=None):
        buckets = self.collect_buckets()
        if not buckets:
            return [], False
        content = [(bucket, sorted(buckets[bucket]))
                   for bucket in sorted(buckets.keys())]
        return content, False

def option_list(option):
    def option_check(argument):
        if isinstance(argument, str):
            return argument.replace(',',' ').split()
        raise ValueError(':{}: must be a list of headers.'.format(option))

    return option_check

class Header(SphinxDirective):
    required_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        'seq': directives.nonnegative_int,
        'guard': directives.unchanged,
        'include': option_list('include'),
        'system-include': option_list('system-include'),
        'c++': directives.flag,
        'copyright': directives.unchanged,
        'license': directives.unchanged,
    }
    has_content = True

    def run(self):
        header = self.arguments[0].strip()
        self.env.temp_data['header_file'] = header
        self.env.temp_data['header_seq'] = self.options.pop('seq',0)
        opt = self.options.copy()
        if 'guard' in opt and not opt['guard']:
            opt['guard'] = re.sub('[^a-zA-Z0-9]','_',header.upper()) + '_H'
        opt['preamble'] = '\n'.join(self.content.data)

        if self.content.data or len(self.options) > 0:
            collision = self.env.domains['psa_c'].add_header(header, opt)
            if collision:
                return [self.state_machine.reporter.error(
                    'Duplicate header "{}" defined in source "{}".'.format(
                        header, collision), line = self.lineno)]

        return []

class header_node(nodes.General, nodes.Element):
    pass

class InsertHeader(SphinxDirective):
    required_arguments = 1
    final_argument_whitespace = True

    def run(self):
        # The generated header will depend on any source files with API elements
        # in the header, and a source file that defines any adornments for the
        # header. Tracking these does not ensure correct detection of every
        # situation where this directive to be re-processed. The simplest
        # reliable approach is to force this source file to always be re-read.
        self.env.note_reread()
        return [header_node('', header=self.arguments[0].strip())]

def process_header_nodes(app, doctree, docname):
    for node in doctree.traverse(header_node):
        header = node['header']
        text = app.builder.env.domains['psa_c'].prototype_header(header, notice=False, doxy=False, db=False)
        if not text:
            logger.warning('Cannot insert header with no content: "%s"',
                header, location=node)
            inline_error = nodes.inline('', 'Header "{}" has no content'.format(header),
                classes = ['issue'])
            node.replace_self(inline_error)
        else:
            h = autolinking_literal_block(text)
            node.replace_self(resolve_references(h, docname, app))

ApiElement = namedtuple('ApiElement', 'seq name type prototype annotated')

class PSA_C_Domain(sphinx.domains.Domain):
    """C language domain for PSA."""
    name = 'psa_c'
    label = 'PSA C'
    directives = {
        'enum': Enum,
        'function': Function,
        'macro': Macro,
        'struct': Struct,
        'typedef': Typedef,
        'attribute': Attribute,
        'header': Header,
        'insert-header': InsertHeader,
        'template-image': TemplateImage,
        'title': TitlePage,
        'front-matter': FrontMatter,
        'maintoc': MainToc,
        'appendix': Appendix,
        'about': About,
        'insert-section': InsertFrontSection,
        'banner': Banner,
        'insert-banner': InsertBanner,
        'include-license': IncludeLicense,
        'release': Release,
        'release-table': ReleaseTable,
        'reference': Reference,
        'reference-table': ReferenceTable,
        'term': Term,
        'scterm': Term,
        'abbr': Term,
        'term-table': TermTable,
        'threat': Threat,
    }
    directives.update(psa_directives)
    roles = psa_roles
    indices = [PSA_C_Index]
    initial_data = {
        'prototypes': {},   # header  -> { docname: [ApiElement] }
        'elements': {},     # name    -> docname, objtype
        'headers': {},      # header  -> docname, options
        'front-matter': {}, # section -> docname, content
        'cite': {},         # citeref -> docname, reference
        'term': {},         # term    -> docname, definition
        'release': {},      # seq_id  -> docname, release
        'sra': {},          # sra.id  -> docname, None
    }

    def clear_doc(self, docname):
        for set in self.initial_data.keys():
            d = self.data[set]
            if set == 'prototypes':
                for header, elements in list(d.items()):
                    if docname in elements:
                        del elements[docname]
                        if not elements:
                            del d[header]
            else:
                for key, (fn, _) in list(d.items()):
                    if fn == docname:
                        del d[key]

    def merge_domaindata(self, docnames, otherdata):
        for set in ('elements','headers','front-matter'):
            d = self.data[set]
            for key, (fn, data) in d.items():
                if fn in docnames:
                    d[key] = (fn, data)

        #if 'prototypes' in otherdata:
        #    self.data['prototypes'].update(otherdata['prototypes'])

    def get_objects(self):
        for refname, (docname, type) in list(self.data['elements'].items()):
            yield (refname, refname, type, docname, make_c_target(refname), 1)

    def add_prototype(self, name, header, type, prototype, annotated, docname):
        if header is None:
            header = self.env.temp_data.get('header_file',
                                    self.env.config.psa_api_c_header)
        seq = self.env.temp_data.get('header_seq',0)
        sig = ApiElement(seq, name, type, prototype, annotated)
        self.data['prototypes'].setdefault(header,{}).setdefault(docname,[]).append(sig)

    def sequenced_prototypes(self, header):
        # Order the prototypes according to source sequence numbers.
        # Sphinx partial rebuild results in variation in the order of
        # the prototypes in the primary list.
        #
        p = sum(self.data['prototypes'].get(header,{}).values(),[])
        p.sort(key = attrgetter('seq'))
        return p

    def sorted_prototypes(self, header):
        # Sort the prototypes to enable accurate diffing of the API between
        # versions of the documentation source code. Sphinx partial rebuild
        # results in variation in the order of the prototypes in the primary
        # list.
        #
        # The current ordering is by:
        # - type, reversed so typedef, then macro, then function
        # - API name
        #
        p = sum(self.data['prototypes'].get(header,{}).values(),[])
        p.sort(key = attrgetter('name'))
        p.sort(key = attrgetter('type'), reverse = True)
        return p

    def add_header(self, header, options):
        # stash the extra header data
        headers = self.data['headers']
        if header in headers:
            return headers[header][0]
        headers[header] = (self.env.docname, options)
        return None

    def get_header_options(self, header):
        h = self.data['headers'].get(header)
        return h[1] if h else {}

    def prototype_header(self, header, notice, doxy, db):
        # Output a string with the content for a specific header file
        # If the header has no content then return None
        if not header in self.data['prototypes']:
            return None

        h = self.get_header_options(header)
        lines = []
        postamble = []
        if notice:
            # REUSE-IgnoreStart
            if 'copyright' in h:
                lines.append('// SPDX-FileCopyrightText: {}'.format(h['copyright']))
            if 'license' in h:
                lines.append('// SPDX-License-Identifier: {}'.format(h['license']))
            # REUSE-IgnoreEnd
            if lines:
                lines.append('')
        if not db:
            if 'preamble' in h:
                lines.extend([h['preamble'],''])
            guard = h.get('guard')
            if guard:
                lines.extend( ['#ifndef {}'.format(guard), '#define {}'.format(guard), ''] )
                postamble = ['','#endif // {}'.format(guard)]
            includes = h.get('system-include',[])
            if includes:
                for inc in includes:
                    lines.append('#include <{}>'.format(inc))
                lines.append('')
            includes = h.get('include',[])
            if includes:
                for inc in includes:
                    lines.append('#include "{}"'.format(inc))
                lines.append('')
            if 'c++' in h:
                lines.extend( ['#ifdef __cplusplus', 'extern "C" {', '#endif', ''] )
                postamble[0:0] = ['', '#ifdef __cplusplus', '}', '#endif']

        if db:
            apis = self.sorted_prototypes(header)
        else:
            apis = self.sequenced_prototypes(header)
        if doxy and not db:
            lines += [api.annotated for api in apis]
        else:
            lines += [api.prototype for api in apis]
        lines.extend(postamble)

        return '\n'.join(lines) + '\n'

    def output_prototypes(self, outdir, format):
        # Output the prototypes as C header files into the path at `outdir`
        # * for 'api-ref' include annotaton, and use source sequencing
        # * for 'api-db' strip annotations, and use identifier order
        db = (format == 'api-db')
        os.makedirs(outdir, exist_ok=True)
        clean_dir(outdir)
        for h in self.data['prototypes'].keys():
            fn = os.path.join(outdir, h + '.h')
            os.makedirs(os.path.dirname(fn), exist_ok=True)
            sig_file = open(fn, 'w', encoding='utf-8')
            sig_file.write(self.prototype_header(h, notice=True, doxy=True, db=db))


    def resolve_any_xref(self, env, fromdocname, builder, target,
                         node, contnode):
        # Check if this is a citation reference
        m = re.match(r'\[([a-zA-Z0-9][-a-zA-Z0-9_. ]+)\]',target)
        if m:
            refnode = self.resolve_citeref(env, fromdocname, builder, 'cite', m.group(1), contnode)
            if refnode:
                return [('psa_c:cite', refnode)]

        m = re.match(r'(DM|AM|SG|T|M)\.[-a-zA-Z0-9+_.]+',target)
        if m:
            refnode = self.resolve_sraref(env, fromdocname, builder, target, contnode)
            if refnode:
                return [('psa_c:' + m.group(1).lower(), refnode)]

        # strip trailing parens, and check if an API item
        refnode = self.resolve_apiref(env, fromdocname, builder, target, node, contnode)
        if refnode:
            return [('psa_c:ref', refnode)]
        return []

    # Weak reference: turn into a reference if the target is available,
    # and keep as-is otherwise.
    def resolve_xref(self, env, fromdocname, builder,
                     typ, target, node, contnode):
        type = typ.split(':')[-1]
        if type in ('cite', 'cite-title'):
            return self.resolve_citeref(env, fromdocname, builder, type, target, contnode)
        if type in ('term', 'scterm'):
            return self.resolve_termref(env, fromdocname, builder, type, target, contnode)
        if type == 'secref':
            return self.resolve_secref(env, fromdocname, builder, type, target, node, contnode)
        if type in ('numref', '*'):
            return self.resolve_numref(env, fromdocname, builder, type, target, node, contnode)
        if type in ('am', 'dm', 'sg', 't', 'm'):
            return self.resolve_sraref(env, fromdocname, builder,
                                       canonical_sra_id(type, target), contnode)

        return self.resolve_apiref(env, fromdocname, builder, target, node, contnode)

    # Try and resolve an API reference
    def resolve_apiref(self, env, fromdocname, builder,
                     target, node, contnode):
        # strip trailing parens
        target = target.rstrip('()')
        if target not in self.data['elements']:
            return None
        obj = self.data['elements'][target]
        return make_refnode(builder, fromdocname, obj[0], make_c_target(target),
                            contnode, target)

    def resolve_secref(self, env, fromdocname, builder,
                     typ, target, node, contnode):
        # resolve a section reference - use title for link text
        # and add a latex pageref
        # use the standard resolver to do the label lookup
        e = env.domains['std'].resolve_xref(env, fromdocname, builder,
                         'ref', target.lower(), node, contnode)
        if not e:
            contnode['classes'] = ['issue','secref']
        else:
            e[0]['classes'] = ['secref']
            if 'refuri' in e:
                id = e['refuri'][1:].replace('#', ':')
            else:
                id = fromdocname + ':' + e['refid']
            e += latexpageref(id)
        return e

    def resolve_numref(self, env, fromdocname, builder,
                     typ, target, node, contnode):
        # resolve a number reference
        e = env.domains['std'].resolve_xref(env, fromdocname, builder,
                         'numref', target.lower(), node, contnode)
        if not (e and isinstance(e, nodes.reference)):
            contnode['classes'] = ['issue','numref']
            return e

        # number_reference nodes are stripped in the latex writer
        # so need to return a composite node which includes the number_refernence
        # followed by the latex page-ref node
        e[0]['classes'] = ['numref']
        n = nodes.inline('')
        n += e
        if 'refuri' in e:
            id = e['refuri'][1:].replace('#', ':')
        else:
            id = fromdocname + ':' + e['refid']
        n += latexpageref(id)
        return n

    def resolve_citeref(self, env, fromdocname, builder,
                     typ, target, contnode):
        # resolve a citation reference (with optional title inclusion)
        target = canonical_rfc(target)
        docname, id, title = ReferenceTable.resolve_ref(env, target.lower())
        if not docname:
            # If the xref cannot resolve, Sphinx writes the contnode - so
            # make it show up as an issue in the output
            contnode['classes'].append('issue')
            return None
        contnode = nodes.inline('', nodes_text(contnode), classes = ['cite'])
        refnode = make_refnode(builder, fromdocname, docname, id, contnode)
        if typ == 'cite-title' and title:
            n = nodes.inline(nodes_text(contnode))
            n += nodes.emphasis('', *title)
            n += nodes.Text(' ')
            n += refnode
            return n
        return refnode

    def resolve_termref(self, env, fromdocname, builder,
                     typ, target, contnode):
        # resolve a term reference
        docname, label = TermTable.resolve_ref(env, target.lower())
        contnode['classes'].append(typ)
        if not docname:
            # If the xref cannot resolve, Sphinx writes the contnode - so
            # make it show up as an issue in the output
            contnode['classes'].append('issue')
            return None
        return make_refnode(builder, fromdocname, docname, label, contnode)

    def resolve_sraref(self, env, fromdocname, builder,
                       target, contnode):
        key = nodes.make_id(target)
        docname, label = SRADefinition.resolve_ref(env, key)
        if not docname:
            # If the xref cannot resolve, make it show up as an issue in the output
            contnode['classes'].extend(['issue'])
            return None

        n = nodes.inline(nodes_text(contnode), label, classes=['sraref'])
        return make_refnode(builder, fromdocname, docname, 'sra-' + key, n)

class BuildAPI(sphinx.builders.Builder):
    def get_outdated_docs(self):
        return 'api'

    def prepare_writing(self, docnames):
        return

    def get_target_uri(self, docname, typ = None):
        return docname

    # The API prototype output is independent of the structure of the source
    # documents. So no output is generated for each updated source doc, instead
    # the entire prototype API is output during the `finish()` method.
    def write_doc(self, docname, doctree):
        return

    def finish(self):
        self.env.domains['psa_c'].output_prototypes(self.app.outdir, self.name)

class API_db(BuildAPI):
    name = 'api-db'

class API_ref(BuildAPI):
    name = 'headers'

def alpha_section(secnum):
    if not secnum or len(secnum) == 0:
        return secnum

    secnum = list(secnum)
    return tuple([string.ascii_uppercase[secnum[0]-1]] + secnum[1:])

def rewrite_section_numbers(env):
    # This relies on the callbacks running _after_ the built-in
    # TocTreeCollector has run, rewriting the section numbers to
    # use Alphabetic section numbers for chapters in the Appendix

    def _walk_toc(node, alpha_sec=False):
        for subnode in node.children:
            if isinstance(subnode, (nodes.bullet_list, nodes.list_item, addnodes.only)):
                _walk_toc(subnode, alpha_sec)
            elif isinstance(subnode, addnodes.compact_paragraph) and alpha_sec:
                reference = cast(nodes.reference, subnode[0])
                reference['secnumber'] = alpha_section(reference['secnumber'])
            elif isinstance(subnode, addnodes.toctree):
                _walk_toctree(subnode)

    def _walk_toctree(toctreenode: addnodes.toctree, alpha_sec=False) -> None:
        for (_, ref) in toctreenode['entries']:
            if ref in env.tocs:
                _walk_toc(env.tocs[ref], alpha_sec)

                if alpha_sec:
                    env.toc_secnumbers[ref] = {k: alpha_section(v)
                        for k, v in env.toc_secnumbers[ref].items()}

    for docname in env.numbered_toctrees:
        doctree = env.get_doctree(docname)
        for toctreenode in doctree.traverse(addnodes.toctree):
            alpha_sec = toctreenode.get('alpha_numbers', False)
            _walk_toctree(toctreenode, alpha_sec)

    return []

def assign_figure_numbers(env):
    # Rassign a figure number to each figure under a numbered toctree.
    # But using the alpha section labels for appendices
    # Each doc either has alpha chapters or number chapters (maintoc vs appendix)
    # For each alpha-docs, accumulate the minimum fig index per fig-section
    # Then rewrite the fignumbers using an alpha chapter, and reducing the index as required.

    if env.config.numfig and env.config.numfig_secnum_depth > 0:
        alphadocs = []
        figoffset = {}
        for docname, secnums in env.toc_secnumbers.items():
            if type(secnums[""][0]) is not int and docname in env.toc_fignumbers:
                # This document is in an alpha-sectioned chapter
                alphadocs.append(docname)
                for kind, fignums in env.toc_fignumbers[docname].items():
                    offsets = figoffset.setdefault(kind,{})
                    for fignum in fignums.values():
                        offsets[fignum[:-1]] = min(offsets.get(fignum[:-1],999), fignum[-1] - 1)
        for docname in alphadocs:
            for kind, fignums in env.toc_fignumbers[docname].items():
                for id, fignum in fignums.items():
                    figbase = fignum[:-1]
                    fignums[id] = alpha_section(figbase) + (fignum[-1] - figoffset[kind][figbase],)

    return []

def apply_alpha_sections(app, env):
    return rewrite_section_numbers(env) + assign_figure_numbers(env)

def note_dependency(app, doctree):
    # Rebuild everything when this extension's code changes.
    app.env.note_dependency(__file__)

def process_doctree_resolved(app, doctree, docname):
    process_banner_nodes(app, doctree, docname)
    process_header_nodes(app, doctree, docname)

def setup(app):
    # This version of the extension depends on table support only added in v5.3
    app.require_sphinx('5.3')

    app.add_config_value('psa_api_c_header', 'prototypes', 'env')
    app.add_config_value('psa_api_tool_path', '', 'env')
    app.add_config_value('psa_api_template_path', '', 'env')
    app.add_config_value('psa_api_license', 'missing', 'env')
    app.add_config_value('psa_api_api_version', '', 'env')
    app.add_config_value('psa_api_retval_order', [], 'env')
    app.add_config_value('psa_api_header_doxygen', 0, 'env')
    app.add_config_value('psa_api_front_sections', [], 'env')
    # This should be triggered on 'config-inited', but that event doesn't
    # exist before Sphinx 1.8.
    app.add_node(banner)
    app.add_node(header_node)
    app.connect('doctree-read', note_dependency)
    app.connect('doctree-resolved', process_doctree_resolved)
    app.connect('env-get-updated', apply_alpha_sections, 600)

    app.add_domain(PSA_C_Domain)
    app.add_domain(DescriptionDomain)
    app.add_builder(API_db)
    app.add_builder(API_ref)
    FrontSection.add_directives(app)

    return {
        'version': '1.0',
        'env_version': 14,
        'parallel_read_safe': False,    # Can only verify this on Linux, not supported on MacOS or Windows
        'parallel_write_safe': True,
    }
