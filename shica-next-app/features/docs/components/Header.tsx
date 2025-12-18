// components/Header.tsx
import React from "react";
import Link from "next/link";

interface HeaderProps {
  title?: string;
  subtitle?: string;
}

const docsMenu = [
  { label: "W-Shica Docs", href: "/docs/w-shica" },
  { label: "R-Shica Docs", href: "/docs/r-shica" },
];

export default function Header({
  title = "W-Shica Documentation",
  subtitle = "A programming language for state-based, event-driven, and distributed physical computing systems.",
}: HeaderProps) {
  return (
    <header className="w-full bg-slate-900 text-white">
      {/* Hero */}
      <div className="max-w-7xl mx-auto px-6 py-16">
        <h1 className="text-4xl md:text-5xl font-semibold tracking-tight mb-4">
          {title}
        </h1>
        <p className="text-lg md:text-xl text-slate-300 max-w-3xl leading-relaxed">
          {subtitle}
        </p>
      </div>

      {/* Navigation */}
      <nav
        aria-label="Main navigation"
      >
        <div className="max-w-7xl mx-auto px-6">
          <ul className="flex items-center gap-8 py-4 text-sm md:text-base">
            <NavLink href="/">Home</NavLink>
            <NavLink href="/user-study">User Study</NavLink>
            <NavLink href="/docs/w-shica">Docs</NavLink>
            <NavLink href="/ide">IDE</NavLink>
          </ul>
        </div>
      </nav>
    </header>
  );
}

/* -------------------------
 * Sub components
 * ------------------------ */

function NavLink({
  href,
  children,
}: {
  href: string;
  children: React.ReactNode;
}) {
  return (
    <li>
      <Link
        href={href}
        className="hover:text-slate-300 transition-colors hover:underline"
      >
        {children}
      </Link>
    </li>
  );
}
