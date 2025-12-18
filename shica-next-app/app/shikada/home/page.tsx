'use client'


import { useState } from 'react'
import Header from '../../../features/docs/components/Header'
import Sidebar from '../../../features/docs/components/Sidebar'
import LessonContent from '../../../features/docs/components/LessonContent'
import NavigationButtons from '../../../features/docs/components/NavigationButtons'
import { home } from '../../../features/docs/data/home'

export function ShicaHomePage() {
  const [currentLesson, setCurrentLesson] = useState(0)

  const handleLessonChange = (index: number) => {
    setCurrentLesson(index)
    window.scrollTo({ top: 0, behavior: 'smooth' })
  }

  const navigate = (direction: number) => {
    const newLesson = currentLesson + direction
    if (newLesson >= 0 && newLesson < home.length) {
      handleLessonChange(newLesson)
    }
  }

  return (
<div className="min-h-screen bg-white">
  <Header title="Shica Programming Language" subtitle="Introduction to W-Shica programming language."
  />

  <div className="max-w-7xl mx-auto">
    <div className="px-5 mb-20 lg:px-0">
      <div className="grid grid-cols-1">

        <main className="bg-white p-6 lg:p-10">
          <article>
            <LessonContent lesson={home[0]} />
          </article>
        </main>

      </div>
    </div>
  </div>
</div>

  )

}
export default ShicaHomePage