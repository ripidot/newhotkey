import React, { useRef, useEffect, useState } from "react";

const functional_w = 90;
const functional_h = 63;
const functional_init_x = 140;
const functional_x = 97.5;
const functional_init_y = 37;
const functional_label = ["VK_F1","VK_F2","VK_F3","VK_F4","VK_F5","VK_F6","VK_F7","VK_F8","VK_F9","VK_F10","VK_F11","VK_F12"];
const functionalkey = Array.from({ length: 12 }, (_, i) => ({
  label: functional_label[i], x: functional_init_x + functional_x * (i), y: functional_init_y, 
  w: functional_w, h: functional_h, value: i * 5
}));

const numeric_w = 100;
const numeric_h = 99;
const numeric_init_x = 149;
const numeric_x = 106;
const numeric_init_y = 105;
const numeric_label = ["VK_NUMPAD1", "VK_NUMPAD2","VK_NUMPAD3","VK_NUMPAD4","VK_NUMPAD5","VK_NUMPAD6","VK_NUMPAD7","VK_NUMPAD8","VK_NUMPAD9","VK_NUMPAD0","VK_MINUS","VK_CARET","VK_YEN"];
const numerickey = Array.from({ length: 14 }, (_, i) => ({
  label: numeric_label[i], x: numeric_init_x + numeric_x * (i), y: numeric_init_y, 
  w: numeric_w, h: numeric_h, value: i * 5
}));

const qwerty_w = 101;
const qwerty_h = 97;
const qwerty_init_x = 222;
const qwerty_x = 106;
const qwerty_init_y = 212;
const qwerty_label = ["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P","VK_AT", "VK_LBRAKET"];
const qwertykey = Array.from({ length: 12 }, (_, i) => ({
  label: qwerty_label[i], x: qwerty_init_x + qwerty_x * (i), y: qwerty_init_y, 
  w: qwerty_w, h: qwerty_h, value: i * 5
}));

const asdfgh_w = 100;
const asdfgh_h = 98;
const asdfgh_init_x = 244;
const asdfgh_x = 106;
const asdfgh_init_y = 315;
const asdfgh_label = ["A", "S", "D", "F", "G", "H", "J", "K", "L","VK_SEMICOLON","VK_COLON","VK_RBRAKET"];
const asdfghkey = Array.from({ length: 12 }, (_, i) => ({
  label: asdfgh_label[i], x: asdfgh_init_x + asdfgh_x * (i), y: asdfgh_init_y, 
  w: asdfgh_w, h: asdfgh_h, value: i * 5
}));

const zxcvbn_w = 99;
const zxcvbn_h = 97;
const zxcvbn_init_x = 272;
const zxcvbn_x = 106;
const zxcvbn_init_y = 420;
const zxcvbn_label = ["Z", "X", "C", "V", "B", "N", "M", "VK_COMMA", "VK_PERIOD", "VK_SRASH", "VK_BACKSRASH"];
const zxcvbnkey = Array.from({ length: 11 }, (_, i) => ({
  label: zxcvbn_label[i], x: zxcvbn_init_x + zxcvbn_x * (i), y: zxcvbn_init_y, 
  w: zxcvbn_w, h: zxcvbn_h, value: i * 5
}));

const modifier_w = 100;
const modifier_h = 98;
const modifier_init_x = 40;
const modifier_x = 106;
const modifier_init_y = 525;
const modifier_label = ["VK_FN", "VK_LCONTROL", "VK_LWIN", "VK_LALT"];
const modifierkey = Array.from({ length: 4 }, (_, i) => ({
  label: modifier_label[i], x: modifier_init_x + modifier_x * (i), y: modifier_init_y, 
  w: modifier_w, h: modifier_h, value: i * 5
}));

const modifier2_w = [98, 367, 95, 98, 93, 98];
const modifier2_h = 98;
const modifier2_init_x = 461;
const modifier2_x = [0, 101, 470, 568, 669, 767];
const modifier2_init_y = 525;
const modifier2_label = ["VK_NONCONVERT", "VK_SPACE", "VK_CONVERT", "VK_HIRAGANA", "VK_RALT", "VK_RCONTROL"];
const modifier2key = Array.from({ length: 6 }, (_, i) => ({
  label: modifier2_label[i], x: modifier2_init_x + modifier2_x[i], y: modifier2_init_y, 
  w: modifier2_w[i], h: modifier2_h, value: i * 5
}));

const leftmodifier_w = 102;
const leftmodifier_h = 98;
const leftmodifier_init_x = 40;
const leftmodifier_x = 106;
const leftmodifier_y = 104;
const leftmodifier_init_y = 0;
const leftmodifier_label = ["VK_ESCAPE", "VK_ZENHANKAKU", "VK_TAB", "VK_CAPITAL", "VK_LSHIFT"];
const leftmodifierkey = Array.from({ length: 5 }, (_, i) => ({
  label: leftmodifier_label[i], x: leftmodifier_init_x, y: leftmodifier_init_y + leftmodifier_y * (i), 
  w: leftmodifier_w + leftmodifier_x * (i), h: leftmodifier_h, value: i * 5
}));

const edit_w = 110;
const edit_h = 63;
const edit_init_x = 1310;
const edit_x = 108;
const edit_init_y = 37;
const edit_label = ["VK_PAUSE", "VK_INSERT", "VK_DELETE"];
const editkey = Array.from({ length: 3 }, (_, i) => ({
  label: edit_label[i], x: edit_init_x + edit_x * (i), y: edit_init_y, 
  w: edit_w, h: edit_h, value: i * 5
}));

const arrow_w = [99,90,97,90];
const arrow_h = 97;
const arrow_init_x = 1332;
const arrow_x = [0,106,197,106];
const arrow_init_y = 525;
const arrow_y = [0,0,0,-105];
const arrow_label = ["VK_LEFT","VK_DOWN", "VK_RIGHT", "VK_UP"];
const arrowkey = Array.from({ length: 4 }, (_, i) => ({
  label: arrow_label[i], x: arrow_init_x + arrow_x[i], y: arrow_init_y + arrow_y[i], 
  w: arrow_w[i], h: arrow_h, value: i * 5
}));

const rightmodifier_w = 202;
const rightmodifier_h = [210,100];
const rightmodifier_init_x = 1492;
const rightmodifier_x = 106;
const rightmodifier_y = [0,213];
const rightmodifier_init_y = 205;
const rightmodifier_label = ["VK_ENTER","VK_RSHIFT"];
const rightmodifierkey = Array.from({ length: 2 }, (_, i) => ({
  label: rightmodifier_label[i], x: rightmodifier_init_x, y: rightmodifier_init_y + rightmodifier_y[i], 
  w: rightmodifier_w + rightmodifier_x * (i), h: rightmodifier_h[i], value: i * 5
}));

const keys = [...leftmodifierkey, ...functionalkey, ...numerickey, ...rightmodifierkey, ...qwertykey, ...asdfghkey, ...zxcvbnkey, ...modifierkey, ...modifier2key, ...editkey, ...arrowkey];

interface Aggregate {
  func: string;
  alias: string;
}
interface OrderBy {
  field: string;
  direction: "asc" | "desc";
}
interface QueryRequest {
  select: string[];
  where?: Record<string, string>;
  group_by?: string[];
  aggregates?: Aggregate[];
  order_by?: OrderBy[];
  limit?: number;
}
interface QueryRecord {
  key: string;
  count: number;
}

const getColor = (value: number, min: number, max: number) => {
  const ratio = (value - min) / (max - min);
  const r = Math.round(255 * ratio);
  const b = Math.round(255 * (1 - ratio));
  return `rgb(${r},0,${b})`;
};
type PanelId = string;
export function KeyboardHeatmap({
  id,
  registerUpdateCoords,
  unregisterUpdateCoords,
}: {
  id: PanelId;
  registerUpdateCoords: (id: PanelId, fn: () => void) => void;
  unregisterUpdateCoords: (id: PanelId) => void;
}) {
  const values = keys.map((k) => k.value);
  const min = Math.min(...values);
  const max = Math.max(...values);
  
  const imgRef = useRef<HTMLImageElement>(null);
  const [coords, setCoords] = useState<{ x: number; y: number; w: number; h: number }>();
  const [loaded, setLoaded] = useState(false);
  const [queryData, setQueryData] = useState<QueryRecord[]>([]);

  const updateCoords = () => {
    if (imgRef.current) {
      const rect = imgRef.current.getBoundingClientRect();
      setCoords(prev => {
        if (
          prev &&
          prev.x === rect.left &&
          prev.y === rect.top &&
          prev.w === rect.width &&
          prev.h === rect.height
        ) {
          return prev;
        }
        return { x: rect.left, y: rect.top, w: rect.width, h: rect.height };
      });
    }
  };
  const fetchData = async () => {
    const requestData: QueryRequest = {
      select: ["key"],
      where: { process_name: "Explorer.EXE" },
      group_by: ["key"],
      aggregates: [{ func: "count", alias: "count" }],
      order_by: [{ field: "count", direction: "desc" }],
      limit: 5,
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
      setQueryData(data.results);
    } catch (error) {
      console.error("送信エラー:", error);
    }
  };

  useEffect(() => {
    registerUpdateCoords(id, updateCoords);
    fetchData();
    return () => {
      unregisterUpdateCoords(id);
    };
  }, [id, registerUpdateCoords, unregisterUpdateCoords]);

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
          top: 0,
          left: 0,
          width: "100%",
          height: "auto",
          objectFit: "contain",
          zIndex: 1,
        }}
      />
    );
  }
  function FillColor({coords, queryData}: { 
    coords: { x: number; y: number; w: number; h: number },
    queryData: QueryRecord[] }){
    if (!loaded) return null;
    if (!coords) return null;

    return (
      <svg
        style={{
          position: "absolute",
          left: 0,
          top: 0,
          width: coords.w,
          height: coords.h,
          pointerEvents: "none" // クリックは画像に通す
        }}
        viewBox={`0 0 ${1669} ${660}`} // 元画像サイズでスケーリング
      >
        {keys.map((key, i) => (
          <g key={i}>
            <rect
              x={key.x}
              y={key.y}
              width={key.w}
              height={key.h}
              fill={getColor(key.value, min, max)}
              stroke="#333"
            />
          </g>
        ))}
      </svg>
    );
  }
  return (
    <div style={{ position: "relative", width: "100%", height: "100%" }}>
    <KeyboardImage/>
    {coords && queryData && <FillColor coords={coords} queryData={queryData} />}
    </div>
  );
}
