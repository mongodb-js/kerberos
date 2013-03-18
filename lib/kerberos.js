var KerberosNative = require('../build/Release/kerberos').Kerberos;

var Kerberos = function() {
  this._native_kerberos = new KerberosNative(); 
}

Kerberos.prototype.authGSSClientInit = function(uri, flags, callback) {
  return this._native_kerberos.authGSSClientInit(uri, flags, callback);
}

// Some useful result codes
Kerberos.AUTH_GSS_CONTINUE     = 0;
Kerberos.AUTH_GSS_COMPLETE     = 1;
     
// Some useful gss flags 
Kerberos.GSS_C_DELEG_FLAG      = 1;
Kerberos.GSS_C_MUTUAL_FLAG     = 2;
Kerberos.GSS_C_REPLAY_FLAG     = 4;
Kerberos.GSS_C_SEQUENCE_FLAG   = 8;
Kerberos.GSS_C_CONF_FLAG       = 16; 
Kerberos.GSS_C_INTEG_FLAG      = 32;
Kerberos.GSS_C_ANON_FLAG       = 64;
Kerberos.GSS_C_PROT_READY_FLAG = 128; 
Kerberos.GSS_C_TRANS_FLAG      = 256;

exports.Kerberos = Kerberos;