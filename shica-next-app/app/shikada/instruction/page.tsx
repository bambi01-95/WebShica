"use client";
import Sidebar from "@/component/page/Sidebar";
import type { SidebarItem } from "@/component/page/Sidebar";

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
    <div>
      <div className="h-full">
        <Sidebar
          items={sidebarItems}
          title="サイト目次"
          isCollapsible={true}
          defaultCollapsed={false}
          onItemClick={(item) => console.log("Clicked:", item.name)}
        />
      </div>
    </div>
  );
};

export default InstructionPage;
