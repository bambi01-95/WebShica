"use client";
import React, { useState, useEffect } from "react";

// タイトルコンポーネント
import { TitleComponent } from "@/component/page/TitleComponent";

// 説明文コンポーネント
import { DescriptionComponent } from "@/component/page/DescriptionComponent";

import { CodeSpan } from "@/component/page/CodeSpan";
import { CodeBlock } from "@/component/page/CodeBlock";
import { FeatureCard } from "@/component/page/FeatureCard";
import { PageList, PageItem } from "@/component/page/PageList";

// メインページコンポーネント
const ProgrammingLanguagePage = () => {
  useEffect(() => {
    // ページロード時のアニメーション
    const components = document.querySelectorAll(
      ".title-component, .description-component, .code-block-component"
    );

    components.forEach((component, index) => {
      setTimeout(() => {
        (component as HTMLElement).style.opacity = "1";
        (component as HTMLElement).style.transform = "translateY(0)";
      }, index * 200);
    });

    // スクロール時のアニメーション効果
    const handleScroll = () => {
      const cards = document.querySelectorAll(".feature-card");
      const triggerBottom = window.innerHeight * 0.8;

      cards.forEach((card) => {
        const cardTop = card.getBoundingClientRect().top;

        if (cardTop < triggerBottom) {
          (card as HTMLElement).style.opacity = "1";
          (card as HTMLElement).style.transform = "translateY(0)";
        }
      });
    };

    window.addEventListener("scroll", handleScroll);
    return () => window.removeEventListener("scroll", handleScroll);
  }, []);

  return (
    <div className="container">
      <div className="flex flex-col gap-10">
        <TitleComponent
          title="Shica"
          subtitle="State base, Event Driven, Distributed programming language"
        />

        <DescriptionComponent title="What is Shica?">
          <p>
            Shica is a programming language that is based on the state machine
            model. And also, it is inspired by Linden Scripting Language.
          </p>
          <p>
            Shica main purpose is integrating the state base, event driven, and
            distributed programming.
          </p>
        </DescriptionComponent>

        <CodeBlock filename="state_model.stt">
          {`// State Model
stt s1(){ // State 1
  clickEH(x,y){
    setXY(100,100);
    stt s2;
  }
}

stt s2(){ // State 2
  clickEH(x,y){
    setXY(200,200);
    stt s1;
  }
}
`}
        </CodeBlock>

        <CodeBlock filename="shica_example.stt">
          {`// function definition
fn fib(n){
  if(n <= 1){
    return n;
  }
  return fib(n - 1) + fib(n - 2);
}

// variable definition
n = 10;
numbers = [1, 2, 3, 4, 5];

// state model definition & multiple event handlers
stt s1(){
  clickEH(x,y){
    setXY(100,100);
  }
  touchEH(){
    speak("Hello, World!");
  }
}
`}
        </CodeBlock>

        <PageList
          pages={[
            {
              id: "web-shica",
              name: "Web Shica Simulator",
              description:
                "Web Shica Simulator is a web-based simulator for Shica programming language.",
              url: "/shikada/shica",
              profileImage: "🧑‍💻",
            },
            {
              id: "shica-instruction",
              name: "Web Shica Instruction",
              description:
                "Web Shica Instruction is a web-based instruction for Shica programming language on the web.",
              url: "/shikada/instruction",
              profileImage: "📖",
            },
            {
              id: "shica-editor",
              name: "Shica Editor",
              description:
                "Shica Editor is a web-based editor for Shica programming language for local development.",
              url: "/shikada/editor",
              profileImage: "💻",
            },
          ]}
          title="Web Shica"
        ></PageList>

        <DescriptionComponent title="Features">
          <p>Shica has the following features:</p>
          <p>
            • <strong>Intuitive state model</strong>：Using{" "}
            <CodeSpan>stt</CodeSpan> to define states
          </p>
          <p>
            • <strong>Concurrent processing</strong>：
            <CodeSpan>eventEH(arg:arg==0)</CodeSpan> allows multiple same event
            handlers to be defined in the same state
          </p>
          <p>
            • <strong>Distributed processing</strong>：Using{" "}
            <CodeSpan>GroupListenerEH()</CodeSpan> to handle events from
            multiple agents
          </p>
        </DescriptionComponent>
      </div>
    </div>
  );
};

export default ProgrammingLanguagePage;
