'use client';

import React, { useState, useRef, useEffect } from 'react';

interface Message {
  id: string;
  sender: string;
  content: string;
  timestamp: string;
  topicName: string;
}

interface TopicHost {
  topicName: string;
  hostId: string;
  isActive: boolean;
  connectedUsers: Set<string>;
  messageQueue: Message[];
}

interface UserSession {
  userId: string;
  currentTopic: string;
  isConnected: boolean;
  messages: Message[];
  input: string;
}

const TopicBasedWebRTCChat = () => {
  const [currentUserId, setCurrentUserId] = useState('user1');
  const [availableTopics] = useState(['fish', 'beer', 'technology', 'music', 'sports']);
  const [customTopic, setCustomTopic] = useState('');
  
  const [userSessions, setUserSessions] = useState<UserSession[]>([
    { 
      userId: 'user1', 
      currentTopic: 'fish',
      isConnected: false, 
      messages: [], 
      input: ''
    },
    { 
      userId: 'user2', 
      currentTopic: 'fish',
      isConnected: false, 
      messages: [], 
      input: ''
    }
  ]);

  // トピックごとのホスト管理
  const [topicHosts, setTopicHosts] = useState<Map<string, TopicHost>>(new Map());

  // WebRTC接続管理 - トピックごと
  const topicHostConnectionsRef = useRef<Map<string, Map<string, RTCPeerConnection>>>(new Map());
  const topicHostDataChannelsRef = useRef<Map<string, Map<string, RTCDataChannel>>>(new Map());
  const userToTopicHostConnectionRef = useRef<Map<string, Map<string, RTCPeerConnection>>>(new Map());
  const userToTopicHostDataChannelRef = useRef<Map<string, Map<string, RTCDataChannel>>>(new Map());
  
  const messagesEndRef = useRef<HTMLDivElement>(null);

  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' }
    ]
  };

  useEffect(() => {
    // 初期トピックホストの作成
    initializeTopicHost('fish');
    return () => {
      cleanup();
    };
  }, []);

  useEffect(() => {
    scrollToBottom();
  }, [userSessions]);

  const cleanup = () => {
    // 全てのトピックホスト接続を閉じる
    topicHostConnectionsRef.current.forEach(topicConnections => {
      topicConnections.forEach(conn => conn.close());
    });
    topicHostConnectionsRef.current.clear();
    topicHostDataChannelsRef.current.clear();

    // 全てのユーザー接続を閉じる
    userToTopicHostConnectionRef.current.forEach(userConnections => {
      userConnections.forEach(conn => conn.close());
    });
    userToTopicHostConnectionRef.current.clear();
    userToTopicHostDataChannelRef.current.clear();
  };

  // 特定のトピック用ホストを初期化
  const initializeTopicHost = async (topicName: string) => {
    if (topicHosts.has(topicName)) {
      console.log(`🏢 Topic host for "${topicName}" already exists`);
      return;
    }

    console.log(`🏢 Initializing Topic Host for "${topicName}"...`);
    
    const newHost: TopicHost = {
      topicName,
      hostId: `host-${topicName}-${Date.now()}`,
      isActive: true,
      connectedUsers: new Set(),
      messageQueue: []
    };

    setTopicHosts(prev => new Map(prev.set(topicName, newHost)));
    
    // トピック専用の接続マップを初期化
    if (!topicHostConnectionsRef.current.has(topicName)) {
      topicHostConnectionsRef.current.set(topicName, new Map());
      topicHostDataChannelsRef.current.set(topicName, new Map());
    }
    const topic = topicHosts.get(topicName);
    const isActive = topic?.isActive ?? false;

    console.log(`🟢 Topic Host for "${topicName}" is now ${isActive ? "active" : "inactive"}`);
  };

  // ユーザーのトピック切り替え
  const switchUserTopic = async (userId: string, newTopic: string) => {
    const session = userSessions.find(s => s.userId === userId);
    if (!session) return;

    const oldTopic = session.currentTopic;

    // 古いトピックから切断
    if (session.isConnected && oldTopic) {
      await disconnectUserFromTopic(userId, oldTopic);
    }

    // 新しいトピックのホストを作成（存在しない場合）
    await initializeTopicHost(newTopic);

    // ユーザーセッションを更新
    setUserSessions(prev => 
      prev.map(s => 
        s.userId === userId 
          ? { 
              ...s, 
              currentTopic: newTopic,
              messages: [], // トピック変更時にメッセージをクリア
              isConnected: false
            }
          : s
      )
    );

    console.log(`🔄 ${userId} switched from "${oldTopic}" to "${newTopic}"`);
  };

  // 特定のトピックホストにユーザー接続を作成
  const createTopicHostToUserConnection = async (topicName: string, userId: string): Promise<boolean> => {
    const topicHost = topicHosts.get(topicName);
    if (!topicHost || !topicHost.isActive) {
      console.error(`❌ Topic host for "${topicName}" is not active`);
      return false;
    }

    try {
      console.log(`🔗 Topic Host "${topicName}" creating connection to ${userId}...`);
      
      const pc = new RTCPeerConnection(iceServers);
      
      // トピック専用の接続を保存
      const topicConnections = topicHostConnectionsRef.current.get(topicName) || new Map();
      topicConnections.set(userId, pc);
      topicHostConnectionsRef.current.set(topicName, topicConnections);

      // データチャンネルを作成
      const dataChannel = pc.createDataChannel(`${topicName}-host-to-${userId}`, {
        ordered: true
      });
      
      const topicChannels = topicHostDataChannelsRef.current.get(topicName) || new Map();
      topicChannels.set(userId, dataChannel);
      topicHostDataChannelsRef.current.set(topicName, topicChannels);

      // ホストデータチャンネルイベント
      dataChannel.onopen = () => {
        console.log(`✅ Topic "${topicName}" host connection to ${userId} established`);
        
        setTopicHosts(prev => {
          const newHosts = new Map(prev);
          const host = newHosts.get(topicName);
          if (host) {
            host.connectedUsers.add(userId);
            newHosts.set(topicName, { ...host });
          }
          return newHosts;
        });
        
        setUserSessions(prev => 
          prev.map(s => 
            s.userId === userId && s.currentTopic === topicName
              ? { ...s, isConnected: true }
              : s
          )
        );
      };

      dataChannel.onmessage = (event) => {
        const messageData = JSON.parse(event.data);
        console.log(`📨 Topic "${topicName}" host received message from ${userId}:`, messageData);
        handleMessageFromUserInTopic(messageData, userId, topicName);
      };

      dataChannel.onclose = () => {
        console.log(`🔴 Topic "${topicName}" host connection to ${userId} closed`);
        
        setTopicHosts(prev => {
          const newHosts = new Map(prev);
          const host = newHosts.get(topicName);
          if (host) {
            host.connectedUsers.delete(userId);
            newHosts.set(topicName, { ...host });
          }
          return newHosts;
        });
      };

      dataChannel.onerror = (error) => {
        console.error(`❌ Topic "${topicName}" host data channel error for ${userId}:`, error);
      };

      // ピア接続イベント
      pc.onconnectionstatechange = () => {
        console.log(`🔄 Topic "${topicName}" Host->${userId} connection state:`, pc.connectionState);
      };

      pc.onicecandidate = (event) => {
        if (event.candidate) {
          handleICECandidateForUserInTopic(topicName, userId, event.candidate);
        }
      };

      // offer/answerの交換をシミュレート
      await simulateOfferAnswerExchangeInTopic(pc, topicName, userId);
      
      return true;
    } catch (error) {
      console.error(`❌ Failed to create topic "${topicName}" host connection to ${userId}:`, error);
      return false;
    }
  };

  // ユーザーからトピックホストへの接続を作成
  const createUserToTopicHostConnection = async (topicName: string, userId: string): Promise<boolean> => {
    try {
      console.log(`🔗 ${userId} connecting to topic "${topicName}" host...`);
      
      const pc = new RTCPeerConnection(iceServers);
      
      // ユーザーの接続を保存
      if (!userToTopicHostConnectionRef.current.has(userId)) {
        userToTopicHostConnectionRef.current.set(userId, new Map());
        userToTopicHostDataChannelRef.current.set(userId, new Map());
      }
      
      const userConnections = userToTopicHostConnectionRef.current.get(userId)!;
      userConnections.set(topicName, pc);

      // 受信データチャンネルを処理
      pc.ondatachannel = (event) => {
        const dataChannel = event.channel;
        const userChannels = userToTopicHostDataChannelRef.current.get(userId)!;
        userChannels.set(topicName, dataChannel);
        
        dataChannel.onopen = () => {
          console.log(`✅ ${userId} connected to topic "${topicName}" host`);
        };

        dataChannel.onmessage = (event) => {
          const messageData = JSON.parse(event.data);
          console.log(`📨 ${userId} received message from topic "${topicName}" host:`, messageData);
          handleMessageFromTopicHost(messageData, userId, topicName);
        };

        dataChannel.onclose = () => {
          console.log(`🔴 ${userId} disconnected from topic "${topicName}" host`);
          setUserSessions(prev => 
            prev.map(s => 
              s.userId === userId && s.currentTopic === topicName
                ? { ...s, isConnected: false }
                : s
            )
          );
        };
      };

      pc.onconnectionstatechange = () => {
        console.log(`🔄 ${userId}->Topic "${topicName}" Host connection state:`, pc.connectionState);
      };

      return true;
    } catch (error) {
      console.error(`❌ Failed to create ${userId} connection to topic "${topicName}" host:`, error);
      return false;
    }
  };

  // WebRTC offer/answer交換のシミュレート
  const simulateOfferAnswerExchangeInTopic = async (hostPc: RTCPeerConnection, topicName: string, userId: string) => {
    setTimeout(async () => {
      try {
        // ユーザー接続を作成
        await createUserToTopicHostConnection(topicName, userId);
        const userConnections = userToTopicHostConnectionRef.current.get(userId);
        const userPc = userConnections?.get(topicName);
        
        if (userPc) {
          // ホストからオファーを作成
          const offer = await hostPc.createOffer();
          await hostPc.setLocalDescription(offer);
          await userPc.setRemoteDescription(offer);

          // ユーザーからアンサーを作成
          const answer = await userPc.createAnswer();
          await userPc.setLocalDescription(answer);
          await hostPc.setRemoteDescription(answer);

          console.log(`🤝 Offer/Answer exchange completed for ${userId} in topic "${topicName}"`);
        }
      } catch (error) {
        console.error(`❌ Offer/Answer exchange failed for ${userId} in topic "${topicName}":`, error);
      }
    }, 100);
  };

  // ICE候補の処理
  const handleICECandidateForUserInTopic = async (topicName: string, userId: string, candidate: RTCIceCandidate) => {
    setTimeout(async () => {
      const userConnections = userToTopicHostConnectionRef.current.get(userId);
      const userPc = userConnections?.get(topicName);
      if (userPc) {
        try {
          await userPc.addIceCandidate(candidate);
        } catch (error) {
          console.error(`❌ Failed to add ICE candidate for ${userId} in topic "${topicName}":`, error);
        }
      }
    }, 50);
  };

  // トピックホストがユーザーからのメッセージを処理してルーティング
  const handleMessageFromUserInTopic = (messageData: { message: Message }, fromUserId: string, topicName: string) => {
    const { message } = messageData;
    
    console.log(`🏢 Topic "${topicName}" host routing message from ${fromUserId}`);
    
    // ホストのメッセージキューに追加
    setTopicHosts(prev => {
      const newHosts = new Map(prev);
      const host = newHosts.get(topicName);
      if (host) {
        host.messageQueue.push(message);
        newHosts.set(topicName, { ...host });
      }
      return newHosts;
    });

    // 同じトピックの他の接続されたユーザーに配信
    const topicChannels = topicHostDataChannelsRef.current.get(topicName);
    if (topicChannels) {
      topicChannels.forEach((dataChannel, userId) => {
        if (userId !== fromUserId && dataChannel.readyState === 'open') {
          try {
            dataChannel.send(JSON.stringify(messageData));
            console.log(`📤 Topic "${topicName}" host broadcasted message to ${userId}`);
          } catch (error) {
            console.error(`❌ Failed to broadcast to ${userId} in topic "${topicName}":`, error);
          }
        }
      });
    }
  };

  // ユーザーがトピックホストからメッセージを受信
  const handleMessageFromTopicHost = (messageData: { message: Message }, userId: string, topicName: string) => {
    const { message } = messageData;
    
    setUserSessions(prev => {
      return prev.map(session => {
        // 現在のトピックと一致し、対象ユーザーの場合のみメッセージを追加
        if (session.userId === userId && session.currentTopic === topicName) {
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

  const createMessage = (sender: string, content: string, topicName: string): Message => {
    return {
      id: `${sender}-${Date.now()}-${Math.random()}`,
      sender,
      content,
      timestamp: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
      topicName
    };
  };

  const sendMessage = (sessionUserId: string) => {
    const session = userSessions.find(s => s.userId === sessionUserId);
    if (!session || !session.input.trim() || !session.isConnected) return;

    const message = createMessage(sessionUserId, session.input, session.currentTopic);
    const messageData = { message };

    // トピックホスト経由でメッセージを送信
    const userChannels = userToTopicHostDataChannelRef.current.get(sessionUserId);
    const userChannel = userChannels?.get(session.currentTopic);
    
    if (userChannel && userChannel.readyState === 'open') {
      try {
        userChannel.send(JSON.stringify(messageData));
        console.log(`📤 ${sessionUserId} sent message to topic "${session.currentTopic}" host`);
        
        // 送信者のローカル状態に即座に追加
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
        console.error(`❌ Failed to send message from ${sessionUserId} to topic "${session.currentTopic}":`, error);
      }
    } else {
      console.warn(`⚠️ ${sessionUserId} is not connected to topic "${session.currentTopic}" host`);
    }
  };

  const connectUserToCurrentTopic = async (userId: string) => {
    const session = userSessions.find(s => s.userId === userId);
    if (!session || !session.currentTopic) return;

    // トピックホストを初期化（存在しない場合）
    await initializeTopicHost(session.currentTopic);
    
    const success = await createTopicHostToUserConnection(session.currentTopic, userId);
    if (success) {
      console.log(`✅ ${userId} connection process initiated for topic "${session.currentTopic}"`);
    }
  };

  const disconnectUserFromTopic = async (userId: string, topicName: string) => {
    // トピックホスト接続を閉じる
    const topicConnections = topicHostConnectionsRef.current.get(topicName);
    if (topicConnections) {
      const hostConnection = topicConnections.get(userId);
      if (hostConnection) {
        hostConnection.close();
        topicConnections.delete(userId);
      }
      
      const topicChannels = topicHostDataChannelsRef.current.get(topicName);
      if (topicChannels) {
        topicChannels.delete(userId);
      }
    }

    // ユーザー接続を閉じる
    const userConnections = userToTopicHostConnectionRef.current.get(userId);
    if (userConnections) {
      const userConnection = userConnections.get(topicName);
      if (userConnection) {
        userConnection.close();
        userConnections.delete(topicName);
      }
      
      const userChannels = userToTopicHostDataChannelRef.current.get(userId);
      if (userChannels) {
        userChannels.delete(topicName);
      }
    }

    setUserSessions(prev => 
      prev.map(s => 
        s.userId === userId && s.currentTopic === topicName
          ? { ...s, isConnected: false }
          : s
      )
    );

    setTopicHosts(prev => {
      const newHosts = new Map(prev);
      const host = newHosts.get(topicName);
      if (host) {
        host.connectedUsers.delete(userId);
        newHosts.set(topicName, { ...host });
      }
      return newHosts;
    });

    console.log(`🔴 ${userId} disconnected from topic "${topicName}"`);
  };

  const addUser = () => {
    if (userSessions.length >= 12) return;
    
    const newUserId = `user${userSessions.length + 1}`;
    setUserSessions(prev => [...prev, {
      userId: newUserId,
      currentTopic: 'fish', // デフォルトトピック
      isConnected: false,
      messages: [],
      input: ''
    }]);
  };

  const removeUser = (userId: string) => {
    if (userSessions.length <= 2) return;
    
    const session = userSessions.find(s => s.userId === userId);
    if (session && session.isConnected) {
      disconnectUserFromTopic(userId, session.currentTopic);
    }
    
    setUserSessions(prev => prev.filter(s => s.userId !== userId));
  };

  const toggleUserConnection = async (userId: string) => {
    const session = userSessions.find(s => s.userId === userId);
    if (!session) return;

    if (session.isConnected) {
      await disconnectUserFromTopic(userId, session.currentTopic);
    } else {
      await connectUserToCurrentTopic(userId);
    }
  };

  const updateInput = (userId: string, value: string) => {
    setUserSessions(prev =>
      prev.map(s =>
        s.userId === userId ? { ...s, input: value } : s
      )
    );
  };

  const switchUser = (userId: string) => {
    setCurrentUserId(userId);
  };

  const addCustomTopic = () => {
    if (!customTopic.trim() || availableTopics.includes(customTopic)) return;
    
    availableTopics.push(customTopic);
    setCustomTopic('');
  };

  const getTopicStats = () => {
    const topicUsers = new Map<string, number>();
    const topicMessages = new Map<string, number>();
    
    userSessions.forEach(session => {
      if (session.isConnected) {
        topicUsers.set(session.currentTopic, (topicUsers.get(session.currentTopic) || 0) + 1);
      }
    });
    
    Array.from(topicHosts.values()).forEach(host => {
      topicMessages.set(host.topicName, host.messageQueue.length);
    });
    
    return { topicUsers, topicMessages };
  };

  const { topicUsers, topicMessages } = getTopicStats();

  return (
    <div style={{ padding: '20px', maxWidth: '1400px', margin: '0 auto' }}>
      {/* ヘッダー */}
      <div style={{ marginBottom: '20px', textAlign: 'center' }}>
        <h1>🌐 Topic-Based WebRTC Chat</h1>
        <p style={{ color: '#666', marginBottom: '15px' }}>
          各トピックが専用のホストを持ち、ユーザーはトピックごとに通信します
        </p>
        
        <div style={{ 
          display: 'flex', 
          justifyContent: 'center', 
          alignItems: 'center', 
          gap: '20px',
          marginBottom: '15px'
        }}>
          <div style={{ 
            padding: '10px 15px',
            backgroundColor: '#28a745',
            color: 'white',
            borderRadius: '8px',
            fontWeight: 'bold'
          }}>
            🏢 Active Topic Hosts: {topicHosts.size}
          </div>
          <div style={{ 
            padding: '10px 15px',
            backgroundColor: '#007bff',
            color: 'white',
            borderRadius: '8px'
          }}>
            👥 Total Users: {userSessions.length}
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
              {session.userId} {session.isConnected ? '🟢' : '🔴'} [{session.currentTopic}]
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

      {/* トピックホスト状態 */}
      <div style={{ 
        marginBottom: '20px', 
        padding: '15px', 
        backgroundColor: '#f8f9fa', 
        borderRadius: '8px',
        border: '2px solid #e9ecef'
      }}>
        <h4>🏢 Topic Hosts Status</h4>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '15px' }}>
          {Array.from(topicHosts.values()).map(host => (
            <div key={host.topicName} style={{ 
              padding: '10px', 
              backgroundColor: 'white', 
              borderRadius: '5px',
              border: '1px solid #ddd'
            }}>
              <div><strong>📝 Topic:</strong> {host.topicName}</div>
              <div><strong>🟢 Status:</strong> {host.isActive ? 'Active' : 'Inactive'}</div>
              <div><strong>👥 Users:</strong> {host.connectedUsers.size}</div>
              <div><strong>💬 Messages:</strong> {host.messageQueue.length}</div>
              <div style={{ fontSize: '12px', color: '#666' }}>
                Users: {Array.from(host.connectedUsers).join(', ') || 'None'}
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* カスタムトピック追加 */}
      <div style={{ 
        marginBottom: '20px', 
        padding: '10px', 
        backgroundColor: '#e7f3ff', 
        borderRadius: '5px' 
      }}>
        <h5>➕ Add Custom Topic</h5>
        <div style={{ display: 'flex', gap: '10px', alignItems: 'center' }}>
          <input
            type="text"
            value={customTopic}
            onChange={(e) => setCustomTopic(e.target.value)}
            placeholder="Enter custom topic name..."
            style={{ 
              padding: '5px 10px', 
              border: '1px solid #ccc', 
              borderRadius: '5px',
              flex: 1
            }}
          />
          <button
            onClick={addCustomTopic}
            disabled={!customTopic.trim() || availableTopics.includes(customTopic)}
            style={{ 
              padding: '5px 15px', 
              backgroundColor: '#28a745', 
              color: 'white', 
              border: 'none', 
              borderRadius: '5px',
              cursor: customTopic.trim() && !availableTopics.includes(customTopic) ? 'pointer' : 'not-allowed'
            }}
          >
            Add Topic
          </button>
        </div>
      </div>

      {/* ユーザーセッショングリッド */}
      <div style={{ 
        display: 'grid', 
        gridTemplateColumns: 'repeat(auto-fit, minmax(350px, 1fr))', 
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
            {/* ユーザーヘッダー */}
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
                  onClick={() => toggleUserConnection(session.userId)}
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

            {/* トピック選択 */}
            <div style={{ marginBottom: '10px' }}>
              <label style={{ display: 'block', fontWeight: 'bold', marginBottom: '5px' }}>
                📝 Current Topic: {session.currentTopic}
              </label>
              <select
                value={session.currentTopic}
                onChange={(e) => switchUserTopic(session.userId, e.target.value)}
                disabled={session.isConnected}
                style={{ 
                  width: '100%',
                  padding: '5px', 
                  border: '1px solid #ccc', 
                  borderRadius: '5px',
                  backgroundColor: session.isConnected ? '#f0f0f0' : 'white'
                }}
              >
                {availableTopics.map(topic => (
                  <option key={topic} value={topic}>
                    {topic} {topicUsers.get(topic) ? `(${topicUsers.get(topic)} users)` : ''}
                  </option>
                ))}
              </select>
              {session.isConnected && (
                <div style={{ fontSize: '12px', color: '#666', marginTop: '2px' }}>
                  Disconnect to change topic
                </div>
              )}
            </div>

            {/* メッセージエリア */}
            <div style={{ 
              height: '300px', 
              overflowY: 'auto', 
              border: '1px solid #ccc', 
              padding: '10px', 
              marginBottom: '10px',
              backgroundColor: '#fafafa'
            }}>
              {session.messages
                .filter(msg => msg.topicName === session.currentTopic)
                .map(msg => (
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
                    color: msg.sender === session.userId ? 'white' : 'black'
                  }}>
                    <div style={{ fontSize: '14px', fontWeight: 'bold', marginBottom: '2px' }}>
                      {msg.sender}
                      <span style={{ fontSize: '10px', marginLeft: '5px', opacity: 0.7 }}>
                        [{msg.topicName}]
                      </span>
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

            {/* 入力エリア */}
            {session.isConnected && (
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
                  placeholder={`Send message to ${session.currentTopic}...`}
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
                  disabled={!session.input.trim()}
                  style={{ 
                    padding: '8px 16px', 
                    backgroundColor: '#007bff', 
                    color: 'white', 
                    border: 'none', 
                    borderRadius: '20px',
                    cursor: session.input.trim() ? 'pointer' : 'not-allowed',
                    opacity: session.input.trim() ? 1 : 0.6
                  }}
                >
                  Send
                </button>
              </div>
            )}

            {!session.isConnected && (
              <div style={{ 
                textAlign: 'center', 
                padding: '20px', 
                color: '#6c757d',
                fontStyle: 'italic'
              }}>
                Select a topic and click "Connect" to join the discussion
              </div>
            )}
          </div>
        ))}
      </div>

      {/* アーキテクチャ情報 */}
      <div style={{ marginTop: '20px', padding: '15px', backgroundColor: '#e7f3ff', borderRadius: '5px', fontSize: '14px' }}>
        <h4>🏗️ Topic-Based Host Architecture</h4>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', gap: '15px' }}>
          <div>
            <p><strong>📡 Communication Flow:</strong></p>
            <ul>
              <li>Each topic has its own dedicated host</li>
              <li>Users connect to topic-specific hosts</li>
              <li>Messages are isolated by topic</li>
              <li>Dynamic topic creation and switching</li>
            </ul>
          </div>
          <div>
            <p><strong>✅ Advantages:</strong></p>
            <ul>
              <li><strong>Topic Isolation:</strong> Messages stay within topics</li>
              <li><strong>Scalable:</strong> Add topics without affecting others</li>
              <li><strong>Flexible:</strong> Users can switch topics dynamically</li>
              <li><strong>Organized:</strong> Separate discussions by topic</li>
            </ul>
          </div>
          <div>
            <p><strong>🔧 Features:</strong></p>
            <ul>
              <li>Custom topic creation</li>
              <li>Topic-specific message history</li>
              <li>Real-time topic user count</li>
              <li>Independent host management per topic</li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  );
};

export default TopicBasedWebRTCChat;
