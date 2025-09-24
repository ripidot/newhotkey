import { useEffect, useState} from "react";
import type {QueryRequest, QueryRecord} from "@/src/types/interface";

export function useQueryData(program_name: string) {
  const [queryData, setQueryData] = useState<QueryRecord[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    // 非同期関数をuseEffect内で定義
    async function fetchData() {
      setLoading(true);
      setError(null);

      const requestData: QueryRequest = program_name
        ? {
            select: ["count"],
            where: { process_name: program_name },
            aggregates: [{ func: "count", alias: "count" }],
            order_by: [{ field: "count", direction: "desc" }],
          }
        : {
            select: ["count"],
            aggregates: [{ func: "count", alias: "count" }],
            order_by: [{ field: "count", direction: "desc" }],
          };

      try {
        const response = await fetch("http://localhost:8000/postall", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(requestData),
        });

        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }

        const data = await response.json();
        setQueryData(data.results);
      } catch (err: any) {
        setError(err);
      } finally {
        setLoading(false);
      }
    }

    fetchData();
  }, [program_name]); // program_name が変わるたびに再取得

  return { queryData, loading, error };
}
