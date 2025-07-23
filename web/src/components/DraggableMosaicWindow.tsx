// components/DraggableMosaicWindow.tsx
import React from "react";
import { useDrag } from "react-dnd";
import { MosaicWindow } from "react-mosaic-component";

type Props = {
  id: string;
  path: string[];
  children: React.ReactNode;
};

export const DraggableMosaicWindow: React.FC<Props> = ({ id, path, children }) => {
  const [, dragRef] = useDrag(() => ({
    type: "mosaic-panel",
    item: { id },
  }));

  return (
    <div ref={(node) => {
			dragRef(node);
		}}>
      <MosaicWindow<string> title={id} path={path}>
        {children}
      </MosaicWindow>
    </div>
  );
};
