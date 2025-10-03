import React from "react";
import { Trash2 } from "lucide-react";
import { ConfigPanel } from "@/src/components/sidebar/ConfigPanel";
import { ValidFormState } from "@/src/types/interface";
import type { PanelType } from "@/src/types/interface";
interface SidebarProps {
  onAddPanel: (panel: PanelType) => void;
  onCreatePanel: (validformstate: ValidFormState ) => void;
  onUpdateCoords: () => void;
  onCountTreeNodes: () => void;
  onCheckLog: () => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ onAddPanel, onCreatePanel, onUpdateCoords, onCountTreeNodes, onCheckLog}) => {
  return ( // aside : 付随的な内容 Margin(自コンテンツのゆとり), Padding(他要素との間隔), space-y-4(子要素同士の間隔)
    <aside className="sidebar p-4 space-y-4 w-full">
      <ConfigPanel onCreatePanel={onCreatePanel}/>
      <div className="text-2xl text-center">
        <button onClick={() => onAddPanel("KeyboardBarChart")} title="Key Timeline">
          🕒
        </button>
        <button onClick={() => onAddPanel("KeyboardLineChart")} title="User Sessions">
          👥
        </button>
        <button onClick={() => onAddPanel("userSessions_all")} title="User all Sessions">
          👦
        </button>
        <button onClick={() => onAddPanel("KeyboardCounter")} title="Counter">
          ◎
        </button>
        <button onClick={() => onAddPanel("CircleGraph")} title="Circlegraph">
          🔥
        </button>
        <button onClick={() => onAddPanel("KeyboardHeatmap")} title="KeyboardHeatmap">
          🔑
        </button>
      </div>
      <div className="align text-center">
        <button onClick={() => onUpdateCoords()} title="Update Coords">
          ✨
        </button>
        <button onClick={() => onCountTreeNodes()} title="Count Nodes">
          🌲
        </button>
        <button onClick={() => onCheckLog()} title="Check log">
          📃
        </button>
      </div>
      <div className="delete-area mt-8 text-center text-sm opacity-60">
        <Trash2 className="mx-auto" />
        Drop to delete
      </div>
    </aside>
  );
};
