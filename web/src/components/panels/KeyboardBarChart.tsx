// src/components/panels/KeyTimeline.tsx
import { BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";
import type { QueryRequest, QueryRecordKey, QueryResult } from "@/src/types/interface";
import { ReturnProcessName, DrawExcept } from "@/src/lib/utils";
import { useQueryRecord } from "@/src/hooks/useQueryRecord";

export function KeyboardBarChart({
    process_name, aggcolumn
  }: {
    process_name: string;
    aggcolumn: QueryRecordKey;
  }) {
    return (
      <div>
        <Graph process_name={process_name} aggcolumn={aggcolumn}/>
      </div>
    );
}
function Graph<T extends QueryRecordKey>({ process_name, aggcolumn }: { process_name: string; aggcolumn: T }) {
  // const aggcolumn = "key";
  // const process_name = "Explorer.EXE";

  const baseRequest: QueryRequest = {
    select: [aggcolumn],
    group_by: [aggcolumn],
    aggregates: [{ func: "count", alias: "count" }],
    order_by: [{ field: "count", direction: "desc" }],
    limit: 20,
  };
// ...( { where: { process_name: "Explorer.EXE" } } )が展開される
  const requestData: QueryRequest = {
    ...baseRequest,
    ...(process_name ? { where: { process_name } } : {}),
  };

  const {queryRecord, loading, error} = useQueryRecord<QueryResult<T>>(requestData);

  if (loading) return <DrawExcept loading={true} error={null}/>;
  if (error) return <DrawExcept loading={false} error={error} />;

  return (
    <div className="testbox space-y-4">
    <ReturnProcessName aggcolumn={aggcolumn} process_name={process_name} vtype={"graph"} />
      <BarChart width={300} height={200} data={queryRecord}>
        <XAxis dataKey={aggcolumn} />
        <YAxis />
        <Tooltip />
        <Bar dataKey="count" style={{fill: "var(--color-graph)"}} />
      </BarChart>
    </div>
  );
}