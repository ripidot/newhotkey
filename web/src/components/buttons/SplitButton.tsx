// import 'blueprintjs/core/lib/css/blueprint.css';
// import 'blueprintjs/icons/lib/css/blueprint-icons.css';
import React, { useContext } from "react";
import { MosaicWindowContext } from "react-mosaic-component";
import { MosaicWindowProps } from 'react-mosaic-component';
import { Button } from "@blueprintjs/core";
import { IconNames } from "@blueprintjs/icons";


type PanelId = string;

type SplitButtonProps = Pick<MosaicWindowProps<PanelId>, 'path'>;

export const SplitButton: React.FC<SplitButtonProps> = ({ path }) => {
  const context = useContext(MosaicWindowContext);

  const handleClick = () => {
    console.log(context);
    if (context.mosaicWindowActions && path) {
      context.mosaicWindowActions.split(path);
    } else {
      console.warn("❗ mosaicActions または path が undefined です", context);
    }
  };
  return (
    <Button icon={IconNames.SPLIT_COLUMNS} onClick={handleClick} />
  );
};