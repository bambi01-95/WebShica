"use client";
import { useShicaWebRTC } from '@/hooks/shikada/optbroadcast';
import FileLists from "@/component/code/FileLists";
import { ShicaCodeEditor } from "@/component/code/ShicaCodeEditor";
import { useState, useEffect, useRef, useCallback } from "react";
import Output, { Log, LogLevel } from "@/component/code/Output";
import SizeWarningPage from "@/component/code/SizeWaring";
import { useVM } from "@/hooks/shikada/useShica";
import InlineCodeWithCopy from "@/component/code/InlineCode";
import ThemeToggleButton from "@/component/ui/ThemeToggleButton";
import { Roboto } from "next/font/google";
import Script from 'next/script';
import { sampleCodes } from './sample';

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

interface agentObject {
  index: number;
  x: number;
  y: number;
  vx: number;
  vy: number;
  isClick: number;
  distance: number;
  status: number;
  red: number;
  green: number;
  blue: number;
  isLEDOn: number;
}
const agentObjectOffset = {
  index: 0,
  x: 4,
  y: 8,
  vx: 12,
  vy: 16,
  isClick: 20,
  distance: 24,
  status: 28,
  red: 32,
  green: 33,
  blue: 34,
  isLEDOn: 35,
};

export interface Robot {
  x: number;
  y: number;
  r: number;
  g: number;
  b: number;
}

// Example codes for initial state

interface Agent{
  uid: number;
  filename: string;
  code: string;
  compiled: boolean;
}
const ShicaPage = () => {
  // <CodeEditor>のコード管理
  const [codes, setCodes] = useState<{ filename: string; code: string; compiled: boolean }[]>([
    {
      filename: "Agent0",
      code:
        sampleCodes[0] ||
        "stt s1(){\n    clickEH(x,y){\n        setXY(x,y);\n    }\n}",
      compiled: false, 
    },
  ]);
  const updateItem = (index: number, newValue: string) => {
    setCodes((prev) =>
      prev.map((item, i) => (i === index ? { ...item, code: newValue } : item))
    );
  };

  const robotsRef = useRef<Robot[]>([{ x: 0, y: 0, r: 0, g: 0, b: 0 }]);

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

  const {
     initializeTopicHost,
     addUser,
     connectUserToTopic,
     sendMessage,
     disconnectUserFromTopic,
     userSessions,
  } = useShicaWebRTC(Module, isReady);

  const _addWebRtcBroadcast = useCallback(async (number: number, channel: string, password: string, ptr: any) => {
    console.log(`🛜 Adding WebRTC Broadcast User: ${number} to channel: ${channel}`);
    await addUser(number, ptr);
    console.log(`🔍 After addUser: session exists=${userSessions.has(number)}`);
    await connectUserToTopic(number, channel, ptr);
    console.log(`✅ User ${number} fully connected to ${channel}`);
  }, [addUser, connectUserToTopic, userSessions]);

  const _sendWebRtcBroadcast = useCallback((index: number, msg: string, num: number) => {
    console.log(`📡 Sending WebRTC Broadcast Message from User: ${index} to ${num}`);
    sendMessage(index, msg);
  }, [sendMessage]);

  const _removeWebRtcBroadcast = useCallback((number: number, channel: string) => {
    disconnectUserFromTopic(number, channel);
  }, [disconnectUserFromTopic]);

  const addWebRtcBroadcastRef = useRef(_addWebRtcBroadcast);
  const sendWebRtcBroadcastRef = useRef(_sendWebRtcBroadcast);
  const removeWebRtcBroadcastRef = useRef(_removeWebRtcBroadcast);

  useEffect(() => {
    addWebRtcBroadcastRef.current = _addWebRtcBroadcast;
  }, [_addWebRtcBroadcast]);

  useEffect(() => {
    sendWebRtcBroadcastRef.current = _sendWebRtcBroadcast;
  }, [_sendWebRtcBroadcast]);

  useEffect(() => {
    removeWebRtcBroadcastRef.current = _removeWebRtcBroadcast;
  }, [_removeWebRtcBroadcast]);

  useEffect(() => {
    if (!isReady) return;
    (globalThis as any)._addWebRtcBroadcast = (...args: Parameters<typeof _addWebRtcBroadcast>) =>
      addWebRtcBroadcastRef.current?.(...args);
    (globalThis as any)._sendWebRtcBroadcast = (...args: Parameters<typeof _sendWebRtcBroadcast>) =>
      sendWebRtcBroadcastRef.current?.(...args);
    (globalThis as any)._removeWebRtcBroadcast = (...args: Parameters<typeof _removeWebRtcBroadcast>) =>
      removeWebRtcBroadcastRef.current?.(...args);
    console.log("🌐 Registered WebRTC bridge functions to globalThis");

    return () => {
      delete (globalThis as any)._addWebRtcBroadcast;
      delete (globalThis as any)._sendWebRtcBroadcast;
      delete (globalThis as any)._removeWebRtcBroadcast;
    };
  }, [isReady]);
  
  //for user sample code
  const [clickXY, setClickXY] = useState<{ x: number; y: number }>({
    x: 0,
    y: 0,
  });
  const [fps, setFps] = useState(500);

  const [rgb, setRgb] = useState({ r: 0, g: 0, b: 0 });

  const addRobot = () => {
    const numRobots = robotsRef.current.length;
    const newRobot: Robot = {
      x: 50 * numRobots,
      y: 0,
      r: 0,
      g: 0,
      b: 0,
    };
    robotsRef.current = [...robotsRef.current, newRobot];
    setForceUpdate((prev) => prev + 1); // 強制再レンダリング
  };

  // <FileLists>コンポーネントのファイル管理
  const addItem = (newItem: string = "") => {
    setCodes((prev) => [
      ...prev,
      {
        filename: `Agent${codes.length}`,
        code: sampleCodes[1],
        compiled: false,
      },
    ]);
    setSelectedIndex(codes.length); // 新しく追加したファイルを選択
    addRobot();
    addUser(codes.length, 0);//WebRTC OptBroadcast user add
    const ret = Module?.ccall("addWebCode", "number", [], []);
    if (ret !== 0) {
      console.error("Failed to add web code");
      addLog(LogLevel.ERROR, "touch failed - maximum file count reached");
      return;
    } else {
      addLog(LogLevel.SUCCESS, `touch Agent${codes.length}.shica`);
    }
  };
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

  // <Output>コンポーネントのログ管理
  const addLog = (level: LogLevel, message: string) => {
    setLogs((prevLogs) => [
      ...prevLogs,
      { level, message, timestamp: Date.now() },
    ]);
  };
  const clearLogs = () => {
    setLogs([]);
  };

  const processError = () => {
    if (!Module || !isReady) return;
    const numErrors = Module.ccall("getNumOfErrorMsg", "number", [], []);
    if (numErrors > 0) {
      for (let i = 0; i < numErrors; i++) {
        const errorMsg = Module.ccall("getErrorMsg", "string", [], []);
        const errorLevel = errorMsg.charAt(0);
        const errorLine = errorMsg.slice(1, 5);

        const errorMessage = errorMsg.slice(5);
        let logLevel: LogLevel;
        switch (errorLevel) {
          case "0":
            logLevel = LogLevel.SHICA;
            break;
          case "1":
            logLevel = LogLevel.INFO;
            break;
          case "2":
            logLevel = LogLevel.LOG;
            const logMessage = `[${errorLine} %] ${errorMessage}`;
            addLog(logLevel, logMessage);
            continue;
          case "3":
            logLevel = LogLevel.WARN;
            break;
          case "4":
            logLevel = LogLevel.ERROR;
            break;
          case "5":
            logLevel = LogLevel.FATAL;
            break;
          default:
            logLevel = LogLevel.DEBUG;
        }
        if (errorLine === "0000") {
          addLog(logLevel, errorMessage);
        } else {
          const logMessage = `[Line: ${errorLine}] ${errorMessage}`;
          addLog(logLevel, logMessage);
        }
      }
    }
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
    // TEST
//REVIEW
    for (let i = 0; i < 12; i++) {
      const index = Module.getValue(
        agentDataPtr + i * 36 + agentObjectOffset.index,
        "i32"
      );
      const x = Module.getValue(
        agentDataPtr + i * 36 + agentObjectOffset.x,
        "i32"
      );
      const y = Module.getValue(
        agentDataPtr + i * 36 + agentObjectOffset.y,
        "i32"
      );
      const vx = Module.getValue(
        agentDataPtr + i * 36 + agentObjectOffset.vx,
        "i32"
      );
      const vy = Module.getValue(
        agentDataPtr + i * 36 + agentObjectOffset.vy,
        "i32"
      );
    }
    // END TEST

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
        
        // Initialize WebRTC sessions for initial agents
        for (let i = 0; i < codes.length; i++) {
          addUser(i,0); // Add user with dummy pointer
          console.log(`🔧 Initialized WebRTC session for Agent ${i}`);
        }
      }
    }
  }, [Module]);

  // Compile web codes
  useEffect(() => {
    if (!Module || !isReady || !isCompiling) return;
    const selectedCode = codes[selectedIndex].code;
    console.log(`CompileWebCode index:${selectedIndex}`);
    const ret = Module.ccall(
      "compileWebCode",
      "number",
      [ "number", "string"],
      [ selectedIndex, selectedCode]
    );
    // change .shica to .stt, and meke output filename
    const outputFilename = codes[selectedIndex].filename.replace(
      /\.shica$/,
      ".stt"
    );

    if (ret) {
      processError();
      return;
    }
    addLog(
      LogLevel.SUCCESS,
      `shica ${outputFilename} -o ${codes[selectedIndex].filename}`
    );
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

  // Run web codes
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
        processError();
        const agentptr = Module.ccall(
          "getAnAgentDataPtr",
          "number",
          ["number"],
          [0]
        );
//REVIEW
        for (let i = 0; i < codes.length; i++) {
          const robot = robotsRef.current[i];
          const offset = i * 36; // 4 bytes each for x, y, vx, vy
          const index = Module.getValue(
            agentptr + offset + agentObjectOffset.index,
            "i32"
          );
          const x = Module.getValue(
            agentptr + offset + agentObjectOffset.x,
            "i32"
          );
          const y = Module.getValue(
            agentptr + offset + agentObjectOffset.y,
            "i32"
          );
          const vx = Module.getValue(
            agentptr + offset + agentObjectOffset.vx,
            "i32"
          );
          const vy = Module.getValue(
            agentptr + offset + agentObjectOffset.vy,
            "i32"
          );
          robot.r = Module.HEAPU8[agentptr + offset + agentObjectOffset.red]; // unsigned char shuld use HEAPU8
          robot.g = Module.HEAPU8[agentptr + offset + agentObjectOffset.green];
          robot.b = Module.HEAPU8[agentptr + offset + agentObjectOffset.blue];
          robot.x = x + vx;
          robot.y = y + vy;
          Module.setValue(
            agentptr + offset + agentObjectOffset.x,
            robot.x,
            "i32"
          );
          Module.setValue(
            agentptr + offset + agentObjectOffset.y,
            robot.y,
            "i32"
          );
        }
        setForceUpdate((prev) => prev + 1); // 位置更新を画面に反映
        setTime(time + fps);
        Module.setValue(Module.clickPtr + 8, 0, "i32"); // inactive
      }, fps);
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

  //Event handler for click on the map
  const clickEH = (x: number, y: number) => {
    if (!Module || !isReady) return;
    const rect = mapRef.current?.getBoundingClientRect();
    if (!rect) return;
    console.log(`Click at (${x}, ${y}), rect:`, rect);
    const xc = x - rect.left - 20 < 0 ? 0 : Math.round(x - rect.left - 20);
    const yc = y - rect.top - 20 < 0 ? 0 : Math.round(y - rect.top - 20);
    if (isRunning) {
      Module.setValue(Module.clickPtr + 0, xc, "i32");
      Module.setValue(Module.clickPtr + 4, yc, "i32");
      Module.setValue(Module.clickPtr + 8, 1, "i32"); // active
    }
    setClickXY({ x: xc, y: yc });
  };
  // Download code.shica button
  const downloadFile = (ext: string) => {
    if (!codes[selectedIndex].code.trim()) {
      alert("Please write code before downloading.");
      return;
    }

    // Blobオブジェクトを作成（ファイルの内容）
    const blob = new Blob([codes[selectedIndex].code], { type: "text/plain" });

    // ダウンロードリンクを作成
    const url = window.URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.href = url;
    link.download = codes[selectedIndex].filename + ext;

    // リンクをクリックしてダウンロード開始
    document.body.appendChild(link);
    link.click();

    // クリーンアップ
    document.body.removeChild(link);
    window.URL.revokeObjectURL(url);
  };

  // Colorset for user sample code
  const handleColorChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const hex = e.target.value;
    const rgbValue = hexToRgb(hex);
    setRgb(rgbValue);
  };

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      setCodes((prev) =>
        prev.map((item, i) =>
          i === selectedIndex
            ? { ...item, code: e.target?.result as string }
            : item
        )
      );
    };
    reader.readAsText(file); // テキストとして読み込む
  };
  // Download code.stt button
  const downloadSTTFile = async () => {
    const ret = Module.ccall(
      "getCompiledWebCode",
      "number",
      ["number"],
      [selectedIndex]
    );
    if (ret) {
      addLog(LogLevel.ERROR, "Failed to get compiled web code"); //TODO: reportError();
      return;
    }
    const u8 = Module.FS.readFile("/ints.bin"); // Uint8Array
    const blob = new Blob([u8], { type: "application/octet-stream" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = `${codes[selectedIndex].filename}.stt`;
    a.click();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
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
                        background: `rgb(${robot.r}, ${robot.g}, ${robot.b})`,
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
                <ShicaCodeEditor
                  key={codes[selectedIndex].filename}
                  filename={codes[selectedIndex].filename}
                  language=".shica"
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
              <label
                htmlFor="fileInput"
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 hover:scale-105`}
                style={{
                  backgroundColor: "var(--color-background-secondary)",
                  color: "var(--color-text-primary)",
                }}
              >
                <span className="text-sm text-gray-500">Upload File</span>
              </label>
              <input
                className="px-2 py-1 rounded bg-gray-100 text-gray-700"
                style={{ width: "200px" }}
                id="fileInput"
                type="file"
                hidden
                disabled={isRunning}
                accept=".c,.h,.txt"
                onChange={handleFileChange}
              />
              <button
                onClick={() => downloadFile(".shica")}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 hover:scale-105`}
                style={{
                  backgroundColor: "var(--color-background-secondary)",
                  color: "var(--color-text-primary)",
                }}
              >
                <span className="text-sm text-gray-500">
                  {codes[selectedIndex].filename}.shica
                </span>
              </button>

              <button
                onClick={() => downloadSTTFile()}
                className={`flex items-center space-x-2 px-4 py-2 rounded text-sm font-medium transition-all duration-200 hover:scale-105`}
                style={{
                  backgroundColor: "var(--color-background-secondary)",
                  color: "var(--color-text-primary)",
                }}
              >
                <span className="text-sm text-gray-500">
                  {codes[selectedIndex].filename}.stt
                </span>
              </button>
              <select
                disabled={isRunning}
                onChange={(e) => setFps(Number(e.target.value))}
                className="px-2 py-1 rounded bg-gray-100 text-gray-700"
              >
                <option value={500}>500</option>
                <option value={250}>250</option>
                <option value={100}>100</option>
                <option value={50}>50</option>
                <option value={25}>25</option>
              </select>
              <select>
                <option value="stt">State</option>
                <option value="send">Send</option>
                <option value="receive">Receive</option>
              </select>
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
              Shica IDE - Powered by Programming System Lab.
            </span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ShicaPage;
