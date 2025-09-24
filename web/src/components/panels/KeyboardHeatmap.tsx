import React, { useRef, useEffect, useState } from "react";
import type { QueryRequest, QueryRecord} from "@/src/types/interface";
import type { PanelId, HeatmapProps } from "@/src/types/interface";
import { useKeyboardLayoutLoader } from "@/src/hooks/useKeyboardLayoutLoader";
import { mixHslHex } from "@/src/lib/utils";
const fcolor = "C9F4FF";
const tcolor = "FFC9C9";

export function KeyboardHeatmap({
  id,
  registerUpdateCoords,
  unregisterUpdateCoords,
}: {
  id: PanelId;
  registerUpdateCoords: (id: PanelId, fn: () => void) => void;
  unregisterUpdateCoords: (id: PanelId) => void;
}) {
  const imgRef = useRef<HTMLImageElement>(null);
  const [imgcoords, setImgCoords] = useState<{ left: number; top: number; w: number; h: number }>();
  const [, setLoaded] = useState(false);
  const [queryRecord, setqueryRecord] = useState<QueryRecord[]>([]);
  const top = 50;
  const layout = useKeyboardLayoutLoader("/keyboard_layout.json");

  const program_name = "Explorer.EXE";

  const updateCoords = () => {
    if (imgRef.current) {
      const rect = imgRef.current.getBoundingClientRect();
      setImgCoords(prev => {
        if (
          prev &&
          prev.left === rect.left &&
          prev.top === rect.top &&
          prev.w === rect.width &&
          prev.h === rect.height
        ) {
          return prev;
        }
        return { left: rect.left, top: rect.top, w: rect.width, h: rect.height };
      });
    }
  };
  const fetchData = async () => {
    const requestData: QueryRequest = {
      select: ["key"],
      where: { process_name: program_name },
      group_by: ["key"],
      aggregates: [{ func: "count", alias: "count" }],
      order_by: [{ field: "count", direction: "desc" }],
      limit: 50,
    };

    try {
      const response = await fetch("http://localhost:8000/postall", {
        method: "POST",                 // POSTで送信
        headers: {
          "Content-Type": "application/json", // JSONで送信することを明示
        },
        body: JSON.stringify(requestData),    // オブジェクトをJSON文字列に変換
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      setqueryRecord(data.results);
    } catch (error) {
      console.error("送信エラー:", error);
    }
  };

  function KeyboardImage() {
    return (
      <img
        ref={imgRef}
        src="/keyboard.png"
        alt="keyboard"
        onLoad={() => {
          updateCoords(); // 画像のロード後
          setLoaded(true);
        }}
        style={{
          position: "absolute",
          top: top,
          left: 0,
          width: "100%",
          height: "auto",
          objectFit: "contain",
          zIndex: 1,
        }}
      />
    );
  }

  
  const HeatmapSvg: React.FC<HeatmapProps> = ({imgcoords, keys, queryRecord }) => {
    const queryMap = new Map(queryRecord.map((q) => [q.key, q.count]));
    // const querymax = Math.max(...queryRecord.map(q => q.count));
    const max = 100;
    const querymin = 0;
    return (
      <svg
        style={{
          position: "absolute",
          left: 0,
          top: top,
          width: imgcoords.w,
          height: imgcoords.h,
          pointerEvents: "none"
        }}
        viewBox={`0 0 ${1669} ${660}`} // 元画像サイズでスケーリング
      >
        {keys.map((key, i) =>{
          const count = queryMap.get(key.label)?? 0; // 存在すれば数値、なければ undefined
          return (count !== 0 && count !== undefined) ? (
            <g key={i}>
              <rect
                x={key.x}
                y={key.y}
                width={key.w}
                height={key.h}
                fill = {mixHslHex(fcolor, tcolor, Math.min(1, Math.max(0,((count - querymin) / (max - querymin)))))}
                stroke="#333"
              />
            </g>
          ) :
            <g key={i}>
              <rect
                x={key.x}
                y={key.y}
                width={key.w}
                height={key.h}
                fill={"#333"}
                stroke="#333"
              />
            </g>
        })}
      </svg>
    );
  };

  useEffect(() => {
    registerUpdateCoords(id, updateCoords);
    fetchData();
    return () => {
      unregisterUpdateCoords(id);
    };
  }, [id, registerUpdateCoords, unregisterUpdateCoords]);
  
  useEffect(() => {
    console.log(` mounted: ${id}`);
    return () => {
      console.log(` unmounted: ${id}`);
    };
  }, [id]);

  return (
    <div style={{ position: "relative", width: "100%", height: "100%" }}>
    <p>集計プロセス名: {program_name}</p>
    <KeyboardImage/>
    {imgcoords && queryRecord && <HeatmapSvg imgcoords={imgcoords} keys={layout} queryRecord={queryRecord}/>}
    </div>
  );
}
