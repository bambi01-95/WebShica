"use client";
import { CodeEditor } from "@/component/code/CodeEditor";
import { useState } from "react";
import FileLists from "@/component/code/FileLists";
import Console from "@/component/code/Console";
import Terminal from "@/component/code/Terminal";

const EditorPage = () => {
  const [code, setCode] = useState<{ filename: string; code: string }[]>([
    { filename: "test0.shica", code: "" },
  ]);
  const [selectedIndex, setSelectedIndex] = useState(0);

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

  return (
    <div style={{ height: "100vh", display: "flex" }}>
      {/* 縦タブバー */}
      <FileLists
        code={code}
        selectedIndex={selectedIndex}
        setSelectedIndex={setSelectedIndex}
        removeItem={removeItem}
        disableRemove={code.length <= 1}
        addItem={addItem}
      />

      {/* エディタエリア */}
      <div
        style={{
          flex: 1,
          backgroundColor: "#1e1e1e",
          display: "flex",
          flexDirection: "column",
        }}
      >
        {code.length > 0 && (
          <>
            {/* 現在のファイル名表示 */}
            <div
              style={{
                padding: "8px 16px",
                backgroundColor: "#2d2d30",
                borderBottom: "1px solid #3e3e42",
                color: "#cccccc",
                fontSize: "13px",
                fontWeight: "500",
              }}
            >
              {code[selectedIndex].filename}
            </div>

            {/* エディタ */}
            <div className="flex-1">
              <CodeEditor
                key={code[selectedIndex].filename}
                filename={code[selectedIndex].filename}
                initialCode={code[selectedIndex].code}
                onCodeChange={(newCode) => updateItem(selectedIndex, newCode)}
              />
            </div>
            <div style={{ display: "flex", flexDirection: "row" }}>
              <Console />
              <Terminal />
            </div>
          </>
        )}
      </div>
    </div>
  );
};

export default EditorPage;
