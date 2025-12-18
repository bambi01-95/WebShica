// components/Sidebar.tsx
interface SidebarProps {
  lessons: Array<{ id: number; title: string }>
  currentLesson: number
  onLessonSelect: (index: number) => void
}
function Sidebar({ lessons, currentLesson, onLessonSelect }: SidebarProps) {
  return (
    <aside className="sticky top-6 h-fit border-r border-gray-200 bg-white">
      <div className="px-6 py-5">
        <h3 className="text-sm font-semibold text-gray-500 uppercase tracking-wide mb-4">
          Lessons
        </h3>

        <ul className="space-y-1">
          {lessons.map((lesson, index) => {
            const active = currentLesson === index;

            return (
              <li
                key={lesson.id}
                onClick={() => onLessonSelect(index)}
                className={`
                  cursor-pointer px-3 py-2 text-mmd transition-colors
                  ${active
                    ? 'border-l-4 border-slate-500 bg-slate-50 text-slate-900 font-medium'
                    : 'text-gray-700 hover:bg-gray-100'}
                `}
              >
                {lesson.title}
              </li>
            );
          })}
        </ul>
      </div>
    </aside>
  );
}


export default Sidebar