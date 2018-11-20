'use strict';

const kerberos = require('bindings')('kerberos');
const KerberosClient = kerberos.KerberosClient;
const KerberosServer = kerberos.KerberosServer;
const defineOperation = require('./util').defineOperation;

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

/**
 * @class KerberosClient
 *
 * @property {string} username The username used for authentication
 * @property {string} response The last response received during authentication steps
 * @property {string} responseConf Indicates whether confidentiality was applied or not (GSSAPI only)
 * @property {boolean} contextComplete Indicates that authentication has successfully completed or not
 */

/**
 * Processes a single kerberos client-side step using the supplied server challenge.
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data (which may be empty for the first step)
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
KerberosClient.prototype.step = defineOperation(KerberosClient.prototype.step, [
  { name: 'challenge', type: 'string' },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * Perform the client side kerberos wrap step.
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge The response returned after calling `unwrap`
 * @param {object} [options] Optional settings
 * @param {string} [options.user] The user to authorize
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
KerberosClient.prototype.wrap = defineOperation(KerberosClient.prototype.wrap, [
  { name: 'challenge', type: 'string' },
  { name: 'options', type: 'object' },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * Perform the client side kerberos unwrap step
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
KerberosClient.prototype.unwrap = defineOperation(KerberosClient.prototype.unwrap, [
  { name: 'challenge', type: 'string' },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * @class KerberosServer
 *
 * @property {string} username The username used for authentication
 * @property {string} response The last response received during authentication steps
 * @property {string} targetName The target used for authentication
 * @property {boolean} contextComplete Indicates that authentication has successfully completed or not
 */

/**
 * Processes a single kerberos server-side step using the supplied client data.
 *
 * @kind function
 * @memberof KerberosServer
 * @param {string} challenge A string containing the base64-encoded client data
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
KerberosServer.prototype.step = defineOperation(KerberosServer.prototype.step, [
  { name: 'challenge', type: 'string' },
  { name: 'callback', type: 'function', required: false }
]);

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
 * only be used for testing. Do not use this in any production system - your
 * security could be compromised if you do.
 *
 * @kind function
 * @param {string} username The Kerberos user name. If no realm is supplied, then the `defaultRealm` will be used.
 * @param {string} password The password for the user.
 * @param {string} service The Kerberos service to check access for.
 * @param {string} [defaultRealm] The default realm to use if one is not supplied in the user argument.
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
const checkPassword = defineOperation(kerberos.checkPassword, [
  { name: 'username', type: 'string' },
  { name: 'password', type: 'string' },
  { name: 'service', type: 'string' },
  { name: 'defaultRealm', type: 'string', required: false },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * This function returns the service principal for the server given a service type and hostname.
 *
 * Details are looked up via the `/etc/keytab` file.
 *
 * @kind function
 * @param {string} service The Kerberos service type for the server.
 * @param {string} hostname The hostname of the server.
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
const principalDetails = defineOperation(kerberos.principalDetails, [
  { name: 'service', type: 'string' },
  { name: 'hostname', type: 'string' },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * Initializes a context for client-side authentication with the given service principal.
 *
 * @kind function
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @param {object} [options] Optional settings
 * @param {string} [options.principal] Optional string containing the client principal in the form 'user@realm' (e.g. 'jdoe@example.com').
 * @param {number} [options.gssFlags] Optional integer used to set GSS flags. (e.g.  GSS_C_DELEG_FLAG|GSS_C_MUTUAL_FLAG|GSS_C_SEQUENCE_FLAG will allow for forwarding credentials to the remote host)
 * @param {number} [options.mechOID] Optional GSS mech OID. Defaults to None (GSS_C_NO_OID). Other possible values are `GSS_MECH_OID_KRB5`, `GSS_MECH_OID_SPNEGO`.
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
const initializeClient = defineOperation(kerberos.initializeClient, [
  { name: 'service', type: 'string' },
  { name: 'options', type: 'object', default: { mechOID: GSS_C_NO_OID } },
  { name: 'callback', type: 'function', required: false }
]);

/**
 * Initializes a context for server-side authentication with the given service principal.
 *
 * @kind function
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @param {function} [callback]
 * @return {Promise} returns Promise if no callback passed
 */
const initializeServer = defineOperation(kerberos.initializeServer, [
  { name: 'service', type: 'string' },
  { name: 'callback', type: 'function', required: false }
]);

module.exports = {
  initializeClient,
  initializeServer,
  principalDetails,
  checkPassword,

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
