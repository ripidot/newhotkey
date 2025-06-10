import { Line } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  LineElement,
  CategoryScale,
  LinearScale,
  PointElement,
  Tooltip,
  Legend,
} from 'chart.js';

ChartJS.register(LineElement, CategoryScale, LinearScale, PointElement, Tooltip, Legend);

export default function KeyLogChart({ data }: { data: { timestamp: string; count: number }[] }) {
  return (
    <Line
      data={{
        labels: data.map((d) => d.timestamp),
        datasets: [
          {
            label: 'Key Logs per Minute',
            data: data.map((d) => d.count),
            borderColor: 'rgba(75,192,192,1)',
            backgroundColor: 'rgba(75,192,192,0.2)',
            fill: true,
          },
        ],
      }}
    />
  );
}
