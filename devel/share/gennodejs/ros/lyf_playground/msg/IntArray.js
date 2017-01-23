// Auto-generated. Do not edit!

// (in-package lyf_playground.msg)


"use strict";

let _serializer = require('../base_serialize.js');
let _deserializer = require('../base_deserialize.js');
let _finder = require('../find.js');

//-----------------------------------------------------------

class IntArray {
  constructor() {
    this.data = [];
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type IntArray
    // Serialize the length for message field [data]
    bufferInfo = _serializer.uint32(obj.data.length, bufferInfo);
    // Serialize message field [data]
    obj.data.forEach((val) => {
      bufferInfo = _serializer.int32(val, bufferInfo);
    });
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type IntArray
    let tmp;
    let len;
    let data = new IntArray();
    // Deserialize array length for message field [data]
    tmp = _deserializer.uint32(buffer);
    len = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [data]
    data.data = new Array(len);
    for (let i = 0; i < len; ++i) {
      tmp = _deserializer.int32(buffer);
      data.data[i] = tmp.data;
      buffer = tmp.buffer;
    }
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a message object
    return 'lyf_playground/IntArray';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '563b27884d008b0d2adff54dc1f9e4f5';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32[] data
    
    `;
  }

};

module.exports = IntArray;
