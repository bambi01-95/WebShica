import React, { useState, useEffect } from 'react';

// タイトルコンポーネント
const TitleComponent = ({ title, subtitle }: { title: string, subtitle: string }) => {
  return (
    <div className="title-component glow-effect">
      <h1>{title}</h1>
      <p className="subtitle">{subtitle}</p>
    </div>
  );
};

// 説明文コンポーネント
const DescriptionComponent = ({ title, children }: { title: string, children: React.ReactNode }) => {
  return (
    <div className="description-component">
      <h2>{title}</h2>
      <div className="description-content">
        {children}
      </div>
    </div>
  );
};

// コードスパンコンポーネント
const CodeSpan = ({ children }: { children: React.ReactNode }) => {
  return (
    <span className="code-span-component">
      {children}
    </span>
  );
};

// コードブロックコンポーネント
const CodeBlock = ({ filename, children, language = 'zen' }: { filename: string, children: React.ReactNode, language?: string }) => {
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
      <div className="code-block-header">{filename}</div>
      <button 
        className="copy-button"
        onClick={copyCode}
        style={{
          background: copied ? 'rgba(76, 175, 80, 0.3)' : 'rgba(255, 255, 255, 0.1)'
        }}
      >
        {copied ? 'コピー完了!' : 'コピー'}
      </button>
      <div className="code-block-content">
        <pre>{children}</pre>
      </div>
    </div>
  );
};

// 特徴カードコンポーネント
const FeatureCard = ({ title, children }: { title: string, children: React.ReactNode }) => {
  return (
    <div className="feature-card">
      <h3>{title}</h3>
      <div>{children}</div>
    </div>
  );
};

// メインページコンポーネント
const ProgrammingLanguagePage = () => {
  useEffect(() => {
    // ページロード時のアニメーション
    const components = document.querySelectorAll('.title-component, .description-component, .code-block-component');
    
    components.forEach((component, index) => {
      setTimeout(() => {
        (component as HTMLElement).style.opacity = '1';
        (component as HTMLElement).style.transform = 'translateY(0)';
      }, index * 200);
    });

    // スクロール時のアニメーション効果
    const handleScroll = () => {
      const cards = document.querySelectorAll('.feature-card');
      const triggerBottom = window.innerHeight * 0.8;
      
      cards.forEach(card => {
        const cardTop = card.getBoundingClientRect().top;
        
        if (cardTop < triggerBottom) {
          (card as HTMLElement).style.opacity = '1';
          (card as HTMLElement).style.transform = 'translateY(0)';
        }
      });
    };

    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  return (
    <div className="container">
      <TitleComponent 
        title="ZenLang"
        subtitle="直感的で美しいプログラミング言語"
      />

      <DescriptionComponent title="ZenLangについて">
        <p>
          ZenLangは、日本の禅の思想からインスピレーションを得た革新的なプログラミング言語です。
          シンプルさと美しさを重視し、<CodeSpan>直感的な構文</CodeSpan>で複雑な処理を表現できます。
        </p>
        <p>
          初心者にも優しく、経験豊富な開発者にもパワフルな機能を提供します。
          <CodeSpan>zen.think()</CodeSpan>というメソッドで思考プロセスを可視化することも可能です。
        </p>
      </DescriptionComponent>

      <CodeBlock filename="hello_world.zen">
{`// ZenLangでのハローワールド
zen main() {
    思考("こんにちは、世界！");
    表現("Hello, World!");
    
    // 変数の宣言
    心 message = "ZenLangへようこそ";
    数 count = 42;
    
    // 条件分岐
    もし count > 0 {
        表現(message);
    }
}`}
      </CodeBlock>

      <div className="feature-grid">
        <FeatureCard title="直感的な構文">
          <p>
            日本語キーワードを使用し、自然言語に近い形でプログラムを記述できます。
            <CodeSpan>思考()</CodeSpan>で思考プロセスを、
            <CodeSpan>表現()</CodeSpan>で出力を表現します。
          </p>
        </FeatureCard>
        
        <FeatureCard title="型システム">
          <p>
            シンプルな型定義で安全なコードを記述できます。
            <CodeSpan>心</CodeSpan>は文字列型、
            <CodeSpan>数</CodeSpan>は数値型を表します。
          </p>
        </FeatureCard>
        
        <FeatureCard title="関数型プログラミング">
          <p>
            関数型プログラミングの概念を取り入れ、
            <CodeSpan>純粋関数</CodeSpan>や
            <CodeSpan>不変性</CodeSpan>をサポートします。
          </p>
        </FeatureCard>
      </div>

      <CodeBlock filename="advanced_example.zen">
{`// 関数の定義
純粋関数 フィボナッチ(数 n) -> 数 {
    もし n <= 1 {
        返す n;
    }
    返す フィボナッチ(n - 1) + フィボナッチ(n - 2);
}

// リストの操作
配列 numbers = [1, 2, 3, 4, 5];
配列 doubled = numbers.変換(x => x * 2);

// 並行処理
並行 {
    思考("処理A開始");
    思考("処理B開始");
}`}
      </CodeBlock>

      <DescriptionComponent title="特徴">
        <p>ZenLangは以下の特徴を持っています：</p>
        <p>• <strong>直感的な日本語キーワード</strong>：プログラムの意図が明確になります</p>
        <p>• <strong>型安全性</strong>：コンパイル時に多くのエラーを検出します</p>
        <p>• <strong>関数型プログラミング</strong>：純粋関数と不変性をサポート</p>
        <p>• <strong>並行処理</strong>：<CodeSpan>並行{}</CodeSpan>ブロックで簡単に並行処理を記述</p>
        <p>• <strong>豊富な標準ライブラリ</strong>：Web開発からデータ分析まで幅広く対応</p>
      </DescriptionComponent>

      <style jsx>{`
        .container {
          max-width: 1200px;
          margin: 0 auto;
          padding: 20px;
        }

        :global(body) {
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          background: linear-gradient(135deg, #ff9a56 0%, #ffb347 100%);
          min-height: 100vh;
          color: #333;
          line-height: 1.6;
          margin: 0;
          padding: 0;
        }

        :global(*) {
          box-sizing: border-box;
        }

        .title-component {
          background: rgba(255, 255, 255, 0.95);
          backdrop-filter: blur(10px);
          border-radius: 20px;
          padding: 40px;
          margin-bottom: 30px;
          text-align: center;
          box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
          border: 1px solid rgba(255, 255, 255, 0.2);
          animation: fadeInUp 0.8s ease-out;
        }

        .title-component h1 {
          font-size: 3.5rem;
          background: linear-gradient(45deg, #ff6b35, #f9ca24);
          -webkit-background-clip: text;
          -webkit-text-fill-color: transparent;
          background-clip: text;
          margin-bottom: 15px;
          font-weight: 700;
          letter-spacing: -2px;
        }

        .title-component .subtitle {
          font-size: 1.3rem;
          color: #666;
          font-weight: 300;
          opacity: 0.8;
        }

        .description-component {
          background: rgba(255, 255, 255, 0.95);
          backdrop-filter: blur(10px);
          border-radius: 15px;
          padding: 35px;
          margin-bottom: 30px;
          box-shadow: 0 15px 35px rgba(0, 0, 0, 0.1);
          border: 1px solid rgba(255, 255, 255, 0.2);
          animation: fadeInUp 0.8s ease-out 0.2s both;
          position: relative;
          overflow: hidden;
        }

        .description-component::before {
          content: '';
          position: absolute;
          top: 0;
          left: 0;
          right: 0;
          height: 4px;
          background: linear-gradient(90deg, #ff6b35, #f9ca24);
        }

        .description-component h2 {
          font-size: 2rem;
          margin-bottom: 20px;
          color: #333;
          font-weight: 600;
        }

        .description-component p {
          font-size: 1.1rem;
          color: #555;
          margin-bottom: 15px;
        }

        .code-block-component {
          background: #1a1a1a;
          border-radius: 15px;
          padding: 25px;
          margin-bottom: 30px;
          box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
          position: relative;
          overflow: hidden;
          animation: fadeInUp 0.8s ease-out 0.4s both;
        }

        .code-block-component::before {
          content: '';
          position: absolute;
          top: 0;
          left: 0;
          right: 0;
          height: 40px;
          background: linear-gradient(90deg, #ff6b35, #f9ca24);
          display: flex;
          align-items: center;
          padding: 0 15px;
        }

        .code-block-component::after {
          content: '●●●';
          position: absolute;
          top: 12px;
          left: 15px;
          color: #fff;
          font-size: 12px;
          letter-spacing: 3px;
        }

        .code-block-header {
          margin-top: 15px;
          margin-bottom: 15px;
          font-size: 0.9rem;
          color: #888;
          font-family: 'Courier New', monospace;
        }

        .code-block-content {
          background: #2d2d2d;
          border-radius: 8px;
          padding: 20px;
          font-family: 'Courier New', monospace;
          font-size: 14px;
          line-height: 1.8;
          color: #f8f8f2;
          overflow-x: auto;
          border: 1px solid #444;
        }

        .code-block-content pre {
          margin: 0;
          white-space: pre-wrap;
        }

        .code-span-component {
          display: inline-block;
          background: linear-gradient(135deg, #ff6b35, #f9ca24);
          color: white;
          padding: 4px 8px;
          border-radius: 6px;
          font-family: 'Courier New', monospace;
          font-size: 0.9em;
          font-weight: 500;
          box-shadow: 0 2px 8px rgba(255, 107, 53, 0.3);
          transition: all 0.3s ease;
        }

        .code-span-component:hover {
          transform: translateY(-2px);
          box-shadow: 0 4px 12px rgba(255, 107, 53, 0.4);
        }

        .copy-button {
          position: absolute;
          top: 50px;
          right: 15px;
          background: rgba(255, 255, 255, 0.1);
          color: white;
          border: none;
          padding: 8px 12px;
          border-radius: 6px;
          cursor: pointer;
          font-size: 12px;
          transition: background 0.3s ease;
        }

        .copy-button:hover {
          background: rgba(255, 255, 255, 0.3);
        }

        .feature-grid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
          gap: 20px;
          margin-top: 30px;
        }

        .feature-card {
          background: rgba(255, 255, 255, 0.95);
          backdrop-filter: blur(10px);
          border-radius: 15px;
          padding: 25px;
          border: 1px solid rgba(255, 255, 255, 0.2);
          transition: transform 0.3s ease, box-shadow 0.3s ease;
        }

        .feature-card:hover {
          transform: translateY(-5px);
          box-shadow: 0 20px 40px rgba(0, 0, 0, 0.15);
        }

        .glow-effect {
          position: relative;
          animation: pulse 2s infinite;
        }

        @keyframes fadeInUp {
          from {
            opacity: 0;
            transform: translateY(30px);
          }
          to {
            opacity: 1;
            transform: translateY(0);
          }
        }

        @keyframes pulse {
          0%, 100% { transform: scale(1); }
          50% { transform: scale(1.05); }
        }

        @media (max-width: 768px) {
          .title-component h1 {
            font-size: 2.5rem;
          }
          
          .title-component .subtitle {
            font-size: 1.1rem;
          }
          
          .description-component {
            padding: 25px;
          }
          
          .code-block-component {
            padding: 20px;
          }
        }
      `}</style>
    </div>
  );
};

export default ProgrammingLanguagePage;
