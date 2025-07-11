'use client';
import React from 'react';

interface PageItem {
  id: string;
  name: string;
  description: string;
  url: string;
  profileImage: string;
}

export function PageList(props: { pages: PageItem[] }) {
  const { pages } = props;

  const handleOpenPage = (url: string) => {
    window.open(url, '_blank');
  };

  return (
    <div className="page-list-component">
      <style jsx>{`
        .page-card {
          transition: all 0.3s ease;
        }
        .page-card:hover {
          transform: translateY(-2px);
          box-shadow: 0 10px 30px rgba(255, 152, 0, 0.2);
        }
        .profile-image {
          background: linear-gradient(135deg, #ff9800, #ffb74d);
          transition: all 0.3s ease;
        }
        .page-card:hover .profile-image {
          transform: scale(1.05);
        }
      `}</style>

      <div className="mb-8">
        <h1 className="text-4xl font-bold text-orange-900 mb-2">Pages</h1>
        <p className="text-orange-700 text-lg">List of pages</p>
      </div>

      <div className="grid gap-6 md:grid-cols-2 lg:grid-cols-3">
        {pages.map((page) => (
          <div
            key={page.id}
            className="page-card bg-white rounded-lg shadow-md hover:shadow-lg border border-orange-200 overflow-hidden p-6"
          >
            <div className="flex flex-col items-center text-center space-y-4">
              {/* プロフィール画像 */}
              <div className="profile-image w-16 h-16 rounded-full flex items-center justify-center text-2xl shadow-lg">
                {page.profileImage}
              </div>

              {/* 名前 */}
              <h3 className="text-xl font-semibold text-orange-900">
                {page.name}
              </h3>

              {/* 説明 */}
              <p className="text-gray-600 text-sm leading-relaxed">
                {page.description}
              </p>

              {/* 開くボタン */}
              <button 
                onClick={() => handleOpenPage(page.url)}
                className="bg-gradient-to-r from-orange-500 to-amber-500 text-white px-6 py-2 rounded-lg font-medium hover:from-orange-600 hover:to-amber-600 transition-all duration-200 shadow-md hover:shadow-lg w-full"
              >
                Open
              </button>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default PageList;