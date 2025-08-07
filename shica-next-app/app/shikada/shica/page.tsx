"use client";
import FileLists from "@/component/code/FileLists";
import { CodeEditor } from "@/component/code/CodeEditor";
import { useState, useEffect, useRef } from "react";
import Output, { Log, LogLevel } from "@/component/code/Output";
import { Robot } from "@/component/code/Map";
import SizeWarningPage from "@/component/code/SizeWaring";
import { useVM } from "@/hooks/shikada/useShica";
import InlineCodeWithCopy from "@/component/code/InlineCode";
import ThemeToggleButton from "@/component/ui/ThemeToggleButton";
import { Roboto } from "next/font/google";

const roboto = Roboto({
  subsets: ["latin", "latin-ext"],
  weight: ["400", "600", "500", "700"],
});

//for color picker
const hexToRgb = (hex: string) => {
  const sanitized = hex.replace("#", "");
  const r = parseInt(sanitized.substring(0, 2), 16);
  const g = parseInt(sanitized.substring(2, 4), 16);
  const b = parseInt(sanitized.substring(4, 6), 16);
  return { r, g, b };
};

const ShicaPage = () => {
  const [codes, setCodes] = useState<{ filename: string; code: string }[]>([
    {
      filename: "Agent0.shica",
      code: "stt s1(){\n    clickEH(x,y){\n        setXY(50,50);\n        setVX(5);\n        setVY(0);\n    }\n}",
    },
  ]);

  const robotsRef = useRef<Robot[]>([{ x: 0, y: 0, vx: 1, vy: 1 }]);
  const mapRef = useRef<HTMLDivElement>(null);
  const [time, setTime] = useState(0);
  const [forceUpdate, setForceUpdate] = useState(0); // 強制再レンダリング用

  const [Module, isReady] = useVM();
  const [selectedIndex, setSelectedIndex] = useState(0);
  const [isCompiling, setIsCompiling] = useState(false);
  const [isCompiled, setIsCompiled] = useState(false);
  const [process, setProcess] = useState("none");
  const [isRunning, setIsRunning] = useState(false);
  const [isRunInit, setIsRunInit] = useState(false);
  const [logs, setLogs] = useState<Log[]>([]);

  //for user sample code
  const [clickXY, setClickXY] = useState<{ x: number; y: number }>({
    x: 0,
    y: 0,
  });
  const [rgb, setRgb] = useState({ r: 0, g: 0, b: 0 });

  const addRobot = () => {
    const numRobots = robotsRef.current.length;
    const newRobot: Robot = { x: 50 * numRobots, y: 0, vx: 1, vy: 1 };
    robotsRef.current = [...robotsRef.current, newRobot];
    setForceUpdate((prev) => prev + 1); // 強制再レンダリング
  };

  // 配列に要素を追加する例
  const addItem = (newItem: string = "") => {
    setCodes((prev) => [
      ...prev,
      {
        filename: `Agent${codes.length}.shica`,
        code: "stt s1(){\n    clickEH(x,y){\n        setXY(x,y);\n    }\n}",
      },
    ]);
    setSelectedIndex(codes.length); // 新しく追加したファイルを選択
    addRobot();
    const ret = Module?.ccall("addWebCode", "number", [], []);
    if (ret !== 0) {
      console.error("Failed to add web code");
      addLog(LogLevel.ERROR, "touch failed - maximum file count reached");
      return;
    } else {
      addLog(LogLevel.SUCCESS, `touch Agent${codes.length}.shica`);
    }
  };

  // インデックスで要素を更新する例
  const updateItem = (index: number, newValue: string) => {
    setCodes((prev) =>
      prev.map((item, i) => (i === index ? { ...item, code: newValue } : item))
    );
  };

  // 条件で要素を削除する例
  const removeItem = (index: number) => {
    if (codes.length <= 1) return; // 最低1つのファイルは残す

    setCodes((prev) => prev.filter((_, i) => i !== index));
    const ret = Module?.ccall("deleteWebCode", "number", ["number"], [index]);
    if (ret !== 0) {
      console.error("Failed to delete web code");
      addLog(LogLevel.ERROR, "rm failed - minimum file count reached");
      return;
    } else {
      addLog(LogLevel.SUCCESS, `rm ${codes[index].filename}`);
    }
    // 選択中のファイルが削除された場合の処理
    if (selectedIndex === index) {
      setSelectedIndex(Math.max(0, index - 1));
    } else if (selectedIndex > index) {
      setSelectedIndex(selectedIndex - 1);
    }
  };

  const addLog = (level: LogLevel, message: string) => {
    setLogs((prevLogs) => [
      ...prevLogs,
      { level, message, timestamp: Date.now() },
    ]);
  };
  const clearLogs = () => {
    setLogs([]);
  };
  // once when the page is loaded
  useEffect(() => {
    if (!Module || !isReady) return;
    const agentDataPtr = Module.ccall(
      "getAnAgentDataPtr",
      "number",
      ["number"],
      [0]
    );
    for (let i = 0; i < 12; i++) {
      const x = Module.getValue(agentDataPtr + i * 32 + 0, "i32");
      const y = Module.getValue(agentDataPtr + i * 32 + 4, "i32");
      const vx = Module.getValue(agentDataPtr + i * 32 + 8, "i32");
      const vy = Module.getValue(agentDataPtr + i * 32 + 12, "i32");
      console.log(`Agent ${i} - x: ${x}, y: ${y}, vx: ${vx}, vy: ${vy}`);
    }

    if (process === "none") {
      addLog(LogLevel.SHICA, "Welcome to Shica Code Simulator d-.-b");
      // Initialize web codes if not already done
      console.log("Initializing web codes...");
      let ret = Module.ccall("initWebCodes", "number", ["number"], [12]);
      if (ret !== 0) {
        console.error("Failed to initialize web codes");
        addLog(LogLevel.FATAL, "Failed to initialize web codes");
        return;
      }
      ret = Module.ccall("addWebCode", "number", [], []);
      if (ret) {
        console.error("Failed to add initial web code");
        addLog(LogLevel.FATAL, "Failed to add initial web code");
        return;
      } else {
        addLog(LogLevel.INFO, "Initialized web codes");
        addLog(LogLevel.SUCCESS, `touch ${codes[0].filename}`);
      }
    }
  }, [Module]);

  useEffect(() => {
    if (!Module || !isReady || !isCompiling) return;
    const selectedCode = codes[selectedIndex].code;
    const bool = process === "compile" ? 1 : 0;
    const ret = Module.ccall(
      "compileWebCode",
      "number",
      ["number", "number", "string"],
      [bool, selectedIndex, selectedCode]
    );
    // change .shica to .stt, and meke output filename
    const outputFilename = codes[selectedIndex].filename.replace(
      /\.shica$/,
      ".stt"
    );

    if (ret === 0) {
      addLog(
        LogLevel.SUCCESS,
        `shica ${outputFilename} -o ${codes[selectedIndex].filename}`
      );
    } else {
      addLog(
        LogLevel.ERROR,
        `shica ${outputFilename} -o ${codes[selectedIndex].filename}`
      );
    }
    setProcess("compile");
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
        const numCodes = codes.length;
        console.log(`${numCodes} web codes to run`);

        const ret = Module.ccall(
          "initWebAgents",
          "number",
          ["number"],
          [numCodes]
        );
        if (ret !== 0) {
          addLog(
            LogLevel.ERROR,
            "run failed - error in initializing web agents"
          );
          return;
        }
        setIsRunInit(true);
        setProcess("run");
        const allFFileNames = codes.map((c) => c.filename).join(" ./");
        addLog(LogLevel.SUCCESS, "run ./" + allFFileNames);
      } else {
        addLog(LogLevel.SUCCESS, "run continued - running web codes");
      }
      intervalRef.current = setInterval(() => {
        if (!Module || !isReady) return;
        Module.setValue(Module.timerPtr, time, "i32");

        const res = Module.ccall("executeWebCodes", "number", [], []);
        if (res !== 0) {
          addLog(LogLevel.ERROR, "run failed - error in web codes");
          clearInterval(intervalRef.current!);
          return;
        }
        const agentptr = Module.ccall(
          "getAnAgentDataPtr",
          "number",
          ["number"],
          [0]
        );
        for (let i = 0; i < codes.length; i++) {
          const robot = robotsRef.current[i];
          const offset = i * 32; // 4 bytes each for x, y, vx, vy
          const x = Module.getValue(agentptr + offset + 0, "i32");
          const y = Module.getValue(agentptr + offset + 4, "i32");
          const vx = Module.getValue(agentptr + offset + 8, "i32");
          const vy = Module.getValue(agentptr + offset + 12, "i32");
          robot.x = x + vx;
          robot.y = y + vy;
          robot.vx = vx;
          robot.vy = vy;
          Module.setValue(agentptr + offset + 0, robot.x, "i32");
          Module.setValue(agentptr + offset + 4, robot.y, "i32");
          console.log(`Agent ${i} - x: ${x}, y: ${y}, vx: ${vx}, vy: ${vy}`);
        }
        setForceUpdate((prev) => prev + 1); // 位置更新を画面に反映
        setTime(time + 500);
        Module.setValue(Module.clickPtr + 8, 0, "i32"); // inactive
      }, 500);
    } else {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
        const ret = Module.ccall("stopWebCodes", "number", [], []);
        if (ret !== 0) {
          addLog(LogLevel.ERROR, "run failed - error in stopping web codes");
        } else {
          addLog(LogLevel.SUCCESS, "run stopped - web codes stopped");
        }
      }
    }
  }, [isRunning, Module, isReady, codes]);

  const run = () => {
    if (!isCompiled) {
      addLog(LogLevel.ERROR, "run failed - compile first");
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
        <div
          className="px-4 py-3 flex items-center justify-between"
          style={{ backgroundColor: "var(--color-background-primary)" }}
        >
          <div className="flex items-center space-x-4">
            <div className="flex items-center">
              <span
                className={`text-2xl font-bold weight:400 ${roboto.className}`}
                style={{ color: "var(--color-text-primary)" }}
              >
                Shica Code Simulator
              </span>
            </div>
            <div className="ml-4">
              <ThemeToggleButton
                background="var(--color-background-secondary)"
                color="var(--color-text-primary)"
              />
            </div>
          </div>
        </div>
        {/* END TOP */}
        {/* MIDDLE */}
        <div className="flex flex-row h-full">
          <div className="w-1/2">
            <div
              className="w-full h-full flex flex-col space-y-2 items-center justify-center"
              style={{
                backgroundColor: "var(--color-background-primary)",
                border: "1px solid var(--color-code-background700)",
              }}
            >
              {/* grid map 10x10 */}
              <div
                className="h-[500px] w-[500px]"
                style={{ backgroundColor: "var(--color-background-secondary)" }}
              >
                <div
                  className="relative cursor-pointer"
                  style={{
                    width: `500px`,
                    height: `500px`,
                    backgroundImage:
                      "linear-gradient(var(--color-code-background800) 1px, transparent 1px), linear-gradient(90deg, var(--color-code-background800) 1px, transparent 1px)",
                    backgroundSize: `10px 10px`,
                  }}
                  ref={mapRef}
                  onClick={(e) => {
                    clickEH(e.clientX, e.clientY);
                  }}
                >
                  {robotsRef.current.map((robot, i) => (
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
                        left: `${robot.x}px`,
                        top: `${robot.y}px`,
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
            <div className="flex flex-row h-[600px]">
              <div className="w-1/4">
                <FileLists
                  code={codes}
                  selectedIndex={selectedIndex}
                  setSelectedIndex={setSelectedIndex}
                  removeItem={removeItem}
                  disableRemove={codes.length <= 1}
                  addItem={addItem}
                  MAX_FILE_COUNT={12}
                  width="w-full"
                  height="h-full"
                />
              </div>

              <div className="w-3/4">
                <CodeEditor
                  key={codes[selectedIndex].filename}
                  filename={codes[selectedIndex].filename}
                  initialCode={codes[selectedIndex].code}
                  onCodeChange={(newCode) => updateItem(selectedIndex, newCode)}
                  isRounded={false}
                  width="w-full"
                  height="h-[600px]"
                />
              </div>
            </div>

            {/* MIDDLE */}
            <div
              className="flex flex-row justify-center items-center gap-4 p-4"
              style={{
                backgroundColor: "var(--color-background-primary)",
                border: "1px solid var(--color-code-background700)",
              }}
            >
              <button
                onClick={run}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 hover:scale-105`}
                style={{
                  backgroundColor: isRunning
                    ? "var(--color-code-background600)"
                    : "var(--color-background-secondary)",
                  color: isRunning
                    ? "var(--color-code-text-secondary)"
                    : "var(--color-code-text)",
                }}
              >
                {isRunning ? "Stop" : "Run"}
              </button>
              <button
                onClick={compile}
                disabled={isCompiling}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 hover:scale-105`}
                style={{
                  background: isCompiling
                    ? "var(--color-code-background600)"
                    : "var(--color-background-secondary)",
                  color: isCompiling
                    ? "var(--color-code-text-secondary)"
                    : "var(--color-code-text)",
                }}
              >
                {isCompiling ? "Compiling..." : "Compile"}
              </button>
            </div>
            {/* BOTTOM */}
            <div className="h-full overflow-hidden">
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
        <div>
          <div
            className="flex items-center justify-center p-4"
            style={{ backgroundColor: "var(--color-background-secondary)" }}
          >
            <span
              className="text-sm text-gray-500"
              style={{ color: "var(--color-text-secondary)" }}
            >
              Shica IDE - Powered by Shikada
            </span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ShicaPage;
