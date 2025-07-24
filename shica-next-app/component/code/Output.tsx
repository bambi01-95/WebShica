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
  INFO = "info",
  ERROR = "error",
  WARNING = "warning",
  DEBUG = "debug",
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
      case LogLevel.INFO:
        return "text-blue-500";
      case LogLevel.ERROR:
        return "text-red-500";
      case LogLevel.WARNING:
        return "text-yellow-500";
      case LogLevel.DEBUG:
        return "text-gray-500";
      default:
        return "text-gray-500";
    }
  };

  return (
    <div
      className={`h-full font-mono text-sm border ${
        isRounded ? "rounded-lg" : ""
      }`}
      style={{ borderColor: "var(--color-code-background700)", width, height }}
    >
      <div
        className={`px-4 py-2 ${
          isRounded ? "rounded-t-lg" : ""
        } flex items-center justify-between`}
        style={{
          backgroundColor: "var(--color-background-secondary)",
          color: "var(--color-code-text)",
          borderColor: "var(--color-code-background700)",
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
          className="code-button text-xs px-2 py-1 rounded"
        >
          Clear
        </button>
      </div>
      {/* Output Area */}
      
      <div
        ref={outputRef}
        className="p-4 flex-1 overflow-y-auto scrollbar-thin scrollbar-thumb-gray-600 scrollbar-track-gray-800"
        style={{
          backgroundColor: "var(--color-background-primary)",
          color: "var(--color-code-text)",
        }}
      >
        {logs.length === 0 ? (
          <div
            className="text-center py-8"
            style={{ color: "var(--color-code-text-secondary)" }}
          >
            No logs to display
          </div>
        ) : (
          logs.map((log, index) => (
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
          ))
        )}
      </div>
    </div>
  );
};

export default Output;
