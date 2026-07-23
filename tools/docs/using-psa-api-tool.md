<!--
SPDX-FileCopyrightText: Copyright 2018-2026 Arm Limited
SPDX-License-Identifier: Apache-2.0
-->

# Building PSA API Specifications

This guide explains how the PSA API specifications in this repository are built with the
integrated `psa-api-tool` copy in `tools/`.

For source editing conventions, custom directives, roles, and API documentation
conventions, see `psa-api-tool-notes.md`.

For occasional lifecycle tasks, such as starting work on a new issue or version,
preparing a release candidate, or finalizing a publication, see
`specification-lifecycle-workflows.md`. That guide intentionally contains placeholders
until the publication process is settled after the repository transfer.

## Tool Layout

The repository has a small top-level `Makefile`:

```make
PSA_API_TOOL ?= tools

ifeq ($(wildcard $(PSA_API_TOOL)/make),)
 $(error The 'PSA_API_TOOL' variable is not set, or does not point to a suitable installation of psa-api-tool)
endif

include $(PSA_API_TOOL)/make
```

By default, builds use the checked-in tool under `tools/`. Editors can override
`PSA_API_TOOL` to test another compatible copy:

```sh
make PSA_API_TOOL=/path/to/psa-api-tool doc/crypto/html
```

Each specification has its own `conf.py` under `doc/<spec>/`. The document configuration
sets `psa_api_tool_path` to the repository `tools/` directory, allows the `PSA_API_TOOL`
environment variable to override that path, then executes `psa-api-conf.py` from the
selected tool copy.

Each specification directory also contains a minimal `pyproject.toml` file. These files
act as project-boundary markers for editor integrations such as Esbonio, allowing each
specification to have a separate Sphinx session and live preview while using the same
checked-in tool copy. They are not used by the repository make targets.

Older PSA API source revisions used the former `atg-sphinx-spec` name. The shared
makefile exports `ATG_SPHINX_SPEC` as an alias for `PSA_API_TOOL`, and the tool provides
`atg-sphinx-conf.py` as a compatibility wrapper around `psa-api-conf.py`. This allows
older source trees to be built with the newer tool by invoking:

```sh
make -f /path/to/psa-api-tool/make doc/crypto/html
```

## Requirements

The core HTML and structured-output build path requires:

- Python 3.
- Sphinx.
- A POSIX-like shell and `make`.

PDF output also requires a LaTeX toolchain that provides `pdflatex`.

The `images` target may require additional tools, depending on the figure sources used
by the specification:

- `wavedrompy` for `.json` bitfield diagrams.
- Java and PlantUML for `.puml` diagrams.
- `rsvg-convert` for SVG-to-PDF conversion.

Graphviz is only required for documents that use Sphinx Graphviz directives.

Most rendered graphics are checked in, so a text-only edit normally does not require the
full graphics toolchain.

The PDF target uses `qpdf` to optimize generated PDF files when it is available. This is
optional.

### Version and Platform Guidance

The build tooling is not currently defined by a pinned requirements file or a repeatable
CI environment. Contributors should report the tool and platform versions used when
diagnosing build differences.

The integrated tool introduction branch has been tested on macOS arm64 with Python
3.13.7, Sphinx 8.1.0, GNU Make 3.81, MiKTeX-pdfTeX 4.10, OpenJDK 21.0.2, PlantUML
1.2025.2, Graphviz 12.2.1, `rsvg-convert` 2.60.0, and `qpdf` 12.3.2. The precursor
tooling was also used successfully with Git Bash on Windows 11, and the tooling is
expected to work on Linux with the equivalent packages installed. These platforms are
descriptive, not a formal support matrix.

The tools are maintained against recent Sphinx releases. Sphinx 8.1.0 is the current
known-good version; Sphinx 5.3 is the oldest version expected to be plausible, but it is
not currently validated. When setting up a new environment, start with the newest stable
versions available from the platform package manager, then validate with the specific
targets needed for the change under review.

## Common Builds

From the repository root, build one output format for one specification:

```sh
make doc/crypto/html
make doc/crypto/pdf
make doc/crypto/headers
make doc/crypto/api-diff
```

The same pattern works for the other specification directories:

- `doc/attestation`
- `doc/crypto`
- `doc/crypto-driver`
- `doc/fwu`
- `doc/status-code`
- `doc/storage`

Build one output format for every specification:

```sh
make html
make pdf
```

Build all default outputs for one specification:

```sh
make doc/crypto
```

Generated output is written under `build/`, mirroring the document path. For example,
`make doc/crypto/html` writes HTML under `build/doc/crypto/html`.

## Targets

| Target | Purpose |
| --- | --- |
| `html` | Build HTML output and rewrite it for the repository website layout. |
| `latex` | Generate LaTeX output. |
| `pdf` | Generate LaTeX output, run `pdflatex`, and optimize the PDF with `qpdf` when available. |
| `xml` | Build XML structured document output. |
| `headers` | Generate reference C header files from API directives. |
| `api-db` | Generate normalized API database headers. |
| `api-diff` | Compare generated API database headers with the checked-in `api.db/` reference. |
| `api-update` | Update the checked-in `api.db/` reference after an intentional API change. |
| `images` | Regenerate converted or generated image assets when required tools are installed. |
| `clean` | Remove generated build output for the selected document or documents. |

Use `INTERNAL=1` to build an internal-tagged output variant when a document uses
internal-only content.

## Recommended Validation

For a source-only documentation change, build the affected HTML output first:

```sh
make doc/<spec>/html
```

For a change that affects API directives, generated C declarations, manifest
definitions, or reference API headers, run:

```sh
make doc/<spec>/api-diff
```

If the API change is intentional, review the diff and update the checked-in API
database:

```sh
make doc/<spec>/api-update
```

When publishing a new revision, the reference headers are updated using the output
from the build:

```sh
make doc/<spec>/headers
```

For changes that affect title pages, front matter, page breaks, LaTeX styling, or
publication-ready layout, build:

```sh
make doc/<spec>/pdf
```

For changes to graphics sources, run:

```sh
make doc/<spec>/images
```

The XML output can be useful when reviewing generated document structure, resolved
references, table structure, glossary entries, and API sections:

```sh
make doc/<spec>/xml
```

Treat XML as supplementary validation. It does not replace rendered HTML/PDF inspection
or API database checks. It can be helpful when diagnosing why some source content is
not rendering as expected.

## Document Configuration

Each document `conf.py` defines a `doc_info` dictionary and then executes the shared
tool configuration. Keep document configuration focused on document metadata and
document-specific choices. Avoid setting Sphinx configuration variables directly in
`conf.py` unless the shared configuration cannot support the required behavior.

The current PSA API documents use the Arm-style `psa-api-2022` and `psa-api-2025`
templates. These templates preserve the existing front matter and release metadata model
while allowing the repository to build without an external `atg-sphinx-spec` checkout.

Important `doc_info` keys used by these specifications include:

| Key | Purpose |
| --- | --- |
| `template` | Template directory under `tools/templates/`. |
| `title` | Document title used by Sphinx and the title page. |
| `version` | Required document lifecycle version. This is the Sphinx version and is used for document release labels, filenames, and `|docversion|`. |
| `api_version` | Optional target API version, normally `X.Y`. It defaults to `version` for compatibility. It is used for `|APIversion|`, `|majorversion|`, `|minorversion|`, `|hexversion|`, and `:api-version:` macro values. |
| `issue_no` | Document issue or maintenance revision. |
| `draft` | Draft flag or draft revision, depending on the selected publication model. |
| `release_candidate` | Release-candidate number for existing Arm-style documents. |
| `quality` | API maturity code such as `ALP`, `BET`, or `REL`. |
| `header` | Default generated C header path for API directives. |
| `header_doxygen` | Generated header annotation level. |
| `error_order` | Document-wide order for generated return values. |
| `identifier_index` | Controls the generated C identifier index. |
| `prolog_files` | Shared substitution files included in the Sphinx prolog. |

`version` identifies the document lifecycle version. The optional `api_version`
identifies the API described by the document and must use `X.Y` format. If it is
omitted, the tool derives the API version from the document version's major and
minor components, so a document version of `1.5.0` describes API version `1.5`.

An explicit `api_version` can match the document's major/minor version. For a
non-published document it can instead target only the next minor version or the
next major version. Published documents must describe the API with the same
major/minor version as the document. Invalid API-version settings stop the build
with a configuration diagnostic.

For detailed directive and role behavior, use `psa-api-tool-notes.md` as the editing
reference.

----

*Copyright 2018-2026 Arm Limited*
