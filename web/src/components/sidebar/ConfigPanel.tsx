import { useState } from "react";
import { motion } from "framer-motion";
import { SelectPanelType } from "@/src/components/sidebar/SelectPanelType";

import React from "react";

export function ConfigPanel() {
  const [open, setOpen] = useState(false);

  return (
    <div className="p-4 border rounded">
      <button
        onClick={() => setOpen(!open)}
        className="sidebar_button font-semibold"
      >
        新しいパネル
      </button>

      <motion.div
        initial={false}
        animate={{ height: open ? "auto" : 0, opacity: open ? 1 : 0 }}
        transition={{ duration: 0.3 }}
        style={{ overflow: "hidden" }}
      >
        <div className="mt-4">
          <label className="items-center">
            <SelectPanelType/>
          </label>
        </div>
      </motion.div>
    </div>
  );
}