"use client";

const OPTIONS = [
  { w: 400, h: 300, zh: '4.2"', en: '4.2"' },
  { w: 296, h: 128, zh: '2.9"', en: '2.9"' },
  { w: 648, h: 480, zh: '5.83"', en: '5.83"' },
  { w: 640, h: 384, zh: '7.5" 640×384', en: '7.5" 640×384' },
  { w: 800, h: 480, zh: '7.5" 800×480', en: '7.5" 800×480' },
] as const;

interface ScreenSizeSelectProps {
  width: number;
  height: number;
  onChange: (w: number, h: number) => void;
  tr: (zh: string, en: string) => string;
}

export function ScreenSizeSelect({ width, height, onChange, tr }: ScreenSizeSelectProps) {
  return (
    <div className="flex" title={tr("屏幕尺寸", "Screen size")}>
      {OPTIONS.map((o, i) => {
        const active = o.w === width && o.h === height;
        return (
          <button
            key={o.w + "x" + o.h}
            type="button"
            onClick={() => onChange(o.w, o.h)}
            className={`px-2 py-1 text-xs border border-ink/20 transition-colors ${
              i === 0 ? "rounded-l-sm" : ""
            }${i === OPTIONS.length - 1 ? "rounded-r-sm" : ""
            }${i > 0 ? " -ml-px" : ""
            } ${active
              ? "bg-ink text-white border-ink z-10 relative"
              : "bg-white text-ink hover:bg-ink/5"
            }`}
          >
            {tr(o.zh, o.en)}
          </button>
        );
      })}
    </div>
  );
}
