'use client';
import React, { useState,useEffect } from 'react';

interface ErrorReport {
    msg: string;
}
interface Message {
    id: number;
    content: string;
    timestamp: Date;
}

interface Agent {
    id: number;
    messages: Message[];
    pc: RTCPeerConnection | null;
    channel: RTCDataChannel | null;
}

const UdpTestPage = () => { 
    const [errorReports, setErrorReports] = useState<ErrorReport[]>([]);
    const addErrorReport = (msg: string) => {
        setErrorReports([...errorReports, { msg }]);
        // Auto-remove after 5 seconds
        setTimeout(() => {
            setErrorReports(reports => reports.filter(report => report.msg !== msg));
        }, 5000);
    };

    const [agents, setAgents] = useState<Agent[]>([{ id: 1, messages: [], pc: null, channel: null }
                , { id: 2, messages: [], pc: null, channel: null }]);
    const [selectedFromAgent, setSelectedFromAgent] = useState<number | null>(null);
    const [selectedToAgent, setSelectedToAgent] = useState<number | null>(null);
    const [messageContent, setMessageContent] = useState<string>('');

    const [count, setCount] = useState<number>(0);
    useEffect(() => {
        // Initialize WebRTC connectio

        agents.map(async (agent) =>{
            console.log(`Setting up WebRTC for Agent ${agent.id}`);
            const pc = new RTCPeerConnection();
            const channel = pc.createDataChannel("chat");
            channel.onopen = () => console.log(`Agent ${agent.id} channel opened`);
            channel.onmessage = (event) => {
                const msg: Message = {
                    id: agent.id,
                    content: event.data,
                    timestamp: new Date()
                };
                setAgents(prevAgents => prevAgents.map(a => a.id === agent.id ? { ...a, messages: [...a.messages, msg] } : a));
            };
            pc.onicecandidate = (event) => {
                if (event.candidate) {
                    console.log(`Agent ${agent.id} ICE candidate:`, event.candidate);
                }
            };
            const offer = await pc.createOffer();
            await pc.setLocalDescription(offer);
            console.log(`Agent ${agent.id} offer:`, offer);
            const response = await fetch('/api/test/webrtc', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: agent.id, sdk: 'your-sdk-value', offer })
            });
            const data = await response.json();
            console.log(`Agent ${agent.id} received answer:`, data);
            await pc.setRemoteDescription(new RTCSessionDescription(data.answer));

            
            agent.pc = pc;
            agent.channel = channel;
        });
    }, []);

    const addAgent = () =>{
        if(agents.length >= 6) return; // Limit to 5 agents for simplicity
        setAgents([...agents, { id: agents.length + 1, messages: [], pc: null, channel: null }]);
    }


    const removeAgent = () =>{
        if(agents.length === 0) return;
        setAgents(agents.slice(0, -1));
    }

    const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
        setMessageContent(e.target.value);
    };

    const sendMessage = () => {


        if (selectedFromAgent === null || selectedToAgent === null || messageContent.trim() === '') return;
        console.log(`Agent ${selectedFromAgent} sends message to Agent ${selectedToAgent}: ${messageContent}`);
        // Simulate message sending by updating the recipient's message list
        setAgents(agents.map(agent => {
            if (agent.id === selectedToAgent) {
                const newMessage: Message = {
                    id: selectedFromAgent,
                    content: messageContent,
                    timestamp: new Date()
                };
                return { ...agent, messages: [...agent.messages, newMessage] };
            }
            if(agent.id === selectedFromAgent){
                const newMessage: Message = {
                    id: selectedFromAgent,
                    content: messageContent,
                    timestamp: new Date()
                };
                return { ...agent, messages: [...agent.messages, newMessage] };
            }   
            return agent;
        }));
        setMessageContent('');
    };

    
    const msgMonitor = (agent: Agent) => {
        return (
      <div key={agent.id} className='flex-1 min-w-80 bg-white border-2 border-gray-300 rounded-lg shadow-lg mx-2'>
        {/* LINEライクなヘッダー */}
        <div className='bg-green-500 text-white p-3 rounded-t-lg'>
          <h3 className='text-lg font-bold text-center'>Agent {agent.id}</h3>
          <div className='text-center text-sm opacity-90'>
            {agent.messages.length} messages
          </div>
        </div>
        
        {/* メッセージ表示エリア */}
        <div className='h-96 overflow-y-auto bg-gray-50 p-4'>
          {agent.messages.length === 0 ? (
            <div className='text-center text-gray-400 mt-20'>
              メッセージはまだありません
            </div>
          ) : (
            agent.messages.map((msg, index) => {
              const isFromThisAgent = msg.id === agent.id;
              return (
                <div key={index} className={`flex mb-3 ${isFromThisAgent ? 'justify-end' : 'justify-start'}`}>
                  {/* 相手のメッセージ（左側） */}
                  {!isFromThisAgent && (
                    <div className='flex items-start max-w-xs'>
                      <div className='w-8 h-8 bg-gray-400 rounded-full flex items-center justify-center text-white text-sm font-bold mr-2'>
                        {msg.id}
                      </div>
                      <div>
                        <div className='bg-white border border-gray-300 rounded-2xl px-4 py-2 shadow-sm'>
                          <div className='text-sm'>{msg.content}</div>
                        </div>
                        <div className='text-xs text-gray-500 mt-1 ml-2'>
                          {msg.timestamp.toLocaleTimeString()}
                        </div>
                      </div>
                    </div>
                  )}
                  
                  {/* 自分のメッセージ（右側） */}
                  {isFromThisAgent && (
                    <div className='flex items-start max-w-xs'>
                      <div>
                        <div className='bg-green-500 text-white rounded-2xl px-4 py-2 shadow-sm'>
                          <div className='text-sm'>{msg.content}</div>
                        </div>
                        <div className='text-xs text-gray-500 mt-1 mr-2 text-right'>
                          {msg.timestamp.toLocaleTimeString()}
                        </div>
                      </div>
                      <div className='w-8 h-8 bg-green-500 rounded-full flex items-center justify-center text-white text-sm font-bold ml-2'>
                        {msg.id}
                      </div>
                    </div>
                  )}
                </div>
              );
            })
          )}
        </div>
        
        {/* フッター */}
        <div className='border-t border-gray-200 p-2 bg-gray-100 rounded-b-lg'>
          <div className='text-xs text-gray-500 text-center'>
            Agent {agent.id}
          </div>
        </div>
      </div>
    );
    };
    return (
        <div>
            <h1 className='text-4xl font-bold'>Web RTC Test Page {count}</h1>
            <p className='text-2xl text-gray-600'>This is a placeholder for the UDP test page.</p>
            <div className='grid grid-cols-7 gap-4 m-4'>
                <button className='bg-gradient-to-r from-blue-500 to-blue-700 text-white p-2 rounded-lg' onClick={addAgent}>Add Agent</button>
                <button className='bg-gradient-to-r from-red-500 to-red-700 text-white p-2 rounded-lg' onClick={removeAgent}>Remove Agent</button>
                <select className='border p-2 rounded-lg' value={selectedFromAgent ?? ''} onChange={(e) => setSelectedFromAgent(Number(e.target.value))}>
                    <option value=''>Select From Agent</option>
                    {agents.map(agent => (
                        <option key={agent.id} value={agent.id}>Agent {agent.id}</option>
                    ))}
                </select>
                <select className='border p-2 rounded-lg' value={selectedToAgent ?? ''} onChange={(e) => setSelectedToAgent(Number(e.target.value))}>
                    <option value=''>Select To Agent</option>
                    {agents.map(agent => (
                        <option key={agent.id} value={agent.id}>Agent {agent.id}</option>
                    ))}
                </select>
                <input type="text" className='col-span-2 border p-2 rounded-lg' value={messageContent} onChange={(e) => {handleInputChange(e)}} />
                <button className='bg-gradient-to-r from-green-500 to-green-700 text-white p-2 rounded' onClick={sendMessage}>Send Message</button>
            </div>
            <div className='grid grid-cols-2 gap-4 bg-white p-4 mt-4 border rounded shadow-md'>
                {agents.map(agent => msgMonitor(agent))}
            </div>
        </div>
    );
};

export default UdpTestPage;