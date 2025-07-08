import React from 'react';

// Description Component for the code page
export const DescriptionComponent = ({ title, children }: { title: string, children: React.ReactNode }) => {
    return (
      <div className="description-component">
        <h2>{title}</h2>
        <div className="description-content">
          {children}
        </div>
      </div>
    );
  };