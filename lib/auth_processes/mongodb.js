var format = require('util').format;

var MongoAuthProcess = function(host, port) {  
  // Check what system we are on
  if(process.platform == 'win32') {
    this._processor = new Win32MongoProcessor(host, port);
  } else {
    this._processor = new UnixMongoProcessor(host, port);
  }
}

MongoAuthProcess.prototype.init = function(username, password, callback) {
  this._processor.init(username, password, callback);
}

MongoAuthProcess.prototype.transition = function(payload, callback) {
  this._processor.transition(payload, callback);
}

/*******************************************************************
 *
 * Win32 SSIP Processor for MongoDB
 *
 *******************************************************************/
var Win32MongoProcessor = function(host, port) {
  this.host = host;
  this.port = port  
  // SSIP classes
  this.ssip = require("../kerberos").SSIP;
  // Set up first transition
  this._transition = Win32MongoProcessor.first_transition(this);
  // Set up target
  this.target = format("mongodb/%s", host);
}

Win32MongoProcessor.prototype.init = function(username, password, callback) {
  var self = this;
  // Save the values used later
  this.username = username;
  this.password = password;
  // Aquire credentials
  this.ssip.SecurityCredentials.aquire_kerberos(username, password, function(err, security_credentials) {
    if(err) return callback(err);
    // Save credentials
    self.security_credentials = security_credentials;
    // Callback with success
    callback(null);
  });
}

Win32MongoProcessor.prototype.transition = function(payload, callback) {
  this._transition(payload, callback);
}

Win32MongoProcessor.first_transition = function(self) {
  return function(payload, callback) {    
    self.ssip.SecurityContext.initialize(
      self.security_credentials, 
      self.target, 
      payload, function(err, security_context) {   
        if(err) return callback(err);
        // Set next transition
        self._transition = Win32MongoProcessor.second_transition(self);
        self.security_context = security_context;
        // Return the payload
        callback(null, security_context.payload);
    });
  }
}

Win32MongoProcessor.second_transition = function(self) {
  return function(payload, callback) {    
    // Perform a step
    self.security_context.initialize(self.target, payload, function(err, security_context) {
      if(err) return callback(err);
      // Set next transition
      self._transition = Win32MongoProcessor.third_transition(self);
      // Return the payload
      callback(null, security_context.payload);
    });
  }  
}

Win32MongoProcessor.third_transition = function(self) {
  return function(payload, callback) {   
    var messageLength = 0;
    // Get the raw bytes
    var encryptedBytes = new Buffer(payload, 'base64');
    var encryptedMessage = new Buffer(messageLength);
    // Copy first byte
    encryptedBytes.copy(encryptedMessage, 0, 0, messageLength);
    // Set up trailer
    var securityTrailerLength = encryptedBytes.length - messageLength;
    var securityTrailer = new Buffer(securityTrailerLength);
    // Copy the bytes
    encryptedBytes.copy(securityTrailer, 0, messageLength, securityTrailerLength);

    // Types used
    var SecurityBuffer = self.ssip.SecurityBuffer;
    var SecurityBufferDescriptor = self.ssip.SecurityBufferDescriptor;

    // Set up security buffers
    var buffers = [
        new SecurityBuffer(SecurityBuffer.DATA, encryptedBytes)
      , new SecurityBuffer(SecurityBuffer.STREAM, securityTrailer)
    ];

    // Set up the descriptor
    var descriptor = new SecurityBufferDescriptor(buffers);

    // Decrypt the data
    self.security_context.decryptMessage(descriptor, function(err, security_context) {
      if(err) return callback(err);

      var length = 4;
      if(self.username != null) {
        length += self.username.length;          
      }

      var bytesReceivedFromServer = new Buffer(length);
      bytesReceivedFromServer[0] = 0x01;  // NO_PROTECTION
      bytesReceivedFromServer[1] = 0x00;  // NO_PROTECTION
      bytesReceivedFromServer[2] = 0x00;  // NO_PROTECTION
      bytesReceivedFromServer[3] = 0x00;  // NO_PROTECTION        

      if(self.username != null) {
        var authorization_id_bytes = new Buffer(self.username, 'utf8');
        authorization_id_bytes.copy(bytesReceivedFromServer, 4, 0);
      }

      self.security_context.queryContextAttributes(0x00, function(err, sizes) {
        if(err) return callback(err);

        var buffers = [
            new SecurityBuffer(SecurityBuffer.TOKEN, new Buffer(sizes.securityTrailer))
          , new SecurityBuffer(SecurityBuffer.DATA, bytesReceivedFromServer)
          , new SecurityBuffer(SecurityBuffer.PADDING, new Buffer(sizes.blockSize))
        ]

        var descriptor = new SecurityBufferDescriptor(buffers);

        self.security_context.encryptMessage(descriptor, 0x80000001, function(err, security_context) {
          if(err) return callback(err);
          callback(null, security_context.payload);
        });
      });
    });
  }  
}

/*******************************************************************
 *
 * UNIX MIT Kerberos processor
 *
 *******************************************************************/
var UnixMongoProcessor = function() {  
}

// Set the process
exports.MongoAuthProcess = MongoAuthProcess;