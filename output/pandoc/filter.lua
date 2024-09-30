-- Lua filter to replace '*' in plain text with '\*'
-- See: https://pandoc.org/lua-filters.html

-- Replace textual '*' occurences.
-- function Str(elem)
--   elem.text = elem.text:gsub("%*", "\\*")
--   return elem
-- end

-- pandoc treats newlines that are not hard linebreaks as a SoftBreak node.
-- Replace each soft linebreak by a hard linebreak.
function SoftBreak(elem)
  return pandoc.LineBreak()
end

-- Return a table where the first and last element are the 'enc' (enclosure)
-- param. The elements in between are the children of src.
function enclose_children(src, enc)
  local new_elems = {}
  table.insert(new_elems, enc)
  for _, item in ipairs(src.content) do
    table.insert(new_elems, item)
  end
  table.insert(new_elems, enc)
  return new_elems
end

-- The markdown AST may contain Emph (italics) elements.
-- Replace each Emph by a '*' enclosed string.
function Emph(elem)
  star = pandoc.Str("*")
  return enclose_children(elem, star)
end

-- The markdown AST may contain Strong (italics) elements.
-- Replace each Emph by a '**' enclosed string.
function Strong(elem)
  star = pandoc.Str("**")
  return enclose_children(elem, star)
end
