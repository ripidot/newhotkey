import KeyLogChart from '@/components/KeyLogChart';

export default function Home() {
  // 仮データ（FastAPIからfetchするように後で置き換え）
  const sampleData = [
    { timestamp: '10:00', count: 5 },
    { timestamp: '10:01', count: 8 },
    { timestamp: '10:02', count: 4 },
    { timestamp: '10:03', count: 12 },
  ];

  return (
    <main>
      <h1>キーログ可視化</h1>
      <KeyLogChart data={sampleData} />
    </main>
  );
}
