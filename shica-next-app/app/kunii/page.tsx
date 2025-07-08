'use client';
import React, { useState, useEffect } from 'react';

// タイトルコンポーネント
import { TitleComponent } from '@/component/code/TitleComponent';

// 説明文コンポーネント
import { DescriptionComponent } from '@/component/code/DescriptionComponent';

import { CodeSpan } from '@/component/code/CodeSpan';
import { CodeBlock } from '@/component/code/CodeBlock';
import { FeatureCard } from '@/component/code/FeatureCard';



// メインページコンポーネント
const ProgrammingLanguagePage = () => {
  useEffect(() => {
    // ページロード時のアニメーション
    const components = document.querySelectorAll('.title-component, .description-component, .code-block-component');
    
    components.forEach((component, index) => {
      setTimeout(() => {
        (component as HTMLElement).style.opacity = '1';
        (component as HTMLElement).style.transform = 'translateY(0)';
      }, index * 200);
    });

    // スクロール時のアニメーション効果
    const handleScroll = () => {
      const cards = document.querySelectorAll('.feature-card');
      const triggerBottom = window.innerHeight * 0.8;
      
      cards.forEach(card => {
        const cardTop = card.getBoundingClientRect().top;
        
        if (cardTop < triggerBottom) {
          (card as HTMLElement).style.opacity = '1';
          (card as HTMLElement).style.transform = 'translateY(0)';
        }
      });
    };

    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  return (
    <div className="container">
      <TitleComponent 
        title="Sub-C"
        subtitle="Subset of C programming language"
      />

      <DescriptionComponent title="What is Sub-C?">
        <p>
          <strong>Sub-C</strong> is a subset of C programming language.
          It is designed to be a more seafty programming.
        </p>
        <p>
            Sub-C is interprited by <strong>Sub-C interpreter</strong>.
            It is a simple interpreter that is written in C and Peg/Leg.
        </p>
      </DescriptionComponent>

      <CodeBlock filename="state_model.stt">
{`// It's grammar is same as C programming language.
#include <stdio.h>
int main(){
    printf("Hello, World!\n");
    return 0;
}
`}
      </CodeBlock>

      <div className="feature-grid">
        <FeatureCard title="Subset of C programming language">
          <p>
            You can use 'Sub-C' in unit test without any change.
          </p>
        </FeatureCard>
        
        <FeatureCard title="More seafty">
          <p>
            Sub-C is designed to be a more seafty programming.
            It detect many errors at runtime.
          </p>
        </FeatureCard>
        
        <FeatureCard title="Interpriter written in C and Peg/Leg">
          <p>
            Sub-C is interprited by <strong>Sub-C interpreter</strong>.
          </p>
        </FeatureCard>
      </div>

      <CodeBlock filename="advanced_example.zen">
{`
int array[5] = {1, 2, 3, 4, 5};
int main(){
    for(int i = 0; i <= 5; i++){
        printf("%d\n", array[i]); // array[5] is out of bounds
    }
    return 0;
}
`}
      </CodeBlock>

      <DescriptionComponent title="Features">
        <p>Sub-C has the following features:</p>
        <p>• <strong>Subset of C programming language</strong>：You can use 'Sub-C' in unit test without any change</p>
        <p>• <strong>More seafty</strong>：It detect many errors at runtime</p>
        <p>• <strong>Interpriter written in C and Peg/Leg</strong>：It is a simple interpreter that is written in C and Peg/Leg</p>
      </DescriptionComponent>

    </div>
  );
};

export default ProgrammingLanguagePage;
