// src/components/panels/KeyTimeline.tsx
import { LineChart, Line, XAxis, YAxis, Tooltip } from "recharts";
import type { QueryRequest, QueryRecordKey, QueryResult } from "@/src/types/interface";
import { useQueryRecord } from "@/src/hooks/useQueryRecord";
import { ReturnProcessName, DrawExcept } from "@/src/lib/utils";
// src/components/panels/KeyTimeline.tsx

export function KeyboardLineChart({
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
  // const aggcolumn = "week";
  const baseRequest: QueryRequest = {
    select: [aggcolumn],
    group_by: [aggcolumn],
    aggregates: [{ func: "count", alias: "count" }],
    order_by: [{ field: aggcolumn, direction: "asc" }],
    limit: 20,
  };
  const requestData: QueryRequest = {
    ...baseRequest,
    ...(process_name && { where: { process_name } }),
  };

  const {queryRecord, loading, error} = useQueryRecord<QueryResult<T>>(requestData);

  if (loading) return <DrawExcept loading={true} error={null}/>;
  if (error) return <DrawExcept loading={false} error={error} />;

  return (
  <div className="testbox space-y-4">
    <ReturnProcessName aggcolumn={aggcolumn} process_name={process_name} vtype={"graph"} />
      <LineChart width={300} height={200} data={queryRecord}>
        <XAxis dataKey={aggcolumn} tickFormatter={(value: string) => value.split("T")[0]}/>
        <YAxis />
        <Tooltip />
        <Line style={{stroke: "var(--color-graph)"}} type="linear" dataKey="count"/>
      </LineChart>
    </div>
  );
}