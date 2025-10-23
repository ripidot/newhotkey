import { useEffect, useState } from "react";
import type { QueryRequest } from "@/src/types/interface";

export function useQueryRecord<T>(requestData: QueryRequest) {
  const [queryRecord, setQueryRecord] = useState<T[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<unknown>(null);

  useEffect(() => {
    console.log("in useEffect");
    // 非同期関数をuseEffect内で定義
    async function fetchData() {
      setLoading(true);
      setError(null);

      try {
        const response = await fetch("https://newhotkey.onrender.com/logs/search", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(requestData),
        });

        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }

        const jresponse = await response.json();
        setQueryRecord(jresponse.data);
      } catch (err: unknown) {
        setError(err);
      } finally {
        setLoading(false);
      }
    }

    fetchData();
  }, []);

  return { queryRecord, loading, error };
}
