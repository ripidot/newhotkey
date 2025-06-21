'use client';

import Dashboard from "@/components/dashboard/Dashboard"
import App from "@/components/app/App"

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
  return <App />;
  // return <Dashboard />;
  // const [logs, setLogs] = useState<KeyLogEntry[]>([]);
  // useEffect(() => {
  //   const fetchLogs = async () => {
  //     const res = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/keylogs`);
  //     const data = await res.json();
  //     console.log('data: ', data);
  //     setLogs(data);
  //   };
  //   fetchLogs();
  // }, []);

  // return (
  //   <main>
  //     <h1 className="text-xl font-bold mb-4">アメss一覧</h1>
  //     <ul>
  //       {logs.map(log => (
  //         <li key={log.id}>
  //           [{log.timestamp}] {log.key}
  //         </li>
  //       ))}
  //     </ul>
  //   </main>
  // );
}
