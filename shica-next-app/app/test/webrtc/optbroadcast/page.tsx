'use client';

import React, { useState, useRef, useEffect } from 'react';

interface Message {
  id: string;
  sender: string;
  content: string;
  timestamp: string;
  target: string;
  isPrivate: boolean;
}

interface UserSession {
  userId: string;
  isConnected: boolean;
  messages: Message[];
  input: string;
  selectedTarget: string;
}

interface CommunicationHost {
  id: string;
  isActive: boolean;
  connectedUsers: Set<string>;
  messageQueue: Message[];
}

const OptimizedWebRTCChat = () => {
  const [currentUserId, setCurrentUserId] = useState('user1');
  const [userSessions, setUserSessions] = useState<UserSession[]>([
    { 
      userId: 'user1', 
      isConnected: false, 
      messages: [], 
      input: '', 
      selectedTarget: 'everyone'
    },
    { 
      userId: 'user2', 
      isConnected: false, 
      messages: [], 
      input: '', 
      selectedTarget: 'everyone'
    }
  ]);

  // Communication Host (separate from users)
  const [communicationHost, setCommunicationHost] = useState<CommunicationHost>({
    id: 'comm-host',
    isActive: false,
    connectedUsers: new Set(),
    messageQueue: []
  });

  // WebRTC connections - Host manages all user connections
  const hostConnectionsRef = useRef<Map<string, RTCPeerConnection>>(new Map());
  const hostDataChannelsRef = useRef<Map<string, RTCDataChannel>>(new Map());
  
  // User connections to host
  const userToHostConnectionRef = useRef<Map<string, RTCPeerConnection>>(new Map());
  const userToHostDataChannelRef = useRef<Map<string, RTCDataChannel>>(new Map());
  
  const messagesEndRef = useRef<HTMLDivElement>(null);
  const isHostActiveRef = useRef(false);

  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  useEffect(() => {
    initializeCommunicationHost();
    return () => {
      cleanup();
    };
  }, []);

  useEffect(() => {
    scrollToBottom();
  }, [userSessions]);

  const cleanup = () => {
    // Close all host connections
    hostConnectionsRef.current.forEach(conn => conn.close());
    hostConnectionsRef.current.clear();
    hostDataChannelsRef.current.clear();

    // Close all user connections
    userToHostConnectionRef.current.forEach(conn => conn.close());
    userToHostConnectionRef.current.clear();
    userToHostDataChannelRef.current.clear();

    isHostActiveRef.current = false;
  };

  // Initialize dedicated communication host
  const initializeCommunicationHost = async () => {
    console.log('🏢 Initializing Communication Host...');
    
    setCommunicationHost(prev => ({
      ...prev,
      isActive: true,
      connectedUsers: new Set()
    }));
    
    isHostActiveRef.current = true;
    console.log('🟢 Communication Host is now active');
  };

  // Host creates connection to a specific user
  const createHostToUserConnection = async (userId: string): Promise<boolean> => {
    if (!isHostActiveRef.current) {
      console.error('❌ Communication Host is not active');
      return false;
    }

    try {
      console.log(`🔗 Host creating connection to ${userId}...`);
      
      const pc = new RTCPeerConnection(iceServers);
      hostConnectionsRef.current.set(userId, pc);

      // Create data channel from host to user
      const dataChannel = pc.createDataChannel(`host-to-${userId}`, {
        ordered: true
      });
      
      hostDataChannelsRef.current.set(userId, dataChannel);

      // Host data channel events
      dataChannel.onopen = () => {
        console.log(`✅ Host connection to ${userId} established`);
        
        setCommunicationHost(prev => ({
          ...prev,
          connectedUsers: new Set([...prev.connectedUsers, userId])
        }));
        
        setUserSessions(prev => 
          prev.map(s => 
            s.userId === userId 
              ? { ...s, isConnected: true }
              : s
          )
        );
      };

      dataChannel.onmessage = (event) => {
        const messageData = JSON.parse(event.data);
        console.log(`📨 Host received message from ${userId}:`, messageData);
        handleMessageFromUser(messageData, userId);
      };

      dataChannel.onclose = () => {
        console.log(`🔴 Host connection to ${userId} closed`);
        setCommunicationHost(prev => {
          const newConnectedUsers = new Set(prev.connectedUsers);
          newConnectedUsers.delete(userId);
          return {
            ...prev,
            connectedUsers: newConnectedUsers
          };
        });
      };

      dataChannel.onerror = (error) => {
        console.error(`❌ Host data channel error for ${userId}:`, error);
      };

      // Peer connection events
      pc.onconnectionstatechange = () => {
        console.log(`🔄 Host->${userId} connection state:`, pc.connectionState);
      };

      pc.onicecandidate = (event) => {
        if (event.candidate) {
          // In a real implementation, this would be sent through signaling server
          // For this demo, we'll simulate the ICE exchange
          handleICECandidateForUser(userId, event.candidate);
        }
      };

      // Simulate offer/answer exchange
      await simulateOfferAnswerExchange(pc, userId);
      
      return true;
    } catch (error) {
      console.error(`❌ Failed to create host connection to ${userId}:`, error);
      return false;
    }
  };

  // User creates connection to host
  const createUserToHostConnection = async (userId: string): Promise<boolean> => {
    try {
      console.log(`🔗 ${userId} connecting to host...`);
      
      const pc = new RTCPeerConnection(iceServers);
      userToHostConnectionRef.current.set(userId, pc);

      // Handle incoming data channel from host
      pc.ondatachannel = (event) => {
        const dataChannel = event.channel;
        userToHostDataChannelRef.current.set(userId, dataChannel);
        
        dataChannel.onopen = () => {
          console.log(`✅ ${userId} connected to host`);
        };

        dataChannel.onmessage = (event) => {
          const messageData = JSON.parse(event.data);
          console.log(`📨 ${userId} received message from host:`, messageData);
          handleMessageFromHost(messageData, userId);
        };

        dataChannel.onclose = () => {
          console.log(`🔴 ${userId} disconnected from host`);
          setUserSessions(prev => 
            prev.map(s => 
              s.userId === userId 
                ? { ...s, isConnected: false }
                : s
            )
          );
        };
      };

      pc.onconnectionstatechange = () => {
        console.log(`🔄 ${userId}->Host connection state:`, pc.connectionState);
      };

      return true;
    } catch (error) {
      console.error(`❌ Failed to create ${userId} connection to host:`, error);
      return false;
    }
  };

  // Simulate WebRTC offer/answer exchange (normally done through signaling server)
  const simulateOfferAnswerExchange = async (hostPc: RTCPeerConnection, userId: string) => {
    // In a real implementation, this would involve a signaling server
    // For demo purposes, we simulate the exchange
    setTimeout(async () => {
      try {
        // Create user connection
        await createUserToHostConnection(userId);
        const userPc = userToHostConnectionRef.current.get(userId);
        
        if (userPc) {
          // Create offer from host
          const offer = await hostPc.createOffer();
          await hostPc.setLocalDescription(offer);
          await userPc.setRemoteDescription(offer);

          // Create answer from user
          const answer = await userPc.createAnswer();
          await userPc.setLocalDescription(answer);
          await hostPc.setRemoteDescription(answer);

          console.log(`🤝 Offer/Answer exchange completed for ${userId}`);
        }
      } catch (error) {
        console.error(`❌ Offer/Answer exchange failed for ${userId}:`, error);
      }
    }, 100);
  };

  // Handle ICE candidates (simplified)
  const handleICECandidateForUser = async (userId: string, candidate: RTCIceCandidate) => {
    setTimeout(async () => {
      const userPc = userToHostConnectionRef.current.get(userId);
      if (userPc) {
        try {
          await userPc.addIceCandidate(candidate);
        } catch (error) {
          console.error(`❌ Failed to add ICE candidate for ${userId}:`, error);
        }
      }
    }, 50);
  };

  // Host handles message from user and routes it
  const handleMessageFromUser = (messageData: { message: Message }, fromUserId: string) => {
    const { message } = messageData;
    
    console.log(`🏢 Host routing message from ${fromUserId} to ${message.target}`);
    
    // Add to host's message queue
    setCommunicationHost(prev => ({
      ...prev,
      messageQueue: [...prev.messageQueue, message]
    }));

    // Route message based on target
    if (message.target === 'everyone') {
      // Broadcast to all connected users except sender
      hostDataChannelsRef.current.forEach((dataChannel, userId) => {
        if (userId !== fromUserId && dataChannel.readyState === 'open') {
          try {
            dataChannel.send(JSON.stringify(messageData));
            console.log(`📤 Host broadcasted message to ${userId}`);
          } catch (error) {
            console.error(`❌ Failed to broadcast to ${userId}:`, error);
          }
        }
      });
    } else {
      // Send to specific user
      const targetChannel = hostDataChannelsRef.current.get(message.target);
      if (targetChannel && targetChannel.readyState === 'open') {
        try {
          targetChannel.send(JSON.stringify(messageData));
          console.log(`📤 Host sent private message to ${message.target}`);
        } catch (error) {
          console.error(`❌ Failed to send private message to ${message.target}:`, error);
        }
      }
    }
  };

  // User handles message from host
  const handleMessageFromHost = (messageData: { message: Message }, userId: string) => {
    const { message } = messageData;
    
    setUserSessions(prev => {
      return prev.map(session => {
        // Show message in target user's session
        if (session.userId === userId) {
          const messageExists = session.messages.find(m => m.id === message.id);
          if (!messageExists) {
            return {
              ...session,
              messages: [...session.messages, message]
            };
          }
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
      timestamp: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
      target,
      isPrivate: target !== 'everyone'
    };
  };

  const sendMessage = (sessionUserId: string) => {
    const session = userSessions.find(s => s.userId === sessionUserId);
    if (!session || !session.input.trim() || !session.isConnected) return;

    const message = createMessage(sessionUserId, session.input, session.selectedTarget);
    const messageData = { message };

    // Send message through host
    const userChannel = userToHostDataChannelRef.current.get(sessionUserId);
    if (userChannel && userChannel.readyState === 'open') {
      try {
        userChannel.send(JSON.stringify(messageData));
        console.log(`📤 ${sessionUserId} sent message to host`);
        
        // Add message to sender's local state immediately
        setUserSessions(prev => {
          return prev.map(s => {
            if (s.userId === sessionUserId) {
              return {
                ...s,
                messages: [...s.messages, message],
                input: ''
              };
            }
            return s;
          });
        });
      } catch (error) {
        console.error(`❌ Failed to send message from ${sessionUserId}:`, error);
      }
    } else {
      console.warn(`⚠️ ${sessionUserId} is not connected to host`);
    }
  };

  const connectUser = async (userId: string) => {
    if (!isHostActiveRef.current) {
      console.error('❌ Cannot connect user: Communication Host is not active');
      return;
    }

    const success = await createHostToUserConnection(userId);
    if (success) {
      console.log(`✅ ${userId} connection process initiated`);
    }
  };

  const disconnectUser = async (userId: string) => {
    // Close host connection to user
    const hostConnection = hostConnectionsRef.current.get(userId);
    if (hostConnection) {
      hostConnection.close();
      hostConnectionsRef.current.delete(userId);
      hostDataChannelsRef.current.delete(userId);
    }

    // Close user connection to host
    const userConnection = userToHostConnectionRef.current.get(userId);
    if (userConnection) {
      userConnection.close();
      userToHostConnectionRef.current.delete(userId);
      userToHostDataChannelRef.current.delete(userId);
    }

    setUserSessions(prev => 
      prev.map(s => 
        s.userId === userId 
          ? { ...s, isConnected: false }
          : s
      )
    );

    setCommunicationHost(prev => {
      const newConnectedUsers = new Set(prev.connectedUsers);
      newConnectedUsers.delete(userId);
      return {
        ...prev,
        connectedUsers: newConnectedUsers
      };
    });

    console.log(`🔴 ${userId} disconnected`);
  };

  const addUser = () => {
    if (userSessions.length >= 12) return;
    
    const newUserId = `user${userSessions.length + 1}`;
    setUserSessions(prev => [...prev, {
      userId: newUserId,
      isConnected: false,
      messages: [],
      input: '',
      selectedTarget: 'everyone'
    }]);
  };

  const removeUser = (userId: string) => {
    if (userSessions.length <= 2) return;
    
    // Disconnect user first
    disconnectUser(userId);
    
    // Remove from sessions
    setUserSessions(prev => prev.filter(s => s.userId !== userId));
  };

  const toggleConnection = async (userId: string) => {
    const session = userSessions.find(s => s.userId === userId);
    if (!session) return;

    if (session.isConnected) {
      await disconnectUser(userId);
    } else {
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

  const getConnectionStats = () => {
    const connectedCount = Array.from(communicationHost.connectedUsers).length;
    const totalUsers = userSessions.length;
    const meshConnections = totalUsers * (totalUsers - 1);
    const starConnections = totalUsers; // Each user connects to host
    
    return {
      connected: connectedCount,
      total: totalUsers,
      starConnections,
      meshConnections
    };
  };

  const stats = getConnectionStats();

  return (
    <div style={{ padding: '20px', maxWidth: '1400px', margin: '0 auto' }}>
      {/* Header */}
      <div style={{ marginBottom: '20px', textAlign: 'center' }}>
        <h1>WebRTC with Dedicated Communication Host</h1>
        <div style={{ 
          display: 'flex', 
          justifyContent: 'center', 
          alignItems: 'center', 
          gap: '20px',
          marginBottom: '15px'
        }}>
          <div style={{ 
            padding: '10px 15px',
            backgroundColor: communicationHost.isActive ? '#28a745' : '#dc3545',
            color: 'white',
            borderRadius: '8px',
            fontWeight: 'bold'
          }}>
            🏢 Communication Host: {communicationHost.isActive ? 'ACTIVE' : 'INACTIVE'}
          </div>
          <div style={{ 
            padding: '10px 15px',
            backgroundColor: '#007bff',
            color: 'white',
            borderRadius: '8px'
          }}>
            Connected: {stats.connected}/{stats.total}
          </div>
          <div style={{ 
            padding: '10px 15px',
            backgroundColor: '#6c757d',
            color: 'white',
            borderRadius: '8px'
          }}>
            Connections: {stats.starConnections} (vs {stats.meshConnections} mesh)
          </div>
        </div>
        
        <div style={{ marginTop: '10px' }}>
          <p><strong>Current User:</strong> {currentUserId}</p>
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
              {session.userId} {session.isConnected ? '🟢' : '🔴'}
            </button>
          ))}
        </div>
        
        <div style={{ marginTop: '10px' }}>
          <button 
            onClick={addUser} 
            disabled={userSessions.length >= 12} 
            style={{ 
              marginRight: '10px', 
              padding: '8px 16px', 
              backgroundColor: '#28a745', 
              color: 'white', 
              border: 'none', 
              borderRadius: '5px', 
              cursor: userSessions.length >= 12 ? 'not-allowed' : 'pointer' 
            }}
          >
            Add User (Max: 12)
          </button>
        </div>
      </div>

      {/* Communication Host Status */}
      <div style={{ 
        marginBottom: '20px', 
        padding: '15px', 
        backgroundColor: '#f8f9fa', 
        borderRadius: '8px',
        border: '2px solid #e9ecef'
      }}>
        <h4>🏢 Communication Host Status</h4>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '10px' }}>
          <div>
            <strong>Status:</strong> {communicationHost.isActive ? '🟢 Active' : '🔴 Inactive'}
          </div>
          <div>
            <strong>Connected Users:</strong> {Array.from(communicationHost.connectedUsers).join(', ') || 'None'}
          </div>
          <div>
            <strong>Messages Processed:</strong> {communicationHost.messageQueue.length}
          </div>
          <div>
            <strong>Architecture:</strong> Star Topology (Hub & Spoke)
          </div>
        </div>
      </div>

      {/* User Sessions Grid */}
      <div style={{ 
        display: 'grid', 
        gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))', 
        gap: '20px',
        maxHeight: '70vh',
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
            {/* User Header */}
            <div style={{ 
              display: 'flex', 
              justifyContent: 'space-between', 
              alignItems: 'center', 
              marginBottom: '10px',
              paddingBottom: '10px',
              borderBottom: '1px solid #eee'
            }}>
              <h3 style={{ margin: 0 }}>
                👤 {session.userId}
                <span style={{ 
                  marginLeft: '8px',
                  fontSize: '12px',
                  padding: '2px 6px',
                  borderRadius: '10px',
                  backgroundColor: session.isConnected ? '#28a745' : '#dc3545',
                  color: 'white'
                }}>
                  {session.isConnected ? 'CONNECTED' : 'DISCONNECTED'}
                </span>
              </h3>
              <div>
                <button
                  onClick={() => toggleConnection(session.userId)}
                  style={{
                    padding: '4px 8px',
                    backgroundColor: session.isConnected ? '#dc3545' : '#28a745',
                    color: 'white',
                    border: 'none',
                    borderRadius: '3px',
                    cursor: 'pointer',
                    marginRight: '5px'
                  }}
                >
                  {session.isConnected ? 'Disconnect' : 'Connect'}
                </button>
                {userSessions.length > 2 && (
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

            {/* Messages Area */}
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
                    backgroundColor: msg.sender === session.userId ? '#007bff' : (msg.isPrivate ? '#fd7e14' : '#e9ecef'),
                    color: msg.sender === session.userId ? 'white' : (msg.isPrivate ? 'white' : 'black'),
                    border: msg.isPrivate ? '2px solid #fd7e14' : 'none'
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
                      {msg.timestamp}
                    </div>
                  </div>
                </div>
              ))}
              <div ref={messagesEndRef} />
            </div>

            {/* Input Area */}
            {session.isConnected && (
              <div style={{ display: 'flex', flexDirection: 'column', gap: '5px' }}>
                <select
                  value={session.selectedTarget}
                  onChange={(e) => updateSelectedTarget(session.userId, e.target.value)}
                  style={{ 
                    padding: '5px', 
                    border: '1px solid #ccc', 
                    borderRadius: '5px',
                    backgroundColor: session.selectedTarget !== 'everyone' ? '#fff3cd' : 'white'
                  }}
                >
                  <option value="everyone">🌐 Send to Everyone</option>
                  {getAvailableTargets(session.userId).map(target => (
                    <option key={target.userId} value={target.userId}>
                      👤 Send to {target.userId} (Private)
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
                    placeholder="Type a message..."
                    style={{ 
                      flex: 1, 
                      padding: '8px', 
                      border: '1px solid #ccc', 
                      borderRadius: '20px',
                      marginRight: '8px',
                      backgroundColor: session.selectedTarget !== 'everyone' ? '#fff3cd' : 'white'
                    }}
                  />
                  <button
                    onClick={() => sendMessage(session.userId)}
                    disabled={!session.input.trim()}
                    style={{ 
                      padding: '8px 16px', 
                      backgroundColor: session.selectedTarget !== 'everyone' ? '#fd7e14' : '#007bff', 
                      color: 'white', 
                      border: 'none', 
                      borderRadius: '20px',
                      cursor: session.input.trim() ? 'pointer' : 'not-allowed',
                      opacity: session.input.trim() ? 1 : 0.6
                    }}
                  >
                    {session.selectedTarget !== 'everyone' ? 'Send Private' : 'Send All'}
                  </button>
                </div>
              </div>
            )}

            {!session.isConnected && (
              <div style={{ 
                textAlign: 'center', 
                padding: '20px', 
                color: '#6c757d',
                fontStyle: 'italic'
              }}>
                Click "Connect" to join the communication network
              </div>
            )}
          </div>
        ))}
      </div>

      {/* Architecture Info */}
      <div style={{ marginTop: '20px', padding: '15px', backgroundColor: '#e7f3ff', borderRadius: '5px', fontSize: '14px' }}>
        <h4>🏗️ Dedicated Communication Host Architecture</h4>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', gap: '15px' }}>
          <div>
            <p><strong>📡 Communication Flow:</strong></p>
            <ul>
              <li>Users connect only to dedicated host</li>
              <li>Host routes all messages between users</li>
              <li>No direct user-to-user connections</li>
              <li>Host handles message broadcasting and private messaging</li>
            </ul>
          </div>
          <div>
            <p><strong>✅ Advantages:</strong></p>
            <ul>
              <li><strong>Scalable:</strong> N connections (vs N×(N-1) mesh)</li>
              <li><strong>Equal participation:</strong> No user has special role</li>
              <li><strong>Centralized control:</strong> Message routing, moderation</li>
              <li><strong>Reliable:</strong> Host manages all communications</li>
            </ul>
          </div>
          <div>
            <p><strong>⚠️ Considerations:</strong></p>
            <ul>
              <li>Host is single point of failure</li>
              <li>Requires dedicated host infrastructure</li>
              <li>All traffic goes through central point</li>
              <li>Host bandwidth usage scales with users</li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  );
};

export default OptimizedWebRTCChat;