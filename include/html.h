#include <AsyncTCP.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
<meta charset="UTF-8">
<style>
  body { font-family: sans-serif; margin: 0; display: flex; flex-direction: column; height: 100vh; background: #eee; overflow: hidden; }
  
  /* Header */
  #header { background: #50B8B4; color: white; padding: 15px; text-align: center; font-size: 1.5rem; font-weight: bold; flex-shrink: 0; }
  
  /* Layout Wrapper */
  #wrapper { display: flex; flex-grow: 1; height: 100%; overflow: hidden; }

  /* Sidebar */
  #side { width: 70px; background: #222; color: #fff; display: flex; flex-direction: column; align-items: center; padding: 20px 0; flex-shrink: 0; }
  #track { width: 20px; height: 60vh; background: #444; border-radius: 10px; display: flex; flex-direction: column-reverse; }
  #bar { width: 100%; height: 0px; background: #00acc1; border-radius: 10px; transition: 0.3s; }

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
    width: 98%; 
    display: flex; 
    align-items: center; 
    gap: 15px; 
    box-sizing: border-box;
    flex-shrink: 0;
  }
  
  .label-text { font-weight: bold; width: 60px; color: #333; flex-shrink: 0; font-size: 0.9rem; }

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
  .sld:before { position: absolute; content: ""; height: 16px; width: 16px; left: 3px; bottom: 3px; background: #fff; border-radius: 50%; transition: 0.4s; }
  input:checked + .sld { background: #50B8B4; }
  input:checked + .sld:before { transform: translateX(22px); }
</style>
</head><body>
  <div id="header">PetFeeder</div>
  <div id="wrapper">
    <div id="side">
      <div id="track"><div id="bar"></div></div>
      <div style="margin-top:10px; font-size: 14px;"><span id="v">0</span>%</div>
    </div>
    <div id="main">
      <div class="box">
        <span class="label-text">Morgen</span>
        <input type="range" min="0" max="100" value="50">
        <label class="sw"><input type="checkbox"><span class="sld"></span></label>
      </div>
      <div class="box">
        <span class="label-text">Mittag</span>
        <input type="range" min="0" max="100" value="50">
        <label class="sw"><input type="checkbox"><span class="sld"></span></label>
      </div>
      <div class="box">
        <span class="label-text">Abend</span>
        <input type="range" min="0" max="100" value="50">
        <label class="sw"><input type="checkbox"><span class="sld"></span></label>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
  var src = new EventSource('/events');
  src.addEventListener('temperature', function(e) {
    var n = parseFloat(e.data);
    var p = String.fromCharCode(37); 
    document.getElementById("bar").style.height = n + p;
    document.getElementById("v").innerHTML = Math.round(n);
  }, false);
}
</script>
</body></html>)rawliteral";