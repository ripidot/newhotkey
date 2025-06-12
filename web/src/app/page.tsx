'use client';

import { useEffect, useState } from 'react';

type LogEntry = {
  id: number;
  message: string;
  timestamp: string;
};

export default function Page() {
  const [logs, setLogs] = useState<LogEntry[]>([]);

  useEffect(() => {
    const fetchLogs = async () => {
      const res = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/logs`);
      const data = await res.json();
      setLogs(data);
    };
    fetchLogs();
  }, []);

  return (
    <main>
      <h1 className="text-xl font-bold mb-4">ログ一覧</h1>
      <ul>
        {logs.map(log => (
          <li key={log.id}>
            [{log.timestamp}] {log.message}
          </li>
        ))}
      </ul>
    </main>
  );
}
