// src/components/dashboard/BarChartSample.tsx
'use client'

import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'

const data = [
  { name: 'A', value: 12 },
  { name: 'B', value: 19 },
  { name: 'C', value: 3 },
  { name: 'D', value: 5 },
  { name: 'E', value: 2 },
  { name: 'F', value: 3 }
]

export default function BarChartSample() {
  return (
    <div className="w-full h-64 bg-white rounded shadow p-4">
      <h2 className="text-lg font-semibold mb-2">キーログ頻度（仮）</h2>
      <ResponsiveContainer width="100%" height="100%">
        <BarChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="name" />
          <YAxis />
          <Tooltip />
          <Legend />
          <Bar dataKey="value" fill="#3b82f6" />
        </BarChart>
      </ResponsiveContainer>
    </div>
  )
}
