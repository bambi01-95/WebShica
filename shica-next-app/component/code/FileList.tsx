// FileList.tsx
import React from 'react';

interface FileListProps {
  /** ファイルのインデックス */
  index: number;
  /** 現在選択されているファイルのインデックス */
  selectedIndex: number;
  /** ファイルを選択したときのハンドラ */
  setSelectedIndex: (index: number) => void;
  /** ファイルを削除したときのハンドラ */
  removeItem: (index: number) => void;
  /** 削除ボタンを無効にするかどうか */
  disableRemove: boolean;
}

const FileList: React.FC<FileListProps> = ({
  index,
  selectedIndex,
  setSelectedIndex,
  removeItem,
  disableRemove,
}) => {
  const isSelected = selectedIndex === index;

  return (
    <div
      style={{
        display: 'flex',
        alignItems: 'center',
        padding: '10px 12px',
        borderBottom: '1px solid #3e3e42',
        cursor: 'pointer',
        fontSize: '13px',
        position: 'relative',
        borderLeft: isSelected ? '3px solid #007acc' : '3px solid transparent',
      }}
      className={`${isSelected ? 'bg-gray-800' : 'bg-gray-900'} text-gray-300`}
      onClick={() => setSelectedIndex(index)}
      onMouseEnter={(e) => {
        if (!isSelected) e.currentTarget.classList.add('bg-gray-800');
      }}
      onMouseLeave={(e) => {
        if (!isSelected) e.currentTarget.classList.remove('bg-gray-800');
      }}
    >
      <span
        style={{
          flex: 1,
          overflow: 'hidden',
          textOverflow: 'ellipsis',
          whiteSpace: 'nowrap',
        }}
      >
        test{index}.shica
      </span>
      <button
        onClick={(e) => {
          e.stopPropagation();
          removeItem(index);
        }}
        className="bg-gray-700"
        style={{
          marginLeft: '8px',
          width: '16px',
          height: '16px',
          border: 'none',
          cursor: 'pointer',
          color: '#cccccc',
          fontSize: '12px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          borderRadius: '3px',
          opacity: isSelected ? 1 : 0.7,
        }}
        disabled={disableRemove}
        onMouseEnter={(e) => {
          e.currentTarget.style.backgroundColor = '#3e3e42';
          e.currentTarget.style.opacity = '1';
        }}
        onMouseLeave={(e) => {
          e.currentTarget.style.backgroundColor = 'transparent';
          e.currentTarget.style.opacity = isSelected ? '1' : '0.7';
        }}
      >
        ×
      </button>
    </div>
  );
};

export default FileList;