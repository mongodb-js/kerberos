'use strict';

function validateParameter(parameter, specs, specIndex) {
  const spec = specs[specIndex];
  if (parameter == null && spec.required === false) {
    return;
  }

  if (parameter == null) {
    throw new TypeError(`Required parameter \`${spec.name}\` missing`);
  }

  const paramType = typeof parameter;
  if (spec.type && paramType !== spec.type) {
    if (spec.required === false) {
      if (specs.slice(specIndex).some(def => def.type === paramType)) {
        return false;
      }
    }

    throw new TypeError(
      `Invalid type for parameter \`${spec.name}\`, expected \`${
        spec.type
      }\` but found \`${typeof parameter}\``
    );
  }

  return true;
}

function hasOwnProperty(object, property) {
  return Object.prototype.hasOwnProperty.call(object, property);
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
  return function () {
    const args = Array.prototype.slice.call(arguments);
    const params = [];
    for (let i = 0, argIdx = 0; i < paramDefs.length; ++i, ++argIdx) {
      const def = paramDefs[i];
      let arg = args[argIdx];

      if (hasOwnProperty(def, 'default') && arg == null) arg = def.default;
      if (def.type === 'object' && def.default != null) {
        arg = Object.assign({}, def.default, arg);
      }

      // special case to allow `options` to be optional
      if (def.name === 'options' && (typeof arg === 'function' || arg == null)) {
        arg = {};
      }

      if (validateParameter(arg, paramDefs, i)) {
        params.push(arg);
      } else {
        argIdx--;
      }
    }

    const callback = arguments[arguments.length - 1];
    if (typeof callback !== 'function') {
      return new Promise((resolve, reject) => {
        params.push((err, response) => {
          if (err) return reject(err);
          resolve(response);
        });

        fn.apply(this, params);
      });
    }

    fn.apply(this, params);
  };
}

module.exports = { defineOperation, validateParameter };
