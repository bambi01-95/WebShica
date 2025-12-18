// components/CodeBlock.tsx
"use client";
import React from "react";
import { Roboto_Mono } from "next/font/google";
import { highlightShicaCode } from "./ShicaHighlight";

const robotoMono = Roboto_Mono({
  subsets: ["latin"],
  weight: ["400", "500", "700"],
});

interface CodeBlockProps {
  code: string;
  output?: string;
  filename?: string;
  language?: string;
}

export const CodeBlock: React.FC<CodeBlockProps> = ({
  code,
    output,
  filename,
  language = "shica",
}) => {
  return (
    <div className="my-6 rounded-md overflow-hidden border border-gray-200 bg-gray-50">
      {/* Header */}
      {(filename || language) && (
        <div className="flex justify-between items-center px-4 py-2 text-sm bg-gray-600 text-gray-100">
          <span>{filename}</span>
          <span>{language}</span>
        </div>
      )}


      {/* Code */}
      <pre
        className={`${robotoMono.className} p-4 text-sm leading-6 overflow-x-auto whitespace-pre`}
        style={{ color: "var(--color-text-primary)" }}
        dangerouslySetInnerHTML={{
          __html: highlightShicaCode(code),
        }}
      />
        {/* Output */}  
        {output && (
        <div className="px-4 py-2 bg-gray-200 text-gray-500 text-sm border-t border-gray-200">
        <strong className="block mb-1">Output:</strong>
        <pre className="whitespace-pre-wrap">{output}</pre>
        </div>
      )}
    </div>
  );
};

export default CodeBlock