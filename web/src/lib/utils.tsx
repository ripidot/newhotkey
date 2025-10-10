import { clsx, type ClassValue } from "clsx"
import { twMerge } from "tailwind-merge"
import { Layout, queryRecordKeys, QueryRecordKey, VisualizationType } from "@/src/types/interface";
import type { LayoutKey } from "@/src/types/interface";

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}

// 型ガード関数
export function isQueryRecordKey(fs: string): fs is QueryRecordKey {
  return (queryRecordKeys as readonly string[]).includes(fs);
}
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
  let h = 0, s = 0
  const l = (max + min) / 2;

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
// 🔹 Hueを時計回りで最短経路補間する関数
// function clockwiseHue(h1: number, h2: number, ratio: number): number {
//   let delta = h2 - h1;
//   return (h1 + delta * ratio + 360) % 360;
// }

// HSL補間 + HEX出力（Hueはラップ補間）
export function mixHslHex(hex1: string, hex2: string, ratio: number): string {
  const [h1, s1, l1] = hexToHsl(hex1);
  const [h2, s2, l2] = hexToHsl(hex2);

  const h = interpolateHue(h1, h2, ratio);
  const s = s1 * (1 - ratio) + s2 * ratio;
  const l = l1 * (1 - ratio) + l2 * ratio;

  const [r, g, b] = hslToRgb(h, s, l);
  return rgbToHex(r, g, b);
}
export function normalizing(min: number, max: number, c: number) {
  const v = Math.min(1, Math.max(0,((c - min) / (max - min))));
  return v;
}
export function logscaling(min: number, max: number, c: number){
  const v = (Math.log(1 + c) - Math.log(1 + min)) / (Math.log(1 + max) - Math.log(1 + min));
  return v;
}
export function ReturnProcessName({
  aggcolumn,
  process_name,
  vtype
}: {
  aggcolumn: string;
  process_name: string;
  vtype: VisualizationType;
}) {
  const aggLabel =
    !aggcolumn
      ? "" // falsyなら空
      : (aggcolumn === "day" || aggcolumn === "week" || aggcolumn === "month")
        ? `集計期間: ${aggcolumn}`
        : `集計データ: ${aggcolumn}`;
  if (vtype === "graph")
    return (
      <p>
        {aggLabel && <>{aggLabel}<br /></>}
        集計プロセス名: {process_name ? process_name : "全て"}
      </p>
    );
  else if (vtype === "counter")
    return (
      <p>
        集計プロセス名: {process_name ? process_name : "全て"}
      </p>
    );
}

export function ReturnProcessNameforCounter({
  aggcolumn,
  process_name,
}: {
  aggcolumn: string;
  process_name: string;
}) {
  const aggLabel =
    !aggcolumn
      ? "" // falsyなら空
      : (aggcolumn === "day" || aggcolumn === "week" || aggcolumn === "month")
        ? `集計期間: ${aggcolumn}`
        : `集計データ: ${aggcolumn}`;

  return (
    <p>
      {aggLabel && <>{aggLabel}<br /></>}
      集計プロセス名: {process_name ? process_name : "全て"}
    </p>
  );
}

export function DrawExcept({ loading, error }: { loading: boolean, error: unknown }){
  if (loading) { // 非同期処理のためのロード中の処理
    return <p>Loading...</p>;
  }
  if (error instanceof Error) {
    return <p>Error: {error.message}</p>;
  }
}
const seriesAddPath = "M10.68 7.9c.44.54 1.07.92 1.79 1.05l-2.76 2.76c-.18.18-.43.29-.71.29s-.53-.11-.71-.3L5 8.41l-3 3V13h13c.55 0 1 .45 1 1s-.45 1-1 1H1c-.55 0-1-.45-1-1V4c0-.55.45-1 1-1s1 .45 1 1v4.59l2.29-2.3C4.47 6.11 4.72 6 5 6s.53.11.71.29L9 9.59l1.68-1.69zM15 3c.55 0 1 .45 1 1s-.45 1-1 1h-1v1c0 .55-.45 1-1 1s-1-.45-1-1V5h-1c-.55 0-1-.45-1-1s.45-1 1-1h1V2c0-.55.45-1 1-1s1 .45 1 1v1h1z";

export const ThinSeriesAdd: React.FC<React.SVGProps<SVGSVGElement>> = (props) => (
  <svg
    viewBox="0 0 16 16"
    width={16}
    height={16}
    fill="var(--color-black)"
    stroke="currentColor"
    strokeWidth={0}
    strokeLinejoin="round"
    strokeLinecap="round"
    {...props}
  >
    <path d={seriesAddPath} />
  </svg>
);

export function RemoveNanoId(uid: string){
  const name = uid.split("-")[0];
  return name;
}
// function isLeaf(layout: Layout): layout is string {
//   return typeof layout === "string";
// }

export function getLayoutByPath(layout: Layout | null, path: LayoutKey[]): string {
  if (!layout) throw new Error("invalid input");
  let current = layout;

  for (const key of path) {
    console.log("current: ", current, ", key: ", key);
    if ((typeof current !== "string"))
      if ((key in current)) {
        current = current[key]; // ここで安全にアクセスできる
     }else throw new Error("pass key to leaf")
  }
  if (typeof current === "string")
    return String(current);
  throw new Error("invalid input");
}