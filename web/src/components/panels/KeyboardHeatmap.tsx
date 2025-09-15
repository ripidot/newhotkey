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

type PanelId = string;
type KeyData = {
  label: string;
  x: number;
  y: number;
  w: number;
  h: number;
  value: number;
};
type QueryData = {
  key: string;
  count: number;
};
type Props = {
  coords: {x: number, y: number, w: number, h: number};
  keys: KeyData[];
  queryData: QueryData[];
  getColor: (value: number, min: number, max: number) => string;
};

const fcolor = "C9F4FF";
const tcolor = "FFC9C9";

// HEX → RGB
function hexToRgb(hex: string): [number, number, number] {
  const clean = hex.replace(/^#/, "");
  const bigint = parseInt(clean, 16);
  const r = (bigint >> 16) & 255;
  const g = (bigint >> 8) & 255;
  const b = bigint & 255;
  return [r, g, b];
}

// RGB → HSL
function rgbToHsl(r: number, g: number, b: number): [number, number, number] {
  r /= 255;
  g /= 255;
  b /= 255;
  const max = Math.max(r, g, b), min = Math.min(r, g, b);
  let h = 0, s = 0, l = (max + min) / 2;

  if (max !== min) {
    const d = max - min;
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
    switch (max) {
      case r: h = (g - b) / d + (g < b ? 6 : 0); break;
      case g: h = (b - r) / d + 2; break;
      case b: h = (r - g) / d + 4; break;
    }
    h *= 60;
  }

  return [h, s * 100, l * 100];
}

// HSL → RGB
function hslToRgb(h: number, s: number, l: number): [number, number, number] {
  s /= 100;
  l /= 100;
  const c = (1 - Math.abs(2 * l - 1)) * s;
  const x = c * (1 - Math.abs(((h / 60) % 2) - 1));
  const m = l - c / 2;
  let r = 0, g = 0, b = 0;

  if (0 <= h && h < 60) [r, g, b] = [c, x, 0];
  else if (60 <= h && h < 120) [r, g, b] = [x, c, 0];
  else if (120 <= h && h < 180) [r, g, b] = [0, c, x];
  else if (180 <= h && h < 240) [r, g, b] = [0, x, c];
  else if (240 <= h && h < 300) [r, g, b] = [x, 0, c];
  else if (300 <= h && h < 360) [r, g, b] = [c, 0, x];

  return [
    Math.round((r + m) * 255),
    Math.round((g + m) * 255),
    Math.round((b + m) * 255),
  ];
}

// RGB → HEX
function rgbToHex(r: number, g: number, b: number): string {
  return "#" + [r, g, b].map(x => x.toString(16).padStart(2, "0")).join("").toUpperCase();
}

// HEX → HSL
function hexToHsl(hex: string): [number, number, number] {
  return rgbToHsl(...hexToRgb(hex));
}

// 🔹 Hueを360°ラップで最短経路補間する関数
function interpolateHue(h1: number, h2: number, ratio: number): number {
  let delta = h2 - h1;
  if (Math.abs(delta) > 180) {
    // 例: 350° と 10° → delta = -340° → +20°に補正
    delta -= Math.sign(delta) * 360;
  }
  return (h1 + delta * ratio + 360) % 360;
}

// HSL補間 + HEX出力（Hueはラップ補間）
function mixHslHex(hex1: string, hex2: string, ratio: number): string {
  const [h1, s1, l1] = hexToHsl(hex1);
  const [h2, s2, l2] = hexToHsl(hex2);

  const h = interpolateHue(h1, h2, ratio);
  const s = s1 * (1 - ratio) + s2 * ratio;
  const l = l1 * (1 - ratio) + l2 * ratio;

  const [r, g, b] = hslToRgb(h, s, l);
  return rgbToHex(r, g, b);
}


const getColor = (value: number, min: number, max: number) => {
  const ratio = (value - min) / (max - min);
  const r = Math.round(255 * ratio);
  const b = Math.round(255 * (1 - ratio));
  return `rgb(${r},0,${b})`;
};

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
  const [coords, setCoords] = useState<{ x: number; y: number; w: number; h: number }>();
  const [, setLoaded] = useState(false);
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

  const HeatmapSvg: React.FC<Props> = ({coords, keys, queryData, getColor }) => {
    // queryData.key を高速に参照できるように Set 化
    const queryMap = new Map(queryData.map((q) => [q.key, q.count]));
    // const querymax = Math.max(...queryData.map(q => q.count));
    const max = 100;
    const querymin = 0;
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
                // fill={getColor(count, querymin, max)}
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

  return (
    <div style={{ position: "relative", width: "100%", height: "100%" }}>
    <KeyboardImage/>
    {coords && queryData && <HeatmapSvg coords={coords} keys={keys} queryData={queryData} getColor={getColor} />}
    </div>
  );
}
