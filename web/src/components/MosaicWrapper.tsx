import React, { useState, useCallback, useEffect, useRef } from "react";
import { MosaicNode } from "react-mosaic-component";
import { nanoid } from "nanoid";

import { Sidebar } from "@/src/components/sidebar/Sidebar";
import { KeyboardBarChart } from "@/src/components/panels/KeyboardBarChart";
import { KeyboardLineChart } from "@/src/components/panels/KeyboardLineChart";
import { Counter } from "@/src/components/panels/Counter";
import { CircleGraph } from "@/src/components/panels/CircleGraph";
import { KeyboardHeatmap } from "@/src/components/panels/KeyboardHeatmap";

import Hexagon from "@/src/components/effect/Hexagon";
import { MosaicArea } from "@/src/components/mosaic/MosaicArea";

import { isQueryRecordKey } from "@/src/lib/utils";
import type { PanelInfo, Layout, PanelId, PanelType, QueryRecordKey, ValidFormState } from "@/src/types/interface";

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
  function FormstateToPanelType(charttype : string){
    switch(charttype){
      case "棒グラフ":
        return "KeyboardBarChart";
      case "折れ線グラフ":
        return "KeyboardLineChart";
      case "カウンター":
        return "KeyboardCounter";
      case "キーボードヒートマップ":
        return "KeyboardHeatmap";
    }return "Error";
  }


  const createPanelElement = (type: PanelType, id: PanelId): JSX.Element => {
    switch (type) {
      case "KeyboardBarChart":
        return <KeyboardBarChart key={id} process_name={""} aggcolumn={"key"}/>;
      case "KeyboardLineChart":
        return <KeyboardLineChart key={id} process_name={""} aggcolumn={"week"}/>;
      case "userSessions_all":
        return <KeyboardBarChart key={id} process_name={""} aggcolumn={"week"}/>;
      case "KeyboardCounter":
        return <Counter key={id} process_name={""} aggcolumn={"count"}/>;
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
  const createPanelElementwithFormState = (type: PanelType, id: PanelId, validformstate: ValidFormState): JSX.Element => {
      const process_name = (validformstate.processname !== "全て") ? validformstate.processname : "";
      const aggcolumn: QueryRecordKey  = ((validformstate.duration !== undefined) && (isQueryRecordKey(validformstate.duration)))
        ? (validformstate.duration) : "key";
      switch (type) {
        case "KeyboardBarChart":
          return <KeyboardBarChart key={id} process_name={ process_name } aggcolumn={aggcolumn}/>;

        case "KeyboardLineChart":
          return <KeyboardLineChart key={id} process_name={ process_name } aggcolumn={aggcolumn}/>;
        
        case "KeyboardCounter":
          return <Counter key={id} process_name={ process_name } aggcolumn={"count"}/>;
        case "CircleGraph":
          return <CircleGraph key={id} />;
        case "KeyboardHeatmap":
          return (
            <KeyboardHeatmap
              key={id}
              id={id}
              registerUpdateCoords={handleRegisterUpdateCoords}
              unregisterUpdateCoords={handleUnregisterUpdateCoords}
              process_name={process_name}
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

// 横方向に再帰的に並べる
function createHorizontalLayout(ids: string[]): Layout {
  if (ids.length === 1) return ids[0];
  const [first, ...rest] = ids;
  return { direction: "row", first, second: createHorizontalLayout(rest) };
}

// 縦方向に再帰的に並べる
function createVerticalLayout(layouts: Layout[]): Layout {
  if (layouts.length === 1) return layouts[0];
  const [first, ...rest] = layouts;
  return { direction: "column", first, second: createVerticalLayout(rest) };
}

// グリッド状に配置
function createGridLayout(ids: string[]): Layout | null {
  if (ids.length === 0) return null;
  if (ids.length === 1) return ids[0];

  const cols = Math.ceil(Math.sqrt(ids.length));
  const rows: Layout[] = [];

  for (let i = 0; i < ids.length; i += cols) {
    const rowIds = ids.slice(i, i + cols);
    rows.push(createHorizontalLayout(rowIds));
  }

  return createVerticalLayout(rows);
}

 // layout情報(uniqueIDのみで、パネルタイプはわからない)
  const [mosaicLayout, setMosaicLayout] = useState<MosaicNode<PanelId> | null>
  ( null );
// uniqueIDとpanelの種類の紐づけ
  const [panelMap, setPanelMap] = useState<
    Record<PanelId, PanelInfo>
  >({});


  const addPanel = useCallback(
    (panelType: PanelType) => {
      if (uniquePanel.includes(panelType)){

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

  const addPanelwithFormState = useCallback(
    (validformstate: ValidFormState) => {
      const panelType : PanelType = FormstateToPanelType(validformstate.charttype);
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
            render: () => createPanelElementwithFormState(panelType, uniqueId, validformstate),
          },
        }));
    },
    [panelMap]
  );

  const updateCoords = () => {
    updateCoordsMap.current.forEach((fn) => fn());
  };

  const alignPanel = () => {
    const ids = Object.keys(panelMap);
    if (ids.length === 0) return;
    const newLayout = createGridLayout(ids); // または createVerticalLayout(ids)
    setMosaicLayout(newLayout);
  }
  const checkSomething = () => {
    console.log("panelMap: ", panelMap);
    console.log("mosaicLayout: ", mosaicLayout);
  }

  useEffect(() => {
    updateCoords();
    window.addEventListener("resize", updateCoords);
    return () => window.removeEventListener("resize", updateCoords);
  }, [mosaicLayout]);

  useEffect(() => {
    if (!containerRef.current) return;
    const observer = new ResizeObserver(entries => {
      for (const entry of entries) {
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
      <div className="w-70 flex h-full">
        <Sidebar
          onAddPanel={addPanel}
          onCreatePanel={addPanelwithFormState}
          onUpdateCoords={updateCoords}
          onAlignBoard={alignPanel}
          onCheckLog={checkSomething}
        />
      </div>
      {/* Mosaic */}
      <div ref={containerRef} className="relative flex-1 h-full">
        <Hexagon size={size}/>
        <MosaicArea
          panelMap={panelMap}
          setPanelMap={setPanelMap}
          mosaicLayout={mosaicLayout}
          setMosaicLayout={setMosaicLayout}
        />
      </div>
    </div>
  );
}
