"use client";

import { useState, useRef } from "react";

export interface OutputProps {
  width?: string;
  height?: string;
  isRounded?: boolean;
  logs?: Log[];
  onClear?: () => void;
}

export enum LogLevel {
  SHICA = "shica:",
  INFO = "info:",
  WARN = "warn:",
  ERROR = "error:",
  FATAL = "fatal:",
  DEBUG = "debug:",
  SUCCESS = "success: ",
}

export interface Log {
  level: LogLevel;
  message: string;
  timestamp: number;
}

const Code2Icon = (props: React.SVGProps<SVGSVGElement>) => (
  <svg
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    strokeWidth="2"
    {...props}
  >
    <polyline points="6 8 2 12 6 16" />
    <polyline points="18 16 22 12 18 8" />
    <line x1="14" y1="4" x2="10" y2="20" />
  </svg>
);

const Output = ({
  width = "w-full",
  height = "h-full",
  isRounded = true,
  logs = [],
  onClear = () => {},
}: OutputProps) => {
  const [title, setTitle] = useState("Output");
  const outputRef = useRef<HTMLDivElement>(null);

  const getLogStyle = (level: LogLevel) => {
    switch (level) {
      case LogLevel.SHICA:
        return "text-orange-500";
      case LogLevel.SUCCESS:
      case LogLevel.INFO:
        return "text-gray-500";
      case LogLevel.WARN:
        return "text-yellow-500";
      case LogLevel.ERROR:
        return "text-red-500";
      case LogLevel.DEBUG:
        return "text-gray-500";
      case LogLevel.FATAL:
        return "text-red-700 font-bold";
      default:
        return "text-gray-500";
    }
  };

  return (
    <div
      className={`font-mono text-sm border flex flex-col ${
        isRounded ? "rounded-lg" : ""
      } ${height}`}
      style={{
        borderColor: "var(--color-code-background700)",
        width,
        overflow: "hidden", // 重要: 外側コンテナのオーバーフローを防ぐ
      }}
    >
      {/* 固定ヘッダー */}
      <div
        className={`px-4 py-2 ${
          isRounded ? "rounded-t-lg" : ""
        } flex items-center justify-between shrink-0`} // shrink-0 を追加
        style={{
          backgroundColor: "var(--color-background-secondary)",
          color: "var(--color-code-text)",
          borderBottom: "1px solid var(--color-code-background700)",
        }}
      >
        <div className="flex items-center space-x-2">
          <Code2Icon className="w-4 h-4" />
          <span style={{ color: "var(--color-code-text)" }}>{title}</span>
          <span className="text-xs" style={{ color: "var(--color-code-text)" }}>
            ({logs.length} logs)
          </span>
        </div>
        <button
          onClick={onClear}
          className="text-xs px-2 py-1 rounded transition-colors hover:bg-opacity-20 hover:bg-white"
          style={{
            color: "var(--color-code-text)",
            backgroundColor: "var(--color-code-background700)",
          }}
        >
          Clear
        </button>
      </div>

      {/* スクロール可能なアウトプットエリア */}
      <div
        ref={outputRef}
        className="overflow-y-auto min-h-0" // flex-1 と p-4 を削除
        style={{
          backgroundColor: "var(--color-background-primary)",
          color: "var(--color-code-text)",
          flex: "1 1 0", // flex-1 の代わりに明示的に指定
        }}
      >
        {logs.length === 0 ? (
          <div
            className="text-center py-8 px-4" // px-4 を追加
            style={{ color: "var(--color-code-text-secondary)" }}
          >
            No logs to display
          </div>
        ) : (
          <div className="p-4 pb-2">
            {" "}
            {/* ログコンテナを追加、下パディングを小さく */}
            {logs.map((log, index) => (
              <div key={index} className={`mb-1 ${getLogStyle(log.level)}`}>
                <span
                  className="text-xs mr-2"
                  style={{ color: "var(--color-code-text-secondary)" }}
                >
                  [{new Date(log.timestamp).toLocaleTimeString()}]
                </span>
                <span className="text-xs mr-2 px-1 rounded">
                  {log.level.toUpperCase()}
                </span>
                <span className="whitespace-pre-wrap">{log.message}</span>
              </div>
            ))}
          </div>
        )}
      </div>

      {/* カスタムスクロールバー */}
      <style jsx>{`
        .flex-1::-webkit-scrollbar {
          width: 8px;
        }
        .flex-1::-webkit-scrollbar-track {
          background: var(--color-background-primary);
        }
        .flex-1::-webkit-scrollbar-thumb {
          background: #4b5563;
          border-radius: 4px;
        }
        .flex-1::-webkit-scrollbar-thumb:hover {
          background: #6b7280;
        }
      `}</style>
    </div>
  );
};

export default Output;
