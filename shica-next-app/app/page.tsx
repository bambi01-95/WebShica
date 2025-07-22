import Image from "next/image";
import { PageList } from "@/component/page/PageList";
import { TitleComponent } from "@/component/page/TitleComponent";
import { DescriptionComponent } from "@/component/page/DescriptionComponent";

interface PageItem {
  id: string;
  name: string;
  description: string;
  url: string;
  profileImage: string;
}


const pages: PageItem[] = [
  {
    id: '0',
    name: 'Programming System Laboratory',
    description: 'Supivisor: Ian Piumarta',
    url: '/piumarta',
    profileImage: '🏫'
  },
  {
    id: '1',
    name: 'Smart White Cane',
    description: 'DEL VALLE CASTRO Alexandro Edberg',
    url: '/alex',
    profileImage: '🇲🇽'
  },
  {
    id: '2',
    name: 'Shica Lang.',
    description: 'Hiroto SHIKADA',
    url: '/shikada',
    profileImage: '🇯🇵'
  },
  {
    id: '3',
    name: 'Im-C/Sub-C',
    description: 'Kunii MASAKI',
    url: '/kunii',
    profileImage: '🇯🇵'
  },
  {
    id: '4',
    name: 'Meta-programming',
    description: 'SOLTAN Mahmoud Talaat Abdelaziz Mohamed',
    url: '/soltan',
    profileImage: '🇪🇬'
  },
];

export default function Home() {
  return (
    <div className="container mx-auto flex flex-col gap-10">
      <TitleComponent title="Programming System Laboratory" subtitle="Supivisor: Ian Piumarta" />
      <DescriptionComponent title="Programming System Laboratory" >
        <strong>Programming System Laboratory</strong> is working on Electorical WorkShop(EWS), 4th floor north of building.
        </DescriptionComponent>
      <PageList pages={pages} />
      <DescriptionComponent title="Access" >
        <h1>Location: 4th floor north of building.</h1>
        <h1>Time: 10:00 - 17:00</h1>
        <h1>Contact: ian.piumarta@kuas.ac.jp</h1>
      </DescriptionComponent>
    </div>
  );
}
