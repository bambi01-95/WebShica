/**
 * Shica WebRTC OptBroadcast Library - Custom Hook
 * トピックベースのWebRTCブロードキャスト通信を管理するカスタムフック
 * 
 * @description
 * このフックは、複数ユーザー間でのトピックベースのメッセージング機能を提供します。
 * 各トピックにはホストが存在し、ホストを介してメッセージがルーティングされます。
 * 
 * @example
 * ```tsx
 * const {
 *   userSessions,
 *   topicHosts,
 *   addUser,
 *   removeUser,
 *   sendMessage,
 *   connectUserToTopic,
 *   disconnectUserFromTopic,
 * } = useShicaWebRTC();
 * ```
 */

'use client';

import { useEffect, useRef, useState } from 'react';
import type { Agent, TopicHost, Message, TopicStats } from './types';
import { initialize } from 'next/dist/server/lib/render-server';

export const useShicaWebRTC = (Module: any, isReady: boolean) => {
  const [userSessions, setUserSessions] = useState<Map<number, Agent>>(new Map());

  const [topicHosts, setTopicHosts] = useState<Map<string, TopicHost>>(new Map());

  // WebRTC接続管理
  const topicHostConnectionsRef = useRef<Map<string, Map<number, RTCPeerConnection>>>(new Map());
  const topicHostDataChannelsRef = useRef<Map<string, Map<number, RTCDataChannel>>>(new Map());
  const userToTopicHostConnectionRef = useRef<Map<number, Map<string, RTCPeerConnection>>>(new Map());
  const userToTopicHostDataChannelRef = useRef<Map<number, Map<string, RTCDataChannel>>>(new Map());

  // STUNサーバー設定
  const iceServers = {
    iceServers: [
      { urls: 'stun:stun.l.google.com:19302' },
      { urls: 'stun:stun1.l.google.com:19302' },
    ],
  };
  useEffect(() => {
    userSessions.forEach((agent, uid) => {
      if (agent.isConnected && agent.currentTopic) {
        // ユーザーが接続されていて、現在のトピックがある場合
        const topicName = agent.currentTopic;
        const topicHost = topicHosts.get(topicName);
        if (topicHost && topicHost.isActive) {
          // トピックホストが存在し、アクティブな場合
          console.log(`🔄 Re-establishing connection for user ${uid} to topic "${topicName}"`);
          createTopicHostToUserConnection(topicName, uid);
        }
      }else{
        console.log(`ℹ️ User ${uid} is not connected or has no current topic`);
      }
    });
  }, [userSessions.size]);

  // クリーンアップ関数
  const cleanup = () => {
    // 全てのトピックホスト接続を閉じる
    topicHostConnectionsRef.current.forEach((topicConnections) => {
      topicConnections.forEach((conn) => conn.close());
    });
    topicHostConnectionsRef.current.clear();
    topicHostDataChannelsRef.current.clear();

    // 全てのユーザー接続を閉じる
    userToTopicHostConnectionRef.current.forEach((userConnections) => {
      userConnections.forEach((conn) => conn.close());
    });
    userToTopicHostConnectionRef.current.clear();
    userToTopicHostDataChannelRef.current.clear();
  };

  // 特定のトピック用ホストを初期化
  const initializeTopicHost = async (topicName: string) => {
    // ✅ 既存ホストがあっても最新の状態を確認
    const existingHost = topicHosts.get(topicName);
    if (existingHost?.isActive) {
      console.log(`🏢 Topic host for "${topicName}" is already active`);
      return; // アクティブな場合のみreturn
    }

    console.log(`🏢 Initializing Topic Host for "${topicName}"...`);

    const newHost: TopicHost = {
      topicName,
      hostId: `host-${topicName}-${Date.now()}`,
      isActive: true,
      connectedUsers: new Set(),
      messageQueue: [],
    };

    setTopicHosts((prev) => {
      const newMap = new Map(prev);
      newMap.set(topicName, newHost);
      return newMap;
    });

    // トピック専用の接続マップを初期化
    if (!topicHostConnectionsRef.current.has(topicName)) {
      topicHostConnectionsRef.current.set(topicName, new Map());
      topicHostDataChannelsRef.current.set(topicName, new Map());
    }

    console.log(`🟢 Topic Host for "${topicName}" is now active`);
  };

  // 特定のトピックホストにユーザー接続を作成
  const createTopicHostToUserConnection = async (topicName: string, uid: number): Promise<boolean> => {
    // topicHosts (state) は非同期更新なので、ref を使って即座にチェック
    const topicHostExists = topicHostConnectionsRef.current.has(topicName);
    if (!topicHostExists) {
      console.error(`❌ Topic host for "${topicName}" is not initialized (ref check)`);
      return false;
    }
    
    // state のチェックは参考情報として残す（後で利用可能）
    const topicHost = topicHosts.get(topicName);
    if (topicHost && !topicHost.isActive) {
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
        ordered: true,
      });

      const topicChannels = topicHostDataChannelsRef.current.get(topicName) || new Map();
      topicChannels.set(uid, dataChannel);
      topicHostDataChannelsRef.current.set(topicName, topicChannels);

      // ホストデータチャンネルイベント
      dataChannel.onopen = () => {
        console.log(`✅ Topic "${topicName}" host connection to ${uid} established`);

        setTopicHosts((prev) => {
          const newHosts = new Map(prev);
          const host = newHosts.get(topicName);
          if (host) {
            host.connectedUsers.add(uid);
            newHosts.set(topicName, { ...host });
          }
          return newHosts;
        });

        setUserSessions((prev) => {
          const newSessions = new Map(prev);
          const session = newSessions.get(uid);
          if (session) {
            newSessions.set(uid, {
              ...session,
              isConnected: true,
              currentTopic: topicName,
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

        setTopicHosts((prev) => {
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
          setUserSessions(prev => {
            const newSessions = new Map(prev);
            const session = newSessions.get(uid);
            if (session) {
              newSessions.set(uid, {
                ...session,
                isConnected: true,
                currentTopic: topicName,
              });
            }
            return newSessions;
          });
        };

        dataChannel.onmessage = (event) => {
          const messageData = JSON.parse(event.data);
          console.log(`📨 ${uid} received message from topic "${topicName}" host:`, messageData);
          handleMessageFromTopicHost(messageData, uid, topicName);
        };

        dataChannel.onclose = () => {
          console.log(`🔴 ${uid} disconnected from topic "${topicName}" host`);
          setUserSessions((prev) => {
            const newSessions = new Map(prev);
            const session = newSessions.get(uid);
            if (session) {
              newSessions.set(uid, {
                ...session,
                isConnected: false,
              });
            }
            return newSessions;
          });
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

  // WebRTC offer/answer交換のシミュレート
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

  // ICE候補の処理
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
    setTopicHosts((prev) => {
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
    console.log('\t\t get data');
    const { message } = messageData;

    // const userSession = userSessions.get(uid);
    // if(!userSession){console.error(`❌ No session found for user ${uid}`); return;}
    // if(userSession.currentTopic !== topicName){
    //   console.error(`❌ User ${uid} current topic mismatch: expected "${userSession.currentTopic}", got "${topicName}"`);
    //   return;
    // }
    // if(!Module || typeof Module.ccall !== 'function'){
    //   console.error(`❌ Module or Module.ccall is not available`);
    //   return;
    // }
    // if(userSession.eventHandlerPtrAddr === 0){
    //   console.error(`❌ User ${uid} has no valid event handler pointer address`);
    //   return;
    // }
    // console.log(`get message pointer address:`, userSession.eventHandlerPtrAddr);
    // Module.ccall('_web_rtc_broadcast_receive_', 'number', ['number', 'string'], [userSession.eventHandlerPtrAddr, message.content]);//CCALL

    setUserSessions((prev) => {
      const newSessions = new Map(prev);
      const userSession = newSessions.get(uid);
      if (!userSession) return prev;
      if (userSession.currentTopic !== topicName) return prev; // 現在のトピックと異なる場合は無視

      // 重複チェック：同じIDのメッセージが既に存在する場合はスキップ
      const isDuplicate = userSession.messages.some(msg => msg.id === message.id);
      if (isDuplicate) {
        console.log(`⚠️ Duplicate message detected for ${uid}, skipping: ${message.id}`);
        return prev;
      }

      console.log(`👤 ${uid} processing message from topic "${topicName}" host`);
      // Module.ccall は Shica WASM がロード済みの場合のみ実行
      if (Module && typeof Module.ccall === 'function') {
        console.log(`get message ${message.content} from ${userSession.currentTopic} pointer address:`, userSession.eventHandlerPtrAddr);
        Module.ccall('_web_rtc_broadcast_receive_', 'number', ['number', 'number', 'string', 'number'], [uid, userSession.eventHandlerPtrAddr, message.content, message.sender]);//CCALL
      }
      
      // イミュータブルな配列更新（スプレッド演算子で新しい配列を作成）
      // newSessions.set(uid, {
      //   ...userSession,
      //   messages: [...userSession.messages, message],
      // });
      // return newSessions;
      return prev;
    });
  };

  // メッセージIDカウンター（重複防止）
  const messageCounterRef = useRef(0);
  
  const createMessage = (sender: number, content: string, topicName: string): Message => {
    messageCounterRef.current += 1;
    return {
      id: `${sender}-${Date.now()}-${messageCounterRef.current}-${Math.random().toString(36).substr(2, 9)}`,
      sender,
      content,
    };
  };

  // メッセージ送信
  // Shica: `_sendWebRtcBroadcast(index, channel, msg)`;// JSCALL
  const sendMessage = (uid: number, content: string) => {
    console.log(`🦌 sendMessage() ${content}--${uid}`);
    const session = userSessions.get(uid);
    
    // 詳細デバッグログ
    console.log(`🔍 Debug: session exists=${!!session}, content="${content}", contentLength=${content.length}`);
    if (session) {
      console.log(`🔍 Debug: isConnected=${session.isConnected}, currentTopic="${session.currentTopic}"`);
      const userChannels = userToTopicHostDataChannelRef.current.get(uid);
      const userChannel = userChannels?.get(session.currentTopic);
      console.log(`🔍 Debug: userChannel exists=${!!userChannel}, readyState=${userChannel?.readyState}`);
    }
    
    if (!session || !content.trim() || !session.isConnected) {
      console.log('❌session error: missing session, empty content, or not connected');
      return;
    }
    console.log(`🦌 Sending message from user ${uid} in topic "${session.currentTopic}": ${content}`);

    const message = createMessage(uid, content, session.currentTopic);
    const messageData = { message };

    // トピックホスト経由でメッセージを送信
    const userChannels = userToTopicHostDataChannelRef.current.get(uid);
    const userChannel = userChannels?.get(session.currentTopic);

    if (userChannel && userChannel.readyState === 'open') {
      try {
        userChannel.send(JSON.stringify(messageData));

        console.log(`📤 ${uid} sent message to topic "${session.currentTopic}" host`);
        // 送信者のローカル状態に即座に追加（イミュータブル更新）
        setUserSessions((prev) => {
          const newSessions = new Map(prev);
          const updatedSession = newSessions.get(uid);
          if (updatedSession) {
            // 重複チェック
            const isDuplicate = updatedSession.messages.some(msg => msg.id === message.id);
            if (!isDuplicate) {
              newSessions.set(uid, {
                ...updatedSession,
                messages: [...updatedSession.messages, message],
              });
            }
          }
          return newSessions;
        });
      } catch (error) {
        console.error(`❌ Failed to send message from ${uid} to topic "${session.currentTopic}":`, error);
      }
    } else {
      console.warn(`⚠️ ${uid} is not connected to topic "${session.currentTopic}" host`);
    }
  };

  // Shica: var chat = broadcast(topic);
  // `_addWebRtcBroadcast(index, channel, password, ptr)`
  const connectUserToTopic = async (uid: number, topicName: string, eventHandlerPtrAddr: number): Promise<void> => {
    return new Promise(async (resolve, reject) => {
      // トピック名を設定
      setUserSessions((prev) => {
        const newSessions = new Map(prev);
        const session = newSessions.get(uid);
        //red color for debug
        console.log("   ",`ptr address set for user ${uid}:`, session?.eventHandlerPtrAddr);
        if (session) {
          newSessions.set(uid, {
            ...session,
            eventHandlerPtrAddr: eventHandlerPtrAddr,
            currentTopic: topicName,
          });
        }
        return newSessions;
      });

      // トピックホストを初期化（存在しない場合）
      console.log("🦌 1");
      await initializeTopicHost(topicName);
      console.log("🦌 2");
      
      // 接続確立を開始
      const success = await createTopicHostToUserConnection(topicName, uid);
      console.log("🦌 3");
      
      if (!success) {
        reject(new Error(`Failed to create connection for user ${uid} to topic ${topicName}`));
        return;
      }
      
      console.log(`🔄 ${uid} connection process initiated for topic "${topicName}"`);
      
      // データチャネルが open になるまで待つ
      const checkInterval = setInterval(() => {
        const userChannels = userToTopicHostDataChannelRef.current.get(uid);
        const dataChannel = userChannels?.get(topicName);
        
        if (dataChannel && dataChannel.readyState === 'open') {
          clearInterval(checkInterval);
          console.log(`✅ ${uid} data channel fully open for topic "${topicName}"`);
          resolve();
        }
      }, 50);
      
      // タイムアウト（5秒）
      setTimeout(() => {
        clearInterval(checkInterval);
        reject(new Error(`Timeout waiting for data channel to open for user ${uid}`));
      }, 5000);
    });
  };

  // Shica: chat.close()相当
  const disconnectUserFromTopic = async (uid: number, topicName: string) => {
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

    // 状態を更新
    setUserSessions((prev) => {
      const newSessions = new Map(prev);
      const session = newSessions.get(uid);
      if (!session) return prev;
      if (session.currentTopic !== topicName) return prev; // 現在のトピックと異なる場合は無視

      session.isConnected = false;
      session.currentTopic = '';
      session.messages = [];
      newSessions.set(uid, { ...session });
      return newSessions;
    });

    setTopicHosts((prev) => {
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
  const addUser = (id: number, eventHandlerPtrAddr: any) => {
    if (userSessions.size >= 12 && !userSessions.has(id)) {
      console.warn(`⚠️ Maximum user limit (12) reached, cannot add user ${id}`);
      return;
    }
    if(eventHandlerPtrAddr !== 0){
      console.log(`🛜 Adding user ${id} with event handler pointer address:`, eventHandlerPtrAddr);
    }else{
      console.log(`🛜 Adding user ${id} with dummy event handler pointer address`);
    }
    
    setUserSessions((prev) => {
      const newUserSessions = new Map(prev);
      
      // 既存ユーザーの場合はスキップ（既存の状態を保持）
      if (newUserSessions.has(id)) {
        console.log(`ℹ️ User ${id} already exists, keeping current state`);
        return prev;
      }
      
      // 新規ユーザーを追加
      newUserSessions.set(id, {
        uid: id,
        currentTopic: '',
        eventHandlerPtrAddr: eventHandlerPtrAddr,
        isConnected: false,
        messages: [],
      });
      
      console.log(`✅ User ${id} added to sessions`);
      return newUserSessions;
    });
  };

  // ユーザーの削除
  const removeUser = (uid: number) => {
    if (userSessions.size <= 1) return;

    const session = userSessions.get(uid);
    if (session && session.isConnected) {
      disconnectUserFromTopic(uid, session.currentTopic);
    }

    setUserSessions((prev) => {
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
      if (session.currentTopic) {
        await connectUserToTopic(uid, session.currentTopic,0);
      }
    }
  };

  // トピックごとの統計情報を取得
  const getTopicStats = (): TopicStats => {
    const topicUsers = new Map<string, number>();
    const topicMessages = new Map<string, number>();

    userSessions.forEach((session) => {
      if (session.isConnected) {
        topicUsers.set(session.currentTopic, (topicUsers.get(session.currentTopic) || 0) + 1);
      }
    });

    Array.from(topicHosts.values()).forEach((host) => {
      topicMessages.set(host.topicName, host.messageQueue.length);
    });

    return { topicUsers, topicMessages };
  };

  // 初期化とクリーンアップ
  useEffect(() => {
    initializeTopicHost('shica');
    return () => {
      cleanup();
    };
  }, []);

  return {
    // State
    userSessions,
    topicHosts,

    // User Management
    addUser,
    removeUser,
    toggleUserConnection,

    // Topic & Connection Management
    initializeTopicHost,
    connectUserToTopic,
    disconnectUserFromTopic,

    // Messaging
    sendMessage,

    // Stats
    getTopicStats,
  };
};
