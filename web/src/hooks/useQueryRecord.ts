import { useEffect, useState } from "react";
import type { ApiResponse, QueryRequest } from "@/src/types/interface";

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
        const API_URL = process.env.NEXT_PUBLIC_API_URL;
        console.log("API_URL in runtime:", API_URL);
        
        const response = await fetch(`${API_URL}/logs/search`, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(requestData),
        });

        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }

        const jresponse: ApiResponse<T> = await response.json();
        // const jresponse = await response.json();
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
