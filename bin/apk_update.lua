local deltatime = 0
local pTime = 0
local time = 0
local stepTime = 0
local step = 0
local red = Color(0.66, 0.1, 0.0, 1.0)
local green = Color(0.0, 0.65, 0.3, 1.0)
local ff = 0.992

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
  pTime = time
  if(step%3==0)then
    self.color = Color(self.color.r,self.color.g,self.color.b, self.color.a * ff)
  end
  step = step + 1;
end

function onReceiveOSC(message, connections)
  print('m_mute_btn')
  local path = message[1]
  local arguments = message[2]

  if('/mlive' == path)then
    if(arguments[1].value==true)then
        self.color = red
    else
        self.color = green
    end
  end
end
