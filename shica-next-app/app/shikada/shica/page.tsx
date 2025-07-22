"use client";
import FileLists from "@/component/code/FileLists";
import { CodeEditor } from "@/component/code/CodeEditor";
import { useState, useEffect, useRef } from "react";
import Output, { Log, LogLevel } from "@/component/code/Output";
import { Robot } from "@/component/code/Map";
import SizeWarningPage from "@/component/code/SizeWaring";
import { useVM } from "@/hooks/shikada/useVM";
import InlineCodeWithCopy from "@/component/code/InlineCode";

//for color picker
const hexToRgb = (hex: string) => {
  const sanitized = hex.replace("#", "");
  const r = parseInt(sanitized.substring(0, 2), 16);
  const g = parseInt(sanitized.substring(2, 4), 16);
  const b = parseInt(sanitized.substring(4, 6), 16);
  return { r, g, b };
};

const ShicaPage = () => {
  const [code, setCode] = useState<{ filename: string; code: string }[]>([
    {
      filename: "Agent0.shica",
      code: "stt s1(){\n    clickEH(x,y){\n        setXY(x,y);\n    }\n}",
    },
  ]);

  const [isMap, setIsMap] = useState(false);
  const robotsRef = useRef<Robot[]>([{ x: 0, y: 0, vx: 1, vy: 1 }]);
  const isRunningRef = useRef(true);
  const mapRef = useRef<HTMLDivElement>(null);
  const [time, setTime] = useState(0);

  const [Module, isReady] = useVM();
  const [selectedIndex, setSelectedIndex] = useState(0);
  const [isCompiling, setIsCompiling] = useState(false);
  const [isCompiled, setIsCompiled] = useState(false);
  const [isRunning, setIsRunning] = useState(false);
  const [isRunInit, setIsRunInit] = useState(false);
  const [logs, setLogs] = useState<Log[]>([]);

  //for user sample code
  const [clickXY, setClickXY] = useState<{ x: number; y: number }>({
    x: 0,
    y: 0,
  });
  const [rgb, setRgb] = useState({ r: 0, g: 0, b: 0 });

  // 配列に要素を追加する例
  const addItem = (newItem: string = "") => {
    setCode((prev) => [
      ...prev,
      { filename: `Agent${code.length}.shica`, code: newItem },
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

  const addLog = (level: LogLevel, message: string) => {
    setLogs([...logs, { level, message, timestamp: Date.now() }]);
  };
  const clearLogs = () => {
    setLogs([]);
  };

  useEffect(() => {
    if (!Module || !isReady || !isCompiling) return;
    const selectedCode = code[selectedIndex].code;
    const res = Module.ccall(
      "compileWebCode",
      "number",
      ["string"],
      [selectedCode]
    );
    if (res === 1) {
      addLog(LogLevel.INFO, "COMPILE: Compile success");
    } else {
      addLog(LogLevel.ERROR, "COMPILE: Compile failed");
    }
    setIsCompiling(false);
  }, [isCompiling, Module, isReady]);

  const compile = () => {
    if (isRunning) {
      return;
    }
    setIsCompiling(true);
    setIsRunInit(false);
    setTimeout(() => {
      setIsCompiling(false);
    }, 1000);
    setIsCompiled(true);
  };
  const drawRobots = () => {
    const robotElems = mapRef.current?.querySelectorAll(".robot-vacuum");
    if (!robotElems) return;

    robotsRef.current.forEach((robot, index) => {
      const elem = robotElems[index] as HTMLDivElement;
      if (elem) {
        elem.style.left = `${robot.x}px`;
        elem.style.top = `${robot.y}px`;
      }
    });
  };
  const clickEH = (x: number, y: number) => {
    if (!Module || !isReady) return;
    const rect = mapRef.current?.getBoundingClientRect();
    if (!rect) return;
    const xc = x - rect.left - 20 < 0 ? 0 : Math.round(x - rect.left - 20);
    const yc = y - rect.top - 20 < 0 ? 0 : Math.round(y - rect.top - 20);
    if (isRunning) {
      Module.setValue(Module.clickPtr + 0, xc, "i32");
      Module.setValue(Module.clickPtr + 4, yc, "i32");
      Module.setValue(Module.clickPtr + 8, 1, "i32"); // active
    }
    setClickXY({ x: xc, y: yc });
  };

  const intervalRef = useRef<NodeJS.Timeout | null>(null);

  useEffect(() => {
    if (!Module || !isReady) return;
    if (isRunning) {
      if (!isRunInit) {
        Module.ccall("initRunWeb", "number", [], []);
        setIsRunInit(true);
        addLog(LogLevel.INFO, "RUN: Start");
      } else {
        addLog(LogLevel.INFO, "RUN: Continue");
      }
      intervalRef.current = setInterval(() => {
        if (!Module || !isReady) return;
        Module.setValue(Module.timerPtr, time, "i32");
        const robot = robotsRef.current[0];
        const res = Module.ccall("runWeb", "number", [], []);
        const x = Module.getValue(Module.agentPtr + 0, "i32");
        const y = Module.getValue(Module.agentPtr + 4, "i32");
        const vx = Module.getValue(Module.agentPtr + 8, "i32");
        const vy = Module.getValue(Module.agentPtr + 12, "i32");
        robot.x = x + vx;
        robot.y = y + vy;
        robot.vx = vx;
        robot.vy = vy;
        drawRobots();
        Module.setValue(Module.agentPtr + 0, robot.x, "i32");
        Module.setValue(Module.agentPtr + 4, robot.y, "i32");
        setTime(time + 50);
        Module.setValue(Module.clickPtr + 8, 0, "i32"); // inactive
      }, 50);
    } else {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
        addLog(LogLevel.INFO, "RUN: Stopped");
      }
    }
  }, [isRunning, Module, isReady]);

  const run = () => {
    if (!isCompiled) {
      addLog(LogLevel.ERROR, "RUN: Please compile the code first");
      return;
    }
    setIsRunning(!isRunning);
  };

  const handleColorChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const hex = e.target.value;
    const rgbValue = hexToRgb(hex);
    setRgb(rgbValue);
  };

  return (
    <div>
      <div className="xl:hidden">
        <SizeWarningPage />
      </div>
      {/* LEFT */}
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
            <div className="w-full h-full bg-gray-900 flex flex-col space-y-2 items-center justify-center border border-gray-700">
              {/* grid map 10x10 */}
              <div className="h-[500px] w-[500px] bg-gray-800">
                <div
                  className="relative cursor-pointer"
                  style={{
                    width: `500px`,
                    height: `500px`,
                    backgroundImage:
                      "linear-gradient(rgba(255,255,255,0.1) 1px, transparent 1px), linear-gradient(90deg, rgba(255,255,255,0.1) 1px, transparent 1px)",
                    backgroundSize: `10px 10px`,
                  }}
                  ref={mapRef}
                  onClick={(e) => {
                    clickEH(e.clientX, e.clientY);
                  }}
                >
                  {robotsRef.current.map((_, i) => (
                    <div
                      key={i}
                      className="robot-vacuum"
                      style={{
                        width: `40px`,
                        height: `40px`,
                        backgroundColor: "red",
                        borderRadius: "50%",
                        position: "absolute",
                        transition: "all 0.1s linear",
                        left: "0px",
                        top: "0px",
                      }}
                    />
                  ))}
                </div>
              </div>
              <div className="flex flex-row space-x-2">
                <div className="text-white text-2xl">👆</div>
                <InlineCodeWithCopy
                  code={`setXY(${clickXY?.x}, ${clickXY?.y})`}
                />
              </div>
              <div className="flex flex-row space-x-2">
                <input
                  type="color"
                  className="w-20"
                  onChange={handleColorChange}
                />
                <InlineCodeWithCopy
                  code={`setColor(${rgb.r}, ${rgb.g}, ${rgb.b})`}
                />
              </div>
            </div>
          </div>
          {/* END OF LEFT */}
          {/* RIGHT */}
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
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 ${
                  isRunning
                    ? "bg-green-600 hover:bg-green-700 text-white hover:scale-105"
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
                onClear={clearLogs}
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
