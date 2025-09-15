// src/components/panels/Counter.tsx
import { motion, useMotionValue, useTransform, animate } from "framer-motion";
import { useEffect, useState} from "react";


export function Counter() {
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

  //// カウンタ

  // 数値
  // const displayValue = useTransform(numberProgress, (p) => Math.round(p).toLocaleString("jp-JP"));
  const displayValue = useTransform(numberProgress, (p) => (Math.round(p) * 10 + 234567).toLocaleString("jp-JP"));
  const keywhite = "#f8f8f8";

  // 中心座標
  const countercx = 110;
  const countercy = 110;

  // 円エフェクト
  const countereffectradius = 103;
  const countereffectwidth = 0.5;
  const countereffectratio = 0.9;
  const effectcircumference = 2 * Math.PI * countereffectradius * countereffectratio;
  const counteffectdashoffset = useTransform(gaugeProgress, (p) =>
    effectcircumference - (p / Math.max(1, max)) * effectcircumference
  );
  const effectduration = 6;

  const countereffectradius2 = 110;
  const countereffectwidth2 = 1;
  const countereffectratio2 = 0.9;
  const effectcircumference2 = 2 * Math.PI * countereffectradius2 * countereffectratio2;
  const counteffectdashoffset2 = useTransform(gaugeProgress, (p) =>
    effectcircumference2 - (p / Math.max(1, max)) * effectcircumference2
  );
  const effectduration2 = 2.7;
  
  const countereffectradius3 = 80;
  const countereffectwidth3 = 5;
  const countereffectratio3 = 0.9;
  const effectcircumference3 = 2 * Math.PI * countereffectradius3 * countereffectratio3;
  const counteffectdashoffset3 = useTransform(gaugeProgress, (p) =>
    effectcircumference3 - (p / Math.max(1, max)) * effectcircumference3
  );
  const effectduration3 = 2.5;

  // 円
  const countercircleradius = 80;
  const countercirclewidth = 5;
  const circumference = 2 * Math.PI * countercircleradius;
  const dashoffset = useTransform(gaugeProgress, (p) =>
    circumference - (p / Math.max(1, max)) * circumference
  );

  // テキスト
  const countertextfontsize = 32;
  const countertextheight = 10;
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
    <svg width="600" height="600" viewBox="0 0 300 300">
      <motion.circle // effect
        cx={countercx}
        cy={countercy}
        r={countereffectradius}
        // stroke={keywhite}
        strokeWidth={countereffectwidth}
        fill="transparent"
        strokeDasharray={effectcircumference}
        strokeDashoffset="0"
        animate={{ rotate: 360 }}
        transition={{
          repeat: Infinity,
          ease: "linear",  // 等速にするため
          duration: effectduration     // 1周にかける秒数
        }}
        style={{ originX: "50%", originY: "50%" , stroke: "var(--color-secondary)"}} // 中心で回転
      />
      <motion.circle // effect
        cx={countercx}
        cy={countercy}
        r={countereffectradius2}
        // stroke={keywhite}
        strokeWidth={countereffectwidth2}
        fill="transparent"
        strokeDasharray={effectcircumference2}
        strokeDashoffset="0"
        animate={{ rotate: 360 }}
        transition={{
          repeat: Infinity,
          ease: "linear",  // 等速にするため
          duration: effectduration2      // 1周にかける秒数
        }}
        style={{ originX: "50%", originY: "50%"  , stroke: "var(--color-secondary)"}} // 中心で回転
      />

      <motion.circle // effect
        cx={countercx}
        cy={countercy}
        r={countereffectradius3}
        strokeWidth={countereffectwidth3}
        fill="transparent"
        strokeDasharray={effectcircumference3}
        strokeDashoffset="0"
        animate={{ rotate: 360 }}
        transition={{
          repeat: Infinity,
          ease: "linear",  // 等速にするため
          duration: effectduration3      // 1周にかける秒数
        }}
        style={{ originX: "50%", originY: "50%"  , stroke: "var(--color-secondary)"}} // 中心で回転
      />

      <circle // shadow
        cx={countercx}
        cy={countercy}
        r={countercircleradius}
        stroke="#0000"
        strokeWidth={countercirclewidth}
        fill="transparent"
      />
      
      {/* <motion.circle // main
        cx={countercx}
        cy={countercy}
        r={countercircleradius}
        stroke={keywhite}
        strokeWidth={countercirclewidth}
        fill="transparent"
        strokeDasharray={circumference}
        strokeDashoffset={dashoffset}
        transform={`rotate(-90 ${countercx} ${countercy})`} // 上(12時)から時計回りに
      /> */}
        <motion.text
          style={{ fontFamily: "var(--font-sans)" , fill: "var(--color-secondary)"}}
          x={countercx}
          y={countercy + countertextheight}
          fontSize={countertextfontsize}
          textAnchor="middle"
        >
          {displayValue}
        </motion.text>
    </svg>
  );
}