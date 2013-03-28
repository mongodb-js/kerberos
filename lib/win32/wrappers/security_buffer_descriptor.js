var SecurityBufferDescriptorNative = require('../../../build/Release/kerberos').SecurityBufferDescriptor
  , SecurityBuffer = require('./security_buffer').SecurityBuffer;

// exports.SecurityBufferDescriptor = SecurityBufferDescriptorNative;
// var SecurityBufferDescriptor = function(option) {  
//   var data = [];

//   if(typeof option == 'number') {
//     data.push(new SecurityBuffer(SecurityBuffer.TOKEN, option));
//   } else if(Array.isArray(option)) {
//     for(var i = 0; i < option.length; i++) {
//       if(!(option[i] instanceof SecurityBuffer)) {
//         throw new Error("Constructor takes either a buffer size or array of Security Buffers");
//       }
//     }

//     // Set array to option
//     data = option;
//   } else if(!Array.isArray(option)) {
//     throw new Error("Constructor takes either a buffer size or array of Security Buffers");
//   }

//   // Set up descriptor
//   this._native_security_buffer_descriptor = new SecurityBufferDescriptorNative(data);
// }

// // Add some attributes
// SecurityBuffer.VERSION  = 0;
// SecurityBuffer.EMPTY    = 0;
// SecurityBuffer.DATA     = 1;
// SecurityBuffer.TOKEN    = 2;
// SecurityBuffer.PADDING  = 9;
// SecurityBuffer.STREAM   = 10;

// SecurityBuffer.prototype.toBuffer = function() {
//   return this._native_security_buffer.toBuffer();
// }

// Export the modified class
exports.SecurityBufferDescriptor = SecurityBufferDescriptorNative;