"use client";
import { useEffect, useState } from "react";

export function useVM() {
  const [Module, setModule] = useState<any>(null);
  const [isReady, setIsReady] = useState(false); // 初期化済みフラグ

  useEffect(() => {
    let isMounted = true;

    const loadVM = async () => {
      // vm.jsはESMではなく、通常のスクリプトとしてグローバルにModuleをエクスポートしている場合を考慮
      // そのため、importではなく、動的にscriptタグを挿入してロードする
      if (typeof window !== "undefined") {
        // すでにロード済みなら再ロードしない
        if ((window as any).Module) {
          const instance = (window as any).Module;
          if (isMounted) {
            setModule(instance);
            setIsReady(true);
          }
          return;
        }

        const script = document.createElement("script");
        script.src = "/shikada/js/vm.js";
        script.async = true;

        script.onload = () => {
          // vm.jsがグローバルにModuleを定義している前提
          const instance = (window as any).Module;
          if (instance) {
            // locateFileやonRuntimeInitializedをセット
            instance.locateFile = (path: string) => `/shikada/js/${path}`;
            instance.onRuntimeInitialized = () => {
              console.log("✅ WASM initialized");

              instance.ccall("memory_init", "number", [], []);
              instance.timerPtr = instance.ccall("initWebTimerPtr", "number", [], []);
              instance.clickPtr = instance.ccall("initWebClickSTTPtr", "number", [], []);
              instance.agentPtr = instance.ccall("initAnAgnetDataPtr", "number", [], []);

              if (isMounted) {
                setModule(instance);
                setIsReady(true);
              }
            };
          }
        };

        document.body.appendChild(script);
      }
    };

    loadVM();

    return () => {
      isMounted = false;
    };
  }, []);

  return [Module, isReady] as const;
}