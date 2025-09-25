
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
}

const BroadcastChat = () => {
  const [currentUserId, setCurrentUserId] = useState('user1');
  const [userSessions, setUserSessions] = useState<UserSession[]>([
    { 
      userId: 'user1', 
      isConnected: true, 
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

  const broadcastChannelRef = useRef<BroadcastChannel | null>(null);
  const messagesEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    // Initialize Broadcast Channel
    broadcastChannelRef.current = new BroadcastChannel('chat-channel');
    
    // Listen for messages from other users
    broadcastChannelRef.current.onmessage = (event) => {
      const { message, fromUserId } = event.data;
      
      // Don't process messages from ourselves
      if (fromUserId === currentUserId) return;
      
      // Add message to all relevant sessions
      setUserSessions(prev => {
        return prev.map(session => {
          // Show message in sender's session and target's session (if private)
          const shouldShowMessage = 
            session.userId === message.sender || 
            message.target === 'everyone' ||
            (message.target === session.userId && message.isPrivate);
          
          if (shouldShowMessage) {
            return {
              ...session,
              messages: [...session.messages, message]
            };
          }
          return session;
        });
      });
    };

    return () => {
      broadcastChannelRef.current?.close();
    };
  }, [currentUserId]);

  useEffect(() => {
    scrollToBottom();
  }, [userSessions]);

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
    if (!session || !session.input.trim()) return;

    const message = createMessage(sessionUserId, session.input, session.selectedTarget);
    
    // Broadcast message to other users
    if (broadcastChannelRef.current) {
      broadcastChannelRef.current.postMessage({
        message,
        fromUserId: currentUserId
      });
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
      selectedTarget: 'everyone'
    }]);
  };

  const removeUser = (userId: string) => {
    if (userSessions.length <= 2) return;
    setUserSessions(prev => prev.filter(s => s.userId !== userId));
  };

  const toggleConnection = (userId: string) => {
    setUserSessions(prev => 
      prev.map(s => 
        s.userId === userId 
          ? { ...s, isConnected: !s.isConnected }
          : s
      )
    );
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

  return (
    <div style={{ padding: '20px', maxWidth: '1200px', margin: '0 auto' }}>
      <div style={{ marginBottom: '20px', textAlign: 'center' }}>
        <h1>Broadcast Channel Chat System</h1>
        <p>Current User: <strong>{currentUserId}</strong></p>
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
              {session.userId}
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
              <h3 style={{ margin: 0 }}>{session.userId}</h3>
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
        <h4>Broadcast Channel Communication</h4>
        <p><strong>Advantages:</strong></p>
        <ul>
          <li>Simple implementation using browser's Broadcast Channel API</li>
          <li>Efficient for same-origin communication</li>
          <li>No complex P2P connection management</li>
          <li>Works instantly without ICE candidates or STUN servers</li>
        </ul>
        <p><strong>Limitations:</strong></p>
        <ul>
          <li>Only works within the same browser tab/origin</li>
          <li>Cannot communicate across different devices/browsers</li>
          <li>Messages are not truly peer-to-peer across network</li>
        </ul>
      </div>
    </div>
  );
};

export default BroadcastChat;