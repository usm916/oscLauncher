local pT = false
local cT = false
local cPos = 0.0
local pPos = 0.0
local acc = 0.0
local cacc = 0.0
local myValue = 0.0
local pValue = 0.0
local sum = 0.0
local ff = 0.5
local acScale = 1.7
local cDelta = 0.0
local onFrDelta = 0

function onValueChanged(key)
    --print('\t', key, '=', self.values[key])
    cT = self.values['touch']
    cPos = self.values['x']
    
    if pT==false and cT==true then
      acc = 0.0
      cacc = 0.0
      cDelta = 0.0
      sum = 0.0
      onFrDelta = 0
    --   print('------------------------')
    --   print('fr= ', fr, ' touch x =', pPos, ' cx = ', cPos)
      pPos = cPos
      myValue = cPos + 1000
    end
    --
    local d = 0.1
    if cPos < d and pPos > 1.0-d then
       pPos = pPos - 1.0;
    elseif cPos > 1.0-d and pPos < d then
        pPos = pPos + 1.0;
    end

    if pT==true and cT==true then
        myValue = cPos + 1000
        cDelta = cPos - pPos
        cacc = cacc * ff + cDelta * (1.0 - ff) * acScale
        -- print('cDelta = ', cDelta, ' : ', cPos, ' : ', pPos)
    end

    if pT==true and cT==false then
        myValue = cPos + 1000
        acc = acc + cacc
        cacc = 0.0
        cDelta = 0.0
        sum = 0.0

        -- avoid jump frame
        if math.abs(acc) < 0.008 then
            acc = 0.0
        end

        -- avoid jump frame
        if onFrDelta < 4 then
            acc = 0.0
        end

        -- print('Out acc = ', acc)
    end
    
    --print('delta = ', acc, 'cPos = ', cPos)
    pPos = cPos
    pT = cT
    onFrDelta = onFrDelta+1
end

function update()
    local step = 0.04
    sum = sum + cDelta
    local cnt = math.floor(math.abs(sum/step))
    -- print('sum/step = ', sum/step, ' cd ', cDelta);
    sum = math.fmod(sum, step)
    -- print('sum ', sum)
    if math.abs(myValue - pValue)>0.2 then
        pValue = myValue
        -- print('JUMP!')
    end

    if math.abs(cnt) > 0 then
        local dir
        if(sum < 0.0)then
            dir = '/key__left'
        else            
            dir = '/key__right'
        end
        -- print('cnt =' , cnt, ' dir = ', sum/math.abs(sum))
        for i = 1, cnt, 1 do
            sendOSC(dir);
        end
    end

    acc = acc * 0.98
    cDelta = acc
    -- cacc = cacc * 0.6
    myValue = myValue + acc

    pValue = myValue
    self.values['x'] = math.fmod(myValue, 1.0)
   -- print('update wheel')
end

