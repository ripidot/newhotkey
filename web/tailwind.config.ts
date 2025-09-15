import { Exo_2, Noto_Sans } from "next/font/google";

// web/tailwind.config.js
module.exports = {
  content: [
    "./app/**/*.{js,ts,jsx,tsx}",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors:{
        brand:{
          primary: '#a0d9f8',
          white10: '#f8f8f8',
          dark: '#333',
        }
      },
      fontFamily: {
        sans: ['ui-sans-serif', 'system-ui'], // 標準のサンセリフ
        serif: ['ui-serif', 'Georgia'],       // 標準のセリフ
        mono: ['ui-monospace', 'SFMono-Regular'], // 標準の等幅
        orbitron: ["var(--font-orbitron)", "sans-serif"],
        audiowide: ["var(--font-audiowide)", "sans-serif"], // 第二引数:fallback
        exo_2: ["var(--font-exo_2)", "sans-serif"],
        inter: ["var(--font-inter)", "sans-serif"],
        Noto_Sans: ["var(--font-noto_sans)", "sans-serif"],
      },
    },
  },
  plugins: [],
};
