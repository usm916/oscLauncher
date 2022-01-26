local deltatime = 0
local pTime = 0
local time = 0
local stepTime = 0
local step = 15000

function init()
  print('init')
  deltatime = 0
  pTime = 0
  time = 0
  stepTime = 0
  step = 15000
  pTime = getMillis()
end

function update()
  time = getMillis()
  deltatime = time - pTime
  stepTime = stepTime + deltatime
  if(stepTime > step)then
    stepTime = stepTime - step
    print('pn = ', time, ' d = ', deltatime, ' len = ', stepTime)
    sendOSC("ping")
  end
  pTime = time
end

function onPointer(pointers)
  print('onPointer')
  for i=1,#pointers do
    local pointer = pointers[i]
    print('\t', pointer.ID, pointer.x, pointer.y, pointer.state, pointer.created, pointer.modified)
  end
end

function onValueChanged(key)
  print('onValueChanged')
  print('\t', key, '=', self.values[key])
end

function onReceiveMIDI(message, connections)
  print('onReceiveMIDI')
  print('\t message     =', table.unpack(message))
  print('\t connections =', table.unpack(connections))
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
  if('/ping_time')then
    local p = self.children['pager']
    local nchild = child.children['win']
    local xchild = child.children['slive']
    xchild.color = Color(1)
  end
end

function onReceiveNotify(key, value)
  print('onReceiveNotify')
  print('\t key   =', key)
  print('\t value =', value)
end

------------------------------------------
local red = Color(1,0.2,0,1)
local green = Color(0.0,1.0,0.2,1)

function onReceiveOSC(message, connections)
  print('onReceiveOSC')
  local path = message[1]
  local arguments = message[2]
  print('\t path =', path)
  print('\t arg  =', arguments)
  if('/ping_time' == path)then
    if(arguments)then
      self.color = red
    else
      self.color = green
    end
  end
end

-0---------------------------------------------
arguments?red:green

