// tailwind.config.js
/** @type {import('tailwindcss').Config} */
module.exports = {
  // ...その他の設定
  darkMode: "class", // 'media' または 'class' を選択
  theme: {
    extend: {
      // カスタムプロパティやカラーパレットを定義する
      colors: {
        // 例: CSS変数を使用
        "primary-text": "var(--color-text-primary)",
        "secondary-text": "var(--color-text-secondary)",
        "background-primary": "var(--color-background-primary)",
        "background-secondary": "var(--color-background-secondary)",
        // ...その他のカスタムカラー
      },
    },
  },
  plugins: [],
};
