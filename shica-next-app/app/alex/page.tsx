"use client";
import React, { useState, useEffect } from "react";

// タイトルコンポーネント
import { TitleComponent } from "@/component/page/TitleComponent";
// 説明文コンポーネント
import { DescriptionComponent } from "@/component/page/DescriptionComponent";
import { CodeBlock } from "@/component/page/CodeBlock";
import { FeatureCard } from "@/component/page/FeatureCard";

// メインページコンポーネント
export default function Home() {
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
        title="Smart White Cane"
        subtitle="DEL VALLE CASTRO Alexandro Edberg"
      />

      <DescriptionComponent title="What is Smart White Cane?">
        <p>
          <strong>Smart White Cane</strong> is a smart white cane that is designed to be a more seafty life for blind people.
        </p>
      </DescriptionComponent>
    </div>
  );
};
