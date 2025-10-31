import { useEffect, useState } from "react";
import type { Position, KeyPosition } from  "@/src/types/interface";

export function useKeyboardLayoutLoader(jsonPath: string) {
  const [layoutMap, setLayout] = useState<KeyPosition[]>([]);


  useEffect(() => {
    const loadLayout = async () => {
      try {
        const response = await fetch(jsonPath);
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        console.log("fetched");
        const data: Record<string, Position> = await response.json();

        // JSONオブジェクトをMapに変換
        const positions: KeyPosition[] = Object.entries(data).map(
          ([label, pos]) => ({
            label,
            x: pos.x,
            y: pos.y,
            w: pos.w,
            h: pos.h,
          })
        );
        setLayout(positions);
      } catch (error) {
        console.error("キーボードレイアウトの読み込みに失敗:", error);
      }
    };

    loadLayout();
  }, [jsonPath]);

  return layoutMap;
}
