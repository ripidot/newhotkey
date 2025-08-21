// src/components/panels/HeatmapView.tsx
import { motion, useMotionValue, useTransform, animate } from "framer-motion";
import { useEffect, useState} from "react";

export function HeatmapView() {
  const [count, setCount] = useState<number | null>(null);

  useEffect(() => {
    // API からデータ取得
    fetch("http://localhost:8000/countall")
      .then((res) => res.json())        // JSONレスポンスを想定（{"count": 123}）
      .then((data) => setCount(data.count))
      .catch((err) => {
        console.error("API error:", err);
      });
  }, []);

  return (
    <div className="p-4 bg-[#ffffff00]">
      {count !== null ? (
        <Gauge value={count} max={count} />
      ) : (
        <p>Loading...</p>
      )}
    </div>
  );
}

export default function Gauge({ value, max }: { value: number; max: number }) {
  const numberProgress = useMotionValue(0);
  const gaugeProgress = useMotionValue(0);

  // 表示用（整数に丸め）
  const displayValue = useTransform(numberProgress, (p) => Math.round(p));

  const radius = 80;
  const circumference = 2 * Math.PI * radius;

  // strokeDashoffset 用に MotionValue を作っておくと型が安定します
  const dashoffset = useTransform(gaugeProgress, (p) =>
    circumference - (p / Math.max(1, max)) * circumference
  );

  useEffect(() => {

    // 現在値から目的地へ数値アニメーションして、onUpdateでMotionValueへ反映
    const stopNumber = animate(numberProgress.get(), value, {
      duration: 1.5,
      ease: [0.22, 1, 0.36, 1], // easeOutCubic相当（最初速く→最後ゆっくり）
      onUpdate: (v) => numberProgress.set(v),
    });

    const stopGauge = animate(gaugeProgress.get(), value, {
      duration: 2.0,             // 数値より遅らせて「追いつく」演出
      ease: [0.16, 1, 0.3, 1],   // easeOutExpo風
      onUpdate: (v) => gaugeProgress.set(v),
    });

    return () => {
      stopNumber.stop();
      stopGauge.stop();
    };
  }, [value]);

return (
    <svg width="400" height="400" viewBox="0 0 200 200">
      <circle
        cx="100"
        cy="100"
        r={radius}
        stroke="#eee0"
        strokeWidth="10"
        fill="transparent"
      />
      <motion.circle
        cx="100"
        cy="100"
        r={radius}
        stroke="#6666ff"
        strokeWidth="10"
        fill="transparent"
        strokeDasharray={circumference}
        strokeDashoffset={dashoffset}
        transform="rotate(-90 100 100)" // 上(12時)から時計回りに
      />
      <motion.text
        x="100"
        y="110"
        textAnchor="middle"
        fontSize="24"
        fill="#333"
      >
        {displayValue}
      </motion.text>
    </svg>
  );
}