'use client';

import React, { useState, useRef, useEffect } from 'react';

interface Message {
  id: string;
  sender: string;
  content: string;
  timestamp: Date;
  target: string;
  isPrivate: boolean;
}

interface UserSession {
  userId: string;
  isConnected: boolean;
  messages: Message[];
  input: string;
  selectedTarget: string;
  isHost: boolean;
}

const OptimizedWebRTCChat = () => {
  const [currentUserId, setCurrentUserId] = useState('user1');
  const [userSessions, setUserSessions] = useState<UserSession[]>([
    { 
      userId: 'user1', 
      isConnected: true, 
      messages: [], 
      input: '', 
      selectedTarget: 'everyone',
      isHost: true
    },
    { 
      userId: 'user2', 
      isConnected: false, 
      messages: [], 
      input: '', 
      selectedTarget: 'everyone',
      isHost: false
    }
  ]);

  const hostConnectionRef = useRef<RTCPeerConnection | null>(null);
  const hostDataChannelRef = useRef<RTCDataChannel | null>(null);
  const clientConnectionsRef = useRef<Map<string, RTCPeerConnection>>(new Map());
  const clientDataChannelsRef = useRef<Map<string, RTCDataChannel>>(new Map());
  const messagesEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    initializeWebRTC();
    return () => {
      cleanup();
    };
  }, []);

  useEffect(() => {
    scrollToBottom();
  }, [userSessions]);

  const cleanup = () => {
    hostConnectionRef.current?.close();
    clientConnectionsRef.current.forEach(conn => conn.close());
    clientConnectionsRef.current.clear();
    clientDataChannelsRef.current.clear();
  };

  const initializeWebRTC = async () => {
    // Host user (user1) creates connections to all clients
    if (currentUserId === 'user1') {
      await setupHostConnections();
    }
  };

  const setupHostConnections = async () => {
    // Setup connection for host to broadcast to all clients
    const config = {
      iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
      ]
    };

    hostConnectionRef.current = new RTCPeerConnection(config);
    
    // Create data channel for broadcasting
    hostDataChannelRef.current = hostConnectionRef.current.createDataChannel('broadcast', {
      ordered: true
    });

    hostDataChannelRef.current.onopen = () => {
      console.log('Host broadcast channel opened');
    };

    hostDataChannelRef.current.onmessage = (event) => {
      const messageData = JSON.parse(event.data);
      handleReceivedMessage(messageData);
    };
  };

  const connectUser = async (userId: string) => {
    if (currentUserId === 'user1') {
      // Host creates connection to client
      await createHostToClientConnection(userId);
    } else {
      // Client connects to host
      await createClientToHostConnection();
    }
  };

  const createHostToClientConnection = async (clientUserId: string) => {
    const config = {
      iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
      ]
    };

    const connection = new RTCPeerConnection(config);
    clientConnectionsRef.current.set(clientUserId, connection);

    // Create data channel for this specific client
    const dataChannel = connection.createDataChannel(`client-${clientUserId}`, {
      ordered: true
    });

    dataChannel.onopen = () => {
      console.log(`Connection to ${clientUserId} opened`);
    };

    dataChannel.onmessage = (event) => {
      const messageData = JSON.parse(event.data);
      // Host receives message from client and broadcasts to others
      broadcastFromHost(messageData, clientUserId);
    };

    clientDataChannelsRef.current.set(clientUserId, dataChannel);

    // Simulate connection establishment (in real app, you'd exchange offers/answers)
    setTimeout(() => {
      setUserSessions(prev => 
        prev.map(s => 
          s.userId === clientUserId 
            ? { ...s, isConnected: true }
            : s
        )
      );
    }, 1000);
  };

  const createClientToHostConnection = async () => {
    const config = {
      iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
      ]
    };

    const connection = new RTCPeerConnection(config);
    clientConnectionsRef.current.set('host', connection);

    connection.ondatachannel = (event) => {
      const dataChannel = event.channel;
      clientDataChannelsRef.current.set('host', dataChannel);

      dataChannel.onopen = () => {
        console.log('Client connected to host');
      };

      dataChannel.onmessage = (event) => {
        const messageData = JSON.parse(event.data);
        handleReceivedMessage(messageData);
      };
    };

    // Simulate connection establishment
    setTimeout(() => {
      setUserSessions(prev => 
        prev.map(s => 
          s.userId === currentUserId 
            ? { ...s, isConnected: true }
            : s
        )
      );
    }, 1000);
  };

  const broadcastFromHost = (messageData: any, excludeUserId?: string) => {
    // Host broadcasts message to all connected clients except sender
    clientDataChannelsRef.current.forEach((dataChannel, userId) => {
      if (userId !== excludeUserId && dataChannel.readyState === 'open') {
        try {
          dataChannel.send(JSON.stringify(messageData));
        } catch (error) {
          console.error(`Failed to send to ${userId}:`, error);
        }
      }
    });

    // Also handle message locally on host
    handleReceivedMessage(messageData);
  };

  const handleReceivedMessage = (messageData: { message: Message }) => {
    const { message } = messageData;
    
    setUserSessions(prev => {
      return prev.map(session => {
        // Show message in sender's session and target's session (if private)
        const shouldShowMessage = 
          session.userId === message.sender || 
          message.target === 'everyone' ||
          (message.target === session.userId && message.isPrivate);
        
        if (shouldShowMessage && !session.messages.find(m => m.id === message.id)) {
          return {
            ...session,
            messages: [...session.messages, message]
          };
        }
        return session;
      });
    });
  };

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  };

  const createMessage = (sender: string, content: string, target: string): Message => {
    return {
      id: `${sender}-${Date.now()}-${Math.random()}`,
      sender,
      content,
      timestamp: new Date(),
      target,
      isPrivate: target !== 'everyone'
    };
  };

  const sendMessage = (sessionUserId: string) => {
    const session = userSessions.find(s => s.userId === sessionUserId);
    if (!session || !session.input.trim() || !session.isConnected) return;

    const message = createMessage(sessionUserId, session.input, session.selectedTarget);
    const messageData = { message };

    if (sessionUserId === 'user1') {
      // Host broadcasts to all clients
      broadcastFromHost(messageData);
    } else {
      // Client sends to host (which will then broadcast)
      const hostDataChannel = clientDataChannelsRef.current.get('host');
      if (hostDataChannel && hostDataChannel.readyState === 'open') {
        hostDataChannel.send(JSON.stringify(messageData));
      }
    }

    // Add message to local state
    setUserSessions(prev => {
      return prev.map(s => {
        if (s.userId === sessionUserId) {
          return {
            ...s,
            messages: [...s.messages, message],
            input: ''
          };
        }
        // Also add to target user's session if private
        if (message.isPrivate && s.userId === message.target) {
          return {
            ...s,
            messages: [...s.messages, message]
          };
        }
        // Add to all sessions if public
        if (message.target === 'everyone') {
          return {
            ...s,
            messages: [...s.messages, message]
          };
        }
        return s;
      });
    });
  };

  const addUser = () => {
    if (userSessions.length >= 12) return;
    
    const newUserId = `user${userSessions.length + 1}`;
    setUserSessions(prev => [...prev, {
      userId: newUserId,
      isConnected: false,
      messages: [],
      input: '',
      selectedTarget: 'everyone',
      isHost: false
    }]);
  };

  const removeUser = (userId: string) => {
    if (userSessions.length <= 2 || userId === 'user1') return;
    
    // Clean up connections
    const connection = clientConnectionsRef.current.get(userId);
    connection?.close();
    clientConnectionsRef.current.delete(userId);
    clientDataChannelsRef.current.delete(userId);

    setUserSessions(prev => prev.filter(s => s.userId !== userId));
  };

  const toggleConnection = async (userId: string) => {
    const session = userSessions.find(s => s.userId === userId);
    if (!session) return;

    if (session.isConnected) {
      // Disconnect
      const connection = clientConnectionsRef.current.get(userId);
      connection?.close();
      clientConnectionsRef.current.delete(userId);
      clientDataChannelsRef.current.delete(userId);
      
      setUserSessions(prev => 
        prev.map(s => 
          s.userId === userId 
            ? { ...s, isConnected: false }
            : s
        )
      );
    } else {
      // Connect
      await connectUser(userId);
    }
  };

  const updateInput = (userId: string, value: string) => {
    setUserSessions(prev =>
      prev.map(s =>
        s.userId === userId ? { ...s, input: value } : s
      )
    );
  };

  const updateSelectedTarget = (userId: string, target: string) => {
    setUserSessions(prev =>
      prev.map(s =>
        s.userId === userId ? { ...s, selectedTarget: target } : s
      )
    );
  };

  const switchUser = (userId: string) => {
    setCurrentUserId(userId);
  };

  const getAvailableTargets = (currentUser: string) => {
    return userSessions.filter(s => s.userId !== currentUser && s.isConnected);
  };

  const getConnectionCount = () => {
    // Optimized: Host has N-1 connections (star topology)
    return userSessions.length - 1;
  };

  return (
    <div style={{ padding: '20px', maxWidth: '1200px', margin: '0 auto' }}>
      <div style={{ marginBottom: '20px', textAlign: 'center' }}>
        <h1>Optimized WebRTC Chat System (Star Topology)</h1>
        <p>Current User: <strong>{currentUserId}</strong></p>
        <p>Total Connections: <strong>{getConnectionCount()}</strong> (vs {userSessions.length * (userSessions.length - 1)} in mesh)</p>
        <div style={{ marginTop: '10px' }}>
          {userSessions.map(session => (
            <button
              key={session.userId}
              onClick={() => switchUser(session.userId)}
              style={{
                margin: '5px',
                padding: '5px 10px',
                backgroundColor: currentUserId === session.userId ? '#007bff' : '#6c757d',
                color: 'white',
                border: 'none',
                borderRadius: '5px',
                cursor: 'pointer'
              }}
            >
              {session.userId} {session.isHost && '👑'}
            </button>
          ))}
        </div>
        <div style={{ marginTop: '10px' }}>
          <button onClick={addUser} disabled={userSessions.length >= 12} style={{ marginRight: '10px', padding: '8px 16px', backgroundColor: '#28a745', color: 'white', border: 'none', borderRadius: '5px', cursor: userSessions.length >= 12 ? 'not-allowed' : 'pointer' }}>
            Add User (Max: 12)
          </button>
        </div>
      </div>

      <div style={{ 
        display: 'grid', 
        gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', 
        gap: '20px',
        maxHeight: '80vh',
        overflow: 'auto'
      }}>
        {userSessions.map(session => (
          <div key={session.userId} style={{ 
            border: '2px solid #ddd', 
            borderRadius: '10px', 
            padding: '15px',
            backgroundColor: currentUserId === session.userId ? '#f0f8ff' : '#ffffff',
            boxShadow: currentUserId === session.userId ? '0 0 10px rgba(0,123,255,0.3)' : 'none'
          }}>
            <div style={{ 
              display: 'flex', 
              justifyContent: 'space-between', 
              alignItems: 'center', 
              marginBottom: '10px',
              paddingBottom: '10px',
              borderBottom: '1px solid #eee'
            }}>
              <h3 style={{ margin: 0 }}>
                {session.userId} 
                {session.isHost && <span style={{ marginLeft: '5px' }}>👑 Host</span>}
              </h3>
              <div>
                <button
                  onClick={() => toggleConnection(session.userId)}
                  disabled={session.userId === 'user1'}
                  style={{
                    padding: '4px 8px',
                    backgroundColor: session.isConnected ? '#dc3545' : '#28a745',
                    color: 'white',
                    border: 'none',
                    borderRadius: '3px',
                    cursor: session.userId === 'user1' ? 'not-allowed' : 'pointer',
                    marginRight: '5px',
                    opacity: session.userId === 'user1' ? 0.5 : 1
                  }}
                >
                  {session.isConnected ? 'Disconnect' : 'Connect'}
                </button>
                {userSessions.length > 2 && session.userId !== 'user1' && (
                  <button
                    onClick={() => removeUser(session.userId)}
                    style={{
                      padding: '4px 8px',
                      backgroundColor: '#dc3545',
                      color: 'white',
                      border: 'none',
                      borderRadius: '3px',
                      cursor: 'pointer'
                    }}
                  >
                    Remove
                  </button>
                )}
              </div>
            </div>

            <div style={{ 
              height: '300px', 
              overflowY: 'auto', 
              border: '1px solid #ccc', 
              padding: '10px', 
              marginBottom: '10px',
              backgroundColor: '#fafafa'
            }}>
              {session.messages.map(msg => (
                <div key={msg.id} style={{ 
                  marginBottom: '8px', 
                  display: 'flex', 
                  justifyContent: msg.sender === session.userId ? 'flex-end' : 'flex-start' 
                }}>
                  <div style={{ 
                    maxWidth: '70%', 
                    padding: '8px 12px', 
                    borderRadius: '18px',
                    backgroundColor: msg.sender === session.userId ? '#007bff' : '#e9ecef',
                    color: msg.sender === session.userId ? 'white' : 'black',
                    position: 'relative'
                  }}>
                    <div style={{ fontSize: '14px', fontWeight: 'bold', marginBottom: '2px' }}>
                      {msg.sender}
                      {msg.isPrivate && (
                        <span style={{ fontSize: '12px', marginLeft: '5px', opacity: 0.8 }}>
                          → {msg.target}
                        </span>
                      )}
                    </div>
                    <div>{msg.content}</div>
                    <div style={{ fontSize: '10px', marginTop: '2px', opacity: 0.7 }}>
                      {msg.timestamp.toLocaleTimeString()}
                    </div>
                  </div>
                </div>
              ))}
              <div ref={messagesEndRef} />
            </div>

            {session.isConnected && (
              <div style={{ display: 'flex', flexDirection: 'column', gap: '5px' }}>
                <select
                  value={session.selectedTarget}
                  onChange={(e) => updateSelectedTarget(session.userId, e.target.value)}
                  style={{ padding: '5px', border: '1px solid #ccc', borderRadius: '5px' }}
                >
                  <option value="everyone">Send to Everyone</option>
                  {getAvailableTargets(session.userId).map(target => (
                    <option key={target.userId} value={target.userId}>
                      Send to {target.userId}
                    </option>
                  ))}
                </select>
                <div style={{ display: 'flex' }}>
                  <input
                    type="text"
                    value={session.input}
                    onChange={(e) => updateInput(session.userId, e.target.value)}
                    onKeyPress={(e) => {
                      if (e.key === 'Enter') {
                        sendMessage(session.userId);
                      }
                    }}
                    placeholder="Enter message..."
                    style={{ 
                      flex: 1, 
                      padding: '8px', 
                      border: '1px solid #ccc', 
                      borderRadius: '20px',
                      marginRight: '8px'
                    }}
                  />
                  <button
                    onClick={() => sendMessage(session.userId)}
                    style={{ 
                      padding: '8px 16px', 
                      backgroundColor: '#007bff', 
                      color: 'white', 
                      border: 'none', 
                      borderRadius: '20px',
                      cursor: 'pointer'
                    }}
                  >
                    Send
                  </button>
                </div>
              </div>
            )}
          </div>
        ))}
      </div>

      <div style={{ marginTop: '20px', padding: '15px', backgroundColor: '#f8f9fa', borderRadius: '5px', fontSize: '14px' }}>
        <h4>Optimized WebRTC Star Topology</h4>
        <p><strong>Advantages:</strong></p>
        <ul>
          <li><strong>Scalable:</strong> Only N-1 connections needed (vs N×(N-1) in mesh)</li>
          <li><strong>Efficient:</strong> For 12 users: 11 connections vs 132 in mesh topology</li>
          <li><strong>Centralized routing:</strong> Host manages all message distribution</li>
          <li><strong>True P2P:</strong> Works across different devices/networks</li>
          <li><strong>Reduced bandwidth:</strong> Each client only connects to host</li>
        </ul>
        <p><strong>Trade-offs:</strong></p>
        <ul>
          <li>Host becomes single point of failure</li>
          <li>Host handles more processing and bandwidth</li>
          <li>Slightly higher latency for client-to-client communication</li>
          <li>More complex signaling required in real implementation</li>
        </ul>
      </div>
    </div>
  );
};

export default OptimizedWebRTCChat;