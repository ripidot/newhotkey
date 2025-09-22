// Chart.js用データ
import { Pie } from "react-chartjs-2";
import { Chart, ArcElement, Tooltip as ChartTooltip } from "chart.js";

Chart.register(ArcElement, ChartTooltip);

export function CircleGraph() {
  return (
    <div>
    <p className="text-xl font-semibold">Modifier Usage</p>
      <Graph/>
    </div>
  );
}

const pieData = {
  labels: ["Ctrl", "Alt", "Shift"],
  datasets: [
    {
      label: "Modifier Keys",
      data: [12, 29, 7],
      backgroundColor: ["#36A2EB", "#FF6384", "#FFCE56"],
    },
  ],
};
export default function Graph() {
  return (
    <Pie data={pieData} />
  );
}