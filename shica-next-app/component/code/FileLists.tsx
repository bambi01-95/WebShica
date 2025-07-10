import FileList from "./FileList";

interface FileListsProps {
  code: { filename: string; code: string }[];
  selectedIndex: number;
  setSelectedIndex: (index: number) => void;
  removeItem: (index: number) => void;
  disableRemove: boolean;
  addItem: (newItem: string) => void;
  MAX_FILE_COUNT?: number;
}

export default function FileLists({
  code,
  selectedIndex,
  setSelectedIndex,
  removeItem,
  MAX_FILE_COUNT=10,
  addItem,
}: FileListsProps) {
  return (
    <div
      style={{
        width: "200px",
        backgroundColor: "#2d2d30",
        borderRight: "1px solid #3e3e42",
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
          />
        ))}
      </div>
      {/* 新規ファイル追加ボタン */}
      <div
        style={{
          padding: "12px",
          borderTop: "1px solid #3e3e42",
          backgroundColor: "#252526",
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
            backgroundColor: "transparent",
            border: "1px solid #3e3e42",
            borderRadius: "3px",
            cursor: "pointer",
            color: "#cccccc",
            fontSize: "13px",
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
          }}
          onMouseEnter={(e) => {
            e.currentTarget.style.backgroundColor = "#3e3e42";
          }}
          onMouseLeave={(e) => {
            e.currentTarget.style.backgroundColor = "transparent";
          }}
        >
          + Add New File
        </button>
      </div>
    </div>
  );
}
