#include "RemoteWeb.h"
#include "CommandLine.h"   // cli commands, access_points, stations, display_obj, wifi_scan_obj
#include "WiFiScan.h"      // AccessPoint / Station structs, wifi_scan_obj

extern CommandLine cli_obj;

RemoteWeb remote_web_obj;

static const char RW_PAGE[] PROGMEM = R"HTMLPAGE(
<!doctype html><html lang="es"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<title>ESP32 Marauder Remote</title>
<style>
:root{--bg:#070b0e;--bg2:#0d141a;--card:#111a21;--line:#1d2a33;--fg:#e7f0f4;--mut:#7f93a0;--ac:#39d353;--dang:#ff4d4d;--warn:#ffb020;--blue:#3ea0ff}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;background:radial-gradient(1200px 600px at 50% -10%,#0f1b22,#070b0e);color:var(--fg);font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;padding-bottom:40px}
header{position:sticky;top:0;z-index:5;background:linear-gradient(180deg,#0b141a,#0b141aee);backdrop-filter:blur(6px);border-bottom:1px solid var(--line);padding:12px 14px;display:flex;align-items:center;gap:12px}
header svg{flex:0 0 auto;filter:drop-shadow(0 0 6px #39d35355)}
.htxt b{display:block;font-size:16px;letter-spacing:.5px}
.htxt small{color:var(--mut);font-size:11px;letter-spacing:2px}
.live{margin-left:auto;display:flex;align-items:center;gap:7px;font-size:12px;color:var(--mut)}
.dot{width:10px;height:10px;border-radius:50%;background:#555;box-shadow:0 0 0 0 #39d35300}
.dot.on{background:var(--ac);animation:pulse 1.6s infinite}
@keyframes pulse{0%{box-shadow:0 0 0 0 #39d35366}70%{box-shadow:0 0 0 8px #39d35300}100%{box-shadow:0 0 0 0 #39d35300}}
.wrap{max-width:680px;margin:0 auto;padding:14px}
.stats{display:grid;grid-template-columns:repeat(5,1fr);gap:8px;margin-bottom:14px}
.stat{background:var(--card);border:1px solid var(--line);border-radius:12px;padding:9px 6px;text-align:center}
.stat .v{font-size:17px;font-weight:700;font-variant-numeric:tabular-nums}
.stat .k{font-size:9.5px;color:var(--mut);text-transform:uppercase;letter-spacing:.8px;margin-top:2px}
.tabs{display:flex;gap:8px;margin-bottom:12px}
.tab{flex:1;padding:10px;border:1px solid var(--line);background:var(--bg2);color:var(--mut);border-radius:10px;font-size:13px;text-align:center;cursor:pointer;font-weight:600}
.tab.sel{color:var(--fg);border-color:var(--ac);box-shadow:inset 0 0 20px #39d35314}
.pane{display:none}.pane.sel{display:block}
.grid{display:grid;grid-template-columns:repeat(2,1fr);gap:10px}
.btn{display:flex;align-items:center;gap:10px;background:var(--card);border:1px solid var(--line);border-radius:12px;padding:14px 12px;color:var(--fg);font-size:14px;cursor:pointer;text-align:left;transition:transform .05s,border-color .15s}
.btn:active{transform:scale(.97)}
.btn .ic{font-size:19px;width:24px;text-align:center}
.btn.ac{border-color:#2f7a3c}.btn.ac:active{border-color:var(--ac)}
.btn.dg{border-color:#7a2f2f;color:#ffb3b3}
.btn.bl{border-color:#2f567a}
.btn.full{grid-column:1/-1;justify-content:center;font-weight:700}
.sec{color:var(--mut);font-size:11px;text-transform:uppercase;letter-spacing:1.4px;margin:16px 0 8px}
.row{display:flex;gap:8px;margin-top:10px}
input{flex:1;background:#0a1116;color:var(--fg);border:1px solid var(--line);border-radius:10px;padding:13px;font-size:14px}
.send{background:var(--ac);color:#06220f;border:none;border-radius:10px;padding:0 18px;font-weight:800;cursor:pointer}
.term{margin-top:14px;background:#05090c;border:1px solid var(--line);border-radius:12px;overflow:hidden}
.term .bar{display:flex;align-items:center;gap:6px;padding:8px 12px;border-bottom:1px solid var(--line);font-size:11px;color:var(--mut)}
.term .bar i{width:10px;height:10px;border-radius:50%;display:inline-block}
pre{margin:0;padding:12px;white-space:pre-wrap;word-break:break-word;min-height:120px;max-height:40vh;overflow:auto;font-family:ui-monospace,Consolas,monospace;font-size:12px;color:#bfe9c9;line-height:1.5}
pre .cmd{color:var(--ac)}
#toast{position:fixed;left:50%;bottom:22px;transform:translateX(-50%) translateY(30px);background:#0d1a12;border:1px solid var(--ac);color:var(--fg);padding:10px 16px;border-radius:30px;font-size:13px;opacity:0;transition:.25s;pointer-events:none;z-index:9}
#toast.show{opacity:1;transform:translateX(-50%) translateY(0)}
.note{margin-top:14px;background:#17120a;border:1px solid #5c4320;color:#d9c7a3;border-radius:10px;padding:10px 12px;font-size:12px;line-height:1.5}
</style></head><body>
<header>
<svg width="40" height="40" viewBox="0 0 100 100" aria-hidden="true">
<g fill="#eef3f6"><g transform="rotate(45 50 50)"><rect x="16" y="45" width="68" height="8" rx="4"/><circle cx="16" cy="45" r="6"/><circle cx="16" cy="53" r="6"/><circle cx="84" cy="45" r="6"/><circle cx="84" cy="53" r="6"/></g><g transform="rotate(-45 50 50)"><rect x="16" y="45" width="68" height="8" rx="4"/><circle cx="16" cy="45" r="6"/><circle cx="16" cy="53" r="6"/><circle cx="84" cy="45" r="6"/><circle cx="84" cy="53" r="6"/></g></g>
<g fill="#f5f8fa"><rect x="36" y="48" width="28" height="30" rx="9"/><circle cx="50" cy="44" r="23"/></g>
<circle cx="42" cy="42" r="8" fill="#070b0e"/><circle cx="58" cy="42" r="8" fill="#070b0e"/><circle cx="58" cy="42" r="9.5" fill="#070b0e"/><path d="M68 34 L36 27" stroke="#070b0e" stroke-width="3" stroke-linecap="round"/>
<path d="M50 51 l-4 7 h8 z" fill="#070b0e"/>
<rect x="41" y="62" width="18" height="11" rx="2" fill="#070b0e"/><g fill="#f5f8fa"><rect x="43" y="62" width="2.6" height="5"/><rect x="47" y="62" width="2.6" height="5"/><rect x="51" y="62" width="2.6" height="5"/><rect x="55" y="62" width="2.6" height="5"/><rect x="43" y="68" width="2.6" height="5"/><rect x="47" y="68" width="2.6" height="5"/><rect x="51" y="68" width="2.6" height="5"/><rect x="55" y="68" width="2.6" height="5"/></g></svg>
<div class="htxt"><b>ESP32 MARAUDER</b><small>REMOTE CONSOLE</small></div>
<div class="live"><span class="dot" id="dot"></span><span id="livet">conectando…</span></div>
</header>
<div class="wrap">
<div class="stats">
<div class="stat"><div class="v" id="s_mode">—</div><div class="k">Estado</div></div>
<div class="stat"><div class="v" id="s_cli">—</div><div class="k">Conex.</div></div>
<div class="stat"><div class="v" id="s_ap">—</div><div class="k">APs</div></div>
<div class="stat"><div class="v" id="s_sta">—</div><div class="k">Stations</div></div>
<div class="stat"><div class="v" id="s_ram">—</div><div class="k">RAM KB</div></div>
</div>

<div class="tabs">
<div class="tab sel" data-t="w" onclick="tab('w')">📡 WiFi</div>
<div class="tab" data-t="b" onclick="tab('b')">🔵 Bluetooth</div>
<div class="tab" data-t="r" onclick="tab('r')">📊 Resultados</div>
</div>

<div class="pane sel" id="p_w">
<div class="grid">
<button class="btn" onclick="cmd('scanap')"><span class="ic">📡</span>Scan APs</button>
<button class="btn" onclick="cmd('scansta')"><span class="ic">🛰️</span>Scan Stations</button>
<button class="btn" onclick="cmd('sniffbeacon')"><span class="ic">👂</span>Sniff Beacon</button>
<button class="btn" onclick="cmd('sniffprobe')"><span class="ic">👂</span>Sniff Probe</button>
<button class="btn" onclick="cmd('sniffdeauth')"><span class="ic">🚨</span>Sniff Deauth</button>
<button class="btn" onclick="cmd('sniffpwn')"><span class="ic">🐣</span>Sniff Pwnagotchi</button>
</div>
</div>

<div class="pane" id="p_b">
<div class="grid">
<button class="btn bl" onclick="cmd('blespam -t all')"><span class="ic">💥</span>BLE Spam All</button>
<button class="btn bl" onclick="cmd('blespam -t apple')"><span class="ic"></span>BLE Spam Apple</button>
<button class="btn bl" onclick="cmd('blespam -t google')"><span class="ic">🤖</span>BLE Spam Google</button>
<button class="btn bl" onclick="cmd('blespam -t samsung')"><span class="ic">📱</span>BLE Spam Samsung</button>
<button class="btn bl" onclick="cmd('sniffbt')"><span class="ic">🔵</span>Sniff BT</button>
<button class="btn bl" onclick="cmd('sniffbt -t flipper')"><span class="ic">🐬</span>Sniff Flipper</button>
</div>
</div>

<div class="pane" id="p_r">
<div class="grid">
<button class="btn ac" onclick="get('/aps')"><span class="ic">📶</span>Ver APs</button>
<button class="btn ac" onclick="get('/sta')"><span class="ic">💻</span>Ver Stations</button>
<button class="btn ac full" onclick="refresh()"><span class="ic">🔄</span>Actualizar estado</button>
</div>
</div>

<button class="btn dg full" style="margin-top:12px" onclick="cmd('stopscan')"><span class="ic">⏹️</span>STOP · parar todo</button>

<div class="sec">Comando manual</div>
<div class="row"><input id="x" placeholder="ej:  channel -s 6" autocapitalize="off" autocomplete="off" spellcheck="false"><button class="send" onclick="manual()">▶</button></div>

<div class="term">
<div class="bar"><i style="background:#ff5f56"></i><i style="background:#ffbd2e"></i><i style="background:#27c93f"></i>&nbsp; marauder@esp32</div>
<pre id="out"><span class="cmd">#</span> listo. Pulsa un boton o escribe un comando.
</pre>
</div>

<div class="note">⚠ Una sola antena: al iniciar un <b>escaneo/ataque WiFi</b> el telefono se desconecta de <b>ESP32HACKER</b> hasta que pulses STOP. El <b>Bluetooth</b> y la configuracion funcionan conectado. Usa los ataques solo en tus redes/dispositivos.</div>
</div>
<div id="toast"></div>
<script>
var out=document.getElementById('out');
function tstamp(){var d=new Date();return d.toTimeString().slice(0,8)}
function app(t,isCmd){var l=document.createElement('div');if(isCmd){l.innerHTML='<span class="cmd">'+tstamp()+' &gt;</span> '+esc(t)}else{l.textContent=t}out.appendChild(l);out.scrollTop=out.scrollHeight;while(out.childNodes.length>400)out.removeChild(out.firstChild)}
function esc(s){return s.replace(/&/g,'&amp;').replace(/</g,'&lt;')}
function toast(m){var t=document.getElementById('toast');t.textContent=m;t.classList.add('show');clearTimeout(t._);t._=setTimeout(()=>t.classList.remove('show'),1600)}
function cmd(c){app(c,true);toast('Enviado: '+c);fetch('/cmd?c='+encodeURIComponent(c)).then(r=>r.text()).then(t=>app('  '+t)).catch(e=>app('  (enviado; si era WiFi, reconecta al AP para continuar)'))}
function manual(){var v=document.getElementById('x').value.trim();if(v){cmd(v);document.getElementById('x').value=''}}
function get(u){toast('Cargando…');fetch(u).then(r=>r.text()).then(t=>{app('── '+u+' ──');t.split('\n').forEach(li=>{if(li)app(li)})}).catch(e=>app('  error '+u))}
function tab(t){document.querySelectorAll('.tab').forEach(x=>x.classList.toggle('sel',x.dataset.t==t));document.querySelectorAll('.pane').forEach(x=>x.classList.toggle('sel',x.id=='p_'+t))}
document.getElementById('x').addEventListener('keydown',e=>{if(e.key==='Enter')manual()});
var dot=document.getElementById('dot'),livet=document.getElementById('livet');
function refresh(){fetch('/status').then(r=>r.json()).then(d=>{
 dot.classList.add('on');livet.textContent='en linea';
 document.getElementById('s_mode').textContent=d.mode==0?'IDLE':'BUSY';
 document.getElementById('s_mode').style.color=d.mode==0?'#39d353':'#ffb020';
 document.getElementById('s_cli').textContent=d.clients;
 document.getElementById('s_ap').textContent=d.aps;
 document.getElementById('s_sta').textContent=d.sta;
 document.getElementById('s_ram').textContent=Math.round(d.ram/1024);
}).catch(e=>{dot.classList.remove('on');livet.textContent='sin señal'})}
refresh();setInterval(refresh,2500);
</script></body></html>
)HTMLPAGE";

void RemoteWeb::toggle() {
  if (active) stop();
  else start();
}

// Runs from the main loop(): executes a command queued by the web handler.
void RemoteWeb::main() {
  if (!pending) return;
  pending = false;
  char cmd[sizeof(pending_cmd)];
  strlcpy(cmd, pending_cmd, sizeof(cmd));
  pending_cmd[0] = 0;
  cli_obj.runCommand(String(cmd));
}

void RemoteWeb::start(bool show_info) {
  if (active) return;

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32HACKER", "123456789");
  delay(100);

  server = new AsyncWebServer(80);
  setupRoutes();
  server->begin();
  active = true;

  IPAddress ip = WiFi.softAPIP();
  Serial.println("[RemoteWeb] AP 'ESP32HACKER' pass '123456789' @ " + ip.toString());

  #ifdef HAS_FULL_SCREEN
   if (show_info) {
    display_obj.tft.fillScreen(TFT_BLACK);
    display_obj.tft.setTextWrap(true);
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.tft.setTextSize(2);
    display_obj.tft.setCursor(6, 16);
    display_obj.tft.println("REMOTE WEB");
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
    display_obj.tft.println("");
    display_obj.tft.println(" WiFi: ESP32HACKER");
    display_obj.tft.println(" Pass: 123456789");
    display_obj.tft.println(" URL : " + ip.toString());
    display_obj.tft.println("");
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    display_obj.tft.println(" Conecta el telefono y");
    display_obj.tft.println(" abre esa direccion.");
    display_obj.tft.println("");
    display_obj.tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    display_obj.tft.println(" Menu > Remote Web");
    display_obj.tft.println(" otra vez para parar.");
   }
  #endif
}

void RemoteWeb::stop() {
  if (!active) return;
  if (server) {
    server->end();
    delete server;
    server = nullptr;
  }
  WiFi.softAPdisconnect(true);
  active = false;
  Serial.println("[RemoteWeb] stopped");

  #ifdef HAS_FULL_SCREEN
    display_obj.tft.fillScreen(TFT_BLACK);
    display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
    display_obj.tft.setTextSize(2);
    display_obj.tft.setCursor(6, 20);
    display_obj.tft.println("Remote Web OFF");
  #endif
}

static String macStr(const uint8_t* m) {
  char b[18];
  sprintf(b, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
  return String(b);
}

void RemoteWeb::setupRoutes() {
  server->on("/", HTTP_GET, [](AsyncWebServerRequest* r) {
    r->send_P(200, "text/html", RW_PAGE);
  });

  server->on("/cmd", HTTP_GET, [](AsyncWebServerRequest* r) {
    if (!r->hasParam("c")) { r->send(400, "text/plain", "missing c"); return; }
    String c = r->getParam("c")->value();
    // Do NOT run the command here: this runs in the AsyncWebServer task, whose
    // stack is too small for Marauder commands and would reboot the board.
    // Queue it; RemoteWeb::main() runs it from the main loop (big stack).
    strlcpy(remote_web_obj.pending_cmd, c.c_str(), sizeof(remote_web_obj.pending_cmd));
    remote_web_obj.pending = true;
    r->send(200, "text/plain", "OK: " + c);
  });

  server->on("/aps", HTTP_GET, [](AsyncWebServerRequest* r) {
    String out = "";
    if (access_points) {
      for (int i = 0; i < access_points->size(); i++) {
        AccessPoint ap = access_points->get(i);
        out += String(i) + ") " + ap.essid + "  ch" + String(ap.channel) +
               "  " + String(ap.rssi) + "dBm  " + macStr(ap.bssid) + "\n";
      }
    }
    if (out == "") out = "(sin APs - pulsa 'Scan APs' primero)";
    r->send(200, "text/plain", out);
  });

  server->on("/sta", HTTP_GET, [](AsyncWebServerRequest* r) {
    String out = "";
    if (stations) {
      for (int i = 0; i < stations->size(); i++) {
        Station s = stations->get(i);
        out += String(i) + ") " + macStr(s.mac) + "  pkts:" + String(s.packets) + "\n";
      }
    }
    if (out == "") out = "(sin stations - pulsa 'Scan Stations' primero)";
    r->send(200, "text/plain", out);
  });

  server->on("/status", HTTP_GET, [](AsyncWebServerRequest* r) {
    String j = "{";
    j += "\"mode\":" + String(wifi_scan_obj.currentScanMode);
    j += ",\"ram\":" + String(ESP.getFreeHeap());
    j += ",\"aps\":" + String(access_points ? access_points->size() : 0);
    j += ",\"sta\":" + String(stations ? stations->size() : 0);
    j += ",\"clients\":" + String(WiFi.softAPgetStationNum());
    j += "}";
    r->send(200, "application/json", j);
  });
}
