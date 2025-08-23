import React, { useRef, useEffect, useState } from "react";

// キー配置データ（例: 一部だけ、実際は60キーぶん作る）
const keys = [
  { label: "Q", x: 0, y: 0, w: 50, h: 50, value: 10 },
  { label: "W", x: 60, y: 0, w: 50, h: 50, value: 5 },
  { label: "E", x: 120, y: 0, w: 50, h: 50, value: 20 },
  { label: "Space", x: 0, y: 60, w: 170, h: 50, value: 30 },
];

// 値を色に変換
const getColor = (value: number, min: number, max: number) => {
  const ratio = (value - min) / (max - min);
  const r = Math.round(255 * ratio);
  const b = Math.round(255 * (1 - ratio));
  return `rgb(${r},0,${b})`; // 青→赤
};

export function KeyboardHeatmap() {
  const values = keys.map((k) => k.value);
  const min = Math.min(...values);
  const max = Math.max(...values);
  
  const imgRef = useRef<HTMLImageElement>(null);
  const [coords, setCoords] = useState<{ x: number; y: number; w: number; h: number }>();
  const [loaded, setLoaded] = useState(false); 

  const updateCoords = () => {
    if (imgRef.current) {
      const rect = imgRef.current.getBoundingClientRect();
      setCoords({ x: rect.left, y: rect.top, w: rect.width, h: rect.height });
      console.log(coords);
    }
  };
  useEffect(() => {
    window.addEventListener("resize", updateCoords);
    return () => window.removeEventListener("resize", updateCoords);
  }, []);
  
  function KeyboardImage() {
    return (
      <img
        ref={imgRef}
        src="/keyboard.png"
        alt="keyboard"
        onLoad={() => {
          updateCoords();
          setLoaded(true);
        }}
        style={{
          position: "absolute",
          top: 0,
          left: 0,
          width: "100%",
          height: "auto",
          objectFit: "contain",
          zIndex: 0,
        }}
      />
    );
  }
  function FillColor({ coords }: { coords: { x: number; y: number; w: number; h: number } }){
    if (!loaded) return null;
    if (!coords) return null;

    const scaleX = coords.w / 1669; // window's width / image's width  
    const scaleY = coords.h / 660;

    return (
      <svg
        style={{
          position: "absolute",
          left: coords.x,
          top: coords.y,
          width: coords.w,
          height: coords.h,
          pointerEvents: "none" // クリックは画像に通す
        }}
        viewBox={`0 0 ${1669} ${660}`} // 元画像サイズでスケーリング
      >
        {keys.map((key, i) => (
          <g key={i}>
            <rect
              x={key.x * scaleX}
              y={key.y * scaleY}
              width={key.w * scaleX}
              height={key.h * scaleY}
              fill={getColor(key.value, min, max)}
              stroke="#333"
            />
            <text
              x={key.x + key.w / 2}
              y={key.y + key.h / 2}
              textAnchor="middle"
              dominantBaseline="central"
              fontSize="14"
              fill="#fff"
            >
              {key.label}
            </text>
            <pre style={{ position: "absolute", top: 500, left: 500, color: "red" }}>
              {JSON.stringify("aa")}
            </pre>
          </g>
        ))}
      </svg>
    );
  }
  return (
    <div style={{ position: "relative", width: "100%", height: "100%" }}>
    <KeyboardImage/>
    {coords && <FillColor coords={coords} />}
    </div>
  );
}
