// components/TryIt.tsx
// need to use ShicaHighlight here!
interface TryItProps {
  title: string
  description?: string
  code?: string
}

function TryIt({ title, description, code }: TryItProps) {
  return (
    <section className="my-10 border border-slate-200 bg-slate-50 px-5 py-4">
      
      <div className="mb-3">
        <span className="text-xs font-semibold uppercase tracking-wide text-slate-500">
          Try it yourself
        </span>
      </div>

      <h4 className="text-base font-medium text-slate-900 mb-2">
        {title}
      </h4>

      {description && (
        <p className="text-sm text-slate-700 mb-4">
          {description}
        </p>
      )}

      {code && (
        <div className="mt-4">
          <pre className="bg-slate-900 text-slate-100 text-sm p-4 overflow-x-auto">
            <code
              className="font-mono"
              dangerouslySetInnerHTML={{ __html: formatCode(code) }}
            />
          </pre>
        </div>
      )}
    </section>
  );
}


function formatCode(code: string): string {
  return code
    .replace(/(表示|もし|でなければ|終わり|関数|返す|または|繰り返す|に対して|を|から|まで|が|なら|は|抜ける|回繰り返す|以上|より小さい)/g, '<span class="text-pink-400">$1</span>')
    .replace(/"([^"]*)"/g, '<span class="text-green-400">"$1"</span>')
    .replace(/\/\/.*/g, '<span class="text-gray-500 italic">$&</span>')
    .replace(/\b(\d+)\b/g, '<span class="text-purple-400">$1</span>')
}

export default TryIt