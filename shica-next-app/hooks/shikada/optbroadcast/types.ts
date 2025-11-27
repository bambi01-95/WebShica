/**
 * Shica WebRTC OptBroadcast Library - Type Definitions
 * トピックベースのWebRTCブロードキャスト通信のための型定義
 */

export interface Message {
  id: string;
  sender: number;
  content: string;
}

export interface TopicHost {
  topicName: string;
  hostId: string;
  isActive: boolean;
  connectedUsers: Set<number>;
  messageQueue: Message[];
}

export interface Session {
  currentTopic: string;
// Address of the event-handler pointer stored in fields[2].
// (points to the location that holds the actual event-handler function pointer)
  eventHandlerPtrAddr: any;// C pointer to 
  isConnected: boolean;
  messages: Message[];
}

export interface Agent extends Session {
  uid: number;
}

export interface TopicStats {
  topicUsers: Map<string, number>;
  topicMessages: Map<string, number>;
}
