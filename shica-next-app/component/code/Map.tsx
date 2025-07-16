const Map = () => {
  return (
    <div className="w-full h-full bg-gray-900 flex flex-col items-center justify-center border border-gray-700">
      {/* grid map 10x10 */}
      <div className="grid grid-cols-20 text-white">
        {Array.from({ length: 400 }).map((_, index) => (
          <div
            key={index}
            className="w-10 h-10 flex items-center justify-center border border-gray-700 bg-gray-800"
          >
          </div>
          ))}
        </div>
    </div>
  );
};

export default Map;
