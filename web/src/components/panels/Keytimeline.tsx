// src/components/panels/KeyTimeline.tsx
import { BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";
import { Pie } from "react-chartjs-2";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";

export function KeyTimeline() {
  return (
    <div className="p-4 bg-white">
      <p><Graph/></p>
    </div>
  );
}

Chart.register(ArcElement, ChartTooltip);

// Recharts用データ
const barData = [
  { name: "A", count: 30 },
  { name: "B", count: 50 },
  { name: "C", count: 40 },
  { name: "D", count: 30 },
  { name: "E", count: 60 },
];

// Chart.js用データ
const pieData = {
  labels: ["Ctrl", "Alt", "Shift"],
  datasets: [
    {
      label: "Modifier Keys",
      data: [12, 19, 7],
      backgroundColor: ["#36A2EB", "#FF6384", "#FFCE56"],
    },
  ],
};

export default function Graph() {
  return (
    <div className="grid grid-cols-2 gap-8">
      <div>
        <h2 className="text-xl font-semibold">Key Frequency</h2>
        <BarChart width={400} height={300} data={barData}>
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Bar dataKey="count" fill="#8884d8" />
        </BarChart>
      </div>

      <div>
        <h2 className="text-xl font-semibold">Modifier Usage</h2>
        <Pie data={pieData} />
      </div>
    </div>
  );
}