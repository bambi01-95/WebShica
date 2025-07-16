// app/layout.tsx or app/shikada/shica/page.tsx
import Script from "next/script";
import "./shica.css";

export default function Layout({ children }: { children: React.ReactNode }) {
  return (
    <>
      {children}
    </>
  );
}