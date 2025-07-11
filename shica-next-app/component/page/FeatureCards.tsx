import { FeatureCard } from "./FeatureCard";

interface featureCardProps {
  title: string;
  description: string;
  className?: string;
}

export const FeatureCards = ({
  featureCards,
  className,
}: {
  featureCards: featureCardProps[];
  className?: string;
}) => {
  return (
    <div className="flex flex-row gap-10 p-16">
      <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 gap-4">
        {featureCards.map((featureCard) => (
          <FeatureCard title={featureCard.title}>
            {featureCard.description}
          </FeatureCard>
        ))}
      </div>
    </div>
  );
};
