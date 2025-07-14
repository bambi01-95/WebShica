import React from "react";

export const Button = ({ children, onClick }: { children: React.ReactNode, onClick: () => void }) => {
  return (
    <button 
      className="bg-orange-500 text-white hover:bg-orange-600 px-4 py-2 shadow-md rounded-lg transition-colors duration-300"
      onClick={onClick}
    >
      {children}
    </button>
  );
};

export default Button;