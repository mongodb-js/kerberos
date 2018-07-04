'use strict';

const kerberos = require('bindings')('kerberos');

// Result Codes
const AUTH_GSS_CONTINUE = 0;
const AUTH_GSS_COMPLETE = 1;

// GSS Flags
const GSS_C_DELEG_FLAG = 1;
const GSS_C_MUTUAL_FLAG = 2;
const GSS_C_REPLAY_FLAG = 4;
const GSS_C_SEQUENCE_FLAG = 8;
const GSS_C_CONF_FLAG = 16;
const GSS_C_INTEG_FLAG = 32;
const GSS_C_ANON_FLAG = 64;
const GSS_C_PROT_READY_FLAG = 128;
const GSS_C_TRANS_FLAG = 256;

// GSS_OID
const GSS_C_NO_OID = 0;
const GSS_MECH_OID_KRB5 = 9;
const GSS_MECH_OID_SPNEGO = 6;

function validateParameter(parameter, spec) {
  if (parameter == null) {
    throw new TypeError(`Required parameter \`${spec.name}\` missing`);
  }

  if (spec.type && typeof parameter !== spec.type) {
    throw new TypeError(
      `Invalid type for parameter \`${spec.name}\`, expected \`${
        spec.type
      }\` but found \`${typeof parameter}\``
    );
  }
}

/**
 * This function provides a simple way to verify that a user name and password
 * match those normally used for Kerberos authentication.
 * It does this by checking that the supplied user name and password can be
 * used to get a ticket for the supplied service.
 * If the user name does not contain a realm, then the default realm supplied
 * is used.
 *
 * For this to work properly the Kerberos must be configured properly on this
 * machine.
 * That will likely mean ensuring that the edu.mit.Kerberos preference file
 * has the correct realms and KDCs listed.
 *
 * IMPORTANT: This method is vulnerable to KDC spoofing attacks and it should
 * only used for testing. Do not use this in any production system - your
 * security could be compromised if you do.
 *
 * @param {string} username The Kerberos user name. If no realm is supplied, then the `defaultRealm` will be used.
 * @param {string} password The password for the user.
 * @param {string} service The Kerberos service to check access for.
 * @param {string} [defaultRealm] The default realm to use if one is not supplied in the user argument.
 * @param {function} callback
 */
function checkPassword(username, password, service, defaultRealm, callback) {
  if (typeof defaultRealm === 'function') (callback = defaultRealm), (defaultRealm = null);
  validateParameter(username, { name: 'service', type: 'string' });
  validateParameter(password, { name: 'options', type: 'string' });
  validateParameter(service, { name: 'options', type: 'string' });
  validateParameter(defaultRealm, { name: 'options', type: 'string' });
  validateParameter(callback, { name: 'callback', type: 'function' });

  kerberos.checkPassword(username, password, service, defaultRealm, callback);
}

/**
 * This function returns the service principal for the server given a service
 * type and hostname.
 *
 * Details are looked up via the `/etc/keytab` file.
 *
 * @param {string} service The Kerberos service type for the server.
 * @param {string} hostname The hostname of the server.
 * @param {function} callback
 */
function serverPrincipalDetails(service, hostname, callback) {
  validateParameter(service, { name: 'service', type: 'string' });
  validateParameter(hostname, { name: 'options', type: 'string' });
  validateParameter(callback, { name: 'callback', type: 'function' });

  kerberos.serverPrincipalDetails(service, hostname, callback);
}

/**
 * The callback format for inserts
 * @callback authGSSClientInitCallback
 * @param {Error} error An error instance representing the error during the execution.
 * @param {Array<Object>} result The result object if the command was executed successfully.
 */

/**
 * Initializes a context for GSSAPI client-side authentication with the
 * given service principal. `authGSSClientClean` must be called after this
 * function returns an OK result to dispose of the context once all GSSAPI
 * operations are complete.
 *
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @param {object} [options] Optional settings
 * @param {string} [options.principal] Optional string containing the client principal in the form 'user@realm' (e.g. 'jdoe@example.com').
 * @param {number} [options.gssFlags] Optional integer used to set GSS flags. (e.g.  GSS_C_DELEG_FLAG|GSS_C_MUTUAL_FLAG|GSS_C_SEQUENCE_FLAG will allow for forwarding credentials to the remote host)
 * @param {number} [options.mechOID] Optional GSS mech OID. Defaults to None (GSS_C_NO_OID). Other possible values are GSS_MECH_OID_KRB5, GSS_MECH_OID_SPNEGO.
 * @param {initializeClientCallback} callback The operation callback
 */
function initializeClient(service, options, callback) {
  if (typeof options === 'function') (callback = options), (options = {});
  options = Object.assign({}, { mechOID: GSS_C_NO_OID }, options);

  validateParameter(service, { name: 'service', type: 'string' });
  validateParameter(options, { name: 'options', type: 'object' });
  validateParameter(callback, { name: 'callback', type: 'function' });

  kerberos.initializeClient(service, options, callback);
}

/**
 * Initializes a context for GSSAPI server-side authentication with the given
 * service principal. `authGSSServerClean` must be called after this function
 * returns an OK result to dispose of the context once all GSSAPI operations
 * are complete.
 *
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @param {initializeServerCallback} callback
 */
function initializeServer(service, callback) {
  validateParameter(service, { name: 'service', type: 'string' });
  validateParameter(callback, { name: 'callback', type: 'function' });

  kerberos.initializeServer(service, callback);
}

/**
 * Processes a single GSSAPI client-side step using the supplied server data.
 *
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data (which may be empty for the first step)
 * @param {function} callback Returns a result code, or an error if one was encountered
 */

/**
 * Perform the client side GSSAPI unwrap step
 *
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data
 * @param {function} callback
 */

/**
 * Perform the client side GSSAPI wrap step.
 *
 * @memberof KerberosClient
 * @param {string} challenge The result of the `authGSSClientResponse` after the `authGSSClientUnwrap`
 * @param {object} [options] Optional settings
 * @param {string} [options.user] The user to authorize
 * @param {function} callback
 */

/**
 * Processes a single GSSAPI server-side step using the supplied client data.
 *
 * @memberof KerberosServer
 * @param {string} challenge A string containing the base64-encoded client data
 * @param {function} callback
 */

module.exports = {
  initializeClient,
  initializeServer,
  serverPrincipalDetails,
  checkPassword,

  // result codes
  AUTH_GSS_CONTINUE,
  AUTH_GSS_COMPLETE,

  // gss flags
  GSS_C_DELEG_FLAG,
  GSS_C_MUTUAL_FLAG,
  GSS_C_REPLAY_FLAG,
  GSS_C_SEQUENCE_FLAG,
  GSS_C_CONF_FLAG,
  GSS_C_INTEG_FLAG,
  GSS_C_ANON_FLAG,
  GSS_C_PROT_READY_FLAG,
  GSS_C_TRANS_FLAG,
  GSS_C_NO_OID,

  // mechanism OIDs
  GSS_MECH_OID_KRB5,
  GSS_MECH_OID_SPNEGO
};
