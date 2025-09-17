import { useEffect, useRef, useState } from "react";

export default function HexArea() {
  const containerRef = useRef<HTMLDivElement>(null);
  const [size, setSize] = useState<{width:number, height:number}>({width:0, height:0});

  useEffect(() => {
    if (!containerRef.current) return;
    const observer = new ResizeObserver(entries => {
      for (let entry of entries) {
        setSize({
          width: entry.contentRect.width,
          height: entry.contentRect.height,
        });
      }
    });
    observer.observe(containerRef.current);
    return () => observer.disconnect();
  }, []);

  return (
    <div ref={containerRef} className="relative h-full w-full bg-gray-200">
      <p>Width: {size.width}, Height: {size.height}</p>
    </div>
  );
}
