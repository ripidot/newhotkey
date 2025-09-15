// components/Hexagon.tsx
import React from "react";

type Props = {
  size?: number;
  fill?: string;
  stroke?: string;
  className?: string;
};

export default function Hexagon({
  size = 500,
  fill = "none",
  stroke = "black",
  className = "absolute inset-0 z-10 pointer-events-none",
}: Props) {
  const h = Math.sqrt(3) / 2 * size;
  return (
    // <div className="relative">
      <svg
        width={size}
        height={size}
        viewBox={`0 0 ${size} ${size}`}
        className="absolute inset-0 z-10 pointer-events-none"
        xmlns="http://www.w3.org/2000/svg"
      >
        <polygon
          points={`
            ${size / 2},0
            ${size},${h / 2}
            ${size},${h * 1.5}
            ${size / 2},${h * 2}
            0,${h * 1.5}
            0,${h / 2}
          `}
          fill={fill}
          stroke={stroke}
        />
      </svg>
    // </div>
  );
}
