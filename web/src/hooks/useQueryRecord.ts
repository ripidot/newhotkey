import { useEffect, useState } from "react";
import type { QueryRequest } from "@/src/types/interface";

export function useQueryRecord<T>(requestData: QueryRequest) {
  const [queryRecord, setQueryRecord] = useState<T[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    console.log("in useEffect");
    // 非同期関数をuseEffect内で定義
    async function fetchData() {
      setLoading(true);
      setError(null);

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
        setQueryRecord(data.results);
      } catch (err: any) {
        setError(err);
      } finally {
        setLoading(false);
      }
    }

    fetchData();
  }, []);

  return { queryRecord, loading, error };
}
