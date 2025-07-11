import React from "react";
import { Input } from "../ui/Input";
import EditTitle from "./EditTitle";
import Text from "../ui/Text";

// Description Component for the code page
export const DescriptionComponetEdit = ({
  title,
  script,
  onChangeTitle,
  onChangeScript,
}: {
  title: string;
  script: string;
  onChangeTitle: (text: string) => void;
  onChangeScript: (text: string) => void;
}) => {
  return (
    <div className="description-component flex flex-col gap-2 justify-center items-center">
      <EditTitle title="Description" />
      <Input
        placeholder="Title"
        className="w-full"
        onChange={(e) => onChangeTitle(e.target.value)}
        value={title}
      />
      <Text
        placeholder="Description"
        className="w-full"
        onChange={(e) => onChangeScript(e.target.value)}
        children={script}
      />
    </div>
  );
};
