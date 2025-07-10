"use client";
import React, { useState, useRef, useEffect } from "react";

interface CodeEditorProps {
  filename: string;
  initialCode?: string;
  language?: string;
  onCodeChange?: (code: string) => void;
}

export const CodeEditor: React.FC<CodeEditorProps> = ({
  filename,
  initialCode = "",
  language = "shica",
  onCodeChange,
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
    <div className="w-full max-w-4xl mx-auto bg-gray-900 rounded-lg overflow-hidden shadow-2xl">
      {/* Header */}
      <div className="flex items-center justify-between bg-gray-800 px-4 py-3 border-b border-gray-700">
        <div className="flex items-center space-x-3">
          <div className="flex space-x-2">
            <div className="w-3 h-3 bg-red-500 rounded-full"></div>
            <div className="w-3 h-3 bg-yellow-500 rounded-full"></div>
            <div className="w-3 h-3 bg-green-500 rounded-full"></div>
          </div>
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

      {/* Editor */}
      <div className="flex">
        {/* Line Numbers */}
        <div
          ref={lineNumbersRef}
          className="bg-gray-800 text-gray-400 text-sm font-mono px-3 py-4 border-r border-gray-700 select-none overflow-hidden"
          style={{
            minHeight: "400px",
            maxHeight: "400px",
          }}
        >
          {lineNumbers.map((num) => (
            <div key={num} className="leading-6 text-right">
              {num}
            </div>
          ))}
        </div>

        {/* Code Area */}
        <div className="flex-1 relative">
          <textarea
            ref={textareaRef}
            value={code}
            onChange={handleCodeChange}
            onKeyDown={handleKeyDown}
            onScroll={handleScroll}
            className="w-full h-96 p-4 bg-gray-900 text-gray-100 font-mono text-sm leading-6 resize-none outline-none overflow-y-auto"
            placeholder="Write your code here..."
            spellCheck={false}
            style={{
              minHeight: "400px",
              maxHeight: "400px",
              scrollbarWidth: "thin",
              scrollbarColor: "#4B5563 #1F2937",
            }}
          />

          {/* Custom Scrollbar Styling */}
          <style jsx>{`
            textarea::-webkit-scrollbar {
              width: 8px;
            }
            textarea::-webkit-scrollbar-track {
              background: #1f2937;
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

      {/* Status Bar */}
      <div className="bg-gray-800 px-4 py-2 border-t border-gray-700 flex justify-between items-center text-xs text-gray-400">
        <div className="flex space-x-4">
          <span>Lines: {lineCount}</span>
          <span>Chars: {code.length}</span>
        </div>
        <div className="flex space-x-2">
          <span>UTF-8</span>
          <span>LF</span>
        </div>
      </div>
    </div>
  );
};
