'use client';
import React, { useEffect, useState, useRef } from 'react';

type UppercaseLetter = 
  | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G'
  | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N'
  | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U'
  | 'V' | 'W' | 'X' | 'Y' | 'Z';

interface Message {
  id: string;
  text: string;
  sender: UppercaseLetter;
  timestamp: string;
  target?: 'ALL' | UppercaseLetter;
  isPrivate?: boolean;
}

interface UserSession {
  userId: UppercaseLetter;
  isConnected: boolean;
  messages: Message[];
  input: string;
  selectedTarget: 'ALL' | UppercaseLetter;
}

export default function WebRTCChat() {
  const [userSessions, setUserSessions] = useState<{ [key in UppercaseLetter]?: UserSession }>({
    A: { userId: 'A', isConnected: false, messages: [], input: '', selectedTarget: 'ALL' },
    B: { userId: 'B', isConnected: false, messages: [], input: '', selectedTarget: 'ALL' }
  });
  const [connectionStatus, setConnectionStatus] = useState<string>('Waiting for connection...');
  const [isInitialized, setIsInitialized] = useState(false);
  
  // Store all peer connections between users
  const connectionsRef = useRef<{ [fromUser: string]: { [toUser: string]: RTCPeerConnection } }>({});
  const dataChannelsRef = useRef<{ [fromUser: string]: { [toUser: string]: RTCDataChannel } }>({});
  const chatRefs = useRef<{ [key: string]: React.RefObject<HTMLDivElement | null> }>({});

  // ICE servers configuration
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  // Initialize chat refs for all users
  useEffect(() => {
    Object.keys(userSessions).forEach(userId => {
      if (!chatRefs.current[userId]) {
        chatRefs.current[userId] = React.createRef<HTMLDivElement>();
      }
    });
  }, [userSessions]);

  // Auto scroll function
  const scrollToBottom = (userId: UppercaseLetter) => {
    setTimeout(() => {
      const chatRef = chatRefs.current[userId];
      if (chatRef?.current) {
        chatRef.current.scrollTop = chatRef.current.scrollHeight;
      }
    }, 100);
  };

  const createMessage = ({ text, sender, target, isPrivate }: Partial<Message> = {}): Message => {
    return {
      id: Date.now().toString() + Math.random(),
      text: text || 'Hello!',
      sender: sender || 'A',
      timestamp: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
      target: target || 'ALL',
      isPrivate: isPrivate || false,
    };
  };

  // Add message to specific users based on target
  const addMessage = (msg: Message) => {
    setUserSessions(prev => {
      const updated = { ...prev };
      Object.keys(prev).forEach(userId => {
        const userSession = prev[userId as UppercaseLetter];
        if (userSession) {
          // Show message if it's for everyone, or if it's for this specific user, or if this user sent it
          const shouldShowMessage = 
            msg.target === 'ALL' || 
            msg.target === userId || 
            msg.sender === userId;
            
          if (shouldShowMessage) {
            updated[userId as UppercaseLetter] = {
              ...userSession,
              messages: [...userSession.messages, msg]
            };
          }
        }
      });
      return updated;
    });
    
    // Scroll to bottom for users who received the message
    Object.keys(userSessions).forEach(userId => {
      const shouldShowMessage = 
        msg.target === 'ALL' || 
        msg.target === userId || 
        msg.sender === userId;
      if (shouldShowMessage) {
        scrollToBottom(userId as UppercaseLetter);
      }
    });
  };

  // Create peer connection between two users
  const createPeerConnection = async (fromUser: UppercaseLetter, toUser: UppercaseLetter) => {
    const pc = new RTCPeerConnection(iceServers);
    
    // Initialize connections object for fromUser if it doesn't exist
    if (!connectionsRef.current[fromUser]) {
      connectionsRef.current[fromUser] = {};
    }
    if (!dataChannelsRef.current[fromUser]) {
      dataChannelsRef.current[fromUser] = {};
    }
    
    connectionsRef.current[fromUser][toUser] = pc;

    // Create data channel
    const dataChannel = pc.createDataChannel(`${fromUser}-to-${toUser}`, { ordered: true });
    dataChannelsRef.current[fromUser][toUser] = dataChannel;

    // Data channel events
    dataChannel.onopen = () => {
      console.log(`Data channel opened: ${fromUser} -> ${toUser}`);
      setUserSessions(prev => ({
        ...prev,
        [fromUser]: prev[fromUser] ? { ...prev[fromUser], isConnected: true } : prev[fromUser]
      }));
    };

    // Handle incoming data channel
    pc.ondatachannel = (event) => {
      const channel = event.channel;
      channel.onmessage = (event) => {
        const msg = JSON.parse(event.data) as Message;
        console.log(`Received message from ${msg.sender}: ${msg.text}`);
        addMessage(msg);
      };
    };

    // ICE candidate collection
    const candidates: RTCIceCandidate[] = [];
    pc.onicecandidate = (event) => {
      if (event.candidate) {
        candidates.push(event.candidate);
      }
    };

    return { pc, dataChannel, candidates };
  };

  // Initialize WebRTC connections for current users
  const initializeWebRTC = async () => {
    try {
      setConnectionStatus('Initializing connections...');
      const users = Object.keys(userSessions) as UppercaseLetter[];
      const connectionPairs: { [key: string]: { pc: RTCPeerConnection, dataChannel: RTCDataChannel, candidates: RTCIceCandidate[] } } = {};

      // Create connections between all user pairs
      for (let i = 0; i < users.length; i++) {
        for (let j = i + 1; j < users.length; j++) {
          const userA = users[i];
          const userB = users[j];
          
          // Create A -> B connection
          const connectionAB = await createPeerConnection(userA, userB);
          connectionPairs[`${userA}-${userB}`] = connectionAB;
          
          // Create B -> A connection  
          const connectionBA = await createPeerConnection(userB, userA);
          connectionPairs[`${userB}-${userA}`] = connectionBA;
        }
      }

      // Exchange offers and answers for all pairs
      const connectionKeys = Object.keys(connectionPairs);
      for (let i = 0; i < connectionKeys.length; i += 2) {
        const keyAB = connectionKeys[i];
        const keyBA = connectionKeys[i + 1];
        
        if (keyAB && keyBA) {
          const { pc: pcAB } = connectionPairs[keyAB];
          const { pc: pcBA } = connectionPairs[keyBA];

          // Exchange offer/answer
          const offer = await pcAB.createOffer();
          await pcAB.setLocalDescription(offer);
          await pcBA.setRemoteDescription(offer);

          const answer = await pcBA.createAnswer();
          await pcBA.setLocalDescription(answer);
          await pcAB.setRemoteDescription(answer);

          // Exchange ICE candidates after a delay
          setTimeout(async () => {
            const { candidates: candidatesAB } = connectionPairs[keyAB];
            const { candidates: candidatesBA } = connectionPairs[keyBA];
            
            for (const candidate of candidatesAB) {
              await pcBA.addIceCandidate(candidate);
            }
            for (const candidate of candidatesBA) {
              await pcAB.addIceCandidate(candidate);
            }
          }, 1000);
        }
      }

      setConnectionStatus('P2P Connected');
      setIsInitialized(true);

    } catch (error) {
      console.error('WebRTC initialization failed:', error);
      setConnectionStatus('Connection Failed');
    }
  };

  // Send message from specific user
  const sendMessage = (sender: UppercaseLetter, message: string) => {
    if (!message.trim()) return;

    const senderSession = userSessions[sender];
    if (!senderSession) return;

    const target = senderSession.selectedTarget;
    const isPrivate = target !== 'ALL';
    
    const msg = createMessage({ 
      text: message, 
      sender, 
      target, 
      isPrivate 
    });
    
    addMessage(msg);

    // Send to specific users based on target
    const senderChannels = dataChannelsRef.current[sender];
    if (senderChannels) {
      if (target === 'ALL') {
        // Send to all other users
        Object.values(senderChannels).forEach(dataChannel => {
          if (dataChannel && dataChannel.readyState === 'open') {
            dataChannel.send(JSON.stringify(msg));
          }
        });
      } else {
        // Send to specific user only
        const targetChannel = senderChannels[target];
        if (targetChannel && targetChannel.readyState === 'open') {
          targetChannel.send(JSON.stringify(msg));
        }
      }
    }

    // Clear input field
    setUserSessions(prev => ({
      ...prev,
      [sender]: prev[sender] ? { ...prev[sender], input: '' } : prev[sender]
    }));
  };

  // Update user input
  const updateUserInput = (userId: UppercaseLetter, value: string) => {
    setUserSessions(prev => ({
      ...prev,
      [userId]: prev[userId] ? { ...prev[userId], input: value } : prev[userId]
    }));
  };

  // Update selected target
  const updateSelectedTarget = (userId: UppercaseLetter, target: 'ALL' | UppercaseLetter) => {
    setUserSessions(prev => ({
      ...prev,
      [userId]: prev[userId] ? { ...prev[userId], selectedTarget: target } : prev[userId]
    }));
  };

  // Add new user (max 12)
  const addUserSession = () => {
    const currentUsers = Object.keys(userSessions);
    if (currentUsers.length >= 12) {
      alert('Maximum number of users reached (12)');
      return;
    }

    const newUserId = String.fromCharCode(65 + currentUsers.length) as UppercaseLetter;
    setUserSessions(prev => ({
      ...prev,
      [newUserId]: { userId: newUserId, isConnected: false, messages: [], input: '', selectedTarget: 'ALL' }
    }));
    
    setIsInitialized(false); // Force re-initialization
  };

  // Initialize on component mount and when users change
  useEffect(() => {
    if (!isInitialized) {
      initializeWebRTC();
    }
  }, [userSessions, isInitialized]);

  // Keyboard event handler
  const handleKeyPress = (e: React.KeyboardEvent, sender: UppercaseLetter) => {
    if (e.key === 'Enter') {
      const userSession = userSessions[sender];
      if (userSession) {
        sendMessage(sender, userSession.input);
      }
    }
  };

  // User screen component
  const userScreen = (userSession: UserSession) => {
    if (!chatRefs.current[userSession.userId]) {
      chatRefs.current[userSession.userId] = React.createRef<HTMLDivElement | null>();
    }

    return (
      <div key={userSession.userId} style={{
        width: '300px',
        height: '500px',
        backgroundColor: 'white',
        border: '2px solid #333',
        borderRadius: '20px',
        padding: '15px',
        margin: '10px',
        display: 'flex',
        flexDirection: 'column'
      }}>
        {/* Header */}
        <div style={{
          padding: '10px',
          backgroundColor: userSession.userId === 'A' ? '#2196f3' : userSession.userId === 'B' ? '#4caf50' : '#ff9800',
          color: 'white',
          borderRadius: '10px',
          textAlign: 'center',
          marginBottom: '10px'
        }}>
          <h3>User {userSession.userId}</h3>
          <div style={{ fontSize: '12px' }}>
            Status: {userSession.isConnected ? 'Connected' : 'Disconnected'}
          </div>
        </div>

        {/* Messages */}
        <div 
          ref={chatRefs.current[userSession.userId]}
          style={{
            flex: 1,
            overflowY: 'auto',
            backgroundColor: '#f9f9f9',
            padding: '10px',
            borderRadius: '10px',
            marginBottom: '10px'
          }}
        >
          {userSession.messages.map((msg) => (
            <div key={msg.id} style={{
              display: 'flex',
              justifyContent: msg.sender === userSession.userId ? 'flex-end' : 'flex-start',
              marginBottom: '8px'
            }}>
              <div style={{
                maxWidth: '80%',
                padding: '8px 12px',
                borderRadius: '15px',
                backgroundColor: msg.sender === userSession.userId 
                  ? (msg.isPrivate ? '#ff9800' : '#2196f3') 
                  : (msg.isPrivate ? '#ffecb3' : '#e0e0e0'),
                color: msg.sender === userSession.userId ? 'white' : 'black',
                fontSize: '14px',
                border: msg.isPrivate ? '2px solid #ff6f00' : 'none'
              }}>
                <div style={{ 
                  fontWeight: 'bold', 
                  fontSize: '12px', 
                  marginBottom: '2px',
                  display: 'flex',
                  justifyContent: 'space-between',
                  alignItems: 'center'
                }}>
                  <span>{msg.sender}</span>
                  {msg.isPrivate && (
                    <span style={{ 
                      fontSize: '10px', 
                      backgroundColor: 'rgba(255,255,255,0.3)',
                      padding: '2px 6px',
                      borderRadius: '8px'
                    }}>
                      → {msg.target}
                    </span>
                  )}
                </div>
                {msg.text}
                <div style={{ fontSize: '10px', opacity: 0.7, marginTop: '2px' }}>
                  {msg.timestamp}
                </div>
              </div>
            </div>
          ))}
        </div>

        {/* Target Selection */}
        <div style={{ marginBottom: '8px' }}>
          <label style={{ fontSize: '12px', marginBottom: '4px', display: 'block', fontWeight: 'bold' }}>
            Send to:
          </label>
          <select
            value={userSession.selectedTarget}
            onChange={(e) => updateSelectedTarget(userSession.userId, e.target.value as 'ALL' | UppercaseLetter)}
            disabled={!userSession.isConnected}
            style={{
              width: '100%',
              padding: '6px',
              border: '1px solid #ccc',
              borderRadius: '8px',
              backgroundColor: 'white',
              fontSize: '12px'
            }}
          >
            <option value="ALL">🌐 Everyone</option>
            {Object.keys(userSessions)
              .filter(userId => userId !== userSession.userId)
              .map(userId => (
                <option key={userId} value={userId}>
                  👤 User {userId} (Private)
                </option>
              ))}
          </select>
        </div>

        {/* Input */}
        <div style={{ display: 'flex', gap: '8px' }}>
          <input
            type="text"
            value={userSession.input}
            onChange={(e) => updateUserInput(userSession.userId, e.target.value)}
            onKeyPress={(e) => handleKeyPress(e, userSession.userId)}
            placeholder={userSession.selectedTarget === 'ALL' 
              ? "Type a message to everyone..." 
              : `Type a private message to User ${userSession.selectedTarget}...`}
            disabled={!userSession.isConnected}
            style={{
              flex: 1,
              padding: '8px',
              border: '1px solid #ccc',
              borderRadius: '15px',
              outline: 'none',
              backgroundColor: userSession.selectedTarget === 'ALL' ? 'white' : '#fff3e0'
            }}
          />
          <button
            onClick={() => sendMessage(userSession.userId, userSession.input)}
            disabled={!userSession.isConnected || !userSession.input.trim()}
            style={{
              padding: '8px 12px',
              backgroundColor: userSession.selectedTarget === 'ALL' ? '#2196f3' : '#ff9800',
              color: 'white',
              border: 'none',
              borderRadius: '15px',
              cursor: userSession.isConnected ? 'pointer' : 'not-allowed',
              opacity: userSession.isConnected ? 1 : 0.5,
              fontSize: '12px'
            }}
          >
            {userSession.selectedTarget === 'ALL' ? 'Send All' : 'Send Private'}
          </button>
        </div>
      </div>
    );
  };

  return (
    <div style={{ 
      padding: '20px', 
      backgroundColor: '#f0f0f0', 
      minHeight: '100vh',
      fontFamily: 'Arial, sans-serif'
    }}>
      <div style={{ textAlign: 'center', marginBottom: '20px' }}>
        <h1>WebRTC Multi-User Chat</h1>
        <p>Status: {connectionStatus}</p>
        <button
          onClick={addUserSession}
          disabled={Object.keys(userSessions).length >= 12}
          style={{
            padding: '10px 20px',
            backgroundColor: '#4caf50',
            color: 'white',
            border: 'none',
            borderRadius: '20px',
            cursor: Object.keys(userSessions).length >= 12 ? 'not-allowed' : 'pointer',
            opacity: Object.keys(userSessions).length >= 12 ? 0.5 : 1,
            fontSize: '16px'
          }}
        >
          Add User ({Object.keys(userSessions).length}/12)
        </button>
      </div>
      
      <div style={{ 
        display: 'flex', 
        flexWrap: 'wrap',
        justifyContent: 'center',
        gap: '10px'
      }}>
        {Object.values(userSessions).map(userSession => userScreen(userSession))}
      </div>
    </div>
  );
}