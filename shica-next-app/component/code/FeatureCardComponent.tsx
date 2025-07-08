import React from 'react';

// FeatureCard Component for the code page
export const FeatureCardComponent = ({ title, children }: { title: string, children: React.ReactNode }) => {
    return (
      <div className="feature-card">
        <h3>{title}</h3>
        <div>{children}</div>
      </div>
    );
  };