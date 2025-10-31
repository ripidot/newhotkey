import React, { useContext } from "react";
import { MosaicContext } from "react-mosaic-component";
import { MosaicWindowProps } from 'react-mosaic-component';
import { Button } from "@blueprintjs/core";
import { IconNames } from "@blueprintjs/icons";
import type { PanelId } from "@/src/types/interface";

type ExpandButtonProps = Pick<MosaicWindowProps<PanelId>, 'path'>;

export const ExpandButton: React.FC<ExpandButtonProps> = ({ path }) => {
  const context = useContext(MosaicContext);

  const handleClick = () => {
    console.log(context);
    if (context.mosaicActions && path) {
      context.mosaicActions.expand(path);
    } else {
      console.warn("❗ mosaicActions または path が undefined です", context);
    }
  };
  return (
    <Button icon={IconNames.EXPAND_ALL} onClick={handleClick} />
  );
};