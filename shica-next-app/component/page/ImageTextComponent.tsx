import React from "react";
import Image from "next/image";

interface ImageTextProps {
  imageSrc: string;
  title: string;
  text: string;
  layout: "left" | "right" | "top";
}

export const ImageTextComponent: React.FC<ImageTextProps> = ({
  imageSrc,
  title,
  text,
  layout,
}) => {
  const imageElement =
    imageSrc === "" ? (
      <div className="w-full">
        <div className="bg-gray-200 rounded-lg h-100 flex items-center justify-center ">
          <span className="text-gray-500">No Image</span>
        </div>
      </div>
    ) : (
      <Image src={imageSrc} alt="Image" className="h-100 w-full rounded-lg" />
    );

  const textElement = (
    <div className="flex flex-col gap-2">
      <p className="text-gray-700 text-base font-bold">{title}</p>
      <p className="text-gray-700 text-base">{text}</p>
    </div>
  );

  if (layout === "left") {
    return (
      <div className="flex flex-col bg-white rounded-lg shadow-md image-text-component md:flex-row items-center gap-4">
        <div className="w-full">{imageElement}</div>
        <div className="w-full">{textElement}</div>
      </div>
    );
  }

  if (layout === "right") {
    return (
      <div className="flex flex-col bg-white rounded-lg shadow-md image-text-component md:flex-row items-center gap-4">
        <div className="w-full">{textElement}</div>
        <div className="w-full">{imageElement}</div>
      </div>
    );
  }

  if (layout === "top") {
    return (
      <div className="flex flex-col bg-white rounded-lg shadow-md image-text-component items-center gap-4">
        <div className="w-full">{imageElement}</div>
        <div className="w-full p-4">
          <p className="text-gray-700 whitespace-pre">{title}</p>
        </div>
      </div>
    );
  }

  return null;
};
