"use client";
import { useEffect, useState } from "react";

export function useVM() {
  const [Module, setModule] = useState<any>(null);

  useEffect(() => {
    let isMounted = true;

    // vm.jsを動的に読み込む
    const loadVM = async () => {
      // @ts-ignore
      const module = await import("../../public/shikada/js/vm.js");

      // MODULARIZE=1 の場合は初期化関数が返る
      if (typeof module.default === "function") {
        const initializedModule = await module.default();
        if (isMounted) setModule(initializedModule);
      } else {
        if (isMounted) setModule(module);
      }
    };

    loadVM();

    return () => {
      isMounted = false;
    };
  }, []);

  return Module;
}
