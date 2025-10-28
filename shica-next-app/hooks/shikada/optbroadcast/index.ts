/**
 * Shica WebRTC OptBroadcast Library
 * トピックベースのWebRTCブロードキャスト通信ライブラリ
 * 
 * @module shica-webrtc-optbroadcast
 * @description
 * このライブラリは、複数ユーザー間でトピックベースのリアルタイム通信を実現します。
 * 各トピックにはホストが存在し、ホストを介してメッセージがブロードキャストされます。
 * 
 * @example
 * ```tsx
 * import { useShicaWebRTC } from '@/hooks/shikada/optbroadcast';
 * 
 * const MyComponent = () => {
 *   const {
 *     userSessions,
 *     topicHosts,
 *     addUser,
 *     connectUserToTopic,
 *     sendMessage,
 *     disconnectUserFromTopic,
 *   } = useShicaWebRTC();
 * 
 *   // ユーザーをトピックに接続
 *   const handleConnect = async (uid: number, topicName: string) => {
 *     await connectUserToTopic(uid, topicName);
 *   };
 * 
 *   // メッセージを送信
 *   const handleSend = (uid: number, message: string) => {
 *     sendMessage(uid, message);
 *   };
 * 
 *   return <div>...</div>;
 * };
 * ```
 */

export { useShicaWebRTC } from './useShicaWebRTC';
export type { Message, TopicHost, Session, Agent, TopicStats } from './types';
