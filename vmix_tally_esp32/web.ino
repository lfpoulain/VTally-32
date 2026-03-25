void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html.reserve(24576);
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  html += "<title>VTally-32</title>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box}";
  html += "body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#0a0e27;background:linear-gradient(135deg,#0a0e27 0%,#1a1535 50%,#2d1b4e 100%);color:#e0e7ff;min-height:100vh;padding:20px;position:relative;overflow-x:hidden}";
  html += "body::before{content:'';position:absolute;top:0;left:0;right:0;bottom:0;background:radial-gradient(circle at 20% 50%,rgba(99,102,241,0.1) 0%,transparent 50%),radial-gradient(circle at 80% 80%,rgba(168,85,247,0.1) 0%,transparent 50%);pointer-events:none}";
  html += ".container{max-width:800px;margin:0 auto;position:relative;z-index:1}";
  html += ".card{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3),inset 0 1px 0 rgba(255,255,255,0.05);transition:all 0.3s ease}";
  html += ".card:hover{border-color:rgba(99,102,241,0.4);box-shadow:0 12px 48px rgba(99,102,241,0.15),inset 0 1px 0 rgba(255,255,255,0.05)}";
  html += ".header{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:32px 28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3);transition:border 0.3s ease;position:relative;overflow:hidden;--status-color:#6366f1}";
  html += ".header::before{content:'';position:absolute;top:0;left:0;right:0;height:4px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transition:all 0.15s ease-out}";
  html += ".header.status-live::before,.header.status-preview::before{background:var(--status-color);box-shadow:0 0 20px var(--status-color)}";
  html += "h1{text-align:center;margin-bottom:8px;font-size:2.2em;background:linear-gradient(135deg,#818cf8 0%,#c084fc 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;font-weight:700;letter-spacing:-0.5px;transition:filter 0.15s ease-out}";
  html += ".header.status-live h1,.header.status-preview h1{background:var(--status-color);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;filter:brightness(1.2)}";
  html += ".subtitle{text-align:center;color:#94a3b8;font-size:0.85em;font-weight:500;margin-bottom:8px}";
  html += ".status-badge{display:inline-block;padding:6px 16px;border-radius:20px;font-size:0.8em;font-weight:600;margin-top:12px;background:rgba(99,102,241,0.2);border:1px solid rgba(99,102,241,0.4);color:#c7d2fe;transition:all 0.15s ease-out}";
  html += ".header.status-live .status-badge,.header.status-preview .status-badge{background:color-mix(in srgb,var(--status-color) 20%,transparent);border-color:color-mix(in srgb,var(--status-color) 40%,transparent);color:var(--status-color);filter:brightness(1.3)}";
  html += ".form-group{margin-bottom:22px}";
  html += "label{display:block;margin-bottom:10px;font-weight:500;color:#cbd5e1;font-size:0.9em;letter-spacing:0.3px}";
  html += "input,select{width:100%;padding:14px 16px;border:1px solid rgba(99,102,241,0.3);border-radius:12px;background:rgba(15,23,42,0.8);color:#e0e7ff;font-size:15px;transition:all 0.3s;outline:none}";
  html += "input:focus,select:focus{border-color:rgba(99,102,241,0.6);background:rgba(15,23,42,0.95);box-shadow:0 0 0 3px rgba(99,102,241,0.1)}";
  html += "input::placeholder{color:#64748b}";
  html += "input[readonly]{background:rgba(15,23,42,0.5);color:#94a3b8;cursor:not-allowed}";
  html += ".color-input{height:56px;cursor:pointer;border-width:2px}";
  html += ".color-input:hover{border-color:rgba(168,85,247,0.5)}";
  html += ".btn{background:linear-gradient(135deg,#6366f1 0%,#8b5cf6 100%);color:#fff;padding:14px 28px;border:none;border-radius:12px;cursor:pointer;font-size:15px;font-weight:600;width:100%;margin-bottom:12px;transition:all 0.3s;box-shadow:0 4px 16px rgba(99,102,241,0.3);position:relative;overflow:hidden}";
  html += ".btn::before{content:'';position:absolute;top:50%;left:50%;width:0;height:0;border-radius:50%;background:rgba(255,255,255,0.2);transform:translate(-50%,-50%);transition:width 0.6s,height 0.6s}";
  html += ".btn:hover::before{width:300px;height:300px}";
  html += ".btn:hover{transform:translateY(-2px);box-shadow:0 6px 24px rgba(99,102,241,0.4)}";
  html += ".btn:active{transform:translateY(0)}";
  html += ".btn-danger{background:linear-gradient(135deg,#ef4444 0%,#dc2626 100%);box-shadow:0 4px 16px rgba(239,68,68,0.3)}";
  html += ".btn-danger:hover{box-shadow:0 6px 24px rgba(239,68,68,0.4)}";
  html += ".btn-warning{background:linear-gradient(135deg,#f59e0b 0%,#d97706 100%);box-shadow:0 4px 16px rgba(245,158,11,0.3)}";
  html += ".btn-warning:hover{box-shadow:0 6px 24px rgba(245,158,11,0.4)}";
  html += "h3{font-size:1.3em;margin:24px 0 20px;padding-bottom:12px;border-bottom:2px solid rgba(99,102,241,0.3);color:#c7d2fe;font-weight:600;letter-spacing:-0.3px}";
  html += ".tabs{display:flex;gap:12px;margin-bottom:24px;flex-wrap:wrap}";
  html += ".tab{background:rgba(15,23,42,0.5);padding:14px 28px;border-radius:12px;cursor:pointer;transition:all 0.3s;border:2px solid rgba(99,102,241,0.2);font-weight:500;color:#94a3b8;position:relative}";
  html += ".tab::after{content:'';position:absolute;bottom:0;left:50%;width:0;height:2px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transform:translateX(-50%);transition:width 0.3s}";
  html += ".tab:hover{background:rgba(15,23,42,0.8);border-color:rgba(99,102,241,0.4);color:#c7d2fe}";
  html += ".tab.active{background:rgba(99,102,241,0.15);border-color:rgba(99,102,241,0.5);color:#e0e7ff;box-shadow:0 4px 16px rgba(99,102,241,0.2)}";
  html += ".tab.active::after{width:80%}";
  html += ".tab-content{display:none;animation:fadeIn 0.4s ease}";
  html += ".tab-content.active{display:block}";
  html += "@keyframes fadeIn{from{opacity:0;transform:translateY(10px)}to{opacity:1;transform:translateY(0)}}";
  html += ".diag-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:14px;margin-top:18px}";
  html += ".diag-item{background:rgba(15,23,42,0.75);border:1px solid rgba(99,102,241,0.18);border-radius:14px;padding:16px 18px}";
  html += ".diag-label{font-size:0.78em;color:#94a3b8;margin-bottom:8px;text-transform:uppercase;letter-spacing:0.08em}";
  html += ".diag-value{font-size:1em;color:#e2e8f0;font-weight:600;word-break:break-word}";
  html += ".diag-meta{color:#94a3b8;font-size:0.88em;margin-top:14px}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='header' id='header'>";
  html += "<h1>VTally-32</h1>";
  html += "<div class='subtitle' id='info'>Professional VMix Tally System</div>";
  html += "<div style='text-align:center'><span class='status-badge' id='statusBadge'>Starting...</span></div>";
  html += "</div>";

  html += "<div class='tabs'>";
  html += "<div class='tab active' onclick='switchTab(0)'>VMix & Colors</div>";
  html += "<div class='tab' onclick='switchTab(1)'>Hardware</div>";
  html += "<div class='tab' onclick='switchTab(2)'>WiFi</div>";
  html += "<div class='tab' onclick='switchTab(3)'>Diagnostics</div>";
  html += "</div>";

  html += "<div class='tab-content active' id='tab0'>";
  html += "<div class='card'>";
  html += "<h3>VMix Settings</h3>";
  html += "<form id='vmixForm'>";
  html += "<div class='form-group'><label>vMix IP</label><input type='text' id='host' required pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'></div>";
  html += "<div class='form-group'><label>vMix HTTP / XML API Port (Web Controller)</label><input type='number' id='port' min='1' max='65535' required></div>";
  html += "<div class='form-group'><label>TCP TALLY Port</label><input type='text' id='tcpPort' value='8099' readonly></div>";
  html += "<div class='form-group'><label>Tracking mode</label><select id='trackingMode'><option value='number'>Fixed input (fast)</option><option value='key'>Tracked source by key</option></select></div>";
  html += "<div class='form-group' id='sourceGroup'><label>Tracked source</label><select id='inputList'><option value=''>Loading...</option></select><button type='button' class='btn' onclick='loadVMixInputs()' style='margin-top:10px'>Refresh sources</button></div>";
  html += "<div class='form-group' id='inputNumberGroup'><label>Fixed input</label><input type='number' id='input' min='1' max='999' required></div>";
  html += "<div class='form-group' id='keyRefreshGroup'><label>Key tracking refresh (seconds)</label><input type='number' id='keyRefreshSeconds' min='2' max='3600' value='10'></div>";
  html += "<div class='form-group' id='keyTitleGroup'><label>Tracked source name</label><input type='text' id='inputTitle' readonly></div>";
  html += "<div class='form-group' id='keyValueGroup'><label>Tracked key</label><input type='text' id='inputKey' readonly></div>";

  html += "<h3>Colors</h3>";
  html += "<div class='form-group'><label>Live</label><input type='color' id='live' class='color-input'></div>";
  html += "<div class='form-group'><label>Preview</label><input type='color' id='preview' class='color-input'></div>";
  html += "<div class='form-group'><label>Off</label><input type='color' id='off' class='color-input'></div>";
  html += "<div class='form-group'><label>Brightness: <span id='bval'>255</span></label><input type='range' id='brightness' min='0' max='255' value='255'></div>";

  html += "<button type='submit' class='btn'>Save Configuration</button>";
  html += "</form></div></div>";

  html += "<div class='tab-content' id='tab1'>";
  html += "<div class='card'>";
  html += "<h3>Hardware Settings</h3>";
  html += "<p style='background:rgba(245,158,11,0.1);padding:16px 18px;border-radius:12px;margin-bottom:24px;border:1px solid rgba(245,158,11,0.3);color:#fcd34d;font-size:0.9em;line-height:1.6'><strong style='color:#fbbf24'>⚠️ Warning:</strong> Any hardware change requires an automatic ESP32 restart.</p>";
  html += "<form id='hardwareForm'>";
  html += "<div class='form-group'><label>LED GPIO Pin</label><input type='number' id='led_pin' min='0' max='48' required></div>";
  html += "<div class='form-group'><label>LED Count</label><input type='number' id='led_count' min='1' max='255' required></div>";
  html += "<button type='submit' class='btn btn-warning'>Save and Restart</button>";
  html += "</form></div></div>";

  html += "<div class='tab-content' id='tab2'>";
  html += "<div class='card'>";
  html += "<h3>WiFi Settings</h3>";
  html += "<div class='form-group'><label>Available networks</label><select id='wifiList' onchange='selectNetwork()'><option value=''>Loading...</option></select><button type='button' class='btn' onclick='scanWiFi()' style='margin-top:10px'>🔄 Scan</button></div>";
  html += "<div class='form-group'><label>SSID (or enter manually)</label><input type='text' id='newssid' placeholder='WiFi network name'></div>";
  html += "<div class='form-group'><label>Password</label><input type='password' id='pwd' placeholder='WiFi password'></div>";
  html += "<button type='button' class='btn' onclick='saveWiFi()'>Save WiFi and Restart</button>";
  html += "<button type='button' class='btn btn-danger' onclick='reboot()'>Restart</button>";
  html += "</div></div>";

  html += "<div class='tab-content' id='tab3'>";
  html += "<div class='card'>";
  html += "<h3>System Diagnostics</h3>";
  html += "<div class='diag-meta' id='diagnosticsUpdated'>Loading diagnostics...</div>";
  html += "<div class='diag-grid' id='diagnosticsGrid'></div>";
  html += "</div></div></div>";

  html += "<script>";
  html += "let currentColors={live:'#ff0000',preview:'#00ff00',off:'#000000'};";
  html += "let vmixInputs=[];";
  html += "function toHex(c){let h=((c||0)>>>0).toString(16).padStart(6,'0');return '#'+h;}";
  html += "document.addEventListener('DOMContentLoaded',function(){";
  html += "fetch('/config').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('host').value=d.vmix_host;";
  html += "document.getElementById('port').value=d.vmix_port;";
  html += "document.getElementById('tcpPort').value=d.vmix_tcp_port||8099;";
  html += "document.getElementById('input').value=d.vmix_input;";
  html += "document.getElementById('trackingMode').value=d.vmix_track_by_key?'key':'number';";
  html += "document.getElementById('keyRefreshSeconds').value=d.vmix_key_refresh_seconds||10;";
  html += "document.getElementById('inputKey').value=d.vmix_input_key||'';";
  html += "document.getElementById('inputTitle').value=d.vmix_input_title||'';";
  html += "document.getElementById('brightness').value=d.brightness;";
  html += "document.getElementById('bval').textContent=d.brightness;";
  html += "document.getElementById('led_pin').value=d.led_pin;";
  html += "document.getElementById('led_count').value=d.led_count;";
  html += "currentColors.live=toHex(d.live_color);";
  html += "currentColors.preview=toHex(d.preview_color);";
  html += "currentColors.off=toHex(d.off_color);";
  html += "document.getElementById('live').value=currentColors.live;";
  html += "document.getElementById('preview').value=currentColors.preview;";
  html += "document.getElementById('off').value=currentColors.off;";
  html += "updateTrackingModeUI();";
  html += "renderVMixInputs([],d.vmix_input,d.vmix_input_key);";
  html += "loadVMixInputs();";
  html += "updateStatus();";
  html += "scanWiFi();";
  html += "updateDiagnostics();";
  html += "});";
  html += "document.getElementById('live').addEventListener('input',e=>{currentColors.live=e.target.value;updateStatus();});";
  html += "document.getElementById('preview').addEventListener('input',e=>{currentColors.preview=e.target.value;updateStatus();});";
  html += "document.getElementById('off').addEventListener('input',e=>{currentColors.off=e.target.value;updateStatus();});";
  html += "document.getElementById('brightness').addEventListener('input',e=>{document.getElementById('bval').textContent=e.target.value;});";
  html += "document.getElementById('trackingMode').addEventListener('change',()=>updateTrackingModeUI());";
  html += "document.getElementById('inputList').addEventListener('change',e=>applySelectedVMixInput(e.target));";
  html += "document.getElementById('vmixForm').addEventListener('submit',e=>{";
  html += "e.preventDefault();";
  html += "const trackByKey=document.getElementById('trackingMode').value==='key';";
  html += "if(trackByKey&&!document.getElementById('inputKey').value){alert('Select a vMix source for key tracking.');return;}";
  html += "const data={vmix_host:document.getElementById('host').value,vmix_port:parseInt(document.getElementById('port').value),vmix_input:document.getElementById('input').value,vmix_track_by_key:trackByKey,vmix_input_key:document.getElementById('inputKey').value,vmix_input_title:document.getElementById('inputTitle').value,vmix_key_refresh_seconds:parseInt(document.getElementById('keyRefreshSeconds').value),live_color:parseInt(currentColors.live.substring(1),16),preview_color:parseInt(currentColors.preview.substring(1),16),off_color:parseInt(currentColors.off.substring(1),16),brightness:parseInt(document.getElementById('brightness').value)};";
  html += "fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Settings saved!');}).catch(e=>alert('Error: '+e));";
  html += "});";
  html += "document.getElementById('hardwareForm').addEventListener('submit',e=>{";
  html += "e.preventDefault();";
  html += "if(confirm('Save hardware configuration? The ESP32 will restart.')){";
  html += "const data={led_pin:parseInt(document.getElementById('led_pin').value),led_count:parseInt(document.getElementById('led_count').value)};";
  html += "fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Settings saved! Restarting...');setTimeout(()=>location.reload(),3000);}).catch(e=>alert('Error: '+e));}";
  html += "});";
  html += "setInterval(updateStatus,1000);";
  html += "setInterval(updateDiagnostics,2000);";
  html += "});";
  html += "function updateTrackingModeUI(){";
  html += "const enabled=document.getElementById('trackingMode').value==='key';";
  html += "document.getElementById('sourceGroup').style.display=enabled?'block':'none';";
  html += "document.getElementById('inputNumberGroup').style.display=enabled?'none':'block';";
  html += "document.getElementById('keyRefreshGroup').style.display=enabled?'block':'none';";
  html += "document.getElementById('keyTitleGroup').style.display=enabled?'block':'none';";
  html += "document.getElementById('keyValueGroup').style.display=enabled?'block':'none';";
  html += "}";
  html += "function applySelectedVMixInput(selectEl){";
  html += "if(!selectEl||selectEl.selectedIndex<0)return;";
  html += "const option=selectEl.options[selectEl.selectedIndex];";
  html += "const key=option.dataset.key||'';";
  html += "const number=option.dataset.number||'';";
  html += "const title=option.dataset.title||'';";
  html += "if(number){document.getElementById('input').value=number;}";
  html += "document.getElementById('inputKey').value=key;";
  html += "document.getElementById('inputTitle').value=title;";
  html += "}";
  html += "function renderVMixInputs(inputs,selectedInput,selectedKey){";
  html += "const list=document.getElementById('inputList');";
  html += "if(!list)return;";
  html += "vmixInputs=Array.isArray(inputs)?inputs:[];";
  html += "list.innerHTML='';";
  html += "const placeholder=document.createElement('option');";
  html += "placeholder.value='';";
  html += "placeholder.textContent='-- Select a vMix source --';";
  html += "list.appendChild(placeholder);";
  html += "vmixInputs.forEach(item=>{";
  html += "const opt=document.createElement('option');";
  html += "opt.value=item.key||item.number;";
  html += "opt.dataset.key=item.key||'';";
  html += "opt.dataset.number=item.number||'';";
  html += "opt.dataset.title=item.title||'';";
  html += "const title=item.title&&item.title.length>0?item.title:'Input '+item.number;";
  html += "opt.textContent=item.number+' - '+title;";
  html += "list.appendChild(opt);";
  html += "});";
  html += "if(selectedKey){list.value=String(selectedKey);}";
  html += "if(!list.value&&selectedInput){for(let i=0;i<list.options.length;i++){if((list.options[i].dataset.number||'')===String(selectedInput)){list.selectedIndex=i;break;}}}";
  html += "if(!list.value&&selectedInput){";
  html += "const manual=document.createElement('option');";
  html += "manual.value=String(selectedKey||selectedInput);";
  html += "manual.dataset.key=String(selectedKey||'');";
  html += "manual.dataset.number=String(selectedInput);";
  html += "manual.dataset.title=document.getElementById('inputTitle').value||'Source not found';";
  html += "manual.textContent=String(selectedInput)+' - Source not found';";
  html += "list.appendChild(manual);";
  html += "list.value=String(selectedKey||selectedInput);";
  html += "}";
  html += "applySelectedVMixInput(list);";
  html += "}";
  html += "function loadVMixInputs(){";
  html += "const list=document.getElementById('inputList');";
  html += "const host=document.getElementById('host').value;";
  html += "const port=document.getElementById('port').value;";
  html += "const selected=document.getElementById('input').value;";
  html += "const selectedKey=document.getElementById('inputKey').value;";
  html += "list.innerHTML='<option value=\"\">Loading sources...</option>';";
  html += "fetch('/vmix/inputs?host='+encodeURIComponent(host)+'&port='+encodeURIComponent(port)).then(r=>r.json()).then(d=>{";
  html += "if(d.inputs){renderVMixInputs(d.inputs,d.selected_input||selected,d.selected_key||selectedKey);}else{renderVMixInputs([],selected,selectedKey);list.options[0].textContent=d.error||'Unable to load sources';}}).catch(e=>{renderVMixInputs([],selected,selectedKey);list.options[0].textContent='Loading error';console.error('vMix inputs error:',e);});";
  html += "}";
  html += "function updateStatus(){";
  html += "fetch('/status').then(r=>r.json()).then(d=>{";
  html += "const header=document.getElementById('header'),badge=document.getElementById('statusBadge');";
  html += "document.getElementById('info').textContent='WiFi: '+(d.wifi_ssid||'AP Mode')+' | IP: '+(d.wifi_ip||'192.168.4.1');";
  html += "if(d.connected){";
  html += "if(d.live){";
  html += "header.className='header status-live';";
  html += "header.style.setProperty('--status-color',currentColors.live);";
  html += "badge.textContent='🔴 LIVE';";
  html += "}else if(d.preview){";
  html += "header.className='header status-preview';";
  html += "header.style.setProperty('--status-color',currentColors.preview);";
  html += "badge.textContent='🟢 PREVIEW';";
  html += "}else{";
  html += "header.className='header';";
  html += "header.style.removeProperty('--status-color');";
  html += "badge.textContent='✓ Connected';";
  html += "}";
  html += "}else{";
  html += "header.className='header';";
  html += "header.style.removeProperty('--status-color');";
  html += "badge.textContent='⚠ Disconnected';";
  html += "}";
  html += "}).catch(e=>console.error('Status error:',e));}";
  html += "function renderDiagnostics(d){";
  html += "const grid=document.getElementById('diagnosticsGrid');";
  html += "if(!grid)return;";
  html += "const items=[";
  html += "['Firmware',d.firmware_version],";
  html += "['Uptime',d.uptime_human],";
  html += "['Free heap',d.free_heap+' B'],";
  html += "['Min heap',d.min_free_heap+' B'],";
  html += "['Max block',d.max_alloc_heap+' B'],";
  html += "['WiFi mode',d.wifi_mode],";
  html += "['AP active',d.ap_active?'Yes':'No'],";
  html += "['STA connected',d.sta_connected?'Yes':'No'],";
  html += "['SSID',d.wifi_ssid||'None'],";
  html += "['IP STA',d.wifi_ip||'-'],";
  html += "['IP AP',d.ap_ip||'-'],";
  html += "['RSSI',d.sta_connected?(d.wifi_rssi+' dBm'):'N/A'],";
  html += "['VMix',d.vmix_connected?'Connected':'Disconnected'],";
  html += "['VMix socket',d.vmix_socket_connected?'Open':'Closed'],";
  html += "['VMix API HTTP',d.vmix_host+':'+(d.vmix_api_port||d.vmix_port)],";
  html += "['VMix TCP TALLY',d.vmix_host+':'+(d.vmix_tcp_port||8099)],";
  html += "['Tracking mode',d.vmix_track_by_key?'Tracked source by key':'Fixed input'],";
  html += "['Configured fixed input',d.vmix_input],";
  html += "['Resolved current input',d.resolved_vmix_input||'-'],";
  html += "['Tracked source',d.resolved_vmix_title||d.vmix_input_title||'-'],";
  html += "['Tally',d.tally_state],";
  html += "['LEDs',d.led_count+' on GPIO '+d.led_pin],";
  html += "['Brightness',d.brightness]";
  html += "];";
  html += "grid.innerHTML=items.map(item=>'<div class=\"diag-item\"><div class=\"diag-label\">'+item[0]+'</div><div class=\"diag-value\">'+item[1]+'</div></div>').join('');";
  html += "document.getElementById('diagnosticsUpdated').textContent='Last update: '+new Date().toLocaleTimeString();";
  html += "}";
  html += "function updateDiagnostics(){";
  html += "fetch('/diagnostics').then(r=>r.json()).then(d=>renderDiagnostics(d)).catch(e=>{const el=document.getElementById('diagnosticsUpdated');if(el){el.textContent='Diagnostics error';}console.error('Diagnostics error:',e);});";
  html += "}";
  html += "function saveWiFi(){";
  html += "const ssid=document.getElementById('newssid').value,pwd=document.getElementById('pwd').value;";
  html += "if(!ssid){alert('Enter an SSID');return;}";
  html += "fetch('/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:ssid,password:pwd})}).then(()=>{alert('WiFi configured! Restarting...');setTimeout(()=>location.reload(),3000)}).catch(e=>alert('Error: '+e));";
  html += "}";
  html += "function reboot(){if(confirm('Restart?')){fetch('/reboot',{method:'POST'}).then(()=>alert('Restarting...'));}}";
  html += "function scanWiFi(){";
  html += "const list=document.getElementById('wifiList');";
  html += "list.innerHTML='<option value=\"\">Scanning...</option>';";
  html += "fetch('/scan').then(r=>r.json()).then(d=>{";
  html += "if(d.networks&&d.networks.length>0){";
  html += "list.innerHTML='<option value=\"\">-- Select a network --</option>';";
  html += "d.networks.forEach(n=>{";
  html += "const opt=document.createElement('option');";
  html += "opt.value=n.ssid;";
  html += "opt.textContent=n.ssid+' ('+n.rssi+' dBm)'+(n.secure?' 🔒':'');";
  html += "list.appendChild(opt);";
  html += "});";
  html += "}else{list.innerHTML='<option value=\"\">No networks found</option>';};";
  html += "}).catch(e=>{list.innerHTML='<option value=\"\">Scan error</option>';console.error(e);});";
  html += "}";
  html += "function selectNetwork(){";
  html += "const ssid=document.getElementById('wifiList').value;";
  html += "if(ssid)document.getElementById('newssid').value=ssid;";
  html += "}";
  html += "function switchTab(index){";
  html += "const tabs=document.querySelectorAll('.tab');";
  html += "const contents=document.querySelectorAll('.tab-content');";
  html += "tabs.forEach((t,i)=>{if(i===index){t.classList.add('active');}else{t.classList.remove('active');}});";
  html += "contents.forEach((c,i)=>{if(i===index){c.classList.add('active');}else{c.classList.remove('active');}});";
  html += "if(index===3){updateDiagnostics();}";
  html += "}";
  html += "</script></body></html>";

  server.send(200, "text/html", html);
}

void handleConfig() {
  if (server.method() == HTTP_GET) {
    StaticJsonDocument<1024> doc;
    doc["vmix_host"] = config.vmix_host;
    doc["vmix_port"] = config.vmix_port;
    doc["vmix_tcp_port"] = VMIX_TCP_PORT;
    doc["vmix_input"] = config.vmix_input;
    doc["vmix_track_by_key"] = config.vmix_track_by_key;
    doc["vmix_input_key"] = config.vmix_input_key;
    doc["vmix_input_title"] = config.vmix_input_title;
    doc["vmix_key_refresh_seconds"] = config.vmix_key_refresh_seconds;
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
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      LOG_ERROR("JSON error: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    LOG_WEB("Received configuration:");
    serializeJsonPretty(doc, Serial);
    Serial.println();

    Config updatedConfig = config;
    bool vmixConfigChanged = false;
    bool hardwareConfigChanged = false;

    if (doc.containsKey("vmix_host")) {
      String vmixHost = doc["vmix_host"] | "";
      vmixHost.trim();
      if (vmixHost.length() == 0 || vmixHost.length() >= sizeof(updatedConfig.vmix_host)) {
        server.send(400, "application/json", "{\"error\":\"Invalid vMix host\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_host, vmixHost.c_str(), sizeof(updatedConfig.vmix_host));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_port")) {
      int vmixPort = doc["vmix_port"].as<int>();
      if (!isValidPort(vmixPort)) {
        server.send(400, "application/json", "{\"error\":\"Invalid vMix port\"}");
        return;
      }
      updatedConfig.vmix_port = vmixPort;
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_input")) {
      String vmixInput = doc["vmix_input"] | "";
      vmixInput.trim();
      if (!isValidVMixInput(vmixInput.c_str())) {
        server.send(400, "application/json", "{\"error\":\"Invalid vMix input\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_input, vmixInput.c_str(), sizeof(updatedConfig.vmix_input));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_track_by_key")) {
      updatedConfig.vmix_track_by_key = doc["vmix_track_by_key"].as<bool>();
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_input_key")) {
      String vmixInputKey = doc["vmix_input_key"] | "";
      vmixInputKey.trim();
      if (vmixInputKey.length() >= sizeof(updatedConfig.vmix_input_key)) {
        server.send(400, "application/json", "{\"error\":\"Invalid vMix key\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_input_key, vmixInputKey.c_str(), sizeof(updatedConfig.vmix_input_key));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_input_title")) {
      String vmixInputTitle = doc["vmix_input_title"] | "";
      vmixInputTitle.trim();
      if (vmixInputTitle.length() >= sizeof(updatedConfig.vmix_input_title)) {
        server.send(400, "application/json", "{\"error\":\"Invalid vMix title\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_input_title, vmixInputTitle.c_str(), sizeof(updatedConfig.vmix_input_title));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_key_refresh_seconds")) {
      int refreshSeconds = doc["vmix_key_refresh_seconds"].as<int>();
      if (!isValidVMixKeyRefreshSeconds(refreshSeconds)) {
        server.send(400, "application/json", "{\"error\":\"Invalid key refresh interval\"}");
        return;
      }
      updatedConfig.vmix_key_refresh_seconds = refreshSeconds;
      vmixConfigChanged = true;
    }

    if (updatedConfig.vmix_track_by_key && strlen(updatedConfig.vmix_input_key) == 0) {
      server.send(400, "application/json", "{\"error\":\"Select a vMix source for key tracking\"}");
      return;
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
        server.send(400, "application/json", "{\"error\":\"Invalid brightness\"}");
        return;
      }
      updatedConfig.brightness = brightness;
    }
    if (doc.containsKey("led_pin")) {
      int ledPin = doc["led_pin"].as<int>();
      if (!isValidLedPin(ledPin)) {
        server.send(400, "application/json", "{\"error\":\"Invalid LED pin\"}");
        return;
      }
      updatedConfig.led_pin = ledPin;
      hardwareConfigChanged = true;
    }
    if (doc.containsKey("led_count")) {
      int ledCount = doc["led_count"].as<int>();
      if (!isValidLedCount(ledCount)) {
        server.send(400, "application/json", "{\"error\":\"Invalid LED count\"}");
        return;
      }
      updatedConfig.led_count = ledCount;
      hardwareConfigChanged = true;
    }

    config = updatedConfig;
    resetResolvedVMixInput();

    LOG_DEBUG("Values before save:");
    LOG_DEBUG("  VMix API: %s:%d TCP TALLY:%d Input:%s TrackByKey:%s Key:%s Refresh:%ds",
              config.vmix_host,
              config.vmix_port,
              VMIX_TCP_PORT,
              config.vmix_input,
              config.vmix_track_by_key ? "YES" : "NO",
              config.vmix_input_key,
              config.vmix_key_refresh_seconds);
    LOG_DEBUG("  Colors: Live=0x%06X Preview=0x%06X Off=0x%06X Brightness=%d",
              config.live_color, config.preview_color, config.off_color, config.brightness);
    LOG_DEBUG("  Hardware: Pin=%d Count=%d", config.led_pin, config.led_count);

    saveConfig();

    if (vmixConfigChanged) {
      if (vmixClient.connected()) {
        vmixClient.stop();
      }
      vmixConnected = false;
      setTally(false, false);
      lastVMixCheck = 0;
      LOG_VMIX("VMix configuration changed, reconnecting...");
    }

    if (!hardwareConfigChanged) {
      applyTallyState();
    }

    if (hardwareConfigChanged) {
      server.send(200, "application/json", "{\"status\":\"ok\",\"reboot\":true}");
      LOG_INFO("Hardware configuration changed, restarting...");
      delay(500);
      ESP.restart();
    } else {
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    }
  }
}

void handleVMixInputs() {
  String host = server.hasArg("host") ? server.arg("host") : String(config.vmix_host);
  host.trim();

  int port = config.vmix_port;
  if (server.hasArg("port")) {
    String portArg = server.arg("port");
    portArg.trim();
    if (portArg.length() > 0) {
      port = portArg.toInt();
    }
  }

  if (host.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"Missing vMix host\"}");
    return;
  }

  if (!isValidPort(port)) {
    server.send(400, "application/json", "{\"error\":\"Invalid vMix API port\"}");
    return;
  }

  String xml;
  String errorMessage;
  if (!fetchVMixApiXml(host.c_str(), port, xml, errorMessage)) {
    DynamicJsonDocument errorDoc(256);
    errorDoc["error"] = errorMessage;
    errorDoc["selected_input"] = config.vmix_input;
    errorDoc["selected_key"] = config.vmix_input_key;
    errorDoc["selected_title"] = config.vmix_input_title;
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    server.send(502, "application/json", errorResponse);
    return;
  }

  int inputCount = countVMixInputsInXml(xml);
  size_t capacity = JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(inputCount) + (inputCount * JSON_OBJECT_SIZE(4)) + (inputCount * 96) + 256;
  DynamicJsonDocument doc(capacity);
  doc["selected_input"] = config.vmix_input;
  doc["selected_key"] = config.vmix_input_key;
  doc["selected_title"] = config.vmix_input_title;
  doc["track_by_key"] = config.vmix_track_by_key;
  JsonArray inputs = doc.createNestedArray("inputs");

  if (!fillVMixInputsFromXml(xml, inputs)) {
    doc["error"] = "No vMix sources found";
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleWiFi() {
  if (server.method() == HTTP_POST) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      LOG_ERROR("WiFi JSON error: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";
    ssid.trim();

    if (ssid.length() > 0 && ssid.length() < sizeof(config.wifi_ssid) && password.length() < sizeof(config.wifi_password)) {
      LOG_WEB("Saving WiFi: %s", ssid.c_str());
      strlcpy(config.wifi_ssid, ssid.c_str(), sizeof(config.wifi_ssid));
      strlcpy(config.wifi_password, password.c_str(), sizeof(config.wifi_password));
      saveConfig();

      server.send(200, "application/json", "{\"status\":\"ok\"}");
      delay(500);
      ESP.restart();
    } else {
      server.send(400, "application/json", "{\"error\":\"SSID is required or invalid\"}");
    }
  }
}

void handleStatus() {
  StaticJsonDocument<256> doc;
  doc["connected"] = vmixConnected;
  doc["live"] = isLive;
  doc["preview"] = isPreview;
  doc["vmix_host"] = config.vmix_host;
  doc["vmix_port"] = config.vmix_port;
  doc["vmix_input"] = config.vmix_input;
  doc["wifi_ssid"] = WiFi.SSID();
  doc["wifi_ip"] = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "";

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleScan() {
  LOG_WEB("WiFi scan requested...");
  int n = WiFi.scanNetworks();
  LOG_WEB("%d networks found", n);

  int networkCount = n > 0 ? n : 0;
  size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(networkCount) + (networkCount * JSON_OBJECT_SIZE(3)) + 128;
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
  server.send(200, "application/json", "{\"status\":\"rebooting\"}");
  delay(500);
  ESP.restart();
}
