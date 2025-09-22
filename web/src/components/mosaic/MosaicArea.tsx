import React from "react";
import { Mosaic, MosaicWindow, MosaicNode } from "react-mosaic-component";
import "react-mosaic-component/react-mosaic-component.css";

import { SplitButton } from "@/src/components/buttons/SplitButton";
import { RemoveButton } from "@/src/components/buttons/RemoveButton";
import { ExpandButton } from "@/src/components/buttons/ExpandButton";

type PanelId = string;

interface MosaicAreaProps {
  panelMap: Record<PanelId, {render: () => JSX.Element;}>;
  mosaicLayout: MosaicNode<PanelId> | null;
  setMosaicLayout: (layout: MosaicNode<PanelId> | null) => void;
}

export const MosaicArea: React.FC<MosaicAreaProps> = ({
  panelMap,
  mosaicLayout,
  setMosaicLayout,
}) => {
  return (
    <Mosaic<PanelId>
      renderTile={(id, path) => (
        <MosaicWindow<PanelId>
          title=""
          key={id}
          path={path}
          toolbarControls={[]}
          renderToolbar={(props) => (
            <div
              style={{
                display: "flex",
                justifyContent: "space-between",
                alignItems: "center",
                width: "100%",
                backgroundColor: "#0000",
              }}
            >
              <div style={{ paddingLeft: "8px", fontWeight: "bold" }}>
                {id}
              </div>
              <div style={{ display: "flex", gap: "8px", alignItems: "center" }}>
                <SplitButton {...props} />
                <ExpandButton {...props} />
                <RemoveButton {...props} />
              </div>
            </div>
          )}
        >
          <div className="testbox space-y-4 p-4 bg-[#ffffff00]" style={{ display: "flex", gap: "8px", alignItems: "center" }}>
            {panelMap[id]?.render() || <div>パネルが見つかりません</div>}
          </div>
        </MosaicWindow>
      )}
      value={mosaicLayout} // valueが変わるとuseEffectが動く
      onChange={setMosaicLayout}
    />
  );
};
