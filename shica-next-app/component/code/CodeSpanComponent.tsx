import React from 'react';

// CodeSpan Component for the code page
export const CodeSpanComponent = ({ children }: { children: React.ReactNode }) => {
  return (
    <span className="code-span-component">
      {children}
    </span>
  );
};