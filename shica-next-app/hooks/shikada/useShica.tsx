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
        script.src = "/shikada/js/shica.js";
        script.async = true;
        
        script.onload = () => {
          // shica-web.jsがグローバルにModuleを定義している前提
          const instance = (window as any).Module;
          if (instance) {
            // locateFileやonRuntimeInitializedをセット
            instance.locateFile = (path: string) => `/shikada/js/${path}`;
            instance.onRuntimeInitialized = () => {
              console.log("✅ WASM initialized");

              const ret = instance.ccall("memory_init", "number", [], []);
              if (ret !== 0) {
                console.error("Failed to initialize memory");
                return;
              }
              instance.timerPtr = instance.ccall("initWebTimerPtr", "number", [], []);
              instance.clickPtr = instance.ccall("initWebClickSTTPtr", "number", [], []);
              instance.agentsPtr = instance.ccall("initALLAgentDataPtr", "number", ["number"], [12]);
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