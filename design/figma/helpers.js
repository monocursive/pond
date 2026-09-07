
const created=[],mutated=[];
const fonts=[{family:"Manrope",style:"Medium"},{family:"Manrope",style:"SemiBold"},{family:"Manrope",style:"Regular"},{family:"IBM Plex Mono",style:"Regular"}];
await Promise.all(fonts.map(f=>figma.loadFontAsync(f)));
const allVars=await figma.variables.getLocalVariablesAsync();
const vars={};for(const v of allVars){const c=v.variableCollectionId;vars[(c==="VariableCollectionId:2:3"?"w/":c==="VariableCollectionId:2:4"?"u/":"p/")+v.name]=v;}
const styles=Object.fromEntries((await figma.getLocalTextStylesAsync()).map(s=>[s.name.replace("Pond / ",""),s]));
function track(n){created.push(n.id);return n;}
function fill(n,key,stroke=false){const paint=figma.variables.setBoundVariableForPaint({type:"SOLID",color:{r:0,g:0,b:0}},"color",vars[key]);if(stroke)n.strokes=[paint];else n.fills=[paint];}
function text(parent,str,style="Body",width=600,key="u/ink"){const n=track(figma.createText());n.name=str.slice(0,55);const s=styles[style];n.fontName=s.fontName;n.textStyleId=s.id;n.resize(width,1);n.textAutoResize="HEIGHT";n.characters=str;fill(n,key);parent.appendChild(n);n.layoutSizingVertical="HUG";return n;}
function stack(parent,name,w,gap=16,pad=0,bg=null){const n=track(figma.createAutoLayout("VERTICAL"));n.name=name;n.resize(w,1);n.primaryAxisSizingMode="AUTO";n.counterAxisSizingMode="FIXED";n.itemSpacing=gap;n.paddingTop=n.paddingBottom=n.paddingLeft=n.paddingRight=pad;n.fills=[];if(bg)fill(n,bg);if(parent)parent.appendChild(n);return n;}
function row(parent,name,gap=24){const n=track(figma.createAutoLayout("HORIZONTAL"));n.name=name;n.itemSpacing=gap;n.fills=[];n.counterAxisAlignItems="CENTER";if(parent)parent.appendChild(n);return n;}
function rect(parent,name,w,h,key){const n=track(figma.createRectangle());n.name=name;n.resize(w,h);fill(n,key);parent.appendChild(n);return n;}
function board(name,w=1280){const n=stack(figma.currentPage,name,w,32,48,"u/paper");n.x=80;n.y=80;return n;}
const glyphs={
"0":["01110","11011","11011","11011","11011","11011","01110"],"1":["00110","01110","00110","00110","00110","00110","01111"],
"2":["01110","11011","00011","00110","01100","11000","11111"],"3":["11110","00011","00011","01110","00011","00011","11110"],
"4":["00011","00111","01111","11011","11111","00011","00011"],"5":["11111","11000","11000","11110","00011","11011","01110"],
"6":["00110","01100","11000","11110","11011","11011","01110"],"7":["11111","00011","00110","00110","01100","01100","01100"],
"8":["01110","11011","11011","01110","11011","11011","01110"],"9":["01110","11011","11011","01111","00011","00110","01100"],
":":["0","1","1","0","1","1","0"],
"A":["01110","10001","10001","11111","10001","10001","10001"],"B":["11110","10001","10001","11110","10001","10001","11110"],
"C":["01111","10000","10000","10000","10000","10000","01111"],"D":["11110","10001","10001","10001","10001","10001","11110"],
"E":["11111","10000","10000","11110","10000","10000","11111"],"F":["11111","10000","10000","11110","10000","10000","10000"],
"G":["01111","10000","10000","10111","10001","10001","01110"],"H":["10001","10001","10001","11111","10001","10001","10001"],
"I":["111","010","010","010","010","010","111"],"J":["00111","00010","00010","00010","00010","10010","01100"],
"K":["10001","10010","10100","11000","10100","10010","10001"],"L":["10000","10000","10000","10000","10000","10000","11111"],
"M":["10001","11011","10101","10101","10001","10001","10001"],"N":["10001","11001","11001","10101","10011","10011","10001"],
"O":["01110","10001","10001","10001","10001","10001","01110"],"P":["11110","10001","10001","11110","10000","10000","10000"],
"Q":["01110","10001","10001","10001","10101","10010","01101"],"R":["11110","10001","10001","11110","10100","10010","10001"],
"S":["01111","10000","10000","01110","00001","00001","11110"],"T":["11111","00100","00100","00100","00100","00100","00100"],
"U":["10001","10001","10001","10001","10001","10001","01110"],"V":["10001","10001","10001","10001","10001","01010","00100"],
"W":["10001","10001","10001","10101","10101","11011","10001"],"X":["10001","10001","01010","00100","01010","10001","10001"],
"Y":["10001","10001","01010","00100","00100","00100","00100"],"Z":["11111","00001","00010","00100","01000","10000","11111"],
" ":["000","000","000","000","000","000","000"],"-":["000","000","000","111","000","000","000"]
};
function pixels(parent,label,scale=2,key="w/ink"){const f=row(parent,"Pixel / "+label,scale);f.counterAxisAlignItems="MIN";for(const ch of label){const a=glyphs[ch];if(!a)throw Error("Missing glyph "+ch);let path="";for(let y=0;y<7;y++)for(let x=0;x<a[y].length;x++)if(a[y][x]==="1")path+="M "+x*scale+" "+y*scale+" L "+(x+1)*scale+" "+y*scale+" L "+(x+1)*scale+" "+(y+1)*scale+" L "+x*scale+" "+(y+1)*scale+" Z ";const f2=figma.createNodeFromSvg('<svg xmlns="http://www.w3.org/2000/svg" width="'+a[0].length*scale+'" height="'+7*scale+'" viewBox="0 0 '+a[0].length*scale+' '+7*scale+'"><path d="'+(path||'M 0 0')+'" fill="#000000"/></svg>');created.push(...ids(f2));f2.name="Glyph / "+ch;for(const v of f2.findAllWithCriteria({types:["VECTOR"]}))fill(v,key);f.appendChild(f2);}return f;}
function ids(n){return [n.id,...(n.findAll?n.findAll(()=>true).map(x=>x.id):[])];}
