// HexGrid.tsx
import React, {useState, useEffect} from "react";

type HexCell = { x:number, y:number, radius:number, hexmargin:number, id:string };

function hexPoints(cx:number, cy:number, radius:number, hexmargin:number){
  const pts: string[] = [];
  for(let i=0;i<6;i++){
    const a = Math.PI/180 * (60*i - 30); // orientation adjust
    const x = cx + (radius - hexmargin) * Math.cos(a);
    const y = cy + (radius - hexmargin) * Math.sin(a);
    pts.push(`${x},${y}`);
  }
  return pts.join(" ");
}

export default function Hexagon(){
  const [activeHex, setActiveHex] = useState<Set<string>>(new Set());
  const cells: HexCell[] = [];
  const cols = 8, rows = 6;
  const hexmargin = 0; // hex同士のmargin
  const overlapratio = 1; // hex同士のx方向の重なり具合
  const radius = 30; // hexのradius
  const initx = 10 - radius, inity = 10 - radius; // 原点


  const width = radius * Math.sqrt(3) / 2
  for(let row=0; row<rows; row++){
    for(let col=0; col<cols; col++){
      const cx = col * (width * 2 * overlapratio) + (row % 2 ? width : 0) + initx;
      const cy = row * (radius * 3 / 2) + inity;
      cells.push({ x: cx, y: cy, radius: radius, hexmargin: hexmargin, id: `hex-${row}-${col}` });
    }
  }
  useEffect(() => {
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
  }, []);

  return (
    <svg width="100%" height="100%" viewBox="0 0 800 600" preserveAspectRatio="xMidYMid slice" xmlns="http://www.w3.org/2000/svg" className="absolute inset-0 z-10 animate-pulse hex-stroll">
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
