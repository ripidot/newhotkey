import React, { useContext } from "react";
import { MosaicContext } from "react-mosaic-component";
import { MosaicWindowProps } from 'react-mosaic-component';
import { Button } from "@blueprintjs/core";
import { IconNames } from "@blueprintjs/icons";
import type { PanelId } from "@/src/types/interface";

type RemoveButtonProps = Pick<MosaicWindowProps<PanelId>, "path"> & {
  onClick?: () => void;
};

export const RemoveButton: React.FC<RemoveButtonProps> = ({ path, onClick }) => {
  const context = useContext(MosaicContext);

  const handleClick = () => {
    if (context.mosaicActions && path) {
      console.log("in removebutton , path: ", path);
      context.mosaicActions.remove(path);
    } else {
      console.warn("❗ mosaicActions または path が undefined です", context);
    }

    onClick?.();
  };

  return <Button icon={IconNames.CROSS} onClick={handleClick} />;
};