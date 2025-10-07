import React from "react";
import { ConfigPanel } from "@/src/components/sidebar/ConfigPanel";
import { ValidFormState } from "@/src/types/interface";
import type { PanelType } from "@/src/types/interface";
import { Icon } from "@blueprintjs/core";

interface SidebarProps {
  onAddPanel: (panel: PanelType) => void;
  onCreatePanel: (validformstate: ValidFormState ) => void;
  onUpdateCoords: () => void;
  onAlignBoard: () => void;
  onCheckLog: () => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ onAddPanel, onCreatePanel, onUpdateCoords, onAlignBoard, onCheckLog}) => {
  return ( // aside : 付随的な内容 Margin(自コンテンツのゆとり), Padding(他要素との間隔), space-y-4(子要素同士の間隔)
    <aside className="sidebar p-4 space-y-4 w-full">
      <ConfigPanel onCreatePanel={onCreatePanel}/>
      <div className="sample-button-container">
        <button className="sample-button" onClick={() => onAddPanel("KeyboardBarChart")} title="サンプルA">
          <Icon icon="timeline-bar-chart" className="bp5-icon" />
          サンプルA
        </button>
        <button className="sample-button" onClick={() => onAddPanel("KeyboardLineChart")} title="サンプルB">
          <Icon icon="timeline-line-chart" className="bp5-icon" />
          サンプルB
        </button>
        <button className="sample-button" onClick={() => onAddPanel("KeyboardCounter")} title="サンプルC">
          <Icon icon="dashboard" className="bp5-icon" />
          サンプルC
        </button>
        <button className="sample-button" onClick={() => onAddPanel("KeyboardHeatmap")} title="サンプルD">
          <Icon icon="key" className="bp5-icon" />
          サンプルD
        </button>
      </div>
      <div className="sample-button-container">
        <button className="sample-button" onClick={() => onAlignBoard()} title="自動ソートボタン">
          <Icon icon="sort" className="bp5-icon" />
          自動ソートボタン
        </button>
      </div>
      <div className="align text-center">
        <button onClick={() => onUpdateCoords()} title="Update Coords">
          ✨
        </button>
        <button onClick={() => onAlignBoard()} title="Count Nodes">
          🔑
        </button>
        <button onClick={() => onCheckLog()} title="Check log">
          📃
        </button>
      </div>
    </aside>
  );
};
