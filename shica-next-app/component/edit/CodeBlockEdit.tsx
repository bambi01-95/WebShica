import React from "react";
import { Input } from "../ui/Input";
import EditTitle from "./EditTitle";
import CodeInput from "./CodeInput";

// Title Component for the code page
export const CodeBlockEdit = ({
  title,
  code,
  onChangeTitle,
  onChangeCode,
}: {
  title: string;
  code: string;
  onChangeTitle: (text: string) => void;
  onChangeCode: (text: string) => void;
}) => {
  return (
    <div className="code-block-component flex flex-col gap-2 justify-center items-center">
      {/* <div className="" */}
      <div className="pt-10">
        <EditTitle title="Code Block" />
      </div>
      <Input
        placeholder="filename.c"
        className="text-white w-full"
        onChange={(e) => onChangeTitle(e.target.value)}
        value={title}
      />
      <CodeInput
        initialCode={code}
        onCodeChange={(text) => onChangeCode(text.target.value)}
      />
    </div>
  );
};
