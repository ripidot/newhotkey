import { Button } from "@/src/components/ui/button"
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/src/components/ui/select"
import { ReturnSelectContent } from "@/src/components/sidebar/ReturnSelectContent";
import { useState } from "react";
import type { FormState, Formstring, Item ,  ValidFormState} from "@/src/types/interface";

export function SelectPanelType({ onCreatePanel, onCancel }: { 
  onCreatePanel: (validformstate: ValidFormState) => void; 
  onCancel: () => void }) {
  // カテゴリ一覧
  const categories = ["キー種別データ", "時系列データ", "総量データ"];
  const durations = ["day", "week", "month"];
  // 全アイテム
  const items = [
    { name: "棒グラフ", categories: ["キー種別データ", "時系列データ"] },
    { name: "折れ線グラフ", categories: ["時系列データ"] },
    { name: "カウンター", categories: ["総量データ"] },
    { name: "キーボードヒートマップ", categories: ["キー種別データ"] },
  ];

  const [formState, setFormState] = useState<FormState>({
    datatype: undefined,
    charttype: undefined,
    processname: undefined,
    duration: undefined
  });

  const filteredItems = formState.datatype
    ? items.filter((item) => item.categories.includes(String(formState.datatype)))
    : [];

  // 入力更新用
  const handleChange = (key: keyof typeof formState, value: Formstring) => {
    setFormState((prev) => ({ ...prev, [key]: value }));
  };

  function isItemValidForDatatype(item: Formstring, datatype: Formstring, items: Item[]) {
    if (!item || !datatype) return false;
    return items.some((it) => it.name === item && it.categories.includes(datatype));
  }

function isValidFormState(fs: FormState): fs is ValidFormState {
  return fs.datatype !== undefined &&
         fs.charttype !== undefined &&
         fs.processname !== undefined;
}

  return (
    <div className="w-full">
      {/* Datatype */}
      <div className="mb-4">
        <label className="text-sm font-medium">表示データ</label>
      <Select
        value={formState.datatype}
        onValueChange={(val) => {
          handleChange("datatype", val);
          if (!isItemValidForDatatype(formState.charttype, val, items)) {
            handleChange("charttype", undefined);
          }
          if (val !== "時系列データ"){
            handleChange("duration", undefined);
          }
        }}
      >
        <SelectTrigger>
          <SelectValue placeholder="カテゴリを選択" />
        </SelectTrigger>
        <SelectContent>
          {categories.map((cat) => (
            <SelectItem key={cat} value={cat}>
              {cat}
            </SelectItem>
          ))}
        </SelectContent>
      </Select>
      </div>
      
      {/* duration */}
      {(formState.datatype === "時系列データ") && (
      <div className="mb-4">
        <label className="text-sm font-medium">期間</label>
          <Select
            value={formState.duration}
            onValueChange={(val) => {
              handleChange("duration", val);
            }}
          >
            <SelectTrigger>
              <SelectValue placeholder="期間を選択" />
            </SelectTrigger>
            <SelectContent>
              {durations.map((cat) => (
                <SelectItem key={cat} value={cat}>
                  {cat}
                </SelectItem>
              ))}
            </SelectContent>
          </Select>
          </div>
      )}

      {/* Graph */}
      <div className="mb-4">
        <label className="text-sm font-medium">グラフ</label>
        <Select 
          value={formState.charttype}
          onValueChange={(val) => handleChange("charttype", val)}>
          <SelectTrigger>
            <SelectValue placeholder="グラフを選択" />
          </SelectTrigger>
        {formState.datatype && (
          <SelectContent>
            {filteredItems.map((item) => (
              <SelectItem key={item.name} value={item.name}>
                {item.name}
              </SelectItem>
            ))}
          </SelectContent>
        )}
        </Select>
      </div>

      {/* Processname */}
      <div className="mb-4">
        <label className="text-sm font-medium">プロセス名</label>
        <Select 
          value={formState.processname} 
          onValueChange={(val) => handleChange("processname", val)}>
          <SelectTrigger className="w-full mt-1">
            <SelectValue placeholder="プロセス名を選択" />
          </SelectTrigger>
          <ReturnSelectContent/>
        </Select>
      </div>

      {/* Actions */}
      <div className="flex justify-end space-x-2">
        <Button className="sidebarbutton" variant="ghost" onClick={onCancel}>Cancel</Button>
        <Button className="sidebarbuttonB" type="button"
          onClick={() => {
            if (isValidFormState(formState)){
              const validformstate: ValidFormState = formState;
              onCreatePanel(validformstate)}
            else { console.log("未入力");
              console.log("type: ", typeof formState.charttype, ", value: ", formState.charttype)
              console.log(formState);
             }}}>
              Create</Button>
      </div>
    </div>
  )
}