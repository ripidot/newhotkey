import React from "react";
import { Trash2 } from "lucide-react";

interface SidebarProps {
  onAddPanel: (panel: "KeyboardHeatmap"|"keyTimeline" | "userSessions" | "Counter" | "CircleGraph") => void;
  onUpdateCoords: () => void;
  onCountTreeNodes: () => void;
  onCheckLog: () => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ onAddPanel, onUpdateCoords, onCountTreeNodes, onCheckLog}) => {
  return ( // aside : 付随的な内容 Margin(自コンテンツのゆとり), Padding(他要素との間隔), space-y-4(子要素同士の間隔)
    <aside className="bg-[#222222ff] text-white p-4 space-y-4 w-full">
      <div className="text-2xl text-center">
        <button onClick={() => onAddPanel("keyTimeline")} title="Key Timeline">
          🕒
        </button>
        <button onClick={() => onAddPanel("userSessions")} title="User Sessions">
          👥
        </button>
        <button onClick={() => onAddPanel("Counter")} title="Counter">
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
