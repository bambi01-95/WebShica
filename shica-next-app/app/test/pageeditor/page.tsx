"use client";
import React, { useState } from "react";

//preview component
import { TitleComponent } from "@/component/page/TitleComponent";
import { DescriptionComponent } from "@/component/page/DescriptionComponent";
import { CodeBlock } from "@/component/page/CodeBlock";
import { FeatureCard } from "@/component/page/FeatureCard";
import { PageList } from "@/component/page/PageList";
import { ImageTextComponent } from "@/component/page/ImageTextComponent";

//edit component
import { TitleComponentEdit } from "@/component/edit/TitleComponentEdit";
import { DescriptionComponetEdit } from "@/component/edit/DescriptionComponetEdit";
import { CodeBlockEdit } from "@/component/edit/CodeBlockEdit";
import { FeatureCardsEdit } from "@/component/edit/FeatureCardsEdit";
import { PageListEdit } from "@/component/edit/PageListEdit";
import { ImageTextComponentEdit } from "@/component/edit/ImageTextComponentEdit";

import { Button } from "@/component/ui/Button";

const componetType = {
  title: "title",
  description: "descriptionComponent",
  codeBlock: "codeBlock",
  featureCards: "featureCards",
  pageList: "pageList",
  imageText: "imageText",
};

interface featureCardProps {
  id: number; // ← 追加
  title: string;
  description: string;
}

interface PageItem {
  id: string;
  name: string;
  description: string;
  url: string;
  profileImage: string;
}

interface Component {
  id: number;
  type: string;
  title: string;
  description: string;
  subtitle: string;
  code: string;
  pageList: PageItem[];
  featureCard: featureCardProps[];
}

const PageEditor = () => {
  const [components, setComponents] = useState<Component[]>([
    {
      id: 1,
      type: "title",
      title: "",
      subtitle: "",
      description: "",
      code: "",
      pageList: [],
      featureCard: [],
    },
  ]);

  const addComponent = (type: string) => {
    const newComponent: Component = {
      id: components.length + 1,
      type: type,
      title: "",
      description: "",
      subtitle: "",
      code: "",
      pageList: [],
      featureCard: [],
    };
    setComponents([...components, newComponent]);
  };

  const deleteComponent = (id: number) => {
    setComponents(components.filter((component) => component.id !== id));
  };

  const updateComponent = (id: number, props: Partial<Component>) => {
    setComponents((prev) =>
      prev.map((component) =>
        component.id === id ? { ...component, ...props } : component
      )
    );
  };

  const moveComponent = (id: number, direction: "up" | "down") => {
    const index = components.findIndex((component) => component.id === id);
    const newIndex = direction === "up" ? index - 1 : index + 1;
    const newComponents = [...components];
    newComponents[index] = newComponents[newIndex];
    newComponents[newIndex] = components[index];
    setComponents(newComponents);
  };

  const renderComponentEdit = (component: Component) => {
    if (!component) return null;
    switch (component.type) {
      case componetType.title:
        return (
          <TitleComponentEdit
            title={component.title}
            subtitle={component.subtitle}
            onChangeTitle={(text) =>
              updateComponent(component.id, { title: text })
            }
            onChangeSubtitle={(text) =>
              updateComponent(component.id, { subtitle: text })
            }
          />
        );
      case componetType.description:
        return (
          <DescriptionComponetEdit
            title={component.title}
            script={component.description}
            onChangeTitle={(text) =>
              updateComponent(component.id, { title: text })
            }
            onChangeScript={(text) =>
              updateComponent(component.id, { description: text })
            }
          />
        );
      case componetType.codeBlock:
        return (
          <CodeBlockEdit
            title={component.title}
            code={component.code}
            onChangeTitle={(text) =>
              updateComponent(component.id, { title: text })
            }
            onChangeCode={(text) =>
              updateComponent(component.id, { code: text })
            }
          />
        );
      case componetType.featureCards:
        return (
          <div className="flex flex-col gap-4">
            <FeatureCardsEdit
              componentId={component.id}
              featureCards={component.featureCard}
              onChangeFeatureCards={(componentId, featureCards) =>
                updateComponent(componentId, { featureCard: featureCards })
              }
            />
          </div>
        );
      case componetType.pageList:
        return (
          <PageListEdit
            componentId={component.id}
            pages={component.pageList}
            onChangePages={(componentId, pages) =>
              updateComponent(componentId, { pageList: pages })
            }
          />
        );
      case componetType.imageText:
        if (component.subtitle == null) {
          component.subtitle = "left";
        } else if (
          component.subtitle !== "left" &&
          component.subtitle !== "right" &&
          component.subtitle !== "top"
        ) {
          component.subtitle = "left";
        } else {
          component.subtitle = component.subtitle as "left" | "right" | "top";
        }
        return (
          <ImageTextComponentEdit
            imageSrc={component.code}
            text={component.description}
            title={component.title}
            layout={component.subtitle as "left" | "right" | "top"}
            setImageSrc={(imageSrc) =>
              updateComponent(component.id, { code: imageSrc })
            }
            setTitle={(title) =>
              updateComponent(component.id, { title: title })
            }
            setText={(text) =>
              updateComponent(component.id, { description: text })
            }
            setLayout={(layout) =>
              updateComponent(component.id, { subtitle: layout })
            }
          />
        );
      default:
        return null;
    }
  };

  const renderComponent = (component: Component) => {
    if (!component) return null;
    switch (component.type) {
      case componetType.title:
        return (
          <TitleComponent
            title={component.title}
            subtitle={component.subtitle}
          />
        );
      case componetType.description:
        return (
          <DescriptionComponent title={component.title}>
            {component.description}
          </DescriptionComponent>
        );
      case componetType.codeBlock:
        return (
          <CodeBlock filename={component.title} language="typescript">
            {component.code}
          </CodeBlock>
        );
      case componetType.featureCards:
        if (component.featureCard.length === 0) {
          component.featureCard = [{ id: 1, title: "", description: "" }];
        }
        return (
          <div className="flex flex-wrap gap-4 justify-center items-center">
            {component.featureCard.map((featureCard) => (
              <FeatureCard
                key={featureCard.id}
                title={featureCard.title}
                children={featureCard.description}
                className="w-64" // ← カード幅を固定すると折り返しやすい
              />
            ))}
          </div>
        );
      case componetType.pageList:
        return <PageList pages={component.pageList} />;
      case componetType.imageText:
        return (
          <ImageTextComponent
            imageSrc={component.code}
            title={component.title}
            text={component.description}
            layout={component.subtitle as "left" | "right" | "top"}
          />
        );
      default:
        return null;
    }
  };

  //HTML ZONE
  return (
    <div>
      <div className="hidden 2xl:block">
        <div className="w-full h-screen px-4">
          <div className="flex flex-row justify-between items-start w-full h-full">
            {/* edit zone */}
            <div className="flex-1 h-full overflow-y-auto px-4">
              <div className="flex flex-col gap-10 p-10">
                <TitleComponent title={"Editor"} subtitle={"editor"} />
                {components.map((component) => (
                  <div key={component.id}>{renderComponentEdit(component)}</div>
                ))}
              </div>
              <div className="flex flex-row gap-4 justify-center sticky bottom-0 bg-white py-4">
                <Button onClick={() => addComponent(componetType.title)}>
                  Add Title
                </Button>
                <Button onClick={() => addComponent(componetType.description)}>
                  Add Description
                </Button>
                <Button onClick={() => addComponent(componetType.codeBlock)}>
                  Add Code Block
                </Button>
                <Button onClick={() => addComponent(componetType.featureCards)}>
                  Add Feature Card
                </Button>
                <Button onClick={() => addComponent(componetType.pageList)}>
                  Add Page List
                </Button>
                <Button onClick={() => addComponent(componetType.imageText)}>
                  Add Image Text
                </Button>
              </div>
            </div>

            {/* 仕切り線 */}
            <div className="w-px h-full bg-gray-300"></div>

            {/* preview zone */}
            <div className="flex-1 h-full overflow-y-auto px-4 flex flex-col gap-10 p-10">
              <TitleComponent title={"Preview"} subtitle={"Preview"} />
              {components.map((component) => (
                <div key={component.id}>{renderComponent(component)}</div>
              ))}
            </div>
          </div>
        </div>
      </div>
      <div className="2xl:hidden">
        <div className="w-full h-screen px-4 flex flex-col gap-10 p-10">
          <TitleComponent title={"Open window full screen"} subtitle={"Thank you!!!"} />
        </div>
      </div>
    </div>
  );
};

export default PageEditor;
