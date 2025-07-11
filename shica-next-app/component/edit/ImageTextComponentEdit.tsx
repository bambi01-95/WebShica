import React from "react";
import { Input } from "../ui/Input";
import { Select } from "../ui/Select";
import { Text } from "../ui/Text";
import EditTitle  from "./EditTitle";

interface ImageTextComponentEditProps {
  imageSrc: string;
  text: string;
  title: string;
  layout: "left" | "right" | "top";
  setImageSrc: (imageSrc: string) => void;
  setText: (text: string) => void;
  setTitle: (title: string) => void;
  setLayout: (layout: "left" | "right" | "top") => void;
}

const layoutOptions = [
  { value: "left", label: "Left" },
  { value: "right", label: "Right" },
  { value: "top", label: "Top" },
];

export const ImageTextComponentEdit = ({
  imageSrc,
  text,
  title,
  layout,
  setImageSrc,
  setText,
  setTitle,
  setLayout,
}: ImageTextComponentEditProps) => {
  return (
    <div className="flex flex-col bg-gray-100 p-6 image-text-component rounded-lg gap-6">
      <div className="flex justify-center">
        <EditTitle title="Image & Text Editor" />
      </div>

      <div className="flex flex-col gap-6">

          <div>
            <label className="text-gray-700 font-medium">Layout</label>
            <Select
              className="w-full bg-white mt-1"
              options={layoutOptions}
              value={layout}
              onChange={(e) => setLayout(e as "left" | "right" | "top")}
            />
          </div>

          <div>
            <label className="text-gray-700 font-medium">Image URL</label>
            <Input
              className="w-full bg-white mt-1"
              placeholder="Image URL"
              value={imageSrc}
              onChange={(e) => setImageSrc(e.target.value)}
            />
          </div>

          <div>
            <label className="text-gray-700 font-medium">Title</label>
            <Input
              className="w-full bg-white mt-1"
              placeholder="Title"
              value={title}
              onChange={(e) => setTitle(e.target.value)}
            />
          </div>

          <div>
            <label className="text-gray-700 font-medium">Text</label>
            <Text
              className="w-full bg-white mt-1"
              placeholder="Text"
              children={text}
              onChange={(e) => setText(e.target.value)}
            />
          </div>
        </div>

        {/* 右側にプレビューを置きたい場合はここに追加 */}
      </div>
  );
};
