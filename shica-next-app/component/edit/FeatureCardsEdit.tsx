import { FeatureCardEdit } from "./FeatureCardEdit";
import { Button } from "../ui/Button";
import EditTitle from "./EditTitle";

interface featureCardProps {
  id: number;
  title: string;
  description: string;
}

export const FeatureCardsEdit = ({
  componentId, // ← 追加
  featureCards,
  onChangeFeatureCards,
}: {
  componentId: number;
  featureCards: featureCardProps[];
  onChangeFeatureCards: (
    componentId: number,
    featureCards: featureCardProps[]
  ) => void;
}) => {
  if (featureCards.length === 0) {
    featureCards = [
      {
        id: 1,
        title: "",
        description: "",
      },
    ];
  }

  const handleChangeTitle = (id: number, newTitle: string) => {
    const updatedCards = featureCards.map((card) =>
      card.id === id ? { ...card, title: newTitle } : card
    );
    onChangeFeatureCards(componentId, updatedCards);
  };

  const handleChangeScript = (id: number, newDescription: string) => {
    const updatedCards = featureCards.map((card) =>
      card.id === id ? { ...card, description: newDescription } : card
    );
    onChangeFeatureCards(componentId, updatedCards);
  };

  const handleAddFeatureCard = () => {
    const newCard: featureCardProps = {
      id: Date.now(), // 一意なid生成
      title: "",
      description: "",
    };
    onChangeFeatureCards(componentId, [...featureCards, newCard]);
  };

  return (
    <div className="p-4 rounded-lg shadow-lg feature-cards-component">
      <div className="flex justify-center">
        <EditTitle title="Feature Cards" />
      </div>
      <div className="grid grid-cols-2 gap-4 justify-items-center">
        {featureCards.map((featureCard) => (
          <div key={featureCard.id} className="w-full max-w-xs">
            <FeatureCardEdit
              id={featureCard.id}
              title={featureCard.title}
              script={featureCard.description}
              onChangeTitle={(e) =>
                handleChangeTitle(featureCard.id, e.target.value)
              }
              onChangeScript={(e) =>
                handleChangeScript(featureCard.id, e.target.value)
              }
            />
          </div>
        ))}
      </div>
      <div className="flex justify-center mt-4">
        <Button onClick={handleAddFeatureCard}>Add Feature Card</Button>
      </div>
    </div>
  );
};
