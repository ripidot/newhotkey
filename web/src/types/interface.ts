import { MosaicNode } from "react-mosaic-component";
import { MotionValue } from "framer-motion";

// interface: 継承可能
// type: 継承不可


// 集約関数の型
export interface Aggregate {
  func: string;  // "count" など
  alias: string;
}
// order_by の型
export interface OrderBy {
  field: string;
  direction: "asc" | "desc";
}

// 全体のリクエスト型
export interface QueryRequest {
  select: string[];
  where?: Record<string, string>;  // 今回は process_name: string の形
  group_by?: string[];
  aggregates?: Aggregate[];
  order_by?: OrderBy[];
  limit?: number;
}
export interface QueryRecord {
  [key: string]: string | number;
  count: number;
}

export type VisualizationType = "graph" | "counter";
export type QueryRecordKey = "key" | "process_name" | "day" | "week" | "month" | "count";
export type QueryResult<T extends QueryRecordKey> = {
  [K in T]: string;
} & { count: number };

export type PanelId = string;
export type PanelType =
  | "keyTimeline"
  | "userSessions"
  | "userSessions_all"
  | "Counter"
  | "KeyboardHeatmap"
  | "CircleGraph";


export interface Position {
  x: number;
  y: number;
  w: number;
  h: number;
}
export interface KeyPosition extends Position {
  label: string;
}

export type HeatmapProps = {
  process_name: string;
  imgcoords: {left: number, top: number, w: number, h: number};
  keys: KeyPosition[];
};

export type MosaicAreaProps = {
  panelMap: Record<PanelId, {render: () => JSX.Element;}>;
  mosaicLayout: MosaicNode<PanelId> | null;
  setMosaicLayout: (layout: MosaicNode<PanelId> | null) => void;
};

export interface CircleAnimation {
  cx: number;
  cy: number;
  radius: number;
  width: number;
  ratio: number;
  circumference: number;
  duration: number;
}
export interface CircleText { 
  cx: number;
  cy: number;
  height: number;
  size: number;
  displayValue: MotionValue<string>;
}