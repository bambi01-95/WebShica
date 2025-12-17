// types/lesson.ts
export interface Lesson {
  id: number
  title: string
  content: ContentSection[]
}

export interface ContentSection {
  type: 'paragraph' | 'heading' | 'list' | 'code' | 'info' | 'tryit'
  text?: string
  items?: string[]
  code?: string
  filename?: string
  output?: string
  description?: string
}