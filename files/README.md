# files/ — store showcase media

This directory holds **display media only** (never used for installation —
the machine-facing artifact is the root `plugins.lvx` container). A future
software store reads these to render the listing page.

Conventions (referenced from `../store.json` per app):

| path | purpose |
|---|---|
| `icon.png` | app icon, square PNG, 256×256+ |
| `screenshots/*.png` | showcase screenshots (PNG, ≤2MB each) |
| `demo.mp4` | optional promo video (H.264/MP4) |
| `*.webp` | lightweight alternate images |

Add real assets before publishing the store; until then the JSON entries
above simply reference the intended names.
