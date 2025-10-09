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

export const ShicaCodeEditor: React.FC<CodeEditorProps> = ({
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
  const highlightRef = useRef<HTMLPreElement>(null);

  useEffect(() => {
    const lines = code.split("\n").length;
    setLineCount(lines);
  }, [code]);

  // シンタックスハイライトの適用
  const highlightCode = (text: string) => {
    const keywords = ['stt', 'var', 'func', 'int', 'flo', 'str', 'type', 'if', 'else', 'return', 'for', 'while', 'break', 'continue'];
    
    // トークン化（コメントと文字列を保護）
    const protectedRanges: Array<{start: number, end: number, content: string, type: string}> = [];
    
    // エスケープ処理
    text = text
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;');
    
    // 複数行コメントを検出
    const multiCommentRegex = /\/\*[\s\S]*?\*\//g;
    let match: RegExpExecArray | null;
    
    while ((match = multiCommentRegex.exec(text)) !== null) {
      protectedRanges.push({
        start: match.index,
        end: match.index + match[0].length,
        content: match[0],
        type: 'comment'
      });
    }
    
    // 単一行コメントを検出
    const singleCommentRegex = /\/\/.*/g;

    while ((match = singleCommentRegex.exec(text)) !== null) {
      const inProtected = protectedRanges.some(r => {
        if(match === null) return false;
        match.index >= r.start && match.index < r.end
      });
      if (!inProtected) {
        protectedRanges.push({
          start: match.index,
          end: match.index + match[0].length,
          content: match[0],
          type: 'comment'
        });
      }
    }

    
    // 文字列を検出
    const stringRegex = /"(?:[^"\\]|\\.)*"/g;
    while ((match = stringRegex.exec(text)) !== null) {
      const inProtected = protectedRanges.some(r => {
        if(match === null) return false;
        match.index >= r.start && match.index < r.end
      });
      if (!inProtected) {
        protectedRanges.push({
          start: match.index,
          end: match.index + match[0].length,
          content: match[0],
          type: 'string'
        });
      }
    }
    
    // 範囲をソート
    protectedRanges.sort((a, b) => a.start - b.start);
    
    // ハイライト処理
    let result = '';
    let lastEnd = 0;
    
    protectedRanges.forEach(range => {
      // 保護されていない部分を処理
      if (lastEnd < range.start) {
        let segment = text.substring(lastEnd, range.start);
        
        // キーワードをハイライト
        keywords.forEach(keyword => {
          const regex = new RegExp(`\\b${keyword}\\b`, 'g');
          segment = segment.replace(regex, `<span style="color: #3b82f6; font-weight: bold;">${keyword}</span>`);
        });
        
        // 数値をハイライト
        segment = segment.replace(/\b(\d+(?:\.\d+)?)\b/g, '<span style="color: #059669;">$1</span>');
        
        // 括弧をハイライト
        segment = segment.replace(/([(){\}\[\]])/g, '<span style="color: #3b82f6;">$1</span>');
        
        result += segment;
      }
      
      // 保護された部分を追加
      const color = range.type === 'comment' ? '#059669' : '#dc2626';
      result += `<span style="color: ${color};">${range.content}</span>`;
      lastEnd = range.end;
    });
    
    // 残りの部分を処理
    if (lastEnd < text.length) {
      let segment = text.substring(lastEnd);
      
      keywords.forEach(keyword => {
        const regex = new RegExp(`\\b${keyword}\\b`, 'g');
        segment = segment.replace(regex, `<span style="color: #3b82f6; font-weight: bold;">${keyword}</span>`);
      });
      
      segment = segment.replace(/\b(\d+(?:\.\d+)?)\b/g, '<span style="color: #059669;">$1</span>');
      segment = segment.replace(/([(){\}\[\]])/g, '<span style="color: #3b82f6;">$1</span>');
      
      result += segment;
    }
    
    return result;
  };

  const handleScroll = () => {
    if (textareaRef.current && lineNumbersRef.current && highlightRef.current) {
      lineNumbersRef.current.scrollTop = textareaRef.current.scrollTop;
      highlightRef.current.scrollTop = textareaRef.current.scrollTop;
      highlightRef.current.scrollLeft = textareaRef.current.scrollLeft;
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

      setTimeout(() => {
        if (textareaRef.current) {
          textareaRef.current.selectionStart =
            textareaRef.current.selectionEnd = start + 2;
        }
      }, 0);
    } else if (e.key === 'Enter') {
      e.preventDefault();
      const start = e.currentTarget.selectionStart;
      const end = e.currentTarget.selectionEnd;
      
      // 現在の行を取得
      const beforeCursor = code.substring(0, start);
      const currentLineStart = beforeCursor.lastIndexOf('\n') + 1;
      const currentLine = beforeCursor.substring(currentLineStart);
      
      // インデント（行頭のスペース）を取得
      const indentMatch = currentLine.match(/^(\s*)/);
      const indent = indentMatch ? indentMatch[1] : '';
      
      // 新しいコードを作成（改行 + インデント）
      const newCode = code.substring(0, start) + '\n' + indent + code.substring(end);
      setCode(newCode);
      
      setTimeout(() => {
        if (textareaRef.current) {
          textareaRef.current.selectionStart = 
            textareaRef.current.selectionEnd = start + 1 + indent.length;
        }
      }, 0);
    }
  };

  useEffect(() => {
    if (highlightRef.current) {
      highlightRef.current.innerHTML = highlightCode(code);
    }
  }, [code]);

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

        {/* Editor with Syntax Highlighting */}
        <div className="flex-1 h-full overflow-y-auto relative">
          {/* Syntax Highlighting Overlay */}
          <pre
            ref={highlightRef}
            className={`${robotoMono.className} absolute top-0 left-0 w-full h-full p-4 font-mono text-sm leading-6 pointer-events-none overflow-auto whitespace-pre`}
            style={{
              margin: 0,
              background: 'transparent',
              color: 'var(--color-text-primary)',
              scrollbarWidth: 'none',
              msOverflowStyle: 'none',
            }}
            aria-hidden="true"
          />
          
          {/* Textarea */}
          <textarea
            ref={textareaRef}
            value={code}
            onChange={handleCodeChange}
            onKeyDown={handleKeyDown}
            onScroll={handleScroll}
            className={`${robotoMono.className} absolute top-0 left-0 w-full h-full p-4 font-mono text-sm leading-6 resize-none outline-none`}
            placeholder="Write your code here..."
            spellCheck={false}
            style={{
              background: 'transparent',
              color: 'transparent',
              caretColor: 'var(--color-text-primary)',
              scrollbarWidth: "thin",
              scrollbarColor:
                "var(--color-code-background600) var(--color-code-background900)",
            }}
          />
          
          {/* Custom scrollbar */}
          <style jsx>{`
            pre::-webkit-scrollbar {
              display: none;
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
