/*
  you can call JS functions from C code here.
  remember to declare them in shica-lib.js and library.c
*/
globalThis._addWebRtcBroadcast = function(index, channel, password, ptr) {
  console.log("Adding WebRTC Broadcast (global):", index, channel, password, ptr);
};

globalThis._sendWebRtcBroadcast = function(index, channel, msg) {
  console.log("Sending WebRTC Broadcast (global):", index, channel, msg);
};

mergeInto(LibraryManager.library, {

    //WebRTC broadcast Event Object
  _web_rtc_broadcast_eo_: function (index, channelPtr, passwordPtr, ptr) {
    const channel = UTF8ToString(channelPtr);
    const password = UTF8ToString(passwordPtr);
    console.log("WebRTC Broadcast Event Object:", index, channel, password, ptr);
    _addWebRtcBroadcast(index, channel, password, ptr);
    return 0;
  },
  _lib_web_rtc_broadcast_send_: function( index, channelPtr, msgPtr) {
    const channel = UTF8ToString(channelPtr);
    const msg = UTF8ToString(msgPtr);
    
    _sendWebRtcBroadcast(index, channel, msg);
    return 0; 
  }

});