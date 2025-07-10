import React, { useState, useEffect, useRef } from "react";
// src/components/icons/TerminalIcon.tsx
const TerminalIcon: React.FC<React.SVGProps<SVGSVGElement>> = (
  props
) => (
  <svg viewBox="0 0 24 24" {...props}>
    <path
      d="M4 4h16v16H4V4zm2 2v12h12V6H6zm2.5 3l3 3-3 3"
      stroke="currentColor"
      strokeWidth="2"
      fill="none"
    />
  </svg>
);

// src/components/icons/CloseIcon.tsx
const CloseIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg viewBox="0 0 24 24" {...props}>
    <path d="M6 6l12 12M18 6L6 18" stroke="currentColor" strokeWidth="2" />
  </svg>
);

const MaximizeIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
    <svg
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
      {...props}
    >
      <rect x="4" y="4" width="16" height="16" />
    </svg>
  );

const MinimizeIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
    <svg
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
      {...props}
    >
      <path d="M6 12h12" />
    </svg>
  );

interface TerminalProps {
  title?: string;
  initialCommands?: string[];
  onCommand?: (command: string) => Promise<string> | string;
  className?: string;
  height?: string;
}

interface CommandHistory {
  command: string;
  output: string;
  timestamp: Date;
}

const Terminal: React.FC<TerminalProps> = ({
  title = "Terminal",
  initialCommands = [],
  onCommand,
  className = "",
  height = "400px",
}) => {
  const [history, setHistory] = useState<CommandHistory[]>([]);
  const [currentCommand, setCurrentCommand] = useState("");
  const [isMinimized, setIsMinimized] = useState(false);
  const [isMaximized, setIsMaximized] = useState(false);
  const [currentPath, setCurrentPath] = useState("~");
  const inputRef = useRef<HTMLInputElement>(null);
  const outputRef = useRef<HTMLDivElement>(null);

  // デフォルトコマンドハンドラー
  const defaultCommandHandler = async (cmd: string): Promise<string> => {
    const command = cmd.trim().toLowerCase();

    if (command === "clear") {
      setHistory([]);
      return "";
    }

    if (command === "help") {
      return `Available commands:
  clear     - Clear the terminal
  help      - Show this help message
  date      - Show current date and time
  echo      - Echo back the arguments
  whoami    - Show current user
  pwd       - Show current directory
  ls        - List directory contents`;
    }

    if (command === "date") {
      return new Date().toLocaleString();
    }

    if (command.startsWith("echo ")) {
      return command.substring(5);
    }

    if (command === "whoami") {
      return "user";
    }

    if (command === "pwd") {
      return currentPath;
    }

    if (command === "ls") {
      return "Documents  Downloads  Pictures  Music  Videos";
    }

    if (command === "") {
      return "";
    }

    return `Command not found: ${cmd}. Type 'help' for available commands.`;
  };

  // コマンド実行
  const executeCommand = async (cmd: string) => {
    if (cmd.trim() === "clear") {
      setHistory([]);
      return;
    }

    const handler = onCommand || defaultCommandHandler;
    const output = await handler(cmd);

    setHistory((prev) => [
      ...prev,
      {
        command: cmd,
        output,
        timestamp: new Date(),
      },
    ]);
  };

  // Enter キーでコマンド実行
  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === "Enter") {
      executeCommand(currentCommand);
      setCurrentCommand("");
    }
  };

  // 初期コマンドの実行
  useEffect(() => {
    initialCommands.forEach((cmd) => {
      executeCommand(cmd);
    });
  }, []);

  // 出力エリアの自動スクロール
  useEffect(() => {
    if (outputRef.current) {
      outputRef.current.scrollTop = outputRef.current.scrollHeight;
    }
  }, [history]);

  // フォーカス管理
  useEffect(() => {
    if (!isMinimized && inputRef.current) {
      inputRef.current.focus();
    }
  }, [isMinimized]);

  const terminalStyle = {
    height: isMaximized ? "100vh" : height,
    width: isMaximized ? "100vw" : "100%",
    position: isMaximized ? ("fixed" as const) : ("relative" as const),
    top: isMaximized ? 0 : undefined,
    left: isMaximized ? 0 : undefined,
    zIndex: isMaximized ? 9999 : undefined,
  };

  return (
    <div
      className={`bg-gray-900 text-green-400 font-mono text-sm border border-gray-700 rounded-lg shadow-lg ${className}`}
      style={terminalStyle}
    >
      {/* ヘッダー */}
      <div className="bg-gray-800 px-4 py-2 rounded-t-lg flex items-center justify-between border-b border-gray-700">
        <div className="flex items-center space-x-2">
          <TerminalIcon />
          <span className="text-gray-300">{title}</span>
        </div>
        <div className="flex items-center space-x-2">
          <button
            onClick={() => setIsMinimized(!isMinimized)}
            className="text-gray-400 hover:text-white"
          >
            <MinimizeIcon/>        
          </button>
          <button
            onClick={() => setIsMaximized(!isMaximized)}
            className="text-gray-400 hover:text-white"
          >
            <MaximizeIcon/>
          </button>
        </div>
      </div>

      {/* ターミナル本体 */}
      {!isMinimized && (
        <div className="p-4">
          {/* 出力エリア */}
          <div
            ref={outputRef}
            className="overflow-y-auto scrollbar-thin scrollbar-thumb-gray-600 scrollbar-track-gray-800"
            style={{ height: `calc(${height} - 80px)` }}
          >
            {history.map((entry, index) => (
              <div key={index} className="mb-2">
                <div className="flex">
                  <span className="text-blue-400">user@terminal:</span>
                  <span className="text-yellow-400 ml-1">{currentPath}</span>
                  <span className="text-white ml-1">$ {entry.command}</span>
                </div>
                {entry.output && (
                  <div className="text-gray-300 whitespace-pre-wrap ml-4">
                    {entry.output}
                  </div>
                )}
              </div>
            ))}
          </div>

          {/* 入力エリア */}
          <div className="flex items-center mt-2 border-t border-gray-700 pt-2">
            <span className="text-blue-400">user@terminal:</span>
            <span className="text-yellow-400 ml-1">{currentPath}</span>
            <span className="text-white ml-1">$</span>
            <input
              ref={inputRef}
              type="text"
              value={currentCommand}
              onChange={(e) => setCurrentCommand(e.target.value)}
              onKeyDown={handleKeyDown}
              className="flex-1 bg-transparent border-none outline-none text-white ml-2"
              placeholder="Enter command..."
            />
          </div>
        </div>
      )}
    </div>
  );
};

export default Terminal;
