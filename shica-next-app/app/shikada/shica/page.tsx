"use client";
import FileLists from "@/component/code/FileLists";
import { CodeEditor } from "@/component/code/CodeEditor";
import { useState, useEffect } from "react";
import Output, { Log } from "@/component/code/Output";
import Map from "@/component/code/Map";
import SizeWarningPage from "@/component/code/SizeWaring";
import { useVM } from "@/hooks/shikada/useVM";

const ShicaPage = () => {
  const [code, setCode] = useState<{ filename: string; code: string }[]>([
    { filename: "test0.shica", code: "" },
  ]);
  const [Module, isReady] = useVM();
  const [selectedIndex, setSelectedIndex] = useState(0);
  const [isCompiling, setIsCompiling] = useState(false);
  const [isRunning, setIsRunning] = useState(false);
  const [logs, setLogs] = useState<Log[]>([]);
  const onClear = () => {
    setLogs([]);
  };

  // 配列に要素を追加する例
  const addItem = (newItem: string = "") => {
    setCode((prev) => [
      ...prev,
      { filename: `test${code.length}.shica`, code: newItem },
    ]);
    setSelectedIndex(code.length); // 新しく追加したファイルを選択
  };

  // インデックスで要素を更新する例
  const updateItem = (index: number, newValue: string) => {
    setCode((prev) =>
      prev.map((item, i) => (i === index ? { ...item, code: newValue } : item))
    );
  };

  // 条件で要素を削除する例
  const removeItem = (index: number) => {
    if (code.length <= 1) return; // 最低1つのファイルは残す

    setCode((prev) => prev.filter((_, i) => i !== index));

    // 選択中のファイルが削除された場合の処理
    if (selectedIndex === index) {
      setSelectedIndex(Math.max(0, index - 1));
    } else if (selectedIndex > index) {
      setSelectedIndex(selectedIndex - 1);
    }
  };

  useEffect(() => {
    if (!Module || !isReady) return;

    let interval: NodeJS.Timeout | null = null;
    if (isRunning) {
      interval = setInterval(() => {
        const t = Date.now();
        Module.setValue(Module.timerPtr, t, "i32");

        const result = Module.ccall("runWeb", "number", [], []);
        const x = Module.getValue(Module.agentPtr + 0, "i32");
        const y = Module.getValue(Module.agentPtr + 4, "i32");
      }, 50);
    }
    return () => {
      if (interval) {
        clearInterval(interval);
      }
    };
  }, [isRunning, Module, isReady]);

  useEffect(() => {
    if (!Module || !isReady || !isCompiling) return;
    const selectedCode = code[selectedIndex].code;
    const res = Module.ccall("compileWebCode", "number", ["string"], [selectedCode]);
    console.log(res);
    setIsCompiling(false);
  }, [isCompiling, Module, isReady]);


  const compile = () => {
    if (isRunning) {
      return;
    }
    setIsCompiling(true);
    setTimeout(() => {
      setIsCompiling(false);
    }, 1000);
  };

  const run = () => {
    if (isCompiling) {
      return;
    }
    setIsRunning(!isRunning);
  };

  return (
    <div>
      <div className="xl:hidden">
        <SizeWarningPage />
      </div>
      <div className="hidden xl:flex flex-col w-full h-full h-screen">
        {/* TOP */}
        <div className="bg-gray-800 px-4 py-3 border-b border-gray-700 flex items-center justify-between">
          <div className="flex items-center space-x-4">
            <div className="flex items-center space-x-2">
              <span className="text-gray-300 font-medium">Shica IDE</span>
            </div>
          </div>
        </div>
        {/* END TOP */}
        {/* MIDDLE */}
        <div className="flex flex-row h-full">
          <div className="w-1/2">
            <Map />
          </div>

          {/* TOP */}
          <div className="flex flex-col w-1/2">
            <div className="flex flex-row h-[700px]">
              <div className="w-1/4">
                <FileLists
                  code={code}
                  selectedIndex={selectedIndex}
                  setSelectedIndex={setSelectedIndex}
                  removeItem={removeItem}
                  disableRemove={code.length <= 1}
                  addItem={addItem}
                  MAX_FILE_COUNT={15}
                  width="w-full"
                  height="h-full"
                />
              </div>
              <div className="w-3/4">
                <CodeEditor
                  key={code[selectedIndex].filename}
                  filename={code[selectedIndex].filename}
                  initialCode={code[selectedIndex].code}
                  onCodeChange={(newCode) => updateItem(selectedIndex, newCode)}
                  isRounded={false}
                  width="w-full"
                  height="h-full"
                />
              </div>
            </div>

            <div className="flex flex-row h-[70px] bg-gray-900 justify-center items-center gap-4">
              <button
                onClick={run}
                disabled={isRunning}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 ${
                  isRunning
                    ? "bg-green-600 hover:bg-green-700 text-white hover:scale-105 cursor-not-allowed"
                    : "bg-gray-600 text-gray-400 "
                }`}
              >
                {isRunning ? "Stop" : "Run"}
              </button>
              <button
                onClick={compile}
                disabled={isCompiling}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 ${
                  isCompiling
                    ? "bg-blue-600 hover:bg-blue-700 text-white hover:scale-105 cursor-not-allowed"
                    : "bg-gray-600 text-gray-400 "
                }`}
              >
                {isCompiling ? "Compiling..." : "Compile"}
              </button>
            </div>
            {/* BOTTOM */}
            <div className="h-1/4">
              <Output
                height="h-full"
                isRounded={false}
                logs={logs}
                setLogs={setLogs}
                onClear={onClear}
              />
            </div>
            {/* BOTTOM */}
          </div>
        </div>
        {/* END TOP */}
      </div>
    </div>
  );
};

export default ShicaPage;
