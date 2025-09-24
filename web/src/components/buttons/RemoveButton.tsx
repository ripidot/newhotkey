// import 'blueprintjs/core/lib/css/blueprint.css';
// import 'blueprintjs/icons/lib/css/blueprint-icons.css';
import React, { useContext } from "react";
import { MosaicContext } from "react-mosaic-component";
import { MosaicWindowProps } from 'react-mosaic-component';
import { Button } from "@blueprintjs/core";
import { IconNames } from "@blueprintjs/icons";
import type { PanelId } from "@/src/types/interface";

type RemoveButtonProps = Pick<MosaicWindowProps<PanelId>, 'path'>;

export const RemoveButton: React.FC<RemoveButtonProps> = ({ path }) => {
  const context = useContext(MosaicContext);

  const handleClick = () => {
    console.log(context);
    if (context.mosaicActions && path) {
      context.mosaicActions.remove(path);
    } else {
      console.warn("❗ mosaicActions または path が undefined です", context);
    }
  };
  return (
    <Button icon={IconNames.CROSS} onClick={handleClick} />
  );
};