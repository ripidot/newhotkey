'use client';

import { useEffect, useState } from 'react';

type LogEntry = {
  id: number;
  message: string;
  timestamp: string;
};

type KeyLogEntry = {
    id: number;
    session_id: string;
    sequence_id: number;
    timestamp: string;
    key: string;
    modifiers: string;
    window_title: string;
    process_name: string;
    user_id: number;
};

export default function Page() {
  // const [logs, setLogs] = useState<LogEntry[]>([]);
  const [logs, setLogs] = useState<KeyLogEntry[]>([]);

  useEffect(() => {
    const fetchLogs = async () => {
      // const res = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/logs`);
      const res = await fetch(`http://localhost:8000/keylogs`);
      // const res = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/keylogs`);
      const data = await res.json();
      console.log('data: ', data);
      setLogs(data);
    };
    fetchLogs();
  }, []);

  return (
    <main>
      <h1 className="text-xl font-bold mb-4">ロ一覧</h1>
      <ul>
        {logs.map(log => (
          <li key={log.id}>
            [{log.timestamp}] {log.key}
          </li>
        ))}
      </ul>
    </main>
  );
}
