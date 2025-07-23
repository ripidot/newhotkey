import React from "react";
import { Trash2 } from "lucide-react";

interface SidebarProps {
  onAddPanel: (panel: "keyTimeline" | "userSessions" | "heatmapView") => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ onAddPanel }) => {
  return (
    <aside className="w-64 bg-gray-900 text-white p-4 space-y-4">
      <div className="text-2xl text-center">
        <button onClick={() => onAddPanel("keyTimeline")} title="Key Timeline">
          🕒
        </button>
        <button onClick={() => onAddPanel("userSessions")} title="User Sessions">
          👥
        </button>
        <button onClick={() => onAddPanel("heatmapView")} title="Heatmap">
          🔥
        </button>
      </div>
      <div className="delete-area mt-8 text-center text-sm opacity-60">
        <Trash2 className="mx-auto" />
        Drop to delete
      </div>
    </aside>
  );
};
