// components/ThemeToggleButton.js
"use client"; // App Routerの場合

import { useTheme } from "@/contexts/ThemeContext";

export default function ThemeToggleButton(style: React.CSSProperties = {}) {
  const { theme, toggleTheme } = useTheme();

  return (
    <button
      style={style}
      onClick={toggleTheme}
      className="p-2 rounded-md duration-200"
    >
      {theme === "light" ? "light" : "dark"}
    </button>
  );
}
