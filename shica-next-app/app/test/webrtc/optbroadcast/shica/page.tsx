'use client';

import React, { useEffect, useRef, useState } from 'react';

interface Message {
  id: string;
  sender: number;
  content: string;
}

interface TopicHost {
  topicName: string;
  hostId: string;
  isActive: boolean;
  connectedUsers: Set<number>;
  messageQueue: Message[];
}

interface Session {
  currentTopic: string;
  isConnected: boolean;
  messages: Message[];//　Shicaのqueueに入れる
}

interface Code{
  filename: string;
  code: string;
  compiled: boolean;
}

interface Agent extends Code, Session {
  uid: number;
}

const ShicaWebRTCPage = () => {
    const [currentUserId, setCurrentUserId] = useState('user1');
    const [customTopic, setCustomTopic] = useState('');
    
    const [userSessions, setUserSessions] = useState<Map<number, Agent>>(new Map([
      [1, { 
        uid: 1,
        filename: 'file1.ts',
        code: 'const a = 1;',
        compiled: false,
        currentTopic: 'fish',
        isConnected: false, 
        messages: [], 
      }]
    ]));
  
    // トピックごとのホスト管理
    const [topicHosts, setTopicHosts] = useState<Map<string, TopicHost>>(new Map());
  
    // WebRTC接続管理 - トピックごと
    /*
        RTCPeerConnection = 通信インフラ（接続確立・維持）
        RTCDataChannel = 実際のデータ送受信機能
    */
    const topicHostConnectionsRef = useRef<Map<string, Map<number, RTCPeerConnection>>>(new Map());
    const topicHostDataChannelsRef = useRef<Map<string, Map<number, RTCDataChannel>>>(new Map());
    const userToTopicHostConnectionRef = useRef<Map<number, Map<string, RTCPeerConnection>>>(new Map());
    const userToTopicHostDataChannelRef = useRef<Map<number, Map<string, RTCDataChannel>>>(new Map());
  
    // STUNサーバー設定
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

// クリーンアップ関数
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
  
      console.log(`🟢 Topic Host for "${topicName}" is now active`);
    };
  
// 特定のトピックホストにユーザー接続を作成
    const createTopicHostToUserConnection = async (topicName: string, uid: number): Promise<boolean> => {
      const topicHost = topicHosts.get(topicName);
      if (!topicHost || !topicHost.isActive) {
        console.error(`❌ Topic host for "${topicName}" is not active`);
        return false;
      }
  
      try {
        console.log(`🔗 Topic Host "${topicName}" creating connection to ${uid}...`);
        
        const pc = new RTCPeerConnection(iceServers);
        
        // トピック専用の接続を保存
        const topicConnections = topicHostConnectionsRef.current.get(topicName) || new Map();
        topicConnections.set(uid, pc);
        topicHostConnectionsRef.current.set(topicName, topicConnections);
  
        // データチャンネルを作成
        const dataChannel = pc.createDataChannel(`${topicName}-host-to-${uid}`, {
          ordered: true
        });
        
        const topicChannels = topicHostDataChannelsRef.current.get(topicName) || new Map();
        topicChannels.set(uid, dataChannel);
        topicHostDataChannelsRef.current.set(topicName, topicChannels);
  
        // ホストデータチャンネルイベント
        dataChannel.onopen = () => {
          console.log(`✅ Topic "${topicName}" host connection to ${uid} established`);
          
          setTopicHosts(prev => {
            const newHosts = new Map(prev);
            const host = newHosts.get(topicName);
            if (host) {
              host.connectedUsers.add(uid);
              newHosts.set(topicName, { ...host });
            }
            return newHosts;
          });
          
          setUserSessions(prev => 
          {
            const newSessions = new Map(prev);
            const session = newSessions.get(uid);
            if (session) {
              newSessions.set(uid, {
                ...session,
                isConnected: true,
                currentTopic: topicName
              });
            }
            return newSessions;
          });
        };
        // received message
        dataChannel.onmessage = (event) => {
          const messageData = JSON.parse(event.data);
          console.log(`📨 Topic "${topicName}" host received message from ${uid}:`, messageData);
          handleMessageFromUserInTopic(messageData, uid, topicName);
        };
  
        dataChannel.onclose = () => {
          console.log(`🔴 Topic "${topicName}" host connection to ${uid} closed`);
          
          setTopicHosts(prev => {
            const newHosts = new Map(prev);
            const host = newHosts.get(topicName);
            if (host) {
              host.connectedUsers.delete(uid);
              newHosts.set(topicName, { ...host });
            }
            return newHosts;
          });
        };
  
        dataChannel.onerror = (error) => {
          console.error(`❌ Topic "${topicName}" host data channel error for ${uid}:`, error);
        };
  
        // ピア接続イベント
        pc.onconnectionstatechange = () => {
          console.log(`🔄 Topic "${topicName}" Host->${uid} connection state:`, pc.connectionState);
        };
  
        pc.onicecandidate = (event) => {
          if (event.candidate) {
            handleICECandidateForUserInTopic(topicName, uid, event.candidate);
          }
        };
  
        // offer/answerの交換をシミュレート
        await simulateOfferAnswerExchangeInTopic(pc, topicName, uid);
        
        return true;
      } catch (error) {
        console.error(`❌ Failed to create topic "${topicName}" host connection to ${uid}:`, error);
        return false;
      }
    };
  
// ユーザーからトピックホストへの接続を作成
    const createUserToTopicHostConnection = async (topicName: string, uid: number): Promise<boolean> => {
      try {
        console.log(`🔗 ${uid} connecting to topic "${topicName}" host...`);
        
        const pc = new RTCPeerConnection(iceServers);
        
        // ユーザーの接続を保存
        if (!userToTopicHostConnectionRef.current.has(uid)) {
          userToTopicHostConnectionRef.current.set(uid, new Map());
          userToTopicHostDataChannelRef.current.set(uid, new Map());
        }
        
        const userConnections = userToTopicHostConnectionRef.current.get(uid)!;
        userConnections.set(topicName, pc);
  
        // 受信データチャンネルを処理
        pc.ondatachannel = (event) => {
          const dataChannel = event.channel;
          const userChannels = userToTopicHostDataChannelRef.current.get(uid)!;
          userChannels.set(topicName, dataChannel);
          
          dataChannel.onopen = () => {
            console.log(`✅ ${uid} connected to topic "${topicName}" host`);
          };
  
          dataChannel.onmessage = (event) => {
            const messageData = JSON.parse(event.data);
            console.log(`📨 ${uid} received message from topic "${topicName}" host:`, messageData);
            handleMessageFromTopicHost(messageData, uid, topicName);
          };
  
          dataChannel.onclose = () => {
            console.log(`🔴 ${uid} disconnected from topic "${topicName}" host`);
            const newSessions = new Map(userSessions);
            const session = newSessions.get(uid);
            if (session) {
              newSessions.set(uid, {
                ...session,
                isConnected: false
              });
              setUserSessions(newSessions);
            }
          };
        };
  
        pc.onconnectionstatechange = () => {
          console.log(`🔄 ${uid}->Topic "${topicName}" Host connection state:`, pc.connectionState);
        };
  
        return true;
      } catch (error) {
        console.error(`❌ Failed to create ${uid} connection to topic "${topicName}" host:`, error);
        return false;
      }
    };
  
// WebRTC offer/answer交換のシミュレート: これ何？
    const simulateOfferAnswerExchangeInTopic = async (hostPc: RTCPeerConnection, topicName: string, uid: number) => {
      setTimeout(async () => {
        try {
          // ユーザー接続を作成
          await createUserToTopicHostConnection(topicName, uid);
          const userConnections = userToTopicHostConnectionRef.current.get(uid);
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
  
            console.log(`🤝 Offer/Answer exchange completed for ${uid} in topic "${topicName}"`);
          }
        } catch (error) {
          console.error(`❌ Offer/Answer exchange failed for ${uid} in topic "${topicName}":`, error);
        }
      }, 100);
    };
  
// ICE候補の処理: これ何？
    const handleICECandidateForUserInTopic = async (topicName: string, uid: number, candidate: RTCIceCandidate) => {
      setTimeout(async () => {
        const userConnections = userToTopicHostConnectionRef.current.get(uid);
        const userPc = userConnections?.get(topicName);
        if (userPc) {
          try {
            await userPc.addIceCandidate(candidate);
          } catch (error) {
            console.error(`❌ Failed to add ICE candidate for ${uid} in topic "${topicName}":`, error);
          }
        }
      }, 50);
    };
  
// トピックホストがユーザーからのメッセージを処理してルーティング
    const handleMessageFromUserInTopic = (messageData: { message: Message }, fromUserId: number, topicName: string) => {
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
        topicChannels.forEach((dataChannel, uid) => {
          if (uid !== fromUserId && dataChannel.readyState === 'open') {
            try {
              dataChannel.send(JSON.stringify(messageData));
              console.log(`📤 Topic "${topicName}" host broadcasted message to ${uid}`);
            } catch (error) {
              console.error(`❌ Failed to broadcast to ${uid} in topic "${topicName}":`, error);
            }
          }
        });
      }
    };

// ユーザーがトピックホストからメッセージを受信
    const handleMessageFromTopicHost = (messageData: { message: Message }, uid: number, topicName: string) => {
      const { message } = messageData;
      const userSession = userSessions.get(uid);
      if (!userSession) return;
      if(userSession.currentTopic !== topicName) return; // 現在のトピックと異なる場合は無視
      console.log(`👤 ${uid} processing message from topic "${topicName}" host`);
        //Module.ccall('_web_rtc_broadcast_receive_', 'number', ['number', 'string'], [uid, JSON.stringify(message)]);//CCALL
        userSession.messages.push(message);
        setUserSessions(prev => new Map(prev).set(uid, { ...userSession }));
    };
  
    const createMessage = (sender: number, content: string, topicName: string): Message => {
      return {
        id: `${sender}-${Date.now()}-${Math.random()}`,
        sender,
        content,
      };
    };
    //`_sendWebRtcBroadcast(index, channel, msg)`;// JSCALL
    const sendMessage = (uid: number, content: string) => {
      const session = userSessions.get(uid);
      if (!session || !content.trim() || !session.isConnected) return;
  
      const message = createMessage(uid, content, session.currentTopic);
      const messageData = { message };
  
      // トピックホスト経由でメッセージを送信
      const userChannels = userToTopicHostDataChannelRef.current.get(uid);
      const userChannel = userChannels?.get(session.currentTopic);
      
      if (userChannel && userChannel.readyState === 'open') {
        try {
          userChannel.send(JSON.stringify(messageData));
          
          console.log(`📤 ${uid} sent message to topic "${session.currentTopic}" host`);
          // 送信者のローカル状態に即座に追加
          session.messages.push(message);
          setUserSessions(new Map(userSessions.set(uid, { ...session})));
        } catch (error) {
          console.error(`❌ Failed to send message from ${uid} to topic "${session.currentTopic}":`, error);
        }
      } else {
        console.warn(`⚠️ ${uid} is not connected to topic "${session.currentTopic}" host`);
      }
    };

// Shica: var chat = broadcast(topic);
//`_addWebRtcBroadcast(index, channel, password, ptr)`
    const connectUserToCurrentTopic = async (uid: number) => {
      const session = userSessions.get(uid);
      if (!session || !session.currentTopic) return;
  
      // トピックホストを初期化（存在しない場合）
      await initializeTopicHost(session.currentTopic);
      
      const success = await createTopicHostToUserConnection(session.currentTopic, uid);
      if (success) {
        console.log(`✅ ${uid} connection process initiated for topic "${session.currentTopic}"`);
      }
    };

//　Shica: chat.close()相当
    const disconnectUserFromTopic = async (uid: number, topicName: string) => {
    // 1. トピック側の接続を閉じる
    // 2. ユーザー側の接続を閉じる
    // 3. 状態を更新してUIに反映

      // トピックホスト接続を閉じる
      const topicConnections = topicHostConnectionsRef.current.get(topicName);
      if (topicConnections) {
        const hostConnection = topicConnections.get(uid);
        if (hostConnection) {
          hostConnection.close();
          topicConnections.delete(uid);
        }
        
        const topicChannels = topicHostDataChannelsRef.current.get(topicName);
        if (topicChannels) {
          topicChannels.delete(uid);
        }
      }
  
      // ユーザー接続を閉じる
      const userConnections = userToTopicHostConnectionRef.current.get(uid);
      if (userConnections) {
        const userConnection = userConnections.get(topicName);
        if (userConnection) {
          userConnection.close();
          userConnections.delete(topicName);
        }
        
        const userChannels = userToTopicHostDataChannelRef.current.get(uid);
        if (userChannels) {
          userChannels.delete(topicName);
        }
      }
      const session = userSessions.get(uid);
        if (!session) return;
        if(session.currentTopic !== topicName) return; // 現在のトピックと異なる場合は無視
            
        // 状態を更新
        session.isConnected = false;
        session.currentTopic = '';
        session.messages = [];
        setUserSessions(prev => new Map(prev).set(uid, { ...session }));
  
      setTopicHosts(prev => {
        const newHosts = new Map(prev);
        const host = newHosts.get(topicName);
        if (host) {
          host.connectedUsers.delete(uid);
          newHosts.set(topicName, { ...host });
        }
        return newHosts;
      });
  
      console.log(`🔴 ${uid} disconnected from topic "${topicName}"`);
    };

// ユーザーの追加: コードエディタとセッションを初期化
    const addUser = () => {
      if (userSessions.size >= 12) return;

      const newUserId = userSessions.size + 1;
      setUserSessions(prev => new Map(prev).set(newUserId, {
        uid: userSessions.size + 1,
        filename: `file${userSessions.size + 1}.ts`,
        code: 'const a = 1;',
        compiled: false,
        currentTopic: '',
        isConnected: false,
        messages: [],
      }));
    };

// ユーザーの削除
    const removeUser = (uid: number) => {
      if (userSessions.size <= 2) return;

      const session = userSessions.get(uid);
      if (session && session.isConnected) {
        disconnectUserFromTopic(uid, session.currentTopic);
      }

      setUserSessions(prev => {
        const newSessions = new Map(prev);
        newSessions.delete(uid);
        return newSessions;
      });
    };

// ユーザーの接続/切断トグル
    const toggleUserConnection = async (uid: number) => {
      const session = userSessions.get(uid);
      if (!session) return;
  
      if (session.isConnected) {
        await disconnectUserFromTopic(uid, session.currentTopic);
      } else {
        await connectUserToCurrentTopic(uid);
      }
    };

    // DEBUG用のコード
// トピックごとのユーザー数とメッセージ数を取得
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

    return <div>Shica WebRTC OptBroadcast Page</div>;
};
// Add user to channel
    // when channel not exist, create channel
// Remove user from channel
// Send message to channel
// Receive message from channel
export default ShicaWebRTCPage;