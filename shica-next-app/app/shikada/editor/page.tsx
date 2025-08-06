"use client";
import { CodeEditor } from "@/component/code/CodeEditor";
import { useState } from "react";
import Output from "@/component/code/Output";
import ThemeToggleButton from "@/component/ui/ThemeToggleButton";

const EditorPage = () => {
  const [code, setCode] = useState<{ filename: string; code: string }>({
    filename: "test0.shica",
    code: "",
  });

  return (
    <div className="hidden xl:flex flex-col w-full h-screen">
      {/* メインエディタエリア */}
      <div
        className="flex-1 flex flex-col"
        style={{
          backgroundColor: "var(--color-background-primary)",
        }}
      >
        {/* ヘッダー */}
        <div className="flex justify-between items-center p-2">
          <h1 className="text-lg font-bold">
            Shica Code Editor for Native Environment
          </h1>
          <ThemeToggleButton
            background="var(--color-code-background-800)"
            color="var(--color-text-primary)"
          />
        </div>

        {/* メインコンテンツ */}
        <div
          className="flex flex-1 h-full"
          style={{ border: "1px solid var(--code-background-700)" }}
        >
          {/* サイドバー - コントロールボタン */}
          <div className="flex flex-col items-start p-4 w-64">
            <button className="code-button shadow-md w-full p-2 mb-2 rounded-full">
              Compile
            </button>
            <button className="code-button shadow-md w-full p-2 mb-2 rounded-full">
              Download .shica
            </button>
            <button className="code-button shadow-md w-full p-2 mb-2 rounded-full">
              Download .stt
            </button>
          </div>

          {/* エディタ・出力エリア */}
          <div className="flex-1 flex flex-col">
            {/* コードエディタ */}
            <div className="flex-1 min-h-0">
              <CodeEditor
                filename={code.filename}
                initialCode={code.code}
                onCodeChange={(newCode) =>
                  setCode((prev) => ({ ...prev, code: newCode }))
                }
                width="w-full"
                height="h-full"
                isRounded={false}
              />
            </div>

            {/* 出力セクション */}
            <div className="flex-1 min-h-0">
              <div className="h-full overflow-y-auto">
                <Output isRounded={false} />
              </div>
            </div>
          </div>
        </div>

        {/* フッター */}
        <div
          className="py-2"
          style={{ backgroundColor: "var(--color-background-secondary)" }}
        >
          <p
            className="text-xs text-center"
            style={{ color: "var(--color-text-secondary)" }}
          >
            Programming system lab
          </p>
        </div>
      </div>
    </div>
  );
};

export default EditorPage;
