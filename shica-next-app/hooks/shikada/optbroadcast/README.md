# Shica WebRTC OptBroadcast Library

トピックベースのWebRTCブロードキャスト通信を管理するReactカスタムフックライブラリ

## 📦 概要

このライブラリは、複数ユーザー間でのトピックベースのリアルタイムメッセージング機能を提供します。各トピックにはホストが存在し、ホストを介してメッセージがルーティングされます。

## 🎯 特徴

- **トピックベース通信**: 複数のトピックを同時に管理可能
- **ホスト経由ルーティング**: 各トピックにホストが存在し、効率的にメッセージをブロードキャスト
- **WebRTC P2P通信**: 低遅延のリアルタイム通信を実現
- **TypeScript完全対応**: 型安全な開発体験
- **カスタムフック形式**: React Hooksパターンで簡単に統合可能

## 📂 構成

```
hooks/shikada/optbroadcast/
├── index.ts              # エクスポート集約
├── types.ts              # 型定義
├── useShicaWebRTC.ts     # メインのカスタムフック
└── README.md             # このファイル
```

## 🚀 使用方法

### 基本的な使い方

```tsx
'use client';
import React from 'react';
import { useShicaWebRTC } from '@/hooks/shikada/optbroadcast';

const MyApp = () => {
  const {
    userSessions,
    topicHosts,
    addUser,
    connectUserToTopic,
    sendMessage,
    disconnectUserFromTopic,
    getTopicStats,
  } = useShicaWebRTC();

  const handleAddUser = () => {
    addUser();
  };

  const handleConnect = async (uid: number) => {
    await connectUserToTopic(uid, 'my-topic');
  };

  const handleSend = (uid: number) => {
    sendMessage(uid, 'Hello, World!');
  };

  const handleDisconnect = async (uid: number) => {
    const session = userSessions.get(uid);
    if (session) {
      await disconnectUserFromTopic(uid, session.currentTopic);
    }
  };

  return (
    <div>
      <h1>Shica WebRTC Demo</h1>
      <button onClick={handleAddUser}>Add User</button>
      
      {[...userSessions.values()].map((user) => (
        <div key={user.uid}>
          <h3>User #{user.uid}</h3>
          <p>Topic: {user.currentTopic || 'None'}</p>
          <p>Status: {user.isConnected ? 'Connected' : 'Disconnected'}</p>
          
          <button onClick={() => handleConnect(user.uid)}>
            Connect to "my-topic"
          </button>
          <button onClick={() => handleSend(user.uid)}>
            Send Message
          </button>
          <button onClick={() => handleDisconnect(user.uid)}>
            Disconnect
          </button>
          
          <div>
            <h4>Messages:</h4>
            {user.messages.map((msg) => (
              <p key={msg.id}>
                [{msg.sender}]: {msg.content}
              </p>
            ))}
          </div>
        </div>
      ))}
    </div>
  );
};

export default MyApp;
```

## 🔧 API リファレンス

### `useShicaWebRTC()`

メインのカスタムフック

#### 戻り値

| プロパティ | 型 | 説明 |
|----------|-----|------|
| `userSessions` | `Map<number, Agent>` | 全ユーザーのセッション情報 |
| `topicHosts` | `Map<string, TopicHost>` | 全トピックホストの情報 |
| `addUser` | `() => void` | 新しいユーザーを追加 |
| `removeUser` | `(uid: number) => void` | ユーザーを削除 |
| `toggleUserConnection` | `(uid: number) => Promise<void>` | 接続状態をトグル |
| `initializeTopicHost` | `(topicName: string) => Promise<void>` | トピックホストを初期化 |
| `connectUserToTopic` | `(uid: number, topicName: string) => Promise<void>` | ユーザーをトピックに接続 |
| `disconnectUserFromTopic` | `(uid: number, topicName: string) => Promise<void>` | ユーザーをトピックから切断 |
| `sendMessage` | `(uid: number, content: string) => void` | メッセージを送信 |
| `getTopicStats` | `() => TopicStats` | トピックの統計情報を取得 |

### 型定義

#### `Agent`

```typescript
interface Agent extends Code, Session {
  uid: number;
}
```

#### `Session`

```typescript
interface Session {
  currentTopic: string;
  isConnected: boolean;
  messages: Message[];
}
```

#### `Message`

```typescript
interface Message {
  id: string;
  sender: number;
  content: string;
}
```

#### `TopicHost`

```typescript
interface TopicHost {
  topicName: string;
  hostId: string;
  isActive: boolean;
  connectedUsers: Set<number>;
  messageQueue: Message[];
}
```

## 🎓 使用例

### 1. ユーザーを追加してトピックに接続

```tsx
const { addUser, connectUserToTopic, userSessions } = useShicaWebRTC();

// ユーザーを追加
addUser(); // uid: 2が作成される

// トピックに接続
await connectUserToTopic(2, 'general-chat');
```

### 2. メッセージの送受信

```tsx
const { sendMessage, userSessions } = useShicaWebRTC();

// メッセージを送信
sendMessage(1, 'こんにちは!');

// メッセージを表示
const user = userSessions.get(1);
user?.messages.forEach(msg => {
  console.log(`[${msg.sender}]: ${msg.content}`);
});
```

### 3. トピックの統計情報を取得

```tsx
const { getTopicStats } = useShicaWebRTC();

const stats = getTopicStats();
console.log('接続ユーザー数:', stats.topicUsers);
console.log('メッセージ数:', stats.topicMessages);
```

## 🔍 デバッグ

ライブラリは詳細なコンソールログを出力します：

- `🏢` トピックホスト関連
- `🔗` 接続確立中
- `✅` 接続成功
- `🔴` 接続切断
- `📨` メッセージ受信
- `📤` メッセージ送信
- `❌` エラー

## ⚙️ 内部アーキテクチャ

### 接続フロー

```
1. initializeTopicHost(topicName)
   ↓
2. connectUserToTopic(uid, topicName)
   ↓
3. createTopicHostToUserConnection()
   ↓
4. createUserToTopicHostConnection()
   ↓
5. simulateOfferAnswerExchangeInTopic()
   ↓
6. 接続確立 (isConnected = true)
```

### メッセージフロー

```
User A (sendMessage)
   ↓
Topic Host (handleMessageFromUserInTopic)
   ↓
Broadcast to all users in topic
   ↓
User B, C, D... (handleMessageFromTopicHost)
```

## 🔒 制約事項

- 最大12ユーザーまで同時接続可能
- 最小1ユーザー必須
- トピック名は重複不可
- 同一ユーザーは1つのトピックにのみ接続可能

## 🛠 今後の拡張案

- [ ] Shica WASM連携 (`Module.ccall` 統合)
- [ ] パスワード保護されたトピック
- [ ] トピックの動的作成/削除UI
- [ ] メッセージ履歴の永続化
- [ ] ファイル送信機能
- [ ] ユーザー認証機能

## 📝 ライセンス

このライブラリはShica言語プロジェクトの一部です。

## 🤝 コントリビューション

改善提案やバグ報告は歓迎します！


## ❗️既知の問題と対処策

### グローバルブリッジ経由で `session error` が発生する
**原因**  
`globalThis` に登録した `_sendWebRtcBroadcast` などのブリッジ関数が初期レンダー時の `sendMessage` クロージャを保持し続け、後続の `userSessions` 更新を参照できなくなる。

**解決策**  
`useCallback` で `_addWebRtcBroadcast` / `_sendWebRtcBroadcast` / `_removeWebRtcBroadcast` を最新ステートに追従させ、`useEffect` の依存配列にそれらを含めて毎回 `globalThis` へ再登録・クリーンアップする。これにより常に最新の `userSessions` を参照でき、`session error` が解消される。