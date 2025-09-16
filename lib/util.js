'use strict';

function loadBindings() {
  try {
    return require('../build/Release/kerberos.node');
  } catch {
    // Webpack will fail when just returning the require, so we need to wrap
    // in a try/catch and rethrow.
    /* eslint no-useless-catch: 0 */
    try {
      return require('../build/Debug/kerberos.node');
    } catch (error) {
      throw error;
    }
  }
}

module.exports = { loadBindings };
