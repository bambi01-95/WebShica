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
  timestamp: String;
}

interface UserSession {
  userId: UppercaseLetter;
  connection?: RTCPeerConnection;
  dataChannel?: RTCDataChannel;
  isConnected: boolean;
  messages: Message[];
}

export default function WebRTCChat() {
  const [userSessions, setUserSessions] = useState<{ [key in UppercaseLetter]?: UserSession }>({
    A: { userId: 'A', isConnected: false, messages: [] },
    B: { userId: 'B', isConnected: false, messages: [] }
  });
  const [connectionStatus, setConnectionStatus] = useState<string>('Waiting for connection...');
  const [usersSessionsMap, setUsersSessionsMap] = useState<{ [key in UppercaseLetter]?: UserSession }>({});

  const userConnectionsRef = useRef<{ [key in UppercaseLetter]?: RTCPeerConnection }>({});
  const userDataChannelsRef = useRef<{ [key in UppercaseLetter]?: RTCDataChannel }>({});

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
  const addMessage = (msg: Message, storage: UppercaseLetter) => {
    setUserSessions(prev => {
      const userSession = prev[storage];
      if (userSession) {
        return {
          ...prev,
          [storage]: {
            ...userSession,
            messages: [...userSession.messages, msg]
          }
        };
      }
      return prev;
    });
  };
  // ICE servers configuration
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };
  // WebRTC connection initialization
  const initializeWebRTC = async () => {
    try {
      // User A configuration (offer side)
      const pcA = new RTCPeerConnection(iceServers);
      const pcB = new RTCPeerConnection(iceServers);

      userConnectionsRef.current['A'] = pcA;
      userConnectionsRef.current['B'] = pcB;

      // データチャンネルの作成（A→B用）
      const dataChannelA = pcA.createDataChannel('chatA', { ordered: true });
      userDataChannelsRef.current['A'] = dataChannelA;

      // データチャンネルの作成（B→A用）  
      const dataChannelB = pcB.createDataChannel('chatB', { ordered: true });
      userDataChannelsRef.current['B'] = dataChannelB;

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
        setUsersSessionsMap(prev => ({ ...prev, A: { userId: 'A', isConnected: true, messages: [] } }));
        setConnectionStatus('P2P Connected');
      };

      // データチャンネルB側のイベント設定
      dataChannelB.onopen = () => {
        console.log('Data channel B opened');
        setUsersSessionsMap(prev => ({ ...prev, B: { userId: 'B', isConnected: true, messages: [] } }));
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
  const sendMessage = (sender: UppercaseLetter, message: string) => {
    if (!message.trim()) return;

    const dataChannel = userDataChannelsRef.current[sender];
    
    if (dataChannel && dataChannel.readyState === 'open') {
      // 自分のメッセージを即座に表示
      console.log(`Sending message from ${sender}: ${message}`);
      const msg = createMessage({ text: message, sender });

      addMessage(msg, sender);

      // 相手に送信（相手側では受信として表示される）
      dataChannel.send(JSON.stringify(msg));

      // 入力フィールドをクリア
    }
  };

  const [time, setTime] = useState(new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }));
  useEffect(() => {
    initializeWebRTC();
    const timer = setInterval(() => {
      setTime(new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }));
    }, 60000); // 1分ごとに更新

    return () => {
      // クリーンアップ
      clearInterval(timer);
        Object.values(userConnectionsRef.current).forEach(pc => pc?.close());
    };
  }, []);


  const addUserSession = () => {
    const newUserId = String.fromCharCode(65 + Object.keys(userSessions).length) as UppercaseLetter; // 'A' is 65 in ASCII
    if (newUserId > 'Z') {
      alert('Maximum number of users reached (A-Z)');
      return;
    }
    setUserSessions(prev => ({
      ...prev,
      [newUserId]: { userId: newUserId, isConnected: false, messages: [] }
    }));
  }
  const userScrean = (userSession: UserSession) => {
    // smart phone
    return (
        <div className='w-[300px] h-[550px] bg-white border-10 border-black rounded-4xl p-4 m-2 overflow-y-auto' key={userSession.userId}>
            <h2>User {userSession.userId}{time}</h2>
            <p>Status: {userSession.isConnected ? 'Connected' : 'Disconnected'}</p>
            <div>
                {userSession.messages.map((msg, index) => (
                    <div key={index}>
                        <strong>{msg.sender}:</strong> {msg.text}
                    </div>
                ))}
            </div>
        </div>
    )
};

  return (
    <div className="container mx-auto p-4 bg-gray-100 min-h-screen">
      <h1>WebRTC Chat</h1>
      <p>Status: {connectionStatus}</p>
      <div className="flex flex-wrap">
        {Object.values(userSessions).map(userSession => userScrean(userSession))}
      </div>
    </div>
  );
}