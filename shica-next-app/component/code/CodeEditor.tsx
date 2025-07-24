"use client";
import React, { useState, useRef, useEffect } from "react";
import { Roboto_Mono } from "next/font/google";

const robotoMono = Roboto_Mono({
  subsets: ["latin"],
  weight: ["400", "500", "700"],
});

interface CodeEditorProps {
  filename: string;
  initialCode?: string;
  language?: string;
  onCodeChange?: (code: string) => void;
  isRounded?: boolean;
  isFullScreen?: boolean;
  width?: string;
  height?: string;
}

export const CodeEditor: React.FC<CodeEditorProps> = ({
  filename,
  initialCode = "",
  language = "shica",
  onCodeChange,
  isRounded = true,
  isFullScreen = false,
  width = "w-full",
  height = "h-full",
}) => {
  const [code, setCode] = useState(initialCode);
  const [copied, setCopied] = useState(false);
  const [lineCount, setLineCount] = useState(1);
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const lineNumbersRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const lines = code.split("\n").length;
    setLineCount(lines);
  }, [code]);

  const handleScroll = () => {
    if (textareaRef.current && lineNumbersRef.current) {
      lineNumbersRef.current.scrollTop = textareaRef.current.scrollTop;
    }
  };

  const handleCodeChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    const newCode = e.target.value;
    setCode(newCode);
    if (onCodeChange) {
      onCodeChange(newCode);
    }
  };

  const copyCode = () => {
    navigator.clipboard.writeText(code).then(() => {
      setCopied(true);
      setTimeout(() => setCopied(false), 2000);
    });
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === "Tab") {
      e.preventDefault();
      const start = e.currentTarget.selectionStart;
      const end = e.currentTarget.selectionEnd;
      const newCode = code.substring(0, start) + "  " + code.substring(end);
      setCode(newCode);

      // カーソル位置を調整
      setTimeout(() => {
        if (textareaRef.current) {
          textareaRef.current.selectionStart =
            textareaRef.current.selectionEnd = start + 2;
        }
      }, 0);
    }
  };

  const lineNumbers = Array.from({ length: lineCount }, (_, i) => i + 1);

  return (
    <div
      className={`w-full bg-gray-900 flex flex-col ${
        isRounded ? "rounded-lg" : ""
      } overflow-hidden ${height}`}
      style={{
        border: "1px solid var(--color-code-background700)",
        backgroundColor: "var(--color-background-primary)",
      }}
    >
      {/* Header */}
      <div
        className="flex items-center justify-between px-4 py-3 border-b shrink-0"
        style={{
          backgroundColor: "var(--color-background-secondary)",
          borderColor: "var(--color-code-background700)",
        }}
      >
        <div className="flex items-center space-x-3">
          <span
            className="text-sm font-medium"
            style={{ color: "var(--color-text-primary)" }}
          >
            {filename}
          </span>
          <span
            className="text-xs px-2 py-1 rounded"
            style={{ color: "var(--color-text-secondary)" }}
          >
            {language}
          </span>
        </div>
        <button
          onClick={copyCode}
          className="px-3 py-1 text-sm font-medium text-white rounded transition-all duration-200 hover:scale-105"
          style={{
            background: copied
              ? "linear-gradient(45deg, #4CAF50, #45a049)"
              : "var(--color-code-background700)",
            color: "var(--color-text-primary)",
          }}
        >
          {copied ? "Copied!" : "Copy"}
        </button>
      </div>

      {/* Scrollable Editor Area */}
      <div className="flex flex-1 overflow-hidden">
        {/* Line Numbers */}
        <div
          ref={lineNumbersRef}
          className="text-sm font-mono px-3 py-4 select-none overflow-hidden"
          style={{
            backgroundColor: "var(--color-background-secondary)",
            color: "var(--color-text-secondary)",
            borderRightColor: "var(--color-code-background700)",
          }}
        >
          {lineNumbers.map((num) => (
            <div key={num} className="leading-6 text-right">
              {num}
            </div>
          ))}
        </div>

        {/* Textarea */}
        <div className="flex-1 h-full overflow-y-auto">
          <textarea
            ref={textareaRef}
            value={code}
            onChange={handleCodeChange}
            onKeyDown={handleKeyDown}
            onScroll={handleScroll}
            className={`${robotoMono.className} w-full h-full p-4 font-mono text-sm leading-6 resize-none outline-none`}
            placeholder="Write your code here..."
            spellCheck={false}
            style={{
              scrollbarWidth: "thin",
              scrollbarColor:
                "var(--color-code-background600) var(--color-code-background900)",
            }}
          />
          {/* Custom scrollbar */}
          <style jsx>{`
            textarea {
              background: var(--color-background-primary);
              color: var(--color-text-primary);
            }
            textarea::-webkit-scrollbar {
              width: 8px;
            }
            textarea::-webkit-scrollbar-track {
              background: var(--color-background-primary);
            }
            textarea::-webkit-scrollbar-thumb {
              background: #4b5563;
              border-radius: 4px;
            }
            textarea::-webkit-scrollbar-thumb:hover {
              background: #6b7280;
            }
          `}</style>
        </div>
      </div>

      {/* Footer */}
      <div
        className="px-4 py-2 border-t flex justify-between items-center text-xs text-gray-400 shrink-0"
        style={{
          backgroundColor: "var(--color-background-secondary)",
          borderColor: "var(--color-code-background700)",
          color: "var(--color-text-secondary)",
        }}
      >
        <div className="flex space-x-4">
          <span>Lines: {lineCount}</span>
          <span>Chars: {code.length}</span>
        </div>
        <div className="flex space-x-2">
          <span>UTF-8</span>
          <span>LF</span>
          <span>{language}</span>
        </div>
      </div>
    </div>
  );
};
