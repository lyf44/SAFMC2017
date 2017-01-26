// Auto-generated. Do not edit!

// (in-package vicon_xb.srv)


"use strict";

let _serializer = require('../base_serialize.js');
let _deserializer = require('../base_deserialize.js');
let _finder = require('../find.js');

//-----------------------------------------------------------


//-----------------------------------------------------------

class viconXbSrvRequest {
  constructor() {
    this.block = false;
    this.logfile_idx = 0;
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type viconXbSrvRequest
    // Serialize message field [block]
    bufferInfo = _serializer.bool(obj.block, bufferInfo);
    // Serialize message field [logfile_idx]
    bufferInfo = _serializer.int32(obj.logfile_idx, bufferInfo);
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type viconXbSrvRequest
    let tmp;
    let len;
    let data = new viconXbSrvRequest();
    // Deserialize message field [block]
    tmp = _deserializer.bool(buffer);
    data.block = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [logfile_idx]
    tmp = _deserializer.int32(buffer);
    data.logfile_idx = tmp.data;
    buffer = tmp.buffer;
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a service object
    return 'vicon_xb/viconXbSrvRequest';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'ed681b93b567be83d5e7ee5788d064f3';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    bool block
    int32 logfile_idx
    
    `;
  }

};

class viconXbSrvResponse {
  constructor() {
    this.result = 0;
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type viconXbSrvResponse
    // Serialize message field [result]
    bufferInfo = _serializer.uint8(obj.result, bufferInfo);
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type viconXbSrvResponse
    let tmp;
    let len;
    let data = new viconXbSrvResponse();
    // Deserialize message field [result]
    tmp = _deserializer.uint8(buffer);
    data.result = tmp.data;
    buffer = tmp.buffer;
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a service object
    return 'vicon_xb/viconXbSrvResponse';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '25458147911545c320c4c0a299eff763';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint8 result
    
    
    `;
  }

};

module.exports = {
  Request: viconXbSrvRequest,
  Response: viconXbSrvResponse
};
