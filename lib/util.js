'use strict';

function validateParameter(parameter, spec) {
  if (parameter == null && spec.required === false) {
    return;
  }

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
 * Monkey-patches an existing method to support parameter validation, as well
 * as adding support for returning Promises if callbacks are not provided.
 *
 * @private
 * @param {function} fn the function to override
 * @param {Array<Object>} paramDefs the definitions of each parameter to the function
 */
function defineOperation(fn, paramDefs) {
  return function() {
    const args = Array.prototype.slice.call(arguments);
    const params = [];
    for (let i = 0; i < paramDefs.length; ++i) {
      const def = paramDefs[i];
      let arg = args[i];

      if (def.default && arg == null) arg = def.default;
      if (def.type === 'object' && def.default != null) {
        arg = Object.assign({}, def.default, arg);
      }

      // special case to allow `options` to be optional
      if (def.name === 'options' && (typeof arg === 'function' || arg == null)) {
        arg = {};
      }

      validateParameter(arg, paramDefs[i]);
      params.push(arg);
    }

    const callback = params.pop();
    if (typeof callback !== 'function') {
      return new Promise((resolve, reject) => {
        params.push((err, response) => {
          if (err) return reject(err);
          resolve(response);
        });

        fn.apply(this, params);
      });
    }

    params.push(callback);
    fn.apply(this, params);
  };
}

module.exports = { defineOperation, validateParameter };
