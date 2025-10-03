import React, { useState, useCallback, useEffect, useRef } from "react";
import { MosaicNode } from "react-mosaic-component";
import { nanoid } from "nanoid";

import { Sidebar } from "@/src/components/sidebar/Sidebar";
import { KeyTimeline } from "@/src/components/panels/Keytimeline";
import { UserSessions } from "@/src/components/panels/UserSessions";
import { Counter } from "@/src/components/panels/Counter";
import { CircleGraph } from "@/src/components/panels/CircleGraph";
import { KeyboardHeatmap } from "@/src/components/panels/KeyboardHeatmap";

import Hexagon from "@/src/components/effect/Hexagon";
import { MosaicArea } from "@/src/components/mosaic/MosaicArea";

import type { PanelId, PanelType } from "@/src/types/interface";

export default function MosaicWrapper() {
  const updateCoordsMap = useRef<Map<string, () => void>>(new Map());
  const containerRef = useRef<HTMLDivElement>(null);
  const [size, setSize] = useState<{width:number, height:number}>({width:0, height:0});
  const handleRegisterUpdateCoords = (id: string, fn: () => void) => {
    updateCoordsMap.current.set(id, fn);
  };
  const handleUnregisterUpdateCoords = (id: string) => {
    updateCoordsMap.current.delete(id);
  };
  const uniquePanel = [""]; // 重複を許さないリスト

  const createPanelElement = (type: PanelType, id: PanelId): JSX.Element => {
    switch (type) {
      case "keyTimeline":
        return <KeyTimeline key={id} process_name={"Explorer.EXE"} aggcolumn={"key"}/>;
      case "userSessions":
        return <UserSessions key={id} process_name={""} aggcolumn={"week"}/>;
      case "userSessions_all":
        return <UserSessions key={id} process_name={"Explorer.EXE"} aggcolumn={"week"}/>;
      case "Counter":
        return <Counter key={id} process_name={"Explorer.EXE"} aggcolumn={"count"}/>;
      case "CircleGraph":
        return <CircleGraph key={id} />;
      case "KeyboardHeatmap":
        return (
          <KeyboardHeatmap
            key={id}
            id={id}
            registerUpdateCoords={handleRegisterUpdateCoords}
            unregisterUpdateCoords={handleUnregisterUpdateCoords}
            process_name={"Explorer.EXE"}
          />
        );
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

  const [mosaicLayout, setMosaicLayout] = useState<MosaicNode<PanelId> | null>(
    null
  );
  const [panelMap, setPanelMap] = useState<
    Record<PanelId, { type: PanelType; render: () => JSX.Element }>
  >({});


  const addPanel = useCallback(
    (panelType: PanelType) => {
      if (uniquePanel.includes(panelType)){
      // if (panelType === "Counter") {

        const existingId = Object.keys(panelMap).find((id) =>
          id.startsWith("Counter-")
        );
        if (existingId) {
          setMosaicLayout((prevLayout) =>
            removePanelById(prevLayout, existingId)
          );
          setPanelMap((prev) => {
            const newMap = { ...prev };
            delete newMap[existingId];
            return newMap;
          });
        } else {
          const uniqueId = `Counter-${nanoid(6)}`;
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
    render: () => createPanelElement(panelType, uniqueId),
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
    render: () => createPanelElement(panelType, uniqueId),
  },
}));

    },
    [panelMap]
  );

  const updateCoords = () => {
    updateCoordsMap.current.forEach((fn) => fn());
  };

  useEffect(() => {
    updateCoords();
    window.addEventListener("resize", updateCoords);
    return () => window.removeEventListener("resize", updateCoords);
  }, [mosaicLayout]);

  useEffect(() => {
    if (!containerRef.current) return;
    const observer = new ResizeObserver(entries => {
      for (let entry of entries) {
        setSize({
          width: entry.contentRect.width,
          height: entry.contentRect.height,
        });
      }
    });
    observer.observe(containerRef.current);
    return () => observer.disconnect();
  }, []);
  return (
    <div className="flex flex-row w-full h-full">
      {/* サイドバー */}
      <div className="w-56 flex h-full">
        <Sidebar
          onAddPanel={addPanel}
          onUpdateCoords={updateCoords}
          onCountTreeNodes={() => {}}
          onCheckLog={() => {}}
        />
      </div>
      {/* Mosaic */}
      <div ref={containerRef} className="relative flex-1 h-full">
        <Hexagon size={size}/>
        <MosaicArea
          panelMap={panelMap}
          mosaicLayout={mosaicLayout}
          setMosaicLayout={setMosaicLayout}
        />
      </div>
    </div>
  );
}
