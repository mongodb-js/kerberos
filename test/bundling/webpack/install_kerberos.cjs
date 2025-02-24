'use strict';

const { execSync } = require('node:child_process');
const { readFileSync } = require('node:fs');
const { resolve } = require('node:path');

const xtrace = (...args) => {
  console.log(`running: ${args[0]}`);
  return execSync(...args);
};

const kerberosRoot = resolve(__dirname, '../../..');
console.log(`kerberos package root: ${kerberosRoot}`);

const kerberosVersion = JSON.parse(
  readFileSync(resolve(kerberosRoot, 'package.json'), { encoding: 'utf8' })
).version;
console.log(`kerberos Version: ${kerberosVersion}`);

xtrace('npm pack --pack-destination test/bundling/webpack', { cwd: kerberosRoot });

xtrace(`npm install --no-save kerberos-${kerberosVersion}.tgz`);

console.log('kerberos installed!');
