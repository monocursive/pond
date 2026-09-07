
const primary=await figma.getNodeByIdAsync("10:6"),secondary=await figma.getNodeByIdAsync("10:12"),destructive=await figma.getNodeByIdAsync("10:18"),disabled=await figma.getNodeByIdAsync("10:10");
const navRow=await figma.getNodeByIdAsync("11:2"),choiceOn=await figma.getNodeByIdAsync("11:8"),choiceOff=await figma.getNodeByIdAsync("11:13");
function action(parent,label,kind="Primary"){const comp=kind==="Secondary"?secondary:kind==="Destructive"?destructive:kind==="Disabled"?disabled:primary;const n=track(comp.createInstance());parent.appendChild(n);const key=Object.keys(n.componentProperties).find(k=>k.startsWith("Label#"));n.setProperties({[key]:label});n.name="Action / "+label;return n;}
function setting(parent,label,detail,kind="Navigate"){const comp=kind==="Selected"?choiceOn:kind==="Unselected"?choiceOff:navRow;const n=track(comp.createInstance());parent.appendChild(n);const props={};for(const k of Object.keys(n.componentProperties)){if(k.startsWith("Title#"))props[k]=label;if(k.startsWith("Detail#"))props[k]=detail;}n.setProperties(props);n.name="Setting / "+label;return n;}
function phone(name,x,y,back=true){const f=stack(figma.currentPage,name,390,0,0,"u/paper");f.resize(390,844);f.primaryAxisSizingMode="FIXED";f.counterAxisSizingMode="FIXED";f.x=x;f.y=y;f.clipsContent=true;
const header=row(f,"Pebble settings / host navigation",0);header.resize(390,64);header.primaryAxisSizingMode="FIXED";header.counterAxisSizingMode="FIXED";header.paddingLeft=header.paddingRight=24;header.primaryAxisAlignItems="SPACE_BETWEEN";fill(header,"u/paper");const b=text(header,back?"‹ Back":"Pebble","Small",90);const title=text(header,"Pond","Body",100);title.textAlignHorizontal="CENTER";const end=text(header,"Settings","Label",90,"u/muted");end.textAlignHorizontal="RIGHT";
const viewport=track(figma.createFrame());viewport.name="Scrollable settings";viewport.resize(390,680);viewport.fills=[];viewport.clipsContent=true;viewport.overflowDirection="VERTICAL";f.appendChild(viewport);
const content=stack(viewport,"Settings content",390,16,24);content.paddingBottom=40;
const footer=stack(f,"Action area",390,0,24,"u/paper");footer.resize(390,100);footer.primaryAxisSizingMode="FIXED";
return {f,header,back:b,content,footer,viewport};
}
function note(parent,title,body,color="u/wash"){const n=stack(parent,title,342,8,16,color);n.setBoundVariable("cornerRadius",vars["u/radius/12"]);text(n,title,"Body",310);text(n,body,"Small",310,"u/muted");return n;}
async function navigate(node,to){await node.setReactionsAsync([{trigger:{type:"ON_CLICK"},actions:[{type:"NODE",destinationId:to,navigation:"NAVIGATE",transition:null,resetScrollPosition:true}]}]);mutated.push(node.id);}
