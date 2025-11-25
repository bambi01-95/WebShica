'use client';

import React, { useState } from 'react';
import { useShicaWebRTC } from '@/hooks/shikada/optbroadcast';

/**
 * useShicaWebRTC テストページ
 * WebRTC OptBroadcast の全機能をテストできるインタラクティブなUI
 */
export default function WebRTCTestPage() {
  const {
    userSessions,
    topicHosts,
    addUser,
    removeUser,
    connectUserToTopic,
    disconnectUserFromTopic,
    sendMessage,
    getTopicStats,
  } = useShicaWebRTC(null, true); // Module不要のテスト用

  const [selectedUserId, setSelectedUserId] = useState<number | null>(null);
  const [topicName, setTopicName] = useState('test-topic');
  const [messageContent, setMessageContent] = useState('');

  // ユーザー追加
  const handleAddUser = () => {
    const newId = userSessions.size;
    addUser(newId);
    console.log(`✅ Added user ${newId}`);
  };

  // ユーザー削除
  const handleRemoveUser = (uid: number) => {
    removeUser(uid);
    console.log(`🗑️ Removed user ${uid}`);
    if (selectedUserId === uid) {
      setSelectedUserId(null);
    }
  };

  // トピックに接続
  const handleConnect = async (uid: number) => {
    try {
      await connectUserToTopic(uid, topicName);
      console.log(`✅ User ${uid} connected to ${topicName}`);
    } catch (error) {
      console.error(`❌ Connection failed:`, error);
      alert(`接続失敗: ${error}`);
    }
  };

  // トピックから切断
  const handleDisconnect = async (uid: number) => {
    const session = userSessions.get(uid);
    if (session && session.currentTopic) {
      await disconnectUserFromTopic(uid, session.currentTopic);
      console.log(`🔴 User ${uid} disconnected from ${session.currentTopic}`);
    }
  };

  // メッセージ送信
  const handleSendMessage = () => {
    if (selectedUserId === null) {
      alert('ユーザーを選択してください');
      return;
    }
    if (!messageContent.trim()) {
      alert('メッセージを入力してください');
      return;
    }
    sendMessage(selectedUserId, messageContent);
    setMessageContent('');
  };

  // 統計情報
  const stats = getTopicStats();

  return (
    <div className="min-h-screen bg-gray-100 p-8">
      <div className="max-w-7xl mx-auto">
        <h1 className="text-4xl font-bold mb-8 text-gray-800">
          🧪 useShicaWebRTC テストページ
        </h1>

        {/* コントロールパネル */}
        <div className="bg-white rounded-lg shadow-md p-6 mb-6">
          <h2 className="text-2xl font-semibold mb-4 text-gray-700">
            📋 コントロールパネル
          </h2>
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-2">
                トピック名
              </label>
              <input
                type="text"
                value={topicName}
                onChange={(e) => setTopicName(e.target.value)}
                className="w-full px-4 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
                placeholder="例: test-topic"
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-2">
                ユーザー管理
              </label>
              <button
                onClick={handleAddUser}
                disabled={userSessions.size >= 12}
                className="w-full px-4 py-2 bg-green-500 text-white rounded-md hover:bg-green-600 disabled:bg-gray-400 disabled:cursor-not-allowed transition-colors"
              >
                ➕ ユーザーを追加 ({userSessions.size}/12)
              </button>
            </div>
          </div>
        </div>

        {/* 統計情報 */}
        <div className="bg-white rounded-lg shadow-md p-6 mb-6">
          <h2 className="text-2xl font-semibold mb-4 text-gray-700">
            📊 統計情報
          </h2>
          <div className="grid grid-cols-2 gap-4">
            <div className="bg-blue-50 p-4 rounded-lg">
              <p className="text-sm text-blue-600 font-medium">トピック数</p>
              <p className="text-3xl font-bold text-blue-700">
                {topicHosts.size}
              </p>
            </div>
            <div className="bg-purple-50 p-4 rounded-lg">
              <p className="text-sm text-purple-600 font-medium">
                アクティブユーザー
              </p>
              <p className="text-3xl font-bold text-purple-700">
                {Array.from(userSessions.values()).filter((s) => s.isConnected)
                  .length}
              </p>
            </div>
          </div>
          <div className="mt-4">
            <h3 className="font-semibold text-gray-700 mb-2">
              トピック別統計
            </h3>
            {Array.from(topicHosts.values()).map((host) => (
              <div
                key={host.topicName}
                className="bg-gray-50 p-3 rounded mb-2 flex justify-between"
              >
                <span className="font-medium">{host.topicName}</span>
                <div className="flex gap-4 text-sm text-gray-600">
                  <span>👥 {host.connectedUsers.size} ユーザー</span>
                  <span>📬 {host.messageQueue.length} メッセージ</span>
                  <span
                    className={`px-2 py-1 rounded ${
                      host.isActive
                        ? 'bg-green-100 text-green-700'
                        : 'bg-red-100 text-red-700'
                    }`}
                  >
                    {host.isActive ? '🟢 Active' : '🔴 Inactive'}
                  </span>
                </div>
              </div>
            ))}
          </div>
        </div>

        {/* ユーザーリスト */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
          {Array.from(userSessions.values()).map((user) => (
            <div
              key={user.uid}
              className={`bg-white rounded-lg shadow-md p-6 border-2 transition-all ${
                selectedUserId === user.uid
                  ? 'border-blue-500 shadow-lg'
                  : 'border-transparent'
              }`}
              onClick={() => setSelectedUserId(user.uid)}
            >
              <div className="flex justify-between items-start mb-4">
                <div>
                  <h3 className="text-xl font-bold text-gray-800">
                    👤 User #{user.uid}
                  </h3>
                  <div className="mt-2 space-y-1">
                    <p className="text-sm text-gray-600">
                      📍 Topic:{' '}
                      <span className="font-medium">
                        {user.currentTopic || 'None'}
                      </span>
                    </p>
                    <p className="text-sm">
                      状態:{' '}
                      <span
                        className={`px-2 py-1 rounded text-xs font-semibold ${
                          user.isConnected
                            ? 'bg-green-100 text-green-700'
                            : 'bg-gray-100 text-gray-700'
                        }`}
                      >
                        {user.isConnected ? '🟢 Connected' : '⚫ Disconnected'}
                      </span>
                    </p>
                  </div>
                </div>
                <button
                  onClick={(e) => {
                    e.stopPropagation();
                    handleRemoveUser(user.uid);
                  }}
                  className="text-red-500 hover:text-red-700 text-2xl"
                  title="ユーザーを削除"
                >
                  🗑️
                </button>
              </div>

              {/* コントロールボタン */}
              <div className="flex gap-2 mb-4">
                {!user.isConnected ? (
                  <button
                    onClick={(e) => {
                      e.stopPropagation();
                      handleConnect(user.uid);
                    }}
                    className="flex-1 px-4 py-2 bg-blue-500 text-white rounded-md hover:bg-blue-600 transition-colors text-sm font-medium"
                  >
                    🔗 接続
                  </button>
                ) : (
                  <button
                    onClick={(e) => {
                      e.stopPropagation();
                      handleDisconnect(user.uid);
                    }}
                    className="flex-1 px-4 py-2 bg-red-500 text-white rounded-md hover:bg-red-600 transition-colors text-sm font-medium"
                  >
                    🔌 切断
                  </button>
                )}
              </div>

              {/* メッセージリスト */}
              <div className="bg-gray-50 rounded-lg p-3 max-h-48 overflow-y-auto">
                <h4 className="text-sm font-semibold text-gray-700 mb-2">
                  💬 メッセージ履歴 ({user.messages.length})
                </h4>
                {user.messages.length === 0 ? (
                  <p className="text-xs text-gray-400 italic">
                    メッセージはまだありません
                  </p>
                ) : (
                  <div className="space-y-2">
                    {user.messages.map((msg) => (
                      <div
                        key={msg.id}
                        className="bg-white p-2 rounded border border-gray-200"
                      >
                        <div className="flex justify-between items-start mb-1">
                          <span className="text-xs font-semibold text-blue-600">
                            From: User #{msg.sender}
                          </span>
                          <span className="text-xs text-gray-400">
                            {new Date(
                              parseInt(msg.id.split('-')[1])
                            ).toLocaleTimeString()}
                          </span>
                        </div>
                        <p className="text-sm text-gray-700">{msg.content}</p>
                      </div>
                    ))}
                  </div>
                )}
              </div>
            </div>
          ))}
        </div>

        {/* メッセージ送信パネル */}
        {selectedUserId !== null && (
          <div className="bg-white rounded-lg shadow-md p-6 sticky bottom-4">
            <h2 className="text-2xl font-semibold mb-4 text-gray-700">
              📤 メッセージ送信 (User #{selectedUserId})
            </h2>
            <div className="flex gap-4">
              <input
                type="text"
                value={messageContent}
                onChange={(e) => setMessageContent(e.target.value)}
                onKeyPress={(e) => {
                  if (e.key === 'Enter') {
                    handleSendMessage();
                  }
                }}
                placeholder="メッセージを入力..."
                className="flex-1 px-4 py-3 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
              />
              <button
                onClick={handleSendMessage}
                disabled={!messageContent.trim()}
                className="px-8 py-3 bg-blue-500 text-white rounded-md hover:bg-blue-600 disabled:bg-gray-400 disabled:cursor-not-allowed transition-colors font-semibold"
              >
                送信
              </button>
            </div>
            <p className="mt-2 text-sm text-gray-500">
              💡 Enter キーでも送信できます
            </p>
          </div>
        )}

        {/* 使い方ガイド */}
        {userSessions.size === 0 && (
          <div className="bg-blue-50 border-2 border-blue-200 rounded-lg p-6 mt-6">
            <h3 className="text-lg font-semibold text-blue-800 mb-3">
              📖 使い方
            </h3>
            <ol className="list-decimal list-inside space-y-2 text-blue-700">
              <li>「➕ ユーザーを追加」ボタンで複数のユーザーを作成</li>
              <li>各ユーザーの「🔗 接続」ボタンでトピックに接続</li>
              <li>ユーザーをクリックして選択</li>
              <li>下部の入力欄からメッセージを送信</li>
              <li>
                他の接続中ユーザーにメッセージがブロードキャストされます
              </li>
            </ol>
          </div>
        )}
      </div>

      {/* デバッグ情報 */}
      <details className="mt-8 bg-gray-800 text-gray-100 rounded-lg p-4">
        <summary className="cursor-pointer font-semibold">
          🔧 デバッグ情報（開発者向け）
        </summary>
        <div className="mt-4 space-y-4">
          <div>
            <h4 className="font-semibold mb-2">User Sessions:</h4>
            <pre className="bg-gray-900 p-3 rounded overflow-x-auto text-xs">
              {JSON.stringify(
                Array.from(userSessions.entries()).map(([id, user]) => ({
                  id,
                  ...user,
                  messages: user.messages.length,
                })),
                null,
                2
              )}
            </pre>
          </div>
          <div>
            <h4 className="font-semibold mb-2">Topic Hosts:</h4>
            <pre className="bg-gray-900 p-3 rounded overflow-x-auto text-xs">
              {JSON.stringify(
                Array.from(topicHosts.entries()).map(([name, host]) => ({
                  name,
                  ...host,
                  connectedUsers: Array.from(host.connectedUsers),
                  messageQueue: host.messageQueue.length,
                })),
                null,
                2
              )}
            </pre>
          </div>
        </div>
      </details>
    </div>
  );
}
