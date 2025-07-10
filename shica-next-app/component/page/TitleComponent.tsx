import React from 'react';

// Title Component for the code page
export const TitleComponent = ({ title, subtitle }: { title: string, subtitle: string }) => {
    return (
      <div className="title-component glow-effect">
        <h1>{title}</h1>
        <p className="subtitle">{subtitle}</p>
      </div>
    );
  };