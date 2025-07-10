import React, { useState, useEffect, useRef } from "react";

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
const DownloadIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    strokeLinecap="round"
    strokeLinejoin="round"
    {...props}
  >
    {/* 下向き矢印 */}
    <polyline points="7 10 12 15 17 10" />
    <line x1="12" y1="15" x2="12" y2="3" />
    {/* ベースライン */}
    <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
  </svg>
);

const CopyIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    strokeLinecap="round"
    strokeLinejoin="round"
    {...props}
  >
    {/* 手前の四角 */}
    <rect x="9" y="9" width="13" height="13" rx="2" />
    {/* 背後の四角 */}
    <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1" />
  </svg>
);

const Trash2Icon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    strokeLinecap="round"
    strokeLinejoin="round"
    {...props}
  >
    {/* 上部バー */}
    <path d="M3 6h18" />
    {/* フタ */}
    <path d="M8 6V4h8v2" />
    {/* 本体 */}
    <rect x="5" y="6" width="14" height="14" rx="2" />
    {/* 中の仕切り線 */}
    <line x1="10" y1="11" x2="10" y2="17" />
    <line x1="14" y1="11" x2="14" y2="17" />
  </svg>
);

const XIcon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    strokeLinecap="round"
    strokeLinejoin="round"
    {...props}
  >
    <line x1="18" y1="6" x2="6" y2="18" />
    <line x1="6" y1="6" x2="18" y2="18" />
  </svg>
);

const Code2Icon: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    strokeLinecap="round"
    strokeLinejoin="round"
    {...props}
  >
    {/* “<” */}
    <polyline points="6 8 2 12 6 16" />
    {/* “>” */}
    <polyline points="18 16 22 12 18 8" />
    {/* 中央の縦線 */}
    <line x1="14" y1="4" x2="10" y2="20" />
  </svg>
);

interface ConsoleProps {
  title?: string;
  logs?: LogEntry[];
  onLogAdd?: (log: LogEntry) => void;
  onClear?: () => void;
  className?: string;
  height?: string;
  showControls?: boolean;
}

export interface LogEntry {
  id: string;
  level: "log" | "info" | "warn" | "error" | "debug";
  message: string;
  timestamp: Date;
  data?: any;
}

const Console: React.FC<ConsoleProps> = ({
  title = "Console",
  logs = [],
  onLogAdd,
  onClear,
  className = "",
  height = "400px",
  showControls = true,
}) => {
  const [internalLogs, setInternalLogs] = useState<LogEntry[]>(logs);
  const [filter, setFilter] = useState<string>("all");
  const [isMinimized, setIsMinimized] = useState(false);
  const [isMaximized, setIsMaximized] = useState(false);
  const [searchTerm, setSearchTerm] = useState("");
  const outputRef = useRef<HTMLDivElement>(null);

  // ログレベルの色とアイコン
  const getLogStyle = (level: LogEntry["level"]) => {
    switch (level) {
      case "error":
        return "text-red-400 bg-red-900/20";
      case "warn":
        return "text-yellow-400 bg-yellow-900/20";
      case "info":
        return "text-blue-400 bg-blue-900/20";
      case "debug":
        return "text-purple-400 bg-purple-900/20";
      default:
        return "text-gray-300 bg-gray-900/20";
    }
  };

  // ログの追加
  const addLog = (level: LogEntry["level"], message: string, data?: any) => {
    const newLog: LogEntry = {
      id: Date.now().toString(),
      level,
      message,
      timestamp: new Date(),
      data,
    };

    setInternalLogs((prev) => [...prev, newLog]);
    onLogAdd?.(newLog);
  };

  // ログのクリア
  const clearLogs = () => {
    setInternalLogs([]);
    onClear?.();
  };

  // ログのフィルタリング
  const filteredLogs = internalLogs.filter((log) => {
    const matchesFilter = filter === "all" || log.level === filter;
    const matchesSearch =
      searchTerm === "" ||
      log.message.toLowerCase().includes(searchTerm.toLowerCase());
    return matchesFilter && matchesSearch;
  });

  // ログのコピー
  const copyLogs = () => {
    const logText = filteredLogs
      .map(
        (log) =>
          `[${log.timestamp.toLocaleTimeString()}] ${log.level.toUpperCase()}: ${
            log.message
          }`
      )
      .join("\n");
    navigator.clipboard.writeText(logText);
  };

  // ログのダウンロード
  const downloadLogs = () => {
    const logText = filteredLogs
      .map(
        (log) =>
          `[${log.timestamp.toLocaleTimeString()}] ${log.level.toUpperCase()}: ${
            log.message
          }${log.data ? "\n" + JSON.stringify(log.data, null, 2) : ""}`
      )
      .join("\n");

    const blob = new Blob([logText], { type: "text/plain" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = `console-logs-${new Date().toISOString().split("T")[0]}.txt`;
    a.click();
    URL.revokeObjectURL(url);
  };

  // 自動スクロール
  useEffect(() => {
    if (outputRef.current) {
      outputRef.current.scrollTop = outputRef.current.scrollHeight;
    }
  }, [internalLogs]);

  // デモ用のログ生成
  const generateDemoLogs = () => {
    const demoMessages = [
      { level: "info" as const, message: "Application started successfully" },
      { level: "log" as const, message: "User authentication completed" },
      {
        level: "warn" as const,
        message: "API response time is slower than expected",
      },
      { level: "error" as const, message: "Failed to connect to database" },
      {
        level: "debug" as const,
        message: "Debug: Processing user data",
        data: { userId: 123, action: "login" },
      },
    ];

    demoMessages.forEach((demo) => {
      addLog(demo.level, demo.message, demo.data);
    });
  };

  const consoleStyle = {
    height: isMaximized ? "100vh" : height,
    width: isMaximized ? "100vw" : "100%",
    position: isMaximized ? ("fixed" as const) : ("relative" as const),
    top: isMaximized ? 0 : undefined,
    left: isMaximized ? 0 : undefined,
    zIndex: isMaximized ? 9999 : undefined,
  };

  return (
    <div
      className={`bg-gray-900 text-gray-300 font-mono text-sm border border-gray-700 rounded-lg shadow-lg ${className}`}
      style={consoleStyle}
    >
      {/* ヘッダー */}
      <div className="bg-gray-800 px-4 py-2 rounded-t-lg flex items-center justify-between border-b border-gray-700">
        <div className="flex items-center space-x-2">
          <Code2Icon/>
          <span className="text-gray-300">{title}</span>
          <span className="text-gray-500 text-xs">
            ({filteredLogs.length} logs)
          </span>
        </div>
        <div className="flex items-center space-x-2">
          <button
            onClick={() => setIsMinimized(!isMinimized)}
            className="text-gray-400 hover:text-white"
          >
            <MinimizeIcon />
          </button>
          <button
            onClick={() => setIsMaximized(!isMaximized)}
            className="text-gray-400 hover:text-white"
          >
            <MaximizeIcon />
          </button>
        </div>
      </div>

      {/* コンソール本体 */}
      {!isMinimized && (
        <div className="p-4">
          {/* コントロールパネル */}
          {showControls && (
            <div className="mb-4 flex flex-wrap items-center gap-2 text-xs">
              <select
                value={filter}
                onChange={(e) => setFilter(e.target.value)}
                className="bg-gray-800 border border-gray-600 rounded px-2 py-1"
              >
                <option value="all">All Levels</option>
                <option value="log">Log</option>
                <option value="info">Info</option>
                <option value="warn">Warn</option>
                <option value="error">Error</option>
                <option value="debug">Debug</option>
              </select>

              <input
                type="text"
                placeholder="Search logs..."
                value={searchTerm}
                onChange={(e) => setSearchTerm(e.target.value)}
                className="bg-gray-800 border border-gray-600 rounded px-2 py-1 flex-1 min-w-0"
              />

              <button
                onClick={generateDemoLogs}
                className="bg-blue-600 hover:bg-blue-700 px-2 py-1 rounded"
              >
                Demo Logs
              </button>

              <button
                onClick={copyLogs}
                className="bg-gray-600 hover:bg-gray-700 px-2 py-1 rounded flex items-center gap-1"
              >
                <CopyIcon/>
                Copy
              </button>

              <button
                onClick={downloadLogs}
                className="bg-gray-600 hover:bg-gray-700 px-2 py-1 rounded flex items-center gap-1"
              >
                <DownloadIcon/>
                Download
              </button>

              <button
                onClick={clearLogs}
                className="bg-red-600 hover:bg-red-700 px-2 py-1 rounded flex items-center gap-1"
              >
                <Trash2Icon/>
                Clear
              </button>
            </div>
          )}

          {/* ログ出力エリア */}
          <div
            ref={outputRef}
            className="overflow-y-auto scrollbar-thin scrollbar-thumb-gray-600 scrollbar-track-gray-800 space-y-1"
            style={{
              height: showControls
                ? `calc(${height} - 140px)`
                : `calc(${height} - 60px)`,
            }}
          >
            {filteredLogs.length === 0 ? (
              <div className="text-gray-500 text-center py-8">
                No logs to display
              </div>
            ) : (
              filteredLogs.map((log) => (
                <div
                  key={log.id}
                  className={`p-2 rounded border-l-4 ${getLogStyle(log.level)}`}
                >
                  <div className="flex items-start justify-between">
                    <div className="flex-1">
                      <div className="flex items-center space-x-2">
                        <span className="text-xs text-gray-400">
                          {log.timestamp.toLocaleTimeString()}
                        </span>
                        <span
                          className={`text-xs px-2 py-1 rounded ${getLogStyle(
                            log.level
                          )}`}
                        >
                          {log.level.toUpperCase()}
                        </span>
                      </div>
                      <div className="mt-1 whitespace-pre-wrap break-words">
                        {log.message}
                      </div>
                      {log.data && (
                        <details className="mt-2">
                          <summary className="text-xs text-gray-400 cursor-pointer hover:text-gray-300">
                            Show data
                          </summary>
                          <pre className="text-xs bg-gray-800 p-2 rounded mt-1 overflow-x-auto">
                            {JSON.stringify(log.data, null, 2)}
                          </pre>
                        </details>
                      )}
                    </div>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>
      )}
    </div>
  );
};

export default Console;
