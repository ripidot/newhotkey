import React, { useState, useCallback, useEffect } from "react";
import {
    Mosaic,
    MosaicNode,
    MosaicWindow,
} from "react-mosaic-component";
import "react-mosaic-component/react-mosaic-component.css";
import { SplitButton } from "@/src/components/buttons/SplitButton";
import { RemoveButton } from "@/src/components/buttons/RemoveButton";
import { ExpandButton } from "@/src/components/buttons/ExpandButton";
import { Sidebar } from "@/src/components/sidebar/Sidebar";
import { KeyTimeline } from "@/src/components/panels/Keytimeline";
import { UserSessions } from "@/src/components/panels/UserSessions";
import { HeatmapView } from "@/src/components/panels/HeatmapView";
import { CircleGraph } from "@/src/components/panels/CircleGraph";
import { nanoid } from "nanoid";

type PanelId = string;
type PanelType = "keyTimeline" | "userSessions" | "heatmapView" | "CircleGraph";

const createPanelElement = (type: PanelType, id: PanelId): JSX.Element => {
    switch (type) {
        case "keyTimeline":
            return <KeyTimeline key={id} />;
        case "userSessions":
            return <UserSessions key={id} />;
        case "heatmapView":
            return <HeatmapView key={id} />;
        case "CircleGraph":
            return <CircleGraph key={id} />;
        default:
            return <div key={id}>未定義パネル</div>;
    }
};

const removePanelById = (
    layout: MosaicNode<PanelId> | null,
    targetId: PanelId
): MosaicNode<PanelId> | null => {
    if (!layout) return null;
    if (layout === targetId) return null;
    if (typeof layout === "object") {
        const left = removePanelById(layout.first, targetId);
        const right = removePanelById(layout.second, targetId);

        if (!left) return right;
        if (!right) return left;

        return { ...layout, first: left, second: right };
    }
    return layout;
};

export default function App() {
    const [mosaicLayout, setMosaicLayout] = useState<MosaicNode<PanelId> | null>(null);
    const [panelMap, setPanelMap] = useState<Record<PanelId, { type: PanelType; element: JSX.Element }>>({});

    const addPanel = useCallback((panelType: PanelType) => {
        if (panelType === "heatmapView") {
            const existingId = Object.keys(panelMap).find((id) => id.startsWith("heatmap-"));
            if (existingId) {
                setMosaicLayout((prevLayout) => removePanelById(prevLayout, existingId));
                setPanelMap((prev) => {
                    const newMap = { ...prev };
                    delete newMap[existingId];
                    return newMap;
                });
            } else {
                const uniqueId = `heatmap-${nanoid(6)}`;
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
            }
            return;
        }

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
    }, [panelMap]);

    // 🧠 削除機能（useEffect で DOM を監視）
    useEffect(() => {
        let draggingPanelId: string | null = null;

        const onMouseDown = (e: MouseEvent) => {
            const header = (e.target as HTMLElement).closest(".mosaic-window-title");
            if (header) {
                const parent = header.closest("[data-react-mosaic-id]");
                const id = parent?.getAttribute("data-react-mosaic-id");
                draggingPanelId = id ?? null;
                console.log("🟡 Drag start from panel:", draggingPanelId);
            }
        };

        const onMouseUp = (e: MouseEvent) => {
            console.log("🔵 in onMouseUp:");
            if (!draggingPanelId) return;

            const dropTarget = document.elementFromPoint(e.clientX, e.clientY);
            const dropClass = (dropTarget as HTMLElement)?.className || "";
            console.log("🔵 Mouse up over:", dropClass);

            if (dropTarget?.classList.contains("delete-area")) {
                console.log("🔴 Deleting panel:", draggingPanelId);
                setMosaicLayout((prev) => removePanelById(prev, draggingPanelId!));
                setPanelMap((prev) => {
                    const newMap = { ...prev };
                    delete newMap[draggingPanelId!];
                    return newMap;
                });
            }

            draggingPanelId = null;
        };

        document.addEventListener("mousedown", onMouseDown);
        document.addEventListener("mouseup", onMouseUp);

        return () => {
            document.removeEventListener("mousedown", onMouseDown);
            document.removeEventListener("mouseup", onMouseUp);
        };
    }, []);


    return (
        <div style={{display: "flex", height: "100vh"}}>
            <Sidebar onAddPanel={addPanel} />
            <div style={{flex: 1}}>
                <Mosaic<PanelId>
                    renderTile={(id, path) => (
                        <MosaicWindow<PanelId>
                            title=""
                            path={path}
                            createNode={() => `${id}-${nanoid(4)}`}
                            toolbarControls={[]}
                            renderToolbar={(props) => {
                                return(
                                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', width: '100%', backgroundColor: "#0000" }}>
                                        <div style={{ paddingLeft: '8px', fontWeight: 'bold', backgroundColor: "#fff0" }}>{id}</div>
                                        <div style={{ display: 'flex', gap: '8px', alignItems: 'center', backgroundColor: "#0000" }}>
                                            <SplitButton {...props}/>
                                            <ExpandButton {...props}/>
                                            <RemoveButton {...props}/>
                                        </div>
                                    </div>
                                );
                            }}
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