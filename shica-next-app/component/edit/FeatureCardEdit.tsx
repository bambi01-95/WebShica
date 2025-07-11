import React from "react";
import { Input } from "../ui/Input";
import Text from "../ui/Text";

// FeatureCard Component for the code page
export const FeatureCardEdit = ({
  id,
  title,
  script,
  onChangeTitle,
  onChangeScript,
}: {
  id: number;
  title: string;
  script: string;
  onChangeTitle: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onChangeScript: (e: React.ChangeEvent<HTMLTextAreaElement>) => void;
}) => {
  return (
    <div className="shadow-md rounded-lg p-4 space-y-3">
      <div className="flex flex-col gap-4">
        <Input
          placeholder="Feature Card Title"
          className="w-full"
          onChange={(e) => onChangeTitle(e)}
          value={title}
        />
        <Text
          placeholder="Feature Card Description"
          className="w-full"
          onChange={(e) => onChangeScript(e)}
          children={script}
        />
      </div>
    </div>
  );
};
