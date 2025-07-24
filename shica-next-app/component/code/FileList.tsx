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
  filename: string;
}

const FileList: React.FC<FileListProps> = ({
  index,
  selectedIndex,
  setSelectedIndex,
  removeItem,
  disableRemove,
  filename,
}) => {
  const isSelected = selectedIndex === index;

  return (
    <div
      style={{
        display: 'flex',
        alignItems: 'center',
        padding: '10px 12px',
        borderBottom: '1px solid var(--color-code-background700)',
        cursor: 'pointer',
        fontSize: '13px',
        position: 'relative',
        borderLeft: isSelected ? '3px solid #007acc' : '3px solid transparent',
        backgroundColor: isSelected ? 'var(--color-background-secondary)' : 'transparent',
        color: isSelected ? 'var(--color-text-primary)' : 'var(--color-text-secondary)'
      }}
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
        {filename}
      </span>
      <button
        onClick={(e) => {
          e.stopPropagation();
          removeItem(index);
        }}
        style={{
          marginLeft: '8px',
          width: '16px',
          height: '16px',
          border: 'none',
          cursor: 'pointer',
          color: 'var(--color-text-secondary)',
          fontSize: '12px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          borderRadius: '3px',
          opacity: isSelected ? 1 : 0.7,
        }}
        disabled={disableRemove}
        onMouseEnter={(e) => {
          e.currentTarget.style.backgroundColor = 'var(--color-background-secondary-hover)';
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