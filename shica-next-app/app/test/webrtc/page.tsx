'use client';
import React, { useEffect, useState, useRef } from 'react';

interface Message {
  id: string;
  text: string;
  sender: 'A' | 'B';
  timestamp: Date;
}

interface UserSession {
  userId: 'A' | 'B';
  connection?: RTCPeerConnection;
  dataChannel?: RTCDataChannel;
  isConnected: boolean;
}

export default function WebRTCPage() {
  // 共通のメッセージ状態
  const [messages, setMessages] = useState<Message[]>([]);
  
  // ユーザーA用の状態
  const [userAInput, setUserAInput] = useState('');
  const [userASession, setUserASession] = useState<UserSession>({ userId: 'A', isConnected: false });
  
  // ユーザーB用の状態
  const [userBInput, setUserBInput] = useState('');
  const [userBSession, setUserBSession] = useState<UserSession>({ userId: 'B', isConnected: false });
  
  // 接続状態
  const [connectionStatus, setConnectionStatus] = useState<string>('接続待機中...');
  
  // Refs for RTCPeerConnection management
  const userAConnectionRef = useRef<RTCPeerConnection | null>(null);
  const userBConnectionRef = useRef<RTCPeerConnection | null>(null);
  const userADataChannelRef = useRef<RTCDataChannel | null>(null);
  const userBDataChannelRef = useRef<RTCDataChannel | null>(null);

  // ICE servers configuration
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  // メッセージ追加関数
  const addMessage = (text: string, sender: 'A' | 'B') => {
    const newMessage: Message = {
      id: Date.now().toString() + Math.random(),
      text,
      sender,
      timestamp: new Date()
    };
    setMessages(prev => [...prev, newMessage]);
  };

  // WebRTC接続の初期化
  const initializeWebRTC = async () => {
    try {
      // ユーザーA側の設定（オファー側）
      const pcA = new RTCPeerConnection(iceServers);
      const pcB = new RTCPeerConnection(iceServers);

      userAConnectionRef.current = pcA;
      userBConnectionRef.current = pcB;

      // データチャンネルの作成（A→B）
      const dataChannelA = pcA.createDataChannel('chatA', { ordered: true });
      userADataChannelRef.current = dataChannelA;

      // データチャンネルの受信設定（B側）
      pcB.ondatachannel = (event) => {
        const dataChannelB = event.channel;
        userBDataChannelRef.current = dataChannelB;

        dataChannelB.onopen = () => {
          console.log('Data channel B opened');
          setUserBSession(prev => ({ ...prev, isConnected: true }));
          setConnectionStatus('P2P接続完了');
        };

        dataChannelB.onmessage = (event) => {
          const data = JSON.parse(event.data);
          addMessage(data.message, 'A');
        };
      };

      // データチャンネルA側のイベント設定
      dataChannelA.onopen = () => {
        console.log('Data channel A opened');
        setUserASession(prev => ({ ...prev, isConnected: true }));
      };

      dataChannelA.onmessage = (event) => {
        const data = JSON.parse(event.data);
        addMessage(data.message, 'B');
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

      setConnectionStatus('WebRTC接続中...');

    } catch (error) {
      console.error('WebRTC initialization failed:', error);
      setConnectionStatus('接続失敗');
    }
  };

  // メッセージ送信関数
  const sendMessage = (sender: 'A' | 'B', message: string) => {
    if (!message.trim()) return;

    const dataChannel = sender === 'A' ? userADataChannelRef.current : userBDataChannelRef.current;
    
    if (dataChannel && dataChannel.readyState === 'open') {
      dataChannel.send(JSON.stringify({ message, sender }));
      addMessage(message, sender);
      
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
          <h2>ユーザーA</h2>
          <div style={{ fontSize: '12px' }}>
            状態: {userASession.isConnected ? '接続済み' : '未接続'}
          </div>
        </div>
        
        {/* チャット履歴 */}
        <div style={{ 
          flex: 1, 
          padding: '10px', 
          overflowY: 'auto',
          backgroundColor: 'white'
        }}>
          {messages.map((msg) => (
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
                  {msg.timestamp.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })}
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
            placeholder="メッセージを入力..."
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
            送信
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
          <h2>ユーザーB</h2>
          <div style={{ fontSize: '12px' }}>
            状態: {userBSession.isConnected ? '接続済み' : '未接続'}
          </div>
        </div>
        
        {/* チャット履歴 */}
        <div style={{ 
          flex: 1, 
          padding: '10px', 
          overflowY: 'auto',
          backgroundColor: 'white'
        }}>
          {messages.map((msg) => (
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
                  {msg.timestamp.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })}
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
            placeholder="メッセージを入力..."
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
            送信
          </button>
        </div>
      </div>
    </div>
  );
}
