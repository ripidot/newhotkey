// src/components/panels/KeyTimeline.tsx
import { BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";
import { useEffect, useState } from "react";
import type { QueryRequest, QueryRecord} from "@/src/types/interface";

export function KeyTimeline() {
  return (
    <div>
      <p><Graph/></p>
    </div>
  );
}

Chart.register(ArcElement, ChartTooltip);

export function Graph() {
  const [queryData, setQueryData] = useState<QueryRecord[]>([]);
  const program_name = "Explorer.EXE";
  useEffect(() => {
  const fetchData = async () => {
    const requestData: QueryRequest = {
      select: ["key"],
      where: { process_name: program_name },
      group_by: ["key"],
      aggregates: [{ func: "count", alias: "count" }],
      order_by: [{ field: "count", direction: "desc" }],
      limit: 20,
    };

    try {
      const response = await fetch("http://localhost:8000/postall", {
        method: "POST",                 // POSTで送信
        headers: {
          "Content-Type": "application/json", // JSONで送信することを明示
        },
        body: JSON.stringify(requestData),    // オブジェクトをJSON文字列に変換
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      setQueryData(data.results);
      console.log("args : ", data.results)
      console.log("querydata: ", queryData)
    } catch (error) {
      console.error("送信エラー:", error);
    }
  };
    fetchData();
  }, []);
  
  if (queryData.length === 0) {
    return <p>Loading...</p>;
  }
  return (
    <div className="testbox space-y-4">
      {program_name? 
      (<p>集計プロセス名: {program_name}</p>)
      : (<p>集計プロセス名: 全て</p>)}
      <BarChart width={300} height={200} data={queryData}>
        <XAxis dataKey="key" />
        <YAxis />
        <Tooltip />
        <Bar dataKey="count" style={{fill: "var(--color-graph)"}} />
      </BarChart>
    </div>
  );
}