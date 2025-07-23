// import 'blueprintjs/core/lib/css/blueprint.css';
// import 'blueprintjs/icons/lib/css/blueprint-icons.css';
import classNames from 'classnames';
import React, { useContext } from "react";
import { MosaicContext } from "react-mosaic-component";
import { MosaicWindowProps } from 'react-mosaic-component';
import { Button } from "@blueprintjs/core";
import { IconNames } from "@blueprintjs/icons";


type PanelId = string;

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