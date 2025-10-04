// src/components/panels/Counter.tsx

import { useEffect, useMemo } from "react";
import type { QueryRequest } from "@/src/types/interface";
import { motion, useMotionValue, useTransform, animate } from "framer-motion";
import { useQueryRecord } from "@/src/hooks/useQueryRecord";
import type { CircleAnimation, CircleText} from "@/src/types/interface";
import { ReturnProcessName, DrawExcept } from "@/src/lib/utils";
import type { QueryRecordKey, QueryResult } from "@/src/types/interface";

// CounterGraphをファイル分割しているが、これは再生成を防ぐため
// react-mosaicがリサイズを検知すると
// Counter内で定義されている関数を再定義する(再生成)

// そのため、リサイズをすると、useEffectの依存配列を設定せずとも
// CounterGraphが再生成されてしまう

// Counterの外であればいいため、このファイルで外に定義してもいいが、
// 可読性のために分割している
// Counterの外に出すことで再生成を再レンダーに落ち着かせている

// 再レンダー : 仮想DOMの生成
// 再生成 : アンマウント, マウント

export function Counter({
  process_name, aggcolumn
}: {
  process_name: string;
  aggcolumn: QueryRecordKey;
}) {

  return (
    <div>
      <p><CounterGraph process_name={process_name} aggcolumn={aggcolumn}/></p>
    </div>
  );
}

export function CounterGraph<T extends QueryRecordKey>({ process_name, aggcolumn }: { process_name: string; aggcolumn: T }) {
    const baseRequest: QueryRequest = {
        select: [aggcolumn],
        aggregates: [{ func: "count", alias: "cnt" }],
    };
    const requestData: QueryRequest = {
        ...baseRequest,
        ...(process_name && { where: { process_name } }),
    };

    const {queryRecord, loading, error} = useQueryRecord<{ count: number; cnt: number }>(requestData);
    if (loading || error)
        return <DrawExcept loading={loading} error={error}/>

    return (
        <div className="testbox space-y-4">
        <ReturnProcessName aggcolumn={aggcolumn} process_name={process_name} vtype={"counter"} />

        {queryRecord[0].count !== null ? (
            <Gauge value={queryRecord[0].count} max={queryRecord[0].count} />
        ) : (
            <p>データを表示できません</p>
        )}
        </div>
    );
}


function Gauge({ value, max }: { value: number; max: number }) {
    //// カウンタ
    const numberProgress = useMotionValue(0);
    const gaugeProgress = useMotionValue(0);

    // 中心座標
    const countercx = 110;
    const countercy = 110;

    // 数値
    const displayValue = useTransform(numberProgress, (p) => Math.round(p).toLocaleString("jp-JP"));
    // const displayValue = useTransform(numberProgress, (p) => (Math.round(p) * 10 + 234567).toLocaleString("jp-JP"));

    // テキスト
    const countertextfontsize = 28;
    const countertextheight = 10;
    const e1 = createCircleAnimation(countercx, countercy, 80, 3, 0.9, 2.5);
    const e2 = createCircleAnimation(countercx, countercy, 103, 0.5, 0.9, 6);
    const e3 = createCircleAnimation(countercx, countercy, 110, 1.5, 0.9, 2.7);
    const c1 : CircleText = {
        cx : countercx, cy : countercy,
        height : countertextheight,
        size : countertextfontsize, displayValue : displayValue
    };

    useEffect(() => {
        // 現在値から目的地へ数値アニメーションして、onUpdateでMotionValueへ反映
        const stopNumber = animate(numberProgress.get(), value, {
        duration: 1.5,
        ease: [0.22, 1, 0.36, 1],
        onUpdate: (v) => numberProgress.set(v),
        });

        const stopGauge = animate(gaugeProgress.get(), value, {
        duration: 2.0,
        ease: [0.16, 1, 0.3, 1],
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
        className="countercircle"
        cx={props.cx}
        cy={props.cy}
        r={props.radius}
        strokeWidth={props.width}
        strokeDasharray={props.circumference}
        strokeDashoffset="0"
        animate={{ rotate: 360 }}
        transition={{
            repeat: Infinity,
            ease: "linear",  // 等速にするため
            duration: props.duration     // 1周にかける秒数
        }}
        />
    )
}
function WriteCircleText(props : CircleText) {
    return (
    <motion.text
        className = "countertext"
        x={props.cx}
        y={props.cy + props.height}
        textAnchor="middle"
    >
        {props.displayValue}
    </motion.text>
    )
}