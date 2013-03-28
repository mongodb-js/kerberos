console.log("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")

var SecurityCredentialsNative = require('../../../build/Release/kerberos').SecurityCredentials;
// Add simple kebros helper
SecurityCredentialsNative.aquire_kerberos = function(username, password, domain) {
  return SecurityCredentialsNative.aquire('Kerberos', username, password, domain);
}

console.dir(SecurityCredentialsNative)

// Export the modified class
exports.SecurityCredentials = SecurityCredentialsNative;