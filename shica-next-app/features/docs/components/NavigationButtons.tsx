// components/NavigationButtons.tsx
interface NavigationButtonsProps {
  currentLesson: number
  totalLessons: number
  onNavigate: (direction: number) => void
}

function NavigationButtons({
  currentLesson,
  totalLessons,
  onNavigate
}: NavigationButtonsProps) {
  return (
    <nav className="mt-16 flex items-center justify-between border-t border-slate-200 pt-6">
      
      {/* Previous */}
      <button
        onClick={() => onNavigate(-1)}
        disabled={currentLesson === 0}
        className={`
          text-md font-medium
          ${currentLesson === 0
            ? 'text-slate-400 cursor-not-allowed'
            : 'text-sky-600 hover:text-sky-800'}
        `}
      >
        ← Previous
      </button>

      {/* Next */}
      <button
        onClick={() => onNavigate(1)}
        disabled={currentLesson === totalLessons - 1}
        className={`
          text-md font-medium
          ${currentLesson === totalLessons - 1
            ? 'text-slate-400 cursor-not-allowed'
            : 'text-sky-600 hover:text-sky-800'}
        `}
      >
        Next →
      </button>
    </nav>
  );
}

export default NavigationButtons