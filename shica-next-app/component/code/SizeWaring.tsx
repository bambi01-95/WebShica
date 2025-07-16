import React from "react";

const SizeWarningPage: React.FC = () => {
  return (
    <div className="min-h-screen bg-gray-800 text-[#cccccc] font-mono">
      {/* VS Code風タイトルバー */}
      <div className="bg-gray-700 border-b border-gray-600 px-4 py-2 flex items-center">
        <div className="ml-4 text-sm text-gray-400">
          Application - Shica IDE
        </div>
      </div>

      {/* メインコンテンツ */}
      <div className="flex flex-col items-center justify-center min-h-[calc(100vh-40px)] px-8">
        <div className="max-w-md w-full bg-gray-700 border border-gray-600 rounded-lg p-8 text-center">
          {/* メインメッセージ */}
          <h1 className="text-xl font-semibold mb-4 text-white">
            The page size is too small
          </h1>

          <p className="text-gray-400 mb-6 leading-relaxed">
            The page size is too small, please adjust the browser size for
            optimal display.
          </p>

          {/* 推奨アクション */}
          <div className="space-y-3 mb-6">
            <div className="flex items-center justify-center space-x-2 text-sm text-gray-400">
              <span>Please adjust the browser size for optimal display</span>
            </div>
          </div>

          <button
            onClick={() => window.location.reload()}
            className="w-full bg-blue-500 hover:bg-blue-600 text-white py-2 px-4 rounded-md shadow-md hover:shadow-lg text-sm"
          >
            Reload
          </button>
        </div>

        {/* フッター情報 */}
        <div className="mt-8 text-xs text-gray-400 text-center">
          <p className="mt-1">
            Please adjust the browser size for optimal display
          </p>
        </div>
      </div>
    </div>
  );
};

export default SizeWarningPage;
