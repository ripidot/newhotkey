import React, {useState, useEffect} from "react";

type HexCell = { x:number, y:number, radius:number, hexmargin:number, id:string };

function hexPoints(cx:number, cy:number, radius:number, hexmargin:number){
  const pts: string[] = [];
  for(let i=0;i<6;i++){
    const a = Math.PI/180 * (60 * i - 30); // orientation adjust
    const x = cx + (radius - hexmargin) * Math.cos(a);
    const y = cy + (radius - hexmargin) * Math.sin(a);
    pts.push(`${x},${y}`);
  }
  return pts.join(" ");
}
interface HexagonProps {
  size: {width:number, height:number};
}
  const Hexagon: React.FC<HexagonProps> = ({size}) => {
  const [activeHex, setActiveHex] = useState<Set<string>>(new Set());
  const [rand] = useState(() => ({x:Math.random(), y:Math.random()}));
  let cells: HexCell[] = [];
  
  const overlapratio = 1; // hex同士のx方向の重なり具合
  const radius =  60; // hexのradius
  const hexmargin = radius / 20; // hex同士のmargin

  const initx = - (radius / 2) - Math.ceil(rand.x * (radius) / 2);
  const inity = - (radius / 4) - Math.ceil(rand.y * (radius) / 2); // 原点

  const width = radius * Math.sqrt(3) / 2; // hexのwidth
  const height = radius; // hexのheight
  const cols = Math.ceil(size.width / (width * 2) + 1),
  rows = Math.ceil(size.height / (height * 0.75 * 2) + 1);

  for(let row=0; row<rows; row++){
    for(let col=0; col<cols; col++){
      const cx = col * (width * 2 * overlapratio) + (row % 2 ? width : 0) + initx;
      const cy = row * (radius * 3 / 2) + inity;
      cells.push({ x: cx, y: cy, radius: radius, hexmargin: hexmargin, id: `hex-${row}-${col}` });
    }
  }

  useEffect(() => {
    // console.log("im in useEffect");
    // console.log("cols, rows: ", cols, ",", rows)
    // console.log("size.h, size.w : ", size.height, size.width);
    cells = [];
    for(let row=0; row<rows; row++){
      for(let col=0; col<cols; col++){
        const cx = col * (width * 2 * overlapratio) + (row % 2 ? width : 0) + initx;
        const cy = row * (radius * 3 / 2) + inity;
        cells.push({ x: cx, y: cy, radius: radius, hexmargin: hexmargin, id: `hex-${row}-${col}` });
      }
    }
    const interval = setInterval(() => {
      setActiveHex(prev => {
        const newSet = new Set(prev);
        newSet.clear();
        // ランダムに3つ選ぶ
        const candidates = cells.map(c => c.id);
        for (let i = 0; i < 7; i++) {
          const rand = Math.floor(Math.random() * candidates.length);
          newSet.add(candidates[rand]);
        }
        return newSet;
      });
    }, 2000); // 2秒ごとに切り替え

    return () => clearInterval(interval);
  }, [size]);

  return (
  <svg xmlns="http://www.w3.org/2000/svg" className="absolute w-full h-full inset-0 z-1 animate-pulse hex-stroll pointer-events-none">
      {cells.map((c) => {
        return (
          <polygon
            key={c.id}
            points={hexPoints(c.x, c.y, c.radius, c.hexmargin)}
            className={`hex ${activeHex.has(c.id) ? "glow" : ""}`}
          />
        );
      })}
    </svg>
  );
}
export default Hexagon;