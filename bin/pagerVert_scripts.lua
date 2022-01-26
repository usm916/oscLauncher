local deltatime = 0
local pTime = 0
local time = 0
local stepTime = 0
local step = 15000
local step = 0
local red = Color(0.66, 0.1, 0.0, 1.0)
local green = Color(0.0, 0.65, 0.3, 1.0)
local ff = 0.987

function init()
  print('init')
  deltatime = 0
  pTime = 0
  time = 0
  stepTime = 0
  step = 15000
  pTime = getMillis()

  local alive = self.children['alive']
  alive.color = red
end

function update()
  time = getMillis()
  deltatime = time - pTime
  stepTime = stepTime + deltatime
  if(stepTime > step)then
    stepTime = stepTime - step
    -- print('pn = ', time, ' d = ', deltatime, ' len = ', stepTime)
    sendOSC("ping")
  end
  if(step%3==0)then
    -- alive value
    local alive = self.children['alive']
    alive.color = Color(alive.color.r,alive.color.g,alive.color.b, alive.color.a * ff)
  end
  pTime = time
  step = step + 1;
end

function onReceiveOSC(message, connections)
    local path = message[1]
    local arguments = message[2]
    if(path=='/alive')then
        local alive = self.children['alive']
        alive.color = green
    elseif(path=='/slive')then
        local slive = self.children['pager'].children['1'].children['pager2'].children['1'].children['board'].children['spk_grp'].children['slive']
        local sVol = self.children['pager'].children['1'].children['pager2'].children['1'].children['board'].children['spk_grp'].children['sys_snd_spk']
        local cColor
        if(arguments[1].value==true)then
            cColor = green
        else
            cColor = red
        end
        slive.color = cColor
        sVol.color = cColor
    elseif(path=='/mlive')then
        local mlive = self.children['pager'].children['1'].children['pager2'].children['1'].children['board'].children['mic_grp'].children['mlive']
        local sVol = self.children['pager'].children['1'].children['pager2'].children['1'].children['board'].children['mic_grp'].children['sys_snd_mic']
        local cColor
        if(arguments[1].value==true)then
            cColor = green
        else
            cColor = red
        end
        mlive.color = cColor
        sVol.color = cColor
    end
    -- print('onRecOSC ', path,' / ', arguments[1].tag,' / ', arguments[1].value)
end