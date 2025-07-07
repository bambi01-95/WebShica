const editor = ace.edit("editor");
editor.session.setMode("ace/mode/c_cpp");
editor.setTheme("ace/theme/tomorrow_night");

// 初期のRGB値を設定
const initialColor = "#3498db";
const initialRgb = hexToRgb(initialColor);
editor.session.setValue(
  `// Your Web Shica code here\n// Example:\n// setColorRGB(${initialRgb.r},${initialRgb.g},${initialRgb.b});\nstt s1(){\n    clickEH(x,y){\n        setXY(x,y);\n    }\n}`
);

let runInterval = null;
let timerPtr, clickPtr;
let agentPtr;
let editorCount = 1;
let editors = [];
let robots = [];
let robotColors = [initialColor]; // ロボットの色を管理する配列

const compileBtn = document.getElementById("compileBtn");
const runBtn = document.getElementById("runBtn");
const addBtn = document.getElementById("addBtn");
const editorsContainer = document.getElementById("editors-container");
const outputIR = document.getElementById("outputIR");
const outputResult = document.getElementById("outputResult");
const infoDisplay = document.getElementById("info");
const clickInfoDisplay = document.getElementById("clickInfo");
const timerInfoDisplay = document.getElementById("timerInfo");
const robot = document.getElementById("robot1");
const map = document.getElementById("map");
const colorPicker1 = document.getElementById("colorPicker1");
const colorDisplay1 = document.getElementById("colorDisplay1");

// 16進数カラーをRGBに変換する関数
function hexToRgb(hex) {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result
    ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16),
      }
    : null;
}

// エディタのコード例を更新する関数
function updateCodeExample(editorInstance, color, robotNum) {
  const rgb = hexToRgb(color);
  if (rgb) {
    const currentCode = editorInstance.getValue();
    const newCode = currentCode.replace(
      /\/\/ setColorRGB\(\d+,\d+,\d+\);/g,
      `// setColorRGB(${rgb.r},${rgb.g},${rgb.b});`
    );
    editorInstance.session.setValue(newCode);
  }
}

// ロボットの色を変更する関数
function updateRobotColor(robotElement, color) {
  const rgb = hexToRgb(color);
  if (rgb) {
    robotElement.style.filter = `none`;
    robotElement.style.borderColor = color;
    robotElement.style.boxShadow = `0 4px 15px ${color}50`;

    // 各ロボットの中央の円の色を個別に設定
    const robotId = robotElement.id;
    const beforeStyle = `
            #${robotId}::before {
              background: ${color} !important;
            }
          `;

    // 既存のスタイル要素を削除（あれば）
    const existingStyle = document.getElementById(`style-${robotId}`);
    if (existingStyle) {
      existingStyle.remove();
    }

    // 新しいスタイル要素を追加
    const styleElement = document.createElement("style");
    styleElement.id = `style-${robotId}`;
    styleElement.textContent = beforeStyle;
    document.head.appendChild(styleElement);
  }
}

// 初期のカラーピッカーイベントリスナー
colorPicker1.addEventListener("change", (e) => {
  const newColor = e.target.value;
  robotColors[0] = newColor;
  colorDisplay1.style.backgroundColor = newColor;
  updateRobotColor(robot, newColor);
  updateCodeExample(editor, newColor, 1);
});

Module.onRuntimeInitialized = () => {
  console.log("WASM initialized");

  Module.ccall("memory_init", "number", [], []);
  timerPtr = Module.ccall("initWebTimerPtr", "number", [], []);
  clickPtr = Module.ccall("initWebClickSTTPtr", "number", [], []);
  agentPtr = Module.ccall("initAnAgnetDataPtr", "number", [], []);

  // 最初のエディタとロボットを配列に追加
  editors.push(editor);
  robots.push(robot);

  // 初期ロボットの色を設定
  updateRobotColor(robot, initialColor);

  // Add Editor ボタンのイベントリスナー
  addBtn.addEventListener("click", () => {
    editorCount++;
    const newEditorId = `editor${editorCount}`;
    const newCompileBtnId = `compileBtn${editorCount}`;
    const newRobotId = `robot${editorCount}`;
    const newColorPickerId = `colorPicker${editorCount}`;
    const newColorDisplayId = `colorDisplay${editorCount}`;

    // 新しいロボットの色を生成（HSL色相を使用）
    const hue = ((editorCount - 1) * 60) % 360;
    const newColor = `hsl(${hue}, 70%, 60%)`;
    const newColorHex = hslToHex(hue, 70, 60);
    robotColors.push(newColorHex);

    // 新しいロボットのHTMLを作成
    const newRobotHTML = `<div class="robot-vacuum" id="${newRobotId}"></div>`;

    // マップに新しいロボットを追加
    map.insertAdjacentHTML("beforeend", newRobotHTML);

    // 新しいロボット要素を取得して配列に追加
    const newRobotElement = document.getElementById(newRobotId);
    robots.push(newRobotElement);

    // 新しいエディタのHTMLを作成
    const newEditorHTML = `
            <details open>
              <summary>
                <span>Web Shica Code Editor ${editorCount} (Robot ${editorCount})</span>
                <div class="color-picker-container">
                  <div class="color-display" id="${newColorDisplayId}" style="background-color: ${newColorHex};"></div>
                  <input type="color" class="color-picker" id="${newColorPickerId}" value="${newColorHex}">
                </div>
              </summary>
              <div class="control-group">
                <div id="${newEditorId}" class="editor-container"></div>
                <div class="button-container">
                  <button id="${newCompileBtnId}" class="compile-btn">Compile</button>
                </div>
              </div>
            </details>
          `;

    // コンテナに追加
    editorsContainer.insertAdjacentHTML("beforeend", newEditorHTML);

    // 少し待ってからACEエディタを初期化（DOM要素が確実に存在するため）
    setTimeout(() => {
      const newEditor = ace.edit(newEditorId);
      newEditor.session.setMode("ace/mode/c_cpp");
      newEditor.setTheme("ace/theme/tomorrow_night");

      const rgb = hexToRgb(newColorHex);
      newEditor.session.setValue(
        `// Web Shica code editor ${editorCount} for Robot ${editorCount}\n// Example:\n// setColorRGB(${rgb.r},${rgb.g},${rgb.b});\nstt s${editorCount}(){\n    clickEH(x,y){\n        setXY(x,y);\n    }\n}`
      );

      // エディタを配列に追加
      editors.push(newEditor);

      // 新しいロボットの色を設定
      updateRobotColor(newRobotElement, newColorHex);

      // 新しいカラーピッカーのイベントリスナー
      const newColorPicker = document.getElementById(newColorPickerId);
      const newColorDisplay = document.getElementById(newColorDisplayId);

      newColorPicker.addEventListener("change", (e) => {
        const selectedColor = e.target.value;
        robotColors[editorCount - 1] = selectedColor;
        newColorDisplay.style.backgroundColor = selectedColor;
        updateRobotColor(newRobotElement, selectedColor);
        updateCodeExample(newEditor, selectedColor, editorCount);
      });

      // 新しいCompileボタンのイベントリスナー
      document.getElementById(newCompileBtnId).addEventListener("click", () => {
        const code = newEditor.getValue();
        outputIR.textContent = "Compiling...";
        setTimeout(() => {
          const result = Module.ccall(
            "compileWebCode",
            "number",
            ["string"],
            [code]
          );
          outputIR.textContent = `Compile result: ${
            result === 1 ? "Success" : "Failed"
          }\nReturn code: ${result} (Robot ${editorCount})`;
        }, 10);
      });
    }, 100);
  });

  compileBtn.addEventListener("click", () => {
    const code = editor.getValue();
    outputIR.textContent = "Compiling...";
    setTimeout(() => {
      const result = Module.ccall(
        "compileWebCode",
        "number",
        ["string"],
        [code]
      );
      outputIR.textContent = `Compile result: ${
        result === 1 ? "Success" : "Failed"
      }\nReturn code: ${result}`;
    }, 10);
  });

  runBtn.addEventListener("click", () => {
    if (runInterval) {
      clearInterval(runInterval);
      runInterval = null;
      runBtn.textContent = "Run";
      runBtn.classList.remove("running");
      outputResult.textContent = "Execution stopped.";
      return;
    }

    Module.ccall("initRunWeb", "number", [], []);
    runBtn.textContent = "Stop";
    runBtn.classList.add("running");
    outputResult.textContent = "Execution running...";

    runInterval = setInterval(() => {
      const result = Module.ccall("runWeb", "number", [], []);

      // Update Timer
      const now = Date.now();
      Module.setValue(timerPtr, now, "i32");
      const timerVal = Module.getValue(timerPtr, "i32");

      // Read AgentData
      // ROBOT POSITION
      const x = Module.getValue(agentPtr + 0, "i32");
      const y = Module.getValue(agentPtr + 4, "i32");
      // ROBOT VELOCITY
      const vx = Module.getValue(agentPtr + 8, "i32");
      const vy = Module.getValue(agentPtr + 12, "i32");
      // ROBOT CLICK
      const isClick = Module.getValue(agentPtr + 16, "i32");
      const distance = Module.getValue(agentPtr + 20, "i32");
      const status = Module.getValue(agentPtr + 24, "i32");
      // FEATURE WORKS HERE
      /*
      // ROBOT COLOR
      const r = Module.getValue(agentPtr + 25, "i8");
      const g = Module.getValue(agentPtr + 26, "i8");
      const b = Module.getValue(agentPtr + 27, "i8");
      // ROBOT LED ON/OFF
      const isLedOn = Module.getValue(agentPtr + 28, "i8");
      */

      // Apply velocity
      Module.setValue(agentPtr + 0, x + vx, "i32");
      Module.setValue(agentPtr + 4, y + vy, "i32");

      // Read Click Data
      const clickX = Module.getValue(clickPtr + 0, "i32");
      const clickY = Module.getValue(clickPtr + 4, "i32");
      const clickStatus = Module.getValue(clickPtr + 8, "i32");

      // Update UI for all robots
      robots.forEach((robotElement, index) => {
        const robotX = x + vx + index * 60; // 各ロボットを少しずつずらして表示
        const robotY = y + vy + index * 60;
        infoDisplay.textContent = `Robots: ${robots.length} active`;
        robotElement.style.transform = `translate(${robotX}px, ${robotY}px) translate(-50%, -50%)`;
      });

      clickInfoDisplay.textContent = `Click: ${
        clickStatus ? `Active at (${clickX}, ${clickY})` : "Inactive"
      }`;
      timerInfoDisplay.textContent = `Timer: ${timerVal}`;
    }, 50);
  });
};

// HSLをHexに変換する関数
function hslToHex(h, s, l) {
  l /= 100;
  const a = (s * Math.min(l, 1 - l)) / 100;
  const f = (n) => {
    const k = (n + h / 30) % 12;
    const color = l - a * Math.max(Math.min(k - 3, 9 - k, 1), -1);
    return Math.round(255 * color)
      .toString(16)
      .padStart(2, "0");
  };
  return `#${f(0)}${f(8)}${f(4)}`;
}

map.addEventListener("mousedown", (e) => {
  const mapRect = map.getBoundingClientRect();
  const robotSize = 50; // CSSで設定したロボットのサイズ
  const halfRobotSize = robotSize / 2;

  // クリック座標を計算
  let x = e.clientX - mapRect.left;
  let y = e.clientY - mapRect.top;

  // ✨ ロボットがマップからはみ出さないように座標を制限する
  x = Math.max(halfRobotSize, Math.min(x, mapRect.width - halfRobotSize));
  y = Math.max(halfRobotSize, Math.min(y, mapRect.height - halfRobotSize));

  Module.setValue(clickPtr + 0, Math.round(x), "i32");
  Module.setValue(clickPtr + 4, Math.round(y), "i32");
  Module.setValue(clickPtr + 8, 1, "i32"); // active
});

window.addEventListener("mouseup", () => {
  Module.setValue(clickPtr + 8, 0, "i32"); // inactive
});
