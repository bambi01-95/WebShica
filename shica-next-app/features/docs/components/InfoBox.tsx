// components/InfoBox.tsx
interface InfoBoxProps {
  text: string
}

function InfoBox({ text }: InfoBoxProps) {
  return (
    <div className="border-l-4 border-slate-400 bg-slate-50 px-4 py-3 my-6">
      <p className="text-lg text-slate-700 leading-relaxed">
        <span className="font-medium text-slate-900">Note: </span>{" "}
        {text}
      </p>
    </div>
  );
}

export default InfoBox