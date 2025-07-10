"use client";
import React, { useState, useEffect } from "react";

// タイトルコンポーネント
import { TitleComponent } from "@/component/page/TitleComponent";

// 説明文コンポーネント
import { DescriptionComponent } from "@/component/page/DescriptionComponent";

import { CodeSpan } from "@/component/page/CodeSpan";
import { CodeBlock } from "@/component/page/CodeBlock";
import { FeatureCard } from "@/component/page/FeatureCard";

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

      <div className="feature-grid">
        <FeatureCard title="直感的な構文">
          <p>
            日本語キーワードを使用し、自然言語に近い形でプログラムを記述できます。
            <CodeSpan>思考()</CodeSpan>で思考プロセスを、
            <CodeSpan>表現()</CodeSpan>で出力を表現します。
          </p>
        </FeatureCard>

        <FeatureCard title="型システム">
          <p>
            シンプルな型定義で安全なコードを記述できます。
            <CodeSpan>心</CodeSpan>は文字列型、
            <CodeSpan>数</CodeSpan>は数値型を表します。
          </p>
        </FeatureCard>

        <FeatureCard title="関数型プログラミング">
          <p>
            関数型プログラミングの概念を取り入れ、
            <CodeSpan>純粋関数</CodeSpan>や<CodeSpan>不変性</CodeSpan>
            をサポートします。
          </p>
        </FeatureCard>
      </div>

      <CodeBlock filename="advanced_example.zen">
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
          <CodeSpan>GroupListenerEH()</CodeSpan> to handle events from multiple
          agents
        </p>
      </DescriptionComponent>
    </div>
  );
};

export default ProgrammingLanguagePage;
