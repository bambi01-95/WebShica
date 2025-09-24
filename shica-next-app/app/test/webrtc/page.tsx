'use client';
import React, { useEffect, useState, useRef } from 'react';

interface Message {
  id: string;
  text: string;
  sender: 'A' | 'B';
  timestamp: String;
}

interface UserSession {
  userId: 'A' | 'B';
  connection?: RTCPeerConnection;
  dataChannel?: RTCDataChannel;
  isConnected: boolean;
}

export default function WebRTCPage() {
  // Common message state
  const [messagesA, setMessagesA] = useState<Message[]>([]);
  const [messagesB, setMessagesB] = useState<Message[]>([]);

  // User A state
  const [userAInput, setUserAInput] = useState('');
  const [userASession, setUserASession] = useState<UserSession>({ userId: 'A', isConnected: false });
  
  // User B state
  const [userBInput, setUserBInput] = useState('');
  const [userBSession, setUserBSession] = useState<UserSession>({ userId: 'B', isConnected: false });
  
  // Connection status
  const [connectionStatus, setConnectionStatus] = useState<string>('Waiting for connection...');
  
  // Refs for RTCPeerConnection management
  const userAConnectionRef = useRef<RTCPeerConnection | null>(null);
  const userBConnectionRef = useRef<RTCPeerConnection | null>(null);
  const userADataChannelRef = useRef<RTCDataChannel | null>(null);
  const userBDataChannelRef = useRef<RTCDataChannel | null>(null);
  
  // Refs for chat scroll management
  const userAChatRef = useRef<HTMLDivElement>(null);
  const userBChatRef = useRef<HTMLDivElement>(null);

  // ICE servers configuration
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  // Auto scroll function
  const scrollToBottom = () => {
    setTimeout(() => {
      if (userAChatRef.current) {
        userAChatRef.current.scrollTop = userAChatRef.current.scrollHeight;
      }
      if (userBChatRef.current) {
        userBChatRef.current.scrollTop = userBChatRef.current.scrollHeight;
      }
    }, 100);
  };
  const createMessage = ({ text, sender }: Partial<Message> = {}): Message => {
    return {
      id: Date.now().toString() + Math.random(),
      text: text || 'what?',
      sender: sender || 'A',
      timestamp: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
      ...{}
    };
  };

  // Add message function
  const addMessage = (msg: Message, storage: 'A' | 'B') => {
    if (storage === 'A') {
      setMessagesA(prev => [...prev, msg]);
    } else {
      setMessagesB(prev => [...prev, msg]);
    }
    scrollToBottom(); // Auto scroll when message is added
  };

  // WebRTC connection initialization
  const initializeWebRTC = async () => {
    try {
      // User A configuration (offer side)
      const pcA = new RTCPeerConnection(iceServers);
      const pcB = new RTCPeerConnection(iceServers);

      userAConnectionRef.current = pcA;
      userBConnectionRef.current = pcB;

      // データチャンネルの作成（A→B用）
      const dataChannelA = pcA.createDataChannel('chatA', { ordered: true });
      userADataChannelRef.current = dataChannelA;

      // データチャンネルの作成（B→A用）  
      const dataChannelB = pcB.createDataChannel('chatB', { ordered: true });
      userBDataChannelRef.current = dataChannelB;

      // A側でBからのデータチャンネルを受信
      pcA.ondatachannel = (event) => {
        const receivedChannel = event.channel;
        receivedChannel.onmessage = (event) => {
          const data = JSON.parse(event.data) as Message;
          if (data.sender === 'B') {
            console.log(`Received message at A from B: ${data.text}`);
            addMessage(data, 'A');
          }
        };
      };

      // B側でAからのデータチャンネルを受信
      pcB.ondatachannel = (event) => {
        const receivedChannel = event.channel;
        receivedChannel.onmessage = (event) => {
          const data:Message = JSON.parse(event.data) as Message;
          if (data.sender === 'A') {
            console.log(`Received message at B from A: ${data.text}`);
            addMessage(data, 'B');
          }
        };
      };

      // データチャンネルA側のイベント設定
      dataChannelA.onopen = () => {
        console.log('Data channel A opened');
        setUserASession(prev => ({ ...prev, isConnected: true }));
      };

      // データチャンネルB側のイベント設定
      dataChannelB.onopen = () => {
        console.log('Data channel B opened');
        setUserBSession(prev => ({ ...prev, isConnected: true }));
        setConnectionStatus('P2P Connected');
      };

      // ICE候補の交換
      const iceCandidatesA: RTCIceCandidate[] = [];
      const iceCandidatesB: RTCIceCandidate[] = [];

      pcA.onicecandidate = (event) => {
        if (event.candidate) {
          iceCandidatesA.push(event.candidate);
        }
      };

      pcB.onicecandidate = (event) => {
        if (event.candidate) {
          iceCandidatesB.push(event.candidate);
        }
      };

      // オファー/アンサーの交換
      const offer = await pcA.createOffer();
      await pcA.setLocalDescription(offer);
      await pcB.setRemoteDescription(offer);

      const answer = await pcB.createAnswer();
      await pcB.setLocalDescription(answer);
      await pcA.setRemoteDescription(answer);

      // ICE候補の追加
      setTimeout(async () => {
        for (const candidate of iceCandidatesA) {
          await pcB.addIceCandidate(candidate);
        }
        for (const candidate of iceCandidatesB) {
          await pcA.addIceCandidate(candidate);
        }
      }, 1000);

      setConnectionStatus('WebRTC Connecting...');

    } catch (error) {
      console.error('WebRTC initialization failed:', error);
      setConnectionStatus('Connection Failed');
    }
  };

  // Message sending function
  const sendMessage = (sender: 'A' | 'B', message: string) => {
    if (!message.trim()) return;

    const dataChannel = sender === 'A' ? userADataChannelRef.current : userBDataChannelRef.current;
    
    if (dataChannel && dataChannel.readyState === 'open') {
      // 自分のメッセージを即座に表示
      console.log(`Sending message from ${sender}: ${message}`);
      const msg = createMessage({ text: message, sender });

      addMessage(msg, sender);

      // 相手に送信（相手側では受信として表示される）
      dataChannel.send(JSON.stringify(msg));

      // 入力フィールドをクリア
      if (sender === 'A') {
        setUserAInput('');
      } else {
        setUserBInput('');
      }
    }
  };

  // 初期化
  useEffect(() => {
    initializeWebRTC();
    
    return () => {
      // クリーンアップ
      userAConnectionRef.current?.close();
      userBConnectionRef.current?.close();
      userADataChannelRef.current?.close();
      userBDataChannelRef.current?.close();
    };
  }, []);

  // メッセージが変更されたときの自動スクロール
  useEffect(() => {
    scrollToBottom();
  }, [messagesA, messagesB]);

  // キーボードイベント
  const handleKeyPress = (e: React.KeyboardEvent, sender: 'A' | 'B') => {
    if (e.key === 'Enter') {
      const message = sender === 'A' ? userAInput : userBInput;
      sendMessage(sender, message);
    }
  };

  return (
    <div style={{ 
      display: 'flex', 
      height: '100vh', 
      fontFamily: 'Arial, sans-serif',
      backgroundColor: '#f0f0f0'
    }}>
      {/* ユーザーA側 */}
      <div style={{ 
        flex: 1, 
        display: 'flex', 
        flexDirection: 'column',
        backgroundColor: '#e3f2fd',
        border: '1px solid #2196f3'
      }}>
        <div style={{ 
          padding: '15px', 
          backgroundColor: '#2196f3', 
          color: 'white',
          textAlign: 'center'
        }}>
          <h2>User A</h2>
          <div style={{ fontSize: '12px' }}>
            Status: {userASession.isConnected ? 'Connected' : 'Disconnected'}
          </div>
        </div>
        
        {/* チャット履歴 */}
        <div 
          ref={userAChatRef}
          style={{ 
            flex: 1, 
            padding: '10px', 
            overflowY: 'auto',
            backgroundColor: 'white'
          }}
        >
          {messagesA.map((msg) => (
            <div
              key={msg.id}
              style={{
                display: 'flex',
                justifyContent: msg.sender === 'A' ? 'flex-end' : 'flex-start',
                marginBottom: '10px'
              }}
            >
              <div
                style={{
                  maxWidth: '70%',
                  padding: '8px 12px',
                  borderRadius: '18px',
                  backgroundColor: msg.sender === 'A' ? '#2196f3' : '#e0e0e0',
                  color: msg.sender === 'A' ? 'white' : 'black',
                  fontSize: '14px'
                }}
              >
                {msg.text}
                <div style={{ 
                  fontSize: '10px', 
                  opacity: 0.7, 
                  marginTop: '2px' 
                }}>
                  {msg.timestamp}
                </div>
              </div>
            </div>
          ))}
        </div>
        
        {/* 入力欄 */}
        <div style={{ 
          padding: '10px', 
          backgroundColor: '#f5f5f5',
          display: 'flex',
          gap: '10px'
        }}>
          <input
            type="text"
            value={userAInput}
            onChange={(e) => setUserAInput(e.target.value)}
            onKeyPress={(e) => handleKeyPress(e, 'A')}
            placeholder="Type a message..."
            disabled={!userASession.isConnected}
            style={{
              flex: 1,
              padding: '10px',
              border: '1px solid #ccc',
              borderRadius: '20px',
              outline: 'none'
            }}
          />
          <button
            onClick={() => sendMessage('A', userAInput)}
            disabled={!userASession.isConnected || !userAInput.trim()}
            style={{
              padding: '10px 20px',
              backgroundColor: '#2196f3',
              color: 'white',
              border: 'none',
              borderRadius: '20px',
              cursor: userASession.isConnected ? 'pointer' : 'not-allowed',
              opacity: userASession.isConnected ? 1 : 0.5
            }}
          >
            Send
          </button>
        </div>
      </div>

      {/* 中央の区切り線 */}
      <div style={{ 
        width: '2px', 
        backgroundColor: '#333',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        position: 'relative'
      }}>
        <div style={{
          backgroundColor: '#333',
          color: 'white',
          padding: '5px 10px',
          borderRadius: '10px',
          fontSize: '12px',
          position: 'absolute'
        }}>
          {connectionStatus}
        </div>
      </div>

      {/* ユーザーB側 */}
      <div style={{ 
        flex: 1, 
        display: 'flex', 
        flexDirection: 'column',
        backgroundColor: '#e8f5e8',
        border: '1px solid #4caf50'
      }}>
        <div style={{ 
          padding: '15px', 
          backgroundColor: '#4caf50', 
          color: 'white',
          textAlign: 'center'
        }}>
          <h2>User B</h2>
          <div style={{ fontSize: '12px' }}>
            Status: {userBSession.isConnected ? 'Connected' : 'Disconnected'}
          </div>
        </div>
        
        {/* チャット履歴 */}
        <div 
          ref={userBChatRef}
          style={{ 
            flex: 1, 
            padding: '10px', 
            overflowY: 'auto',
            backgroundColor: 'white'
          }}
        >
          {messagesB.map((msg) => (
            <div
              key={msg.id}
              style={{
                display: 'flex',
                justifyContent: msg.sender === 'B' ? 'flex-end' : 'flex-start',
                marginBottom: '10px'
              }}
            >
              <div
                style={{
                  maxWidth: '70%',
                  padding: '8px 12px',
                  borderRadius: '18px',
                  backgroundColor: msg.sender === 'B' ? '#4caf50' : '#e0e0e0',
                  color: msg.sender === 'B' ? 'white' : 'black',
                  fontSize: '14px'
                }}
              >
                {msg.text}
                <div style={{ 
                  fontSize: '10px', 
                  opacity: 0.7, 
                  marginTop: '2px' 
                }}>
                  {msg.timestamp}
                </div>
              </div>
            </div>
          ))}
        </div>
        
        {/* 入力欄 */}
        <div style={{ 
          padding: '10px', 
          backgroundColor: '#f5f5f5',
          display: 'flex',
          gap: '10px'
        }}>
          <input
            type="text"
            value={userBInput}
            onChange={(e) => setUserBInput(e.target.value)}
            onKeyPress={(e) => handleKeyPress(e, 'B')}
            placeholder="Type a message..."
            disabled={!userBSession.isConnected}
            style={{
              flex: 1,
              padding: '10px',
              border: '1px solid #ccc',
              borderRadius: '20px',
              outline: 'none'
            }}
          />
          <button
            onClick={() => sendMessage('B', userBInput)}
            disabled={!userBSession.isConnected || !userBInput.trim()}
            style={{
              padding: '10px 20px',
              backgroundColor: '#4caf50',
              color: 'white',
              border: 'none',
              borderRadius: '20px',
              cursor: userBSession.isConnected ? 'pointer' : 'not-allowed',
              opacity: userBSession.isConnected ? 1 : 0.5
            }}
          >
            Send
          </button>
        </div>
      </div>
    </div>
  );
}
