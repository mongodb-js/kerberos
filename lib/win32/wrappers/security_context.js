'use strict';

const kerberos = require('bindings')('kerberos');
const SecurityContext = kerberos.SecurityContext;

module.exports = {
  SecurityContext
};
