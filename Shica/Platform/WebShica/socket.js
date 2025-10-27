mergeInto(LibraryManager.library, {
    sendWebSocketMessage: function(ptr, len) {
        wsRef.current?.send(UTF8ToString(ptr, len));
    }
});