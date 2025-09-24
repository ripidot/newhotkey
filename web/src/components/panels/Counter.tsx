// src/components/panels/Counter.tsx
import { motion, useMotionValue, useTransform, animate } from "framer-motion";
import { useEffect } from "react";
import { useQueryData } from "@/src/hooks/useQueryData";
import type { CircleAnimation, CircleText} from "@/src/types/interface";

export function Counter() {

  function Graph() {
    const program_name = "Explorer.EXE";
    const {queryData, loading, error} = useQueryData(program_name);

    if (loading) { // 非同期処理のためのロード中の処理
      return <p>Loading...</p>;
    }
    if (error) {
      return <p>Error: {error.message}</p>
    }
    return (
      <div className="testbox space-y-4">
        {program_name? 
        (<p>集計プロセス名: {program_name}</p>)
        : (<p>集計プロセス名: 全て</p>)}

        {queryData[0].count !== null ? (
          <Gauge value={queryData[0].count} max={queryData[0].count} />
        ) : (
          <p>データを表示できません</p>
        )}
      </div>
    );
  }

  function Gauge({ value, max }: { value: number; max: number }) {
    // 円エフェクト
    function createCircleAnimation(
      cx: number, cy: number,
      radius: number, width: number,
      ratio: number, duration: number): CircleAnimation {
        return{
          cx: cx,
          cy: cy,
          radius : radius,
          width : width,
          ratio : ratio,
          circumference: 2 * Math.PI * radius * ratio,
          duration : duration
        };
    }
    function DrawCircleAnimation(props : CircleAnimation){
      return(
          <motion.circle // effect
            cx={props.cx}
            cy={props.cy}
            r={props.radius}
            strokeWidth={props.width}
            fill="transparent"
            strokeDasharray={props.circumference}
            strokeDashoffset="0"
            animate={{ rotate: 360 }}
            transition={{
              repeat: Infinity,
              ease: "linear",  // 等速にするため
              duration: props.duration     // 1周にかける秒数
            }}
            style={{transformOrigin: "center center", stroke: "var(--color-secondary)"}} // 中心で回転
          />
      )
    }
    function WriteCircleText(props : CircleText) {
      return (
        <motion.text
          style={{ fontFamily: "var(--font-sans)" , fill: "var(--color-secondary)"}}
          x={props.cx}
          y={props.cy + props.height}
          fontSize={props.size}
          textAnchor="middle"
        >
          {props.displayValue}
        </motion.text>
      )
    }
    //// カウンタ
    const numberProgress = useMotionValue(0);
    const gaugeProgress = useMotionValue(0);

    // 中心座標
    const countercx = 110;
    const countercy = 110;

    // 数値
    // const displayValue = useTransform(numberProgress, (p) => Math.round(p).toLocaleString("jp-JP"));
    const displayValue = useTransform(numberProgress, (p) => (Math.round(p) * 10 + 234567).toLocaleString("jp-JP"));

    // テキスト
    const countertextfontsize = 28;
    const countertextheight = 10;
    const e1 = createCircleAnimation(countercx, countercy, 80, 3, 0.9, 2.5);
    const e2 = createCircleAnimation(countercx, countercy, 103, 0.5, 0.9, 6);
    const e3 = createCircleAnimation(countercx, countercy, 110, 1, 0.9, 2.7);
    const c1 : CircleText = {
      cx : countercx, cy : countercy,
      height : countertextheight,
      size : countertextfontsize, displayValue : displayValue
    };

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
      <svg width="400" height="400" viewBox="0 0 300 300">
        <DrawCircleAnimation {...e1}/>
        <DrawCircleAnimation {...e2}/>
        <DrawCircleAnimation {...e3}/>
        <WriteCircleText {...c1}/>
      </svg>
    );
  }
  
  return (
    <div>
      <p><Graph/></p>
    </div>
  );
}