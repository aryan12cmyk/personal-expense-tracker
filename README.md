# Hisaab Web

A student expense tracker as a website. **C backend** (own HTTP server, no frameworks) + **MySQL** storage + one HTML page.

Paste a bank payment SMS (HDFC, SBI, Kotak wording) or type `spent 50 on chai`. Hisaab logs it, files it under a category, warns when a budget crosses 80% / 100%, and shows **safe-to-spend today** and a **Money Health score**. OTP, failed-payment and money-received texts are ignored.

## Folder

| File | What it is |
|---|---|
| `hisaab_web.c` | Server, parser, categoriser, alerts, MySQL layer, self-test |
| `public/index.html` | The website (dashboard, SMS box, budgets, payments table) |
| `schema.sql` | Table definitions (optional: the server creates them itself) |
| `Makefile`, `build.bat` | Linux/macOS and Windows builds |

## Run it with MySQL

1. Install MySQL (or MariaDB) and the client library.
   - Linux: `sudo apt install libmysqlclient-dev` (or `libmariadb-dev`)
   - Windows: install MySQL Server plus **MySQL Connector/C**
2. Build:
   - Linux/macOS/MSYS2: `make`
   - Windows (MinGW gcc): `set MYSQL_DIR=C:\path\to\connector` then `build.bat`
3. Tell it how to log in (defaults: host `127.0.0.1`, user `root`, empty password, database `hisaab`):
   ```
   export HISAAB_DB_USER=root
   export HISAAB_DB_PASS=yourpassword
   ./hisaab_web            # Windows: set HISAAB_DB_PASS=... then hisaab_web.exe
   ```
   Other settings: `HISAAB_DB_HOST`, `HISAAB_DB_PORT`, `HISAAB_DB_NAME`.
4. Open http://127.0.0.1:8080 (different port: `./hisaab_web 8081`).

The database and tables are created automatically on first start.

## Run it without MySQL (quick demo)

`make nodb && ./hisaab_web_nodb` (Windows: `build.bat nodb`). Same website, but data lives in memory and is lost when you stop it. `make test` runs the self-test (58 checks).

## 90-second demo

1. Click **Load demo data**. Food sits at 72% of its budget.
2. Click the **HDFC** sample, then **Add from SMS**. Food jumps past 80% and the early-warning banner appears.
3. Add the **SBI** and **Kotak** samples. They log normally, with no repeat warning.
4. Add the **OTP** and **Failed** samples. Both are ignored, and the reason is shown.
5. Quick add `spent 50 on chai`. It is logged as a cash payment under Food.
6. Change a payment's category in the table. That merchant is remembered from then on.

## How the numbers work

- **Safe to spend today** = (total budget - spent this month) / days left, today included.
- **Money Health (0-100)** = 50 for pace (spending vs how far through the month you are) + 30 for category discipline (-5 per category past 80%, -10 per category past 100%) + 20 for logging habit (full marks if you log on about 60% of days).
- Default monthly budgets: Food 4000, Travel 2500, Shopping 3000, Bills 2000, Entertainment 1000, Health 1000, Other 500. Click any budget in the page to change it.

## API

`GET /api/status`, `GET /api/payments`, `GET /api/export` (CSV);
`POST /api/alert {"text"}`, `/api/quick {"text"}`, `/api/fix {"id","category"}`, `/api/budget {"category","amount"}`, `/api/seed`, `/api/reset`.
POSTs must send `Content-Type: application/json`.

## Honest limits (judges will ask)

- It cannot read your SMS by itself. You paste them in. Automatic reading would need an Android app.
- No login. The server listens on `127.0.0.1` only, so only your own computer can open it. Do not set `HISAAB_BIND=0.0.0.0` on a shared network.
- Data is not encrypted at rest, and the MySQL password goes in an environment variable.
- The SMS parser is rule-based and covers the common HDFC / SBI / Kotak / UPI wordings. An unusual bank format may be ignored or get a wrong merchant name; fix the category in the table and it learns.
- One request at a time (single-threaded), which is fine for one user and a demo.
