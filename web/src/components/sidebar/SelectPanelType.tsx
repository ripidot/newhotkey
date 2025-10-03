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
import type { FormState, Formstring, Item } from "@/src/types/interface";

export function SelectPanelType({ onCancel }: { onCancel: () => void }) {
  // カテゴリ一覧
  const categories = ["キーの種別データ", "キーの時系列別データ", "総量データ"];
  // 全アイテム
  const items = [
    { name: "棒グラフ", categories: ["キーの種別データ", "キーの時系列別データ"] },
    { name: "折れ線グラフ", categories: ["キーの種別データ", "キーの時系列別データ"] },
    { name: "カウンター", categories: ["キーの種別データ", "キーの時系列別データ", "総量データ"] },
  ];

  const [formState, setFormState] = useState<FormState>({
    category: undefined,
    item: undefined,
    process: undefined,
  });

  const filteredItems = formState.category
    ? items.filter((item) => item.categories.includes(String(formState.category)))
    : [];

  // 入力更新用
  const handleChange = (key: keyof typeof formState, value: Formstring) => {
    setFormState((prev) => ({ ...prev, [key]: value }));
  };

  function isItemValidForCategory(item: Formstring, category: Formstring, items: Item[]) {
    if (!item || !category) return false;
    return items.some((it) => it.name === item && it.categories.includes(category));
  }

  // 送信処理
  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault(); // ページリロードを防ぐ
    console.log("最終的な入力:", formState);

    // ここで MosaicWindow を追加したり API を叩いたりできる
    // addMosaicWindow(formState);
  };

  return (
    <div className="w-full">
    <form onSubmit={handleSubmit}>
      {/* Datatype */}
      <div className="mb-4">
        <label className="text-sm font-medium">表示データ</label>
      <Select
        value={formState.category}
        onValueChange={(val) => {
          handleChange("category", val);
          if (!isItemValidForCategory(formState.item, val, items)) {
            handleChange("item", undefined);
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

      {/* Graph */}
      <div className="mb-4">
        <label className="text-sm font-medium">グラフ</label>
        <Select 
          value={formState.item} 
          onValueChange={(val) => handleChange("item", val)}>
          <SelectTrigger>
            <SelectValue placeholder="アイテムを選択" />
          </SelectTrigger>
        {formState.category && (
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
          value={formState.process} 
          onValueChange={(val) => handleChange("process", val)}>
          <SelectTrigger className="w-full mt-1">
            <SelectValue placeholder="プロセス名を選択" />
          </SelectTrigger>
          <ReturnSelectContent/>
        </Select>
      </div>

      {/* Actions */}
      <div className="flex justify-end space-x-2">
        <Button className="sidebarbutton" variant="ghost" onClick={onCancel}>Cancel</Button>
        <Button className="sidebarbuttonB"type="submit">Create</Button>
      </div>
      </form>
    </div>
  )
}