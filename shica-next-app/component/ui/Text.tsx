import React from "react";

export const Text = ({
  children,
  className,
  onChange,
  placeholder,
}: {
  children: string;
  className: string;
  placeholder?: string;
  onChange: (e: React.ChangeEvent<HTMLTextAreaElement>) => void;
}) => {
  return (
    <p className={className}>
      <textarea
        placeholder={placeholder}
        value={children}
        onChange={(e) => onChange(e)}
        rows={4} // 初期行数を指定
        className={`${className} w-full border border-gray-300 rounded-lg px-4 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-blue-500 transition shadow-sm resize-none`}
      ></textarea>
    </p>
  );
};
export default Text;
