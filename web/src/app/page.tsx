'use client';

import App from "@/src/components/App"

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
}
