# 🧪 useShicaWebRTC テストページ

WebRTC OptBroadcast ライブラリの全機能をテストできるインタラクティブな UI

## 🚀 アクセス方法

```
http://localhost:3000/test/webrtc/hooks
```

## 📋 機能一覧

### ✅ 実装済み機能

- ✨ **ユーザー管理**
  - ユーザーの追加（最大12人）
  - ユーザーの削除
  - ユーザー選択

- 🔗 **接続管理**
  - トピックへの接続
  - トピックからの切断
  - カスタムトピック名の設定

- 💬 **メッセージング**
  - メッセージ送信
  - メッセージ受信
  - メッセージ履歴表示
  - Enter キーで送信

- 📊 **統計情報**
  - アクティブユーザー数
  - トピック数
  - トピック別の接続ユーザー数
  - メッセージキュー数

- 🔧 **デバッグ**
  - User Sessions の状態表示
  - Topic Hosts の状態表示
  - リアルタイムコンソールログ

## 🎯 使い方

### 1. ユーザーを追加

「➕ ユーザーを追加」ボタンをクリックして複数のユーザーを作成します。

### 2. トピックに接続

各ユーザーカードの「🔗 接続」ボタンをクリックして、指定したトピックに接続します。

**期待されるログ：**
```
🦌 1
🏢 Topic host for "test-topic" already exists (or Initializing)
🦌 2
🔗 Topic Host "test-topic" creating connection to 0...
🦌 3
🔄 0 connection process initiated for topic "test-topic"
🔗 0 connecting to topic "test-topic" host...
🤝 Offer/Answer exchange completed for 0 in topic "test-topic"
🔄 Connection states: connecting → connected
✅ Topic "test-topic" host connection to 0 established
✅ 0 connected to topic "test-topic" host
✅ 0 data channel fully open for topic "test-topic"
✅ Added user 0
```

### 3. メッセージを送信

1. ユーザーカードをクリックして選択（青枠がつく）
2. 下部の入力欄にメッセージを入力
3. 「送信」ボタンまたは Enter キーで送信

**期待されるログ：**
```
🦌 sendMessage() Hello World--0
🔍 Debug: session exists=true, isConnected=true, readyState=open
🦌 Sending message from user 0 in topic "test-topic": Hello World
📤 0 sent message to topic "test-topic" host
🏢 Topic "test-topic" host routing message from 0
📤 Topic "test-topic" host broadcasted message to 1
📨 1 received message from topic "test-topic" host
👤 1 processing message from topic "test-topic" host
```

### 4. 接続を切断

「🔌 切断」ボタンをクリックして、トピックから切断します。

## 🧪 テストシナリオ

### シナリオ 1: 1対1通信

1. User 0 と User 1 を追加
2. 両方を同じトピックに接続
3. User 0 からメッセージ送信
4. User 1 でメッセージ受信を確認

### シナリオ 2: マルチキャストブロードキャスト

1. User 0, 1, 2 を追加
2. 全員を同じトピックに接続
3. User 0 からメッセージ送信
4. User 1 と User 2 でメッセージ受信を確認

### シナリオ 3: 異なるトピック

1. User 0, 1, 2 を追加
2. User 0, 1 を "topic-A" に接続
3. User 2 を "topic-B" に接続
4. User 0 からメッセージ送信
5. User 1 でのみメッセージ受信を確認（User 2 では受信しない）

### シナリオ 4: 接続/切断の繰り返し

1. User 0 を追加して接続
2. メッセージを送信
3. 切断
4. 再接続
5. 再度メッセージを送信

### シナリオ 5: 最大ユーザー数テスト

1. 12人のユーザーを追加
2. 全員を同じトピックに接続
3. 各ユーザーから順番にメッセージ送信
4. 全員がメッセージを受信することを確認

## 🔍 デバッグ方法

### ブラウザコンソールで確認

1. F12 キーで開発者ツールを開く
2. Console タブを表示
3. 以下のログが出力されます：
   - 🏢 トピックホスト関連
   - 🔗 接続確立中
   - ✅ 接続成功
   - 🔴 接続切断
   - 📨 メッセージ受信
   - 📤 メッセージ送信
   - 🔍 デバッグ情報
   - ❌ エラー

### 期待される成功パターン

```
# 接続成功
✅ 0 data channel fully open for topic "test-topic"

# 送信成功
🔍 Debug: session exists=true, isConnected=true, readyState=open
📤 0 sent message to topic "test-topic" host

# 受信成功
📨 1 received message from topic "test-topic" host
```

### よくあるエラー

```
# セッションエラー
❌session error: missing session, empty content, or not connected
→ 原因: addUser が呼ばれていない、または接続前に送信

# タイムアウトエラー
Timeout waiting for data channel to open for user 0
→ 原因: WebRTC 接続が5秒以内に確立できなかった

# チャンネルエラー
⚠️ 0 is not connected to topic "test-topic" host
→ 原因: データチャネルがまだ open になっていない
```

## 📝 デバッグ情報の見方

ページ最下部の「🔧 デバッグ情報」セクションを展開すると、内部状態を確認できます。

### User Sessions

```json
{
  "id": 0,
  "uid": 0,
  "currentTopic": "test-topic",
  "isConnected": true,
  "messages": 5
}
```

### Topic Hosts

```json
{
  "name": "test-topic",
  "topicName": "test-topic",
  "hostId": "host-test-topic-1234567890",
  "isActive": true,
  "connectedUsers": [0, 1, 2],
  "messageQueue": 10
}
```

## 🎨 UI 説明

### カラーコーディング

- 🟢 **緑**: 接続成功、アクティブ状態
- 🔴 **赤**: 切断、エラー
- 🔵 **青**: 選択中、アクション可能
- ⚫ **グレー**: 未接続、無効

### ユーザーカード

- **青枠**: 現在選択中（メッセージ送信対象）
- **白枠**: 未選択
- **🟢 Connected**: トピックに接続中
- **⚫ Disconnected**: 未接続

## 🚨 トラブルシューティング

### メッセージが送信できない

1. ユーザーが選択されているか確認
2. ユーザーが接続中（🟢 Connected）か確認
3. メッセージ入力欄が空でないか確認
4. コンソールで `🔍 Debug` ログを確認

### 接続ボタンが押せない

1. トピック名が入力されているか確認
2. ユーザーが既に接続中でないか確認
3. コンソールでエラーログを確認

### メッセージが受信できない

1. 送信者と受信者が同じトピックに接続しているか確認
2. 両方のユーザーが 🟢 Connected 状態か確認
3. コンソールで `📨 received message` ログを確認

## 🔗 関連ファイル

- `/hooks/shikada/optbroadcast/useShicaWebRTC.ts` - メインフック
- `/hooks/shikada/optbroadcast/types.ts` - 型定義
- `/hooks/shikada/optbroadcast/README.md` - ライブラリドキュメント

## 📚 参考資料

- [WebRTC API - MDN](https://developer.mozilla.org/en-US/docs/Web/API/WebRTC_API)
- [RTCPeerConnection - MDN](https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection)
- [RTCDataChannel - MDN](https://developer.mozilla.org/en-US/docs/Web/API/RTCDataChannel)
