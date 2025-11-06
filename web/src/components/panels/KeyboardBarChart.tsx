"use client";

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
  const baseRequest: QueryRequest = {
    select: [aggcolumn],
    group_by: [aggcolumn],
    aggregates: [{ func: "count", alias: "count" }],
    order_by: [{ field: "count", direction: "desc" }],
    limit: 20,
  };

  const requestData: QueryRequest = {
    ...baseRequest,
    ...(process_name ? { where: { process_name } } : {}),
  };

  const {queryRecord, loading, error} = useQueryRecord<QueryResult<T>>(requestData);

  if (loading) return <DrawExcept loading={true} error={null}/>;
  if (error) return <DrawExcept loading={false} error={error} />;

  return (
    <div className="vispanel space-y-4">
    <ReturnProcessName aggcolumn={aggcolumn} process_name={process_name} vtype={"graph"} />
      <BarChart width={300} height={200} data={queryRecord}>
        <XAxis dataKey={aggcolumn} tickFormatter={(value: string) => {
          if (["day", "week", "month"].includes(aggcolumn)) {
            const datePart = value.split("T")[0]; // "2025-05-19"
            const [, month, day] = datePart.split("-"); // ["2025", "05", "19"]
            return `${month}/${day}`; // "05/19"
          }
          return value;
        }} />
        <YAxis />
          <Tooltip
            labelFormatter={(value: string) => {
              if (["day", "week", "month"].includes(aggcolumn)) {
                const vkPart = value.replace(/^VK_/, "");
                return vkPart;
              }
              return value;
            }}
            formatter={(value: number) => [`${value}回`]}
          />
        <Bar dataKey="count" style={{fill: "var(--color-graph)"}} />
      </BarChart>
    </div>
  );
}