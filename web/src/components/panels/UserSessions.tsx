// src/components/panels/UserSessions.tsx
import { useEffect, useState } from "react";

export function UserSessions() {
  return (
    <div className="p-4 bg-[#ff00ff]">
      <p className="text-lg font-bold mb-2">UserSessions</p>
      <p>
        <CountAllComponent />
      </p>
    </div>
  );
}
type CountResponse = {
  count: number; // 例: APIが {"count": 123} を返す場合
};

export default function CountAllComponent() {
  const [count, setCount] = useState<number | null>(null);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch("http://localhost:8000/countall");
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data: CountResponse = await response.json();
        setCount(data["count"]);
      } catch (error) {
        console.error("データ取得エラー:", error);
      }
    };

    fetchData();
  }, []);

  if (count === null) {
    return <>Loading...</>;
  }
  return <>Count: {count}</>;
}
