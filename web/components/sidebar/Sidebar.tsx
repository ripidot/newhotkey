import React from "react";

interface SidebarProps {
	onAddPanel: (panel: "keyTimeline" | "userSessions" | "heatmap") => void;
}

export function Sidebar({ onAddPanel }: SidebarProps) {
	return (
	<aside className="w-16 bg-gray-800 text-white flex flex-col items-center py-4 space-y-4">
		<button onClick={() => onAddPanel("keyTimeline")} title="Key Timeline">
		🕒
		</button>
		<button onClick={() => onAddPanel("userSessions")} title="User Sessions">
		👥
		</button>
		<button onClick={() => onAddPanel("heatmap")} title="Heatmap">
		🔥
		</button>
	</aside>
	);
}
