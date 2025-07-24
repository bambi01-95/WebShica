import FileList from "./FileList";

interface FileListsProps {
  code: { filename: string; code: string }[];
  selectedIndex: number;
  setSelectedIndex: (index: number) => void;
  removeItem: (index: number) => void;
  disableRemove: boolean;
  addItem: (newItem: string) => void;
  MAX_FILE_COUNT?: number;
  width?: string;
  height?: string;
}

export default function FileLists({
  code,
  selectedIndex,
  setSelectedIndex,
  removeItem,
  MAX_FILE_COUNT=10,
  addItem,
  width="w-full",
  height="h-full",
}: FileListsProps) {
  return (
    <div
      className={`bg-gray-900 ${width} ${height}`}
      style={{
        backgroundColor: "var(--color-background-primary)",
        border: "1px solid var(--color-code-background700)",
        display: "flex",
        flexDirection: "column",
        overflow: "auto",
      }}
    >
      <div style={{ flex: 1 }}>
        {code.map((item, index) => (
          <FileList
            key={index}
            index={index}
            selectedIndex={selectedIndex}
            setSelectedIndex={setSelectedIndex}
            removeItem={removeItem}
            disableRemove={code.length <= 1}
            filename={item.filename}
          />
        ))}
      </div>
      {/* 新規ファイル追加ボタン */}
      <div
        style={{
          padding: "12px",
          borderTop: "1px solid var(--color-code-background700)",
        }}
      >
        <button
          onClick={() => {
            if (code.length>MAX_FILE_COUNT) return;
            addItem("");
          }}
          style={{
            width: "100%",
            height: "32px",
            border: "1px solid var(--color-code-background700)",
            borderRadius: "3px",
            cursor: "pointer",
            color: "var(--color-text-secondary)",
            fontSize: "13px",
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            backgroundColor: "var(--color-background-secondary)",
            transition: "background-color 0.2s",
          }}
          onMouseOver={e => (e.currentTarget.style.backgroundColor = "var(--color-background-secondary-hover)")}
          onMouseOut={e => (e.currentTarget.style.backgroundColor = "var(--color-background-secondary)")}
        >
          + Add New File
        </button>
      </div>
    </div>
  );
}
