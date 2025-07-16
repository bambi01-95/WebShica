'use client';
import { useEffect } from "react";

export interface Robot{
  x: number;
  y: number;
  vx: number;
  vy: number;
}

const Map = ({ robots }: { robots: Robot[] }) => {
  useEffect(() => {
    console.log(robots);
  }, [robots]);
  return (
    <div className="w-full h-full bg-gray-900 flex flex-col items-center justify-center border border-gray-700">
      {/* grid map 10x10 */}
      <div className="h-[500px] w-[500px] bg-gray-800">
        <div id="map">
          {robots.map((robot, index) => (
            <div key={index} className="robot-vacuum" id={`robot${index}`} style={{ left: robot.x, top: robot.y}} />
          ))}
        </div>
      </div>
    </div>
  );
};

export default Map;