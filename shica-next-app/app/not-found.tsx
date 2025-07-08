"use client";
import { useState, useEffect } from "react";
import Image from "next/image";

// フローティングアイコンコンポーネント
const FloatingIcon = ({
  children,
  initialX = 0,
  initialY = 0,
  drift = 30,
  speed = 0.02,
}: {
  children: React.ReactNode;
  initialX?: number;
  initialY?: number;
  drift?: number;
  speed?: number;
}) => {
  const [position, setPosition] = useState({ x: initialX, y: initialY });
  const [angle, setAngle] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setAngle((prev) => prev + speed);
      setPosition({
        x: initialX + Math.sin(angle) * drift,
        y: initialY + Math.cos(angle * 0.8) * drift * 0.5,
      });
    }, 16);

    return () => clearInterval(interval);
  }, [angle, initialX, initialY, drift, speed]);

  return (
    <div
      className="absolute transition-all duration-300 ease-out"
      style={{
        transform: `translate(${position.x}px, ${position.y}px)`,
        zIndex: 10,
      }}
    >
      {children}
    </div>
  );
};

// グラデーションオーブコンポーネント
const GradientOrb = ({
  size = 100,
  color = "blue",
  opacity = 0.3,
  blur = 40,
}: {
  size?: number;
  color?: string;
  opacity?: number;
  blur?: number;
}) => {
  return (
    <div
      className={`absolute rounded-full bg-gradient-to-br from-${color}-400 to-${color}-600`}
      style={{
        width: `${size}px`,
        height: `${size}px`,
        opacity,
        filter: `blur(${blur}px)`,
      }}
    />
  );
};

// パーティクルコンポーネント
const Particle = ({
  delay = 0,
  size = 4,
  color = "bg-blue-400",
}: {
  delay?: number;
  size?: number;
  color?: string;
}) => {
  return (
    <div
      className={`absolute ${color} rounded-full opacity-60 animate-pulse`}
      style={{
        width: `${size}px`,
        height: `${size}px`,
        animationDelay: `${delay}ms`,
        animationDuration: "2s",
      }}
    />
  );
};

// 雨滴コンポーネント
const RainDrop = ({
  left,
  delay,
  duration = 1000,
  size = 2,
}: {
  left: number;
  delay: number;
  duration?: number;
  size?: number;
}) => {
  return (
    <div
      className="absolute bg-blue-300 rounded-full opacity-60 animate-rain"
      style={{
        left: `${left}%`,
        width: `${size}px`,
        height: `${size * 4}px`,
        animationDelay: `${delay}ms`,
        animationDuration: `${duration}ms`,
        animationIterationCount: "infinite",
        animationTimingFunction: "linear",
      }}
    />
  );
};

export default function NotFound() {
  const [mounted, setMounted] = useState(false);
  const [rainDrops, setRainDrops] = useState<Array<{ left: number; delay: number; duration: number; size: number }>>([]);

  useEffect(() => {
    setMounted(true);
    
    // 雨滴の初期化
    const drops = Array.from({ length: 50 }, (_, i) => ({
      left: Math.random() * 100,
      delay: Math.random() * 2000,
      duration: 800 + Math.random() * 400,
      size: 1 + Math.random() * 2,
    }));
    setRainDrops(drops);
  }, []);

  if (!mounted) return null;

  return (
    <div className="relative min-h-screen bg-gradient-to-br from-slate-50 via-blue-50 to-indigo-100 overflow-hidden">
      {/* カスタムCSS */}
      <style jsx>{`
        @keyframes rain {
          0% {
            transform: translateY(-100vh);
            opacity: 0.8;
          }
          100% {
            transform: translateY(100vh);
            opacity: 0;
          }
        }
        .animate-rain {
          animation: rain linear infinite;
        }
        
        @keyframes ripple {
          0% {
            transform: scale(0);
            opacity: 0.8;
          }
          100% {
            transform: scale(4);
            opacity: 0;
          }
        }
        .animate-ripple {
          animation: ripple 2s ease-out infinite;
        }
      `}</style>

      {/* 雨の効果 */}
      <div className="absolute inset-0 pointer-events-none z-30">
        {rainDrops.map((drop, index) => (
          <RainDrop
            key={index}
            left={drop.left}
            delay={drop.delay}
            duration={drop.duration}
            size={drop.size}
          />
        ))}
      </div>

      {/* 地面の水たまりエフェクト */}
      <div className="absolute bottom-0 left-0 right-0 h-32 bg-gradient-to-t from-blue-100/30 to-transparent pointer-events-none z-10"></div>
      
      {/* 雨による波紋エフェクト */}
      <div className="absolute inset-0 pointer-events-none z-20">
        {Array.from({ length: 8 }, (_, i) => (
          <div
            key={i}
            className="absolute w-2 h-2 border border-blue-200 rounded-full animate-ripple"
            style={{
              left: `${Math.random() * 100}%`,
              top: `${60 + Math.random() * 30}%`,
              animationDelay: `${Math.random() * 3000}ms`,
              animationDuration: "2s",
            }}
          />
        ))}
      </div>
      {/* 背景のグラデーションオーブ */}
      <div className="absolute inset-0">
        <FloatingIcon initialX={100} initialY={200} drift={50} speed={0.015}>
          <GradientOrb size={200} color="blue" opacity={0.2} blur={60} />
        </FloatingIcon>
        <FloatingIcon initialX={-150} initialY={-100} drift={40} speed={0.02}>
          <GradientOrb size={150} color="indigo" opacity={0.15} blur={50} />
        </FloatingIcon>
        <FloatingIcon initialX={200} initialY={-50} drift={35} speed={0.025}>
          <GradientOrb size={120} color="purple" opacity={0.18} blur={45} />
        </FloatingIcon>
        <FloatingIcon initialX={-100} initialY={300} drift={45} speed={0.018}>
          <GradientOrb size={180} color="cyan" opacity={0.16} blur={55} />
        </FloatingIcon>
        <FloatingIcon initialX={300} initialY={150} drift={30} speed={0.022}>
          <GradientOrb size={130} color="teal" opacity={0.14} blur={48} />
        </FloatingIcon>
        <FloatingIcon initialX={-200} initialY={50} drift={60} speed={0.012}>
          <GradientOrb size={220} color="sky" opacity={0.12} blur={65} />
        </FloatingIcon>
        <FloatingIcon initialX={150} initialY={-150} drift={25} speed={0.028}>
          <GradientOrb size={100} color="violet" opacity={0.17} blur={40} />
        </FloatingIcon>
        <FloatingIcon initialX={-80} initialY={-200} drift={40} speed={0.016}>
          <GradientOrb size={160} color="emerald" opacity={0.13} blur={52} />
        </FloatingIcon>
        <FloatingIcon initialX={250} initialY={250} drift={35} speed={0.024}>
          <GradientOrb size={140} color="rose" opacity={0.15} blur={46} />
        </FloatingIcon>
        <FloatingIcon initialX={-250} initialY={-50} drift={55} speed={0.014}>
          <GradientOrb size={190} color="amber" opacity={0.11} blur={58} />
        </FloatingIcon>
        <FloatingIcon initialX={80} initialY={-250} drift={30} speed={0.026}>
          <GradientOrb size={110} color="pink" opacity={0.16} blur={42} />
        </FloatingIcon>
        <FloatingIcon initialX={-120} initialY={150} drift={45} speed={0.019}>
          <GradientOrb size={170} color="lime" opacity={0.12} blur={54} />
        </FloatingIcon>
      </div>

      {/* パーティクル */}
      <div className="absolute inset-0 pointer-events-none">
        <Particle delay={0} size={3} color="bg-blue-300" />
        <Particle delay={500} size={4} color="bg-indigo-300" />
        <Particle delay={1000} size={2} color="bg-purple-300" />
        <Particle delay={1500} size={5} color="bg-blue-400" />
      </div>

      {/* メインコンテンツ */}
      <div className="relative z-20 flex flex-col items-center justify-center min-h-screen px-4">
        {/* アニメーションする画像 */}
        <div className="mb-12 relative">
          <FloatingIcon initialX={0} initialY={0} drift={15} speed={0.025}>
            <div className="relative">
              <Image
                src="/dear.png"
                alt="Deer"
                width={180}
                height={180}
                className="drop-shadow-2xl filter brightness-95"
              />
              <div className="absolute inset-0 bg-gradient-to-t from-blue-200/30 to-transparent rounded-full blur-sm"></div>
              
              {/* 鹿の上に小さな雨避けエフェクト */}
              <div className="absolute -top-8 left-1/2 transform -translate-x-1/2 w-20 h-1 bg-blue-400/20 rounded-full blur-sm"></div>
            </div>
          </FloatingIcon>
        </div>

        {/* テキストコンテンツ */}
        <div className="text-center space-y-6 max-w-2xl mx-auto">
          <div className="space-y-2">
            <h1 className="text-8xl font-bold bg-gradient-to-r from-blue-600 via-purple-600 to-indigo-600 bg-clip-text text-transparent">
              404
            </h1>
            <div className="h-1 w-24 bg-gradient-to-r from-blue-500 to-purple-500 rounded-full mx-auto"></div>
          </div>

          <h2 className="text-3xl font-semibold text-slate-800 mb-4">
            ページが見つかりません
          </h2>

          <p className="text-lg text-slate-600 leading-relaxed max-w-md mx-auto">
            お探しのページは存在しないか、移動された可能性があります。
            <br />
            <span className="text-blue-600 font-medium">鹿のDが雨宿りしながら迷子になってしまいました...</span>
          </p>

          {/* アクションボタン */}
          <div className="flex flex-col sm:flex-row gap-4 justify-center items-center pt-8">
            <a
              href="/"
              className="group relative inline-flex items-center px-8 py-4 bg-gradient-to-r from-blue-500 to-indigo-600 text-white font-medium rounded-full shadow-lg hover:shadow-xl transition-all duration-300 transform hover:scale-105 hover:from-blue-600 hover:to-indigo-700"
            >
              <svg
                className="w-5 h-5 mr-3 transition-transform group-hover:-translate-x-1"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  strokeLinecap="round"
                  strokeLinejoin="round"
                  strokeWidth={2}
                  d="M10 19l-7-7m0 0l7-7m-7 7h18"
                />
              </svg>
              ホームへ戻る
            </a>

            <button
              onClick={() => window.history.back()}
              className="group inline-flex items-center px-6 py-3 bg-white/70 backdrop-blur-sm text-slate-700 font-medium rounded-full border border-slate-200 hover:bg-white hover:border-slate-300 transition-all duration-200 shadow-sm hover:shadow-md"
            >
              <svg
                className="w-5 h-5 mr-2 transition-transform group-hover:rotate-180"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  strokeLinecap="round"
                  strokeLinejoin="round"
                  strokeWidth={2}
                  d="M3 10h10a8 8 0 018 8v2M3 10l6 6m-6-6l6-6"
                />
              </svg>
              前のページに戻る
            </button>
          </div>
        </div>

        {/* 統計情報（オプション） */}
        <div className="mt-16 grid grid-cols-3 gap-8 text-center">
          <div className="bg-white/30 backdrop-blur-sm rounded-2xl p-6 shadow-sm">
            <div className="text-2xl font-bold text-blue-600">404</div>
            <div className="text-sm text-slate-600 mt-1">エラーコード</div>
          </div>
          <div className="bg-white/30 backdrop-blur-sm rounded-2xl p-6 shadow-sm">
            <div className="text-2xl font-bold text-indigo-600">∞</div>
            <div className="text-sm text-slate-600 mt-1">可能性</div>
          </div>
          <div className="bg-white/30 backdrop-blur-sm rounded-2xl p-6 shadow-sm">
            <div className="text-2xl font-bold text-purple-600">1</div>
            <div className="text-sm text-slate-600 mt-1">解決策</div>
          </div>
        </div>
      </div>

      {/* 装飾的な要素 */}
      <div className="absolute top-1/4 left-1/4 w-2 h-2 bg-blue-400 rounded-full opacity-60 animate-ping"></div>
      <div className="absolute bottom-1/4 right-1/4 w-3 h-3 bg-indigo-400 rounded-full opacity-40 animate-ping animation-delay-300"></div>
      <div className="absolute top-3/4 left-1/6 w-1 h-1 bg-purple-400 rounded-full opacity-50 animate-ping animation-delay-700"></div>
    </div>
  );
}