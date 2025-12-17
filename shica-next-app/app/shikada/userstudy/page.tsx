'use client'


import { useState } from 'react'
import Header from '../../../features/docs/components/Header'
import Sidebar from '../../../features/docs/components/Sidebar'
import LessonContent from '../../../features/docs/components/LessonContent'
import NavigationButtons from '../../../features/docs/components/NavigationButtons'
import { userstudy } from '../../../features/docs/data/userstudy'

export function UserStudyPage() {
  const [currentLesson, setCurrentLesson] = useState(0)

  const handleLessonChange = (index: number) => {
    setCurrentLesson(index)
    window.scrollTo({ top: 0, behavior: 'smooth' })
  }

  const navigate = (direction: number) => {
    const newLesson = currentLesson + direction
    if (newLesson >= 0 && newLesson < userstudy.length) {
      handleLessonChange(newLesson)
    }
  }

  return (
<div className="min-h-screen bg-white">
  <Header title="W-Shica User Study" subtitle="Guided tasks to explore W-Shica features."
  />

  <div className="max-w-7xl mx-auto">
    <div className="px-5 mb-20 lg:px-0">
      <div className="grid grid-cols-1 lg:grid-cols-[200px_1fr]">

        <Sidebar
          lessons={userstudy}
          currentLesson={currentLesson}
          onLessonSelect={handleLessonChange}
        />

        <main className="bg-white p-6 lg:p-10">
          <article>
            <LessonContent lesson={userstudy[currentLesson]} />

            <NavigationButtons
              currentLesson={currentLesson}
              totalLessons={userstudy.length}
              onNavigate={navigate}
            />
          </article>
        </main>

      </div>
    </div>
  </div>
</div>

  )

}
export default UserStudyPage
