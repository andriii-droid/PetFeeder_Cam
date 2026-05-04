#ifndef HTML_H // "If Not Defined"
#define HTML_H
#include <AsyncTCP.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
<meta charset="UTF-8">
<style>
  body { font-family: sans-serif; margin: 0; display: flex; flex-direction: column; height: 100vh; background: #eee; overflow: hidden; }
  
  /* Header */
  #header { background: #50B8B4; color: white; padding: 15px; text-align: center; font-size: 1.5rem; font-weight: bold; flex-shrink: 0; }
  
  /* Layout Wrapper */
  #wrapper { display: flex; flex-grow: 1; height: 100%%; overflow: hidden; }

  /* Sidebar */
  #side { width: 70px; background: #222; color: #fff; display: flex; flex-direction: column; align-items: center; padding: 20px 0; flex-shrink: 0; }
  #track { width: 20px; height: 60vh; background: #444; border-radius: 10px; display: flex; flex-direction: column-reverse; }
  #bar { width: 100%%; height: 0px; background: #00acc1; border-radius: 10px; transition: 0.3s; }

  /* Main Content - Tightened vertical spacing */
  #main { 
    flex-grow: 1; 
    display: flex; 
    flex-direction: column; 
    justify-content: flex-start; /* Align to top instead of spreading out */
    align-items: center; 
    padding: 20px; 
    gap: 10px; /* Controlled distance between sliders */
    box-sizing: border-box;
    overflow-y: auto;
  }
  
  .box { 
    background: #fff; 
    padding: 15px 20px; /* Reduced vertical padding */
    border-radius: 10px; 
    box-shadow: 0 2px 5px rgba(0,0,0,0.05); 
    width: 98%%; 
    display: flex; 
    align-items: center; 
    gap: 15px; 
    box-sizing: border-box;
    flex-shrink: 0;
  }
  
  .label-text { font-weight: bold; width: 60px; color: #333; flex-shrink: 0; font-size: 0.9rem; }
  .small-text-wrapper {
    width: 98%%;           /* Match the .box width */
    text-align: right;    /* Push content to the right */
    padding-right: 5px;   /* Slight offset from the edge */
    margin-top: -5px;     /* Pull it closer to the box above if desired */
  }

  .small-text { 
    color: #888;          /* Slightly lighter for "subtle" look */
    font-size: 0.6rem; 
    font-family: monospace;
  }

  /* Horizontal Slider */
  input[type=range] { 
    flex-grow: 1; 
    height: 10px; 
    cursor: pointer; 
    accent-color: #50B8B4;
  }

  /* Toggle Switch */
  .sw { position: relative; display: block; width: 44px; height: 22px; flex-shrink: 0; }
  .sw input { opacity: 0; width: 0; height: 0; }
  .sld { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background: #ccc; border-radius: 34px; transition: 0.4s; }
  .sld:before { position: absolute; content: ""; height: 16px; width: 16px; left: 3px; bottom: 3px; background: #fff; border-radius: 50%%; transition: 0.4s; }
  input:checked + .sld { background: #50B8B4; }
  input:checked + .sld:before { transform: translateX(22px); }
</style>
</head><body>
  <div id="header">PetFeeder</div>
  <div id="wrapper">
    <div id="side">
      <div id="track"><div id="bar"></div></div>
      <div style="margin-top:10px; font-size: 14px;"><span id="v">0</span>%%</div>
    </div>
    <div id="main">
        <div class="box">
            <span class="label-text">Morgen</span>
            <input type="range" min="0" max="100" id="sMor" class="slider-input">
            <label class="sw"><input type="checkbox" id="tMor" class="toggle-input"><span class="sld"></span></label>
        </div>
        <div class="box">
            <span class="label-text">Mittag</span>
            <input type="range" min="0" max="100" id="sNoo" class="slider-input">
            <label class="sw"><input type="checkbox" id="tNoo" class="toggle-input"><span class="sld"></span></label>
        </div>
        <div class="box">
            <span class="label-text">Abend</span>
            <input type="range" min="0" max="100" id="sEve" class="slider-input">
            <label class="sw"><input type="checkbox" id="tEve" class="toggle-input"><span class="sld"></span></label>
        </div>
        <div class="small-text-wrapper">
            <span class="small-text">Last updated: <span id="last_update">%last%</span></span>
        </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
  var src = new EventSource('/events');

  // Sidebar (Fodder Level)
  src.addEventListener('fodderAmount', function(e) {
    var n = parseFloat(e.data);
    var p = String.fromCharCode(37); 
    document.getElementById("bar").style.height = n + p;
    document.getElementById("v").innerHTML = Math.round(n);
  }, false);

    // Last updated
  src.addEventListener('lastUpdate', function(e) {
    document.getElementById("last_update").innerHTML = e.data;
  }, false);

  // Helper to handle Sliders
  function setupSlider(eventLabel, elementId) {
    src.addEventListener(eventLabel, function(e) {
      document.getElementById(elementId).value = e.data;
    }, false);
  }

  // Helper to handle Toggles
  function setupToggle(eventLabel, elementId) {
    src.addEventListener(eventLabel, function(e) {
      document.getElementById(elementId).checked = (e.data === '1' || e.data === 'true');
    }, false);
  }

  // Initialize all listeners
  setupSlider('sMor', 'sMor');
  setupSlider('sNoo', 'sNoo');
  setupSlider('sEve', 'sEve');
  
  setupToggle('tMor', 'tMor');
  setupToggle('tNoo', 'tNoo');
  setupToggle('tEve', 'tEve');
}

//Input Handling
function updateESP(id, value) {
  console.log("Updating " + id + " to " + value);
  // Sends a request like: /update?id=s1&val=45
  fetch(`/update?id=${id}&val=${value}`);
}

// Attach listeners to all Sliders
document.querySelectorAll('.slider-input').forEach(item => {
  item.addEventListener('input', event => {
    updateESP(event.target.id, event.target.value);
  });
});

// Attach listeners to all Toggles
document.querySelectorAll('.toggle-input').forEach(item => {
  item.addEventListener('change', event => {
    let val = event.target.checked ? 1 : 0;
    updateESP(event.target.id, val);
  });
});
</script>
</body></html>)rawliteral";

#endif // HTML_H