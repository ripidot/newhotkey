// src/components/panels/KeyTimeline.tsx
import { BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";

export function KeyTimeline() {
  return (
    <div className="p-4 grid grid-cols-2 bg-[#ffffff00]">
      {/* <p className="text-xl font-semibold">Key Frequency</p> */}
      <p><Graph/></p>
    </div>
  );
}

Chart.register(ArcElement, ChartTooltip);

// Recharts用データ
const barData = [
  { name: "A", count: 20 },
  { name: "B", count: 50 },
  { name: "C", count: 40 },
  { name: "D", count: 30 },
  { name: "E", count: 60 },
];

export default function Graph() {
  return (
    <BarChart width={400} height={300} data={barData}>
      <XAxis dataKey="name" />
      <YAxis />
      <Tooltip />
      <Bar dataKey="count" fill="#8884d8" />
    </BarChart>
  );
}