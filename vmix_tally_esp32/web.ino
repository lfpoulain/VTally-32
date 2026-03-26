const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1.0'>
<title>VTally-32</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#0a0e27;background:linear-gradient(135deg,#0a0e27 0%,#1a1535 50%,#2d1b4e 100%);color:#e0e7ff;min-height:100vh;padding:20px;position:relative;overflow-x:hidden}
body::before{content:'';position:absolute;top:0;left:0;right:0;bottom:0;background:radial-gradient(circle at 20% 50%,rgba(99,102,241,0.1) 0%,transparent 50%),radial-gradient(circle at 80% 80%,rgba(168,85,247,0.1) 0%,transparent 50%);pointer-events:none}
.container{max-width:800px;margin:0 auto;position:relative;z-index:1}
.card{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3),inset 0 1px 0 rgba(255,255,255,0.05);transition:all 0.3s ease}
.card:hover{border-color:rgba(99,102,241,0.4);box-shadow:0 12px 48px rgba(99,102,241,0.15),inset 0 1px 0 rgba(255,255,255,0.05)}
.header{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:32px 28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3);transition:border 0.3s ease;position:relative;overflow:hidden;--status-color:#6366f1}
.header::before{content:'';position:absolute;top:0;left:0;right:0;height:4px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transition:all 0.15s ease-out}
.header.status-live::before,.header.status-preview::before{background:var(--status-color);box-shadow:0 0 20px var(--status-color)}
h1{text-align:center;margin-bottom:8px;font-size:2.2em;background:linear-gradient(135deg,#818cf8 0%,#c084fc 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;font-weight:700;letter-spacing:-0.5px;transition:filter 0.15s ease-out}
.header.status-live h1,.header.status-preview h1{background:var(--status-color);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;filter:brightness(1.2)}
.subtitle{text-align:center;color:#94a3b8;font-size:0.85em;font-weight:500;margin-bottom:8px}
.status-badge{display:inline-block;padding:6px 16px;border-radius:20px;font-size:0.8em;font-weight:600;margin-top:12px;background:rgba(99,102,241,0.2);border:1px solid rgba(99,102,241,0.4);color:#c7d2fe;transition:all 0.15s ease-out}
.header.status-live .status-badge,.header.status-preview .status-badge{background:color-mix(in srgb,var(--status-color) 20%,transparent);border-color:color-mix(in srgb,var(--status-color) 40%,transparent);color:var(--status-color);filter:brightness(1.3)}
.form-group{margin-bottom:22px}
label{display:block;margin-bottom:10px;font-weight:500;color:#cbd5e1;font-size:0.9em;letter-spacing:0.3px}
input,select{width:100%;padding:14px 16px;border:1px solid rgba(99,102,241,0.3);border-radius:12px;background:rgba(15,23,42,0.8);color:#e0e7ff;font-size:15px;transition:all 0.3s;outline:none}
input:focus,select:focus{border-color:rgba(99,102,241,0.6);background:rgba(15,23,42,0.95);box-shadow:0 0 0 3px rgba(99,102,241,0.1)}
input::placeholder{color:#64748b}
input[readonly]{background:rgba(15,23,42,0.5);color:#94a3b8;cursor:not-allowed}
.color-input{height:56px;cursor:pointer;border-width:2px}
.color-input:hover{border-color:rgba(168,85,247,0.5)}
.btn{background:linear-gradient(135deg,#6366f1 0%,#8b5cf6 100%);color:#fff;padding:14px 28px;border:none;border-radius:12px;cursor:pointer;font-size:15px;font-weight:600;width:100%;margin-bottom:12px;transition:all 0.3s;box-shadow:0 4px 16px rgba(99,102,241,0.3);position:relative;overflow:hidden}
.btn::before{content:'';position:absolute;top:50%;left:50%;width:0;height:0;border-radius:50%;background:rgba(255,255,255,0.2);transform:translate(-50%,-50%);transition:width 0.6s,height 0.6s}
.btn:hover::before{width:300px;height:300px}
.btn:hover{transform:translateY(-2px);box-shadow:0 6px 24px rgba(99,102,241,0.4)}
.btn:active{transform:translateY(0)}
.btn-danger{background:linear-gradient(135deg,#ef4444 0%,#dc2626 100%);box-shadow:0 4px 16px rgba(239,68,68,0.3)}
.btn-danger:hover{box-shadow:0 6px 24px rgba(239,68,68,0.4)}
.btn-warning{background:linear-gradient(135deg,#f59e0b 0%,#d97706 100%);box-shadow:0 4px 16px rgba(245,158,11,0.3)}
.btn-warning:hover{box-shadow:0 6px 24px rgba(245,158,11,0.4)}
h3{font-size:1.3em;margin:24px 0 20px;padding-bottom:12px;border-bottom:2px solid rgba(99,102,241,0.3);color:#c7d2fe;font-weight:600;letter-spacing:-0.3px}
.tabs{display:flex;gap:12px;margin-bottom:24px;flex-wrap:wrap}
.tab{background:rgba(15,23,42,0.5);padding:14px 28px;border-radius:12px;cursor:pointer;transition:all 0.3s;border:2px solid rgba(99,102,241,0.2);font-weight:500;color:#94a3b8;position:relative}
.tab::after{content:'';position:absolute;bottom:0;left:50%;width:0;height:2px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transform:translateX(-50%);transition:width 0.3s}
.tab:hover{background:rgba(15,23,42,0.8);border-color:rgba(99,102,241,0.4);color:#c7d2fe}
.tab.active{background:rgba(99,102,241,0.15);border-color:rgba(99,102,241,0.5);color:#e0e7ff;box-shadow:0 4px 16px rgba(99,102,241,0.2)}
.tab.active::after{width:80%}
.tab-content{display:none;animation:fadeIn 0.4s ease}
.tab-content.active{display:block}
@keyframes fadeIn{from{opacity:0;transform:translateY(10px)}to{opacity:1;transform:translateY(0)}}
.diag-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:14px;margin-top:18px}
.diag-item{background:rgba(15,23,42,0.75);border:1px solid rgba(99,102,241,0.18);border-radius:14px;padding:16px 18px}
.diag-label{font-size:0.78em;color:#94a3b8;margin-bottom:8px;text-transform:uppercase;letter-spacing:0.08em}
.diag-value{font-size:1em;color:#e2e8f0;font-weight:600;word-break:break-word}
.diag-meta{color:#94a3b8;font-size:0.88em;margin-top:14px}
.progress-wrap{width:100%;height:12px;background:rgba(15,23,42,0.8);border-radius:999px;overflow:hidden;border:1px solid rgba(99,102,241,0.2);margin-top:14px}
.progress-bar{height:100%;width:0;background:linear-gradient(90deg,#22c55e 0%,#16a34a 100%);transition:width 0.2s ease}
</style></head><body>
<div class='container'>
<div class='header' id='header'>
<h1 id='titleName'>VTally-32</h1>
<div class='subtitle' id='info'>Professional VMix Tally System</div>
<div style='text-align:center'><span class='status-badge' id='statusBadge'>Démarrage...</span></div>
</div>

<div class='tabs'>
<div class='tab active' onclick='switchTab(0)'>VMix & Couleurs</div>
<div class='tab' onclick='switchTab(1)'>Matériel & Réseau</div>
<div class='tab' onclick='switchTab(2)'>WiFi</div>
<div class='tab' onclick='switchTab(3)'>Diagnostics</div>
</div>

<div class='tab-content active' id='tab0'>
<div class='card'>
<h3>Configuration VMix</h3>
<form id='vmixForm'>
<div class='form-group'><label>IP VMix</label><input type='text' id='host' required pattern='\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}'></div>
<div class='form-group'><label>Input Number</label><input type='number' id='input' min='1' max='999' required></div>

<h3>Couleurs</h3>
<div class='form-group'><label>Live</label><input type='color' id='live' class='color-input'></div>
<div class='form-group'><label>Preview</label><input type='color' id='preview' class='color-input'></div>
<div class='form-group'><label>Off</label><input type='color' id='off' class='color-input'></div>
<div class='form-group'><label>Luminosite: <span id='bval'>255</span></label><input type='range' id='brightness' min='0' max='255' value='255'></div>

<button type='submit' class='btn'>Sauvegarder Configuration</button>
</form></div></div>

<div class='tab-content' id='tab1'>
<div class='card'>
<h3>Général & Matériel</h3>
<p style='background:rgba(245,158,11,0.1);padding:16px 18px;border-radius:12px;margin-bottom:24px;border:1px solid rgba(245,158,11,0.3);color:#fcd34d;font-size:0.9em;line-height:1.6'><strong style='color:#fbbf24'>⚠️ Attention:</strong> Toute modification ici nécessite un redémarrage automatique de l'ESP32.</p>
<form id='hardwareForm'>
<div class='form-group'><label>Nom du Tally (Réseau et mDNS)</label><input type='text' id='tally_name' maxlength='32' required></div>
<div class='form-group'><label>Pin GPIO de la LED</label><input type='number' id='led_pin' min='0' max='48' required></div>
<div class='form-group'><label>Nombre de LEDs</label><input type='number' id='led_count' min='1' max='255' required></div>
<button type='submit' class='btn btn-warning'>Sauvegarder et Redémarrer</button>
</form></div></div>

<div class='tab-content' id='tab2'>
<div class='card'>
<h3>Configuration WiFi</h3>
<div class='form-group'><label>Réseaux disponibles</label><select id='wifiList' onchange='selectNetwork()'><option value=''>Chargement...</option></select><button type='button' class='btn' onclick='scanWiFi()' style='margin-top:10px'>🔄 Scanner</button></div>
<div class='form-group'><label>SSID (ou saisir manuellement)</label><input type='text' id='newssid' placeholder='Nom du réseau WiFi'></div>
<div class='form-group'><label>Mot de passe</label><input type='password' id='pwd' placeholder='Mot de passe WiFi'></div>
<button type='button' class='btn' onclick='saveWiFi()'>Configurer WiFi et Redémarrer</button>
<button type='button' class='btn btn-danger' onclick='reboot()'>Redémarrer</button>
<h3>Mise à jour OTA</h3>
<form id='otaForm'>
<div class='form-group'><label>Firmware `.bin`</label><input type='file' id='ota_file' accept='.bin' required></div>
<button type='submit' class='btn btn-warning'>Installer le firmware</button>
<div class='progress-wrap'><div class='progress-bar' id='otaProgressBar'></div></div>
<div class='diag-meta' id='otaProgressText'>Sélectionnez le fichier `-ota.bin` correspondant à votre carte.</div>
</form>
</div></div>

<div class='tab-content' id='tab3'>
<div class='card'>
<h3>Diagnostics système</h3>
<div class='diag-meta' id='diagnosticsUpdated'>Chargement des diagnostics...</div>
<div class='diag-grid' id='diagnosticsGrid'></div>
</div></div></div>

<script>
let currentColors={live:'#ff0000',preview:'#00ff00',off:'#000000'};
let otaUploadInProgress=false;
function toHex(c){let h=((c||0)>>>0).toString(16).padStart(6,'0');return '#'+h;}
document.addEventListener('DOMContentLoaded',function(){
fetch('/config').then(r=>r.json()).then(d=>{
if(d.tally_name){
document.getElementById('titleName').textContent=d.tally_name;
document.title=d.tally_name;
document.getElementById('tally_name').value=d.tally_name;
}
document.getElementById('host').value=d.vmix_host;
document.getElementById('input').value=d.vmix_input;
document.getElementById('brightness').value=d.brightness;
document.getElementById('bval').textContent=d.brightness;
document.getElementById('led_pin').value=d.led_pin;
document.getElementById('led_count').value=d.led_count;
currentColors.live=toHex(d.live_color);
currentColors.preview=toHex(d.preview_color);
currentColors.off=toHex(d.off_color);
document.getElementById('live').value=currentColors.live;
document.getElementById('preview').value=currentColors.preview;
document.getElementById('off').value=currentColors.off;
updateStatus();
scanWiFi();
updateDiagnostics();
});
document.getElementById('live').addEventListener('input',e=>{currentColors.live=e.target.value;updateStatus();});
document.getElementById('preview').addEventListener('input',e=>{currentColors.preview=e.target.value;updateStatus();});
document.getElementById('off').addEventListener('input',e=>{currentColors.off=e.target.value;updateStatus();});
document.getElementById('brightness').addEventListener('input',e=>{document.getElementById('bval').textContent=e.target.value;});
document.getElementById('vmixForm').addEventListener('submit',e=>{
e.preventDefault();
const data={vmix_host:document.getElementById('host').value,vmix_input:document.getElementById('input').value,live_color:parseInt(currentColors.live.substring(1),16),preview_color:parseInt(currentColors.preview.substring(1),16),off_color:parseInt(currentColors.off.substring(1),16),brightness:parseInt(document.getElementById('brightness').value)};
fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Configuration sauvegardee!');}).catch(e=>alert('Erreur: '+e));
});
document.getElementById('hardwareForm').addEventListener('submit',e=>{
e.preventDefault();
if(confirm('Sauvegarder la configuration matérielle? L\'ESP32 va redémarrer.')){
const data={tally_name:document.getElementById('tally_name').value,led_pin:parseInt(document.getElementById('led_pin').value),led_count:parseInt(document.getElementById('led_count').value)};
fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Configuration sauvegardee! Redemarrage...');setTimeout(()=>location.reload(),3000);}).catch(e=>alert('Erreur: '+e));}
});
document.getElementById('otaForm').addEventListener('submit',uploadOTA);
setInterval(updateStatus,1000);
setInterval(updateDiagnostics,2000);
});
function updateStatus(){
if(otaUploadInProgress)return;
fetch('/status').then(r=>r.json()).then(d=>{
const header=document.getElementById('header'),badge=document.getElementById('statusBadge');
document.getElementById('info').textContent='WiFi: '+(d.wifi_ssid||'AP Mode')+' | IP: '+(d.wifi_ip||'192.168.4.1');
if(d.connected){
if(d.live){
header.className='header status-live';
header.style.setProperty('--status-color',currentColors.live);
badge.textContent='🔴 LIVE';
}else if(d.preview){
header.className='header status-preview';
header.style.setProperty('--status-color',currentColors.preview);
badge.textContent='🟢 PREVIEW';
}else{
header.className='header';
header.style.removeProperty('--status-color');
badge.textContent='✓ Connecté';
}
}else{
header.className='header';
header.style.removeProperty('--status-color');
badge.textContent='⚠ Déconnecté';
}
}).catch(e=>console.error('Erreur status:',e));}
function renderDiagnostics(d){
const grid=document.getElementById('diagnosticsGrid');
if(!grid)return;
const items=[
['Nom Tally',d.tally_name],
['Firmware',d.firmware_version],
['Uptime',d.uptime_human],
['Heap libre',d.free_heap+' B'],
['Heap min',d.min_free_heap+' B'],
['Bloc max',d.max_alloc_heap+' B'],
['Mode WiFi',d.wifi_mode],
['AP actif',d.ap_active?'Oui':'Non'],
['STA connecte',d.sta_connected?'Oui':'Non'],
['SSID',d.wifi_ssid||'Aucun'],
['IP STA',d.wifi_ip||'-'],
['IP AP',d.ap_ip||'-'],
['RSSI',d.sta_connected?(d.wifi_rssi+' dBm'):'N/A'],
['VMix',d.vmix_connected?'Connecté':'Déconnecté'],
['Socket VMix',d.vmix_socket_connected?'Ouvert':'Fermé'],
['VMix cible',d.vmix_host+':8099'],
['Input',d.vmix_input],
['Tally',d.tally_state],
['LEDs',d.led_count+' sur GPIO '+d.led_pin],
['Luminosité',d.brightness]
];
grid.innerHTML=items.map(item=>'<div class="diag-item"><div class="diag-label">'+item[0]+'</div><div class="diag-value">'+item[1]+'</div></div>').join('');
document.getElementById('diagnosticsUpdated').textContent='Dernière mise à jour: '+new Date().toLocaleTimeString();
}
function updateDiagnostics(){
if(otaUploadInProgress)return;
fetch('/diagnostics').then(r=>r.json()).then(d=>renderDiagnostics(d)).catch(e=>{const el=document.getElementById('diagnosticsUpdated');if(el){el.textContent='Erreur diagnostics';}console.error('Erreur diagnostics:',e);});
}
function saveWiFi(){
const ssid=document.getElementById('newssid').value,pwd=document.getElementById('pwd').value;
if(!ssid){alert('Entrez un SSID');return;}
fetch('/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:ssid,password:pwd})}).then(()=>{alert('WiFi configure! Redemarrage...');setTimeout(()=>location.reload(),3000)}).catch(e=>alert('Erreur: '+e));
}
function reboot(){if(confirm('Redemarrer?')){fetch('/reboot',{method:'POST'}).then(()=>alert('Redemarrage...'));}}
function uploadOTA(e){
e.preventDefault();
const fileInput=document.getElementById('ota_file');
const file=fileInput.files[0];
const progressBar=document.getElementById('otaProgressBar');
const progressText=document.getElementById('otaProgressText');
if(!file){alert('Selectionnez un firmware .bin');return;}
if(!confirm('Installer ce firmware via OTA ?'))return;
const formData=new FormData();
formData.append('firmware',file);
otaUploadInProgress=true;
progressBar.style.width='0%';
progressText.textContent='Téléversement 0%';
const xhr=new XMLHttpRequest();
xhr.open('POST','/update',true);
xhr.upload.onprogress=function(evt){
if(evt.lengthComputable){
const percent=Math.round((evt.loaded/evt.total)*100);
progressBar.style.width=percent+'%';
progressText.textContent='Téléversement '+percent+'%';
}
};
xhr.onload=function(){
otaUploadInProgress=false;
if(xhr.status>=200&&xhr.status<300){
progressBar.style.width='100%';
let message='Mise a jour envoyee. Redemarrage...';
try{const data=JSON.parse(xhr.responseText);if(data.message)message=data.message;}catch(e){}
progressText.textContent=message;
alert(message);
setTimeout(()=>location.reload(),5000);
}else{
let error='Echec de la mise a jour OTA';
try{const data=JSON.parse(xhr.responseText);if(data.error)error=data.error;}catch(e){}
progressBar.style.width='0%';
progressText.textContent=error;
alert(error);
}
};
xhr.onerror=function(){
otaUploadInProgress=false;
progressBar.style.width='0%';
progressText.textContent='Erreur reseau OTA';
alert('Erreur reseau OTA');
};
xhr.send(formData);
}
function scanWiFi(){
const list=document.getElementById('wifiList');
list.innerHTML='<option value="">Scan en cours...</option>';
fetch('/scan').then(r=>r.json()).then(d=>{
if(d.networks&&d.networks.length>0){
list.innerHTML='<option value="">-- Sélectionner un réseau --</option>';
d.networks.forEach(n=>{
const opt=document.createElement('option');
opt.value=n.ssid;
opt.textContent=n.ssid+' ('+n.rssi+' dBm)'+(n.secure?' 🔒':'');
list.appendChild(opt);
});
}else{list.innerHTML='<option value="">Aucun réseau trouvé</option>';}
}).catch(e=>{list.innerHTML='<option value="">Erreur de scan</option>';console.error(e);});
}
function selectNetwork(){
const ssid=document.getElementById('wifiList').value;
if(ssid)document.getElementById('newssid').value=ssid;
}
function switchTab(index){
const tabs=document.querySelectorAll('.tab');
const contents=document.querySelectorAll('.tab-content');
tabs.forEach((t,i)=>{if(i===index){t.classList.add('active');}else{t.classList.remove('active');}});
contents.forEach((c,i)=>{if(i===index){c.classList.add('active');}else{c.classList.remove('active');}});
if(index===3){updateDiagnostics();}
}
</script></body></html>
)rawliteral";

void handleRoot() {
  LOG_WEB("Accès à la page d'accueil (GET /)");
  server.send_P(200, "text/html", index_html);
}

void handleConfig() {
  if (server.method() == HTTP_GET) {
    LOG_WEB("Demande de configuration (GET /config)");
    StaticJsonDocument<512> doc;
    doc["tally_name"] = config.tally_name;
    doc["vmix_host"] = config.vmix_host;
    doc["vmix_input"] = config.vmix_input;
    doc["live_color"] = config.live_color;
    doc["preview_color"] = config.preview_color;
    doc["off_color"] = config.off_color;
    doc["brightness"] = config.brightness;
    doc["led_pin"] = config.led_pin;
    doc["led_count"] = config.led_count;
    doc["wifi_ssid"] = WiFi.SSID();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  } else if (server.method() == HTTP_POST) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      LOG_ERROR("Erreur JSON: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
      return;
    }

    LOG_WEB("Réception configuration:");
    serializeJsonPretty(doc, Serial);
    Serial.println();

    Config updatedConfig = config;
    bool vmixConfigChanged = false;
    bool hardwareConfigChanged = false;

    if (doc.containsKey("tally_name")) {
      String tallyName = doc["tally_name"] | "";
      tallyName.trim();
      if (tallyName.length() > 0 && tallyName.length() < sizeof(updatedConfig.tally_name)) {
        strlcpy(updatedConfig.tally_name, tallyName.c_str(), sizeof(updatedConfig.tally_name));
        hardwareConfigChanged = true; // Necessite un redémarrage pour mDNS et AP
      } else {
        server.send(400, "application/json", "{\"error\":\"Nom du Tally invalide\"}");
        return;
      }
    }

    if (doc.containsKey("vmix_host")) {
      String vmixHost = doc["vmix_host"] | "";
      vmixHost.trim();
      if (vmixHost.length() == 0 || vmixHost.length() >= sizeof(updatedConfig.vmix_host)) {
        server.send(400, "application/json", "{\"error\":\"Hôte VMix invalide\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_host, vmixHost.c_str(), sizeof(updatedConfig.vmix_host));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_input")) {
      String vmixInput = doc["vmix_input"] | "";
      vmixInput.trim();
      if (!isValidVMixInput(vmixInput.c_str())) {
        server.send(400, "application/json", "{\"error\":\"Input VMix invalide\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_input, vmixInput.c_str(), sizeof(updatedConfig.vmix_input));
      vmixConfigChanged = true;
    }

    if (doc.containsKey("live_color")) {
      updatedConfig.live_color = doc["live_color"].as<unsigned long>() & 0xFFFFFF;
    }
    if (doc.containsKey("preview_color")) {
      updatedConfig.preview_color = doc["preview_color"].as<unsigned long>() & 0xFFFFFF;
    }
    if (doc.containsKey("off_color")) {
      updatedConfig.off_color = doc["off_color"].as<unsigned long>() & 0xFFFFFF;
    }

    if (doc.containsKey("brightness")) {
      int brightness = doc["brightness"].as<int>();
      if (!isValidBrightness(brightness)) {
        server.send(400, "application/json", "{\"error\":\"Luminosité invalide\"}");
        return;
      }
      updatedConfig.brightness = brightness;
    }
    if (doc.containsKey("led_pin")) {
      int ledPin = doc["led_pin"].as<int>();
      if (!isValidLedPin(ledPin)) {
        server.send(400, "application/json", "{\"error\":\"Pin LED invalide\"}");
        return;
      }
      updatedConfig.led_pin = ledPin;
      hardwareConfigChanged = true;
    }
    if (doc.containsKey("led_count")) {
      int ledCount = doc["led_count"].as<int>();
      if (!isValidLedCount(ledCount)) {
        server.send(400, "application/json", "{\"error\":\"Nombre de LEDs invalide\"}");
        return;
      }
      updatedConfig.led_count = ledCount;
      hardwareConfigChanged = true;
    }

    config = updatedConfig;

    LOG_INFO("Configuration mise à jour:");
    LOG_DEBUG("  Nom: %s", config.tally_name);
    LOG_DEBUG("  VMix: %s:8099 Input:%s", config.vmix_host, config.vmix_input);
    LOG_DEBUG("  Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X Brightness=%d",
              config.live_color, config.preview_color, config.off_color, config.brightness);
    LOG_DEBUG("  Matériel: Pin=%d Count=%d", config.led_pin, config.led_count);

    saveConfig();

    if (vmixConfigChanged) {
      if (vmixClient.connected()) {
        vmixClient.stop();
      }
      vmixConnected = false;
      setTally(false, false);
      lastVMixCheck = 0;
      LOG_VMIX("Configuration VMix changée, reconnexion...");
    }

    if (!hardwareConfigChanged) {
      applyTallyState();
    }

    if (hardwareConfigChanged) {
      server.send(200, "application/json", "{\"status\":\"ok\",\"reboot\":true}");
      LOG_INFO("Configuration matérielle changée, redémarrage...");
      pendingReboot = true;
      rebootTime = millis();
    } else {
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    }
  }
}

void handleWiFi() {
  if (server.method() == HTTP_POST) {
    LOG_WEB("Demande de mise à jour WiFi (POST /wifi)");
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      LOG_ERROR("Erreur JSON WiFi: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
      return;
    }

    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";
    ssid.trim();

    if (ssid.length() > 0 && ssid.length() < sizeof(config.wifi_ssid) && password.length() < sizeof(config.wifi_password)) {
      LOG_WEB("Sauvegarde WiFi: %s", ssid.c_str());
      strlcpy(config.wifi_ssid, ssid.c_str(), sizeof(config.wifi_ssid));
      strlcpy(config.wifi_password, password.c_str(), sizeof(config.wifi_password));
      saveConfig();

      server.send(200, "application/json", "{\"status\":\"ok\"}");
      pendingReboot = true;
      rebootTime = millis();
    } else {
      server.send(400, "application/json", "{\"error\":\"SSID requis ou invalide\"}");
    }
  }
}

void handleStatus() {
  StaticJsonDocument<256> doc;
  doc["connected"] = vmixConnected;
  doc["live"] = isLive;
  doc["preview"] = isPreview;
  doc["vmix_host"] = config.vmix_host;
  doc["vmix_input"] = config.vmix_input;
  doc["wifi_ssid"] = WiFi.SSID();
  doc["wifi_ip"] = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "";

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleScan() {
  LOG_WEB("Scan WiFi demandé...");
  int n = WiFi.scanNetworks();
  LOG_WEB("%d réseaux trouvés", n);

  // Limiter à 15 réseaux pour économiser la RAM de l'ESP32
  int networkCount = n > 0 ? min(n, 15) : 0;
  
  // Utiliser une taille statique ou calculée raisonnable
  size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(networkCount) + (networkCount * JSON_OBJECT_SIZE(3)) + 512;
  DynamicJsonDocument doc(capacity);
  JsonArray networks = doc.createNestedArray("networks");

  for (int i = 0; i < networkCount; i++) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
  }

  WiFi.scanDelete();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleReboot() {
  LOG_WEB("Demande de redémarrage (POST /reboot)");
  server.send(200, "application/json", "{\"status\":\"rebooting\"}");
  pendingReboot = true;
  rebootTime = millis();
}

void handleOTAUpdate() {
  StaticJsonDocument<256> doc;

  if (!otaUploadStarted) {
    doc["status"] = "error";
    doc["error"] = "Aucun fichier recu";
    String response;
    serializeJson(doc, response);
    server.send(400, "application/json", response);
    return;
  }

  if (!otaUploadSuccess) {
    doc["status"] = "error";
    doc["error"] = otaLastError.length() > 0 ? otaLastError : "Echec de la mise a jour OTA";
    String response;
    serializeJson(doc, response);
    server.send(500, "application/json", response);
    return;
  }

  doc["status"] = "ok";
  doc["reboot"] = true;
  doc["message"] = "Mise a jour envoyee. Redemarrage...";

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);

  pendingReboot = true;
  rebootTime = millis();
}

void handleOTAUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    otaUploadStarted = true;
    otaUploadSuccess = false;
    otaLastError = "";

    LOG_WEB("Demande de mise a jour OTA: %s", upload.filename.c_str());

    if (!upload.filename.endsWith(".bin")) {
      otaLastError = "Fichier firmware .bin requis";
      return;
    }

    if (vmixClient.connected()) {
      vmixClient.stop();
    }
    vmixConnected = false;
    setTally(false, false);

    size_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) {
      otaLastError = "Impossible d'initialiser la mise a jour OTA";
      LOG_ERROR("%s", otaLastError.c_str());
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (otaLastError.length() == 0) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        otaLastError = "Erreur d'ecriture pendant la mise a jour OTA";
        LOG_ERROR("%s", otaLastError.c_str());
        Update.printError(Serial);
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (otaLastError.length() == 0) {
      if (Update.end(true)) {
        otaUploadSuccess = true;
        LOG_INFO("Mise a jour OTA terminee: %lu octets", (unsigned long)upload.totalSize);
      } else {
        otaLastError = "Erreur de finalisation de la mise a jour OTA";
        LOG_ERROR("%s", otaLastError.c_str());
        Update.printError(Serial);
      }
    }
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    otaUploadSuccess = false;
    otaLastError = "Mise a jour OTA annulee";
    LOG_WARN("Mise a jour OTA annulee");
  }
}
