import React from "react";
import { Input } from "../ui/Input";
import EditTitle from "./EditTitle";

// Title Component for the code page
export const TitleComponentEdit = ({
  title,
  subtitle,
  onChangeTitle,
  onChangeSubtitle,
}: {
  title: string;
  subtitle: string;
  onChangeTitle: (text: string) => void;
  onChangeSubtitle: (text: string) => void;
}) => {

  return (
    <div className="title-component flex flex-col gap-2 justify-center items-center">
      <EditTitle title="Title" />
      <Input
        placeholder="Title"
        className="w-full"
        onChange={(e) => onChangeTitle(e.target.value)}
        value={title}
      />
      <Input
        placeholder="Subtitle"
        className="w-full"
        onChange={(e) => onChangeSubtitle(e.target.value)}
        value={subtitle}
      />
    </div>
  );
};  
