import React, { useState } from 'react';
import { Input } from '../ui/Input';



// CodeSpan Component for the code page
export const CodeSpanEdit = ({ text, onChange }: { text: string, onChange: (text: string) => void }) => {
  return (
    <span className="code-span-component">
      <Input className="w-full" onChange={(e) => onChange(e.target.value)} value={text} />
    </span>
  );
};