// src/components/panels/KeyTimeline.tsx
import { BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";
import { useEffect, useState } from "react";
export function KeyTimeline() {
  return (
    <div className="p-4 grid grid-cols-2 bg-[#ffffff00]">
      {/* <p className="text-xl font-semibold">Key Frequency</p> */}
      <p><Graph/></p>
    </div>
  );
}

Chart.register(ArcElement, ChartTooltip);

// Recharts用データ
const barData = [
  { name: "A", count: 20 },
  { name: "B", count: 50 },
  { name: "C", count: 40 },
  { name: "D", count: 30 },
  { name: "E", count: 60 },
];
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

// レスポンス型はAPI仕様に応じて定義
interface QueryResponse {
  // 例: レコードの配列
  records: Record<string, any>[];
}

// {
//   "select": [
//     "key"
//   ],
//   "where": {"process_name" : "Explorer.EXE"},
//   "group_by": [
//     "key"
//   ],
//   "aggregates": [
//     {"func": "count", "alias": "cnt"},
//   ],
//   "order_by": [
//     {
//       "field": "count",
//       "direction": "desc"
//     }
//   ],
//   "limit":10
// }
interface QueryRecord {
  key: string;
  count: number;
}
export default function Graph() {
  const [queryData, setQueryData] = useState<QueryRecord[]>([]);
  useEffect(() => {
  const fetchData = async () => {
    const requestData: QueryRequest = {
      select: ["key"],
      where: { process_name: "Explorer.EXE" },
      group_by: ["key"],
      aggregates: [{ func: "count", alias: "count" }],
      order_by: [{ field: "count", direction: "desc" }],
      limit: 5,
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
      // 必要ならステートに保存
      // setResult(data.result);
    } catch (error) {
      console.error("送信エラー:", error);
    }
  };
    fetchData();
  }, []);
  if (queryData != null){console.log("qdata: ", queryData);}

  console.log("bardata: ", barData);
  return (
    <BarChart width={400} height={300} data={queryData}>
      <XAxis dataKey="key" />
      <YAxis />
      <Tooltip />
      <Bar dataKey="count" fill="#8884d8" />
    </BarChart>
  );
}