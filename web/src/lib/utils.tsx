import { clsx, type ClassValue } from "clsx"
import { twMerge } from "tailwind-merge"
import { queryRecordKeys, QueryRecordKey, PanelType, Formstring, FormState, VisualizationType } from "@/src/types/interface";

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
export function mixHslHex(hex1: string, hex2: string, ratio: number): string {
  const [h1, s1, l1] = hexToHsl(hex1);
  const [h2, s2, l2] = hexToHsl(hex2);

  const h = interpolateHue(h1, h2, ratio);
  const s = s1 * (1 - ratio) + s2 * ratio;
  const l = l1 * (1 - ratio) + l2 * ratio;

  const [r, g, b] = hslToRgb(h, s, l);
  return rgbToHex(r, g, b);
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

export function DrawExcept({ loading, error }: { loading: boolean, error: Error | null }){
  if (loading) { // 非同期処理のためのロード中の処理
    return <p>Loading...</p>;
  }
  if (error) {
    return <p>Error: {error.message}</p>;
  }
}

