"use client";
import Sidebar from "@/component/page/Sidebar";
import type { SidebarItem } from "@/component/page/Sidebar";
import ThemeToggleButton from "@/component/ui/ThemeToggleButton";

const sidebarItems: SidebarItem[] = [
  {
    id: "1",
    name: "ホーム",
    description: "メインページです",
    url: "/",
    icon: "🏠",
    isActive: true,
  },
  {
    id: "2",
    name: "プロフィール",
    description: "私について",
    url: "/profile",
    icon: "👤",
  },
  {
    id: "3",
    name: "プロジェクト",
    description: "制作物一覧",
    url: "/projects",
    icon: "💼",
  },
];

const InstructionPage = () => {
  return (
    <div className="flex flex-row">
      <Sidebar
        items={sidebarItems}
        title="サイト目次"
        isCollapsible={true}
        defaultCollapsed={false}
        onItemClick={(item) => console.log("Clicked:", item.name)}
      />
      <div className="min-h-screen p-8 bg-background-primary text-primary-text">
        <h1 className="text-4xl font-bold mb-4">ようこそ！</h1>
        <p className="text-lg mb-8">
          これはNext.jsとTailwind
          CSSで作成されたテーマ切り替え可能なページです。
        </p>
        <ThemeToggleButton />

        <div className="mt-8 p-4 rounded-lg bg-background-secondary shadow-md">
          <h2 className="text-2xl font-semibold mb-2">
            これはコンテンツブロックです
          </h2>
          <p className="text-secondary-text">
            背景色とテキスト色がテーマによって変化します。
          </p>
        </div>
      </div>
    </div>
  );
};

export default InstructionPage;
