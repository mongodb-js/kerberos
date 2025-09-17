// @ts-check

import util from 'node:util';
import process from 'node:process';
import fs from 'node:fs/promises';
import child_process from 'node:child_process';
import events from 'node:events';
import path from 'node:path';
import url from 'node:url';

const __dirname = path.dirname(url.fileURLToPath(import.meta.url));

/** Resolves to the root of this repository */
function resolveRoot(...paths) {
  return path.resolve(__dirname, '..', '..', ...paths);
}

async function parseArguments() {
  const pkg = JSON.parse(await fs.readFile(resolveRoot('package.json'), 'utf8'));

  const options = {
    'kerberos_use_rtld': { type: 'boolean', default: true },
    help: { short: 'h', type: 'boolean', default: false }
  };

  const args = util.parseArgs({ args: process.argv.slice(2), options, allowPositionals: false });

  if (args.values.help) {
    console.log(
      `${path.basename(process.argv[1])} ${[...Object.keys(options)]
        .filter(k => k !== 'help')
        .map(k => `[--${k}=${options[k].type}]`)
        .join(' ')}`
    );
    process.exit(0);
  }

  return {
    kerberos_use_rtld: !!args.values.kerberos_use_rtld,
    pkg
  };
}

/** `xtrace` style command runner, uses spawn so that stdio is inherited */
async function run(command, args = [], options = {}) {
  const commandDetails = `+ ${command} ${args.join(' ')}${options.cwd ? ` (in: ${options.cwd})` : ''}`;
  console.error(commandDetails);
  const proc = child_process.spawn(command, args, {
    shell: process.platform === 'win32',
    stdio: 'inherit',
    cwd: resolveRoot('.'),
    ...options
  });
  await events.once(proc, 'exit');

  if (proc.exitCode != 0) throw new Error(`CRASH(${proc.exitCode}): ${commandDetails}`);
}

async function buildBindings(args, pkg) {
  await fs.rm(resolveRoot('build'), { force: true, recursive: true });
  await fs.rm(resolveRoot('prebuilds'), { force: true, recursive: true });

  // install with "ignore-scripts" so that we don't attempt to download a prebuild
  await run('npm', ['install', '--ignore-scripts']);
  // The prebuild command will make both a .node file in `./build` (local and CI testing will run on current code)
  // it will also produce `./prebuilds/kerberos-vVERSION-napi-vNAPI_VERSION-OS-ARCH.tar.gz`.

  let gypDefines = process.env.GYP_DEFINES ?? '';
  if (!args.kerberos_use_rtld) {
    gypDefines += ' kerberos_use_rtld=false';
  }

  gypDefines = gypDefines.trim();
  const prebuildOptions =
    gypDefines.length > 0
      ? { env: { ...process.env, GYP_DEFINES: gypDefines } }
      : undefined;

  await run('npm', ['run', 'prebuild'], prebuildOptions);

  // TODO(NODE-5140): When we have a TS build step
  // await run('npm', ['run', 'prepare']);

  if (process.platform === 'darwin' && process.arch === 'arm64') {
    // The "arm64" build is actually a universal binary
    // @ts-ignore
    const napiVersion = require('../../package.json').binary.napi_versions[0];
    const armTar = `kerberos-v${pkg.version}-napi-v${napiVersion}-darwin-arm64.tar.gz`;
    const x64Tar = `kerberos-v${pkg.version}-napi-v${napiVersion}-darwin-x64.tar.gz`;
    await fs.copyFile(resolveRoot('prebuilds', armTar), resolveRoot('prebuilds', x64Tar));
  }

  await run('node', ['--print', `require('.')`], { cwd: resolveRoot() })
}

async function main() {
  const { pkg, ...args } = await parseArguments();
  console.log(args);
  await buildBindings(args, pkg);
}

await main();
