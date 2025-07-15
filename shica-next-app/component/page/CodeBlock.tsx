'use client';
import React, { useState } from 'react';

// CodeBlock Component for the code page
export const CodeBlock = ({ filename, children, language = 'shica' }: { filename: string, children: React.ReactNode, language?: string }) => {
    const [copied, setCopied] = useState(false);
  
    const copyCode = () => {
      const text = children?.toString() || '';
      navigator.clipboard.writeText(text).then(() => {
        setCopied(true);
        setTimeout(() => setCopied(false), 2000);
      });
    };
  
    return (
      <div className="code-block-component">
        <div className="code-white-header">{filename}</div>
        <button 
          className="copy-button"
          onClick={copyCode}
          style={{
            background: copied ? 'rgba(76, 175, 80, 0.3)' : 'rgba(255, 255, 255, 0.1)'
          }}
        >
          {copied ? 'Copied!' : 'Copy'}
        </button>
        <div className="code-block-content">
          <pre>{children}</pre>
        </div>
      </div>
    );
  };