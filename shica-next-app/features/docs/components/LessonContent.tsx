// components/LessonContent.tsx
import string from 'figlet/fonts/babyface-lame'
import { Lesson,ContentSection } from '../types/lesson'
import CodeBlock from './CodeBlock'
import InfoBox from './InfoBox'
import TryIt from './TryIt'

interface LessonContentProps {
  lesson: Lesson
}

function LessonContent({ lesson }: LessonContentProps) {
  return (
    <article className="prose prose-slate max-w-none">
      
      {/* Lesson Title */}
      <h1 className="text-3xl font-semibold text-slate-900 mb-8">
        {lesson.title}
      </h1>

      {lesson.content.map((section: ContentSection, index: number) => (
        <section key={index} className="mb-10">

          {section.type === 'heading' && (
            <h2 className="text-2xl font-semibold text-slate-800 mt-10 mb-4">
              {section.text}
            </h2>
          )}

          {section.type === 'paragraph' && (
                ( section.text != '' &&
                <p
                    className="text-lg text-slate-700 leading-relaxed"
                    dangerouslySetInnerHTML={{ __html: section.text || '' }}
                />
                )
          )}

          {section.type === 'list' && (
            <ul className="list-disc ml-6 space-y-1 text-slate-700">
              {section.items?.map((item: any, i: number) => (
                <li className="text-lg" key={i}>{item}</li>
              ))}
            </ul>
          )}

          {section.type === 'code' && (
            <div className="my-6">
              <CodeBlock
                code={section.code || ''}
                output={section.output}
              />
            </div>
          )}

          {section.type === 'info' && (
            <div className="my-6">
              <InfoBox text={section.text || ''} />
            </div>
          )}

          {section.type === 'tryit' && (
            <div className="my-10">
              <TryIt
                title={section.text || ''}
                description={section.description}
                code={section.code}
              />
            </div>
          )}

        </section>
      ))}
    </article>
  );
}

export default LessonContent