import React, { useState, useCallback, useEffect, useRef } from "react";
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
import { Counter } from "@/src/components/panels/Counter";
import { CircleGraph } from "@/src/components/panels/CircleGraph";
import { KeyboardHeatmap } from "@/src/components/panels/KeyboardHeatmap";

import Hexagon from "@/src/components/effect/Hexagon"

import { nanoid } from "nanoid";

export default function App() {
    type PanelId = string;
    type PanelType = "keyTimeline" | "userSessions" | "Counter" | "KeyboardHeatmap" | "CircleGraph";
    const updateCoordsMap = useRef<Map<string, () => void>>(new Map());

    const handleRegisterUpdateCoords = (id: string, fn: () => void) => {
        updateCoordsMap.current.set(id, fn);
        console.log(`registered ${id}`);
    };
    const handleUnregisterUpdateCoords = (id: string) => {
        updateCoordsMap.current.delete(id);
        console.log(`unregistered ${id}`);
    };

    const createPanelElement = (type: PanelType, id: PanelId): JSX.Element => {
        switch (type) {
            case "keyTimeline":
                return <KeyTimeline key={id} />;
            case "userSessions":
                return <UserSessions key={id} />;
            case "Counter":
                return <Counter key={id} />;
            case "CircleGraph":
                return <CircleGraph key={id} />;
            case "KeyboardHeatmap":
                return <KeyboardHeatmap key={id} id={id} registerUpdateCoords={handleRegisterUpdateCoords}
                unregisterUpdateCoords={handleUnregisterUpdateCoords}/>;
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

    const [mosaicLayout, setMosaicLayout] = useState<MosaicNode<PanelId> | null>(null);
    const [panelMap, setPanelMap] = useState<Record<PanelId, { type: PanelType; element: JSX.Element }>>({});

    const addPanel = useCallback((panelType: PanelType) => {
        if (panelType === "Counter") { // 複数追加できないビュー
            const existingId = Object.keys(panelMap).find((id) => id.startsWith("Counter-"));
            if (existingId) {
                setMosaicLayout((prevLayout) => removePanelById(prevLayout, existingId));
                setPanelMap((prev) => {
                    const newMap = { ...prev };
                    delete newMap[existingId];
                    return newMap;
                });
            } else {
                const uniqueId = `Counter-${nanoid(6)}`;
                setMosaicLayout((prevLayout) => { // prevLayoutとしているが、一時変数なためprevでも可
                    if (!prevLayout) return uniqueId;
                    return {
                        direction: "row",
                        first: prevLayout,
                        second: uniqueId,
                    };
                });
                setPanelMap((prev) => ({ //<Record<PanelId, { type: PanelType; element: JSX.Element }>> の辞書, mapよりもjsonに適している
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

    const updateCoords = () => {
        console.log("in updatecoords");
        updateCoordsMap.current.forEach(fn => fn());
    }


    const countTreeNodes = () => {
        type MosaicNode<T> = T | { direction: "row" | "column"; first: MosaicNode<T>; second: MosaicNode<T> } | null;
        function countNodes<T>(node: MosaicNode<T> | null): number {
            if (!node) return 0;

            // 内部ノードかどうか判定
            if (typeof node === "object" && "direction" in node) {
                return countNodes(node.first) + countNodes(node.second);
            }
            // 葉ノード
            return 1;
        }
        console.log(countNodes(mosaicLayout));
    }

    useEffect(() => {
        updateCoords();

        window.addEventListener("resize", updateCoords);
        return () => window.removeEventListener("resize", updateCoords);
    }, [mosaicLayout]); // 状態ドリブン,非同期処理に適している, 外部変更のキャッチ

    return (
        <div style={{display: "flex", height: "100vh"}}>
            <Sidebar onAddPanel={addPanel} onUpdateCoords={updateCoords} onCountTreeNodes={countTreeNodes}/>
            <div style={{flex: 1}}>
                <Hexagon size={300} fill="transparent" stroke="#000" className="animate-pulse" />
                <Mosaic<PanelId>
                    renderTile={(id, path) => (
                        <MosaicWindow<PanelId>
                            title=""
                            path={path}
                            toolbarControls={[]}
                            renderToolbar={(props) => {
                                return( // titlebar -> titlename -> icon, justifycontentで左右に分ける
                                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', width: '100%', backgroundColor: "#0000" }}>
                                        <div style={{ paddingLeft: '8px', fontWeight: 'bold', backgroundColor: "#0000" }}>{id}</div>
                                        <div style={{ display: 'flex', gap: '8px', alignItems: 'center', backgroundColor: "#0000" }}>
                                            <SplitButton {...props}/>
                                            <ExpandButton {...props}/>
                                            <RemoveButton {...props}/>
                                        </div>
                                    </div>
                                );
                            }}
                        >
                        <div style={{ display: 'flex', gap: '8px', alignItems: 'center', backgroundColor: "#0000" }}>
                            {panelMap[id]?.element || <div>パネルが見つかりません</div>}
                        </div>
                        </MosaicWindow>
                    )}
                    value={mosaicLayout} // ツリー構造データ
                    onChange={setMosaicLayout} // valueに変更があった時に呼ばれる, イベントドリブンなので非同期処理には不向き, 外部変更のキャッチ
                />
            </div>
        </div>
    );
}