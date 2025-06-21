'use client'

import React from 'react'
import { Card, CardContent } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs";
import { BarChart2, PieChart, Calendar } from "lucide-react";
import BarChartSample from "@/components/barchartsample/BarChartSample"

export default function Dashboard() {
  return (
    <div className="flex h-screen w-full">
      {/* Sidebar */}
      <aside className="w-64 bg-gray-900 text-white p-4 space-y-4">
        <h1 className="text-2xl font-bold">KeyLogDash</h1>
        <nav className="space-y-2">
          <Button variant="ghost" className="w-full justify-start">
            <BarChart2 className="mr-2 h-4 w-4" /> 日別統計
          </Button>
          <Button variant="ghost" className="w-full justify-start">
            <PieChart className="mr-2 h-4 w-4" /> キー別統計
          </Button>
          <Button variant="ghost" className="w-full justify-start">
            <Calendar className="mr-2 h-4 w-4" /> アプリ別統計
          </Button>
        </nav>
      </aside>

      {/* Main content */}
      <main className="flex-1 bg-gray-100 p-6 overflow-auto">
        <h2 className="text-xl font-semibold mb-4">ダッシュボード</h2>
        <Tabs defaultValue="daily" className="space-y-4">
          <TabsList>
            <TabsTrigger value="daily">日別</TabsTrigger>
            <TabsTrigger value="keys">キー別</TabsTrigger>
            <TabsTrigger value="apps">アプリ別</TabsTrigger>
          </TabsList>

          <TabsContent value="daily">
            <Card>
              <CardContent className="p-4">[ここに折れ線グラフ]</CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="keys">
            <Card>
              <CardContent className="p-4">[ここに円グラフ]</CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="apps">
            <Card>
              <CardContent className="p-4">
                <BarChartSample />
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>
      </main>
    </div>
  );
}