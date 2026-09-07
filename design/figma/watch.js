
const pebble=await figma.getNodeByIdAsync("5:6");
function makeWatch(parent,state="Still",w=144,h=168,round=false){
const c=track(figma.createComponent());c.name="State="+state;c.resize(w,h);fill(c,"w/background");c.clipsContent=true;if(round)c.cornerRadius=w/2;
parent.appendChild(c);
const k=w>=240?8:w>=200?7:5;
const date=pixels(c,"MON 07",2);date.x=Math.floor((w-date.width)/2);date.y=round?Math.round(h*.15):12;
const clock=pixels(c,"10:08",k);clock.x=Math.floor((w-clock.width)/2);clock.y=round?Math.round(h*.30):w>=200?46:38;
const pond=track(figma.createFrame());pond.name="Pond / fixed pixel canvas";pond.resize(w,round?80:w>=200?84:72);pond.fills=[];pond.clipsContent=false;c.appendChild(pond);pond.y=round?(w===180?94:140):w>=200?113:80;
const isRipple=["Ripple","Echo","Reduced"].includes(state),offline=state==="Offline",pending=["Preparing","Sending","Unknown"].includes(state);
if(!offline){const n=ring(pond,w>=200?104:76,w>=240?16:w>=200?24:w===180?14:18);n.x=Math.floor((w-n.width)/2);n.y=w>=240?52:w>=200?44:w===180?29:38;
if(isRipple&&state!=="Reduced"){const o=ring(pond,w>=200?168:120,w>=240?28:w>=200?44:w===180?24:28,"w/ripple");o.x=Math.floor((w-o.width)/2);o.y=w>=240?46:w>=200?34:w===180?23:30;}
}
const stone=track(pebble.createInstance());pond.appendChild(stone);const scale=w>=240?2:1;stone.rescale(scale);stone.x=Math.floor((w-stone.width)/2);stone.y=w>=240?0:w>=200?14:w===180?2:12;
if(state==="Preparing")stone.y-=5;
if(state==="Accepted")stone.y+=3;
if(offline){const a=rect(pond,"Disconnected / left bank",22,2,"w/ink");a.x=w/2-34;a.y=w>=240?64:w===180?40:47;const b=rect(pond,"Disconnected / right bank",22,2,"w/ink");b.x=w/2+12;b.y=w>=240?64:w===180?40:47;}
if(pending){for(let i=0;i<3;i++){const d=rect(pond,"Pending dot "+i,2,2,"w/ink");d.x=w/2-9+i*8;d.y=54;}}
const labels={Still:"STILL",Ripple:"RIPPLE",Echo:"ECHO",Reduced:"RIPPLE",Preparing:"DROP",Sending:"SENDING",Accepted:"DROPPED",Unknown:"UNKNOWN",Offline:"OFFLINE",Setup:"SETUP",Paused:"PAUSED",Night:"QUIET"};
const status=pixels(c,labels[state],2);status.x=Math.floor((w-status.width)/2);status.y=round?(w===180?150:224):h-24;
if(state==="Reduced"){const dot=rect(pond,"Recent ripple / static",4,4,"w/ink");dot.x=w/2+25;dot.y=17;}
c.description="Pond watchface at "+w+" × "+h+" native pixels. State "+state+". Non-interactive face: no custom button/touch targets. Gesture feedback is specified on the interaction page. Watch mode controls colours.";
return c;
}
