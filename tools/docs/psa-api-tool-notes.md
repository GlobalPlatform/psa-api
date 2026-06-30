<!--
SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited
SPDX-FileCopyrightText: Copyright 2026 GlobalPlatform
SPDX-License-Identifier: Apache-2.0
-->

# psa-api-tool Notes

This note is a lightweight contributor and editor guide for the `psa-api-tool` Sphinx
extension and build support.

It is intended to capture the practical behavior of the tool as used in PSA
specification sources. It is written for both human editors and automated editing
agents, so headings, directive names, role names, and examples should remain explicit
and easy to search.

This is not a full implementation reference. When behavior is unclear, verify against
`psa-api-tool.py`, `psa-api-conf.py`, the shared `make` file, and the template files.

## Audience and Scope

These notes describe:

- what each custom directive or role is for
- the important arguments and options
- where it can be used
- shared PSA API source conventions
- known limitations or gotchas
- minimal working examples

This note currently focuses on the parts of `psa-api-tool` that are needed to edit and
extend specification source text effectively, especially:

- API and manifest definition directives
- release metadata inputs
- terms/glossary definitions
- references and citation roles

It also records shared source conventions used across PSA API specifications. Individual
specification repositories can add narrower local rules where needed.

Broader setup and build instructions are in `using-psa-api-tool.md`. This includes
dependency notes, repository layout, configuring document metadata in `conf.py`, and
running common build targets.

## Guide Map

Use these sections as the usual entry points:

- `Shared Source Editing Conventions` for punctuation, wrapping, recommendation wording,
  graphics, API definition layout, generated headers, admonitions, and inline code
  style.
- `Directive Reference` for custom and modified directives, grouped by typical authoring
  context.
- `Role Reference` for custom roles and modified reference behavior.
- `Security Risk Assessment Directives and Roles` for SRA-specific structured threat
  content.

## Shared Source Editing Conventions

These conventions are intended for PSA API specifications that use `psa-api-tool`. They
should be treated as shared conventions unless a consuming repository explicitly says
otherwise.

### Plain source punctuation

Use plain ASCII single and double quote characters in specification sources. Sphinx
converts quotes and apostrophes to the appropriate typographic characters when rendering
HTML and LaTeX output, so source files should not use Unicode curly quotation marks such
as `‘`, `’`, `“`, or `”`.

Use ASCII source forms for dashes when typographic dashes are intended in the rendered
output. Sphinx converts `--` to an en-dash and `---` to an em-dash. Use these source
forms instead of Unicode dash characters. For example, use an em-dash for an explanatory
aside:

```rst
The value is private to the caller --- it cannot be read by another component.
```

Use normal ASCII hyphens for hyphenated terms and ASCII minus signs in code, numeric
ranges, and literal values.

### American English spelling

Use American English spelling in specification sources. For example, use `behavior`,
`initialize`, `optimization`, and `synchronization`.

### Source line wrapping and indentation

These conventions apply to all reStructuredText specification sources, including `.rst`
files and extensionless included source fragments such as `terms`, `references`, and
`releases`.

Use one logical paragraph per source line. Let the editor wrap long source lines for
display. This keeps the source structure aligned with the document structure, avoids
inconsistent wrapping in files edited with different window widths, and matches the
current style used in other PSA API specifications.

Keep block indentation consistent within a source file. Use three spaces for directive
bodies, nested directive content, list continuation text, and other indented
reStructuredText blocks.

Use the same three-space convention for `.. list-table::` directives. The directive
options and outer row list are indented by three spaces, nested cell list items are
indented by six spaces, and table cell continuation text is indented by nine spaces.

Use bullet-list markers with two spaces after the marker, for example `*  List item`.
This means the text starts after three source characters and aligns naturally with
continuation lines that use the three-space indentation convention. Enumerated list
markers already occupy three source characters when they use a single-digit number, an
alphabetic marker, or automatic numbering, for example `1. List item`, `a. List item`,
or `#. List item`.

For nested bullet lists, use `-` as the inner list marker under an outer `*` list.
Indent the nested marker by three spaces and keep the same two spaces after the marker,
for example `   -  Nested item`. The same marker convention is used in `.. list-table::`
row and cell lists.

When making semantic changes, do not reformat unrelated paragraphs or indentation in the
same patch. If a source file has mixed wrapping or indentation conventions, clean it up
in a separate mechanical formatting pass so that review can distinguish formatting churn
from specification changes.

### Recommendation wording

When documenting recommended behavior or design choices, use neutral specification
wording rather than organization-voiced wording.

Prefer:

- `It is recommended that ...`
- `The following behavior is recommended:`

Avoid:

- `<organization> recommends that ...`
- `We recommend that ...`

This keeps advisory specification text independent of a motivated speaker, while still
distinguishing recommendations from normative requirements such as `must`, `must not`,
`shall`, and `shall not`.

### Rendered graphics

Rendered graphics are normally checked in to specification repositories. This means that
editors who only modify text can build the documents without installing every graphics
rendering tool.

When modifying graphics sources, update the rendered assets at the same time. For
example, changes to `.svg` figures must keep the corresponding `.pdf` rendering up to
date for PDF builds. The `images` make target regenerates graphics derived from `.json`,
`.puml`, and `.svg` sources when the required tools are available.

### C API element definitions

The `.. macro::`, `.. typedef::`, `.. struct::`, `.. function::`, and `.. enum::`
directives are top-level directives that define C API elements. This section describes
common aspects of these directives, and best practices for using them.

Details for each directive are provided in the Directive Reference.

#### Rendering

In the current tooling, each API element directive forms a section node within the
document model, and the API element name forms the title for the section. As a result,
an API element defined within a heading level 3 section will be a level 4 section, an
API element defined within a heading level 2 section will be a level 3 section. The
effect is that the heading format for an API element depends on the enclosing section
level.

For consistency in a specification, it is important to have every API element defined at
the same level in the document hierarchy. Existing PSA API specifications commonly use
level 4, the first level that does not include a section number in the heading. This
occasionally requires creative chapter structuring so that this is achieved.

#### Standard options

These options are available for all C API element directives, but are used rarely, only
when required for the individual API element.

- `:header:` (optional) - see the header file section below for details.
- `:guard:` (optional) - this adds an optional #ifdef guard around the API element
  definition in the header file.
- `:comment:` (optional) - specify a comment text to include in the generated header
  before the API element definition.
- `:naked:` (optional) - do not include this API element in any header file.

#### Typical layout of element definitions

After the element directive and any options, which can include a definition for macros,
the following is a recommended ordering for API definitions:

- Summary directive.
- Sub-element directives, such as parameters for a function or macro-like function;
  fields for a structure; or values for an enumeration.
- Return directive.
- Return value directives for a function or macro-like function.
- Description, as text within the element directive.

If a specification or a set of related API elements has shared subsections in the
description, add them as `.. subsection::` directives. Place them consistently in the
source to aid maintenance. Examples include the 'Key format' sections in the Crypto API
key type definitions and the 'Compatible key types' sections in the Crypto API algorithm
definitions.

When rendered, the tool fixes the ordering of the API definition (to match the list
above), then any subsections labeled `:top:`, then the general description, and finally
any subsections not labeled `:top:`. The recommended layout here matches that to
facilitate easier editing.

Optional: it is possible to specify a document-wide ordering of return values (typically
for error codes) in the `doc_info` dictionary, to force consistency in the output
without demanding the same in the source material. This is used for the larger API
specifications, such as the Crypto API.

#### Using `summary` directives

Each C API element directive should have a `.. summary::` directive with a short
description of the API. The first sentence of the summary is used when generating
Doxygen-enhanced reference header file output with the `headers` make format.

#### Placement of `.. versionadded::` directives

After testing, the best placement for these in API element descriptions is at the end of
the `summary` directive. For example:

```rst
.. macro:: EXAMPLE

   .. summary:: An example macro definition.

      .. versionadded:: 2.3

   More content.
```

The same is true for the `.. versionchanged::`, `.. deprecated::`, and `..
versionremoved::` directives.

Shared PSA API convention for API definitions:

- Use `.. versionadded::` only for new API elements.
- Use `.. versionchanged::` for existing API elements whose behavior, constraints, or
  meaning changed materially.
- Use `.. deprecated::` for existing API elements that remain specified but are
  deprecated.
- Omit the directive body when the marker is self-explanatory.
- If a body is useful, use at most one short sentence.
- Keep rationale, migration guidance, and fuller explanation in the normal API prose
  rather than in the version directive body.
- Do not use version markup just because a version-valued macro or function now reports
  the newer document version. Reserve it for API evolution that a reader needs called
  out explicitly.

### Generated header files

The tooling builds API element definitions from the directive-based descriptions in the
source specification. This ensures that element definitions are always consistent
between the detailed description, the API signature, and the content of any generated
embedded or external header file.

#### Associating an API element with a named header file

The tooling supports different types of specification project.

- If a `header` attribute is specified in the `conf.py` `doc_info` dictionary, this is
  the default header for an API element.

- If a documentation source file uses a `.. header::` directive, this defines the header
  to associate with any element definitions that follow it. This directive can be used
  multiple times in a single source file for different headers.

- A single instance of the `.. header::` directive for each header file can also be used
  to specify additional information about a header file, such as inclusion of a license
  or copyright preamble and header guard inclusion. See the description of the `..
  header::` directive for details.

- If an API element uses the `:header:` option, this overrides any default or directive
  setting.

Guidance:

- For a single header project, use the `doc_info['header']` attribute.
- For a multi-header project, use the `.. header::` directive as required.
- For the odd API element that lives in a different file, use the `:header:` option.

#### Use of generated headers

Generated headers are used in three ways:

1. Included inline in the specification output. This uses the `.. insert-header::`
   directive, and produces an in-sequence, unannotated, cross-referenced source listing
   of the canonical header.

2. Output using the `api-db` build format, which is used by the `api-db`, `api-diff`,
   and `api-update` make formats to create, review, and maintain a baseline API
   definition to enable checks that changes to the specification sources only introduce
   expected changes to the API. This format has the elements alphabetically sorted and
   unannotated.

   By default, the `api-*` make targets use or update a set of headers in the document's
   `api.db/` folder.

3. Output using the `headers` build format, which can be used to create
   Doxygen-annotated, in-sequence, copyright-commented canonical header files for
   inclusion in a project repository. This output is useful for implementation
   developers because IDEs can use the annotations for API tooltips and code completion.
   The level of Doxygen annotation is controlled by the `header_doxygen` configuration
   attribute in `doc_info`.

#### API element sequencing within header files

For the `.. insert-header::` directive and the `headers` build format, the API elements
appear in the header file in the order that they are defined in the source files. When
multiple source files provide definitions for the same header file, the `:seq:` option
should be used in a `.. header::` directive to ensure that the definitions from
different source files appear in the intended order in the header file.

In contrast, the `api-db` build format used by `api-diff` and `api-update` normalizes
the output by API element identity, so it is resistant to documentation refactoring and
to incidental variation in Sphinx partial-build ordering. This is why changes to source
ordering often affect the inserted header listings and `headers` output, but do not
affect the `api.db` baseline.

The main practical use of `:seq:` is therefore for generated header listings and
`headers` output when a single header is assembled from multiple source files. In
specifications where each standard header has a single source location, `:seq:` is
generally not needed. In larger APIs such as the Crypto API, where many source files
contribute to one header, `:seq:` is important to keep the generated header order stable
and intentional.

### Using callouts/admonitions

Many of the docutils/Sphinx admonitions are used within the specifications. Here are the
typical uses:

- `.. note::` - informative material for a developer that is related to the current
  text, but is not necessary for normal use of it.
- `.. admonition:: Implementation note` - informative material for an implementer of the
  specification, often providing options or recommendations. This is just a titled
  generic admonition element.
- `.. warning::` - call out an issue that presents a risk for the implementer or user of
  an API. Use sparingly.
- `.. todo::` - identify unfinished work in the specification. These should be resolved
  prior to publishing a specification. By default, these are rendered when the document
  has a non-zero draft revision, but can be enabled explicitly by adding `'todo'` to the
  `doc_info['include_content']` attribute.
- `.. rationale::` - provide justification for a design decision. By default, these are
  rendered when the document has a non-zero draft revision, but can be enabled
  explicitly by adding `'rationale'` to the `doc_info['include_content']` attribute.
- `.. comment::` - provide commentary related to documentation content. Most useful for
  including notes for reviewers of the rendered documents. By default, these are not
  rendered in a build, but can be overridden by adding `'comment'` to the
  `doc_info['include_content']` attribute.

Admonitions can also be included in the bodies of API element directives, subsection
directives, or sub-element directives. Use them sparingly inside API entries so they
don't overwhelm the normative flow.

### Formatting inline code/monospace text

Quick rules-of-thumb:

- Use ``text`` for plain literal monospace
- Use `text` for the default reference role
- Use the `:code:` role for code-like text that should hyperlink defined identifiers
  when possible

In more detail:

- For plain literal monospace text or code fragments, use the standard reStructuredText
  double backticks: ``monospace text``.
- For single API elements or manifest attributes that are defined anywhere in the Sphinx
  document, use the default reference role: `MACRO` or `function()`. This renders as
  monospace code, hyperlinked to the definition of the element/attribute. PSA
  documentation convention includes the function parentheses, but these are optional.
- For code that should link to API elements or manifest identifiers when possible, use
  the `:code:` role: :code:`foo(MACRO,1)` or :code:`<= MAX_VAL`. This renders as
  monospace code, hyperlinking every identifier or attribute that is present in the
  text.
- The `:code:` role is also useful for identifiers that are moving to or from another
  specification, as these will render as links when possible, otherwise as plain code.

## Directive Reference

Documentation conventions for this reference:

- document the practical use of the directive as seen in these spec sources, rather than
  trying to reverse engineer every implementation detail
- prefer minimal examples copied or adapted from real specification usage
- group document assembly directives by authoring context, so editors can find the
  directives relevant to the file they are editing

Use one section per directive. Keep examples minimal.

Recommended format:

- Purpose
- Syntax
- Common options
- Placement rules
- Output/effect
- Example
- Gotchas

### API documentation directives

#### `.. header::`

Purpose:

Specify the header file to associate the following API elements with.

Syntax:

The header file name is the directive argument. The directive body (optional) provides
additional content (e.g. comments) to include after any copyright or license text.

```rst
.. header:: psa/example
   :copyright: Copyright notice
   :license: Source license
   :c++:
   :guard:
   :system-include: stddef.h stdint.h
   :include: psa/error.h

   /* Optional comment block
    */
```

Common options:

All options are optional.

- `:copyright:` - a copyright notice, included in the `headers` format build.
- `:license:` - an SPDX license identifier, included in the `headers` format build.
- `:c++:` - add preprocessing directives to be able to include the header in a C++
  project.
- `:guard:` - add preprocessing directives to guard against double-inclusion of the
  header file.
- `:system-include:` - list of system-include files to add to the header.
- `:include:` - list of project include files to add to the header.
- `:seq:` - a numerical sequence number for the set of API elements that follow. This
  enables definitions across multiple sources to be output in a defined order.

Apart from `:seq:`, these options can only be provided on a single instance of the `..
header::` directive for each header file within the document sources.

Placement rules:

Anywhere in a .rst source, outside of an API element definition.

Output/effect:

Associates all following API definitions with the named header, unless they provide a
`:header:` override option. Defines the order in which API elements are included in a
header file. Specifies additional material to be used when generating the header file.

Example:

```rst
.. header:: psa/crypto
   :seq: 10
   :copyright: Copyright 2026 Example Publisher
   :license: Apache-2.0
   :c++:
   :guard:
   :system-include: stddef.h stdint.h
   :include: psa/error.h

   /* This file is a reference template for implementation of the
    * PSA Crypto API v1.5
    */
```

Gotchas:

When using the copyright option, license option, or including a version in the comments;
these are not affected by changing the document configuration and must be edited to
match the document setup.

#### `.. insert-header::`

Purpose:

Insert the named header into the document.

Syntax:

```rst
.. insert-header:: psa/example
```

Placement rules:

None. Current practice is to use this in an appendix.

This directive is processed after all document sources have been parsed - ensuring that
the inserted header is always consistent with the API definitions in the document.

Output/effect:

The named header is generated as a source listing, and automatically cross-references
all defined API elements.

Example:

```rst
.. insert-header:: psa/client
```

#### `.. macro::`

Purpose:

Define a macro API element.

For function-like macros, include one or more parameter definitions, and an optional
return description as part of the directive body.

Syntax:

```rst
.. macro:: PSA_EXAMPLE
   :definition: (0u)

   .. summary:: Summary text.

   .. param:: parameter_name
      Parameter description.
   .. return::
      Return value description.

   Macro description.
```

The `.. macro::` directive argument is the name of the macro. The optional macro
definition is provided in the `:definition:` option. For a function-like macro:

- The parameter names are taken from `.. param::` directives in the body, in the order
  in the .rst file.
- Optionally, `.. return::` and `.. retval::` directives can be used to describe the
  expected output.

Common options:

- `:definition:` (optional) - provide the macro definition.
- `:api-version: <type>` (optional) - define the macro using the document version
  (from conf.py). `type` is one of `major`, `minor`, or `hex` which results in the
  macro definition being the major version, minor version, or a 16-bit
  `(major << 8) | minor` value respectively. This option cannot be used at the same
  time as `:definition:`.

Also, see the standard API element options.

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Creates a macro definition section in the document, and adds the macro to the generated
API declarations.

If no definition is provided, the default comment definition of `/* ... */` is used.

Example:

```rst
.. macro:: PSA_VERSION_NONE
   :definition: (0u)

   .. summary:: This is the return value from `psa_version()` if the requested RoT
      Service is not present.
```

Gotchas:

The tool still permits legacy use of having the macro definition as additional lines in
the directive argument. Current best practice is to have only the name in the directive
argument, and include any definition in the `:definition:` option.

To force an empty definition for the macro, use a `:definition:` option with no content.

#### `.. function::`

Purpose:

Define a function or function-pointer API element.

Provide the parameter definitions, return type, and important return values as part of
the directive body.

Syntax:

```rst
.. function:: psa_example

   .. summary:: Summary text.

   .. param:: param_type param_name
      Parameter description.
   .. return:: return_type
      Short description of returned value.
   .. retval:: value or value description
      Details on specific error code, or return values.

   Function description.
```

The `.. function::` directive argument is only the name of the function. The function
parameter types and names are taken from `.. param::` directives in the body, in the
order in the .rst file. The function return type is taken from the `.. return::`
directive in the body.

Common options:

- `:type:` - define a function pointer type. Use this for defining callback or
  function-pointer types instead of ordinary functions.

Also, see the standard API element options.

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Creates a function or function-type definition section in the document, and adds the
function or function-type to the generated API declarations.

Example:

```rst
.. function:: psa_framework_version

   .. summary:: This function retrieves the version of the PSA Framework API that is implemented.

   .. return:: uint32_t
```

Gotchas:

For a void-returning function, the `.. return:: void` directive must be included in the
body, to avoid a warning about a missing return type.

#### `.. typedef::`

Purpose:

Define a type alias.

This directive can also be used to define incomplete types and incomplete structure
types.

Syntax:

```rst
.. typedef:: uint32_t psa_example_t

   .. summary:: Summary text.
```

The directive argument is the type definition. It can optionally include the `typedef`
prefix and a trailing `;`.

To define an incomplete type, which an implementation of the API must fully define, a
comment can be used in the definition. For example:

```rst
.. typedef:: /* implementation-defined type */ psa_example_t

   .. summary:: Summary text.
```

Common options:

See the standard API element options.

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Creates a typedef definition section in the document, and adds the type definition to
the generated API declarations.

Example:

```rst
.. typedef:: int32_t psa_handle_t

   .. summary:: This type is used for handles.
```

Gotchas:

Use a `.. function::` directive with the `:type:` option to define a pointer-to-function
type.

#### `.. struct::`

Purpose:

Define a structure or structure type API element.

Provide the member definitions as part of the directive body.

Syntax:

```rst
.. struct:: psa_example
   :type:

   .. summary:: Summary text.

   .. field:: uint32_t member
      Field description.
```

Structure members are declared using the `.. field::` directives in the body, and appear
in the order of definition in the source .rst file.

Common options:

- `:type:` - define a typedef for the structure identifier, not just the named `struct`.

Also, see the standard API element options.

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Creates a structure definition section in the document, and adds the definition to the
generated API declarations.

Example:

```rst
.. struct:: psa_msg_t
   :type:
```

Gotchas:

To define an incomplete structure type, use a `.. typedef::` directive with a commented
structure body. For example:

```rst
.. typedef:: struct { /* implementation-defined */ } psa_example

   Type description.
```

#### `.. enum::`

Purpose:

Define an enumeration or enumeration type API element.

Syntax:

```rst
.. enum:: psa_example
   :type:

   .. summary:: Summary text.

   .. value:: VALUE_1 = 42
      Enumeration value description.
```

Enumeration values are declared using the `.. value::` directives in the body, and
appear in the order of definition in the source .rst file.

Common options:

- `:type:` - define a typedef for the enumeration identifier, not just the named `enum`.

Also, see the standard API element options.

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Creates an enumeration definition section in the document, and adds the definition to
the generated API declarations.

Example:

```rst
.. enum:: psa_operation_mode_t
   :type:

   .. value:: sync = 1
   .. value:: async = 2
```

#### `.. attribute::`

Purpose:

Define a JSON schema element.

Syntax:

```rst
.. attribute:: example_attribute

   Properties: Required.

   Description text.
```

Placement rules:

Anywhere in a document section, but see the note about Rendering of C API element
definitions above.

Output/effect:

Generate a JSON attribute definition section in the document.

JSON attributes do not appear in any generated C header file. However, JSON attributes
can be cross-referenced, as for API identifiers, and will be automatically linked in
inline code and code blocks that specify the `xref` language.

Example:

```rst
.. attribute:: psa_framework_version

   Properties: Required.
```

Gotchas:

Although JSON attributes do not share a programming namespace with C API identifiers,
the psa-api-tool currently does not separate these domains when resolving
cross-references. If a C identifier and a JSON attribute have the same name, this will
result in build warnings and can result in incorrect cross reference linkage.

Shared PSA convention for nested JSON objects:

When documenting nested manifest objects such as `service`, `irq`, or MMIO region
objects, do not use nested `.. attribute::` directives for the sub-attributes unless the
tooling has been extended to support scoped names. Today, repeated attribute names such
as `name` and `description` would collide.

Instead:

- keep the top-level JSON attribute, such as `services` or `irqs`, as the single `..
  attribute::` definition
- structure the body using `.. rubric::` titles for internal grouping
- use `.. container:: apisubitem` with definition-list style content to document nested
  object attributes in a compact, API-like layout

This is a source convention rather than a formal feature of the extension, but it is
stable in practice because `apisubitem` is the same styling hook used internally for API
sub-elements.

#### `.. summary::`

Purpose:

Provide a brief summary of the API element. This is typically a single sentence.

Syntax:

```rst
.. summary:: Summary sentence.
```

Placement rules:

Must be nested under an API element directive.

Output/effect:

This is the first text within an API element definition section, before the prototype of
the API element.

The first sentence of the summary is used as the Doxygen `@brief` text in generated
header files, typically used by IDEs for code completion support.

Example:

```rst
.. macro:: PSA_FRAMEWORK_VERSION

   .. summary:: The version of the PSA Framework API provided by the included header file.
```

Gotchas:

The preferred place to provide `.. versionadded::` directives is as the last part of the
body of the API element's `.. summary::` directive.

#### `.. param::`

Purpose:

Document a parameter to a function or macro.

Syntax:

```rst
.. param:: <type> <name>
   Parameter description.
```

The type and name should be valid C syntax for a parameter declaration.

Additional paragraphs and lists can be used in the description to describe more complex
requirements when required for the parameter.

Placement rules:

Must be nested within a `.. function::` or `.. macro::` directive.

Output/effect:

The parameters to the function or macro are rendered in a list immediately after the API
element signature, in the order declared in the .rst file.

The first sentence of the description is used as the Doxygen `@param` text in generated
header files, typically used by IDEs for code completion support.

Example:

```rst
.. param:: size_t buf_len
   Length of buffer ``buf``.
```

#### `.. output::`

Purpose:

Document an additional output from a function.

Additional outputs are those written into objects or buffers that are passed by
non-const pointer to the function.

Syntax:

```rst
.. output:: <output-location>
   Output description.
```

The output-location is typically written as `*p`, where `p` is a buffer or output
parameter that is passed by pointer.

Placement rules:

Must be nested within a `.. function::` or `.. macro::` directive. In the source file,
these can be placed alongside the related `.. param::` directive, or grouped elsewhere
in the API element directive.

Output/effect:

The output descriptions are rendered in a list following the parameter definitions, in
the order declared in the .rst file.

Example:

```rst
.. output:: *buf
   On success, the data is written to the buffer pointed to by `buf`.
```

Gotchas:

These are used rarely in the PSA API specifications. At present, they currently only
appear in the Attestation API.

Most of the API specifications describe the values output in such parameters as part of
the parameter description, instead of using both a `.. param::` and `.. output::`
directive. For example:

```rst
.. param:: uint8_t * nonce
   Buffer where the generated nonce is to be written.
.. param:: size_t * nonce_length
   On success, the number of bytes of the generated nonce.
```

#### `.. return::`

Purpose:

Document the return from a function, or function-like macro.

Syntax:

```rst
.. return:: [<type>]
   Return description.
```

For function-like macros, the type should be omitted.

For functions that return a status code, such as `psa_status_t`, the description is
usually omitted if the return values are described using `.. retval::` directives.

For simple functions or macros, the computation or evaluation that is performed can be
documented here.

Placement rules:

Must be nested within a `.. function::` or `.. macro::` directive.

Output/effect:

The type forms part of the function signature.

The return value is described after the parameters and outputs.

The first sentence of the description is used as the Doxygen `@returns` text in
generated header files, typically used by IDEs for code completion support.

Examples:

```rst
.. return:: size_t
   The number of bytes read from the message parameter.
```

Gotchas:

Technically, this is required for a function that has a `void` return. If none is
provided, a warning is used during the build, and `void` is assumed.

This directive is not required for `.. macro::` API elements.

#### `.. retval::`

Purpose:

Document a specific return value, or range of return values from a function.

Syntax:

```rst
.. retval:: <value>
   Return value description.
```

The directive argument can be a status code, such as `PSA_ERROR_INVALID_ARGUMENT`, or
can be an expression, such as `> 0`. When rendered, any API elements are hyperlinked to
their definition.

Placement rules:

Must be nested within a `.. function::` or `.. macro::` directive.

Output/effect:

The return values are described after the return type. Return values are listed in the
same order as the source, except when the `'error_order'` attribute is set in `doc_info`
in `conf.py`.

Examples:

```rst
.. retval:: PSA_SUCCESS
   The operation completed successfully.
```

#### `.. field::`

Purpose:

Document a member field in a structure.

Syntax:

```rst
.. field:: <type> <name>
   Field member description.
```

The type and name should be valid C syntax for a structure member declaration.

Placement rules:

Must be nested within a `.. struct::` directive.

Output/effect:

The members of a structure are rendered in a list immediately after the API structure
signature, in the order declared in the .rst file.

The first sentence of the description is used as the Doxygen `@brief` text in generated
header files, typically used by IDEs for code completion support.

Example:

```rst
.. field:: uint8_t major
   Major version number.
```

#### `.. value::`

Purpose:

Document an enumeration member value.

Syntax:

```rst
.. value:: {<name>|<name=value>}
   Value member description.
```

The directive argument must be a valid C declaration of an enumeration member.

Placement rules:

Must be nested within a `.. enum::` directive.

Output/effect:

The members of an enumeration are rendered in a list immediately after the API
enumeration signature, in the order declared in the .rst file.

The first sentence of the description is used as the Doxygen `@brief` text in generated
header files, typically used by IDEs for code completion support.

Example:

```rst
.. value:: block = 0
   Wait for a result.
.. value:: no_block = 1
   Return current status immediately.
```

#### `.. subsection::`

Purpose:

Provide an additional sub-titled section of the API element documentation.

This is useful for providing additional information about API elements in a consistent
manner throughout a specification. For example, the 'Key format' sections in the Crypto
API key type definitions and the 'Compatible key types' sections in the Crypto API
algorithm definitions.

Syntax:

```rst
.. subsection:: <subtitle>

   Subsection content.
```

Common options:

- `:top:` (optional) - a flag to indicate that this subsection should precede the API
  element Description. If not provided, the default placement is after the API element
  Description.

Placement rules:

Must be nested within an API element directive.

Output/effect:

A new subsection of the API definition is output, either immediately before or
immediately after, the Description for the API element. The placement depends on the use
of the `:top:` option, but is otherwise in the same order as in the .rst source.
Subsection headings are similar to the subheadings for Parameters, Outputs, Returns, and
Description.

Example:

```rst
.. subsection:: Availability

   This API is optional. Use `PSA_FRAMEWORK_HAS_MM_IOVEC` to determine availability of this function.
```

Repository convention:

- Prefer `Availability` for describing which framework features, service models, or
  execution models an API can be used with.
- Keep the `.. summary::` focused on what the API is or does.
- Do not overload version-markup bodies with availability rules or feature
  applicability.
- The use of `Availability` is established, but its canonical placement within an API
  entry is still deferred. Keep placement consistent within a local edit pass, and
  normalize it later once the integrated API text is more complete.

### Document assembly directives

The directives in this section are grouped by where editors normally use them. This
means that related producer/consumer directives are not always adjacent. For example,
`.. reference::` entries are normally authored in front-matter inputs, while `..
reference-table::` is normally used by a template or by an appendix that chooses to
render the collected references elsewhere.

A document can override or ignore the configured document template source. For example,
a document can avoid `.. title::` and write its title page directly, or avoid `..
about::` and write the front matter directly. More commonly, a document uses
front-matter section directives such as `.. references::` with `:hide:`, `:replace:`, or
`:extend:` to adjust selected template sections.

The current `psa-api-*` templates use an unnumbered front-matter chapter called "About
this document", normally listed under `.. front-matter::`, followed by a numbered
chapter 1 introduction in the main document.

One non-standard arrangement is a bibliography appendix. In that pattern, the document
writes the front matter directly using template construction directives instead of using
`.. about::`, omits the front-matter References section, and renders the references from
an appendix source that contains the `.. reference::` entries followed by `..
reference-table::`.

### Directives Used in `index.rst`

These directives are normally used in the top-level `index.rst` source.

The `.. front-matter::`, `.. maintoc::`, and `.. appendix::` directives are based on
Sphinx `.. toctree::`, with additional output controls for the document section. They
support these common options:

- `:numbered:` - override the section numbering depth.
- `:maxdepth:` - override the table-of-contents depth.

For HTML output, `:numbered:` is only applied by `.. maintoc::` and `.. appendix::`;
front-matter headings are not numbered in HTML. For LaTeX/PDF output, the
table-of-contents depth is controlled by the configured document template, not per
document section.

PDF page numbering style is also controlled by the configured document template. The
current `psa-api-*` templates use roman page numbers for front matter and Arabic page
numbers for main content and appendices.

#### `.. title::`

Purpose:

Insert the template title page for the document.

Syntax:

```rst
.. title::

   .. abstract::

      This document defines ...
```

Placement rules:

This should be the first directive in the top-level `index.rst` file. Any content in the
directive body is parsed before the template title page is included, so it can provide
`.. abstract::` and `.. banner::` inputs. For compatibility with older sources, ordinary
parsed content that remains after processing collector directives is treated as the
`abstract` front-matter section when the selected template defines one.

Output/effect:

The directive includes `title-page.rst` from the selected template directory. Templates
that use an `abstract` front-section insert the collected abstract at the
template-defined title page location.

Gotchas:

The directive itself has no argument. The document title and metadata come from
`doc_info` in `conf.py` and from the selected template.

#### `.. abstract::`

Purpose:

Provide the content for the title page document abstract.

Syntax:

```rst
.. title::

   .. abstract::

      This document is ...
```

Placement rules:

If used, this must appear in the body of the `.. title::` directive in the document
index.rst file.

Output/effect:

Places an abstract on the title page of the document.

#### `.. banner::`

Purpose:

Provide the content for a highlighted box that is placed on the front page of a
document.

Syntax:

```rst
.. title::

   .. banner:: BETA RELEASE

      Status description.
```

Placement rules:

If used, this must appear in the body of the `.. title::` directive in the document
index.rst file.

Output/effect:

By default, the banner is only output when the document has a non-zero draft revision,
but this can be overridden by adding `'banner'` to the `doc_info['include_content']`
configuration attribute.

#### `.. front-matter::`

Purpose:

A toc-like directive used in the top-level `index.rst` file to define the front-matter
content of the document.

Syntax:

```rst
.. front-matter::
   :maxdepth: 2

   about/about
```

Common options:

- `:numbered:` - accepted, but ignored for HTML output. The default value is `0`, which
  gives unnumbered front-matter headings in PDF output.
- `:maxdepth:` - defaults to `2`.

Gotchas:

- Like the `.. toctree::` directive, the contents are treated as a list of content
  files, and must not be reflowed as a paragraph if modifying indentation and source
  layout.
- This is normally used by Arm-style `psa-api-*` templates for the unnumbered "About
  this document" chapter. GP-style documents normally put `about/about` at the start of
  `.. maintoc::` instead.

#### `.. maintoc::`

Purpose:

A toc-like directive used in the top-level `index.rst` file to define the main body
content of the document.

Syntax:

```rst
.. maintoc::
   :numbered: 3
   :maxdepth: 3

   intro
   architecture
```

Common options:

- `:numbered:` - defaults to `3`.
- `:maxdepth:` - defaults to `3`.

Gotchas:

- Like the `.. toctree::` directive, the contents are treated as a list of content
  files, and must not be reflowed as a paragraph if modifying indentation and source
  layout.
- In GP-style documents, include `about/about` as the first entry so the
  template-provided Introduction chapter is numbered as chapter 1.

#### `.. appendix::`

Purpose:

A toc-like directive used in the top-level `index.rst` file to define the appendix
content of the document.

Syntax:

```rst
.. appendix::
   :numbered: 3
   :maxdepth: 3

   appendix/reference-headers
```

Common options:

- `:numbered:` - defaults to `3`.
- `:maxdepth:` - defaults to `3`.

Gotchas:

- Like the `.. toctree::` directive, the contents are treated as a list of content
  files, and must not be reflowed as a paragraph if modifying indentation and source
  layout.
- Appendix chapters use alphabetic numbering in HTML and PDF output.
- The current templates use Arabic appendix page numbers in PDF output, but this is a
  template convention rather than a directive behavior.

### Directives Used in the Front-Matter Chapter

These directives are normally used in the source that provides template front-section
content, typically `about.rst`, or in files included by that source such as `releases`,
`references`, and `terms`. In Arm-style documents, that source is normally listed by `..
front-matter::`. In GP-style documents, that source is normally the first entry under
`.. maintoc::`.

The common pattern is:

1. Include or define releases, references, and terms.
2. Add front-matter section directives to hide, replace, or extend template sections.
3. End the source with `.. about::`, which includes the configured document template's
   `about-chapter.rst` and consumes the previously collected content.

Placement rule for collected front-matter content:

The content-providing directives must appear before the directive that renders that
content, in the same source document after includes have been expanded.

For example:

- Include or define `.. release::` entries before `.. release-table::`.
- Include or define `.. reference::` entries before `.. reference-table::`.
- Include or define `.. term::`, `.. scterm::`, and `.. abbr::` entries before `..
  term-table::`.
- Place front-matter section directives before `.. about::` or `.. insert-section::`.
- Place `.. banner::` before `.. insert-banner::`.

The usual pattern is to define `releases`, `references`, and `terms` in extensionless
include files, include those files from `about.rst`, apply any front-matter section
controls such as `.. introduction::`, `.. audience::`, or `.. api-status::`, and then
end `about.rst` with `.. about::`.

#### `.. release::`

Purpose:

Define a release entry for the document releases table.

Syntax:

```rst
.. release:: <version>
   :date: <publish-date>

   Release summary
```

Placement rules:

See the placement rule for collected front-matter content above.

The most common pattern is to have all releases defined in a `releases` source file that
is included in the `about.rst` source. The definitions can also just be inline within
the `about.rst` source file.

Gotchas:

-   Releases in the table appear in the same order as the `.. release::` directives in
   the source .rst file.
-   Keep the release text summary concise, as this is part of a table of releases.
   Detailed change information is better to maintain in a document appendix, with a
   reference to the appendix from the `.. release-info::` section.

   To add a reference to a change-history appendix, the following text can be added to `about.rst`:

   ```rst
   .. release-info::
      :extend:

      For a detailed list of changes in each document version, see :secref:`change-history`.
   ```

   Provide a named anchor for the section reference in the appendix source immediately before the heading:

   ```rst
   .. _change-history:

   Document changes
   ================
   ```

#### `.. term::`, `.. scterm::`, and `.. abbr::`

Purpose:

Define a glossary entry for the document's Terms and abbreviations table.

The `.. scterm::` directive defines a small-caps-styled term, used for terminology in
the document that has very specific meaning. The `.. abbr::` directive defines an
abbreviation-only entry.

Syntax:

```rst
.. term:: <term>
   :abbr: <abbreviation>

   Definition

.. abbr:: <abbreviation>

   Meaning
```

Common options:

- `:abbr:` (optional) - an abbreviation of the term. This will automatically include an
  additional glossary entry for the abbreviation, referring to the full term definition.

Placement rules:

See the placement rule for collected front-matter content above.

The most common pattern is to have all terms defined in a `terms` source file that is
included in the `about.rst` source. The definitions can also just be inline within the
`about.rst` source file.

Output/effect:

- Creates one or two definitions in the terms and abbreviations data.
- Creates a link target for the term, and abbreviation if provided, that can be
  referenced with the `:term:` and `:scterm:` roles.

Gotchas:

The current PSA API templates usually render a single combined terms and abbreviations
table.

#### `.. reference::`

Purpose:

Define a citation reference entry for the document's References table.

Syntax:

```rst
.. reference:: <ref_id>
   :title: <title>
   :author: <author>
   :doc_id: <pub_id>
   :kind: normative
   :publication: <date|location>
   :url: <url>
```

The directive argument is the citation identifier used within the document text to refer
to this particular external document or website.

For an RFC document, `RFC nnnn` is the canonical rendered form. Compact source forms
such as `RFCnnnn` are normalized so the `:rfc:` and `:rfc-title:` roles can correctly
link to the citation.

Common options:

- `:title:` (required) - The document title or website name.
- `:author:` (optional) - The person or organization that produced the document.
- `:doc_id:` (optional) - The publisher's own identifier number or label.
- `:kind:` (optional) - The reference classification, either `normative` or
  `informative`. The default is `normative`.
- `:publication:` (optional) - The date or location of the published document.
- `:url:` (optional) - A URL to the document or the publishing organization. If the URL
  text does not start with 'https://' - this is added to the anchor link automatically,
  but not rendered in the anchor text.

Placement rules:

See the placement rule for collected front-matter content above.

The most common pattern is to have all references defined in a `references` source file
that is included in the `about.rst` source. The definitions can also just be inline
within the `about.rst` source file.

Output/effect:

- Creates a citation reference in the References table.
- Creates a link target for the citation that can be referenced with the `:cite:`,
  `:cite-title:`, `:rfc:`, and `:rfc-title:` roles.

Gotchas:

References do not have to be rendered in the front matter. To move references to an
appendix, hide or replace the front-matter `.. references::` section, then place the `..
reference::` directives and `.. reference-table::` in an appendix source.

The current PSA API templates render a single combined references table.

#### Front-matter section directives

Purpose:

Customize front-matter sections provided by the configured document template.

Syntax:

```rst
.. release-info::
   :extend:

   For a detailed list of changes, see :secref:`change-history`.
```

Common options:

- `:replace:` - replace the template's default content for this section.
- `:extend:` - append this content to the template's default content.
- `:hide:` - suppress this section.

At most one of these options can be provided in a directive.

- If no option is provided and the directive has content, the content replaces the
  default section content. Prefer using the `:replace:` option explicitly.
- If no option and no content are provided, the section is treated as hidden. This is
  deprecated usage, it is recommended to explicitly specify `:hide:`.

Supported section directives are defined by the selected template. The GP template
currently provides:

- `.. introduction::`
- `.. api-status::`
- `.. feedback::`
- `.. audience::`
- `.. license::`
- `.. references::`
- `.. terms::`
- `.. abbreviations::`
- `.. release-info::`
- `.. todos::`

The PSA API 2022 and 2025 templates provide the older Arm-style section set:

- `.. abstract::`
- `.. release-info::`
- `.. todos::`
- `.. license::` in the 2025 template
- `.. references::`
- `.. terms::`
- `.. potential-for-change::`
- `.. conventions::`
- `.. pseudocode::`
- `.. assembler::`
- `.. current-status::`
- `.. feedback::`
- `.. inclusive-language::`

Placement rules:

These directives normally appear in a consuming document's `about.rst` before the final
`.. about::` directive. See the placement rule for collected front-matter content above.

#### `.. about::`

Purpose:

Include the about-chapter source from the configured document template.

Syntax:

```rst
.. about::
```

Placement rules:

This is normally used at the end of the consuming document's `about.rst` source after
any releases, references, terms, and front-matter section overrides have been defined.
See the placement rule for collected front-matter content above.

Output/effect:

The directive includes `about-chapter.rst` from the configured document template
directory. The template's `about-chapter.rst` provides the default content and structure
for front matter sections, which is modified according to the preceding directives in
the `about.rst` source file.

### Directives Used to Construct Template Title Pages and Front Matter

These directives are primarily used in template sources, such as `title-page.rst` and
`about-chapter.rst`. Specification sources can use them directly when they intentionally
bypass part of the configured document template, but that should be a deliberate
document-structure decision.

The rendering directives in this section must appear after the corresponding
content-providing directives have been defined or included in the same source document.
See the placement rule for collected front-matter content above.

#### `.. template-image::`

Purpose:

Insert an image from the selected template directory.

Syntax:

```rst
.. template-image:: logo.svg
   :alt: Logo
```

Placement rules:

This directive is primarily for template sources, such as title pages. It accepts the
same image options as the standard reStructuredText `.. image::` directive.

Output/effect:

The image path is resolved relative to the selected template directory instead of
relative to the document source file.

#### `.. insert-banner::`

Purpose:

Insert the title-page banner collected from a `.. banner::` directive.

Syntax:

```rst
.. insert-banner::
```

Placement rules:

This is primarily a title-page template directive. Use this after the corresponding `..
banner::` directive has been defined or included in the same source document.

Output/effect:

The collected banner content is inserted only when banner output is enabled.

#### `.. insert-section::`

Purpose:

Insert a front-matter section. The directive defines the section title and the default
content when used in a template.

When rendered, the content is modified, extended, or removed as directed by a
corresponding front-matter directive in the specification's `about.rst` source.

Syntax:

```rst
.. insert-section:: Release information
   :section: release-info
   :break-after:
```

Common options:

- `:section:` - required; the front-matter section key to insert.
- `:break-after:` - insert a page break after the section in PDF output.
- `:class:` - wrap the section content in the named class or environment.
- `:not-in-toc:` - render a styled title without creating a section in the table of
  contents.
- `:keep-if-empty:` - keep the section even if no content is available.

Placement rules:

This is primarily a template directive. Specification sources normally use the
front-matter section directives instead to control or modify the template content. Use
this after the corresponding front-matter section directive has been defined or included
in the same source document.

#### `.. release-table::`

Purpose:

Render the release entries collected from `.. release::` directives.

Syntax:

```rst
.. release-table::
```

Placement rules:

Use this after the corresponding `.. release::` entries have been defined or included in
the same source document.

Output/effect:

Creates a table with Date, Version, and Change columns.

#### `.. reference-table::`

Purpose:

Render the references collected from `.. reference::` directives.

Syntax:

```rst
.. reference-table:: Documents referenced by this document
   :sorted:
   :kind: all
   :layout: by-ref
```

Common options:

- `:sorted:` - sort the references alphabetically by their reference identifier.
- `:kind:` - one of `normative`, `informative`, or `all`. The default is `all`.
- `:layout:` - one of `by-ref` or `by-id`. The default is `by-ref`.
- `:filter:` - one of `with-id`, `without-id`, or `none`. The historical names `arm` and
  `non-arm` are accepted as aliases for `with-id` and `without-id`, respectively, so
  older specification sources continue to build.
- `:class:` - apply a table class.
- `:name:` - set an explicit target name for the table.

Placement rules:

Use this after the corresponding `.. reference::` entries have been defined or included
in the same source document. It can be used in another document source, such as an
appendix, if the document chooses to render references outside the front matter.

Output/effect:

Creates a References table. When `:filter: without-id` is used, the document-number
column is omitted in the default `by-ref` layout, preserving the table format for
references that do not have publisher document identifiers.

The `by-ref` layout produces Ref, Document Number, and Title columns. The `by-id` layout
produces Standard/specification, Description, and Reference columns. In the `by-id`
layout, the Standard/specification column uses `:doc_id:` when it is available and falls
back to the citation identifier when a reference has no publisher document identifier.
The Reference column contains the citation form used in the document text, such as
`[PSA-SM]`.

#### `.. term-table::`

Purpose:

Render the terms and abbreviations collected from `.. term::`, `.. scterm::`, and `..
abbr::` directives.

Syntax:

```rst
.. term-table::
   :sorted:
   :kind: all
```

Common options:

- `:sorted:` - sort the terms alphabetically by their normalized identifier.
- `:kind:` - one of `terms`, `abbreviations`, or `all`. The default is `all`.

Placement rules:

Use this after the corresponding term or abbreviation entries have been defined or
included in the same source document.

Output/effect:

Creates a table from the collected terminology data. With `:kind: terms`, the table has
Term and Definition columns. With `:kind: abbreviations`, the table has Abbreviation and
Meaning columns. Each entry also becomes the target for `:term:` and `:scterm:`
references.

If no matching entries have been collected, no table is rendered.

#### `.. include-license::`

Purpose:

Include the license text selected by the document configuration.

Syntax:

```rst
.. include-license::
```

Placement rules:

This is primarily a front-matter template directive. It specifies where the configured
license should be included in the document.

Output/effect:

The directive first looks for a license source matching the configured license value
relative to the document. If none is found, it looks in the `tools/license/` directory,
using a lower-case filename with hyphens converted to underscores and an `.rst` suffix.
If no matching license exists, the built-in missing-license text is included and a build
error is reported.

This allows a template to support multiple licenses without template changes. Selecting
a different license for a document issue is a configuration change, while the license
wording remains centralized in the license source file.

### Directives Used in License Source Files

#### `.. license::`

Purpose:

Mark explicitly provided license text as the document's license section.

Syntax:

```rst
.. license::

   License text.
```

Placement rules:

This directive is intended for license source files, such as the standardized license
files in `tools/license/`. A document template normally uses `.. include-license::` to
include the configured license file at the correct place in the front matter.

A document or template can use `.. license::` directly to embed license text in its own
sources, but this is not recommended. Keeping license text in a separate file reduces
normal editing churn and helps avoid accidental deviation from the approved license
wording.

Output/effect:

This behaves like a section insertion helper. The PSA API templates use a dedicated
style class when inserting this section in the document and forces a page break after
the section.

### Modified Standard Directives

#### `.. rationale::`

Purpose:

Include rationale text that is useful while drafting or reviewing a specification, but
is not part of the normal published flow.

Syntax:

```rst
.. rationale:: Optional title

   Rationale text.
```

Output/effect:

The content is rendered as an admonition only when rationale output is enabled.
Rationale output is enabled when the document has a non-zero draft revision and can also
be enabled by adding `'rationale'` to `doc_info['include_content']`.

#### `.. comment::`

Purpose:

Include review commentary in the source.

Syntax:

```rst
.. comment::

   Reviewer-facing comment.
```

Output/effect:

The content is rendered as a Commentary admonition only when comment output is enabled
by adding `'comment'` to `doc_info['include_content']`.

Gotchas:

Use comments sparingly. They are intended for review builds, not as a substitute for
source comments or issue tracking.

#### `.. code-block::`

This directive supports the additional language `xref`.

When `xref` is used as the argument to this directive, the code block is not rendered
using the normal highlighting engine, but instead has every API element from the current
specification hyperlinked in the output.

The `xref` language option supports the standard `:linenos:` and `:lineno-start:`
options.

#### `.. literalinclude::`

This directive supports the additional language `xref`. When `xref` is used as the
language option, the code block is not rendered using the normal highlighting engine,
but instead has every API element from the current specification hyperlinked in the
output.

The `xref` language option supports the standard `:linenos:` and `:lineno-start:`
options.

## Role Reference

Use the same lightweight format for roles:

- Purpose
- Syntax
- Example
- Gotchas

### The Default Reference Role

Sphinx has a default reference role that is used for any text within single-backticks
without a preceding role specifier.

The `psa-api-tool` Sphinx extension extends the capabilities of the default reference
role as follows:

* If the reference text is of the form `[ref_id]`, this is resolved as a `:cite:` role.
* If the reference text is any of `SG.id`, `DM.id`, `AM.id`, `T.id`, or `M.id`, it is
  resolved using the associated Threat model element role.
* If the reference is a single API element, it is resolved as a `:code:` role.

### `:sc:`

Purpose:

Render the role text in small caps.

This role is configured by `psa-api-conf.py` as a common reStructuredText role, rather
than registered by `psa-api-tool.py` as a domain-specific role.

Syntax/example:

```rst
The result is :sc:`implementation defined`.
```

### `:issue:`

Purpose:

Apply the `issue` CSS/LaTeX role class to inline text, normally for visible open-issue
placeholders.

Syntax/example:

```rst
:issue:`<<Document ID>>`
```

Output/effect:

The current templates render the role text inline, in red.

Gotchas:

This role is configured by `psa-api-conf.py` as a common reStructuredText role. It does
not create or link to an external issue tracker entry.

### `:code:`

Purpose:

Format text as inline code, hyperlinking any API element identifiers that are defined in
this specification.

Syntax/example:

```rst
Use a call to :code:`psa_wait(PSA_WAIT_ANY, PSA_BLOCK)` to wait for the next partition event.
```

Output/effect:

The rendered output is formatted as code, hyperlinking every API element to its
definition.

Gotchas:

- Use the `:code:` role instead of the default reference role for linking to API
  elements if the API definition might be in another specification. This results in code
  formatting without a hyperlink if no matching definition is available.
- Use the `:code:` role instead of reStructuredText double backticks for monospace text
  if the code contains API elements.

### `:secref:`

Purpose:

Include a formatted, inline cross-reference to a titled section or object anywhere in
the specification.

This role is similar to the standard `:ref:` role: the target's title is used as the
link text. It is commonly used for section references, but it can also reference titled
listings, tables, and figures. See also `:numref:`.

Example:

```rst
See :secref:`programming-api` for details.
```

For a reference to a document section, this requires that `programming-api` is defined
as a link target associated with the section heading by defining an explicit anchor in
the source immediately before the heading itself, for example:

```rst
.. _programming-api:

Programming API
---------------
```

Output/effect:

The target's title is rendered as a hyperlink in the output. In HTML output, the link
text uses the template's title-reference styling. In LaTeX/PDF output, the link is
followed by an additional `on page nnn` link when the target is on a different page.

Gotchas:

- Make sure all anchor targets are unique in a document. They do not have to exactly
  match the text in the section heading.
- To reference a figure, listing, or table, these have to have a title, and have the
  anchor name set using the `:name:` option.
- The former `:title:` alias has been removed. Use `:secref:` for title-text cross
  references.

### `:numref:`

Purpose:

Include a formatted, inline cross-reference to a numbered object in the document, such
as a listing, table, figure, or section heading. See also `:secref:`.

Syntax/example:

```rst
:numref:`table-error-codes` summarizes the errors produced by this API. See also :numref:`error-codes`.
```

For a reference to a document section, this requires that `error-codes` is defined as a
link target associated with the section heading by defining an explicit anchor in the
source immediately before the heading itself.

Output/effect:

The target's number is rendered as a hyperlink in the output. In PDF output, the
hyperlink text includes the target page number.

Gotchas:

- Make sure all anchor targets are unique in a document. They do not have to exactly
  match the text in the section heading.
- To reference a figure, listing, or table, these have to have an anchor name set using
  the `:name:` option.

### `:term:`

Purpose:

Include a formatted cross-reference to a term or abbreviation that is defined in the
document glossary.

Syntax/example:

```rst
The RoT Service runs within a :term:`Secure Partition`.
```

The capitalization of the term in the role text does not have to match the
capitalization in the glossary.

Output/effect:

The rendered output is a formatted hyperlink, the capitalization of the text is taken
from the role text.

Gotchas:

- The term or abbreviation must be defined in a `.. term::` directive.
- To pluralize a term, an escaped-space can be used in the .rst source. For example,
  `The :term:``RoT Service``\ s` will render as "The RoT Services", with "RoT Service"
  hyperlinked to the glossary.

### `:scterm:`

Purpose:

Include a formatted cross-reference to a smallcaps term or abbreviation that is defined
in the document glossary.

Syntax/example:

```rst
Providing a zero-length name is a :scterm:`Programmer error`.
```

The capitalization of the term in the role text does not have to match the
capitalization in the glossary.

Output/effect:

The rendered output is a smallcaps-formatted hyperlink.

Gotchas:

- The term or abbreviation must be defined in an `.. scterm::` directive.

### `:cite-title:`

Purpose:

Include a formatted reference to the title of a cited work.

Syntax/example:

```rst
The security model is defined in :cite-title:`PSA_SM`.
```

The role text is the citation `ref_id`.

Output/effect:

The rendered output is a hyperlink containing the title of the cited work followed by
'[ref_id]'.

Gotchas:

- The citation must be defined in a `.. reference:: ref_id` directive.

### `:cite:`

Purpose:

Include an untitled citation reference.

Syntax/example:

```rst
:cite:`PSA_SM` also defines the security goals.
```

The role text is the citation `ref_id`.

Output/effect:

The rendered output is a hyperlink containing '[ref_id]'.

Gotchas:

- The citation must be defined in a `.. reference:: ref_id` directive.

### `:rfc-title:`

Purpose:

Include a formatted reference to the title of a published RFC.

Syntax/example:

```rst
The algorithm is defined in :rfc-title:`9910`.
```

The role text is the RFC number, optionally followed by a '#' and section number.

Output/effect:

The rendered output is a hyperlink containing the title of the cited RFC followed by
`[RFC nnnn]`.

If a section number or appendix letter is included in the role text, a second hyperlink
follows which contains the formatted section number and links directly to that section
of the RFC document.

Gotchas:

- The citation must be defined in a `.. reference:: RFC nnnn` directive, or a compact
  `RFCnnnn` source form that normalizes to the same citation.

### `:rfc:`

Purpose:

Include an untitled RFC reference.

Syntax/example:

```rst
:rfc:`9910#B` discusses the security analysis of the algorithm.
```

The role text is the RFC number, optionally followed by a '#' and section number.

Output/effect:

The rendered output is a hyperlink containing `[RFC nnnn]`.

If a section number or appendix letter is included in the role text, a second hyperlink
follows which contains the formatted section number and links directly to that section
of the RFC document.

Gotchas:

- The citation must be defined in a `.. reference:: RFC nnnn` directive, or a compact
  `RFCnnnn` source form that normalizes to the same citation.

### `:url:`

Purpose:

Render an external URL as a hyperlink.

Syntax/example:

```rst
See :url:`example.com/specification`.
```

Output/effect:

If the role text does not include `//`, the link target is prefixed with `https://`. The
visible text is the role text as written.

## Security Risk Assessment Directives and Roles

The Sphinx extension includes directives and roles that support structured security risk
assessment content.

### `.. threat::`

Purpose:

Define a structured threat entry.

Syntax:

```rst
.. threat:: Threat title
   :id: T.example
   :deployment-models: DM.PROTECTED, DM.EXPOSED

   .. description::

      Threat description.

   .. adversarial-model::

      Relevant adversarial model.

   .. security-goal::

      Security goal affected by this threat.

   .. unmitigated:: DM.PROTECTED
      :impact: H
      :likelihood: M

   .. unmitigated:: DM.EXPOSED
      :impact: VH
      :likelihood: M

   .. mitigations::

      Mitigations.

   .. residual:: DM.PROTECTED
      :impact: L
      :likelihood: L

   .. residual:: DM.EXPOSED
      :impact: M
      :likelihood: L
```

Common options:

- `:id:` - explicit threat identifier.
- `:deployment-models:` - optional comma-separated list of deployment models for which
  risk values can be provided. This also defines the order in which deployment-model
  columns are rendered.

Placement rules:

The sub-directives are parsed inside the `.. threat::` directive body.

Output/effect:

The directive renders a section for the threat. The section title contains the threat
identifier, when provided, followed by the threat title.

If no deployment-model distinction is used, the risk values are rendered as a single
evaluation for the threat. If named deployment models are used, the deployment model row
is rendered before the unmitigated risk rows, and the unmitigated and residual risk rows
use a consistent column layout so each evaluation appears under the relevant
deployment-model title.

### Threat sub-directives

The following sub-directives collect prose fields for a threat:

- `.. description::`
- `.. adversarial-model::`
- `.. security-goal::`
- `.. mitigations::`

The following sub-directives collect risk values:

- `.. unmitigated::`
- `.. residual::`

Risk directives support:

- An optional directive argument - the deployment-model scope for these risk values.
- `:impact:` - required impact value.
- `:likelihood:` - required likelihood value.
- `:risk:` - optional explicit risk value. If omitted, the tool derives the risk from
  the impact and likelihood matrix.

Accepted abbreviated risk values are `VL`, `L`, `M`, `H`, and `VH`.

If the optional argument is omitted, the risk values apply to the threat as a whole. If
a deployment-model argument is provided, the risk values apply only to that deployment
model.

When a threat uses multiple deployment models, use `:deployment-models:` on the `..
threat::` directive to list the expected deployment models and their presentation order,
and provide matching `.. unmitigated:: <DM>` and `.. residual:: <DM>` entries for each
deployment model.

Example:

```rst
.. threat:: Eavesdropping
   :deployment-models: DM.PROTECTED, DM.EXPOSED

   .. unmitigated:: DM.PROTECTED
      :impact: M
      :likelihood: L

   .. residual:: DM.PROTECTED
      :impact: M
      :likelihood: VL

   .. unmitigated:: DM.EXPOSED
      :impact: H
      :likelihood: M

   .. residual:: DM.EXPOSED
      :impact: M
      :likelihood: L
```

### SRA definition and reference roles

Definition roles create a definition target and render a canonical identifier:

- `:deployment-model:`
- `:adversarial-model:`
- `:security-goal:`
- `:threat:`
- `:mitigation:`

Reference roles link to those definitions:

- `:dm:`
- `:am:`
- `:sg:`
- `:t:`
- `:m:`

The long definition roles and the short reference roles use canonical prefixes: `DM.`,
`AM.`, `SG.`, `T.`, and `M.`. If the prefix is omitted in the role text, the tool adds
it.

Example:

```rst
:security-goal:`SG.confidentiality`

The mitigation is described in :m:`isolate-components`.
```

----

*Copyright 2018-2026 Arm Limited*\
*Copyright 2026 GlobalPlatform*
