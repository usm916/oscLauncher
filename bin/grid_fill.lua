local elements = { "2","3","4","1","Cancel","5","8","7","6" }

function init()                        -- handler for notifications
  for i=1, #self.children do                         -- iterate from 1 to #ofchildren
    self.children[i].values.text = elements[i]  -- and set the children at index i to the label + the index value 
  end
end