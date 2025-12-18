export function highlightShicaCode(text: string): string {
  const keywords = [
    'stt', 'var', 'func', 'int', 'flo', 'str', 'type',
    'if', 'else', 'return', 'for', 'while', 'break', 'continue'
  ];

  const protectedRanges: Array<{
    start: number;
    end: number;
    content: string;
    type: 'comment' | 'string';
  }> = [];

  // HTML escape
  text = text
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;');

  let match: RegExpExecArray | null;

  // multiline comment
  const multiCommentRegex = /\/\*[\s\S]*?\*\//g;
  while ((match = multiCommentRegex.exec(text)) !== null) {
    protectedRanges.push({
      start: match.index,
      end: match.index + match[0].length,
      content: match[0],
      type: 'comment',
    });
  }

  // single line comment
  const singleCommentRegex = /\/\/.*/g;
  while ((match = singleCommentRegex.exec(text)) !== null) {
    if (!protectedRanges.some(r => match!.index >= r.start && match!.index < r.end)) {
      protectedRanges.push({
        start: match.index,
        end: match.index + match[0].length,
        content: match[0],
        type: 'comment',
      });
    }
  }

  // string
  const stringRegex = /"(?:[^"\\]|\\.)*"/g;
  while ((match = stringRegex.exec(text)) !== null) {
    if (!protectedRanges.some(r => match!.index >= r.start && match!.index < r.end)) {
      protectedRanges.push({
        start: match.index,
        end: match.index + match[0].length,
        content: match[0],
        type: 'string',
      });
    }
  }

  protectedRanges.sort((a, b) => a.start - b.start);

  let result = '';
  let lastEnd = 0;

  const applyNormalHighlight = (segment: string) => {
    keywords.forEach(keyword => {
      segment = segment.replace(
        new RegExp(`\\b${keyword}\\b`, 'g'),
        `<span style="color:#3b82f6;font-weight:bold;">${keyword}</span>`
      );
    });

    segment = segment.replace(
      /\b(\d+(?:\.\d+)?)\b/g,
      '<span style="color:#059669;">$1</span>'
    );

    segment = segment.replace(
      /([(){\}\[\]])/g,
      '<span style="color:#3b82f6;">$1</span>'
    );

    return segment;
  };

  for (const range of protectedRanges) {
    if (lastEnd < range.start) {
      result += applyNormalHighlight(text.substring(lastEnd, range.start));
    }

    const color = range.type === 'comment' ? '#059669' : '#dc2626';
    result += `<span style="color:${color};">${range.content}</span>`;
    lastEnd = range.end;
  }

  if (lastEnd < text.length) {
    result += applyNormalHighlight(text.substring(lastEnd));
  }

  return result;
}
export default highlightShicaCode;