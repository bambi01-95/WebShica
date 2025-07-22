'use client';
import React, { useState } from 'react';

export interface SidebarItem {
  id: string;
  name: string;
  description?: string;
  url: string;
  icon?: string;
  isActive?: boolean;
}

interface SidebarProps {
  items: SidebarItem[];
  title?: string;
  isCollapsible?: boolean;
  defaultCollapsed?: boolean;
  onItemClick?: (item: SidebarItem) => void;
}

export const Sidebar: React.FC<SidebarProps> = ({
  items,
  title = "目次",
  isCollapsible = false,
  defaultCollapsed = false,
  onItemClick
}) => {
  const [isCollapsed, setIsCollapsed] = useState(defaultCollapsed);

  const handleItemClick = (item: SidebarItem) => {
    if (onItemClick) {
      onItemClick(item);
    } else {
      window.location.href = item.url;
    }
  };

  const toggleSidebar = () => {
    setIsCollapsed(!isCollapsed);
  };

  return (
    <div className={`sidebar-component h-screen bg-white border-r border-orange-200 shadow-lg transition-all duration-300 flex-col ${
      isCollapsed ? 'w-24' : 'w-80'
    }`}>
      <style jsx>{`
        .sidebar-item {
          transition: all 0.2s ease;
        }
        .sidebar-item:hover {
          transform: translateX(4px);
          background: linear-gradient(90deg, rgba(255, 152, 0, 0.05), rgba(255, 152, 0, 0.1));
        }
        .sidebar-item.active {
          background: linear-gradient(90deg, rgba(255, 152, 0, 0.1), rgba(255, 152, 0, 0.15));
          border-right: 3px solid #ff9800;
        }
        .sidebar-icon {
          background: linear-gradient(135deg, #ff9800, #ffb74d);
          transition: all 0.2s ease;
        }
        .sidebar-item:hover .sidebar-icon {
          transform: scale(1.05);
          box-shadow: 0 4px 12px rgba(255, 152, 0, 0.3);
        }
        .collapse-btn {
          transition: all 0.2s ease;
        }
        .collapse-btn:hover {
          background: rgba(255, 152, 0, 0.1);
          transform: scale(1.05);
        }
      `}</style>

      {/* ヘッダー部分 */}
      <div className="flex items-center justify-between p-4 border-b border-orange-100">
        {!isCollapsed && (
          <h2 className="text-xl font-bold text-orange-900">{title}</h2>
        )}
        {isCollapsible && (
          <button
            onClick={toggleSidebar}
            className="collapse-btn p-2 rounded-lg text-orange-600 hover:text-orange-800"
            title={isCollapsed ? "サイドバーを展開" : "サイドバーを折りたたむ"}
          >
            <svg
              className={`w-5 h-5 transition-transform duration-300 ${
                isCollapsed ? 'rotate-180' : ''
              }`}
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M15 19l-7-7 7-7"
              />
            </svg>
          </button>
        )}
      </div>

      {/* アイテムリスト */}
      <div className="flex-1 overflow-y-auto">
        <nav className="p-2">
          {items.map((item) => (
            <div
              key={item.id}
              className={`sidebar-item p-3 rounded-lg cursor-pointer mb-1 ${
                item.isActive ? 'active' : ''
              }`}
              onClick={() => handleItemClick(item)}
              title={isCollapsed ? item.name : undefined}
            >
              <div className="flex items-center space-x-3">
                {/* アイコン */}
                <div className="sidebar-icon w-8 h-8 rounded-full flex items-center justify-center text-white text-sm font-medium shadow-sm flex-shrink-0">
                  {item.icon || item.name.charAt(0)}
                </div>

                {/* テキスト部分 */}
                {!isCollapsed && (
                  <div className="flex-1 min-w-0">
                    <h3 className="text-sm font-semibold text-orange-900 truncate">
                      {item.name}
                    </h3>
                    {item.description && (
                      <p className="text-xs text-gray-600 mt-1 line-clamp-2">
                        {item.description}
                      </p>
                    )}
                  </div>
                )}

                {/* アクティブインジケーター */}
                {!isCollapsed && item.isActive && (
                  <div className="w-2 h-2 bg-orange-500 rounded-full flex-shrink-0"></div>
                )}
              </div>
            </div>
          ))}
        </nav>
      </div>

      {/* フッター部分（オプション） */}
      {!isCollapsed && (
        <div className="border-t border-orange-100 p-4">
          <div className="text-xs text-gray-500 text-center">
            {items.length} ページ
          </div>
        </div>
      )}
    </div>
  );
};

export default Sidebar;