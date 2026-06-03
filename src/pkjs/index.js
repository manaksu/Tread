/*
 * TreadWatch — PebbleKit JS
 * AppMessage keys:
 *   0 = CLOCK_STYLE  : 0=Digital  1=Plate
 *   1 = BAT_STYLE    : 0=Default  1=Bike Wheel  2=Vanishing Road
 */

function loadCfg() {
  return {
    clockStyle: +(localStorage.getItem('tw_cs') || '0'),
    batStyle:   +(localStorage.getItem('tw_bs') || '0')
  };
}

function saveCfg(c) {
  localStorage.setItem('tw_cs', c.clockStyle);
  localStorage.setItem('tw_bs', c.batStyle);
}

function sendMsg(c) {
  Pebble.sendAppMessage(
    { '0': c.clockStyle, '1': c.batStyle },
    function() { console.log('TreadWatch: sent ok'); },
    function(e) { console.log('TreadWatch: failed', JSON.stringify(e)); }
  );
}

function buildConfig(c) {
  function radio(name, opts, sel) {
    return opts.map(function(l, i) {
      return '<label class="opt"><input type="radio" name="' + name + '" value="' + i + '"' +
             (i === sel ? ' checked' : '') + '><span>' + l + '</span></label>';
    }).join('');
  }

  var h = '<!DOCTYPE html><html><head>'
    + '<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">'
    + '<style>'
    + 'body{margin:0;font:15px/1.6 -apple-system,sans-serif;background:#f2f1ed;color:#1a1a1a;padding:20px 20px 40px}'
    + 'h3{font-size:10px;text-transform:uppercase;letter-spacing:.12em;color:#8a7060;margin:24px 0 8px}'
    + 'h3:first-child{margin-top:0}'
    + '.opt{display:flex;align-items:center;gap:12px;background:rgba(255,255,255,0.6);border-radius:8px;padding:13px;margin:5px 0;cursor:pointer;border:1.5px solid transparent}'
    + '.opt:has(input:checked){border-color:#321c14;background:rgba(255,255,255,0.9)}'
    + '.opt input{accent-color:#321c14;width:18px;height:18px;flex-shrink:0;margin:0}'
    + '.opt span{font-size:14px;color:#1a1a1a}'
    + '#s{display:block;width:100%;padding:14px;background:#321c14;color:#f2f1ed;border:none;'
    +    'border-radius:8px;font-size:14px;letter-spacing:.06em;text-transform:uppercase;margin-top:28px;cursor:pointer;box-sizing:border-box}'
    + '</style></head><body>'

    + '<h3>Clock style</h3>'
    + radio('clockStyle', ['Digital — HH / MM stacked', 'Plate — Dutch motorbike plate'], c.clockStyle)

    + '<h3>Battery indicator</h3>'
    + radio('batStyle', ['Default — standard Pebble battery', 'Bike Wheel — spoke fill', 'Vanishing Road — road fades'], c.batStyle)

    + '<button id="s">Save</button>'
    + '<script>'
    + 'function g(n){var e=document.querySelector("input[name="+n+"]:checked");return e?+e.value:0;}'
    + 'document.getElementById("s").onclick=function(){'
    +   'location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify({'
    +   'clockStyle:g("clockStyle"),batStyle:g("batStyle")}));'
    + '};<\/script></body></html>';

  return 'data:text/html,' + encodeURIComponent(h);
}

Pebble.addEventListener('ready', function() {
  console.log('TreadWatch: ready');
  sendMsg(loadCfg());
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(buildConfig(loadCfg()));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response || e.response === '' || e.response === 'CANCELLED') return;
  var raw = e.response;
  if (raw.indexOf('#') !== -1) raw = raw.substring(raw.lastIndexOf('#') + 1);
  var c; try { c = JSON.parse(decodeURIComponent(raw)); } catch(err) { return; }
  saveCfg(c);
  sendMsg(c);
});
