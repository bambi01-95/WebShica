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
        }}
        className="bg-gray-800"
      >
        <button
          onClick={() => {
            if (code.length>MAX_FILE_COUNT) return;
            addItem("");
          }}
          style={{
            width: "100%",
            height: "32px",
            border: "1px solid #3e3e42",
            borderRadius: "3px",
            cursor: "pointer",
            color: "#cccccc",
            fontSize: "13px",
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
          }}
          className="bg-gray-700"
          onMouseEnter={(e) => {
            e.currentTarget.classList.add("bg-gray-700");
          }}
          onMouseLeave={(e) => {
            e.currentTarget.classList.remove("bg-gray-700");
          }}
        >
          + Add New File
        </button>
      </div>
    </div>
  );
}
