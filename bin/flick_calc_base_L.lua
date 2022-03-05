local remapPi = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0}
local menuW
local centerX
local centerY
local targetW
local targetH
local iniX
local iniY
local iniW
local iniH
-- local offsety
local OpSize
local CpSize
local px = 0.0
local py = 0.0
local ox = 0.0
local oy = 0.0
local onecex = false
local onecey = false
local isFall = false
local touchTime = 0
local RAD_TO_DEG = 180.0/math.pi
local pager
local cursor
local originCursor

function angle(xa, ya, xb, yb)
    xx = xa*yb - ya*xb
    yy = xa*xb + ya*yb
    local an = math.atan2(xx,yy) * RAD_TO_DEG
    return an
end

function normalize(x, y)
  local length = length(x,y)
  if( length > 0 ) then
    local nx = x / length
    local ny = y / length
    end
  return nx, ny
end

function length(x,y)
    return math.sqrt(x*x + y*y)
end

function getNum(x,y)
    -- num = math.random(6)
    local an = -1 * angle(x, y, 1, 0)
    if (y<0.0) then
        an = -1 * angle(x, y, 1, 0) + 360
    -- else
    --     an = -1 * angle(x, y, 1, 0)
    end

    local separate = 360 / 8
    local len = length(x,y)
    print('an = ', an, ' len = ', len)--, "( ", x, " : ", y, " )")
    if len<0.09 then
        --num = 5
        print('cancel')
    elseif (onecex==false) and (onecey == false) then
        --num = 5
        print('cancel')
    elseif separate*0.5>an then
        num = 4
        print(num)
    elseif separate*1.5>an then
        num = 3
        print(num)
    elseif separate*2.5>an then
        num = 2
        print(num)
    elseif separate*3.5>an then
        num = 1
        print(num)
    elseif separate*4.5>an then
        num = 0
        print(num)
    elseif separate*5.5>an then
        num = 7
        print(num)
    elseif separate*6.5>an then
        num = 6
        print(num)
    elseif separate*7.5>an then
        num = 5
        print(num)
    elseif separate*8.5>an then
        num = 4
        print(num, " - 2")
    else
        num = 0
    end
return num
end

function init()
    menuW = self.parent.children['menuPi'].properties.frame.w
    targetW = 400
    targetH = 400
    iniX = self.properties.frame.x
    iniY = self.properties.frame.y
    iniW = self.properties.frame.w
    iniH = self.properties.frame.h
    centerX = iniX + iniW/2
    centerY = iniY + iniH/2
    --offsety = self.properties.frame.y - (targetH - self.properties.frame.h)
    cursor = self.parent.children['cPt']
    originCursor = self.parent.children['oPt']
    CpSize = cursor.properties.frame.w / 2;
    OpSize = originCursor.properties.frame.w / 2;
    px = 0.0
    py = 0.0
    ox = 0.0
    oy = 0.0
    onecex = false
    onecey = false
    isFall = false
    touchTime = 0
    RAD_TO_DEG = 180.0/math.pi
    pager = self.parent.children['pager2']
end

function onValueChanged(key)
    if key == 'touch' then
        isFall = self.values[key]
        if isFall == true then
            touchTime = getMillis()
            self.properties.frame.w = targetW;
            self.properties.frame.h = targetH;
            self.properties.frame.x = centerX - targetW/2;
            self.properties.frame.y = centerY - targetH/2;
            pager.properties.visible = false
            cursor.properties.visible = true
            --print("touch = ", isFall, "[ x = ", px, ", y = ", py, " ]")
        else
            cx = px--self.values['x']
            cy = py--self.values['y']
            cursor.properties.frame.x = centerX - targetW/2 + cx * targetW - CpSize
            cursor.properties.frame.y = centerY - targetH/2 + (1.0 - cy) * targetH - CpSize
            vx = cx - ox
            vy = cy - oy
            --print("< ", cx, " : ", cy, " >", "< ", ox, " : ", oy, " >")
            -- revert settings
            self.parent.children['menuPi'].properties.frame.x = -500
            self.parent.children['menuPi'].properties.frame.y = -500
            -- print('angle = ', angle(vx, vy, 1.0, 0.0))
            cnum = getNum(vx, vy)
            pager.values.page = cnum
            --print('num = ', cnum)
            onecex = false
            onecey = false
            self.properties.frame.w = iniW;
            self.properties.frame.h = iniH;
            self.properties.frame.x = iniX;
            self.properties.frame.y = iniY;
            pager.properties.visible = true
            cursor.properties.visible = false
        end 
    elseif ((key == 'x') and isFall and (touchTime+17<getMillis())) then
        px = self.values['x']
        if onecex==false then
            ox = px
            originCursor.properties.frame.x = centerX - targetW/2 + px * targetW - OpSize
            self.parent.children['menuPi'].properties.frame.x = centerX - targetW/2 + px * targetW - menuW/2
            onecex = true
        else
            cursor.properties.frame.x = centerX - targetW/2 + px * targetW - CpSize
        end
        --print('update x')
    elseif ((key == 'y') and isFall and (touchTime+17<getMillis())) then
        py = self.values['y']
        if onecey==false then
            oy = py
            originCursor.properties.frame.y = centerY - targetH/2 + (1.0 - py) * targetH - OpSize
            self.parent.children['menuPi'].properties.frame.y = centerY - targetH/2 + (1.0 - py) * targetH - menuW/2
            onecey = true
        else
            cursor.properties.frame.y = centerY - targetH/2 + (1.0 - py) * targetH - CpSize
        end
        --print('update y')
    end
    --print("touch = ", self.values[key])
end