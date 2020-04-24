'use strict';
const os = require('os');
const exec = require('child_process').exec;
const execSync = require('child_process').execSync;
const platformDevDependencies = require('./package.json').platformDevDependencies;

if (process.env.NODE_ENV !== 'production') {
    try {
        require('prettier');
        Objects.keys(platformDevDependencies).forEach(d => {
            const { version, targets } = platformDevDependencies[d];
            if (targets.includes(os.type())) {
                execSync(`npm install --no-save ${d}@${version}`, { stdio: 'inherit' });
            }
        });
    } catch (e) {}
}
execSync("prebuild-install || node-gyp rebuild", { stdio: 'inherit' });
