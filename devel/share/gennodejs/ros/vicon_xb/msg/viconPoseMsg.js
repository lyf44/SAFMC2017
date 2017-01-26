// Auto-generated. Do not edit!

// (in-package vicon_xb.msg)


"use strict";

let _serializer = require('../base_serialize.js');
let _deserializer = require('../base_deserialize.js');
let _finder = require('../find.js');

//-----------------------------------------------------------

class viconPoseMsg {
  constructor() {
    this.time_stamp = {secs: 0, nsecs: 0};
    this.x = 0.0;
    this.y = 0.0;
    this.z = 0.0;
    this.dx = 0.0;
    this.dy = 0.0;
    this.dz = 0.0;
    this.roll = 0.0;
    this.pitch = 0.0;
    this.yaw = 0.0;
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type viconPoseMsg
    // Serialize message field [time_stamp]
    bufferInfo = _serializer.time(obj.time_stamp, bufferInfo);
    // Serialize message field [x]
    bufferInfo = _serializer.float32(obj.x, bufferInfo);
    // Serialize message field [y]
    bufferInfo = _serializer.float32(obj.y, bufferInfo);
    // Serialize message field [z]
    bufferInfo = _serializer.float32(obj.z, bufferInfo);
    // Serialize message field [dx]
    bufferInfo = _serializer.float32(obj.dx, bufferInfo);
    // Serialize message field [dy]
    bufferInfo = _serializer.float32(obj.dy, bufferInfo);
    // Serialize message field [dz]
    bufferInfo = _serializer.float32(obj.dz, bufferInfo);
    // Serialize message field [roll]
    bufferInfo = _serializer.float32(obj.roll, bufferInfo);
    // Serialize message field [pitch]
    bufferInfo = _serializer.float32(obj.pitch, bufferInfo);
    // Serialize message field [yaw]
    bufferInfo = _serializer.float32(obj.yaw, bufferInfo);
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type viconPoseMsg
    let tmp;
    let len;
    let data = new viconPoseMsg();
    // Deserialize message field [time_stamp]
    tmp = _deserializer.time(buffer);
    data.time_stamp = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [x]
    tmp = _deserializer.float32(buffer);
    data.x = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [y]
    tmp = _deserializer.float32(buffer);
    data.y = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [z]
    tmp = _deserializer.float32(buffer);
    data.z = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [dx]
    tmp = _deserializer.float32(buffer);
    data.dx = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [dy]
    tmp = _deserializer.float32(buffer);
    data.dy = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [dz]
    tmp = _deserializer.float32(buffer);
    data.dz = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [roll]
    tmp = _deserializer.float32(buffer);
    data.roll = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [pitch]
    tmp = _deserializer.float32(buffer);
    data.pitch = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [yaw]
    tmp = _deserializer.float32(buffer);
    data.yaw = tmp.data;
    buffer = tmp.buffer;
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a message object
    return 'vicon_xb/viconPoseMsg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'f87f226dd5b11ef67fbde47b43eb429e';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    time time_stamp
    float32 x
    float32 y
    float32 z
    float32 dx
    float32 dy
    float32 dz
    float32 roll
    float32 pitch
    float32 yaw
    
    `;
  }

};

module.exports = viconPoseMsg;
