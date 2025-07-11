"use client";
import React from "react";
import { Button } from "../ui/Button";
import { Input } from "../ui/Input";
import { Text } from "../ui/Text";
import EditTitle from "./EditTitle";

interface PageItem {
  id: string;
  name: string;
  description: string;
  url: string;
  profileImage: string;
}

interface PageItemEditProps {
  pageItem: PageItem;
  onChangeName: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onChangeDescription: (e: React.ChangeEvent<HTMLTextAreaElement>) => void;
  onChangeUrl: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onChangeProfileImage: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onRemove: () => void;
}

const PageItemEdit: React.FC<PageItemEditProps> = ({
  pageItem,
  onChangeName,
  onChangeDescription,
  onChangeUrl,
  onChangeProfileImage,
  onRemove,
}) => {
  return (
    <div className="border border-gray-300 shadow-md rounded-lg p-4 space-y-3">
      <div className="flex justify-between items-center">
        <h3 className="text-lg font-semibold">Page Item</h3>
        <Button onClick={onRemove}>Remove</Button>
      </div>

      <div>
        <label className="block text-sm font-medium mb-1">Name</label>
        <Input
          placeholder="Page Name"
          value={pageItem.name}
          onChange={onChangeName}
          className="w-full"
        />
      </div>

      <div>
        <label className="block text-sm font-medium mb-1">Description</label>
        <Text
          placeholder="Page Description"
          className="w-full"
          onChange={onChangeDescription}
        >
          {pageItem.description}
        </Text>
      </div>

      <div>
        <label className="block text-sm font-medium mb-1">URL</label>
        <Input
          placeholder="Page URL"
          value={pageItem.url}
          onChange={onChangeUrl}
          className="w-full"
        />
      </div>

      <div>
        <label className="block text-sm font-medium mb-1">
          Profile Image URL
        </label>
        <Input
          placeholder="Profile Image URL"
          value={pageItem.profileImage}
          onChange={onChangeProfileImage}
          className="w-full"
        />
      </div>
    </div>
  );
};

interface PageListEditProps {
  componentId: number;
  pages: PageItem[];
  onChangePages: (componentId: number, pages: PageItem[]) => void;
}

export const PageListEdit: React.FC<PageListEditProps> = ({
  componentId,
  pages,
  onChangePages,
}) => {
  const handleChangeName = (id: string, newName: string) => {
    const updatedPages = pages.map((page) =>
      page.id === id ? { ...page, name: newName } : page
    );
    onChangePages(componentId, updatedPages);
  };

  const handleChangeDescription = (id: string, newDescription: string) => {
    const updatedPages = pages.map((page) =>
      page.id === id ? { ...page, description: newDescription } : page
    );
    onChangePages(componentId, updatedPages);
  };

  const handleChangeUrl = (id: string, newUrl: string) => {
    const updatedPages = pages.map((page) =>
      page.id === id ? { ...page, url: newUrl } : page
    );
    onChangePages(componentId, updatedPages);
  };

  const handleChangeProfileImage = (id: string, newProfileImage: string) => {
    const updatedPages = pages.map((page) =>
      page.id === id ? { ...page, profileImage: newProfileImage } : page
    );
    onChangePages(componentId, updatedPages);
  };

  const handleAddPage = () => {
    const newPage: PageItem = {
      id: Date.now().toString(),
      name: "",
      description: "",
      url: "",
      profileImage: "",
    };
    onChangePages(componentId, [...pages, newPage]);
  };

  const handleRemovePage = (id: string) => {
    const updatedPages = pages.filter((page) => page.id !== id);
    onChangePages(componentId, updatedPages);
  };

  return (
    <div className="space-y-4 page-list-component">
      <div className="flex justify-center">
        <EditTitle title="Page List Editor" />
      </div>

      <div className="grid grid-cols-1 grid-cols-2  justify-items-center gap-4">
        {pages.map((page) => (
          <div key={page.id} className="w-full">
            <PageItemEdit
              pageItem={page}
              onChangeName={(e) => handleChangeName(page.id, e.target.value)}
              onChangeDescription={(e) =>
                handleChangeDescription(page.id, e.target.value)
              }
              onChangeUrl={(e) => handleChangeUrl(page.id, e.target.value)}
              onChangeProfileImage={(e) =>
                handleChangeProfileImage(page.id, e.target.value)
              }
              onRemove={() => handleRemovePage(page.id)}
            />
          </div>
        ))}
      </div>

      <div className="flex justify-center mt-4">
        <Button onClick={handleAddPage}>Add Page</Button>
      </div>
    </div>
  );
};
