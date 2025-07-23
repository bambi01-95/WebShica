"use client";
import React, { useState, useRef, useEffect } from "react";

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
      } overflow-hidden`}
      style={{ height: "100%" }} // ① 外部で高さ制限されている前提
    >
      {/* Header */}
      <div className="flex items-center justify-between bg-gray-800 px-4 py-3 border-b border-gray-700 shrink-0">
        <div className="flex items-center space-x-3">
          <span className="text-gray-300 text-sm font-medium">{filename}</span>
          <span className="text-gray-500 text-xs bg-gray-700 px-2 py-1 rounded">
            {language}
          </span>
        </div>
        <button
          onClick={copyCode}
          className="px-3 py-1 text-sm font-medium text-white rounded transition-all duration-200 hover:scale-105"
          style={{
            background: copied
              ? "linear-gradient(45deg, #4CAF50, #45a049)"
              : "linear-gradient(45deg, #374151, #4B5563)",
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
          className="code-background text-gray-400 text-sm font-mono px-3 py-4 border-r border-gray-700 select-none overflow-hidden"
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
            className="w-full h-full p-4 text-gray-100 font-mono text-sm leading-6 resize-none outline-none"
            placeholder="Write your code here..."
            spellCheck={false}
            style={{
              scrollbarWidth: "thin",
              scrollbarColor: "#4B5563 #1F2937",
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
      <div className="bg-gray-800 px-4 py-2 border-t border-gray-700 flex justify-between items-center text-xs text-gray-400 shrink-0">
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
