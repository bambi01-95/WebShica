import React, { useState } from 'react';

interface InlineCodeWithCopyProps {
  code: string;
}

const InlineCodeWithCopy: React.FC<InlineCodeWithCopyProps> = ({ code }) => {
  const [copied, setCopied] = useState(false);

  const handleCopy = async () => {
    try {
      await navigator.clipboard.writeText(code);
      setCopied(true);
      setTimeout(() => setCopied(false), 1000);
    } catch (e) {
      console.error('コピー失敗:', e);
    }
  };

  return (
    <div className="inline-flex items-center bg-gray-800 text-white px-2 py-1 rounded text-sm font-mono">
      <code>{code}</code>
      <button
        onClick={handleCopy}
        className="ml-2 text-xs text-gray-400 hover:text-white"
        title="Copy"
      >
        {copied ? "Copied" : "Copy"}
      </button>
    </div>
  );
};

export default InlineCodeWithCopy;