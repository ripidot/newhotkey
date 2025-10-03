import { useState } from "react";
import { motion } from "framer-motion";
import { SelectPanelType } from "@/src/components/sidebar/SelectPanelType";

export function ConfigPanel() {
  const [open, setOpen] = useState(false);

  return (
    <div className="p-4 border rounded">
      <button
        onClick={() => setOpen(!open)}
        className="sidebarbutton font-semibold"
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
            <SelectPanelType onCancel={() => setOpen(false)}/>
          </label>
        </div>
      </motion.div>
    </div>
  );
}