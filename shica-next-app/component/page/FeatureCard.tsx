import React from 'react';

// FeatureCard Component for the code page
export const FeatureCard = ({ title, children }: { title: string, children: React.ReactNode }) => {
    return (
      <div className="feature-card">
        <h2><strong>{title}</strong></h2>
        <div>{children}</div>
      </div>
    );
  };