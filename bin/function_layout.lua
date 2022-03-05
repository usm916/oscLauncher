local wnum = 4
local wSize = 400/wnum
local hSize = 102
local yShift = 0
local elements = { "Banana", "Apple", "Orange", "Grape", "Lemon", "Berry", "dummy", "folder" }
local colors = { "C587AE","d3add1", "8c759f", "C587AE", "f4b8c7", "FACDBE", "feeec8" }
local icons = { "📥","📁","🔨","📺","❖","💻","⌨","📱" }  

-- kbd

function init()
  print('init@bankFrame')
  for i=0,99 do
    local num = tostring(i+1)
    numRand = math.random(7)
    print(colors[numRand])

    local textname = "text"..num
    if i<25 then
        local text = self.children['bank1'].children[textname]
        text.properties.outline = false
    end

    local btnname = "button"..num
    local button = self.children['bank1'].children[btnname]
    button.properties.frame.x = 9 + i%wnum * wSize
    button.properties.frame.y = 64 + math.floor(i/wnum) * hSize + i%2 * yShift
    button.properties.shape = numRand
    button.properties.color = Color.fromHexString(colors[numRand])
    button.properties.outline = false
    --button.properties.corner = 10

    local btnname = "button"..num
    n = math.random(7)
    local numlabel = tostring(i*2+1)
    local labelname = "label"..numlabel
    local label = self.children['bank1'].children[labelname]
    print(label.properties.name)
    label.properties.frame.x = 13 + i%wnum * wSize
    label.properties.frame.y = 72 + math.floor(i/wnum) * hSize + i%2 * yShift
    label.properties.textSize = 22
    label.properties.textColor = Color.fromHexString(colors[numRand])
    label.values.text = icons[numRand]

    local numlabel2 = numlabel + 1
    local labelname2 = "label"..numlabel2
    local label2 = self.children['bank1'].children[labelname2]
    print(label2.properties.name)
    label2.properties.frame.x = 9 + i%wnum * wSize
    label2.properties.frame.y = 116 + math.floor(i/wnum) * hSize + i%2 * yShift
    label2.properties.textSize = 16
    label2.properties.textColor = Color.fromHexString(colors[numRand])
    label2.values.text = elements[numRand]
  end
end

function onReceiveOSC(message, connections)
  print('onReceiveOSC')
  local path = message[1]
  local arguments = message[2]
  print('\t path        =', path)
  for i=1,#arguments do
    print('\t argument    =', arguments[i].tag, arguments[i].value)
  end
  print('\t connections =', table.unpack(connections))
end