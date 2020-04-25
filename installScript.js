'use strict';
const os = require('os');
const execSync = require('child_process').execSync;
const platformDevDependencies = require('./package.json').platformDevDependencies;

// Install platform specific devDependencies using custom key "platformDevDependencies"
// in package.json
if (process.env.NODE_ENV !== 'production') {
    try {
        require('prettier');
        Object.keys(platformDevDependencies).forEach(d => {
            const { version, targets } = platformDevDependencies[d];
            if (targets.includes(os.type())) {
                execSync(`npm install --no-save ${d}@${version} > /dev/null`, { stdio: 'inherit' });
            }
        });
    } catch (e) {}
}
execSync("prebuild-install || node-gyp rebuild", { stdio: 'inherit' });
