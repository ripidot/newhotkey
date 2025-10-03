import { useState } from "react";
import { motion } from "framer-motion";
import { SelectPanelType } from "@/src/components/sidebar/SelectPanelType";
import type { FormState, ValidFormState } from "@/src/types/interface";

export function ConfigPanel({ onCreatePanel }: {
  onCreatePanel: (validformstate: ValidFormState) => void }) {
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
            <SelectPanelType onCreatePanel={onCreatePanel} onCancel={() => setOpen(false)}/>
          </label>
        </div>
      </motion.div>
    </div>
  );
}