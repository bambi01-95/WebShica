// components/ThemeToggleButton.js
'use client'; // App Routerの場合

import { useTheme } from '@/contexts/ThemeContext';

export default function ThemeToggleButton() {
  const { theme, toggleTheme } = useTheme();

  return (
    <button
      onClick={toggleTheme}
      className="p-2 rounded-md bg-gray-200 dark:bg-gray-700 text-gray-800 dark:text-gray-200"
    >
      {theme === 'light' ? '🌙 ダークモードに切り替え' : '☀️ ライトモードに切り替え'}
    </button>
  );
}