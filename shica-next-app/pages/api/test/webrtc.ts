import type { NextApiRequest, NextApiResponse } from 'next';
import WebSocket, { WebSocketServer } from 'ws';

// 接続中のクライアント管理
interface Client {
  id: string;
  ws: WebSocket;
  isHost: boolean;
}

const clients = new Map<string, Client>();

export default function handler(req: NextApiRequest, res: NextApiResponse) {
  const server = (res.socket as any).server;

  if (!server.wss) {
    console.log('Starting WebRTC Signaling server...');
    const wss = new WebSocketServer({ noServer: true });

    server.on('upgrade', (request: any, socket: any, head: any) => {
      console.log('WebSocket upgrade request:', request.url);
      if (request.url === '/api/test/webrtc') {
        wss.handleUpgrade(request, socket, head, (ws) => {
          wss.emit('connection', ws, request);
        });
      } else {
        socket.destroy();
      }
    });

    wss.on('connection', (ws: WebSocket, request: any) => {
      console.log('WebSocket connection established from:', request.socket.remoteAddress);
      const clientId = Math.random().toString(36).substr(2, 9);
      const isHost = clients.size === 0; // 最初の接続者がホスト
      
      const client: Client = { id: clientId, ws, isHost };
      clients.set(clientId, client);
      
      console.log(`Client ${clientId} connected (${isHost ? 'Host' : 'Peer'}). Total: ${clients.size}`);

      // 接続直後に自分のIDとロールを送信
      ws.send(JSON.stringify({
        type: 'connected',
        clientId,
        isHost,
        message: isHost ? 'あなたがホストです' : `ホストに接続しました (ID: ${clientId})`
      }));

      // 既存のクライアントに新規参加を通知
      const existingClients = Array.from(clients.values()).filter(c => c.id !== clientId);
      existingClients.forEach(existingClient => {
        existingClient.ws.send(JSON.stringify({
          type: 'peer-joined',
          peerId: clientId,
          isNewPeerHost: isHost
        }));
      });

      // 新規参加者に既存のクライアント一覧を送信
      if (existingClients.length > 0) {
        ws.send(JSON.stringify({
          type: 'existing-peers',
          peers: existingClients.map(c => ({ id: c.id, isHost: c.isHost }))
        }));
      }

      ws.on('message', (data) => {
        try {
          const message = JSON.parse(data.toString());
          
          // WebRTCシグナリング用メッセージの中継
          if (message.type === 'offer' || message.type === 'answer' || message.type === 'ice-candidate') {
            const targetClient = clients.get(message.targetId);
            if (targetClient) {
              targetClient.ws.send(JSON.stringify({
                ...message,
                fromId: clientId
              }));
            }
          }
          
          // チャットメッセージの中継（P2P接続前のフォールバック）
          else if (message.type === 'chat') {
            clients.forEach((otherClient) => {
              if (otherClient.id !== clientId && otherClient.ws.readyState === WebSocket.OPEN) {
                otherClient.ws.send(JSON.stringify({
                  type: 'chat',
                  message: message.message,
                  fromId: clientId,
                  fromHost: client.isHost
                }));
              }
            });
          }
        } catch (error) {
          console.error('Failed to parse message:', error);
        }
      });

      ws.on('close', (code, reason) => {
        clients.delete(clientId);
        console.log(`Client ${clientId} disconnected. Code: ${code}, Reason: ${reason}. Total: ${clients.size}`);
        
        // 他のクライアントに切断を通知
        clients.forEach((otherClient) => {
          if (otherClient.ws.readyState === WebSocket.OPEN) {
            otherClient.ws.send(JSON.stringify({
              type: 'peer-left',
              peerId: clientId
            }));
          }
        });
      });

      ws.on('error', (error) => {
        console.error(`WebSocket error for client ${clientId}:`, error);
      });
    });

    server.wss = wss;
  }

  res.end();
}
