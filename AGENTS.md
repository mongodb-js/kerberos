# AGENTS.md

Instructions for AI coding agents working in this repository. This file is the source of truth. Tool-specific files (e.g. CLAUDE.md) should only import it.

## Project Overview

`kerberos` is a Node.js native addon providing Kerberos/GSSAPI authentication, used primarily by the MongoDB Node.js driver for GSSAPI auth. The C++ addon (node-addon-api / N-API 9) wraps GSSAPI on Unix and SSPI on Windows. On install, `prebuild-install` looks for a prebuilt addon binary, falling back to a source build via `node-gyp rebuild` (requires system Kerberos dev headers, e.g. `libkrb5-dev`, on Unix).

## Commands

All scripts from `package.json`:

- `npm install` — installs deps and compiles the addon.
- `npx node-gyp rebuild` — recompile the addon only.
- `npm test` — run mocha tests. Requires a compiled addon and a reachable KDC (configured via `KERBEROS_USERNAME`/`KERBEROS_PASSWORD`/`KERBEROS_REALM`/`KERBEROS_HOSTNAME` env vars).
- `npm run check:lint` — lint JS/TS.
- `npm run format-js` — eslint --fix.
- `npm run format-cxx` — clang-format the C++ sources in `src/`.
- `npm run docs` — regenerate README.md from JSDoc in `lib/index.js` (never edit README.md directly, edit `etc/README.hbs` or the JSDoc).
- `npm run precommit` — `check-clang-format`. Verifies C++ formatting, and is invoked by the pre-commit hook.
- `npm run prepare` — points `core.hooksPath` at `.githooks/` so the tracked pre-commit hook is active. Runs automatically on `npm install` in a clone. It does not run for downstream consumers installing the published package.
- `npm run prebuild` — build prebuilt addon binaries (`prebuild --runtime napi --strip --verbose --all`). Used by CI and release, rarely run locally.

## Structure

- `src/` — C++ addon. `kerberos.cc` defines the N-API bindings.
- `src/unix/` — GSSAPI implementation.
- `src/win32/` — SSPI implementation.
- `lib/index.js` — entrypoint: loads the compiled `.node` binary and exports promisified wrappers plus GSS flag/OID constants.
- `index.d.ts` — the public types.
- `binding.gyp` — node-gyp config. Selects unix vs win32 sources per platform.
- `test/` — mocha tests, plus `test/bundling/webpack` for bundler compatibility.

## Code Conventions

- **Formatting** — Prettier: single quotes, 2-space indent, 100-char width, no trailing commas. C++: clang-format (`.clang-format`).
- **JS style** — CommonJS (`require`), `'use strict'`. There are no TypeScript sources, types are hand-maintained in `index.d.ts`.
- **Docs** — public API JSDoc in `lib/index.js` is the source for README.md.

## Commit Messages

[Conventional Commits](https://www.conventionalcommits.org/) optionally with a Jira ticket: `<type>(NODE-XXXX): <subject>` — types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `chore`; breaking changes use `!` (e.g. `feat(NODE-XXXX)!: …`). This rule is mandatory for PR descriptions, because that is what ends up in the history. The individual commits inside a PR do not have to follow this convention, because we squash PR commits.

## Related Repositories

- [mongodb/node-mongodb-native](https://github.com/mongodb/node-mongodb-native) — the MongoDB Node.js driver, the primary consumer of this package.
