import React, { useState, useCallback } from "react";
import {
	Mosaic,
	MosaicNode,
	MosaicWindow,
	getLeaves,
} from "react-mosaic-component";
import "react-mosaic-component/react-mosaic-component.css";
import { Sidebar } from "@/components/sidebar/Sidebar";
import { KeyTimeline } from "@/components/panels/Keytimeline";
import { UserSessions } from "@/components/panels/UserSessions";
import { HeatmapView } from "@/components/panels/HeatmapView";
import { nanoid } from "nanoid";

// パネルIDは string に変更（例: "userSessions-abc123"）
type PanelId = string;
type PanelType = "keyTimeline" | "userSessions" | "heatmap";

const createPanelElement = (type: PanelType, id: PanelId): JSX.Element => {
	switch (type) {
		case "keyTimeline":
	return <KeyTimeline key={id} />;
		case "userSessions":
	return <UserSessions key={id} />;
		case "heatmap":
	return <HeatmapView key={id} />;
		default:
	return <div key={id}>未定義パネル</div>;
	}
};

export default function App() {
	const [mosaicLayout, setMosaicLayout] = useState<MosaicNode<PanelId> | null>(null);
	const [panelMap, setPanelMap] = useState<Record<PanelId, { type: PanelType; element: JSX.Element }>>({});

	const addPanel = useCallback((panelType: PanelType) => {
		const uniqueId = `${panelType}-${nanoid(6)}`;

		setMosaicLayout((prevLayout) => {
			if (!prevLayout) return uniqueId;
			return {
				direction: "row",
				first: prevLayout,
				second: uniqueId,
			};
		});

		setPanelMap((prev) => ({
			...prev,
			[uniqueId]: {
			type: panelType,
			element: createPanelElement(panelType, uniqueId),
			},
		}));
	}, []);

	return (
		<div style={{ display: "flex", height: "100vh" }}>
			<Sidebar onAddPanel={addPanel} />
			<div style={{ flex: 1 }}>
				<Mosaic<PanelId>
					renderTile={(id, path) => (
						<MosaicWindow<PanelId>
							title={id}
							path={path}
							createNode={() => `${id}-${nanoid(4)}`}
							toolbarControls={[]}
						>
							{panelMap[id]?.element || <div>パネルが見つかりません</div>}
						</MosaicWindow>
					)}
					value={mosaicLayout}
					onChange={setMosaicLayout}
				/>

			</div>
		</div>
	);
}
