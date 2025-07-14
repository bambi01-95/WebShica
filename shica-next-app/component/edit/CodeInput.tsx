"use client";
import React, { useState, useRef, useEffect } from "react";

interface CodeInputProps {
  initialCode?: string;
  onCodeChange?: (e: React.ChangeEvent<HTMLTextAreaElement>) => void;
}

const CodeInput: React.FC<CodeInputProps> = ({
  initialCode = "",
  onCodeChange,
}) => {
  const [code, setCode] = useState(initialCode);
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
      onCodeChange(e);
    }
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === "Tab") {
      e.preventDefault();
      const start = e.currentTarget.selectionStart;
      const end = e.currentTarget.selectionEnd;
      const updatedCode = code.substring(0, start) + "  " + code.substring(end);
      setCode(updatedCode);

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
    <div className="flex w-full border rounded overflow-hidden">
      {/* Line Numbers */}
      <div
        ref={lineNumbersRef}
        className="bg-gray-800 text-gray-400 text-sm font-mono px-3 py-2 select-none overflow-hidden"
        style={{
          minHeight: "200px",
          maxHeight: "400px",
        }}
      >
        {lineNumbers.map((num) => (
          <div key={num} className="leading-6 text-right">
            {num}
          </div>
        ))}
      </div>

      {/* Textarea */}
      <textarea
        ref={textareaRef}
        placeholder="Write your code here..."
        value={code}
        onChange={handleCodeChange}
        onKeyDown={handleKeyDown} // ← 追加
        onScroll={handleScroll}
        className="flex-1 p-2 bg-gray-900 text-gray-100 font-mono text-sm leading-6 resize-none outline-none overflow-y-auto"
        spellCheck={false}
        style={{
          minHeight: "200px",
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
  );
};

export default CodeInput;
