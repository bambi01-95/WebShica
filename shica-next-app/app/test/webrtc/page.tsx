'use client';
import React, { useEffect, useState, useRef } from 'react';

interface Peer {
  id: string;
  isHost: boolean;
  connection?: RTCPeerConnection;
  dataChannel?: RTCDataChannel;
}

export default function WebRTCPage() {
  const [ws, setWs] = useState<WebSocket | null>(null);
  const [messages, setMessages] = useState<string[]>([]);
  const [input, setInput] = useState('');
  const [clientId, setClientId] = useState<string>('');
  const [isHost, setIsHost] = useState(false);
  const [connectionStatus, setConnectionStatus] = useState<string>('接続中...');
  const [peers, setPeers] = useState<Map<string, Peer>>(new Map());
  
  const peersRef = useRef<Map<string, Peer>>(new Map());

  // ICE servers configuration
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  const addMessage = (message: string) => {
    setMessages(prev => [...prev, message]);
  };

  const createPeerConnection = (peerId: string, isInitiator: boolean): RTCPeerConnection => {
    const pc = new RTCPeerConnection(iceServers);
    
    pc.onicecandidate = (event) => {
      if (event.candidate && ws) {
        ws.send(JSON.stringify({
          type: 'ice-candidate',
          candidate: event.candidate,
          targetId: peerId
        }));
      }
    };

    pc.onconnectionstatechange = () => {
      console.log(`Connection with ${peerId}:`, pc.connectionState);
      if (pc.connectionState === 'connected') {
        setConnectionStatus('P2P接続済み');
      }
    };

    // データチャンネルの作成（イニシエーター側）
    if (isInitiator) {
      const dataChannel = pc.createDataChannel('chat', { ordered: true });
      setupDataChannel(dataChannel, peerId);
      
      peersRef.current.set(peerId, {
        ...peersRef.current.get(peerId)!,
        connection: pc,
        dataChannel
      });
    } else {
      // データチャンネルの受信（レシーバー側）
      pc.ondatachannel = (event) => {
        setupDataChannel(event.channel, peerId);
        peersRef.current.set(peerId, {
          ...peersRef.current.get(peerId)!,
          connection: pc,
          dataChannel: event.channel
        });
      };
    }

    return pc;
  };

  const setupDataChannel = (dataChannel: RTCDataChannel, peerId: string) => {
    dataChannel.onopen = () => {
      console.log(`Data channel with ${peerId} opened`);
      addMessage(`🔗 ${peerId} とP2P接続が確立されました`);
    };

    dataChannel.onmessage = (event) => {
      const data = JSON.parse(event.data);
      addMessage(`${data.fromId}: ${data.message}`);
    };

    dataChannel.onclose = () => {
      console.log(`Data channel with ${peerId} closed`);
      addMessage(`❌ ${peerId} との接続が切断されました`);
    };
  };

  const sendOfferToPeer = async (peerId: string) => {
    const peer = peersRef.current.get(peerId);
    if (!peer?.connection) return;

    try {
      const offer = await peer.connection.createOffer();
      await peer.connection.setLocalDescription(offer);
      
      if (ws) {
        ws.send(JSON.stringify({
          type: 'offer',
          offer,
          targetId: peerId
        }));
      }
    } catch (error) {
      console.error('Error creating offer:', error);
    }
  };

  const handleOffer = async (fromId: string, offer: RTCSessionDescriptionInit) => {
    let peer = peersRef.current.get(fromId);
    if (!peer) {
      const newPeer: Peer = { id: fromId, isHost: false };
      peersRef.current.set(fromId, newPeer);
      peer = newPeer;
    }

    if (!peer.connection) {
      peer.connection = createPeerConnection(fromId, false);
    }

    try {
      await peer.connection.setRemoteDescription(offer);
      const answer = await peer.connection.createAnswer();
      await peer.connection.setLocalDescription(answer);
      
      if (ws) {
        ws.send(JSON.stringify({
          type: 'answer',
          answer,
          targetId: fromId
        }));
      }
    } catch (error) {
      console.error('Error handling offer:', error);
    }
  };

  const handleAnswer = async (fromId: string, answer: RTCSessionDescriptionInit) => {
    const peer = peersRef.current.get(fromId);
    if (peer?.connection) {
      try {
        await peer.connection.setRemoteDescription(answer);
      } catch (error) {
        console.error('Error handling answer:', error);
      }
    }
  };

  const handleIceCandidate = async (fromId: string, candidate: RTCIceCandidateInit) => {
    const peer = peersRef.current.get(fromId);
    if (peer?.connection) {
      try {
        await peer.connection.addIceCandidate(candidate);
      } catch (error) {
        console.error('Error adding ICE candidate:', error);
      }
    }
  };

  useEffect(() => {
    // APIに一度GETしてサーバー初期化
    fetch('/api/test/webrtc');

    // WebSocketのURLを動的に決定
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${location.host}/api/test/webrtc`;
    console.log('Connecting to WebSocket:', wsUrl);
    
    const socket = new WebSocket(wsUrl);
    
    socket.onopen = () => {
      console.log('WebSocket connected');
      setConnectionStatus('シグナリングサーバー接続済み');
    };

    socket.onerror = (error) => {
      console.error('WebSocket error:', error);
      setConnectionStatus('WebSocket接続エラー');
    };
    
    socket.onmessage = async (event) => {
      try {
        const data = JSON.parse(event.data);
        
        switch (data.type) {
          case 'connected':
            setClientId(data.clientId);
            setIsHost(data.isHost);
            addMessage(data.message);
            break;
            
          case 'existing-peers':
            // 既存のピアに接続を開始
            for (const peerInfo of data.peers) {
              const peer: Peer = { id: peerInfo.id, isHost: peerInfo.isHost };
              peersRef.current.set(peerInfo.id, peer);
              peer.connection = createPeerConnection(peerInfo.id, true);
              sendOfferToPeer(peerInfo.id);
            }
            setPeers(new Map(peersRef.current));
            break;
            
          case 'peer-joined':
            const newPeer: Peer = { id: data.peerId, isHost: data.isNewPeerHost };
            peersRef.current.set(data.peerId, newPeer);
            setPeers(new Map(peersRef.current));
            addMessage(`👋 ${data.peerId} が参加しました`);
            break;
            
          case 'peer-left':
            peersRef.current.delete(data.peerId);
            setPeers(new Map(peersRef.current));
            addMessage(`👋 ${data.peerId} が退出しました`);
            break;
            
          case 'offer':
            await handleOffer(data.fromId, data.offer);
            break;
            
          case 'answer':
            await handleAnswer(data.fromId, data.answer);
            break;
            
          case 'ice-candidate':
            await handleIceCandidate(data.fromId, data.candidate);
            break;
            
          case 'chat':
            addMessage(`${data.fromId}${data.fromHost ? ' (Host)' : ''}: ${data.message}`);
            break;
        }
      } catch (error) {
        console.error('Error parsing message:', error);
      }
    };

    socket.onclose = (event) => {
      console.log('WebSocket closed:', event.code, event.reason);
      setConnectionStatus(`切断されました (Code: ${event.code})`);
      addMessage(`❌ WebSocket接続が切断されました (Code: ${event.code}, Reason: ${event.reason || 'Unknown'})`);
    };

    setWs(socket);
    return () => {
      socket.close();
      peersRef.current.forEach(peer => {
        peer.connection?.close();
        peer.dataChannel?.close();
      });
    };
  }, []);

  const sendMessage = () => {
    if (!input.trim()) return;

    // P2P接続が利用可能な場合はそちらを使用
    let sentViaP2P = false;
    peersRef.current.forEach(peer => {
      if (peer.dataChannel && peer.dataChannel.readyState === 'open') {
        peer.dataChannel.send(JSON.stringify({
          message: input,
          fromId: clientId
        }));
        sentViaP2P = true;
      }
    });

    if (sentViaP2P) {
      addMessage(`あなた: ${input}`);
    } else {
      // フォールバック: WebSocketサーバー経由
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({
          type: 'chat',
          message: input
        }));
        addMessage(`あなた (via server): ${input}`);
      }
    }

    setInput('');
  };

  const handleKeyPress = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter') {
      sendMessage();
    }
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'Arial, sans-serif' }}>
      <h1>WebRTC P2P Chat</h1>
      
      <div style={{ marginBottom: '10px' }}>
        <strong>状態:</strong> {connectionStatus}<br />
        <strong>あなたのID:</strong> {clientId}<br />
        <strong>ロール:</strong> {isHost ? 'ホスト' : 'ピア'}<br />
        <strong>接続中のピア:</strong> {peers.size}人
      </div>

      <div style={{
        border: '1px solid #ccc',
        padding: '10px',
        height: '300px',
        overflowY: 'auto',
        backgroundColor: '#f9f9f9',
        marginBottom: '10px'
      }}>
        {messages.map((message, index) => (
          <div key={index} style={{ marginBottom: '5px' }}>
            {message}
          </div>
        ))}
      </div>

      <div style={{ display: 'flex', gap: '10px' }}>
        <input
          type="text"
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyPress={handleKeyPress}
          placeholder="メッセージを入力"
          style={{
            flex: 1,
            padding: '8px',
            border: '1px solid #ccc',
            borderRadius: '4px'
          }}
        />
        <button
          onClick={sendMessage}
          style={{
            padding: '8px 16px',
            backgroundColor: '#007bff',
            color: 'white',
            border: 'none',
            borderRadius: '4px',
            cursor: 'pointer'
          }}
        >
          送信
        </button>
      </div>

      <div style={{ marginTop: '20px', fontSize: '14px', color: '#666' }}>
        <h3>接続状況:</h3>
        {Array.from(peers.values()).map(peer => (
          <div key={peer.id}>
            {peer.id} - {peer.connection?.connectionState || 'connecting'} 
            {peer.dataChannel?.readyState === 'open' && ' (P2P通信中)'}
          </div>
        ))}
      </div>
    </div>
  );
}
