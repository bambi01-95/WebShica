import React from 'react';

// FeatureCard Component for the code page
export const FeatureCard = ({ title, children, className }: { title: string, children: React.ReactNode, className?: string }) => {
    return (
      <div className={`feature-card ${className} shadow-lg`}>
        <h2><strong>{title}</strong></h2>
        <div>{children}</div>
      </div>
    );
  };