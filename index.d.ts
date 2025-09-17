export const GSS_C_DELEG_FLAG: number;
export const GSS_C_MUTUAL_FLAG: number;
export const GSS_C_REPLAY_FLAG: number;
export const GSS_C_SEQUENCE_FLAG: number;
export const GSS_C_CONF_FLAG: number;
export const GSS_C_INTEG_FLAG: number;
export const GSS_C_ANON_FLAG: number;
export const GSS_C_PROT_READY_FLAG: number;
export const GSS_C_TRANS_FLAG: number;

export const GSS_C_NO_OID: number;
export const GSS_MECH_OID_KRB5: number;
export const GSS_MECH_OID_SPNEGO: number;

/**
 * Optional settings for *KerberosClient.wrap* method.
 */
export interface WrapOptions {
  /**
   * The user to authorize.
   */
  user?: string;

  /**
   * Indicates if the wrap should request message confidentiality.
   */
  protect?: boolean;
}

/**
 * Options for `initializeClient()`.
 */
export interface InitializeClientOptions {
  /**
   * Optional string containing the client principal in the form 'user@realm' (e.g. 'jdoe@example.com').
   */
  principal?: string;
  /**
   * Optional integer used to set GSS flags. (e.g.  `GSS_C_DELEG_FLAG|GSS_C_MUTUAL_FLAG|GSS_C_SEQUENCE_FLAG` will allow for forwarding credentials to the remote host).
   */
  gssFlag?: number;
  /**
   * Optional GSS mech OID. Defaults to None (GSS_C_NO_OID). Other possible values are `GSS_MECH_OID_KRB5`, `GSS_MECH_OID_SPNEGO`.
   */
  mechOID?: number;

  /**
   * The username with which to authenticate.  Only used on Windows.
   */
  user?: string;

  /**
   * The password with which to authenticate.  Only used on Windows.
   */
  pass?: string;
}

export class KerberosClient {
  /**
   * The username used for authentication.
   */
  username: string;
  /**
   * The last response received during authentication steps.
   */
  response: string;
  /**
   * Indicates whether confidentiality was applied or not (GSSAPI only).
   */
  responseConf: string;
  /**
   * Indicates that authentication has successfully completed or not.
   */
  contextComplete: boolean;

  /**
   * Processes a single kerberos client-side step using the supplied server challenge.
   *
   * @param challenge A string containing the base64-encoded server data (which may be empty for the first step)
   */
  step(challenge: string): Promise<string>;

  /**
   * Perform the client side kerberos wrap step.
   *
   * @param challenge The response returned after calling `unwrap`
   * @param options Optional settings
   */
  wrap(challenge: string, options?: WrapOptions): Promise<string>;

  /**
   * Perform the client side kerberos unwrap step
   *
   * @param challenge A string containing the base64-encoded server data
   */
  unwrap(challenge: string): Promise<string>;
}

export class KerberosServer {
  /**
   * The username used for authentication
   */
  username: string;
  /**
   * @description The last response received during authentication steps
   */
  response: string;
  /**
   * @description The target used for authentication
   */
  targetName: string;
  /**
   * @description Indicates that authentication has successfully completed or not
   */
  contextComplete: boolean;

  /**
   * Processes a single kerberos server-side step using the supplied client data.
   *
   * @param challenge A string containing the base64-encoded client data
   */
  step(challenge: string): Promise<string>;
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
 * only be used for testing. Do not use this in any production system - your
 * security could be compromised if you do.
 *
 * @param username The Kerberos user name. If no realm is supplied, then the `defaultRealm` will be used.
 * @param password The password for the user.
 * @param service The Kerberos service to check access for.
 * @param defaultRealm The default realm to use if one is not supplied in the user argument.
 */
export function checkPassword(
  name: string,
  password: string,
  service: string,
  defaultRealm?: string
): Promise<void>;

/**
 * This function returns the service principal for the server given a service type and hostname.
 *
 * Details are looked up via the `/etc/keytab` file.
 *
 * @param service The Kerberos service type for the server.
 * @param hostname The hostname of the server.
 */
export function principalDetails(service: string, hostname: string): Promise<string>;

/**
 * Initializes a context for client-side authentication with the given service principal.
 *
 * @param service A string containing the service principal in the form '`type@fqdn`'.
 * @param [options] Optional settings
 */
export function initializeClient(
  service: string,
  options?: InitializeClientOptions
): Promise<KerberosClient>;

/**
 * Initializes a context for server-side authentication with the given service principal.
 *
 * @param service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 */
export function initializeServer(service: string): Promise<KerberosServer>;

/**
 * The version of the Kerberos package.
 */
export const version: string;
