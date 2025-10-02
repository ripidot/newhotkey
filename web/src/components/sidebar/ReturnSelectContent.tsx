import type { QueryRequest } from "@/src/types/interface";
import { useQueryRecord } from "@/src/hooks/useQueryRecord";
import { DrawExcept } from "@/src/lib/utils";
import {
  SelectContent,
  SelectItem,
} from "@/src/components/ui/select"

export function ReturnSelectContent(){
  const requestData: QueryRequest = {
    select: ["process_name"],
    group_by: ["process_name"],
    aggregates: [{ func: "count", alias: "count" }],
    order_by: [{ field: "count", direction: "desc" }],
    limit: 10,
  };

  const {queryRecord, loading, error} = useQueryRecord<{ process_name: string; count: number }>(requestData);

  if (loading || error)
    return (
      <SelectContent>
        <SelectItem value="1">
          <DrawExcept loading={loading} error={error}/>
        </SelectItem>
      </SelectContent>
    );

  return (
    <SelectContent>
      <SelectItem key={0} value={"0"}>全て</SelectItem>
      {queryRecord.map((record, index) => (
        <SelectItem key={record.process_name} value={String(index + 1)}>
          {record.process_name}
        </SelectItem>
      ))}
    </SelectContent>
  );
}