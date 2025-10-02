import React from "react";
import { Button } from "@/src/components/ui/button"
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/src/components/ui/select"
import { ReturnSelectContent } from "@/src/components/sidebar/ReturnSelectContent";

export function SelectPanelType() {
  const [area, setArea] = React.useState("")
  const [severity, setSeverity] = React.useState("")
  const [datatype, setDatatype] = React.useState("")
  return (
    <div className="w-full">
      {/* Datatype */}
      <div className="mb-4">
        <label className="text-sm font-medium">表示データ</label>
        <Select value={datatype} onValueChange={setDatatype}>
          <SelectTrigger className="w-full mt-1">
            <SelectValue placeholder="表示データの種類を選択" />
          </SelectTrigger>
          <SelectContent>
            <SelectItem value="1">キーの種別データ</SelectItem>
            <SelectItem value="2">キーの時系列別データ</SelectItem>
          </SelectContent>
        </Select>
      </div>

      {/* Area */}
      <div className="mb-4">
        <label className="text-sm font-medium">グラフ</label>
        <Select value={area} onValueChange={setArea}>
          <SelectTrigger className="w-full mt-1">
            <SelectValue placeholder="グラフの種類を選択" />
          </SelectTrigger>
          <SelectContent>
            <SelectItem value="1">棒グラフ</SelectItem>
            <SelectItem value="2">折れ線グラフ</SelectItem>
            <SelectItem value="3">カウンター</SelectItem>
          </SelectContent>
        </Select>
      </div>

      {/* Security Level */}
      <div className="mb-4">
        <label className="text-sm font-medium">プロセス名</label>
        <Select value={severity} onValueChange={setSeverity}>
          <SelectTrigger className="w-full mt-1">
            <SelectValue placeholder="プロセス名を選択" />
          </SelectTrigger>
          <ReturnSelectContent/>
        </Select>
      </div>

      {/* Actions */}
      <div className="flex justify-end space-x-2">
        <Button variant="ghost">Cancel</Button>
        <Button type="submit">Create</Button>
      </div>
    </div>
  )
}