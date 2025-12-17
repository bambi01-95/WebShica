// components/Header.tsx
interface HeaderProps {
    title?: string;
    subtitle?: string;
}
function Header({ 
    title="W-Shica Documentation", 
    subtitle=" A programming language for state-based, event-driven,and distributed physical computing systems." 
}: HeaderProps
) {
  return (
    <header className="w-full bg-slate-900 text-white">
      <div className="max-w-6xl mx-auto px-6 py-16">
        <h1 className="text-4xl md:text-5xl font-semibold tracking-tight mb-4">
            {title}
        </h1>
        <p className="text-lg md:text-xl text-slate-300 max-w-3xl">
            {subtitle}
        </p>
      </div>
    </header>
  );
}

export default Header