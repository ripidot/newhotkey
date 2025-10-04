import React, { useRef, useEffect, useState } from "react";
import type { QueryRequest } from "@/src/types/interface";
import type { PanelId, HeatmapProps } from "@/src/types/interface";
import { useKeyboardLayoutLoader } from "@/src/hooks/useKeyboardLayoutLoader";
import { useQueryRecord } from "@/src/hooks/useQueryRecord";
import { logscaling, mixHslHex, ReturnProcessName, DrawExcept } from "@/src/lib/utils";

const fcolor = "C9F4FF";
const tcolor = "FFA9A9";
const top = 50;

export function KeyboardHeatmap({
  id,
  registerUpdateCoords,
  unregisterUpdateCoords,
  process_name
}: {
  id: PanelId;
  registerUpdateCoords: (id: PanelId, fn: () => void) => void;
  unregisterUpdateCoords: (id: PanelId) => void;
  process_name: string;
}) {
  const imgRef = useRef<HTMLImageElement>(null);
  const [imgcoords, setImgCoords] = useState<{ left: number; top: number; w: number; h: number }>();
  const [, setLoaded] = useState(false);
  // const [queryRecord, setqueryRecord] = useState<QueryRecord[]>([]);

  const layout = useKeyboardLayoutLoader("/keyboard_layout.json");

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

  useEffect(() => {
    registerUpdateCoords(id, updateCoords);
    return () => {
      unregisterUpdateCoords(id);
    };
  }, [id, registerUpdateCoords, unregisterUpdateCoords]);

  return (
    <div style={{ position: "relative", width: "100%", height: "100%" }}>
    <ReturnProcessName aggcolumn={"key"} process_name={process_name} vtype={"graph"} />
    <KeyboardImage/>
    {imgcoords && <HeatmapSvg process_name={process_name} imgcoords={imgcoords} keys={layout} />}
    </div>
  );
}

const HeatmapSvg: React.FC<HeatmapProps> = ({process_name, imgcoords, keys}) => {

  const baseRequest: QueryRequest = {
    select: ["key"],
    group_by: ["key"],
    aggregates: [{ func: "count", alias: "count" }],
    order_by: [{ field: "count", direction: "desc" }],
    limit: 50,
  };
  const requestData: QueryRequest = {
  ...baseRequest,
  ...(process_name && { where: { process_name } }),
  }

  const {queryRecord, loading, error} = useQueryRecord<{ key: string; count: number }>(requestData);

  const querymin = Math.min(...queryRecord.map(q => q.count));
  const querymax = Math.max(...queryRecord.map(q => q.count));

  const queryMap = new Map(queryRecord.map((q) => [q.key, q.count]));
  if (loading || error)
    return <DrawExcept loading={loading} error={error}/>

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
              fill = {mixHslHex(fcolor, tcolor, logscaling(querymin, querymax, count))}
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