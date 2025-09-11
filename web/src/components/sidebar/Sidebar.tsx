import React from "react";
import { Trash2 } from "lucide-react";

interface SidebarProps {
  onAddPanel: (panel: "KeyboardHeatmap"|"keyTimeline" | "userSessions" | "heatmapView" | "CircleGraph") => void;
  onUpdateCoords: () => void;
  onCountTreeNodes: () => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ onAddPanel, onUpdateCoords, onCountTreeNodes}) => {
  return (
    <aside className="w-64 bg-[#222222ff] text-white p-4 space-y-4">
      <div className="text-2xl text-center">
        <button onClick={() => onAddPanel("keyTimeline")} title="Key Timeline">
          🕒
        </button>
        <button onClick={() => onAddPanel("userSessions")} title="User Sessions">
          👥
        </button>
        <button onClick={() => onAddPanel("heatmapView")} title="Heatmap">
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
      </div>
      <div className="delete-area mt-8 text-center text-sm opacity-60">
        <Trash2 className="mx-auto" />
        Drop to delete
      </div>
    </aside>
  );
};
