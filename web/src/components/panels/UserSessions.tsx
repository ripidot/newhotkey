// src/components/panels/KeyTimeline.tsx
import { LineChart, Line, XAxis, YAxis, Tooltip } from "recharts";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";
import { useEffect, useState } from "react";
export function UserSessions() {
  return (
    <div className="p-4 grid grid-cols-2 bg-[#ffffff00]">
      {/* <p className="text-xl font-semibold">Key Frequency</p> */}
      <p><Graph/></p>
    </div>
  );
}

Chart.register(ArcElement, ChartTooltip);


// 集約関数の型
interface Aggregate {
  func: string;  // "count" など
  alias: string;
}

// order_by の型
interface OrderBy {
  field: string;
  direction: "asc" | "desc";
}

// 全体のリクエスト型
interface QueryRequest {
  select: string[];
  where?: Record<string, string>;  // 今回は process_name: string の形
  group_by?: string[];
  aggregates?: Aggregate[];
  order_by?: OrderBy[];
  limit?: number;
}

interface QueryRecord {
  key: string;
  count: number;
}
export default function Graph() {
  const [queryData, setQueryData] = useState<QueryRecord[]>([]);
  useEffect(() => {
  const fetchData = async () => {
    const requestData: QueryRequest = {
      select: ["week"],
      where: { process_name: "Explorer.EXE" },
      group_by: ["week"],
      aggregates: [{ func: "count", alias: "count" }],
      order_by: [{ field: "week", direction: "asc" }],
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
    } catch (error) {
      console.error("送信エラー:", error);
    }
  };
    fetchData();
  }, []);

  return (
    <LineChart width={400} height={300} data={queryData}>
      <XAxis dataKey="week" />
      <YAxis />
      <Tooltip />
      <Line type="linear" dataKey="count" stroke="#8884d8" />
    </LineChart>
  );
}